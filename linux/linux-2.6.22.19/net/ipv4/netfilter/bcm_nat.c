/*
 * Packet matching code.
 *
 * Copyright (C) 1999 Paul `Rusty' Russell & Michael J. Neuling
 * Copyright (C) 2009-2002 Netfilter core team <coreteam@netfilter.org>
 *
 * 19 Jan 2002 Harald Welte <laforge@gnumonks.org>
 * 	- increase module usage count as soon as we have rules inside
 * 	  a table
 */
#include <linux/cache.h>
#include <linux/skbuff.h>
#include <linux/kmod.h>
#include <linux/vmalloc.h>
#include <linux/netdevice.h>
#include <linux/module.h>
#include <linux/ip.h>
#include <net/route.h>
#include <net/ip.h>
#include <linux/netfilter.h>
#include <linux/netfilter_ipv4.h>
#include <net/netfilter/nf_nat_core.h>
#include <net/netfilter/nf_conntrack.h>
#include <net/netfilter/nf_conntrack_core.h>
#include <linux/netfilter/nf_conntrack_common.h>
#include <linux/netfilter_ipv4/ip_tables.h>

typedef int (*bcmNatHitHook)(struct sk_buff *skb);
typedef int (*bcmNatBindHook)(struct nf_conn *ct,
	enum ip_conntrack_info ctinfo,
	struct sk_buff *skb,
	struct nf_conntrack_l3proto *l3proto,
	struct nf_conntrack_l4proto *l4proto);

extern bcmNatHitHook bcm_nat_hit_hook;
extern bcmNatBindHook bcm_nat_bind_hook;
extern int nf_conntrack_fastnat;

static inline int
bcm_nat_hit_hook_func(bcmNatHitHook hook_func) {
	bcm_nat_hit_hook = hook_func;
	return 1;
};

static inline int
bcm_nat_bind_hook_func(bcmNatBindHook hook_func) {
	bcm_nat_bind_hook = hook_func;
	return 1;
};

extern int
bcm_manip_pkt(u_int16_t proto,
	struct sk_buff *skb,
	unsigned int iphdroff,
	const struct nf_conntrack_tuple *target,
	enum nf_nat_manip_type maniptype);

extern int
bcm_nf_ct_invert_tuple(struct nf_conntrack_tuple *inverse,
	const struct nf_conntrack_tuple *orig,
	const struct nf_conntrack_l3proto *l3proto,
	const struct nf_conntrack_l4proto *l4proto);

/* 
 * Send packets to output.
 */
static inline int
bcm_fast_path_output(struct sk_buff *skb)
{
	struct dst_entry *dst = skb->dst;
	struct net_device *dev = dst->dev;
	int hh_len = LL_RESERVED_SPACE(dev);
	int ret;

	/* Be paranoid, rather than too clever. */
	if (unlikely(skb_headroom(skb) < hh_len && dev->hard_header)) {
		struct sk_buff *skb2;

		skb2 = skb_realloc_headroom(skb, hh_len);
		if (skb2 == NULL) {
			kfree_skb(skb);
			return -ENOMEM;
		}
		if (skb->sk)
			skb_set_owner_w(skb2, skb->sk);
		kfree_skb(skb);
		skb = skb2;
	}

	if (dst->hh)
		ret = neigh_hh_output(dst->hh, skb);
	else if (dst->neighbour)
		ret = dst->neighbour->output(skb);
	else {
		if (net_ratelimit())
			printk(KERN_DEBUG "bcm_fast_path_output: No header cache and no neighbour!\n");
		kfree_skb(skb);
		return -EINVAL;
	}

	/* Don't return 1 */
	return (ret == 1) ? 0 : ret;
}

static inline int
bcm_skb_dst_mtu(struct sk_buff *skb)
{
#ifdef IP_PMTUDISC_PROBE
	struct inet_sock *inet = skb->sk ? inet_sk(skb->sk) : NULL;

	return (inet && inet->pmtudisc == IP_PMTUDISC_PROBE) ?
	       skb->dst->dev->mtu : dst_mtu(skb->dst);
#else
	return dst_mtu(skb->dst);
#endif
}

static int
bcm_fast_path(struct sk_buff *skb)
{
	if (skb->dst == NULL) {
		struct iphdr *iph = ip_hdr(skb);
		struct net_device *dev = skb->dev;

		if (ip_route_input(skb, iph->daddr, iph->saddr, iph->tos, dev)) {
			kfree_skb(skb);
			return -EINVAL;
		}

		/*  Change skb owner to output device */
		skb->dev = skb->dst->dev;
	}

	if (skb->len > bcm_skb_dst_mtu(skb) && !skb_is_gso(skb))
		return ip_fragment(skb, bcm_fast_path_output);
	else
		return bcm_fast_path_output(skb);
}

static int
bcm_do_bindings(struct nf_conn *ct,
	enum ip_conntrack_info ctinfo,
	struct sk_buff *skb,
	struct nf_conntrack_l3proto *l3proto,
	struct nf_conntrack_l4proto *l4proto)
{
	unsigned int i;
	static int hn[2] = {NF_IP_PRE_ROUTING, NF_IP_POST_ROUTING};
	enum ip_conntrack_dir dir = CTINFO2DIR(ctinfo);

	for (i = 0; i < 2; i++) {
		enum nf_nat_manip_type mtype = HOOK2MANIP(hn[i]);
		unsigned long statusbit;

		if (mtype == IP_NAT_MANIP_SRC)
			statusbit = IPS_SRC_NAT;
		else
			statusbit = IPS_DST_NAT;

		/* Invert if this is reply dir. */
		if (dir == IP_CT_DIR_REPLY)
			statusbit ^= IPS_NAT_MASK;

		if (ct->status & statusbit) {
			struct nf_conntrack_tuple target;

			if (!skb_make_writable(skb, 0))
				return NF_DROP;

			if (skb->dst == NULL && mtype == IP_NAT_MANIP_SRC) {
				struct iphdr *iph = ip_hdr(skb);
				struct net_device *dev = skb->dev;

				if (ip_route_input(skb, iph->daddr, iph->saddr, iph->tos, dev))
					return NF_DROP;
				/* Change skb owner to output device */
				skb->dev = skb->dst->dev;
			}

			/* We are aiming to look like inverse of other direction. */
			bcm_nf_ct_invert_tuple(&target, &ct->tuplehash[!dir].tuple, l3proto, l4proto);

			if (!bcm_manip_pkt(target.dst.protonum, skb, 0, &target, mtype))
				return NF_DROP;
		}
	}

	return NF_FAST_NAT;
}

static int __init bcm_nat_init(void)
{
	bcm_nat_hit_hook_func(bcm_fast_path);
	bcm_nat_bind_hook_func(bcm_do_bindings);
	if (!nf_conntrack_fastnat)
		nf_conntrack_fastnat = 1;
	printk("BCM fast NAT: INIT\n");
	return 0;
}

static void __exit bcm_nat_fini(void)
{
	nf_conntrack_fastnat = 0;
	bcm_nat_hit_hook_func(NULL);
	bcm_nat_bind_hook_func(NULL);
}

module_init(bcm_nat_init);
module_exit(bcm_nat_fini);
MODULE_LICENSE("Proprietary");
