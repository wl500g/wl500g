/* (C) 1999-2001 Michal Ludvig <michal@logix.cz>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */

#include <linux/module.h>
#include <linux/skbuff.h>
#include <linux/if_ether.h>
#include <linux/if_packet.h>
#include <linux/in.h>
#include <linux/ip.h>
#include <linux/ipv6.h>

#include <linux/netfilter/xt_pkttype.h>
#include <linux/netfilter/x_tables.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Michal Ludvig <michal@logix.cz>");
MODULE_DESCRIPTION("IP tables match to match on linklayer packet type");
MODULE_ALIAS("ipt_pkttype");
MODULE_ALIAS("ip6t_pkttype");

static bool
pkttype_mt(const struct sk_buff *skb, struct xt_action_param *par)
{
	const struct xt_pkttype_info *info = par->matchinfo;
	u_int8_t type;

	if (skb->pkt_type != PACKET_LOOPBACK)
		type = skb->pkt_type;
	else if (par->family == NFPROTO_IPV4 &&
	    MULTICAST(ip_hdr(skb)->daddr))
		type = PACKET_MULTICAST;
	else if (par->family == NFPROTO_IPV6 &&
	    ipv6_hdr(skb)->daddr.s6_addr[0] == 0xFF)
		type = PACKET_MULTICAST;
	else
		type = PACKET_BROADCAST;

	return (type == info->pkttype) ^ info->invert;
}

static struct xt_match xt_pkttype_match __read_mostly = {
	.name		= "pkttype",
	.revision   = 0,
	.family		= NFPROTO_UNSPEC,
	.match		= pkttype_mt,
	.matchsize	= sizeof(struct xt_pkttype_info),
	.me		= THIS_MODULE,
};

static int __init xt_pkttype_init(void)
{
	return xt_register_match(&xt_pkttype_match);
}

static void __exit xt_pkttype_fini(void)
{
	xt_unregister_match(&xt_pkttype_match);
}

module_init(xt_pkttype_init);
module_exit(xt_pkttype_fini);
