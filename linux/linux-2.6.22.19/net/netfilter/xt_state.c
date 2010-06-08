/* Kernel module to match connection tracking information. */

/* (C) 1999-2001 Paul `Rusty' Russell
 * (C) 2002-2005 Netfilter Core Team <coreteam@netfilter.org>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */

#include <linux/module.h>
#include <linux/skbuff.h>
#include <net/netfilter/nf_conntrack.h>
#include <linux/netfilter/x_tables.h>
#include <linux/netfilter/xt_state.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Rusty Russell <rusty@rustcorp.com.au>");
MODULE_DESCRIPTION("ip[6]_tables connection tracking state match module");
MODULE_ALIAS("ipt_state");
MODULE_ALIAS("ip6t_state");

static bool
match(const struct sk_buff *skb, struct xt_action_param *par)
{
	const struct xt_state_info *sinfo = par->matchinfo;
	enum ip_conntrack_info ctinfo;
	unsigned int statebit;
	struct nf_conn *ct = nf_ct_get(skb, &ctinfo);

	if (!ct)
		statebit = XT_STATE_INVALID;
	else {
		if (nf_ct_is_untracked(ct))
			statebit = XT_STATE_UNTRACKED;
		else
			statebit = XT_STATE_BIT(ctinfo);
	}
	return (sinfo->statemask & statebit);
}

static bool check(const struct xt_mtchk_param *par)
{
	if (nf_ct_l3proto_try_module_get(par->family) < 0) {
		printk(KERN_WARNING "can't load conntrack support for "
				    "proto=%u\n", par->family);
		return false;
	}
	return true;
}

static void destroy(const struct xt_mtdtor_param *par)
{
	nf_ct_l3proto_module_put(par->family);
}

static struct xt_match xt_state_match __read_mostly = {
	.name		= "state",
	.revision   = 0,
	.family		= NFPROTO_UNSPEC,
	.checkentry	= check,
	.match		= match,
	.destroy	= destroy,
	.matchsize	= sizeof(struct xt_state_info),
	.me		= THIS_MODULE,
};

static int __init xt_state_init(void)
{
	return xt_register_match(&xt_state_match);
}

static void __exit xt_state_fini(void)
{
	xt_unregister_match(&xt_state_match);
}

module_init(xt_state_init);
module_exit(xt_state_fini);
