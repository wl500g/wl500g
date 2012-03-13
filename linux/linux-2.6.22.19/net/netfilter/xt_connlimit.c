/*
 * netfilter module to limit the number of parallel tcp
 * connections per IP address.
 *   (c) 2000 Gerd Knorr <kraxel@bytesex.org>
 *   Nov 2002: Martin Bene <martin.bene@icomedias.com>:
 *		only ignore TIME_WAIT or gone connections
 *   (C) CC Computer Consultants GmbH, 2007
 *   Contact: <jengelh@computergmbh.de>
 *
 * based on ...
 *
 * Kernel module to match connection tracking information.
 * GPL (C) 1999  Rusty Russell (rusty@rustcorp.com.au).
 */
#include <linux/in.h>
#include <linux/in6.h>
#include <linux/ip.h>
#include <linux/ipv6.h>
#include <linux/jhash.h>
#include <linux/list.h>
#include <linux/module.h>
#include <linux/random.h>
#include <linux/skbuff.h>
#include <linux/spinlock.h>
#include <linux/netfilter/nf_conntrack_tcp.h>
#include <linux/netfilter/x_tables.h>
#include <linux/netfilter/xt_connlimit.h>
#include <net/netfilter/nf_conntrack.h>
#include <net/netfilter/nf_conntrack_core.h>
#include <net/netfilter/nf_conntrack_tuple.h>

/* we will save the tuples of all connections we care about */
struct xt_connlimit_conn {
	struct hlist_node		node;
	struct nf_conntrack_tuple	tuple;
	union nf_inet_addr		addr;
};

struct xt_connlimit_data {
	struct hlist_head	iphash[256];
	spinlock_t		lock;
};

static u_int32_t connlimit_rnd __read_mostly;

static inline unsigned int connlimit_iphash(u_int32_t addr)
{
	return jhash_1word(addr, connlimit_rnd) & 0xFF;
}

static inline unsigned int
connlimit_iphash6(const union nf_inet_addr *addr,
                  const union nf_inet_addr *mask)
{
	union nf_inet_addr res;
	unsigned int i;

	for (i = 0; i < ARRAY_SIZE(addr->ip6); ++i)
		res.ip6[i] = addr->ip6[i] & mask->ip6[i];

	return jhash2(res.ip6, ARRAY_SIZE(res.ip6), connlimit_rnd) & 0xFF;
}

static inline bool already_closed(const struct nf_conn *conn)
{
	u_int16_t proto = conn->tuplehash[0].tuple.dst.protonum;

	if (proto == IPPROTO_TCP)
		return conn->proto.tcp.state == TCP_CONNTRACK_TIME_WAIT ||
		       conn->proto.tcp.state == TCP_CONNTRACK_CLOSE;
	else
		return 0;
}

static inline unsigned int
same_source_net(const union nf_inet_addr *addr,
		const union nf_inet_addr *mask,
		const union nf_inet_addr *u3, unsigned short family)
{
	if (family == AF_INET) {
		return (addr->ip & mask->ip) == (u3->ip & mask->ip);
	} else {
		union nf_inet_addr lh, rh;
		unsigned int i;

		for (i = 0; i < ARRAY_SIZE(addr->ip6); ++i) {
			lh.ip6[i] = addr->ip6[i] & mask->ip6[i];
			rh.ip6[i] = u3->ip6[i] & mask->ip6[i];
		}

		return memcmp(&lh.ip6, &rh.ip6, sizeof(lh.ip6)) == 0;
	}
}

static int count_them(struct xt_connlimit_data *data,
		      const struct nf_conntrack_tuple *tuple,
		      const union nf_inet_addr *addr,
		      const union nf_inet_addr *mask,
		      unsigned short family)
{
	struct nf_conntrack_tuple_hash *found;
	struct xt_connlimit_conn *conn;
	struct hlist_node *pos, *n;
	struct nf_conn *found_ct;
	struct hlist_head *hash;
	bool addit = true;
	int matches = 0;


	if (family == AF_INET6)
		hash = &data->iphash[connlimit_iphash6(addr, mask)];
	else
		hash = &data->iphash[connlimit_iphash(addr->ip & mask->ip)];

	read_lock_bh(&nf_conntrack_lock);

	/* check the saved connections */
	hlist_for_each_entry_safe(conn, pos, n, hash, node) {
		found    = __nf_conntrack_find(&conn->tuple);
		found_ct = NULL;

		if (found != NULL)
			found_ct = nf_ct_tuplehash_to_ctrack(found);

		if (found_ct != NULL &&
		    nf_ct_tuple_equal(&conn->tuple, tuple) &&
		    !already_closed(found_ct))
			/*
			 * Just to be sure we have it only once in the list.
			 * We should not see tuples twice unless someone hooks
			 * this into a table without "-p tcp --syn".
			 */
			addit = false;

		if (found == NULL) {
			/* this one is gone */
			hlist_del(&conn->node);
			kfree(conn);
			continue;
		}

		if (already_closed(found_ct)) {
			/*
			 * we do not care about connections which are
			 * closed already -> ditch it
			 */
			hlist_del(&conn->node);
			kfree(conn);
			continue;
		}

		if (same_source_net(addr, mask, &conn->addr, family))
			/* same source network -> be counted! */
			++matches;
	}

	read_unlock_bh(&nf_conntrack_lock);

	if (addit) {
		/* save the new connection in our list */
		conn = kzalloc(sizeof(*conn), GFP_ATOMIC);
		if (conn == NULL)
			return -ENOMEM;
		conn->tuple = *tuple;
		conn->addr = *addr;
		hlist_add_head(&conn->node, hash);
		++matches;
	}

	return matches;
}

static bool
connlimit_mt(const struct sk_buff *skb, const struct net_device *in,
             const struct net_device *out, const struct xt_match *match,
             const void *matchinfo, int offset, unsigned int protoff,
             bool *hotdrop)
{
	const struct xt_connlimit_info *info = matchinfo;
	union nf_inet_addr addr;
	struct nf_conntrack_tuple tuple;
	const struct nf_conntrack_tuple *tuple_ptr = &tuple;
	enum ip_conntrack_info ctinfo;
	const struct nf_conn *ct;
	int connections;

	ct = nf_ct_get(skb, &ctinfo);
	if (ct != NULL)
		tuple_ptr = &ct->tuplehash[IP_CT_DIR_ORIGINAL].tuple;
	else if (!nf_ct_get_tuplepr(skb, skb_network_offset(skb),
				    match->family, &tuple))
		goto hotdrop;

	if (match->family == AF_INET6) {
		const struct ipv6hdr *iph = ipv6_hdr(skb);
		memcpy(&addr.ip6, (info->flags & XT_CONNLIMIT_DADDR) ?
		       &iph->daddr : &iph->saddr, sizeof(addr.ip6));
	} else {
		const struct iphdr *iph = ip_hdr(skb);
		addr.ip = (info->flags & XT_CONNLIMIT_DADDR) ?
			  iph->daddr : iph->saddr;
	}

	spin_lock_bh(&info->data->lock);
	connections = count_them(info->data, tuple_ptr, &addr,
	                         &info->_mask, match->family);
	spin_unlock_bh(&info->data->lock);

	if (connections < 0)
		/* kmalloc failed, drop it entirely */
			goto hotdrop;

	return (connections > info->limit) ^
	       !!(info->flags & XT_CONNLIMIT_INVERT);

 hotdrop:
	*hotdrop = true;
	return false;
}

static bool
connlimit_mt_check(const char *tablename, const void *ip,
                   const struct xt_match *match, void *matchinfo,
                   unsigned int hook_mask)
{
	struct xt_connlimit_info *info = matchinfo;
	unsigned int i;

	if (unlikely(!connlimit_rnd)) {
		u_int32_t rand;

		do {
			get_random_bytes(&rand, sizeof(rand));
		} while (!rand);
		cmpxchg(&connlimit_rnd, 0, rand);
	}
	if (nf_ct_l3proto_try_module_get(match->family) < 0) {
		printk(KERN_WARNING "cannot load conntrack support for "
		       "address family %u\n", match->family);
		return false;
	}

	/* init private data */
	info->data = kmalloc(sizeof(struct xt_connlimit_data), GFP_KERNEL);
	if (info->data == NULL) {
		nf_ct_l3proto_module_put(match->family);
		return false;
	}

	spin_lock_init(&info->data->lock);
	for (i = 0; i < ARRAY_SIZE(info->data->iphash); ++i)
		INIT_HLIST_HEAD(&info->data->iphash[i]);

	return true;
}

static void
connlimit_mt_destroy(const struct xt_match *match, void *matchinfo)
{
	struct xt_connlimit_info *info = matchinfo;
	struct xt_connlimit_conn *conn;
	struct hlist_node *pos, *n;
	struct hlist_head *hash = info->data->iphash;
	unsigned int i;

	nf_ct_l3proto_module_put(match->family);

	for (i = 0; i < ARRAY_SIZE(info->data->iphash); ++i) {
		hlist_for_each_entry_safe(conn, pos, n, &hash[i], node) {
			hlist_del(&conn->node);
			kfree(conn);
		}
	}

	kfree(info->data);
}

static struct xt_match connlimit_mt_reg __read_mostly = {
	.name       = "connlimit",
	.revision   = 1,
	.family     = NFPROTO_UNSPEC,
	.checkentry = connlimit_mt_check,
	.match      = connlimit_mt,
	.matchsize  = sizeof(struct xt_connlimit_info),
	.destroy    = connlimit_mt_destroy,
	.me         = THIS_MODULE,
};

static int __init connlimit_mt_init(void)
{
	return xt_register_match(&connlimit_mt_reg);
}

static void __exit connlimit_mt_exit(void)
{
	xt_unregister_match(&connlimit_mt_reg);
}

module_init(connlimit_mt_init);
module_exit(connlimit_mt_exit);
MODULE_AUTHOR("Jan Engelhardt <jengelh@computergmbh.de>");
MODULE_DESCRIPTION("netfilter xt_connlimit match module");
MODULE_LICENSE("GPL");
MODULE_ALIAS("ipt_connlimit");
MODULE_ALIAS("ip6t_connlimit");
