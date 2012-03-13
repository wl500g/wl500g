/*
 *  iptables module to match inet_addr_type() of an ip.
 *
 *  Copyright (c) 2004 Patrick McHardy <kaber@trash.net>
 *  (C) 2007 Laszlo Attila Toth <panther@balabit.hu>
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License version 2 as
 *  published by the Free Software Foundation.
 */

#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/skbuff.h>
#include <linux/netdevice.h>
#include <linux/ip.h>
#include <net/route.h>

#if defined(CONFIG_IP6_NF_IPTABLES) || defined(CONFIG_IP6_NF_IPTABLES_MODULE)
#include <net/ipv6.h>
#include <net/ip6_route.h>
#include <net/ip6_fib.h>
#endif

#include <linux/netfilter/xt_addrtype.h>
#include <linux/netfilter/x_tables.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Patrick McHardy <kaber@trash.net>");
MODULE_DESCRIPTION("Xtables: address type match");
MODULE_ALIAS("ipt_addrtype");
MODULE_ALIAS("ip6t_addrtype");

#if defined(CONFIG_IP6_NF_IPTABLES) || defined(CONFIG_IP6_NF_IPTABLES_MODULE)
static u32 match_lookup_rt6(const struct net_device *dev,
			    const struct in6_addr *addr)
{
	const struct nf_afinfo *afinfo;
	struct flowi flow6;
	struct rt6_info *rt;
	u32 ret;
	int route_err;

	memset(&flow6, 0, sizeof(flow6));
	ipv6_addr_copy(&flow6.fl6_dst, addr);
	if (dev)
		flow6.oif = dev->ifindex;

	rcu_read_lock();

	afinfo = nf_get_afinfo(NFPROTO_IPV6);
	if (afinfo != NULL)
		route_err = afinfo->route((struct dst_entry **)&rt,	&flow6);
	else
		route_err = 1;

	rcu_read_unlock();

	if (route_err)
		return XT_ADDRTYPE_UNREACHABLE;

	if (rt->rt6i_flags & RTF_REJECT)
		ret = XT_ADDRTYPE_UNREACHABLE;
	else
		ret = 0;

	if (rt->rt6i_flags & RTF_LOCAL)
		ret |= XT_ADDRTYPE_LOCAL;
	if (rt->rt6i_flags & RTF_ANYCAST)
		ret |= XT_ADDRTYPE_ANYCAST;


	dst_release(&rt->u.dst);
	return ret;
}

static bool match_type6(const struct net_device *dev,
				const struct in6_addr *addr, u16 mask)
{
	int addr_type = ipv6_addr_type(addr);

	if ((mask & XT_ADDRTYPE_MULTICAST) &&
	    !(addr_type & IPV6_ADDR_MULTICAST))
		return false;
	if ((mask & XT_ADDRTYPE_UNICAST) && !(addr_type & IPV6_ADDR_UNICAST))
		return false;
	if ((mask & XT_ADDRTYPE_UNSPEC) && addr_type != IPV6_ADDR_ANY)
		return false;

	if ((XT_ADDRTYPE_LOCAL | XT_ADDRTYPE_ANYCAST |
	     XT_ADDRTYPE_UNREACHABLE) & mask)
		return !!(mask & match_lookup_rt6(dev, addr));
	return true;
}

static bool
addrtype_mt6(const struct net_device *dev,
	const struct sk_buff *skb, const struct xt_addrtype_info_v1 *info)
{
	const struct ipv6hdr *iph = ipv6_hdr(skb);
	bool ret = true;

	if (info->source)
		ret &= match_type6(dev, &iph->saddr, info->source) ^
		       (info->flags & XT_ADDRTYPE_INVERT_SOURCE);
	if (ret && info->dest)
		ret &= match_type6(dev, &iph->daddr, info->dest) ^
		       !!(info->flags & XT_ADDRTYPE_INVERT_DEST);
	return ret;
}
#endif

static inline bool match_type(const struct net_device *dev, __be32 addr,
			      u_int16_t mask)
{
	return !!(mask & (1 << inet_dev_addr_type(dev, addr)));
}

static bool
addrtype_mt_v0(const struct sk_buff *skb, const struct net_device *in,
	       const struct net_device *out, const struct xt_match *match,
	       const void *matchinfo, int offset, unsigned int protoff,
	       bool *hotdrop)
{
	const struct xt_addrtype_info *info = matchinfo;
	const struct iphdr *iph = ip_hdr(skb);
	int ret = true;

	if (info->source)
		ret &= match_type(NULL, iph->saddr, info->source) ^
		       info->invert_source;
	if (info->dest)
		ret &= match_type(NULL, iph->daddr, info->dest) ^
		       info->invert_dest;

	return ret;
}

static bool
addrtype_mt_v1(const struct sk_buff *skb, const struct net_device *in,
	       const struct net_device *out, const struct xt_match *match,
	       const void *matchinfo, int offset, unsigned int protoff,
	       bool *hotdrop)
{
	const struct xt_addrtype_info_v1 *info = matchinfo;
	const struct iphdr *iph;
	const struct net_device *dev = NULL;
	bool ret = true;

	if (info->flags & XT_ADDRTYPE_LIMIT_IFACE_IN)
		dev = in;
	else if (info->flags & XT_ADDRTYPE_LIMIT_IFACE_OUT)
		dev = out;

#if defined(CONFIG_IP6_NF_IPTABLES) || defined(CONFIG_IP6_NF_IPTABLES_MODULE)
	if (match->family == NFPROTO_IPV6)
		return addrtype_mt6(dev, skb, info);
#endif
	iph = ip_hdr(skb);
	if (info->source)
		ret &= match_type(dev, iph->saddr, info->source) ^
		       (info->flags & XT_ADDRTYPE_INVERT_SOURCE);
	if (ret && info->dest)
		ret &= match_type(dev, iph->daddr, info->dest) ^
		       !!(info->flags & XT_ADDRTYPE_INVERT_DEST);
	return ret;
}

static bool
addrtype_mt_checkentry_v1(const char *tablename, const void *ip_void,
			  const struct xt_match *match, void *matchinfo,
			  unsigned int hook_mask)
{
	struct xt_addrtype_info_v1 *info = matchinfo;

	if (info->flags & XT_ADDRTYPE_LIMIT_IFACE_IN &&
	    info->flags & XT_ADDRTYPE_LIMIT_IFACE_OUT) {
		printk(KERN_ERR "ipt_addrtype: both incoming and outgoing "
				"interface limitation cannot be selected\n");
		return 0;
	}

	if (hook_mask & (1 << NF_IP_PRE_ROUTING | 1 << NF_IP_LOCAL_IN) &&
	    info->flags & XT_ADDRTYPE_LIMIT_IFACE_OUT) {
		printk(KERN_ERR "ipt_addrtype: output interface limitation "
				"not valid in PRE_ROUTING and INPUT\n");
		return 0;
	}

	if (hook_mask & (1 << NF_IP_POST_ROUTING | 1 << NF_IP_LOCAL_OUT) &&
	    info->flags & XT_ADDRTYPE_LIMIT_IFACE_IN) {
		printk(KERN_ERR "ipt_addrtype: input interface limitation "
				"not valid in POST_ROUTING and OUTPUT\n");
		return 0;
	}

#if defined(CONFIG_IP6_NF_IPTABLES) || defined(CONFIG_IP6_NF_IPTABLES_MODULE)
	if (match->family == NFPROTO_IPV6) {
		if ((info->source | info->dest) & XT_ADDRTYPE_BLACKHOLE) {
			pr_err("ipv6 BLACKHOLE matching not supported\n");
			return -EINVAL;
		}
		if ((info->source | info->dest) >= XT_ADDRTYPE_PROHIBIT) {
			pr_err("ipv6 PROHIBT (THROW, NAT ..) matching not supported\n");
			return -EINVAL;
		}
		if ((info->source | info->dest) & XT_ADDRTYPE_BROADCAST) {
			pr_err("ipv6 does not support BROADCAST matching\n");
			return -EINVAL;
		}
	}
#endif
	return true;
}

static struct xt_match addrtype_mt_reg[] __read_mostly = {
	{
		.name		= "addrtype",
		.family		= AF_INET,
		.match		= addrtype_mt_v0,
		.matchsize	= sizeof(struct xt_addrtype_info),
		.me		= THIS_MODULE
	},
	{
		.name		= "addrtype",
		.family		= NFPROTO_UNSPEC,
		.revision	= 1,
		.match		= addrtype_mt_v1,
		.checkentry	= addrtype_mt_checkentry_v1,
		.matchsize	= sizeof(struct xt_addrtype_info_v1),
		.me		= THIS_MODULE
	},
};

static int __init addrtype_mt_init(void)
{
	return xt_register_matches(addrtype_mt_reg,
				   ARRAY_SIZE(addrtype_mt_reg));
}

static void __exit addrtype_mt_fini(void)
{
	xt_unregister_matches(addrtype_mt_reg, ARRAY_SIZE(addrtype_mt_reg));
}

module_init(addrtype_mt_init);
module_exit(addrtype_mt_fini);
