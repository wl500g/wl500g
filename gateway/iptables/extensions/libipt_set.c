/* Copyright (C) 2000-2002 Joakim Axelsson <gozem@linux.nu>
 *                         Patrick Schaaf <bof@bof.de>
 *                         Martin Josefsson <gandalf@wlug.westbo.se>
 * Copyright (C) 2003-2004 Jozsef Kadlecsik <kadlec@blackhole.kfki.hu>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.  
 */

/* Shared library add-on to iptables to add IP set matching. */
#include <stdio.h>
#include <netdb.h>
#include <string.h>
#include <stdlib.h>
#include <getopt.h>
#include <ctype.h>
#include <errno.h>

#include <xtables.h>
#include <linux/netfilter_ipv4/ipt_set.h>
#include "libipt_set.h"

static void set_help(void)
{
	printf("set match options:\n"
	       " [!] --set     name flags\n"
	       "		'name' is the set name from to match,\n" 
	       "		'flags' are the comma separated list of\n"
	       "		'src' and 'dst'.\n");
}

static const struct option set_opts[] = {
	{"set", 1, NULL, '1'},
	{ }
};

static void set_init(struct xt_entry_match *match)
{
	struct ipt_set_info_match *info = 
		(struct ipt_set_info_match *) match->data;
	

	memset(info, 0, sizeof(struct ipt_set_info_match));

}

static int set_parse(int c, char **argv, int invert, unsigned int *flags,
                     const void *entry, struct xt_entry_match **match)
{
	struct ipt_set_info_match *myinfo = 
		(struct ipt_set_info_match *) (*match)->data;
	struct ipt_set_info *info = &myinfo->match_set;

	switch (c) {
	case '1':		/* --set <set> <flag>[,<flag> */
		if (info->flags[0])
			xtables_error(PARAMETER_PROBLEM,
				   "--set can be specified only once");

		xtables_check_inverse(optarg, &invert, &optind, 0);
		if (invert)
			info->flags[0] |= IPSET_MATCH_INV;

		if (!argv[optind]
		    || argv[optind][0] == '-'
		    || argv[optind][0] == '!')
			xtables_error(PARAMETER_PROBLEM,
				   "--set requires two args.");

		if (strlen(argv[optind-1]) > IP_SET_MAXNAMELEN - 1)
			xtables_error(PARAMETER_PROBLEM,
				   "setname `%s' too long, max %d characters.",
				   argv[optind-1], IP_SET_MAXNAMELEN - 1);

		get_set_byname(argv[optind - 1], info);
		parse_bindings(argv[optind], info);
		DEBUGP("parse: set index %u\n", info->index);
		optind++;
		
		*flags = 1;
		break;

	default:
		return 0;
	}

	return 1;
}

static void set_check(unsigned int flags)
{
	if (!flags)
		xtables_error(PARAMETER_PROBLEM,
			   "You must specify `--set' with proper arguments");
	DEBUGP("final check OK\n");
}

static void
print_match(const char *prefix, const struct ipt_set_info *info)
{
	int i;
	char setname[IP_SET_MAXNAMELEN];

	get_set_byid(setname, info->index);
	printf("%s%s %s", 
	       (info->flags[0] & IPSET_MATCH_INV) ? "! " : "",
	       prefix,
	       setname); 
	for (i = 0; i < IP_SET_MAX_BINDINGS; i++) {
		if (!info->flags[i])
			break;		
		printf("%s%s",
		       i == 0 ? " " : ",",
		       info->flags[i] & IPSET_SRC ? "src" : "dst");
	}
	printf(" ");
}

/* Prints out the matchinfo. */
static void set_print(const void *ip, const struct xt_entry_match *match,
                      int numeric)
{
	struct ipt_set_info_match *info = 
		(struct ipt_set_info_match *) match->data;

	print_match("set", &info->match_set);
}

static void set_save(const void *ip, const struct xt_entry_match *match)
{
	struct ipt_set_info_match *info = 
		(struct ipt_set_info_match *) match->data;

	print_match("--set", &info->match_set);
}

static struct xtables_match set_mt_reg = {
	.name		= "set",
	.version	= XTABLES_VERSION,
	.family		= NFPROTO_IPV4,
	.size		= XT_ALIGN(sizeof(struct ipt_set_info_match)),
	.userspacesize	= XT_ALIGN(sizeof(struct ipt_set_info_match)),
	.help		= set_help,
	.init		= set_init,
	.parse		= set_parse,
	.final_check	= set_check,
	.print		= set_print,
	.save		= set_save,
	.extra_opts	= set_opts,
};

void _init(void)
{
	xtables_register_match(&set_mt_reg);
}
