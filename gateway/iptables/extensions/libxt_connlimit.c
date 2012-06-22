/* Shared library add-on to iptables to add connection limit support. */
#include <stdio.h>
#include <netdb.h>
#include <string.h>
#include <stdlib.h>
#include <stddef.h>
#include <getopt.h>
#include <xtables.h>
#include <linux/netfilter/xt_connlimit.h>

enum {
	FL_LIMIT = 1 << 0,
	FL_MASK  = 1 << 1,
	FL_ADDR  = 1 << 2,
};

static void connlimit_help(void)
{
	printf(
"connlimit match options:\n"
"  --connlimit-upto n     match if the number of existing connections is 0..n\n"
"  --connlimit-above n    match if the number of existing connections is >n\n"
"  --connlimit-mask n     group hosts using prefix length (default: max len)\n"
"  --connlimit-saddr      select source address for grouping\n"
"  --connlimit-daddr      select destination addresses for grouping\n");
}

static const struct option connlimit_opts[] = {
	{"connlimit-upto",  1, NULL, 'U'},
	{"connlimit-above", 1, NULL, 'A'},
	{"connlimit-mask",  1, NULL, 'M'},
	{"connlimit-saddr", 0, NULL, 's'},
	{"connlimit-daddr", 0, NULL, 'd'},
	{ .name = NULL }
};

static void connlimit_init(struct xt_entry_match *match)
{
	struct xt_connlimit_info *info = (void *)match->data;

	/* This will also initialize the v4 mask correctly */
	memset(info->v6_mask, 0xFF, sizeof(info->v6_mask));
}

static void prefix_to_netmask(u_int32_t *mask, unsigned int prefix_len)
{
	if (prefix_len == 0) {
		mask[0] = mask[1] = mask[2] = mask[3] = 0;
	} else if (prefix_len <= 32) {
		mask[0] <<= 32 - prefix_len;
		mask[1] = mask[2] = mask[3] = 0;
	} else if (prefix_len <= 64) {
		mask[1] <<= 32 - (prefix_len - 32);
		mask[2] = mask[3] = 0;
	} else if (prefix_len <= 96) {
		mask[2] <<= 32 - (prefix_len - 64);
		mask[3] = 0;
	} else if (prefix_len <= 128) {
		mask[3] <<= 32 - (prefix_len - 96);
	}
	mask[0] = htonl(mask[0]);
	mask[1] = htonl(mask[1]);
	mask[2] = htonl(mask[2]);
	mask[3] = htonl(mask[3]);
}

static int
connlimit_parse(int c, char **argv, int invert, unsigned int *flags,
                struct xt_entry_match **match, unsigned int family)
{
	struct xt_connlimit_info *info = (void *)(*match)->data;
	const unsigned int revision = (*match)->u.user.revision;
	char *err;
	int i;

	switch (c) {
	case 'A': /* --connlimit-above */
		xtables_param_act(XTF_ONLY_ONCE, "connlimit",
			"--connlimit-{upto,above}", *flags & FL_LIMIT);
		*flags |= FL_LIMIT;
		if (invert)
			info->flags |= XT_CONNLIMIT_INVERT;
		info->limit = strtoul(argv[optind-1], NULL, 0);
		return 1;
	case 'U': /* --connlimit-upto */
		xtables_param_act(XTF_ONLY_ONCE, "connlimit",
			"--connlimit-{upto,above}", *flags & FL_LIMIT);
		*flags |= FL_LIMIT;
		if (!invert)
			info->flags |= XT_CONNLIMIT_INVERT;
		info->limit = strtoul(argv[optind-1], NULL, 0);
		return 1;
	case 'M': /* --connlimit-mask */
		xtables_param_act(XTF_NO_INVERT, "connlimit",
			"--connlimit-mask", invert);
		xtables_param_act(XTF_ONLY_ONCE, "connlimit",
			"--connlimit-mask", *flags & FL_MASK);
		*flags |= FL_MASK;
		i = strtoul(argv[optind-1], &err, 0);
		if (family == NFPROTO_IPV6) {
			if (i > 128 || *err != '\0')
				xtables_error(PARAMETER_PROBLEM,
					"--connlimit-mask must be between "
					"0 and 128");
			prefix_to_netmask(info->v6_mask, i);
		} else {
			if (i > 32 || *err != '\0')
				xtables_error(PARAMETER_PROBLEM,
					"--connlimit-mask must be between "
					"0 and 32");
			if (i == 0)
				info->v4_mask = 0;
			else
				info->v4_mask = htonl(0xFFFFFFFF << (32 - i));
		}
		return 1;
	case 's': /* --connlimit-saddr */
		info->flags &= ~XT_CONNLIMIT_DADDR;
		return 1;
	case 'd': /* --connlimit-daddr */
		if (revision < 1)
			xtables_error(PARAMETER_PROBLEM,
				"xt_connlimit.0 does not support "
				"--connlimit-daddr");
		info->flags |= XT_CONNLIMIT_DADDR;
		return 1;
	}
	return 0;
}

static int connlimit_parse4(int c, char **argv, int invert,
                            unsigned int *flags, const void *entry,
                            struct xt_entry_match **match)
{
	return connlimit_parse(c, argv, invert, flags, match, NFPROTO_IPV4);
}

static int connlimit_parse6(int c, char **argv, int invert,
                            unsigned int *flags, const void *entry,
                            struct xt_entry_match **match)
{
	return connlimit_parse(c, argv, invert, flags, match, NFPROTO_IPV6);
}

static void connlimit_check(unsigned int flags)
{
	if (!(flags & 0x1))
		xtables_error(PARAMETER_PROBLEM,
			"You must specify \"--connlimit-above\"");
}

static unsigned int count_bits4(u_int32_t mask)
{
	unsigned int bits = 0;

	for (mask = ~ntohl(mask); mask != 0; mask >>= 1)
		++bits;

	return 32 - bits;
}

static unsigned int count_bits6(const u_int32_t *mask)
{
	unsigned int bits = 0, i;
	u_int32_t tmp[4];

	for (i = 0; i < 4; ++i)
		for (tmp[i] = ~ntohl(mask[i]); tmp[i] != 0; tmp[i] >>= 1)
			++bits;
	return 128 - bits;
}

static void connlimit_print4(const void *ip,
                             const struct xt_entry_match *match, int numeric)
{
	const struct xt_connlimit_info *info = (const void *)match->data;

	printf("#conn %s/%u %s %u ",
	       (info->flags & XT_CONNLIMIT_DADDR) ? "dst" : "src",
	       count_bits4(info->v4_mask),
	       (info->flags & XT_CONNLIMIT_INVERT) ? "<=" : ">", info->limit);
}

static void connlimit_print6(const void *ip,
                             const struct xt_entry_match *match, int numeric)
{
	const struct xt_connlimit_info *info = (const void *)match->data;

	printf("#conn %s/%u %s %u ",
	       (info->flags & XT_CONNLIMIT_DADDR) ? "dst" : "src",
	       count_bits6(info->v6_mask),
	       (info->flags & XT_CONNLIMIT_INVERT) ? "<=" : ">", info->limit);
}

static void connlimit_save4(const void *ip, const struct xt_entry_match *match)
{
	const struct xt_connlimit_info *info = (const void *)match->data;
	const int revision = match->u.user.revision;

	if (info->flags & XT_CONNLIMIT_INVERT)
		printf("--connlimit-upto %u ", info->limit);
	else
		printf("--connlimit-above %u ", info->limit);
	printf("--connlimit-mask %u ", count_bits4(info->v4_mask));
	if (revision >= 1) {
		if (info->flags & XT_CONNLIMIT_DADDR)
			printf("--connlimit-daddr ");
		else
			printf("--connlimit-saddr ");
	}
}

static void connlimit_save6(const void *ip, const struct xt_entry_match *match)
{
	const struct xt_connlimit_info *info = (const void *)match->data;
	const int revision = match->u.user.revision;

	if (info->flags & XT_CONNLIMIT_INVERT)
		printf("--connlimit-upto %u ", info->limit);
	else
		printf("--connlimit-above %u ", info->limit);
	printf("--connlimit-mask %u ", count_bits6(info->v6_mask));
	if (revision >= 1) {
		if (info->flags & XT_CONNLIMIT_DADDR)
			printf("--connlimit-daddr ");
		else
			printf("--connlimit-saddr ");
	}
}

static struct xtables_match connlimit_match_v0 = {
	.name          = "connlimit",
	.revision      = 0,
	.family        = NFPROTO_IPV4,
	.version       = XTABLES_VERSION,
	.size          = XT_ALIGN(sizeof(struct xt_connlimit_info)),
	.userspacesize = offsetof(struct xt_connlimit_info, data),
	.help          = connlimit_help,
	.init          = connlimit_init,
	.parse         = connlimit_parse4,
	.final_check   = connlimit_check,
	.print         = connlimit_print4,
	.save          = connlimit_save4,
	.extra_opts    = connlimit_opts,
};

static struct xtables_match connlimit_match6_v0 = {
	.name          = "connlimit",
	.revision      = 0,
	.family        = NFPROTO_IPV6,
	.version       = XTABLES_VERSION,
	.size          = XT_ALIGN(sizeof(struct xt_connlimit_info)),
	.userspacesize = offsetof(struct xt_connlimit_info, data),
	.help          = connlimit_help,
	.init          = connlimit_init,
	.parse         = connlimit_parse6,
	.final_check   = connlimit_check,
	.print         = connlimit_print6,
	.save          = connlimit_save6,
	.extra_opts    = connlimit_opts,
};

static struct xtables_match connlimit_match = {
	.name          = "connlimit",
	.revision      = 1,
	.family        = NFPROTO_IPV4,
	.version       = XTABLES_VERSION,
	.size          = XT_ALIGN(sizeof(struct xt_connlimit_info)),
	.userspacesize = offsetof(struct xt_connlimit_info, data),
	.help          = connlimit_help,
	.init          = connlimit_init,
	.parse         = connlimit_parse4,
	.final_check   = connlimit_check,
	.print         = connlimit_print4,
	.save          = connlimit_save4,
	.extra_opts    = connlimit_opts,
};

static struct xtables_match connlimit_match6 = {
	.name          = "connlimit",
	.revision      = 1,
	.family        = NFPROTO_IPV6,
	.version       = XTABLES_VERSION,
	.size          = XT_ALIGN(sizeof(struct xt_connlimit_info)),
	.userspacesize = offsetof(struct xt_connlimit_info, data),
	.help          = connlimit_help,
	.init          = connlimit_init,
	.parse         = connlimit_parse6,
	.final_check   = connlimit_check,
	.print         = connlimit_print6,
	.save          = connlimit_save6,
	.extra_opts    = connlimit_opts,
};

void _init(void)
{
	xtables_register_match(&connlimit_match_v0);
	xtables_register_match(&connlimit_match6_v0);
	xtables_register_match(&connlimit_match);
	xtables_register_match(&connlimit_match6);
}
