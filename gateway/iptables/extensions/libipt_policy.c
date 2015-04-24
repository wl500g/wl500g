/* Shared library add-on to iptables to add policy support. */
#include <stdio.h>
#include <netdb.h>
#include <string.h>
#include <stdlib.h>
#include <syslog.h>
#include <getopt.h>
#include <netdb.h>
#include <errno.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <xtables.h>

#include <linux/netfilter_ipv4/ipt_policy.h>

/*
 * HACK: global pointer to current matchinfo for making
 * final checks and adjustments in final_check.
 */
static struct ipt_policy_info *policy_info;

static void policy_help(void)
{
	printf(
"policy match options:\n"
"  --dir in|out			match policy applied during decapsulation/\n"
"				policy to be applied during encapsulation\n"
"  --pol none|ipsec		match policy\n"
"  --strict 			match entire policy instead of single element\n"
"				at any position\n"
"[!] --reqid reqid		match reqid\n"
"[!] --spi spi			match SPI\n"
"[!] --proto proto		match protocol (ah/esp/ipcomp)\n"
"[!] --mode mode 		match mode (transport/tunnel)\n"
"[!] --tunnel-src addr/mask	match tunnel source\n"
"[!] --tunnel-dst addr/mask	match tunnel destination\n"
"  --next 			begin next element in policy\n");
}

static const struct option policy_opts[] =
{
	{
		.name		= "dir",
		.has_arg	= 1,
		.val		= '1',
	},
	{
		.name		= "pol",
		.has_arg	= 1,
		.val		= '2',
	},
	{
		.name		= "strict",
		.val		= '3'
	},
	{
		.name		= "reqid",
		.has_arg	= 1,
		.val		= '4',
	},
	{
		.name		= "spi",
		.has_arg	= 1,
		.val		= '5'
	},
	{
		.name		= "tunnel-src",
		.has_arg	= 1,
		.val		= '6'
	},
	{
		.name		= "tunnel-dst",
		.has_arg	= 1,
		.val		= '7'
	},
	{
		.name		= "proto",
		.has_arg	= 1,
		.val		= '8'
	},
	{
		.name		= "mode",
		.has_arg	= 1,
		.val		= '9'
	},
	{
		.name		= "next",
		.val		= 'a'
	},
	{ .name = NULL }
};

static int parse_direction(char *s)
{
	if (strcmp(s, "in") == 0)
		return IPT_POLICY_MATCH_IN;
	if (strcmp(s, "out") == 0)
		return IPT_POLICY_MATCH_OUT;
	xtables_error(PARAMETER_PROBLEM, "policy_match: invalid dir \"%s\"", s);
}

static int parse_policy(char *s)
{
	if (strcmp(s, "none") == 0)
		return IPT_POLICY_MATCH_NONE;
	if (strcmp(s, "ipsec") == 0)
		return 0;
	xtables_error(PARAMETER_PROBLEM, "policy match: invalid policy \"%s\"", s);
}

static int parse_mode(char *s)
{
	if (strcmp(s, "transport") == 0)
		return IPT_POLICY_MODE_TRANSPORT;
	if (strcmp(s, "tunnel") == 0)
		return IPT_POLICY_MODE_TUNNEL;
	xtables_error(PARAMETER_PROBLEM, "policy match: invalid mode \"%s\"", s);
}

static int policy_parse(int c, char **argv, int invert, unsigned int *flags,
                        const void *entry, struct xt_entry_match **match)
{
	struct ipt_policy_info *info = (void *)(*match)->data;
	struct ipt_policy_elem *e = &info->pol[info->len];
	struct in_addr *addr = NULL, mask;
	unsigned int naddr = 0, num;
	int mode;

	xtables_check_inverse(optarg, &invert, &optind, 0);

	switch (c) {
	case '1':
		if (info->flags & (IPT_POLICY_MATCH_IN|IPT_POLICY_MATCH_OUT))
			xtables_error(PARAMETER_PROBLEM,
			           "policy match: double --dir option");
		if (invert)
			xtables_error(PARAMETER_PROBLEM,
			           "policy match: can't invert --dir option");

		info->flags |= parse_direction(argv[optind-1]);
		break;
	case '2':
		if (invert)
			xtables_error(PARAMETER_PROBLEM,
			           "policy match: can't invert --policy option");

		info->flags |= parse_policy(argv[optind-1]);
		break;
	case '3':
		if (info->flags & IPT_POLICY_MATCH_STRICT)
			xtables_error(PARAMETER_PROBLEM,
			           "policy match: double --strict option");

		if (invert)
			xtables_error(PARAMETER_PROBLEM,
			           "policy match: can't invert --strict option");

		info->flags |= IPT_POLICY_MATCH_STRICT;
		break;
	case '4':
		if (e->match.reqid)
			xtables_error(PARAMETER_PROBLEM,
			           "policy match: double --reqid option");

		e->match.reqid = 1;
		e->invert.reqid = invert;
		if (!xtables_strtoui(optarg, NULL, &num, 0, UINT32_MAX))
			xtables_param_act(XTF_BAD_VALUE, "policy", "--spi", optarg);
		e->reqid = num;
		break;
	case '5':
		if (e->match.spi)
			xtables_error(PARAMETER_PROBLEM,
			           "policy match: double --spi option");

		e->match.spi = 1;
		e->invert.spi = invert;
		if (!xtables_strtoui(optarg, NULL, &num, 0, UINT32_MAX))
			xtables_param_act(XTF_BAD_VALUE, "policy", "--spi", optarg);
		e->spi = num;
		break;
	case '6':
		if (e->match.saddr)
			xtables_error(PARAMETER_PROBLEM,
			           "policy match: double --tunnel-src option");

		xtables_ipparse_any(argv[optind-1], &addr, &mask, &naddr);
		if (naddr > 1)
			xtables_error(PARAMETER_PROBLEM,
			           "policy match: name resolves to multiple IPs");

		e->match.saddr = 1;
		e->invert.saddr = invert;
		e->saddr.a4 = addr[0];
		e->smask.a4 = mask;
                break;
	case '7':
		if (e->match.daddr)
			xtables_error(PARAMETER_PROBLEM,
			           "policy match: double --tunnel-dst option");

		xtables_ipparse_any(argv[optind-1], &addr, &mask, &naddr);
		if (naddr > 1)
			xtables_error(PARAMETER_PROBLEM,
			           "policy match: name resolves to multiple IPs");

		e->match.daddr = 1;
		e->invert.daddr = invert;
		e->daddr.a4 = addr[0];
		e->dmask.a4 = mask;
		break;
	case '8':
		if (e->match.proto)
			xtables_error(PARAMETER_PROBLEM,
			           "policy match: double --proto option");

		e->proto = xtables_parse_protocol(argv[optind-1]);
		if (e->proto != IPPROTO_AH && e->proto != IPPROTO_ESP &&
		    e->proto != IPPROTO_COMP)
			xtables_error(PARAMETER_PROBLEM,
			           "policy match: protocol must ah/esp/ipcomp");
		e->match.proto = 1;
		e->invert.proto = invert;
		break;
	case '9':
		if (e->match.mode)
			xtables_error(PARAMETER_PROBLEM,
			           "policy match: double --mode option");

		mode = parse_mode(argv[optind-1]);
		e->match.mode = 1;
		e->invert.mode = invert;
		e->mode = mode;
		break;
	case 'a':
		if (invert)
			xtables_error(PARAMETER_PROBLEM,
			           "policy match: can't invert --next option");

		if (++info->len == IPT_POLICY_MAX_ELEM)
			xtables_error(PARAMETER_PROBLEM,
			           "policy match: maximum policy depth reached");
		break;
	default:
		return 0;
	}

	policy_info = info;
	return 1;
}

static void policy_check(unsigned int flags)
{
	struct ipt_policy_info *info = policy_info;
	struct ipt_policy_elem *e;
	int i;

	if (info == NULL)
		xtables_error(PARAMETER_PROBLEM,
		           "policy match: no parameters given");

	if (!(info->flags & (IPT_POLICY_MATCH_IN|IPT_POLICY_MATCH_OUT)))
		xtables_error(PARAMETER_PROBLEM,
		           "policy match: neither --in nor --out specified");

	if (info->flags & IPT_POLICY_MATCH_NONE) {
		if (info->flags & IPT_POLICY_MATCH_STRICT)
			xtables_error(PARAMETER_PROBLEM,
			           "policy match: policy none but --strict given");

		if (info->len != 0)
			xtables_error(PARAMETER_PROBLEM,
			           "policy match: policy none but policy given");
	} else
		info->len++;	/* increase len by 1, no --next after last element */

	if (!(info->flags & IPT_POLICY_MATCH_STRICT) && info->len > 1)
		xtables_error(PARAMETER_PROBLEM,
		           "policy match: multiple elements but no --strict");

	for (i = 0; i < info->len; i++) {
		e = &info->pol[i];

		if (info->flags & IPT_POLICY_MATCH_STRICT &&
		    !(e->match.reqid || e->match.spi || e->match.saddr ||
		      e->match.daddr || e->match.proto || e->match.mode))
			xtables_error(PARAMETER_PROBLEM,
			           "policy match: empty policy element");

		if ((e->match.saddr || e->match.daddr)
		    && ((e->mode == IPT_POLICY_MODE_TUNNEL && e->invert.mode) ||
		        (e->mode == IPT_POLICY_MODE_TRANSPORT && !e->invert.mode)))
			xtables_error(PARAMETER_PROBLEM,
			           "policy match: --tunnel-src/--tunnel-dst "
			           "is only valid in tunnel mode");
	}
}

static void print_mode(char *prefix, u_int8_t mode, int numeric)
{
	printf("%smode ", prefix);

	switch (mode) {
	case IPT_POLICY_MODE_TRANSPORT:
		printf("transport ");
		break;
	case IPT_POLICY_MODE_TUNNEL:
		printf("tunnel ");
		break;
	default:
		printf("??? ");
		break;
	}
}

static void print_proto(char *prefix, u_int8_t proto, int numeric)
{
	struct protoent *p = NULL;

	printf("%sproto ", prefix);
	if (!numeric)
		p = getprotobynumber(proto);
	if (p != NULL)
		printf("%s ", p->p_name);
	else
		printf("%u ", proto);
}

#define PRINT_INVERT(x)		\
do {				\
	if (x)			\
		printf("! ");	\
} while(0)

static void print_entry(char *prefix, const struct ipt_policy_elem *e,
                        int numeric)
{
	if (e->match.reqid) {
		PRINT_INVERT(e->invert.reqid);
		printf("%sreqid %u ", prefix, e->reqid);
	}
	if (e->match.spi) {
		PRINT_INVERT(e->invert.spi);
		printf("%sspi 0x%x ", prefix, e->spi);
	}
	if (e->match.proto) {
		PRINT_INVERT(e->invert.proto);
		print_proto(prefix, e->proto, numeric);
	}
	if (e->match.mode) {
		PRINT_INVERT(e->invert.mode);
		print_mode(prefix, e->mode, numeric);
	}
	if (e->match.daddr) {
		PRINT_INVERT(e->invert.daddr);
		printf("%stunnel-dst %s%s ", prefix,
		       xtables_ipaddr_to_numeric((const void *)&e->daddr),
		       xtables_ipmask_to_numeric((const void *)&e->dmask));
	}
	if (e->match.saddr) {
		PRINT_INVERT(e->invert.saddr);
		printf("%stunnel-src %s%s ", prefix,
		       xtables_ipaddr_to_numeric((const void *)&e->saddr),
		       xtables_ipmask_to_numeric((const void *)&e->smask));
	}
}

static void print_flags(char *prefix, const struct ipt_policy_info *info)
{
	if (info->flags & IPT_POLICY_MATCH_IN)
		printf("%sdir in ", prefix);
	else
		printf("%sdir out ", prefix);

	if (info->flags & IPT_POLICY_MATCH_NONE)
		printf("%spol none ", prefix);
	else
		printf("%spol ipsec ", prefix);

	if (info->flags & IPT_POLICY_MATCH_STRICT)
		printf("%sstrict ", prefix);
}

static void policy_print(const void *ip, const struct xt_entry_match *match,
                         int numeric)
{
	const struct ipt_policy_info *info = (void *)match->data;
	unsigned int i;

	printf("policy match ");
	print_flags("", info);
	for (i = 0; i < info->len; i++) {
		if (info->len > 1)
			printf("[%u] ", i);
		print_entry("", &info->pol[i], numeric);
	}
}

static void policy_save(const void *ip, const struct xt_entry_match *match)
{
	const struct ipt_policy_info *info = (void *)match->data;
	unsigned int i;

	print_flags("--", info);
	for (i = 0; i < info->len; i++) {
		print_entry("--", &info->pol[i], 0);
		if (i + 1 < info->len)
			printf("--next ");
	}
}

static struct xtables_match policy_mt_reg = {
	.name		= "policy",
	.version	= XTABLES_VERSION,
	.family		= NFPROTO_IPV4,
	.size		= XT_ALIGN(sizeof(struct ipt_policy_info)),
	.userspacesize	= XT_ALIGN(sizeof(struct ipt_policy_info)),
	.help		= policy_help,
	.parse		= policy_parse,
	.final_check	= policy_check,
	.print		= policy_print,
	.save		= policy_save,
	.extra_opts	= policy_opts,
};

void _init(void)
{
	xtables_register_match(&policy_mt_reg);
}
