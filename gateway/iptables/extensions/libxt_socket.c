/*
 * Shared library add-on to iptables to add early socket matching support.
 *
 * Copyright (C) 2007 BalaBit IT Ltd.
 */
#include <stdio.h>
#include <getopt.h>
#include <xtables.h>

static void socket_mt_help(void)
{
	printf("socket v%s has no options\n\n", XTABLES_VERSION);
}

static int socket_mt_parse(int c, char **argv, int invert, unsigned int *flags,
			const void *entry, struct xt_entry_match **match)
{
	return 0;
}

static void socket_mt_check(unsigned int flags)
{
}

static struct xtables_match socket_mt_reg = {
	.name	       = "socket",
	.version       = XTABLES_VERSION,
	.family	       = NFPROTO_IPV4,
	.size	       = XT_ALIGN(0),
	.userspacesize = XT_ALIGN(0),
	.parse	       = socket_mt_parse,
	.final_check   = socket_mt_check,
	.help	       = socket_mt_help,
};

void _init(void)
{
	xtables_register_match(&socket_mt_reg);
}
