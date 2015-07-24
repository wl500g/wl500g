/* iptables module to match on related connections */
/*
 * (C) 2001 Martin Josefsson <gandalf@wlug.westbo.se>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */

#include <linux/module.h>
#include <linux/skbuff.h>
#include <linux/netfilter.h>
#include <net/netfilter/nf_conntrack.h>
#include <net/netfilter/nf_conntrack_core.h>
#include <net/netfilter/nf_conntrack_helper.h>
#include <linux/netfilter/x_tables.h>
#include <linux/netfilter/xt_helper.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Martin Josefsson <gandalf@netfilter.org>");
MODULE_DESCRIPTION("iptables helper match module");
MODULE_ALIAS("ipt_helper");
MODULE_ALIAS("ip6t_helper");

static bool
helper_mt(const struct sk_buff *skb, struct xt_action_param *par)
{
	const struct xt_helper_info *info = par->matchinfo;
	struct nf_conn *ct;
	struct nf_conn_help *master_help;
	enum ip_conntrack_info ctinfo;
	bool ret = info->invert;

	ct = nf_ct_get((struct sk_buff *)skb, &ctinfo);
	if (!ct) {
		pr_debug("xt_helper: Eek! invalid conntrack?\n");
		return ret;
	}

	if (!ct->master) {
		pr_debug("xt_helper: conntrack %p has no master\n", ct);
		return ret;
	}

	read_lock_bh(&nf_conntrack_lock);
	master_help = nfct_help(ct->master);
	if (!master_help || !master_help->helper) {
		pr_debug("xt_helper: master ct %p has no helper\n",
			ct->master);
		goto out_unlock;
	}

	pr_debug("master's name = %s , info->name = %s\n",
		master_help->helper->name, info->name);

	if (info->name[0] == '\0')
		ret = !ret;
	else
		ret ^= !strncmp(master_help->helper->name, info->name,
				strlen(master_help->helper->name));
out_unlock:
	read_unlock_bh(&nf_conntrack_lock);
	return ret;
}

static bool helper_mt_check(const struct xt_mtchk_param *par)
{
	struct xt_helper_info *info = par->matchinfo;

	if (nf_ct_l3proto_try_module_get(par->family) < 0) {
		printk(KERN_WARNING "can't load conntrack support for "
				    "proto=%u\n", par->family);
		return false;
	}
	info->name[29] = '\0';
	return true;
}

static void destroy(const struct xt_mtdtor_param *par)
{
	nf_ct_l3proto_module_put(par->family);
}

static struct xt_match xt_helper_match __read_mostly = {
	.name		= "helper",
	.revision   = 0,
	.family		= NFPROTO_UNSPEC,
	.checkentry	= helper_mt_check,
	.match		= helper_mt,
	.destroy	= destroy,
	.matchsize	= sizeof(struct xt_helper_info),
	.me		= THIS_MODULE,
};

static int __init xt_helper_init(void)
{
	return xt_register_match(&xt_helper_match);
}

static void __exit xt_helper_fini(void)
{
	xt_unregister_match(&xt_helper_match);
}

module_init(xt_helper_init);
module_exit(xt_helper_fini);

