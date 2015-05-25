/* Shared library add-on to iptables for TARPIT support */
#include <stdio.h>
#include <getopt.h>
#include <iptables.h>

static void
help(void)
{
	fputs(
"TARPIT takes no options\n"
"\n", stdout);
}

static struct option opts[] = {
	{ 0 }
};

static void
init(struct ipt_entry_target *t)
{
}

static int
parse(int c, char **argv, int invert, unsigned int *flags,
      const void *entry,
      struct ipt_entry_target **target)
{
	return 0;
}

static void
print(const void *ip,
      const struct ipt_entry_target *target,
      int numeric)
{
}

static void save(const void *ip, const struct ipt_entry_target *target)
{
}

static
struct xtables_target tarpit = {
	.name		=     "TARPIT",
	.family		=     NFPROTO_IPV4,
	.version	=     XTABLES_VERSION,
	.size		=     XT_ALIGN(0),
	.userspacesize	=     XT_ALIGN(0),
	.help		=     help,
	.init		=     init,
	.parse		=     parse,
	.print		=     print,
	.save		=     save,
	.extra_opts	=     opts
};

void _init(void)
{
	xtables_register_target(&tarpit);
}
