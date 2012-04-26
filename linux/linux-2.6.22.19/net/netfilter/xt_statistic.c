/*
 * Copyright (c) 2006 Patrick McHardy <kaber@trash.net>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 * Based on ipt_random and ipt_nth by Fabrice MARIE <fabrice@netfilter.org>.
 */

#include <linux/init.h>
#include <linux/spinlock.h>
#include <linux/skbuff.h>
#include <linux/net.h>

#include <linux/netfilter/xt_statistic.h>
#include <linux/netfilter/x_tables.h>

struct xt_statistic_priv {
	uint32_t count;
};

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Patrick McHardy <kaber@trash.net>");
MODULE_DESCRIPTION("Xtables: statistics-based matching (\"Nth\", random)");
MODULE_ALIAS("ipt_statistic");
MODULE_ALIAS("ip6t_statistic");

static DEFINE_SPINLOCK(nth_lock);

static bool
match(const struct sk_buff *skb, struct xt_action_param *par)
{
	const struct xt_statistic_info *info = (void *)par->matchinfo;
	bool ret = info->flags & XT_STATISTIC_INVERT;

	switch (info->mode) {
	case XT_STATISTIC_MODE_RANDOM:
		if ((net_random() & 0x7FFFFFFF) < info->u.random.probability)
			ret = !ret;
		break;
	case XT_STATISTIC_MODE_NTH:
		spin_lock_bh(&nth_lock);
		if (info->master->count++ == info->u.nth.every) {
			info->master->count = 0;
			ret = !ret;
		}
		spin_unlock_bh(&nth_lock);
		break;
	}

	return ret;
}

static bool checkentry(const struct xt_mtchk_param *par)
{
	struct xt_statistic_info *info = par->matchinfo;

	if (info->mode > XT_STATISTIC_MODE_MAX ||
	    info->flags & ~XT_STATISTIC_MASK)
		return 0;

	info->master = kzalloc(sizeof(*info->master), GFP_KERNEL);
	if (info->master == NULL) {
		printk(KERN_ERR KBUILD_MODNAME ": Out of memory\n");
		return 0;
	}
	info->master->count = info->u.nth.count;

	return 1;
}

static void statistic_mt_destroy(const struct xt_mtdtor_param *par)
{
	const struct xt_statistic_info *info = par->matchinfo;

	kfree(info->master);
}

static struct xt_match xt_statistic_match __read_mostly = {
	.name		= "statistic",
	.revision   = 0,
	.family		= NFPROTO_UNSPEC,
	.match		= match,
	.checkentry	= checkentry,
	.destroy	= statistic_mt_destroy,
	.matchsize	= sizeof(struct xt_statistic_info),
	.me		= THIS_MODULE,
};

static int __init xt_statistic_init(void)
{
	return xt_register_match(&xt_statistic_match);
}

static void __exit xt_statistic_fini(void)
{
	xt_unregister_match(&xt_statistic_match);
}

module_init(xt_statistic_init);
module_exit(xt_statistic_fini);
