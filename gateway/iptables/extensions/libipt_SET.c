/* Copyright (C) 2000-2002 Joakim Axelsson <gozem@linux.nu>
 *                         Patrick Schaaf <bof@bof.de>
 *                         Martin Josefsson <gandalf@wlug.westbo.se>
 * Copyright (C) 2003-2004 Jozsef Kadlecsik <kadlec@blackhole.kfki.hu>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.  
 */

/* Shared library add-on to iptables to add IP set mangling target. */
#include <stdio.h>
#include <netdb.h>
#include <string.h>
#include <stdlib.h>
#include <getopt.h>
#include <ctype.h>

#include <xtables.h>
#include <linux/netfilter_ipv4/ip_set.h>
#include <linux/netfilter_ipv4/ipt_set.h>
#include "libipt_set.h"

static void SET_help(void)
{
	printf("SET target options:\n"
	       " --add-set name flags\n"
	       " --del-set name flags\n"
	       "		add/del src/dst IP/port from/to named sets,\n"
	       "		where flags are the comma separated list of\n"
	       "		'src' and 'dst'.\n");
}

static const struct option SET_opts[] = {
	{"add-set",   1, NULL, '1'},
	{"del-set",   1, NULL, '2'},
	{ }
};

static void SET_init(struct xt_entry_target *target)
{
	struct ipt_set_info_target *info =
	    (struct ipt_set_info_target *) target->data;

	memset(info, 0, sizeof(struct ipt_set_info_target));
	info->add_set.index =
	info->del_set.index = IP_SET_INVALID_ID;

}

static void
parse_target(char **argv, int invert, unsigned int *flags,
             struct ipt_set_info *info, const char *what)
{
	if (info->flags[0])
		xtables_error(PARAMETER_PROBLEM,
			   "--%s can be specified only once", what);

	if (xtables_check_inverse(optarg, &invert, NULL, 0))
		xtables_error(PARAMETER_PROBLEM,
			   "Unexpected `!' after --%s", what);

	if (!argv[optind]
	    || argv[optind][0] == '-' || argv[optind][0] == '!')
		xtables_error(PARAMETER_PROBLEM,
			   "--%s requires two args.", what);

	if (strlen(argv[optind-1]) > IP_SET_MAXNAMELEN - 1)
		xtables_error(PARAMETER_PROBLEM,
			   "setname `%s' too long, max %d characters.",
			   argv[optind-1], IP_SET_MAXNAMELEN - 1);

	get_set_byname(argv[optind - 1], info);
	parse_bindings(argv[optind], info);
	optind++;
	
	*flags = 1;
}

static int SET_parse(int c, char **argv, int invert, unsigned int *flags,
                     const void *entry, struct xt_entry_target **target)
{
	struct ipt_set_info_target *myinfo =
	    (struct ipt_set_info_target *) (*target)->data;

	switch (c) {
	case '1':		/* --add-set <set> <flags> */
		parse_target(argv, invert, flags,
			     &myinfo->add_set, "add-set");
		break;
	case '2':		/* --del-set <set>[:<flags>] <flags> */
		parse_target(argv, invert, flags,
			     &myinfo->del_set, "del-set");
		break;

	default:
		return 0;
	}
	return 1;
}

static void SET_check(unsigned int flags)
{
	if (!flags)
		xtables_error(PARAMETER_PROBLEM,
			   "You must specify either `--add-set' or `--del-set'");
}

static void
print_target(const char *prefix, const struct ipt_set_info *info)
{
	int i;
	char setname[IP_SET_MAXNAMELEN];

	if (info->index == IP_SET_INVALID_ID)
		return;
	get_set_byid(setname, info->index);
	printf("%s %s", prefix, setname);
	for (i = 0; i < IP_SET_MAX_BINDINGS; i++) {
		if (!info->flags[i])
			break;		
		printf("%s%s",
		       i == 0 ? " " : ",",
		       info->flags[i] & IPSET_SRC ? "src" : "dst");
	}
	printf(" ");
}

static void SET_print(const void *ip, const struct xt_entry_target *target,
                      int numeric)
{
	struct ipt_set_info_target *info =
	    (struct ipt_set_info_target *) target->data;

	print_target("add-set", &info->add_set);
	print_target("del-set", &info->del_set);
}

static void SET_save(const void *ip, const struct xt_entry_target *target)
{
	struct ipt_set_info_target *info =
	    (struct ipt_set_info_target *) target->data;

	print_target("--add-set", &info->add_set);
	print_target("--del-set", &info->del_set);
}

static struct xtables_target set_tg_reg = {
	.name		= "SET",
	.version	= XTABLES_VERSION,
	.family		= NFPROTO_IPV4,
	.size		= XT_ALIGN(sizeof(struct ipt_set_info_target)),
	.userspacesize	= XT_ALIGN(sizeof(struct ipt_set_info_target)),
	.help		= SET_help,
	.init		= SET_init,
	.parse		= SET_parse,
	.final_check	= SET_check,
	.print		= SET_print,
	.save		= SET_save,
	.extra_opts	= SET_opts,
};

void _init(void)
{
	xtables_register_target(&set_tg_reg);
}
