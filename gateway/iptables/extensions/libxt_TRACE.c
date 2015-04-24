/* Shared library add-on to iptables to add TRACE target support. */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <getopt.h>

#include <xtables.h>
#include <linux/netfilter/x_tables.h>

static void TRACE_help(void)
{
	printf("TRACE target takes no options\n");
}

static int TRACE_parse(int c, char **argv, int invert, unsigned int *flags,
                       const void *entry, struct xt_entry_target **target)
{
	return 0;
}

static struct xtables_target trace_target = {
	.family		= AF_UNSPEC,
	.name		= "TRACE",
	.version	= XTABLES_VERSION,
	.size		= XT_ALIGN(0),
	.userspacesize	= XT_ALIGN(0),
	.help		= TRACE_help,
	.parse		= TRACE_parse,
};

void _init(void)
{
	xtables_register_target(&trace_target);
}
