/*
 * Firewall services
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
#include <string.h>
#include <signal.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <dirent.h>

#include <bcmnvram.h>
#include <shutils.h>
#include <rc.h>
#include <netconf.h>
#include <nvparse.h>

#ifndef ASUS_EXT

/* Add filter to specified table */
static void
add_filter(netconf_filter_t *filter, int dir)
{
	filter->dir = dir;
	netconf_add_filter(filter);
}


/* Add port forward and a matching ACCEPT rule to the FORWARD table */
static void
add_forward(netconf_nat_t *nat, int dir, int target)
{
	netconf_filter_t filter;

	/* Set up LAN side match */
	memset(&filter, 0, sizeof(filter));
	filter.match.ipproto = nat->match.ipproto;
	filter.match.src.ports[1] = nat->match.src.ports[1];
	filter.match.dst.ipaddr.s_addr = nat->ipaddr.s_addr;
	filter.match.dst.netmask.s_addr = htonl(0xffffffff);
	filter.match.dst.ports[0] = nat->ports[0];
	filter.match.dst.ports[1] = nat->ports[1];
	strncpy(filter.match.in.name, nat->match.in.name, IFNAMSIZ);

	/* Accept connection */
	filter.target = target;
	filter.dir = dir;

	/* Do it */
	netconf_add_nat(nat);
	netconf_add_filter(&filter);
}


int
start_firewall(void)
{
	DIR *dir;
	struct dirent *file;
	char name[NAME_MAX];
	netconf_filter_t filter;
	netconf_app_t app;
	int i, num;
	char var[256], *next;
	int log_level, log_drop, log_accept;

	/* Reset firewall */
	netconf_reset_fw();

	/* Block obviously spoofed IP addresses */
	if (!(dir = opendir("/proc/sys/net/ipv4/conf")))
		perror("/proc/sys/net/ipv4/conf");
	while (dir && (file = readdir(dir))) {
		if (strncmp(file->d_name, ".", NAME_MAX) != 0 &&
		    strncmp(file->d_name, "..", NAME_MAX) != 0) {
			sprintf(name, "/proc/sys/net/ipv4/conf/%s/rp_filter", file->d_name);
			if (!(fputs_ex(name, "1")))
				break;
		}
	}
	if (dir)
		closedir(dir);

	/* Optionally log connections */
	log_level = atoi(nvram_safe_get("log_level"));
	log_drop = (log_level & 1) ? NETCONF_LOG_DROP : NETCONF_DROP;
	log_accept = (log_level & 2) ? NETCONF_LOG_ACCEPT : NETCONF_ACCEPT;

	/*
	 * Inbound drops
	 */

	/* Drop invalid packets */
	memset(&filter, 0, sizeof(filter));
	filter.match.state = NETCONF_INVALID;
	filter.target = NETCONF_DROP;
	add_filter(&filter, NETCONF_IN);
	add_filter(&filter, NETCONF_FORWARD);

#ifdef REMOVE
	/*
	 * Forward drops
	 */

	/* Client filters */
	for (i = 0; i < MAX_NVPARSE; i++) {
		netconf_filter_t start, end;

		if (get_filter_client(i, &start, &end) && !(start.match.flags & NETCONF_DISABLED)) {
			while (ntohl(start.match.src.ipaddr.s_addr) <= ntohl(end.match.src.ipaddr.s_addr)) {
				/* Override target */
				start.target = log_drop;
				add_filter(&start, NETCONF_FORWARD);
				start.match.src.ipaddr.s_addr = htonl(ntohl(start.match.src.ipaddr.s_addr) + 1);
			}
		}
	}

	/* Filter by MAC address */
	if (!nvram_match("filter_macmode", "disabled")) {
		memset(&filter, 0, sizeof(filter));
		strcpy(filter.match.in.name, nvram_safe_get("lan_ifname"));
		if (nvram_match("filter_macmode", "allow")) {
			/* Allow new connections from the LAN side only from the specified addresses */
			filter.target = log_accept;
			filter.match.state = NETCONF_NEW;
		} else {
			/* Drop connections from the specified addresses */
			filter.target = log_drop;
		}
		foreach(var, nvram_safe_get("filter_maclist"), next) {
			if (ether_atoe(var, filter.match.mac.octet))
				add_filter(&filter, NETCONF_FORWARD);
		}
	}
#endif

	/* Filter by MAC address */
	if (!nvram_match("macfilter_enable_x", "0")) 
	{
		memset(&filter, 0, sizeof(filter));
		strcpy(filter.match.in.name, nvram_safe_get("lan_ifname"));
		if (nvram_match("macfilter_enable_x", "1")) {
			/* Allow new connections from the LAN side only from the specified addresses */
			filter.target = log_accept;
			filter.match.state = NETCONF_NEW;
		} else {
			/* Drop connections from the specified addresses */
			filter.target = log_drop;
		}

		// LAN/WAN filter		
		num = atoi(nvram_safe_get("macfilter_num_x"));

		for (i=0;i<num;i++)
		{	
			if (ether_atoe(mac_conv("macfilter_list_x", i, var), filter.match.mac.octet))
			{
				add_filter(&filter, NETCONF_IN);
				add_filter(&filter, NETCONF_FORWARD);
			}
		} 
	} 

	/*
	 * Inbound accepts
	 */
	/* Allow new connections from the loopback interface */
	memset(&filter, 0, sizeof(filter));
	filter.match.state = NETCONF_NEW;
	strcpy(filter.match.in.name, "lo");
	filter.target = log_accept;
	add_filter(&filter, NETCONF_IN);
	add_filter(&filter, NETCONF_FORWARD);

	/* Allow new connections from the LAN side */
	memset(&filter, 0, sizeof(filter));
	filter.match.state = NETCONF_NEW;
	strcpy(filter.match.in.name, nvram_safe_get("lan_ifname"));
	filter.target = log_accept;
	if (!nvram_match("macfilter_enable_x", "1"))
	{
		add_filter(&filter, NETCONF_IN);
		add_filter(&filter, NETCONF_FORWARD);
	}

	/* Allow established and related outbound connections back in */
	memset(&filter, 0, sizeof(filter));
	filter.match.state = NETCONF_ESTABLISHED | NETCONF_RELATED;
	filter.target = log_accept;
	add_filter(&filter, NETCONF_IN);
	add_filter(&filter, NETCONF_FORWARD);

#ifdef REMOVE
	/*
	 * NAT
	 */

	/* Enable IP masquerading */
	fputs_ex("/proc/sys/net/ipv4/ip_forward", "1");

	/* Application specific port forwards */
	for (i = 0; i < MAX_NVPARSE; i++) {
		memset(&app, 0, sizeof(app));
		if (get_autofw_port(i, &app) && !(app.match.flags & NETCONF_DISABLED))
			netconf_add_fw((netconf_fw_t *) &app);
	}
#endif

    	/*
     	 * Inbound defaults
         */

    	/* Log refused connections */
    	memset(&filter, 0, sizeof(filter));
    	filter.target = log_drop;
    	add_filter(&filter, NETCONF_IN);
    	add_filter(&filter, NETCONF_FORWARD);

	dprintf("done\n");
	return 0;
}


int
start_firewall2(char *wan_ifname)
{
	netconf_nat_t nat;
	netconf_filter_t filter;
	int log_level, log_accept;
	char tmp[100], prefix[] = "wanXXXXXXXXXX_";
	int i;

	/* Optionally log connections */
	log_level = atoi(nvram_safe_get("log_level"));
	log_accept = (log_level & 2) ? NETCONF_LOG_ACCEPT : NETCONF_ACCEPT;
	
	/* Allow new connections from the WAN side */
	if (nvram_match("fw_disable", "1")) {
		memset(&filter, 0, sizeof(filter));
		filter.match.state = NETCONF_NEW;
		strcpy(filter.match.in.name, wan_ifname);
		filter.target = log_accept;
		add_filter(&filter, NETCONF_IN);
		add_filter(&filter, NETCONF_FORWARD);
	}

	/* Enable IP masquerading */
	memset(&nat, 0, sizeof(nat));
	inet_aton(nvram_safe_get("lan_ipaddr"), &nat.match.src.ipaddr);
	inet_aton(nvram_safe_get("lan_netmask"), &nat.match.src.netmask);
	nat.match.src.ipaddr.s_addr &= nat.match.src.netmask.s_addr;
	strcpy(nat.match.out.name, wan_ifname);
	nat.target = NETCONF_MASQ;
	netconf_add_nat(&nat);

	/* Enable remote management */
	if (nvram_invmatch("http_wanport", "")) {
		/* Set up WAN side match */
		memset(&nat, 0, sizeof(nat));
		nat.match.ipproto = IPPROTO_TCP;
		nat.match.src.ports[1] = htons(0xffff);
		nat.match.dst.ports[0] = htons(atoi(nvram_safe_get("http_wanport")));
		nat.match.dst.ports[1] = htons(atoi(nvram_safe_get("http_wanport")));
		strncpy(nat.match.in.name, wan_ifname, IFNAMSIZ);

		/* Set up DNAT to LAN */
		nat.target = NETCONF_DNAT;
		(void) inet_aton(nvram_safe_get("lan_ipaddr"), &nat.ipaddr);
		nat.ports[0] = htons(atoi(nvram_safe_get("http_lanport")));
		nat.ports[1] = htons(atoi(nvram_safe_get("http_lanport")));

		add_forward(&nat, NETCONF_IN, log_accept);
	}

	/* Persistent (static) port forwards */
	for (i = 0; i < MAX_NVPARSE; i++) {
		memset(&nat, 0, sizeof(nat));
		if (get_forward_port(i, &nat) && !(nat.match.flags & NETCONF_DISABLED)) {
			/* Set interface name (match packets entering WAN interface) */
			strncpy(nat.match.in.name, wan_ifname, IFNAMSIZ);
			add_forward(&nat, NETCONF_FORWARD, log_accept);
		}
	}

	/* Forward all WAN ports to DMZ IP address */
	if (nvram_invmatch("dmz_ipaddr", "")) {
		/* Set up WAN side match */
		memset(&nat, 0, sizeof(nat));
		nat.match.src.ports[1] = htons(0xffff);
		nat.match.dst.ports[1] = htons(0xffff);
		strncpy(nat.match.in.name, wan_ifname, IFNAMSIZ);

		/* Set up DNAT to LAN */
		nat.target = NETCONF_DNAT;
		(void) inet_aton(nvram_safe_get("dmz_ipaddr"), &nat.ipaddr);
		nat.ports[1] = htons(0xffff);

		nat.match.ipproto = IPPROTO_TCP;
		add_forward(&nat, NETCONF_FORWARD, log_accept);
		nat.match.ipproto = IPPROTO_UDP;
		add_forward(&nat, NETCONF_FORWARD, log_accept);
	}

	/* Clamp TCP MSS to PMTU of WAN interface */
	sprintf(prefix, "wan%d_", wan_ifunit(wan_ifname));
	if (nvram_match(strcat_r(prefix, "proto", tmp), "pppoe"))
		netconf_clamp_mss_to_pmtu();

	dprintf("done\n");
	return 0;
}

#endif /* ASUS_EXT */
