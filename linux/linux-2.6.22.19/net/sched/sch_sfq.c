/*
 * net/sched/sch_sfq.c	Stochastic Fairness Queueing discipline.
 *
 *		This program is free software; you can redistribute it and/or
 *		modify it under the terms of the GNU General Public License
 *		as published by the Free Software Foundation; either version
 *		2 of the License, or (at your option) any later version.
 *
 * Authors:	Alexey Kuznetsov, <kuznet@ms2.inr.ac.ru>
 */

#include <linux/module.h>
#include <linux/types.h>
#include <linux/kernel.h>
#include <linux/jiffies.h>
#include <linux/string.h>
#include <linux/in.h>
#include <linux/errno.h>
#include <linux/init.h>
#include <linux/ipv6.h>
#include <linux/skbuff.h>
#include <linux/jhash.h>
#include <net/ip.h>
#include <net/netlink.h>
#include <net/pkt_sched.h>
#include <linux/jhash.h>
#include <net/netfilter/nf_conntrack.h>


/*	Stochastic Fairness Queuing algorithm.
	=======================================

	Source:
	Paul E. McKenney "Stochastic Fairness Queuing",
	IEEE INFOCOMM'90 Proceedings, San Francisco, 1990.

	Paul E. McKenney "Stochastic Fairness Queuing",
	"Interworking: Research and Experience", v.2, 1991, p.113-131.


	See also:
	M. Shreedhar and George Varghese "Efficient Fair
	Queuing using Deficit Round Robin", Proc. SIGCOMM 95.


	This is not the thing that is usually called (W)FQ nowadays.
	It does not use any timestamp mechanism, but instead
	processes queues in round-robin order.

	ADVANTAGE:

	- It is very cheap. Both CPU and memory requirements are minimal.

	DRAWBACKS:

	- "Stochastic" -> It is not 100% fair.
	When hash collisions occur, several flows are considered as one.

	- "Round-robin" -> It introduces larger delays than virtual clock
	based schemes, and should not be used for isolating interactive
	traffic	from non-interactive. It means, that this scheduler
	should be used as leaf of CBQ or P3, which put interactive traffic
	to higher priority band.

	We still need true WFQ for top level CSZ, but using WFQ
	for the best effort traffic is absolutely pointless:
	SFQ is superior for this purpose. */

#define SFQ_DEPTH_DEFAULT	128
#define SFQ_DIVISOR_DEFAULT	10

#define SFQ_HEAD 0
#define SFQ_TAIL 1

#define SFQ_PERTURB(period) (jiffies + (unsigned long)period * HZ)

/* This type must contain greater than depth*2 values, so depth is constrained 
 * accordingly. */
typedef unsigned int sfq_index;
#define SFQ_MAX_DEPTH (UINT_MAX / 2 - 1)

/* In practice, the actual divisor size is limited by kcalloc, but we still
 * don't want to left shift by more than 31. */
#define SFQ_MAX_DIVISOR 31

struct sfq_head
{
	sfq_index	next;
	sfq_index	prev;
};

struct sfq_sched_data
{
/* Parameters */
	unsigned	perturb_period;
	unsigned	quantum;	/* Allotment per round: MUST BE >= MTU */
	int		limit;
	unsigned	depth;
	unsigned	hash_divisor;
	unsigned	hash_kind;

/* Variables */
	struct timer_list perturb_timer;
	u32		perturbation;
	sfq_index	tail;		/* Index of current slot in round */
	sfq_index	max_depth;	/* Maximal depth */

	sfq_index	*ht;			/* Hash table */
	sfq_index	*next;			/* Active slots link */
	short		*allot;			/* Current allotment per slot */
	unsigned short	*hash;			/* Hash value indexed by slots */
	struct sk_buff_head	*qs;		/* Slot queue */
	struct sfq_head	*dep;			/* Linked list of slots, indexed by depth */
};

/* This contains the info we will hash. */
struct sfq_packet_info
{
	u32     proto;          /* protocol or port */
	u32     src;            /* source from packet header */
	u32     dst;            /* destination from packet header */
	u32     mark;           /* netfilter mark (fwmark) */
	u32     ctorigsrc;      /* original source from conntrack */
	u32     ctorigdst;      /* original destination from conntrack */
	u32     ctreplsrc;      /* reply source from conntrack */
	u32     ctrepldst;      /* reply destination from conntrack */
};


static unsigned sfq_hash(struct sfq_sched_data *q, struct sk_buff *skb)
{
	struct sfq_packet_info info;
	u32 pert = q->perturbation;
	unsigned mask = (1<<q->hash_divisor) - 1;
#ifdef CONFIG_NET_SCH_SFQ_NFCT
	enum ip_conntrack_info ctinfo;
	struct nf_conn *ct = nf_ct_get(skb, &ctinfo);
#endif

	switch (skb->protocol) {
	case __constant_htons(ETH_P_IP):
	{
		const struct iphdr *iph = ip_hdr(skb);
		info.dst = iph->daddr;
		info.src = iph->saddr;
		if (!(iph->frag_off&htons(IP_MF|IP_OFFSET)) &&
		    (iph->protocol == IPPROTO_TCP ||
		     iph->protocol == IPPROTO_UDP ||
		     iph->protocol == IPPROTO_UDPLITE ||
		     iph->protocol == IPPROTO_SCTP ||
		     iph->protocol == IPPROTO_DCCP ||
		     iph->protocol == IPPROTO_ESP))
			info.proto = *(((u32*)iph) + iph->ihl);
		else
			info.proto = iph->protocol;
		break;
	}
	case __constant_htons(ETH_P_IPV6):
	{
		struct ipv6hdr *iph = ipv6_hdr(skb);
		/* Hash ipv6 addresses into a u32. This isn't ideal,
		* but the code is simple. */
		info.dst = jhash2(iph->daddr.s6_addr32, 4, q->perturbation);
		info.src = jhash2(iph->saddr.s6_addr32, 4, q->perturbation);
		if (iph->nexthdr == IPPROTO_TCP ||
		    iph->nexthdr == IPPROTO_UDP ||
		    iph->nexthdr == IPPROTO_UDPLITE ||
		    iph->nexthdr == IPPROTO_SCTP ||
		    iph->nexthdr == IPPROTO_DCCP ||
		    iph->nexthdr == IPPROTO_ESP)
		    info.proto = *(u32*)&iph[1];
		else
			info.proto = iph->nexthdr;
		break;
	}
	default:
		info.dst   = (u32)(unsigned long)skb->dst;
		info.src   = (u32)(unsigned long)skb->sk;
		info.proto = skb->protocol;
	}

	info.mark = skb->mark;

#ifdef CONFIG_NET_SCH_SFQ_NFCT
	/* defaults if there is no conntrack info */
	info.ctorigsrc = info.src;
	info.ctorigdst = info.dst;
	info.ctreplsrc = info.dst;
	info.ctrepldst = info.src;
	/* collect conntrack info */
	if (ct && !nf_ct_is_untracked(ct)) {
		if (skb->protocol == __constant_htons(ETH_P_IP)) {
			info.ctorigsrc =
			    ct->tuplehash[IP_CT_DIR_ORIGINAL].tuple.src.u3.ip;
			info.ctorigdst =
			    ct->tuplehash[IP_CT_DIR_ORIGINAL].tuple.dst.u3.ip;
			info.ctreplsrc =
			    ct->tuplehash[IP_CT_DIR_REPLY].tuple.src.u3.ip;
			info.ctrepldst =
			    ct->tuplehash[IP_CT_DIR_REPLY].tuple.dst.u3.ip;
		}
		else if (skb->protocol == __constant_htons(ETH_P_IPV6)) {
			/* Again, hash ipv6 addresses into a single u32. */
			info.ctorigsrc = jhash2(
			    ct->tuplehash[IP_CT_DIR_ORIGINAL].tuple.src.u3.ip6,
			    4, pert);
			info.ctorigdst = jhash2(
			    ct->tuplehash[IP_CT_DIR_ORIGINAL].tuple.dst.u3.ip6,
			    4, pert);
			info.ctreplsrc = jhash2(
			    ct->tuplehash[IP_CT_DIR_REPLY].tuple.src.u3.ip6,
			    4, pert);
			info.ctrepldst = jhash2(
			    ct->tuplehash[IP_CT_DIR_REPLY].tuple.dst.u3.ip6,
			    4, pert);
		}
	}
#endif

	switch (q->hash_kind) {
	case TCA_SFQ_HASH_CLASSIC:
		return jhash_3words(info.dst, info.src, info.proto, pert) & mask;
	case TCA_SFQ_HASH_DST:
		return jhash_1word(info.dst, pert) & mask;
	case TCA_SFQ_HASH_SRC:
		return jhash_1word(info.src, pert) & mask;
	case TCA_SFQ_HASH_FWMARK:
		return jhash_1word(info.mark, pert) & mask;
#ifdef CONFIG_NET_SCH_SFQ_NFCT
	case TCA_SFQ_HASH_CTORIGDST:
		return jhash_1word(info.ctorigdst, pert) & mask;
	case TCA_SFQ_HASH_CTORIGSRC:
		return jhash_1word(info.ctorigsrc, pert) & mask;
	case TCA_SFQ_HASH_CTREPLDST:
		return jhash_1word(info.ctrepldst, pert) & mask;
	case TCA_SFQ_HASH_CTREPLSRC:
		return jhash_1word(info.ctreplsrc, pert) & mask;
	case TCA_SFQ_HASH_CTNATCHG:
	{
		if (info.ctorigdst == info.ctreplsrc)
			return jhash_1word(info.ctorigsrc, pert) & mask;
		return jhash_1word(info.ctreplsrc, pert) & mask;
	}
#else
	case TCA_SFQ_HASH_CTORIGDST:
	case TCA_SFQ_HASH_CTORIGSRC:
	case TCA_SFQ_HASH_CTREPLDST:
	case TCA_SFQ_HASH_CTREPLSRC:
	case TCA_SFQ_HASH_CTNATCHG:
	if (net_ratelimit())
		printk(KERN_WARNING "SFQ: Conntrack support not enabled.");
#endif
 	}
	if (net_ratelimit())
		printk(KERN_WARNING "SFQ: Unknown hash method. "
		                    "Falling back to classic.\n");
	q->hash_kind = TCA_SFQ_HASH_CLASSIC;
	return jhash_3words(info.dst, info.src, info.proto, pert) & mask;
}

static inline void sfq_link(struct sfq_sched_data *q, sfq_index x)
{
	sfq_index p, n;
	int d = q->qs[x].qlen + q->depth;

	p = d;
	n = q->dep[d].next;
	q->dep[x].next = n;
	q->dep[x].prev = p;
	q->dep[p].next = q->dep[n].prev = x;
}

static inline void sfq_dec(struct sfq_sched_data *q, sfq_index x)
{
	sfq_index p, n;

	n = q->dep[x].next;
	p = q->dep[x].prev;
	q->dep[p].next = n;
	q->dep[n].prev = p;

	if (n == p && q->max_depth == q->qs[x].qlen + 1)
		q->max_depth--;

	sfq_link(q, x);
}

static inline void sfq_inc(struct sfq_sched_data *q, sfq_index x)
{
	sfq_index p, n;
	int d;

	n = q->dep[x].next;
	p = q->dep[x].prev;
	q->dep[p].next = n;
	q->dep[n].prev = p;
	d = q->qs[x].qlen;
	if (q->max_depth < d)
		q->max_depth = d;

	sfq_link(q, x);
}

static unsigned int sfq_drop(struct Qdisc *sch)
{
	struct sfq_sched_data *q = qdisc_priv(sch);
	sfq_index d = q->max_depth;
	struct sk_buff *skb;
	unsigned int len;

	/* Queue is full! Find the longest slot and
	   drop a packet from it */

	if (d > 1) {
		sfq_index x = q->dep[d + q->depth].next;
		skb = q->qs[x].prev;
		len = skb->len;
		__skb_unlink(skb, &q->qs[x]);
		kfree_skb(skb);
		sfq_dec(q, x);
		sch->q.qlen--;
		sch->qstats.drops++;
		sch->qstats.backlog -= len;
		return len;
	}

	if (d == 1) {
		/* It is difficult to believe, but ALL THE SLOTS HAVE LENGTH 1. */
		d = q->next[q->tail];
		q->next[q->tail] = q->next[d];
		q->allot[q->next[d]] += q->quantum;
		skb = q->qs[d].prev;
		len = skb->len;
		__skb_unlink(skb, &q->qs[d]);
		kfree_skb(skb);
		sfq_dec(q, d);
		sch->q.qlen--;
		q->ht[q->hash[d]] = q->depth;
		sch->qstats.drops++;
		sch->qstats.backlog -= len;
		return len;
	}

	return 0;
}

static int
sfq_q_enqueue(struct sk_buff *skb, struct sfq_sched_data *q, int end)
{
	unsigned hash = sfq_hash(q, skb);
	sfq_index x;

	x = q->ht[hash];
	if (x == q->depth) {
		q->ht[hash] = x = q->dep[q->depth].next;
		q->hash[x] = hash;
	}

	if (end == SFQ_TAIL) {
		/* If selected queue has length q->limit, this means that
		 * all other queues are empty and that we do simple tail drop,
		 * i.e. drop _this_ packet.
		 */
		if (q->qs[x].qlen >= q->limit) {
			unsigned int drop_len = skb->len;

			kfree_skb(skb);
			return drop_len;
		}
		__skb_queue_tail(&q->qs[x], skb);
	} else { /* end == SFQ_HEAD */
		__skb_queue_head(&q->qs[x], skb);
		/* If selected queue has length q->limit+1, this means that
		 * all other queues are empty and we do simple tail drop.
		 * This packet is still requeued at head of queue, tail packet
		 * is dropped.
		 */
		if (q->qs[x].qlen > q->limit) {
			unsigned int drop_len;

			skb = q->qs[x].prev;
			drop_len = skb->len;
			__skb_unlink(skb, &q->qs[x]);
			kfree_skb(skb);
			return drop_len;
		}
	}

	sfq_inc(q, x);
	if (q->qs[x].qlen == 1) {		/* The flow is new */
		if (q->tail == q->depth) {	/* It is the first flow */
			q->tail = x;
			q->next[x] = x;
			q->allot[x] = q->quantum;
		} else {
			q->next[x] = q->next[q->tail];
			q->next[q->tail] = x;
			q->tail = x;
		}
	}

	return 0;
}

static int
sfq_enqueue(struct sk_buff *skb, struct Qdisc* sch)
{
	struct sfq_sched_data *q = qdisc_priv(sch);

	if (sfq_q_enqueue(skb, q, SFQ_TAIL)) {
		sch->qstats.drops++;
		return NET_XMIT_DROP;
	}

	sch->qstats.backlog += skb->len;
	if (++sch->q.qlen <= q->limit) {
		sch->bstats.bytes += skb->len;
		sch->bstats.packets++;
		return 0;
	}

	sfq_drop(sch);
	return NET_XMIT_CN;
}

static int
sfq_requeue(struct sk_buff *skb, struct Qdisc* sch)
{
	struct sfq_sched_data *q = qdisc_priv(sch);
	unsigned int drop_len;

	sch->qstats.backlog += skb->len;
	if ((drop_len = sfq_q_enqueue(skb, q, SFQ_HEAD))) {
		sch->qstats.backlog -= drop_len;
		sch->qstats.drops++;
		return NET_XMIT_CN;
	}

	if (++sch->q.qlen <= q->limit) {
		sch->qstats.requeues++;
		return 0;
	}

	sfq_drop(sch);
	return NET_XMIT_CN;
}

static struct
sk_buff *sfq_q_dequeue(struct sfq_sched_data *q)
{
	struct sk_buff *skb;
	sfq_index a, old_a;

	/* No active slots */
	if (q->tail == q->depth)
		return NULL;

	a = old_a = q->next[q->tail];

	/* Grab packet */
	skb = __skb_dequeue(&q->qs[a]);
	sfq_dec(q, a);

	/* Is the slot empty? */
	if (q->qs[a].qlen == 0) {
		q->ht[q->hash[a]] = q->depth;
		a = q->next[a];
		if (a == old_a) {
			q->tail = q->depth;
			return skb;
		}
		q->next[q->tail] = a;
		q->allot[a] += q->quantum;
	} else if ((q->allot[a] -= skb->len) <= 0) {
		q->tail = a;
		a = q->next[a];
		q->allot[a] += q->quantum;
	}

	return skb;
}

static struct sk_buff
*sfq_dequeue(struct Qdisc* sch)
{
	struct sfq_sched_data *q = qdisc_priv(sch);
	struct sk_buff *skb;

	skb = sfq_q_dequeue(q);
	if (skb == NULL)
		return NULL;
	sch->q.qlen--;
	sch->qstats.backlog -= skb->len;
	return skb;
}

static void
sfq_reset(struct Qdisc* sch)
{
	struct sk_buff *skb;

	while ((skb = sfq_dequeue(sch)) != NULL)
		kfree_skb(skb);
}

static void sfq_perturbation(unsigned long arg)
{
	struct Qdisc *sch = (struct Qdisc*)arg;
	struct sfq_sched_data *q = qdisc_priv(sch);

	get_random_bytes(&q->perturbation, 4);

	if (q->perturb_period)
		mod_timer(&q->perturb_timer, SFQ_PERTURB(q->perturb_period));
}

static void sfq_q_destroy(struct sfq_sched_data *q)
{
	kfree(q->ht);
	kfree(q->dep);
	kfree(q->next);
	kfree(q->allot);
	kfree(q->hash);
	kfree(q->qs);
}

static void sfq_destroy(struct Qdisc *sch)
{
	struct sfq_sched_data *q = qdisc_priv(sch);

	q->perturb_period = 0;
	del_timer_sync(&q->perturb_timer);
	sfq_q_destroy(q);
}

static void
sfq_default_parameters(struct Qdisc *sch)
{
	struct sfq_sched_data *q = qdisc_priv(sch);

	q->quantum        = psched_mtu(sch->dev);
	q->perturbation   = 0;
	q->perturb_period = 0;
	q->hash_divisor   = SFQ_DIVISOR_DEFAULT;
	q->depth          = SFQ_DEPTH_DEFAULT;
	q->limit          = SFQ_DEPTH_DEFAULT - 1;
	q->hash_kind      = TCA_SFQ_HASH_CLASSIC;
}

static void
sfq_copy_parameters(struct sfq_sched_data *dst, struct sfq_sched_data *src)
{
	dst->quantum        = src->quantum;
	dst->perturbation   = src->perturbation;
	dst->perturb_period = src->perturb_period;
	dst->hash_divisor   = src->hash_divisor;
	dst->limit          = src->limit;
	dst->depth          = src->depth;
	dst->hash_kind      = src->hash_kind;
}

static unsigned int sfq_check_hash(unsigned int kind)
{
	switch (kind) {
	case TCA_SFQ_HASH_CTORIGDST:
	case TCA_SFQ_HASH_CTORIGSRC:
	case TCA_SFQ_HASH_CTREPLDST:
	case TCA_SFQ_HASH_CTREPLSRC:
	case TCA_SFQ_HASH_CTNATCHG:
#ifndef CONFIG_NET_SCH_SFQ_NFCT
	{
		if (net_ratelimit())
			printk(KERN_WARNING "ESFQ: Conntrack hash types disabled in kernel config. Falling back to classic.\n");
		return TCA_SFQ_HASH_CLASSIC;
	}
#endif
	case TCA_SFQ_HASH_CLASSIC:
	case TCA_SFQ_HASH_DST:
	case TCA_SFQ_HASH_SRC:
	case TCA_SFQ_HASH_FWMARK:
		return kind;
	default:
	{
		if (net_ratelimit())
			printk(KERN_WARNING "ESFQ: Unknown hash type. Falling back to classic.\n");
		return TCA_SFQ_HASH_CLASSIC;
	}
	}
}


static int
sfq_q_init(struct sfq_sched_data *q, struct rtattr *opt)
{
	struct tc_sfq_qopt *ctl = RTA_DATA(opt);
	sfq_index p = ~0U/2;
	int i;

	if (opt && opt->rta_len < RTA_LENGTH(sizeof(*ctl)))
		return -EINVAL;

	q->perturbation = 0;
	q->hash_kind = TCA_SFQ_HASH_CLASSIC;
	q->max_depth = 0;
	if (opt == NULL) {
		q->perturb_period = 0;
		q->hash_divisor = 1024;
		q->tail = q->limit = q->depth = SFQ_MAX_DEPTH;

	} else {
		struct tc_sfq_qopt *ctl = RTA_DATA(opt);
		if (ctl->quantum)
			q->quantum = ctl->quantum;
		q->perturb_period = ctl->perturb_period*HZ;
		q->hash_divisor = ctl->divisor ? : 1024;
		q->tail = q->limit = q->depth = ctl->flows ? : 128;

		if ( q->depth > p - 1 )
			return -EINVAL;

		if (ctl->limit)
			q->limit = min_t(u32, ctl->limit, q->depth - 1);

		if (ctl->hash_kind) {
			q->hash_kind = sfq_check_hash(ctl->hash_kind);
		}
	}

	q->ht = kcalloc(1<<q->hash_divisor, sizeof(sfq_index), GFP_KERNEL);
	if (!q->ht)
		goto err_case;
	q->dep = kcalloc(1 + q->depth*2, sizeof(struct sfq_head), GFP_KERNEL);
	if (!q->dep)
		goto err_case;
	q->next = kcalloc(q->depth, sizeof(sfq_index), GFP_KERNEL);
	if (!q->next)
		goto err_case;
	q->allot = kcalloc(q->depth, sizeof(short), GFP_KERNEL);
	if (!q->allot)
		goto err_case;
	q->hash = kcalloc(q->depth, sizeof(unsigned short), GFP_KERNEL);
	if (!q->hash)
		goto err_case;
	q->qs = kcalloc(q->depth, sizeof(struct sk_buff_head), GFP_KERNEL);
	if (!q->qs)
		goto err_case;

	for (i=0; i < 1<<q->hash_divisor; i++)
		q->ht[i] = q->depth;
	for (i=0; i < q->depth; i++) {
		skb_queue_head_init(&q->qs[i]);
		q->dep[i + q->depth].next = i + q->depth;
		q->dep[i + q->depth].prev = i + q->depth;
	}

	for (i=0; i < q->depth; i++)
		sfq_link(q, i);
	return 0;
err_case:
	sfq_q_destroy(q);
	return -ENOBUFS;
}

static int sfq_init(struct Qdisc *sch, struct rtattr *opt)
{
	struct sfq_sched_data *q = qdisc_priv(sch);
	int err;

	sfq_default_parameters(sch);
	if ((err = sfq_q_init(q, opt)))
		return err;

	init_timer(&q->perturb_timer);
	q->perturb_timer.data = (unsigned long)sch;
	q->perturb_timer.function = sfq_perturbation;
	if (q->perturb_period) {
		q->perturb_timer.expires = SFQ_PERTURB(q->perturb_period);
		add_timer(&q->perturb_timer);
	}

	return 0;
}

static int sfq_change(struct Qdisc *sch, struct rtattr *opt)
{
	struct sfq_sched_data *q = qdisc_priv(sch);
	struct sfq_sched_data tmp;
	struct sk_buff *skb;
	unsigned int qlen;
	int err;

	/* set up tmp queue */
	memset(&tmp, 0, sizeof(struct sfq_sched_data));
	sfq_copy_parameters(&tmp, q);
	if ((err = sfq_q_init(&tmp, opt)))
		return err;

	/* handle perturbation */
	/* This code avoids resetting the perturb_timer unless perturb_period
	 * is changed. Note that the rest of this function leaves
	 * q->perturb_timer alone, whereas all other members of q get
	 * overwritten from tmp. */
	if (!tmp.perturb_period) {
		tmp.perturbation = 0;
		del_timer(&q->perturb_timer);
	} else if (tmp.perturb_period != q->perturb_period) {
		mod_timer(&q->perturb_timer, SFQ_PERTURB(tmp.perturb_period));
	}

	/* move packets from the old queue to the tmp queue */
	sch_tree_lock(sch);
	qlen = sch->q.qlen;
	while (sch->q.qlen >= tmp.limit - 1)
		sfq_drop(sch);
	qdisc_tree_decrease_qlen(sch, qlen - sch->q.qlen);
	while ((skb = sfq_q_dequeue(q)) != NULL)
		sfq_q_enqueue(skb, &tmp, SFQ_TAIL);

	/* clean up the old queue */
	sfq_q_destroy(q);

	/* copy elements of the tmp queue into the old queue */
	sfq_copy_parameters(q, &tmp);
	q->tail      = tmp.tail;
	q->max_depth = tmp.max_depth;
	q->ht        = tmp.ht;
	q->dep       = tmp.dep;
	q->next      = tmp.next;
	q->allot     = tmp.allot;
	q->hash      = tmp.hash;
	q->qs        = tmp.qs;

	/* finish up */
	sch_tree_unlock(sch);
	return 0;
}

static int sfq_dump(struct Qdisc *sch, struct sk_buff *skb)
{
	struct sfq_sched_data *q = qdisc_priv(sch);
	unsigned char *b = skb_tail_pointer(skb);
	struct tc_sfq_qopt opt;

	opt.quantum = q->quantum;
	opt.perturb_period = q->perturb_period;
	opt.limit = q->limit;
	opt.divisor = q->hash_divisor;
	opt.flows = q->depth;
	opt.hash_kind = q->hash_kind;

	RTA_PUT(skb, TCA_OPTIONS, sizeof(opt), &opt);

	return skb->len;

rtattr_failure:
	nlmsg_trim(skb, b);
	return -1;
}

static struct Qdisc_ops sfq_qdisc_ops __read_mostly = {
	.next		=	NULL,
	.cl_ops		=	NULL,
	.id		=	"sfq",
	.priv_size	=	sizeof(struct sfq_sched_data),
	.enqueue	=	sfq_enqueue,
	.dequeue	=	sfq_dequeue,
	.requeue	=	sfq_requeue,
	.drop		=	sfq_drop,
	.init		=	sfq_init,
	.reset		=	sfq_reset,
	.destroy	=	sfq_destroy,
	.change		=	sfq_change,
	.dump		=	sfq_dump,
	.owner		=	THIS_MODULE,
};

static int __init sfq_module_init(void)
{
	return register_qdisc(&sfq_qdisc_ops);
}
static void __exit sfq_module_exit(void)
{
	unregister_qdisc(&sfq_qdisc_ops);
}
module_init(sfq_module_init)
module_exit(sfq_module_exit)
MODULE_LICENSE("GPL");
