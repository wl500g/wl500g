/*
 * Network configuration layer (Linux)
 *
 * Copyright 2004, Broadcom Corporation
 * All Rights Reserved.
 * 
 * THIS SOFTWARE IS OFFERED "AS IS", AND BROADCOM GRANTS NO WARRANTIES OF ANY
 * KIND, EXPRESS OR IMPLIED, BY STATUTE, COMMUNICATION OR OTHERWISE. BROADCOM
 * SPECIFICALLY DISCLAIMS ANY IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS
 * FOR A SPECIFIC PURPOSE OR NONINFRINGEMENT CONCERNING THIS SOFTWARE.
 *
 * $Id$
 */

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <errno.h>
#include <error.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <syslog.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <net/if.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <net/if_arp.h>
#include <typedefs.h>
#include <proto/ethernet.h>

/* iptables definitions */
#include <libiptc/libiptc.h>
#include <iptables.h>
#include <linux/netfilter_ipv4/ip_tables.h>
#include <linux/netfilter_ipv4/ip_nat_rule.h>
#define ETH_ALEN ETHER_ADDR_LEN
#include <linux/netfilter/xt_conntrack.h>
#include <linux/netfilter_ipv4/ipt_mac.h>
#include <linux/netfilter_ipv4/ipt_state.h>
#include <linux/netfilter/xt_time.h>
#define ipt_time_info xt_time_info
#define ipt_conntrack_info xt_conntrack_mtinfo2
#include <linux/netfilter_ipv4/ipt_LOG.h>
#include <linux/netfilter_ipv4/ip_autofw.h>

#include <bcmconfig.h>
#include <netconf.h>

/* Loops over each match in the ipt_entry */
#define for_each_ipt_match(match, entry) \
	for ((match) = (struct ipt_entry_match *) &(entry)->elems[0]; \
	     (int) (match) < (int) (entry) + (entry)->target_offset; \
	     (match) = (struct ipt_entry_match *) ((int) (match) + (match)->u.match_size))

/* Supported ipt table names */
static const char *netconf_table_names[] = { "filter", "nat", NULL };

/* ipt table name appropriate for target (indexed by netconf_fw_t.target) */
const char *netconf_table_name[] = {
	"filter", "filter", "filter", "filter",
	"nat", "nat", "nat", "nat",
	NULL
};

/* ipt target name (indexed by netconf_fw_t.target) */
const char *netconf_target_name[] = {
	"DROP", "ACCEPT", "logdrop", "logaccept",
	"SNAT", "DNAT", "MASQUERADE", "autofw",
	NULL
};

/* ipt nat chain name appropriate for target (indexed by netconf_nat_t.target) */
static const char *netconf_nat_chain_name[] = { 
	NULL, NULL, NULL, NULL,
	"POSTROUTING", "PREROUTING", "POSTROUTING", "POSTROUTING"
};

/* Returns a netconf_target index */
static int
target_num(const struct ipt_entry *entry, struct iptc_handle *handle)
{
	const char *name = iptc_get_target(entry, handle);
	int i;

	if (!name)
		return -1;

	for (i = NETCONF_DROP; i < NETCONF_TARGET_MAX; i++) {
		if (strncmp(name, netconf_target_name[i], IPT_FUNCTION_MAXNAMELEN) == 0)
			return i;
	}
	return -ENOENT;
}

/*
 * get NAT chain name taking in account
 * wildcard DNAT (virtual servers)
 */
const char *
get_nat_chain_name(const netconf_fw_t *fw)
{
	return (fw->target == NETCONF_DNAT && 
			fw->match.dst.ipaddr.s_addr == INADDR_ANY) ? 
		"VSERVER" : netconf_nat_chain_name[fw->target];
}

/*
 * Get a list of the current NAT entries
 * @param	nat_array	list of NAT entries
 * @return 0 on success and errno on failure
 */
int
netconf_get_nat(netconf_nat_t *nat_list)
{
	const char **table;
	const char *chain;
	const struct ipt_entry *entry;
	struct iptc_handle *handle = NULL;

	/* Initialize list */
	netconf_list_init((netconf_fw_t *)nat_list);

	/* Search all default tables */
	for (table = &netconf_table_names[0]; *table; table++) {

		if (!(handle = iptc_init(*table))) {
			fprintf(stderr, "%s\n", iptc_strerror(errno));
			goto err;
		}

		/* Search all default chains */
		for (chain = iptc_first_chain(handle); chain; chain = iptc_next_chain(handle)) {

			if (strcmp(chain, "INPUT") && strcmp(chain, "FORWARD") && strcmp(chain, "OUTPUT") &&
			    strcmp(chain, "PREROUTING") && strcmp(chain, "POSTROUTING") &&
			    strcmp(chain, "VSERVER") && strcmp(chain, "UPNP"))
				continue;

			/* Search all entries */
			for (entry = iptc_first_rule(chain, handle); entry; entry = iptc_next_rule(entry, handle)) {
				int num = target_num(entry, handle);
				netconf_fw_t *fw = NULL;
				netconf_nat_t *nat = NULL;

				const struct ipt_entry_match *match;
				const struct ipt_entry_target *target;
				struct ipt_mac_info *mac = NULL;
				struct ipt_state_info *state = NULL;
				struct ipt_conntrack_info *conntrack = NULL;
				struct ipt_time_info *time = NULL;

				/* Only know about TCP/UDP */
				if (!netconf_valid_ipproto(entry->ip.proto))
					continue;

				/* Only know about target types in the specified tables */
				if (!netconf_valid_target(num) || (netconf_table_name[num] &&
				    strncmp(netconf_table_name[num], *table, IPT_FUNCTION_MAXNAMELEN) != 0))
					continue;

				/* NAT only */
				if (!netconf_valid_nat(num))
					continue;

				fw = (netconf_fw_t *) (nat = calloc(1, sizeof(netconf_nat_t)));
				if (!fw) {
					perror("calloc");
					goto err;
				}
				netconf_list_add(fw, (netconf_fw_t *)nat_list);

				/* Get IP addresses */
				fw->match.src.ipaddr.s_addr = entry->ip.src.s_addr;
				fw->match.src.netmask.s_addr = entry->ip.smsk.s_addr;
				fw->match.dst.ipaddr.s_addr = entry->ip.dst.s_addr;
				fw->match.dst.netmask.s_addr = entry->ip.dmsk.s_addr;
				fw->match.flags |= (entry->ip.invflags & IPT_INV_SRCIP) ? NETCONF_INV_SRCIP : 0;
				fw->match.flags |= (entry->ip.invflags & IPT_INV_DSTIP) ? NETCONF_INV_DSTIP : 0;

				/* Get interface names */
				strncpy(fw->match.in.name, entry->ip.iniface, IFNAMSIZ);
				strncpy(fw->match.out.name, entry->ip.outiface, IFNAMSIZ);
				fw->match.flags |= (entry->ip.invflags & IPT_INV_VIA_IN) ? NETCONF_INV_IN : 0;
				fw->match.flags |= (entry->ip.invflags & IPT_INV_VIA_OUT) ? NETCONF_INV_OUT : 0;

				fw->match.ipproto = entry->ip.proto;

				/* Get TCP port(s) */
				if (entry->ip.proto == IPPROTO_TCP) {
					struct ipt_tcp *tcp = NULL;

					for_each_ipt_match(match, entry) {
						if (strncmp(match->u.user.name, "tcp", IPT_FUNCTION_MAXNAMELEN) != 0)
							continue;

						tcp = (struct ipt_tcp *) &match->data[0];
						break;
					}

					if (tcp) {
						/* Match ports stored in host order for some stupid reason */
						fw->match.src.ports[0] = htons(tcp->spts[0]);
						fw->match.src.ports[1] = htons(tcp->spts[1]);
						fw->match.dst.ports[0] = htons(tcp->dpts[0]);
						fw->match.dst.ports[1] = htons(tcp->dpts[1]);
						fw->match.flags |= (tcp->invflags & IPT_TCP_INV_SRCPT) ? NETCONF_INV_SRCPT : 0;
						fw->match.flags |= (tcp->invflags & IPT_TCP_INV_DSTPT) ? NETCONF_INV_DSTPT : 0;
					}
				}

				/* Get UDP port(s) */
				else if (entry->ip.proto == IPPROTO_UDP) {
					struct ipt_udp *udp = NULL;

					for_each_ipt_match(match, entry) {
						if (strncmp(match->u.user.name, "udp", IPT_FUNCTION_MAXNAMELEN) != 0)
							continue;

						udp = (struct ipt_udp *) &match->data[0];
						break;
					}

					if (udp) {
						/* Match ports stored in host order for some stupid reason */
						fw->match.src.ports[0] = htons(udp->spts[0]);
						fw->match.src.ports[1] = htons(udp->spts[1]);
						fw->match.dst.ports[0] = htons(udp->dpts[0]);
						fw->match.dst.ports[1] = htons(udp->dpts[1]);
						fw->match.flags |= (udp->invflags & IPT_UDP_INV_SRCPT) ? NETCONF_INV_SRCPT : 0;
						fw->match.flags |= (udp->invflags & IPT_UDP_INV_DSTPT) ? NETCONF_INV_DSTPT : 0;
					}
				}

				/* Get source MAC address */
				for_each_ipt_match(match, entry) {
					if (strncmp(match->u.user.name, "mac", IPT_FUNCTION_MAXNAMELEN) != 0)
						continue;
			
					mac = (struct ipt_mac_info *) &match->data[0];
					break;
				}
				if (mac) {
					memcpy(fw->match.mac.octet, mac->srcaddr, ETHER_ADDR_LEN);
					fw->match.flags |= mac->invert ? NETCONF_INV_MAC : 0;
				}

				/* Get packet state */
				for_each_ipt_match(match, entry) {
					if (strncmp(match->u.user.name, "state", IPT_FUNCTION_MAXNAMELEN) == 0) {
						state = (struct ipt_state_info *) &match->data[0];
						break;
					} else
					if (strncmp(match->u.user.name, "conntrack", IPT_FUNCTION_MAXNAMELEN) == 0) {
						conntrack = (struct ipt_conntrack_info *) &match->data[0];
						break;
					}
				}
				if (conntrack && (conntrack->match_flags & XT_CONNTRACK_STATE)) {
					fw->match.state |= (conntrack->state_mask & XT_CONNTRACK_STATE_INVALID) ? NETCONF_INVALID : 0;
					fw->match.state |= (conntrack->state_mask & XT_CONNTRACK_STATE_BIT(IP_CT_ESTABLISHED)) ? NETCONF_ESTABLISHED : 0;
					fw->match.state |= (conntrack->state_mask & XT_CONNTRACK_STATE_BIT(IP_CT_RELATED)) ? NETCONF_RELATED : 0;
					fw->match.state |= (conntrack->state_mask & XT_CONNTRACK_STATE_BIT(IP_CT_NEW)) ? NETCONF_NEW : 0;
					fw->match.state |= (conntrack->state_mask & XT_CONNTRACK_STATE_UNTRACKED) ? NETCONF_UNTRACKED : 0;
					fw->match.state |= (conntrack->state_mask & XT_CONNTRACK_STATE_SNAT) ? NETCONF_STATE_SNAT : 0;
					fw->match.state |= (conntrack->state_mask & XT_CONNTRACK_STATE_DNAT) ? NETCONF_STATE_DNAT : 0;
				} else
				if (state) {
					fw->match.state |= (state->statemask & IPT_STATE_INVALID) ? NETCONF_INVALID : 0;
					fw->match.state |= (state->statemask & IPT_STATE_BIT(IP_CT_ESTABLISHED)) ? NETCONF_ESTABLISHED : 0;
					fw->match.state |= (state->statemask & IPT_STATE_BIT(IP_CT_RELATED)) ? NETCONF_RELATED : 0;
					fw->match.state |= (state->statemask & IPT_STATE_BIT(IP_CT_NEW)) ? NETCONF_NEW : 0;
				}

				/* Get local time */
				for_each_ipt_match(match, entry) {
					if (strncmp(match->u.user.name, "time", IPT_FUNCTION_MAXNAMELEN) != 0)
						continue;

					/* We added 8 bytes of day range at the end */
					if (match->u.match_size < (IPT_ALIGN(sizeof(struct ipt_entry_match)) +
								   IPT_ALIGN(sizeof(struct ipt_time_info) + 8)))
						continue;

					time = (struct ipt_time_info *) &match->data[0];
					break;
				}
				if (time) {
					fw->match.days    = time->weekdays_match;
					fw->match.secs[0] = time->daytime_start;
					fw->match.secs[1] = time->daytime_stop;
				}

				/* Set target type */
				fw->target = num;
				target = (struct ipt_entry_target *) ((int) entry + entry->target_offset);

				/* Get NAT target information */
				struct ip_nat_multi_range *mr = (struct ip_nat_multi_range *) &target->data[0];
				struct ip_nat_range *range = (struct ip_nat_range *) &mr->range[0];
			
				/* Get mapped IP address */
				nat->ipaddr.s_addr = range->min_ip;
			
				/* Get mapped TCP port(s) */
				if (entry->ip.proto == IPPROTO_TCP) {
					nat->ports[0] = range->min.tcp.port;
					nat->ports[1] = range->max.tcp.port;
				}

				/* Get mapped UDP port(s) */
				else if (entry->ip.proto == IPPROTO_UDP) {
					nat->ports[0] = range->min.udp.port;
					nat->ports[1] = range->max.udp.port;
				}

			}
		}

		if (!iptc_commit(handle)) {
			fprintf(stderr, "%s\n", iptc_strerror(errno));
			goto err;
		}
		iptc_free(handle);
		handle = NULL;
	}

	return 0;

 err:
	if (handle)
		iptc_free(handle);
	netconf_list_free((netconf_fw_t *)nat_list);
	return errno;	
}
