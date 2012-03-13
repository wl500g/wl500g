/* Shared library add-on to iptables for ECN matching
 *
 * (C) 2002 by Harald Welte <laforge@gnumonks.org>
 *
 * This program is distributed under the terms of GNU GPL v2, 1991
 *
 * libipt_ecn.c borrowed heavily from libipt_dscp.c
 *
 */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <getopt.h>

#include <xtables.h>
#include <linux/netfilter/xt_ecn.h>

static void ecn_help(void)
{
	printf(
"ECN match options\n"
"[!] --ecn-tcp-cwr 		Match CWR bit of TCP header\n"
"[!] --ecn-tcp-ece		Match ECE bit of TCP header\n"
"[!] --ecn-ip-ect [0..3]	Match ECN codepoint in IPv4/IPv6 header\n");
}

static const struct option ecn_opts[] = {
	{ .name = "ecn-tcp-cwr", .has_arg = 0, .val = 'F' },
	{ .name = "ecn-tcp-ece", .has_arg = 0, .val = 'G' },
	{ .name = "ecn-ip-ect",  .has_arg = 1, .val = 'H' },
	{ .name = NULL }
};

static int ecn_parse(int c, char **argv, int invert, unsigned int *flags,
                     const void *entry, struct xt_entry_match **match)
{
	unsigned int result;
	struct xt_ecn_info *einfo
		= (struct xt_ecn_info *)(*match)->data;

	switch (c) {
	case 'F':
		if (*flags & XT_ECN_OP_MATCH_CWR)
			xtables_error(PARAMETER_PROBLEM,
			           "ECN match: can only use parameter ONCE!");
		xtables_check_inverse(optarg, &invert, &optind, 0);
		einfo->operation |= XT_ECN_OP_MATCH_CWR;
		if (invert)
			einfo->invert |= XT_ECN_OP_MATCH_CWR;
		*flags |= XT_ECN_OP_MATCH_CWR;
		break;

	case 'G':
		if (*flags & XT_ECN_OP_MATCH_ECE)
			xtables_error(PARAMETER_PROBLEM,
				   "ECN match: can only use parameter ONCE!");
		xtables_check_inverse(optarg, &invert, &optind, 0);
		einfo->operation |= XT_ECN_OP_MATCH_ECE;
		if (invert)
			einfo->invert |= XT_ECN_OP_MATCH_ECE;
		*flags |= XT_ECN_OP_MATCH_ECE;
		break;

	case 'H':
		if (*flags & XT_ECN_OP_MATCH_IP)
			xtables_error(PARAMETER_PROBLEM,
				   "ECN match: can only use parameter ONCE!");
		xtables_check_inverse(optarg, &invert, &optind, 0);
		if (invert)
			einfo->invert |= XT_ECN_OP_MATCH_IP;
		*flags |= XT_ECN_OP_MATCH_IP;
		einfo->operation |= XT_ECN_OP_MATCH_IP;
		if (!xtables_strtoui(optarg, NULL, &result, 0, 3))
			xtables_error(PARAMETER_PROBLEM,
				   "ECN match: Value out of range");
		einfo->ip_ect = result;
		break;
	default:
		return 0;
	}

	return 1;
}

static void ecn_check(unsigned int flags)
{
	if (!flags)
		xtables_error(PARAMETER_PROBLEM,
		           "ECN match: some option required");
}

static void ecn_print(const void *ip, const struct xt_entry_match *match,
                      int numeric)
{
	const struct xt_ecn_info *einfo =
		(const struct xt_ecn_info *)match->data;

	printf(" ECN match ");

	if (einfo->operation & XT_ECN_OP_MATCH_ECE) {
		printf(" %sECE",
		       (einfo->invert & XT_ECN_OP_MATCH_ECE) ? "!" : "");
	}

	if (einfo->operation & XT_ECN_OP_MATCH_CWR) {
		printf(" %sCWR",
		       (einfo->invert & XT_ECN_OP_MATCH_CWR) ? "!" : "");
	}

	if (einfo->operation & XT_ECN_OP_MATCH_IP) {
		printf(" %sECT=%d",
		       (einfo->invert & XT_ECN_OP_MATCH_IP) ? "!" : "",
		       einfo->ip_ect);
	}
}

static void ecn_save(const void *ip, const struct xt_entry_match *match)
{
	const struct xt_ecn_info *einfo =
		(const struct xt_ecn_info *)match->data;
	
	if (einfo->operation & XT_ECN_OP_MATCH_ECE) {
		if (einfo->invert & XT_ECN_OP_MATCH_ECE)
			printf("! ");
		printf("--ecn-tcp-ece ");
	}

	if (einfo->operation & XT_ECN_OP_MATCH_CWR) {
		if (einfo->invert & XT_ECN_OP_MATCH_CWR)
			printf("! ");
		printf("--ecn-tcp-cwr ");
	}

	if (einfo->operation & XT_ECN_OP_MATCH_IP) {
		if (einfo->invert & XT_ECN_OP_MATCH_IP)
			printf("! ");
		printf("--ecn-ip-ect %d", einfo->ip_ect);
	}
}

static struct xtables_match ecn_mt_reg = {
    .name          = "ecn",
    .version       = XTABLES_VERSION,
    .family        = NFPROTO_UNSPEC,
    .size          = XT_ALIGN(sizeof(struct xt_ecn_info)),
    .userspacesize = XT_ALIGN(sizeof(struct xt_ecn_info)),
    .help          = ecn_help,
    .parse         = ecn_parse,
    .final_check   = ecn_check,
    .print         = ecn_print,
    .save          = ecn_save,
    .extra_opts    = ecn_opts,
};

void _init(void)
{
	xtables_register_match(&ecn_mt_reg);
}
