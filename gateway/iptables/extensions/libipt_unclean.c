/* Shared library add-on to iptables for unclean. */
#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <xtables.h>

static void unclean_help(void)
{
	printf("unclean match takes no options\n");
}

static int unclean_parse(int c, char **argv, int invert, unsigned int *flags,
                         const void *entry, struct xt_entry_match **match)
{
	return 0;
}

static struct xtables_match unclean_mt_reg = {
	.name		= "unclean",
	.version	= XTABLES_VERSION,
	.family		= NFPROTO_IPV4,
	.size		= XT_ALIGN(0),
	.userspacesize	= XT_ALIGN(0),
	.help		= unclean_help,
	.parse		= unclean_parse,
};

void _init(void)
{
	xtables_register_match(&unclean_mt_reg);
}
