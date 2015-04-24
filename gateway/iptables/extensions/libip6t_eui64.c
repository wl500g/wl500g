/* Shared library add-on to ip6tables to add EUI64 address checking support. */
#include <stdio.h>
#include <netdb.h>
#include <string.h>
#include <stdlib.h>
#include <getopt.h>
#if defined(__GLIBC__) && __GLIBC__ == 2
#include <net/ethernet.h>
#else
#include <linux/if_ether.h>
#endif
#include <xtables.h>

static void eui64_help(void)
{
	printf(
"eui64 match options:\n"
" This module hasn't got any option\n"
" This module checks for EUI64 IPv6 addresses\n");
}

static int eui64_parse(int c, char **argv, int invert, unsigned int *flags,
                       const void *entry, struct xt_entry_match **match)
{
	return 0;
}

static struct xtables_match eui64_mt6_reg = {
	.name 		= "eui64",
	.version	= XTABLES_VERSION,
	.family		= NFPROTO_IPV6,
	.size		= XT_ALIGN(sizeof(int)),
	.userspacesize	= XT_ALIGN(sizeof(int)),
	.help		= eui64_help,
	.parse		= eui64_parse,
};

void _init(void)
{
	xtables_register_match(&eui64_mt6_reg);
}
