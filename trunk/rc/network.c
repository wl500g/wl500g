/*
 * Network services
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
#include <errno.h>
#include <syslog.h>
#include <ctype.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <net/if.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <net/if_arp.h>
#include <signal.h>
#include <linux/types.h>
#include <linux/sockios.h>
#include <linux/ethtool.h>
#include <semaphore.h>
#include <fcntl.h>

#include <wlutils.h>
#include <bcmutils.h>
#include <etioctl.h>
#include <bcmparams.h>
#include <netconf.h>
#include <nvparse.h>
#include "rc.h"

static void lan_up(const char *lan_ifname);
static int wait_for_ifup(const char *prefix, const char *wan_ifname, struct ifreq *ifr);

#ifdef RC_SEMAPHORE_ENABLED

#define HOTPLUG_DEV_START
sem_t * hotplug_sem = SEM_FAILED;

void hotplug_sem_open()
{
	hotplug_sem = sem_open( "/hotplug_sem", O_CREAT | O_EXCL, S_IRWXU | S_IRWXG, 1 );
	if ( hotplug_sem == SEM_FAILED ) {
		hotplug_sem = sem_open( "/hotplug_sem", 0 );
#ifdef DEBUG
		if (hotplug_sem == SEM_FAILED)
			dprintf( "Semaphore error: %p, %s", hotplug_sem, strerror(errno) );
#endif
	}
}

void
hotplug_sem_close()
{
	if (hotplug_sem != SEM_FAILED) {
		sem_close( hotplug_sem );
		hotplug_sem = SEM_FAILED;
	}
}

void
hotplug_sem_lock()
{
	if (hotplug_sem != SEM_FAILED) sem_wait( hotplug_sem );
	else dprintf("sem_lock with empty semaphore");
}

int
hotplug_sem_trylock()
{
	if (hotplug_sem != SEM_FAILED)
		return sem_trywait( hotplug_sem ) != -1;
	return 1;
}

void
hotplug_sem_unlock()
{
	if (hotplug_sem != SEM_FAILED) sem_post( hotplug_sem );
}
#else /* !RC_SEMAPHORE_ENABLED */
void hotplug_sem_open() {}
void hotplug_sem_close() {}
void hotplug_sem_lock() {}
int hotplug_sem_trylock() { return 1; }
void hotplug_sem_unlock() {}
#endif /* RC_SEMAPHORE_ENABLED */

#ifdef __CONFIG_EMF__
static void emf_mfdb_update(const char *lan_ifname, const char *lan_port_ifname, bool add)
{
        char word[256], *next;
        char *mgrp, *ifname;

        /* Add/Delete MFDB entries corresponding to new interface */
        foreach (word, nvram_safe_get("emf_entry"), next) {
                ifname = word;
                mgrp = strsep(&ifname, ":");

                if ((mgrp == 0) || (ifname == 0))
                        continue;

                /* Add/Delete MFDB entry using the group addr and interface */
                if (strcmp(lan_port_ifname, ifname) == 0) {
                        eval("emf", ((add) ? "add" : "del"),
                             "mfdb", lan_ifname, mgrp, ifname);
                }
        }
}

static void emf_uffp_update(const char *lan_ifname, const char *lan_port_ifname, bool add)
{
        char word[256], *next;
        char *ifname;

        /* Add/Delete UFFP entries corresponding to new interface */
        foreach (word, nvram_safe_get("emf_uffp_entry"), next) {
                ifname = word;

                if (ifname == 0)
                        continue;

                /* Add/Delete UFFP entry for the interface */
                if (strcmp(lan_port_ifname, ifname) == 0) {
                        eval("emf", ((add) ? "add" : "del"),
                             "uffp", lan_ifname, ifname);
                }
        }
}

static void emf_rtport_update(const char *lan_ifname, const char *lan_port_ifname, bool add)
{
        char word[256], *next;
        char *ifname;

        /* Add/Delete RTPORT entries corresponding to new interface */
        foreach (word, nvram_safe_get("emf_rtport_entry"), next) {
                ifname = word;

                if (ifname == 0)
                        continue;

                /* Add/Delete RTPORT entry for the interface */
                if (strcmp(lan_port_ifname, ifname) == 0) {
                        eval("emf", ((add) ? "add" : "del"),
                             "rtport", lan_ifname, ifname);
                }
        }
}

static void start_emf(const char *lan_ifname)
{
        char word[256], *next;
        char *mgrp, *ifname;

        if (!nvram_match("emf_enable", "1"))
                return;

        /* Start EMF */
        eval("emf", "start", lan_ifname);

        /* Add the static MFDB entries */
        foreach (word, nvram_safe_get("emf_entry"), next) {
                ifname = word;
                mgrp = strsep(&ifname, ":");

                if ((mgrp == 0) || (ifname == 0))
                        continue;

                /* Add MFDB entry using the group addr and interface */
                eval("emf", "add", "mfdb", lan_ifname, mgrp, ifname);
        }

        /* Add the UFFP entries */
        foreach (word, nvram_safe_get("emf_uffp_entry"), next) {
                ifname = word;
                if (ifname == 0)
                        continue;

                /* Add UFFP entry for the interface */
                eval("emf", "add", "uffp", lan_ifname, ifname);
        }

        /* Add the RTPORT entries */
        foreach (word, nvram_safe_get("emf_rtport_entry"), next) {
                ifname = word;
                if (ifname == 0)
                        continue;

                /* Add RTPORT entry for the interface */
                eval("emf", "add", "rtport", lan_ifname, ifname);
        }
}
#endif

static int add_routes(const char *prefix, const char *var, const char *ifname)
{
	int err, m;
	char word[80], *next;
	char *ipaddr, *netmask, *gateway, *metric;
	char tmp[100];

	foreach(word, nvram_safe_get(strcat_r(prefix, var, tmp)), next) {

		netmask = word;
		ipaddr = strsep(&netmask, ":");
		if (!ipaddr || !netmask)
			continue;
		gateway = netmask;
		netmask = strsep(&gateway, ":");
		if (!netmask || !gateway)
			continue;
		metric = gateway;
		gateway = strsep(&metric, ":");
		if (!gateway || !metric)
			continue;
			
		if (ip_addr(gateway) == INADDR_ANY) 
			gateway = nvram_safe_get("wanx_gateway");

		m = atoi(metric) + 1;
		dprintf("\n\n\nadd %s %d %s %s %s\n\n\n", ifname, m, ipaddr, gateway, netmask);
		
		if ((err = route_add(ifname, m, ipaddr, gateway, netmask))) {
			logmessage("route", "add failed(%d) '%s': %s dst %s mask %s gw %s metric %d",
			err, strerror(err),
			ifname, ipaddr, gateway, netmask, m);
		}
	}

	return 0;
}

static void add_wanx_routes(const char *prefix, const char *ifname, int metric)
{
	char *routes, *tmp;
	char buf[30];
	struct in_addr mask;
	char *ipaddr, *gateway;
	int bits;
	char netmask[] = "255.255.255.255";

	if (!nvram_match("dr_enable_x", "1"))
		return;

	/* classful static routes */
	routes = strdup(nvram_safe_get(strcat_r(prefix, "routes", buf)));
	for (tmp = routes; tmp && *tmp; )
	{
		ipaddr  = strsep(&tmp, "/");
		gateway = strsep(&tmp, " ");

		if (gateway && ip_addr(ipaddr) != INADDR_ANY)
			route_add(ifname, metric + 1, ipaddr, gateway, netmask);
	}
	free(routes);

	/* rfc3442 or ms classless static routes */
	routes = nvram_safe_get(strcat_r(prefix, "routes_rfc", buf));
	if (!*routes)
		routes = nvram_safe_get(strcat_r(prefix, "routes_ms", buf));
	routes = strdup(routes);
	for (tmp = routes; tmp && *tmp; )
	{
		ipaddr  = strsep(&tmp, "/");
		bits    = atoi(strsep(&tmp, " "));
		gateway = strsep(&tmp, " ");

		if (gateway && bits > 0 && bits <= 32)
		{
			mask.s_addr = htonl(0xffffffff << (32 - bits));
			strcpy(netmask, inet_ntoa(mask));
			route_add(ifname, metric + 1, ipaddr, gateway, netmask);
		}
	}
	free(routes);
}

static int del_routes(const char *prefix, const char *var, const char *ifname)
{
	char word[80], *next;
	char *ipaddr, *netmask, *gateway, *metric;
	char tmp[100];
	
	foreach(word, nvram_safe_get(strcat_r(prefix, var, tmp)), next) {
		dprintf("add %s\n", word);
		
		netmask = word;
		ipaddr = strsep(&netmask, ":");
		if (!ipaddr || !netmask)
			continue;
		gateway = netmask;
		netmask = strsep(&gateway, ":");
		if (!netmask || !gateway)
			continue;
		metric = gateway;
		gateway = strsep(&metric, ":");
		if (!gateway || !metric)
			continue;
			
		if (ip_addr(gateway) == INADDR_ANY) 
			gateway = nvram_safe_get("wanx_gateway");
		
		dprintf("add %s\n", ifname);
		
		route_del(ifname, atoi(metric) + 1, ipaddr, gateway, netmask);
	}

	return 0;
}

static inline int add_lan_routes(const char *lan_ifname)
{
	return add_routes("lan_", "route", lan_ifname);
}

static inline int del_lan_routes(const char *lan_ifname)
{
	return del_routes("lan_", "route", lan_ifname);
}

static void start_igmpproxy(const char *wan_ifname)
{
	FILE *fp;
	const char *igmpproxy_conf = "/etc/igmpproxy.conf";
	char *udpxy_argv[] = { "/usr/sbin/udpxy",
		"-m", (char *)wan_ifname,
		"-p", nvram_get("udpxy_enable_x"),
		"-a", nvram_safe_default_get("lan_ifname"),
		NULL };

	if (atoi(nvram_safe_get("udpxy_enable_x")))
	{
		/* listen on all interfaces */
		if (nvram_invmatch("udpxy_wan_x", "0"))
			udpxy_argv[5] = NULL;

		killall_w("udpxy", 0, 1);
		_eval(udpxy_argv, NULL, 0, NULL);
	}

	if (!nvram_match("mr_enable_x", "1"))
		return;

	if (!exists(igmpproxy_conf))
	{
		if ((fp = fopen(igmpproxy_conf, "w")) == NULL) {
			perror(igmpproxy_conf);
			return;
		}
		fprintf(fp, "# automagically generated\n"
			"quickleave\n\n"
			"phyint %s upstream\n"
			"\taltnet %s\n\n"
			"phyint %s downstream ratelimit 0\n\n", 
			wan_ifname, 
			nvram_get("mr_altnet_x") ? : "0.0.0.0/0", 
			nvram_safe_default_get("lan_ifname"));
		fclose(fp);
	}

	killall_w("igmpproxy", 0, 1);
	eval("/usr/sbin/igmpproxy", igmpproxy_conf);
}

void stop_igmpproxy()
{
	killall("igmpproxy");
	killall("udpxy");
}

void start_lan(void)
{
	const char *lan_ifname = nvram_safe_get("lan_ifname");
	char name[80], *next;
	int s;
	struct ifreq ifr;

	dprintf("%s\n", lan_ifname);

 	/* Bring up bridged interfaces */
	if (strncmp(lan_ifname, "br", 2) == 0) {
		eval("brctl", "addbr", lan_ifname);
		eval("brctl", "setfd", lan_ifname, "0");
		if (nvram_match("router_disable", "1") || nvram_match("lan_stp", "0"))
			eval("brctl", "stp", lan_ifname, "off");

#ifdef __CONFIG_EMF__
                if (nvram_match("emf_enable", "1")) {
                        eval("emf", "add", "bridge", lan_ifname);
                        eval("igs", "add", "bridge", lan_ifname);
                }
#endif

#ifdef ASUS_EXT
		foreach(name, nvram_safe_get("lan_ifnames_t"), next) {
#else
		foreach(name, nvram_safe_get("lan_ifnames"), next) {
#endif
			/* Bring up interface */
			ifconfig(name, IFUP, NULL, NULL);
			/* Set the logical bridge address to that of the first interface */
			if ((s = socket(AF_INET, SOCK_RAW, IPPROTO_RAW)) < 0)
				continue;
			strncpy(ifr.ifr_name, lan_ifname, IFNAMSIZ);
			if (ioctl(s, SIOCGIFHWADDR, &ifr) == 0 &&
			    memcmp(ifr.ifr_hwaddr.sa_data, "\0\0\0\0\0\0", ETHER_ADDR_LEN) == 0) {
				strncpy(ifr.ifr_name, name, IFNAMSIZ);
				if (ioctl(s, SIOCGIFHWADDR, &ifr) == 0) {
					strncpy(ifr.ifr_name, lan_ifname, IFNAMSIZ);
					ifr.ifr_hwaddr.sa_family = ARPHRD_ETHER;
					ioctl(s, SIOCSIFHWADDR, &ifr);
				}
			}
			close(s);
			/* If not a wl i/f then simply add it to the bridge */
			if (eval("wlconf", name, "up"))
			{
				eval("brctl", "addif", lan_ifname, name);
#ifdef __CONFIG_EMF__
				if (nvram_match("emf_enable", "1"))
					eval("emf", "add", "iface", lan_ifname, name);
#endif
			}
			else 
			{
				/* get the instance number of the wl i/f */
				char wl_name[sizeof("wlXXXXXXXXXX_mode")];
				int unit;
#ifdef ASUS_EXT
				/* do not play srom games, let asus firmware do everything */
				/* mac offset == 72 for sroms 1, 2, == 76 for srom 3 */
				/* sync_mac(name, nvram_safe_get("et0macaddr")); */
#endif
				wl_ioctl(name, WLC_GET_INSTANCE, &unit, sizeof(unit));
				snprintf(wl_name, sizeof(wl_name), "wl%d_mode", unit);
				/* Receive all multicast frames in WET mode */
				if (nvram_match(wl_name, "wet"))
					ifconfig(name, IFUP | IFF_ALLMULTI, NULL, NULL);
				/* Do not attach the main wl i/f if in wds mode */
				if (nvram_invmatch(wl_name, "wds")) {
					eval("brctl", "addif", lan_ifname, name);
#ifdef __CONFIG_EMF__
					if (nvram_match("emf_enable", "1"))
						eval("emf", "add", "iface", lan_ifname, name);
#endif
				}
			}
		}
	}
	/* specific non-bridged lan i/f */
	else if (strcmp(lan_ifname, "")) {
		/* Bring up interface */
		ifconfig(lan_ifname, IFUP, NULL, NULL);
		/* config wireless i/f */
		if (eval("wlconf", lan_ifname, "up") == 0) {
			char tmp[100], prefix[sizeof("wanXXXXXXXXXX_")];
			int unit;

			/* get the instance number of the wl i/f */
			wl_ioctl(lan_ifname, WLC_GET_INSTANCE, &unit, sizeof(unit));
			snprintf(prefix, sizeof(prefix), "wl%d_", unit);
			/* Receive all multicast frames in WET mode */
			if (nvram_match(strcat_r(prefix, "mode", tmp), "wet"))
				ifconfig(lan_ifname, IFUP | IFF_ALLMULTI, NULL, NULL);
		}
	}
	/* Get current LAN hardware address */
	if ((s = socket(AF_INET, SOCK_RAW, IPPROTO_RAW)) >= 0) {
		char eabuf[32];

		strncpy(ifr.ifr_name, lan_ifname, IFNAMSIZ);
		if (ioctl(s, SIOCGIFHWADDR, &ifr) == 0)
			nvram_set("lan_hwaddr", ether_etoa(ifr.ifr_hwaddr.sa_data, eabuf));
		close(s);
	}

#ifdef LINUX26 // TODO: Make et version specific?
	/* set the packet size */
	if (nvram_match("jumbo_frame_enable", "1"))
		eval("et", "robowr", "0x40", "0x05", nvram_safe_get("jumbo_frame_size"));
#endif

#ifdef WPA2_WMM
	/* Set QoS mode */
	if ((s = socket(AF_INET, SOCK_RAW, IPPROTO_RAW)) >= 0) {
		int i, qos;
		caddr_t ifrdata;
		struct ethtool_drvinfo info;

		qos = (strcmp(nvram_safe_get("wl_wme"), "on")) ? 0 : 1;
		for (i = 1; i <= DEV_NUMIFS; i ++) {
			ifr.ifr_ifindex = i;
			if (ioctl(s, SIOCGIFNAME, &ifr))
				continue;
			if (ioctl(s, SIOCGIFHWADDR, &ifr))
				continue;
			if (ifr.ifr_hwaddr.sa_family != ARPHRD_ETHER)
				continue;
			/* get flags */
			if (ioctl(s, SIOCGIFFLAGS, &ifr))
				continue;
			/* if up(wan not up yet at this point) */
			if (ifr.ifr_flags & IFF_UP) {
				ifrdata = ifr.ifr_data;
				memset(&info, 0, sizeof(info));
				info.cmd = ETHTOOL_GDRVINFO;
				ifr.ifr_data = (caddr_t)&info;
				if (ioctl(s, SIOCETHTOOL, &ifr) >= 0) {
					/* currently only need to set QoS to et devices */
					if (!strncmp(info.driver, "et", 2)) {
						ifr.ifr_data = (caddr_t)&qos;
						ioctl(s, SIOCSETCQOS, &ifr);
					}
				}
				ifr.ifr_data = ifrdata;
			}
		}
	}
	close(s);
#endif

#ifdef ASUS_EXT
	/* 
	* Configure DHCP connection. The DHCP client will run 
	* 'udhcpc.script bound'/'udhcpc.script deconfig' upon finishing IP address 
	* renew and release.
	*/
	if (nvram_match("router_disable", "1"))
	{
		if (nvram_match("lan_proto_x", "1")) 
		{
			char *dhcp_argv[] = { "/sbin/udhcpc",
					      "-i", (char *)lan_ifname,
					      "-p", "/var/run/udhcpc_lan.pid",
					      "-s", "/tmp/landhcpc",
#ifdef DEBUG
					      "-vv", "-S",
#endif
					      NULL
			};
			pid_t pid;


			/* Bring up and configure LAN interface */
			ifconfig(lan_ifname, IFUP,
		 		nvram_safe_get("lan_ipaddr"),
		 		nvram_safe_get("lan_netmask"));

			symlink("/sbin/rc", "/tmp/landhcpc");

			/* Start dhcp daemon */
			_eval(dhcp_argv, NULL, 0, &pid);
		}
		else
		{
			/* Bring up and configure LAN interface */
			ifconfig(lan_ifname, IFUP,
		 		nvram_safe_get("lan_ipaddr"),
		 		nvram_safe_get("lan_netmask"));
			lan_up(lan_ifname);

			update_lan_status(1);
		}
	}
	else
	{
		/* Bring up and configure LAN interface */
		ifconfig(lan_ifname, IFUP,
		 	nvram_safe_get("lan_ipaddr"),
		 	nvram_safe_get("lan_netmask"));
		/* Install lan specific static routes */
		add_lan_routes(lan_ifname);

		update_lan_status(1);
	}
#else /* ASUS_EXT */
	/* Bring up and configure LAN interface */
	ifconfig(lan_ifname, IFUP,
		 nvram_safe_get("lan_ipaddr"),
		 nvram_safe_get("lan_netmask"));

	/* Install lan specific static routes */
	add_lan_routes(lan_ifname);
#endif /* !ASUS_EXT */

#ifdef __CONFIG_IPV6__
	/* Configure LAN IPv6 address */
	if (nvram_invmatch("ipv6_proto", "") &&
	    nvram_invmatch("ipv6_proto", "dhcp6") &&
	    nvram_invmatch("ipv6_proto", "tun6to4") &&
	    nvram_invmatch("ipv6_proto", "tun6rd"))
	{
		struct in6_addr addr;
		char addrstr[INET6_ADDRSTRLEN];
		int size;

		ipv6_addr(nvram_safe_get("ipv6_lan_addr"), &addr);
    		inet_ntop(AF_INET6, &addr, addrstr, INET6_ADDRSTRLEN);
    		size = atoi(nvram_safe_get("ipv6_lan_netsize"));
    		if (size > 0)
    			sprintf(addrstr, "%s/%d", addrstr, size);
		eval("ip", "-6", "addr", "add", addrstr, "dev", lan_ifname);
		nvram_set("lan_ipv6_addr", addrstr);
	}
#endif /* __CONFIG_IPV6__ */

#ifdef __CONFIG_EMF__
	/* Start the EMF for this LAN */
	start_emf(lan_ifname);
#endif

#ifndef ASUS_EXT
	/* Start syslogd if either log_ipaddr or log_ram_enable is set */
	if (nvram_invmatch("log_ipaddr", "") || nvram_match("log_ram_enable", "1")) {
		char *syslogd_argv[] = {
			"syslogd",
			NULL, 		/* -C */
			NULL, NULL,	/* -R host */
			NULL
		};
		int pid;
		int argc = 1;
		
		if (nvram_match("log_ram_enable", "1")) {
			syslogd_argv[argc++] = "-C";
		}
		else if (!nvram_match("log_ram_enable", "0")) {
			nvram_set("log_ram_enable", "0");
		}
				
		if (nvram_invmatch("log_ipaddr", "")) {
			syslogd_argv[argc++] = "-R";
			syslogd_argv[argc++] = nvram_get("log_ipaddr");
		}

		_eval(syslogd_argv, NULL, 0, &pid);
	}
#endif /* !ASUS_EXT */

	dprintf("%s %s\n",
		nvram_safe_get("lan_ipaddr"),
		nvram_safe_get("lan_netmask"));
}

void stop_lan(void)
{
	char *lan_ifname = nvram_safe_get("lan_ifname");
	char name[80], *next;

	dprintf("%s\n", lan_ifname);

#ifndef ASUS_EXT
	/* Stop the syslogd daemon */
	killall("syslogd");
#endif

	/* Remove static routes */
	del_lan_routes(lan_ifname);

	/* Bring down LAN interface */
	ifconfig(lan_ifname, 0, NULL, NULL);

	/* Bring down bridged interfaces */
	if (strncmp(lan_ifname, "br", 2) == 0) {
#ifdef __CONFIG_EMF__
		if (nvram_match("emf_enable", "1")) {
			eval("emf"  "stop", lan_ifname);
			eval("igs", "del", "bridge", lan_ifname);
			eval("emf", "del", "bridge", lan_ifname);
	}
#endif
#ifdef ASUS_EXT
		foreach(name, nvram_safe_get("lan_ifnames_t"), next) {
#else
		foreach(name, nvram_safe_get("lan_ifnames"), next) {
#endif
			eval("wlconf", name, "down");
			ifconfig(name, 0, NULL, NULL);
			eval("brctl", "delif", lan_ifname, name);
		}
		eval("brctl", "delbr", lan_ifname);
	}
	/* Bring down specific interface */
	else if (strcmp(lan_ifname, ""))
		eval("wlconf", lan_ifname, "down");

	dprintf("done\n");
}

int wan_prefix(const char *wan_ifname, char *prefix)
{
	int unit;
	char tmp[100];

	if ((unit = ppp_ifunit(wan_ifname)) >= 0)
		goto found;
#ifdef __CONFIG_MADWIMAX__
	else if ((unit = wimax_ifunit(wan_ifname)) >= 0)
		goto found;
#endif
	else {
		for (unit = 0; unit < MAX_NVPARSE; unit ++) {
			sprintf(prefix, "wan%d_", unit);
			if (nvram_match(strcat_r(prefix, "ifname", tmp), wan_ifname) &&
			    (nvram_match(strcat_r(prefix, "proto", tmp), "dhcp") ||
			     nvram_match(strcat_r(prefix, "proto", tmp), "bigpond") ||
			     nvram_match(strcat_r(prefix, "proto", tmp), "static")))
				return unit;
		}
	}
	strcpy(prefix, "wanx_");
	return -1;

 found:
	sprintf(prefix, "wan%d_", unit);
	return unit;
}

static int add_wan_routes(const char *wan_ifname)
{
	char prefix[sizeof("wanXXXXXXXXXX_")];

	/* Figure out nvram variable name prefix for this i/f */
	if (wan_prefix(wan_ifname, prefix) < 0)
		return -1;

	return add_routes(prefix, "route", wan_ifname);
}

static int del_wan_routes(const char *wan_ifname)
{
	char prefix[sizeof("wanXXXXXXXXXX_")];

	/* Figure out nvram variable name prefix for this i/f */
	if (wan_prefix(wan_ifname, prefix) < 0)
		return -1;

	return del_routes(prefix, "route", wan_ifname);
}

int wan_valid(const char *ifname)
{
	char name[80], *next;

	foreach(name, nvram_safe_get("wan_ifnames"), next)
		if (ifname && !strcmp(ifname, name))
			return 1;

	if (nvram_invmatch("wl_mode_ex", "ap")) {
		return nvram_match("wl0_ifname", ifname);
	}

	return 0;
}

void start_wan_unit(int unit)
{
	char *wan_ifname;
	char *wan_proto;
	char tmp[100], prefix[sizeof("wanXXXXXXXXXX_")];
	char eabuf[32];
	int s;
	struct ifreq ifr;

#ifdef ASUS_EXT
	update_wan_status(0);
#endif

	do {
		snprintf(prefix, sizeof(prefix), "wan%d_", unit);

		/* make sure the connection exists and is enabled */ 
		wan_ifname = nvram_get(strcat_r(prefix, "ifname", tmp));
		if (!wan_ifname)
			continue;

		wan_proto = nvram_get(strcat_r(prefix, "proto", tmp));
		if (!wan_proto || !strcmp(wan_proto, "disabled"))
			continue;

		dprintf("%s %s\n\n\n\n\n", wan_ifname, wan_proto);

		/* disable the connection if the i/f is not in wan_ifnames */
		if (!wan_valid(wan_ifname)) {
			nvram_set(strcat_r(prefix, "proto", tmp), "disabled");
			continue;
		}

#ifdef __CONFIG_MADWIMAX__
		if (strcmp(wan_proto, "wimax") != 0)
		{
#endif
		/* Set i/f hardware address before bringing it up */
		if ((s = socket(AF_INET, SOCK_RAW, IPPROTO_RAW)) < 0)
			continue;

		strncpy(ifr.ifr_name, wan_ifname, IFNAMSIZ);

		/* Since WAN interface may be already turned up (by vlan.c),
		   if WAN hardware address is specified (and different than the current one),
		   we need to make it down for synchronizing hwaddr. */
		if (ioctl(s, SIOCGIFHWADDR, &ifr)) {
			close(s);
			continue;
		}

		ether_atoe(nvram_safe_get(strcat_r(prefix, "hwaddr", tmp)), eabuf);
		if (memcmp(eabuf, ifr.ifr_hwaddr.sa_data, ETHER_ADDR_LEN))
		{
			/* current hardware address is different than user specified */
			ifconfig(wan_ifname, 0, NULL, NULL);
		}

		/* Configure i/f only once, specially for wireless i/f shared by multiple connections */
		if (ioctl(s, SIOCGIFFLAGS, &ifr)) {
			close(s);
			continue;
		}
		else if ( !(ifr.ifr_flags & IFF_UP) )
		{
			/* Sync connection nvram address and i/f hardware address */
			memset(ifr.ifr_hwaddr.sa_data, 0, ETHER_ADDR_LEN);

			if (!nvram_invmatch(strcat_r(prefix, "hwaddr", tmp), "") ||
			    !ether_atoe(nvram_safe_get(strcat_r(prefix, "hwaddr", tmp)), ifr.ifr_hwaddr.sa_data) ||
			    !memcmp(ifr.ifr_hwaddr.sa_data, "\0\0\0\0\0\0", ETHER_ADDR_LEN)) {
				if (ioctl(s, SIOCGIFHWADDR, &ifr)) {
					close(s);
					continue;
				}
				nvram_set(strcat_r(prefix, "hwaddr", tmp), ether_etoa(ifr.ifr_hwaddr.sa_data, eabuf));
			}
			else {
				ifr.ifr_hwaddr.sa_family = ARPHRD_ETHER;
				ioctl(s, SIOCSIFHWADDR, &ifr);
			}

			/* Bring up i/f */
			ifconfig(wan_ifname, IFUP, NULL, NULL);

			/* do wireless specific config */
			if (eval("wlconf", wan_ifname, "up") == 0) {
				/* Kick wl to join network */
				if (nvram_match("wl0_mode", "wet") || nvram_match("wl0_mode", "sta"))
					system(nvram_safe_get("wl0_join"));
			}
		}
		close(s);

#ifdef ASUS_EXT
		if (nvram_match(strcat_r(prefix, "primary", tmp), "1"))
		{
			setup_ethernet(wan_ifname);
			start_pppoe_relay(wan_ifname);
		}
#endif

#ifdef __CONFIG_MADWIMAX__
		}
#endif

#ifdef ASUS_EXT
		//if (unit==0)
		{
			/* Enable Forwarding */
			fputs_ex("/proc/sys/net/ipv4/ip_forward", "1");
#ifdef __CONFIG_IPV6__
			/* Enable IPv6 Forwarding */
			if (nvram_invmatch("ipv6_proto", ""))
				fputs_ex("/proc/sys/net/ipv6/conf/all/forwarding", "1");
#endif
		}

		/* 
		* Configure PPPoE connection. The PPPoE client will run 
		* ip-up/ip-down scripts upon link's connect/disconnect.
		*/
		if (strcmp(wan_proto, "pppoe") == 0 || strcmp(wan_proto, "pptp") == 0 ||
		    strcmp(wan_proto, "l2tp") == 0) 
		{
			int demand = atoi(nvram_safe_get(strcat_r(prefix, "pppoe_idletime", tmp))) &&
			    strcmp(wan_proto, "l2tp") /* L2TP does not support idling */;

			/* update demand option */
			nvram_set(strcat_r(prefix, "pppoe_demand", tmp), demand ? "1" : "0");

			/* Bring up WAN interface */
			ifconfig(wan_ifname, IFUP, 
				nvram_get(strcat_r(prefix, "pppoe_ipaddr", tmp)),
				nvram_get(strcat_r(prefix, "pppoe_netmask", tmp)));
#ifdef __CONFIG_IPV6__
			if (nvram_match("ipv6_proto", "native") || nvram_match("ipv6_proto", "dhcp6"))
				wan6_up(wan_ifname, unit);
#endif
			/* start firewall */
			start_firewall_ex(nvram_safe_get(strcat_r(prefix, "pppoe_ifname", tmp)),
				"0.0.0.0", "br0", nvram_safe_get("lan_ipaddr"));

		 	/* launch dhcp client and wait for lease forawhile */
		 	if (nvram_match(strcat_r(prefix, "pppoe_ipaddr", tmp), "0.0.0.0")) 
		 	{
				/* Start dhcp daemon */
				start_dhcpc(wan_ifname, unit);
		 	} else {
			 	/* setup static wan routes via physical device */
				add_routes("wan_", "route", wan_ifname);
				/* and set default route if specified with metric 1 */
				if (ip_addr(nvram_safe_get(strcat_r(prefix, "pppoe_gateway", tmp))) &&
				    !nvram_match("wan_heartbeat_x", ""))
					route_add(wan_ifname, 2, "0.0.0.0", 
						nvram_safe_get(strcat_r(prefix, "pppoe_gateway", tmp)), "0.0.0.0");
				/* start multicast router */
				if (nvram_match(strcat_r(prefix, "primary", tmp), "1"))
					start_igmpproxy(wan_ifname);
			}
			
			/* launch pppoe client daemon */
			start_pppd(prefix);

			/* ppp interface name is referenced from this point on */
			wan_ifname = nvram_safe_get(strcat_r(prefix, "pppoe_ifname", tmp));
			
			/* Pretend that the WAN interface is up */
			if (demand) 
			{
				if (!wait_for_ifup(prefix, wan_ifname, &ifr)) continue;
			}
			nvram_set("wan_ifname_t", wan_ifname);
		}
#endif /* ASUS_EXT */

#ifdef __CONFIG_MADWIMAX__
		else if (strcmp(wan_proto, "wimax") == 0) {
			// wait for usb-device initializing
			sleep(1);
			/* launch wimax daemon */
			start_wimax(prefix);

			/* wimax interface name is referenced from this point on */
			wan_ifname = nvram_safe_get(strcat_r(prefix, "wimax_ifname", tmp));
#ifdef ASUS_EXT
			nvram_set("wan_ifname_t", wan_ifname);
#endif
			continue;
		}
#endif /* __CONFIG_MADWIMAX__ */

		/* 
		* Configure DHCP connection. The DHCP client will run 
		* 'udhcpc.script bound'/'udhcpc.script deconfig' upon finishing IP address 
		* renew and release.
		*/
		else if (strcmp(wan_proto, "dhcp") == 0 ||
			 strcmp(wan_proto, "bigpond") == 0 )
		{
#ifdef __CONFIG_IPV6__
			if (nvram_match("ipv6_proto", "native") || nvram_match("ipv6_proto", "dhcp6"))
				wan6_up(wan_ifname, -1);
#endif
			/* Start firewall */
			start_firewall_ex(wan_ifname, "0.0.0.0", "br0", nvram_safe_get("lan_ipaddr"));
			/* (Re)start pre-authenticator */
			start_auth(prefix, 0);
			/* Start dhcp daemon */
			start_dhcpc(wan_ifname, unit);
			/* Update wan information for null DNS server */
			update_wan_status(1);
#ifdef ASUS_EXT
			wanmessage("Can not get IP from server");
			nvram_set("wan_ifname_t", wan_ifname);
#endif
		}
		/* Configure static IP connection. */
		else if (strcmp(wan_proto, "static") == 0 )
		{
			/* Assign static IP address to i/f */
			ifconfig(wan_ifname, IFUP,
				 nvram_safe_get(strcat_r(prefix, "ipaddr", tmp)),
				 nvram_safe_get(strcat_r(prefix, "netmask", tmp)));
			/* (Re)start pre-authenticator */
			start_auth(prefix, 0);
			/* We are done configuration */
			wan_up(wan_ifname);
#ifdef __CONFIG_IPV6__
			if (nvram_match("ipv6_proto", "native") || nvram_match("ipv6_proto", "dhcp6"))
				wan6_up(wan_ifname, -1);
#endif
#ifdef ASUS_EXT
			nvram_set("wan_ifname_t", wan_ifname);
#endif
		}
#ifdef __CONFIG_MODEM__
		/* 
		* Configure PPP connection. The PPP client will run 
		* ip-up/ip-down scripts upon link's connect/disconnect.
		*/
		else if (strcmp(wan_proto, "usbmodem") == 0 )
		{
			int demand = atoi(nvram_safe_get(strcat_r(prefix, "modem_idletime", tmp)));
			
			/* update demand option */
			nvram_set(strcat_r(prefix, "modem_demand", tmp), demand ? "1" : "0");

			/* Bring up WAN interface */
			ifconfig(wan_ifname, IFUP, 
				nvram_get(strcat_r(prefix, "ipaddr", tmp)),
				nvram_get(strcat_r(prefix, "netmask", tmp)));

			/* start firewall */
			start_firewall_ex(nvram_safe_get(strcat_r(prefix, "pppoe_ifname", tmp)),
				"0.0.0.0", "br0", nvram_safe_get("lan_ipaddr"));

			hotplug_sem_lock();
			nvram_set( strcat_r(prefix, "prepared", tmp), "1" );
			
			if ( nvram_match( strcat_r(prefix, "dial_enabled", tmp), "1" ) )
			{
				/* launch ppp client daemon */
				start_modem_dial(prefix);
			} else
			{
				demand=0;
				nvram_set( strcat_r(prefix, "dial_enabled", tmp), "1" );
			}
			hotplug_sem_unlock();

			/* ppp interface name is referenced from this point on */
			wan_ifname = nvram_safe_get(strcat_r(prefix, "pppoe_ifname", tmp));
			
			/* Pretend that the WAN interface is up */
			if (demand)
			{
				if (!wait_for_ifup(prefix, wan_ifname, &ifr)) continue;
			}
		}
#endif /* __CONFIG_MODEM__ */

#ifndef ASUS_EXT
		/* Start connection dependent firewall */
		start_firewall2(wan_ifname);
#endif

		dprintf("%s %s\n",
			nvram_safe_get(strcat_r(prefix, "ipaddr", tmp)),
			nvram_safe_get(strcat_r(prefix, "netmask", tmp)));
	} while (0);

	dprintf("done\n");
}

void start_wan(void)
{
	int unit;

	/* check if we need to setup WAN */
	if (nvram_match("router_disable", "1"))
		return;

#if defined(__CONFIG_MADWIMAX__) || defined( __CONFIG_MODEM__)
	hotplug_sem_open();
#endif

#ifndef ASUS_EXT
	/* start connection independent firewall */
	start_firewall();
#endif

	/* Create links */
	mkdir("/tmp/ppp", 0777);
	symlink("/sbin/rc", "/tmp/ppp/auth-up");
	symlink("/sbin/rc", "/tmp/ppp/auth-down");
	symlink("/sbin/rc", "/tmp/ppp/ip-up");
	symlink("/sbin/rc", "/tmp/ppp/ip-down");
#ifdef __CONFIG_IPV6__
	symlink("/sbin/rc", "/tmp/ppp/ipv6-up");
	symlink("/sbin/rc", "/tmp/ppp/ipv6-down");
	symlink("/sbin/rc", "/tmp/dhcp6c.script");
#endif
	symlink("/sbin/rc", "/tmp/udhcpc.script");
	symlink("/sbin/rc", "/tmp/zcip.script");
#ifdef __CONFIG_EAPOL__
	symlink("/sbin/rc", "/tmp/wpa_cli.script");
#endif

#ifdef __CONFIG_MODEM__
	/* ppp contents */
	mkdir("/tmp/ppp/peers", 0777);
#endif

	//symlink("/dev/null", "/tmp/ppp/connect-errors");

	/* Start each configured and enabled wan connection and its undelying i/f */
	for (unit = 0; unit < MAX_NVPARSE; unit ++)
	{
#ifdef ASUS_EXT // Only multiple pppoe is allowed 
		if (unit>0 && nvram_invmatch("wan_proto", "pppoe")) break;
#endif
		start_wan_unit(unit);
	}

#if defined(__CONFIG_MADWIMAX__) || defined( __CONFIG_MODEM__)
	hotplug_sem_close();
#endif

	dprintf("done\n");
}

void stop_wan_unit(int unit)
{
	char *wan_ifname;
	char *wan_proto;
	char tmp[100], prefix[sizeof("wanXXXXXXXXXX_")];

	do {
		snprintf(prefix, sizeof(prefix), "wan%d_", unit);

		/* make sure the connection exists and is enabled */ 
		wan_ifname = nvram_get(strcat_r(prefix, "ifname", tmp));
		if (!wan_ifname)
			continue;

		wan_proto = nvram_get(strcat_r(prefix, "proto", tmp));
		if (!wan_proto || !strcmp(wan_proto, "disabled"))
			continue;

		if (unit == wan_primary_ifunit())
		{
			stop_igmpproxy();
			killall("pppoe-relay");
		}

		/* Stop authenticator */
		stop_auth(prefix, 0);


#ifdef __CONFIG_MADWIMAX__
		/* Stop WiMAX connection */
		if (strcmp(wan_proto, "wimax") == 0)
		{
			stop_wimax(prefix);
			continue;
		}
#endif
#ifdef __CONFIG_MODEM__
		/* Stop USB modem */
		else if (strcmp(wan_proto, "usbmodem") == 0 )
		{
			nvram_unset(strcat_r(prefix, "prepared", tmp));
			stop_modem_dial(prefix);
			continue;
		}
#endif

		/* Stop VPN connection */
		else if (strcmp(wan_proto, "pppoe") == 0 || strcmp(wan_proto, "pptp") == 0 ||
			 strcmp(wan_proto, "l2tp") == 0)
		{
#ifdef __CONFIG_XL2TPD__
			killall("xl2tpd");
#else
			killall("l2tpd");
#endif
			sprintf(tmp, "/var/run/ppp%d.pid", unit);
			kill_pidfile(tmp);
			usleep(10000);

			if (nvram_match(strcat_r(prefix, "pppoe_ipaddr", tmp), "0.0.0.0"))
				goto stop_dhcp;
			else	goto stop_static;
		}

		/* Stop DHCP connection */
		else if (strcmp(wan_proto, "dhcp") == 0 ||
			 strcmp(wan_proto, "bigpond") == 0)
		{
#ifdef __CONFIG_IPV6__
			if (nvram_match("ipv6_proto", "native") || nvram_match("ipv6_proto", "dhcp6"))
				wan6_down(wan_ifname, unit);
#endif
		stop_dhcp:
			snprintf(tmp, sizeof(tmp), "/var/run/udhcpc%d.pid", unit);
			kill_pidfile_s(tmp, SIGUSR2);
			usleep(10000);
			kill_pidfile(tmp);
		}

		/* Stop Static */
		else if (strcmp(wan_proto, "static") == 0)
		{
#ifdef __CONFIG_IPV6__
			if (nvram_match("ipv6_proto", "native") || nvram_match("ipv6_proto", "dhcp6"))
				wan6_down(wan_ifname, unit);
#endif
		stop_static:
			wan_down(wan_ifname);
		}

	} while (0);

#ifdef ASUS_EXT
	update_wan_status(0);
#endif

	dprintf("done\n");
}

void stop_wan(void)
{
	char name[80], *next;
	int unit;

	/* Shutdown and kill all possible tasks */
	killall("auth-up");
	killall("auth-down");
	killall("ip-up");
	killall("ip-down");
#ifdef __CONFIG_IPV6__
	killall("ipv6-up");
	killall("ipv6-down");
#endif

	/* Stop each configured and enabled wan connection and its undelying i/f */
	for (unit = 0; unit < MAX_NVPARSE; unit ++)
	{
#ifdef ASUS_EXT // Only multiple pppoe is allowed 
		if (unit>0 && nvram_invmatch("wan_proto", "pppoe")) break;
#endif
		stop_wan_unit(unit);
	}

	/* Bring down WAN interfaces */
	foreach(name, nvram_safe_get("wan_ifnames"), next)
	{
		ifconfig(name, 0, NULL, NULL);
	}

	/* Remove dynamically created links */
#ifdef __CONFIG_EAPOL__
	unlink("/tmp/wpa_cli.script");
#endif
	unlink("/tmp/udhcpc.script");
	unlink("/tmp/zcip.script");
#ifdef __CONFIG_IPV6__
	unlink("/tmp/dhcp6c.script");
	unlink("/tmp/ppp/ipv6-up");
	unlink("/tmp/ppp/ipv6-down");
#endif
	unlink("/tmp/ppp/ip-up");
	unlink("/tmp/ppp/ip-down");
	unlink("/tmp/ppp/auth-up");
	unlink("/tmp/ppp/auth-down");
	rmdir("/tmp/ppp");

	dprintf("done\n");
}

int update_resolvconf(const char *ifname, int metric, int up)
{
	FILE *fp;
	char word[100], *next;

	dprintf("%s %d %d\n", ifname, metric, up);

	/* check if auto dns enabled */
	if (nvram_invmatch("wan_dnsenable_x", "1"))
		return 0;

	if (!(fp = fopen("/tmp/resolv.conf", "w+"))) {
		perror("/tmp/resolv.conf");
		return errno;
	}

	foreach(word, (*nvram_safe_get("wan0_dns") ? nvram_safe_get("wan0_dns") :
		nvram_safe_get("wanx_dns")), next)
	{
		fprintf(fp, "nameserver %s\n", word);
		dprintf( "nameserver %s\n", word );
	}

#ifdef __CONFIG_IPV6__
	if (nvram_invmatch("ipv6_proto", ""))
	{
		foreach(word, nvram_safe_get("wan0_ipv6_dns"), next)
		{
			fprintf(fp, "nameserver %s\n", word);
			dprintf( "nameserver %s\n", word );
		}
	}
#endif

	fclose(fp);

	/* Notify dnsmasq of change */
	killall_s("dnsmasq", SIGHUP);

	return 0;
}

void wan_up(const char *wan_ifname)
{
	char tmp[100], prefix[sizeof("wanXXXXXXXXXX_")];
	char *wan_proto, *gateway;
	int unit, metric;

#ifdef DEBUG
	int r;
	r = wan_prefix(wan_ifname, prefix);
	dprintf("wan_up: %s %s %d\n", wan_ifname, prefix, r);
#endif

	/* Figure out nvram variable name prefix for this i/f */
	unit = wan_prefix(wan_ifname, prefix);
	if (unit < 0)
	{
		/* TODO: add multiwan support */
		unit = 0;
		sprintf(prefix, "wan%d_", unit);

		/* called for dhcp+ppp */
		if (!nvram_match(strcat_r(prefix, "ifname", tmp), wan_ifname))
			return;

		/* re-start firewall with old ppp address or 0.0.0.0 */
		start_firewall_ex(
			nvram_safe_get(strcat_r(prefix, "pppoe_ifname", tmp)),
			nvram_safe_get(strcat_r(prefix, "ipaddr", tmp)),
			"br0", nvram_safe_get("lan_ipaddr"));

	 	/* setup static wan routes via physical device */
		add_routes("wan_", "route", wan_ifname);
		/* and one supplied via DHCP */
		add_wanx_routes("wanx_", wan_ifname, 0);
		
		gateway = ip_addr(nvram_safe_get("wan_gateway")) != INADDR_ANY ?
			nvram_get("wan_gateway") : nvram_safe_get("wanx_gateway");

		/* and default route with metric 1 */
		if (ip_addr(gateway) != INADDR_ANY)
		{
			char word[100], *next;

			route_add(wan_ifname, 2, "0.0.0.0", gateway, "0.0.0.0");

			/* ... and to dns servers as well for demand ppp to work */
			if (nvram_match("wan_dnsenable_x", "1"))
				foreach(word, nvram_safe_get("wanx_dns"), next) 
			{
				in_addr_t mask = inet_addr(nvram_safe_get("wanx_netmask"));
				if ((inet_addr(word) & mask) != (inet_addr(nvram_safe_get("wanx_ipaddr")) & mask))
					route_add(wan_ifname, 2, word, gateway, "255.255.255.255");
			}
		}

		/* start multicast router */
		if (nvram_match(strcat_r(prefix, "primary", tmp), "1"))
			start_igmpproxy(wan_ifname);

		update_resolvconf(wan_ifname, 2, 1);

		return;
	}

	wan_proto = nvram_safe_get(strcat_r(prefix, "proto", tmp));

	dprintf("unit %d %s %s\n", unit, wan_ifname, wan_proto);
	metric = atoi(nvram_safe_get(strcat_r(prefix, "priority", tmp)));

	/* Set default route to gateway if specified */
	if (nvram_match(strcat_r(prefix, "primary", tmp), "1")) 
	{
		if (strcmp(wan_proto, "dhcp") == 0 || 
		    strcmp(wan_proto, "static") == 0 
#ifdef __CONFIG_MADWIMAX__
		 || strcmp(wan_proto, "wimax") == 0
#endif
		) {
			/* the gateway is in the local network */
			route_add(wan_ifname, 0, nvram_safe_get(strcat_r(prefix, "gateway", tmp)),
				NULL, "255.255.255.255");
		}
		/* default route via default gateway */
		
		dprintf("metric %s - %d\n",nvram_safe_get(strcat_r(prefix, "priority", tmp)),metric);
		route_add(wan_ifname, metric, "0.0.0.0", 
			nvram_safe_get(strcat_r(prefix, "gateway", tmp)), "0.0.0.0");
		/* hack: avoid routing cycles, when both peer and server has the same IP */
		if (strcmp(wan_proto, "pptp") == 0 || strcmp(wan_proto, "l2tp") == 0) {
			/* delete gateway route as it's no longer needed */
			route_del(wan_ifname, 0, nvram_safe_get(strcat_r(prefix, "gateway", tmp)),
				"0.0.0.0", "255.255.255.255");
		}
	}

	/* Install interface dependent static routes */
	add_wan_routes(wan_ifname);

 	/* setup static wan routes via physical device */
	if (    strcmp(wan_proto, "dhcp") == 0 
	     || strcmp(wan_proto, "static") == 0 
#ifdef __CONFIG_MADWIMAX__
	     || strcmp(wan_proto, "wimax") == 0 
#endif
	) {
		nvram_set("wanx_gateway", nvram_safe_get(strcat_r(prefix, "gateway", tmp)));
		add_routes("wan_", "route", wan_ifname);
	}

	/* and one supplied via DHCP */
	if (    strcmp(wan_proto, "dhcp") == 0 
#ifdef __CONFIG_MADWIMAX__
	     || strcmp(wan_proto, "wimax") == 0 
#endif
	) {
		add_wanx_routes(prefix, wan_ifname, 0);
	}

#ifdef __CONFIG_IPV6__
	if (nvram_invmatch("ipv6_proto", "native") && nvram_invmatch("ipv6_proto", "dhcp6"))
		wan6_up(wan_ifname, -1);
#endif

	/* Add dns servers to resolv.conf */
	update_resolvconf(wan_ifname, metric, 1 );

	/* Sync time */
	//start_ntpc();

	/* (Re)start post-authenticator */
	start_auth(prefix, 1);

#ifdef ASUS_EXT
	update_wan_status(1);
	start_firewall_ex(wan_ifname, nvram_safe_get(strcat_r(prefix, "ipaddr", tmp)), "br0", nvram_safe_get("lan_ipaddr"));
	start_ddns(0);
	//stop_upnp();
	start_upnp();
#endif /* ASUS_EXT */
	
#ifdef QOS
	// start qos related 
	start_qos(nvram_safe_get(strcat_r(prefix, "ipaddr", tmp)));
#endif

	/* start multicast router */
	if (nvram_match(strcat_r(prefix, "primary", tmp), "1") &&
	      (strcmp(wan_proto, "dhcp") == 0
	    || strcmp(wan_proto, "bigpond") == 0
	    || strcmp(wan_proto, "static") == 0 
#ifdef __CONFIG_MADWIMAX__
	    || strcmp(wan_proto, "wimax") == 0
#endif
	)) {
		start_igmpproxy(wan_ifname);
	}

	dprintf("done\n");
}

void wan_down(const char *wan_ifname)
{
	char tmp[100], prefix[sizeof("wanXXXXXXXXXX_")];
	char *wan_proto;
	int metric;

	/* Figure out nvram variable name prefix for this i/f */
	if (wan_prefix(wan_ifname, prefix) < 0)
		return;

	wan_proto = nvram_safe_get(strcat_r(prefix, "proto", tmp));
	
	dprintf("%s %s\n", wan_ifname, wan_proto);

	/* Stop authenticator */
	stop_auth(prefix, 1);

#ifdef __CONFIG_IPV6__
	if (nvram_invmatch("ipv6_proto", "native") && nvram_invmatch("ipv6_proto", "dhcp6"))
		wan6_down(wan_ifname, -1);
#endif

	/* Remove default route to gateway if specified */
	metric = atoi(nvram_safe_get(strcat_r(prefix, "priority", tmp)));
	if (nvram_match(strcat_r(prefix, "primary", tmp), "1"))
		route_del(wan_ifname, metric, "0.0.0.0", 
			nvram_safe_get(strcat_r(prefix, "gateway", tmp)),
			"0.0.0.0");

	/* Remove interface dependent static routes */
	del_wan_routes(wan_ifname);

	/* Update resolv.conf -- leave as is if no dns servers left for demand to work */
	if (*nvram_safe_get("wanx_dns"))
		nvram_unset(strcat_r(prefix, "dns", tmp));
	update_resolvconf(wan_ifname, metric, 0);

#ifdef ASUS_EXT
	update_wan_status(0);
#endif

	dprintf("done\n");
}

#ifdef __CONFIG_IPV6__
void wan6_up(const char *wan_ifname, int unit)
{
	char tmp[100], prefix[sizeof("wanXXXXXXXXXX_")];
	const char *wan6_ifname = "six0";
	char *wan6_ipaddr;
	struct in6_addr addr;
	struct in_addr addr4;
	char addrstr[INET6_ADDRSTRLEN];
	int size, addr4masklen = 0;

	if (!nvram_invmatch("ipv6_proto", ""))
		return;

	/* Figure out nvram variable name prefix for this i/f */
	if (unit >= 0)
		sprintf(prefix, "wan%d_", unit);
	else
	if (wan_prefix(wan_ifname, prefix) < 0)
		return;

	addr4.s_addr = ip_addr(nvram_safe_get(strcat_r(prefix, "ipaddr", tmp)));

	/* Configure tunnel 6in4 & 6to4 & 6rd */
	if (nvram_match("ipv6_proto", "tun6in4") ||
	    nvram_match("ipv6_proto", "tun6to4") ||
	    nvram_match("ipv6_proto", "tun6rd"))
	{
		/* Instantiate tunnel */
		eval("ip", "tunnel", "add", wan6_ifname, "mode", "sit",
			"remote", nvram_match("ipv6_proto", "tun6in4") ? nvram_safe_get("ipv6_sit_remote") : "any",
			"local", inet_ntoa(addr4),
			"ttl", nvram_safe_get("ipv6_sit_ttl"));
		/* Chage local address if any */
		eval("ip", "tunnel", "change", wan6_ifname, "mode", "sit",
			"local", inet_ntoa(addr4));
#ifdef LINUX26
		if (nvram_match("ipv6_proto", "tun6rd"))
		{
			char netstr[sizeof("255.255.255.255/32")] = "0.0.0.0/0";

			size = ipv6_addr(nvram_safe_get(strcat_r(prefix, "ipv6_addr", tmp)), &addr);
			ipv6_network(&addr, size);
			inet_ntop(AF_INET6, &addr, addrstr, INET6_ADDRSTRLEN);
			sprintf(addrstr, "%s/%d", addrstr, size);

			addr4masklen = atoi(nvram_safe_get(strcat_r(prefix, "ipv6_ip4size", tmp)));
			if (addr4masklen > 0 && addr4masklen <= 32)
			{
				struct in_addr net;
				net.s_addr = addr4.s_addr &
				    htonl(0xffffffffUL << (32 - addr4masklen));
				sprintf(netstr, "%s/%d", inet_ntoa(net), addr4masklen);
			}

			eval("ip", "tunnel", "6rd", "dev", wan6_ifname,
				"6rd-prefix", addrstr,
				"6rd-relay_prefix", netstr);
		}
#endif /* LINUX26 */
		/* Set MTU value and enable tunnel */
		eval("ip", "link", "set", "mtu", nvram_safe_get("ipv6_sit_mtu"), "dev", wan6_ifname, "up");
	} else
		wan6_ifname = wan_ifname;

	/* Prepare WAN IPv6 address */
	size = ipv6_addr(nvram_safe_get(strcat_r(prefix, "ipv6_addr", tmp)), &addr);
	if (nvram_match("ipv6_proto", "tun6to4"))
	{
		addr.s6_addr16[0] = htons(0x2002);
		size = 16;
		ipv6_map6rd(&addr, size, &addr4, 0);
	} else
	if (nvram_match("ipv6_proto", "tun6rd"))
	{
//TODO: implement 6RD prefix copy into wan_addr
		ipv6_map6rd(&addr, size, &addr4, addr4masklen);
	}
	inet_ntop(AF_INET6, &addr, addrstr, INET6_ADDRSTRLEN);
	if (size > 0)
		sprintf(addrstr, "%s/%d", addrstr, size);

	/* Check if WAN address changed */
	wan6_ipaddr = nvram_safe_get(strcat_r(prefix, "ipv6_addr_t", tmp));
	if (strcmp(addrstr, wan6_ipaddr) != 0)
	{
		/* Delete old 6to4 address and route */
		if (*wan6_ipaddr && (
		    nvram_match("ipv6_proto", "tun6to4") ||
		    nvram_match("ipv6_proto", "tun6rd")))
			eval("ip", "-6", "addr", "del", wan6_ipaddr, "dev", wan6_ifname);
    		nvram_set(strcat_r(prefix, "ipv6_addr_t", tmp), addrstr);
	}

	/* Configure WAN IPv6 address */
	if (nvram_match("ipv6_proto", "dhcp6"))
		start_dhcp6c(wan6_ifname);
	else
		eval("ip", "-6", "addr", "add", addrstr, "dev", wan6_ifname);

	/* Configure WAN IPv6 specific routes */
	if (nvram_match("ipv6_proto", "tun6to4") ||
	    nvram_match("ipv6_proto", "tun6rd"))
	{
		sprintf(addrstr, "::%s", nvram_safe_get(strcat_r(prefix, "ipv6_relay", tmp)));
		wan6_ipaddr = addrstr;

	} else {
		wan6_ipaddr = nvram_safe_get(strcat_r(prefix, "ipv6_router", tmp));
		if (*wan6_ipaddr)
			eval("ip", "-6", "route", "add", wan6_ipaddr, "dev", wan6_ifname);
	}

	/* Configure WAN IPv6 default gateway */
	if (*wan6_ipaddr)
		eval("ip", "-6", "route", "add", "default", "via", wan6_ipaddr, "metric", "1");
	else {
		char name[64];

		/* Enable stateless autoconfiguration */
		sprintf(name, "/proc/sys/net/ipv6/conf/%s/accept_ra", wan6_ifname);
		fputs_ex(name, "2");
		sprintf(name, "/proc/sys/net/ipv6/conf/%s/forwarding", wan6_ifname);
		fputs_ex(name, "2");
	}

	/* Reconfigure LAN IPv6 address */
	if (nvram_match("ipv6_proto", "tun6to4") ||
	    nvram_match("ipv6_proto", "tun6rd"))
	{
		char *lan6_ifname = nvram_safe_get("lan_ifname");
		char *lan6_ipaddr = nvram_safe_get("lan_ipv6_addr");

		ipv6_addr(nvram_safe_get("ipv6_lan_addr"), &addr);
		if (nvram_match("ipv6_proto", "tun6to4"))
		{
			addr.s6_addr16[0] = htons(0x2002);
			ipv6_map6rd(&addr, 16, &addr4, 0);
    			inet_ntop(AF_INET6, &addr, addrstr, INET6_ADDRSTRLEN);
    			size = atoi(nvram_safe_get("ipv6_lan_netsize"));
    		} else {
//TODO: implement 6RD prefix copy into wan_addr
			struct in6_addr wan6_addr;
			size = ipv6_addr(nvram_safe_get(strcat_r(prefix, "ipv6_addr", tmp)), &wan6_addr);
			ipv6_map6rd(&addr, size, &addr4, addr4masklen);
			inet_ntop(AF_INET6, &addr, addrstr, INET6_ADDRSTRLEN);
			size = atoi(nvram_safe_get("ipv6_lan_netsize"));
		}
		if (size > 0)
			sprintf(addrstr, "%s/%d", addrstr, size);

		/*  Check if LAN address changed */
		if (strcmp(addrstr, lan6_ipaddr) != 0)
		{
			/* Delete old 6to4 address and route */
			if (*lan6_ipaddr)
				eval("ip", "-6", "addr", "del", lan6_ipaddr, "dev", lan6_ifname);
			nvram_set("lan_ipv6_addr", addrstr);
		}
		eval("ip", "-6", "addr", "add", addrstr, "dev", lan6_ifname);

		/* Notify radvd of change */
		if (nvram_match("ipv6_radvd_enable", "1"))
			killall_s("radvd", SIGHUP);
	}

	/* Configure IPv6 DNS servers */
	nvram_set(strcat_r(prefix, "ipv6_dns", tmp), nvram_safe_get("ipv6_dns1_x"));
}

void wan6_down(const char *wan_ifname, int unit)
{
	char tmp[100], prefix[sizeof("wanXXXXXXXXXX_")];
	const char *wan6_ifname = "six0";
	char *wan6_ipaddr;

	if (!nvram_invmatch("ipv6_proto", ""))
		return;

	/* Figure out nvram variable name prefix for this i/f */
	if (unit >= 0)
		sprintf(prefix, "wan%d_", unit);
	else
	if (wan_prefix(wan_ifname, prefix) < 0)
		return;

	if (nvram_match("ipv6_proto", "tun6in4") ||
	    nvram_match("ipv6_proto", "tun6to4") ||
	    nvram_match("ipv6_proto", "tun6rd"))
	{
		/* Disable tunnel */
		eval("ip", "link", "set", "dev", wan6_ifname, "down");
	} else
		wan6_ifname = wan_ifname;

	//if (nvram_match("ipv6_proto", "dhcp6"))
		stop_dhcp6c();

	/* Delete WAN IPv6 specific routes */
	if (nvram_invmatch("ipv6_proto", "tun6to4") &&
	    nvram_invmatch("ipv6_proto", "tun6rd"))
	{
		wan6_ipaddr = nvram_safe_get(strcat_r(prefix, "ipv6_router", tmp));
		if (*wan6_ipaddr)
			eval("ip", "-6", "route", "del", wan6_ipaddr, "dev", wan6_ifname);
	}

	/* Delete WAN IPv6 default gateway */
	eval("ip", "-6", "route", "del", "default", "metric", "1");

        /* Delete WAN address */
	wan6_ipaddr = nvram_safe_get(strcat_r(prefix, "ipv6_addr_t", tmp));
	if (*wan6_ipaddr)
		eval("ip", "-6", "addr", "del", wan6_ipaddr, "dev", wan6_ifname);
	nvram_unset(strcat_r(prefix, "ipv6_addr_t", tmp));

	/* Delete IPv6 DNS servers */
	nvram_unset(strcat_r(prefix, "ipv6_dns", tmp));

	dprintf("done\n");

}
#endif /* __CONFIG_IPV6__ */

#ifdef ASUS_EXT
static void lan_up(const char *lan_ifname)
{
	FILE *fp;
	char word[100], *next;

	/* Set default route to gateway if specified */
	route_add(lan_ifname, 0, "0.0.0.0", 
			nvram_safe_get("lan_gateway"),
			"0.0.0.0");

	/* Open resolv.conf to read */
	if (!(fp = fopen("/tmp/resolv.conf", "w"))) {
		perror("/tmp/resolv.conf");
		return;
	}

	/*if (nvram_invmatch("lan_gateway", ""))
		fprintf(fp, "nameserver %s\n", nvram_safe_get("lan_gateway"));*/

	foreach(word, nvram_safe_get("lan_dns"), next)
	{
		fprintf(fp, "nameserver %s\n", word);
	}

#ifdef __CONFIG_IPV6__
	if (nvram_invmatch("ipv6_proto", "") && nvram_invmatch("ipv6_dns1_x", ""))
 	{
 		fprintf(fp, "nameserver %s\n", nvram_safe_get("ipv6_dns1_x"));
	}
#endif

	fclose(fp);

	/* Notify dnsmasq of change */
	killall_s("dnsmasq", SIGHUP);

	/* Sync time */
	//start_ntpc();
}

#if 0
void lan_down(const char *lan_ifname)
{
	/* Remove default route to gateway if specified */
	route_del(lan_ifname, 0, "0.0.0.0", 
			nvram_safe_get("lan_gateway"),
			"0.0.0.0");

	/* remove resolv.conf */
	unlink("/tmp/resolv.conf");
}
#endif

void lan_up_ex(const char *lan_ifname)
{
	FILE *fp;
	char word[100], *next;

	/* Set default route to gateway if specified */
	route_add(lan_ifname, 0, "0.0.0.0", 
			nvram_safe_get("lan_gateway_t"),
			"0.0.0.0");

	/* Open resolv.conf to read */
	if (!(fp = fopen("/tmp/resolv.conf", "w"))) {
		perror("/tmp/resolv.conf");
		return;
	}

	/*if (nvram_invmatch("lan_gateway_t", ""))
		fprintf(fp, "nameserver %s\n", nvram_safe_get("lan_gateway_t"))*/;

	foreach(word, nvram_safe_get("lan_dns_t"), next)
	{
		fprintf(fp, "nameserver %s\n", word);
	}

#ifdef __CONFIG_IPV6__
	if (nvram_invmatch("ipv6_proto", "") && nvram_invmatch("ipv6_dns1_x", ""))
 	{
 		fprintf(fp, "nameserver %s\n", nvram_safe_get("ipv6_dns1_x"));
	}
#endif

	fclose(fp);

	/* Notify dnsmasq of change */
	killall_s("dnsmasq", SIGHUP);

	/* Sync time */
	//start_ntpc();
	//update_lan_status(1);
}

void lan_down_ex(const char *lan_ifname)
{
	/* Remove default route to gateway if specified */
	route_del(lan_ifname, 0, "0.0.0.0", 
			nvram_safe_get("lan_gateway_t"),
			"0.0.0.0");

	/* remove resolv.conf */
	unlink("/tmp/resolv.conf");

	update_lan_status(0);
}
#endif /* ASUS_EXT */

static int notify_nas(const char *type, const char *ifname, const char *action)
{
#ifdef __CONFIG_BCMWL5__

	/* Inform driver to send up new WDS link event */
	if (wl_iovar_setint(ifname, "wds_enable", 1)) {
		dprintf("%s: set wds_enable failed\n", ifname);
	}
	return 1;

#else   /* __CONFIG_BCMWL5__ */
	char *nas_argv[] = {"nas4not", type, ifname, action, 
			NULL,	/* role */
			NULL,	/* crypto */
			NULL,	/* auth */
			NULL,	/* passphrase */
			NULL,	/* ssid */
			NULL};
	char *str = NULL;
	int retries = 10;
	char tmp[100], prefix[sizeof("wanXXXXXXXXXX_")];
	int unit;
	char remote[ETHER_ADDR_LEN];
	char ssid[48], pass[80], auth[16], crypto[16], role[8];
	int i;

	/* the wireless interface must be configured to run NAS */
	wl_ioctl(ifname, WLC_GET_INSTANCE, &unit, sizeof(unit));
	snprintf(prefix, sizeof(prefix), "wl%d_", unit);
#ifdef WPA2_WMM
	if (nvram_match(strcat_r(prefix, "akm", tmp), "") &&
	    nvram_match(strcat_r(prefix, "auth_mode", tmp), "none"))
#else
	if (nvram_match(strcat_r(prefix, "auth_mode", tmp), "open") ||
	    nvram_match(strcat_r(prefix, "auth_mode", tmp), "shared"))
#endif
		return 0;

	/* find WDS link configuration */
	wl_ioctl(ifname, WLC_WDS_GET_REMOTE_HWADDR, remote, ETHER_ADDR_LEN);
	for (i = 0; i < MAX_NVPARSE; i ++) {
		char mac[ETHER_ADDR_STR_LEN];
		uint8 ea[ETHER_ADDR_LEN];

		if (get_wds_wsec(unit, i, mac, role, crypto, auth, ssid, pass) &&
		    ether_atoe(mac, ea) && !memcmp(ea, remote, ETHER_ADDR_LEN)) {
			nas_argv[4] = role;
			nas_argv[5] = crypto;
			nas_argv[6] = auth;
			nas_argv[7] = pass;
			nas_argv[8] = ssid;
			break;
		}
	}

	/* did not find WDS link configuration, use wireless' */
	if (i == MAX_NVPARSE) {
		/* role */
		nas_argv[4] = "auto";
		/* crypto */
		nas_argv[5] = nvram_safe_get(strcat_r(prefix, "crypto", tmp));
		/* auth mode */
#ifdef WPA2_WMM
		nas_argv[6] = nvram_safe_get(strcat_r(prefix, "akm", tmp));
#else
		nas_argv[6] = nvram_safe_get(strcat_r(prefix, "auth_mode", tmp));
#endif
		/* passphrase */
		nas_argv[7] = nvram_safe_get(strcat_r(prefix, "wpa_psk", tmp));
		/* ssid */
		nas_argv[8] = nvram_safe_get(strcat_r(prefix, "ssid", tmp));
	}

	/* wait till nas is started */
	while (retries -- > 0 && !(str = file2str("/tmp/nas.lan.pid")))
		sleep(1);
	if (str) {
		int pid;
		free(str);
		return _eval(nas_argv, ">/dev/console", 0, &pid);
	}
	return -1;
#endif /* !__CONFIG_BCMWL5__ */
}

#ifdef __CONFIG_BCMWL5__
void start_wl(void)
{
	char ifname[64], *ifnext;
	char *lan_ifname = nvram_safe_get("lan_ifname");

	if (strncmp(lan_ifname, "br", 2) == 0) {
		foreach(ifname, nvram_safe_get("lan_ifnames"), ifnext) {
			eval("wlconf", ifname, "start"); /* start wl iface */
                }
	}
	else if (strcmp(lan_ifname, "")) {
		/* specific non-bridged lan iface */
		eval("wlconf", lan_ifname, "start");
	}
}
#endif // __CONFIG_BCMWL5__

int hotplug_net(void)
{
	char *lan_ifname = nvram_safe_get("lan_ifname");
	char *interface, *action;

	if (!(interface = getenv("INTERFACE")) ||
	    !(action = getenv("ACTION")))
		return EINVAL;

	if (strncmp(interface, "wds", 3))
		return 0;

#ifdef LINUX26
	if (!strcmp(action, "add")) {
#else
	if (!strcmp(action, "register")) {
#endif
		/* Bring up the interface and add to the bridge */
		ifconfig(interface, IFUP, NULL, NULL);

#ifdef __CONFIG_EMF__
                if (nvram_match("emf_enable", "1")) {
                        eval("emf", "add", "iface", lan_ifname, interface);
                        emf_mfdb_update(lan_ifname, interface, TRUE);
                        emf_uffp_update(lan_ifname, interface, TRUE);
                        emf_rtport_update(lan_ifname, interface, TRUE);
                }
#endif

		/* Bridge WDS interfaces */
		if (!strncmp(lan_ifname, "br", 2) && 
		    eval("brctl", "addif", lan_ifname, interface))
		    return 0;

		/* Notify NAS of adding the interface */
		notify_nas("lan", interface, "up");
	}
	return 0;
}

int preset_wan_routes(const char *wan_ifname)
{
	char tmp[100], prefix[sizeof("wanXXXXXXXXXX_")];

	/* Figure out nvram variable name prefix for this i/f */
	if (wan_prefix(wan_ifname, prefix) < 0)
		return -1;

	/* Set default route to gateway if specified */
	if (nvram_match(strcat_r(prefix, "primary", tmp), "1"))
	{
		route_add(wan_ifname, 0, "0.0.0.0", "0.0.0.0", "0.0.0.0");
	}

	/* Install interface dependent static routes */
	add_wan_routes(wan_ifname);
	return 0;
}

int wan_primary_ifunit(void)
{
	int unit;
	
	for (unit = 0; unit < MAX_NVPARSE; unit ++) {
		char tmp[100], prefix[sizeof("wanXXXXXXXXXX_")];

		snprintf(prefix, sizeof(prefix), "wan%d_", unit);
		if (nvram_match(strcat_r(prefix, "primary", tmp), "1"))
			return unit;
	}

	return 0;
}

// return 0 if failed
static int wait_for_ifup(const char *prefix, const char *wan_ifname, struct ifreq *ifr)
{
	int timeout = 5;
	int s, pid;
	char tmp[200];

	char *ping_argv[] = { "ping", "-c1", "", NULL};

	/* Wait for pppx to be created */
	while (ifconfig(wan_ifname, IFUP, NULL, NULL) && timeout--)
		sleep(1);

	/* Retrieve IP info */
	if ((s = socket(AF_INET, SOCK_RAW, IPPROTO_RAW)) < 0)
		return 0;
	strncpy(ifr->ifr_name, wan_ifname, IFNAMSIZ);

	/* Set temporary IP address */
	if (ioctl(s, SIOCGIFADDR, ifr) < 0)
		perror(wan_ifname);
	nvram_set(strcat_r(prefix, "ipaddr", tmp), inet_ntoa(sin_addr(&ifr->ifr_addr)));
	nvram_set(strcat_r(prefix, "netmask", tmp), "255.255.255.255");

	/* Set temporary P-t-P address */
	if (ioctl(s, SIOCGIFDSTADDR, ifr) < 0)
		perror(wan_ifname);
	nvram_set(strcat_r(prefix, "gateway", tmp), inet_ntoa(sin_addr(&ifr->ifr_dstaddr)));

	close(s);

	/* 
	* Preset routes so that traffic can be sent to proper pppx even before 
	* the link is brought up.
	*/

	preset_wan_routes(wan_ifname);

	/* Stimulate link up */
	ping_argv[2] = nvram_safe_get(strcat_r(prefix, "gateway", tmp));
	_eval(ping_argv, NULL, 0, &pid);
	return 1;
}

#if defined(__CONFIG_MADWIMAX__) || defined(__CONFIG_MODEM__)
void hotplug_network_device(const char *interface, const char *action, const char *product, const char *device)
{
	char *wan_ifname;
	char *wan_proto;
	char *dev_vidpid;
	int unit;
	int found=0;
	char tmp[100], prefix[sizeof("wanXXXXXXXXXX_")];
	char str_devusb[100];

	dprintf( "%s %s %s", interface, action, product );

	int action_add = (strcmp(action, "add") == 0);

	snprintf(str_devusb, sizeof(str_devusb), "%s : %s", product, device);

	hotplug_sem_open();
	hotplug_sem_lock();

	// prevent multiple processing of the same device
	for (unit = 0; !found && unit < MAX_NVPARSE; unit ++)
	{
		snprintf(prefix, sizeof(prefix), "wan%d_", unit);
		dev_vidpid = nvram_get( strcat_r(prefix, "usb_device", tmp) );
		if (dev_vidpid)
			found = (strcmp(dev_vidpid, str_devusb) == 0);
		if (found) dprintf("already processed\n");
	}

	/* Start each configured and enabled wan connection and its undelying i/f */
	if (!found) for (unit = 0; unit < MAX_NVPARSE; unit ++) 
	{
		snprintf(prefix, sizeof(prefix), "wan%d_", unit);

		/* make sure the connection exists and is enabled */ 
		wan_ifname = nvram_get(strcat_r(prefix, "ifname", tmp));
		if (!wan_ifname)
			continue;

		wan_proto = nvram_get(strcat_r(prefix, "proto", tmp));
		if (!wan_proto || !strcmp(wan_proto, "disabled"))
			continue;

		dprintf("%s %s \n\n\n\n\n", wan_ifname, wan_proto);

		if (!found) {
			if (action_add) {
#ifdef __CONFIG_MADWIMAX__
			    if ( !strcmp(wan_proto, "wimax") &&
			         hotplug_check_wimax(interface, product, prefix) ) {
				found = 1;
			    } else 
#endif
#ifdef __CONFIG_MODEM__
			    if ( !strcmp(wan_proto, "usbmodem") &&
			         hotplug_check_modem(interface, product, device, prefix) ) {
				found = 2;
			    }
#else
			    {}
#endif
			} else {
				dev_vidpid = nvram_get(strcat_r(prefix, "usb_device", tmp));
				if (dev_vidpid)
					found = (strcmp(dev_vidpid, str_devusb) == 0);
			}
		}
		if (found)
		{

		    if (action_add)
		    {
			dev_vidpid = nvram_get( strcat_r(prefix, "usb_device", tmp) );
			if ( !dev_vidpid || !*dev_vidpid ) 
			{
#ifdef __CONFIG_MADWIMAX__
			    if ( found==1 && strcmp(wan_proto, "wimax") == 0 )
			    {
				nvram_set(strcat_r(prefix, "usb_device", tmp), str_devusb );
#ifdef HOTPLUG_DEV_START
				start_wimax( prefix );
#endif
			    } else
#endif
#ifdef __CONFIG_MODEM__
			    if ( found==2 && strcmp(wan_proto, "usbmodem") == 0 )
			    {
				nvram_set(strcat_r(prefix, "usb_device", tmp), str_devusb );
#ifdef HOTPLUG_DEV_START
				// now starts from hotplug usb-serial
				//usb_modem_check(prefix);
#endif
			    }
#else
			    {}
#endif
			}
		    } else if (found) {
#ifdef __CONFIG_MADWIMAX__
			if ( strcmp(wan_proto, "wimax") == 0 )
			{
			    nvram_unset(strcat_r(prefix, "usb_device", tmp) );
			    nvram_unset(strcat_r(prefix, "usb_device_name", tmp) );
			} else
#endif
#ifdef __CONFIG_MODEM__
			if ( strcmp(wan_proto, "usbmodem") == 0 )
			{
			    nvram_unset(strcat_r(prefix, "usb_device", tmp) );
			    nvram_unset(strcat_r(prefix, "usb_device_name", tmp) );
			    stop_modem_dial(prefix);
			}
#else
			{}
#endif
		    }

		    break;
		}
	}
	hotplug_sem_unlock();
	hotplug_sem_close();

	dprintf("done");
};
#endif /* defined(__CONFIG_MADWIMAX__) || defined(__CONFIG_MODEM__) */
