/* Shared library add-on to iptables to add redirect support. */
#include <stdio.h>
#include <netdb.h>
#include <string.h>
#include <stdlib.h>
#include <getopt.h>
#include <xtables.h>
#include <limits.h> /* INT_MAX in ip_tables.h */
#include <linux/netfilter_ipv4/ip_tables.h>
#include <net/netfilter/nf_nat.h>

#define IPT_REDIRECT_OPT_DEST	0x01
#define IPT_REDIRECT_OPT_RANDOM	0x02

static void REDIRECT_help(void)
{
	printf(
"REDIRECT target options:\n"
" --to-ports <port>[-<port>]\n"
"				Port (range) to map to.\n");
}

static const struct option REDIRECT_opts[] = {
	{ "to-ports", 1, NULL, '1' },
	{ "random", 0, NULL, '2' },
	{ .name = NULL }
};

static void REDIRECT_init(struct xt_entry_target *t)
{
	struct nf_nat_multi_range *mr = (struct nf_nat_multi_range *)t->data;

	/* Actually, it's 0, but it's ignored at the moment. */
	mr->rangesize = 1;

}

/* Parses ports */
static void
parse_ports(const char *arg, struct nf_nat_multi_range *mr)
{
	const char *dash;
	int port;

	mr->range[0].flags |= IP_NAT_RANGE_PROTO_SPECIFIED;

	if (strchr(arg, '.'))
		xtables_error(PARAMETER_PROBLEM, "IP address not permitted\n");

	port = atoi(arg);
	if (port == 0)
		port = xtables_service_to_port(arg, NULL);

	if (port == 0 || port > 65535)
		xtables_error(PARAMETER_PROBLEM, "Port \"%s\" not valid\n", arg);

	dash = strchr(arg, '-');
	if (!dash) {
		mr->range[0].min.tcp.port
			= mr->range[0].max.tcp.port
			= htons(port);
	} else {
		int maxport;

		maxport = atoi(dash + 1);
		if (maxport == 0 || maxport > 65535)
			xtables_error(PARAMETER_PROBLEM,
				   "Port `%s' not valid\n", dash+1);
		if (maxport < port)
			/* People are stupid. */
			xtables_error(PARAMETER_PROBLEM,
				   "Port range `%s' funky\n", arg);
		mr->range[0].min.tcp.port = htons(port);
		mr->range[0].max.tcp.port = htons(maxport);
	}
}

static int REDIRECT_parse(int c, char **argv, int invert, unsigned int *flags,
                          const void *e, struct xt_entry_target **target)
{
	const struct ipt_entry *entry = e;
	struct nf_nat_multi_range *mr
		= (struct nf_nat_multi_range *)(*target)->data;
	int portok;

	if (entry->ip.proto == IPPROTO_TCP
	    || entry->ip.proto == IPPROTO_UDP
	    || entry->ip.proto == IPPROTO_SCTP
	    || entry->ip.proto == IPPROTO_DCCP
	    || entry->ip.proto == IPPROTO_ICMP)
		portok = 1;
	else
		portok = 0;

	switch (c) {
	case '1':
		if (!portok)
			xtables_error(PARAMETER_PROBLEM,
				   "Need TCP, UDP, SCTP or DCCP with port specification");

		if (xtables_check_inverse(optarg, &invert, NULL, 0))
			xtables_error(PARAMETER_PROBLEM,
				   "Unexpected `!' after --to-ports");

		parse_ports(optarg, mr);
		if (*flags & IPT_REDIRECT_OPT_RANDOM)
			mr->range[0].flags |= IP_NAT_RANGE_PROTO_RANDOM;
		*flags |= IPT_REDIRECT_OPT_DEST;
		return 1;

	case '2':
		if (*flags & IPT_REDIRECT_OPT_DEST) {
			mr->range[0].flags |= IP_NAT_RANGE_PROTO_RANDOM;
			*flags |= IPT_REDIRECT_OPT_RANDOM;
		} else
			*flags |= IPT_REDIRECT_OPT_RANDOM;
		return 1;

	default:
		return 0;
	}
}

static void REDIRECT_print(const void *ip, const struct xt_entry_target *target,
                           int numeric)
{
	struct nf_nat_multi_range *mr
		= (struct nf_nat_multi_range *)target->data;
	struct nf_nat_range *r = &mr->range[0];

	if (r->flags & IP_NAT_RANGE_PROTO_SPECIFIED) {
		printf("redir ports ");
		printf("%hu", ntohs(r->min.tcp.port));
		if (r->max.tcp.port != r->min.tcp.port)
			printf("-%hu", ntohs(r->max.tcp.port));
		printf(" ");
		if (mr->range[0].flags & IP_NAT_RANGE_PROTO_RANDOM)
			printf("random ");
	}
}

static void REDIRECT_save(const void *ip, const struct xt_entry_target *target)
{
	struct nf_nat_multi_range *mr
		= (struct nf_nat_multi_range *)target->data;
	struct nf_nat_range *r = &mr->range[0];

	if (r->flags & IP_NAT_RANGE_PROTO_SPECIFIED) {
		printf("--to-ports ");
		printf("%hu", ntohs(r->min.tcp.port));
		if (r->max.tcp.port != r->min.tcp.port)
			printf("-%hu", ntohs(r->max.tcp.port));
		printf(" ");
		if (mr->range[0].flags & IP_NAT_RANGE_PROTO_RANDOM)
			printf("--random ");
	}
}

static struct xtables_target redirect_tg_reg = {
	.name		= "REDIRECT",
	.version	= XTABLES_VERSION,
	.family		= NFPROTO_IPV4,
	.size		= XT_ALIGN(sizeof(struct nf_nat_multi_range)),
	.userspacesize	= XT_ALIGN(sizeof(struct nf_nat_multi_range)),
	.help		= REDIRECT_help,
	.init		= REDIRECT_init,
 	.parse		= REDIRECT_parse,
	.print		= REDIRECT_print,
	.save		= REDIRECT_save,
	.extra_opts	= REDIRECT_opts,
};

void _init(void)
{
	xtables_register_target(&redirect_tg_reg);
}
