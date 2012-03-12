/*
 *	libxt_owner - iptables addon for xt_owner
 *
 *	Copyright © CC Computer Consultants GmbH, 2007 - 2008
 *	Jan Engelhardt <jengelh@computergmbh.de>
 */
#include <getopt.h>
#include <grp.h>
#include <netdb.h>
#include <pwd.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>

#include <xtables.h>
#include <linux/netfilter/xt_owner.h>
#include <linux/netfilter_ipv4/ipt_owner.h>
#include <linux/netfilter_ipv6/ip6t_owner.h>

/*
 *	Note: "UINT32_MAX - 1" is used in the code because -1 is a reserved
 *	UID/GID value anyway.
 */

enum {
	FLAG_UID_OWNER     = 1 << 0,
	FLAG_GID_OWNER     = 1 << 1,
	FLAG_SOCKET_EXISTS = 1 << 2,
	FLAG_PID_OWNER     = 1 << 3,
	FLAG_SID_OWNER     = 1 << 4,
	FLAG_COMM          = 1 << 5,
};

static void owner_mt_help(void)
{
	printf(
"owner match options:\n"
"[!] --uid-owner userid[-userid]      Match local UID\n"
"[!] --gid-owner groupid[-groupid]    Match local GID\n"
"[!] --socket-exists                  Match if socket exists\n");
}

static const struct option owner_mt_opts[] = {
	{.name = "uid-owner",     .has_arg = true,  .val = 'u'},
	{.name = "gid-owner",     .has_arg = true,  .val = 'g'},
	{.name = "socket-exists", .has_arg = false, .val = 'k'},
	{ .name = NULL }
};

static void owner_parse_range(const char *s, unsigned int *from,
                              unsigned int *to, const char *opt)
{
	char *end;

	/* -1 is reversed, so the max is one less than that. */
	if (!xtables_strtoui(s, &end, from, 0, UINT32_MAX - 1))
		xtables_param_act(XTF_BAD_VALUE, "owner", opt, s);
	*to = *from;
	if (*end == '-' || *end == ':')
		if (!xtables_strtoui(end + 1, &end, to, 0, UINT32_MAX - 1))
			xtables_param_act(XTF_BAD_VALUE, "owner", opt, s);
	if (*end != '\0')
		xtables_param_act(XTF_BAD_VALUE, "owner", opt, s);
}

static int owner_mt_parse(int c, char **argv, int invert, unsigned int *flags,
                          const void *entry, struct xt_entry_match **match)
{
	struct xt_owner_match_info *info = (void *)(*match)->data;
	struct passwd *pwd;
	struct group *grp;
	unsigned int from, to;

	switch (c) {
	case 'u':
		xtables_param_act(XTF_ONLY_ONCE, "owner", "--uid-owner",
		          *flags & FLAG_UID_OWNER);
		if ((pwd = getpwnam(optarg)) != NULL)
			from = to = pwd->pw_uid;
		else
			owner_parse_range(optarg, &from, &to, "--uid-owner");
		if (invert)
			info->invert |= XT_OWNER_UID;
		info->match  |= XT_OWNER_UID;
		info->uid_min = from;
		info->uid_max = to;
		*flags       |= FLAG_UID_OWNER;
		return true;

	case 'g':
		xtables_param_act(XTF_ONLY_ONCE, "owner", "--gid-owner",
		          *flags & FLAG_GID_OWNER);
		if ((grp = getgrnam(optarg)) != NULL)
			from = to = grp->gr_gid;
		else
			owner_parse_range(optarg, &from, &to, "--gid-owner");
		if (invert)
			info->invert |= XT_OWNER_GID;
		info->match  |= XT_OWNER_GID;
		info->gid_min = from;
		info->gid_max = to;
		*flags      |= FLAG_GID_OWNER;
		return true;

	case 'k':
		xtables_param_act(XTF_ONLY_ONCE, "owner", "--socket-exists",
		          *flags & FLAG_SOCKET_EXISTS);
		if (invert)
			info->invert |= XT_OWNER_SOCKET;
		info->match |= XT_OWNER_SOCKET;
		*flags |= FLAG_SOCKET_EXISTS;
		return true;

	}
	return false;
}

static void owner_mt_check(unsigned int flags)
{
	if (flags == 0)
		xtables_error(PARAMETER_PROBLEM, "owner: At least one of "
		           "--uid-owner, --gid-owner or --socket-exists "
		           "is required");
}

static void
owner_mt_print_item(const struct xt_owner_match_info *info, const char *label,
                    u_int8_t flag, bool numeric)
{
	if (!(info->match & flag))
		return;
	if (info->invert & flag)
		printf("! ");
	printf("%s ", label);

	switch (info->match & flag) {
	case XT_OWNER_UID:
		if (info->uid_min != info->uid_max) {
			printf("%u-%u ", (unsigned int)info->uid_min,
			       (unsigned int)info->uid_max);
			break;
		} else if (!numeric) {
			const struct passwd *pwd = getpwuid(info->uid_min);

			if (pwd != NULL && pwd->pw_name != NULL) {
				printf("%s ", pwd->pw_name);
				break;
			}
		}
		printf("%u ", (unsigned int)info->uid_min);
		break;

	case XT_OWNER_GID:
		if (info->gid_min != info->gid_max) {
			printf("%u-%u ", (unsigned int)info->gid_min,
			       (unsigned int)info->gid_max);
			break;
		} else if (!numeric) {
			const struct group *grp = getgrgid(info->gid_min);

			if (grp != NULL && grp->gr_name != NULL) {
				printf("%s ", grp->gr_name);
				break;
			}
		}
		printf("%u ", (unsigned int)info->gid_min);
		break;
	}
}

static void owner_mt_print(const void *ip, const struct xt_entry_match *match,
                           int numeric)
{
	const struct xt_owner_match_info *info = (void *)match->data;

	owner_mt_print_item(info, "owner socket exists", XT_OWNER_SOCKET, numeric);
	owner_mt_print_item(info, "owner UID match",     XT_OWNER_UID,    numeric);
	owner_mt_print_item(info, "owner GID match",     XT_OWNER_GID,    numeric);
}

static void owner_mt_save(const void *ip, const struct xt_entry_match *match)
{
	const struct xt_owner_match_info *info = (void *)match->data;

	owner_mt_print_item(info, "--socket-exists",  XT_OWNER_SOCKET, false);
	owner_mt_print_item(info, "--uid-owner",      XT_OWNER_UID,    false);
	owner_mt_print_item(info, "--gid-owner",      XT_OWNER_GID,    false);
}

static struct xtables_match owner_mt_reg = {
	.version       = XTABLES_VERSION,
	.name          = "owner",
	.revision      = 1,
	.family        = NFPROTO_UNSPEC,
	.size          = XT_ALIGN(sizeof(struct xt_owner_match_info)),
	.userspacesize = XT_ALIGN(sizeof(struct xt_owner_match_info)),
	.help          = owner_mt_help,
	.parse         = owner_mt_parse,
	.final_check   = owner_mt_check,
	.print         = owner_mt_print,
	.save          = owner_mt_save,
	.extra_opts    = owner_mt_opts,
};

void _init(void)
{
	xtables_register_match(&owner_mt_reg);
}
