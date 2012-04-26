/* Kernel module to match MAC address parameters. */

/* (C) 1999-2001 Paul `Rusty' Russell
 * (C) 2002-2004 Netfilter Core Team <coreteam@netfilter.org>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */

#include <linux/module.h>
#include <linux/skbuff.h>
#include <linux/if_ether.h>
#include <linux/etherdevice.h>

#include <linux/netfilter_ipv4.h>
#include <linux/netfilter_ipv6.h>
#include <linux/netfilter/xt_mac.h>
#include <linux/netfilter/x_tables.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Netfilter Core Team <coreteam@netfilter.org>");
MODULE_DESCRIPTION("iptables mac matching module");
MODULE_ALIAS("ipt_mac");
MODULE_ALIAS("ip6t_mac");

static bool match(const struct sk_buff *skb, struct xt_action_param *par)
{
    const struct xt_mac_info *info = par->matchinfo;

    /* Is mac pointer valid? */
    return (skb_mac_header(skb) >= skb->head &&
	    (skb_mac_header(skb) + ETH_HLEN) <= skb->data
	    /* If so, compare... */
	    && ((!compare_ether_addr(eth_hdr(skb)->h_source, info->srcaddr))
		^ info->invert));
}

static struct xt_match xt_mac_match __read_mostly = {
	.name		= "mac",
	.revision   = 0,
	.family		= NFPROTO_UNSPEC,
	.match		= match,
	.matchsize	= sizeof(struct xt_mac_info),
	.hooks		= (1 << NF_IP_PRE_ROUTING) |
			  (1 << NF_IP_LOCAL_IN) |
			  (1 << NF_IP_FORWARD),
	.me		= THIS_MODULE,
};

static int __init xt_mac_init(void)
{
	return xt_register_match(&xt_mac_match);
}

static void __exit xt_mac_fini(void)
{
	xt_unregister_match(&xt_mac_match);
}

module_init(xt_mac_init);
module_exit(xt_mac_fini);
