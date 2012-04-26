/*
 * Copyright (c) 2006 Patrick McHardy <kaber@trash.net>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */

#include <linux/module.h>
#include <linux/init.h>
#include <linux/skbuff.h>

#include <linux/netfilter/x_tables.h>
#include <linux/netfilter/xt_NFLOG.h>

MODULE_AUTHOR("Patrick McHardy <kaber@trash.net>");
MODULE_DESCRIPTION("x_tables NFLOG target");
MODULE_LICENSE("GPL");
MODULE_ALIAS("ipt_NFLOG");
MODULE_ALIAS("ip6t_NFLOG");

static unsigned int
nflog_target(struct sk_buff *skb, const struct xt_action_param *par)
{
	const struct xt_nflog_info *info = par->targinfo;
	struct nf_loginfo li;

	li.type		     = NF_LOG_TYPE_ULOG;
	li.u.ulog.copy_len   = info->len;
	li.u.ulog.group	     = info->group;
	li.u.ulog.qthreshold = info->threshold;

	nf_log_packet(par->family, par->hooknum, skb, par->in,
	              par->out, &li, "%s", info->prefix);
	return XT_CONTINUE;
}

static bool nflog_checkentry(const struct xt_tgchk_param *par)
{
	const struct xt_nflog_info *info = par->targinfo;

	if (info->flags & ~XT_NFLOG_MASK)
		return false;
	if (info->prefix[sizeof(info->prefix) - 1] != '\0')
		return false;
	return true;
}

static struct xt_target xt_nflog_target __read_mostly = {
	.name		= "NFLOG",
	.family		= NFPROTO_UNSPEC,
	.checkentry	= nflog_checkentry,
	.target		= nflog_target,
	.targetsize	= sizeof(struct xt_nflog_info),
	.me		= THIS_MODULE,
};

static int __init xt_nflog_init(void)
{
	return xt_register_target(&xt_nflog_target);
}

static void __exit xt_nflog_fini(void)
{
	xt_unregister_target(&xt_nflog_target);
}

module_init(xt_nflog_init);
module_exit(xt_nflog_fini);
