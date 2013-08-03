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
	int err;
	char word[80], *next;
	char *ipaddr, *netmask, *gateway, *metric;
	char tmp[100];

	foreach(word, nvram_safe_get(strcat_r(prefix, var, tmp)), next)
	{
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
			gateway = nvram_safe_get(strcat_r(prefix, "xgateway", tmp));

		dprintf("=> ");
		if ((err = route_add(ifname, atoi(metric) + 1, ipaddr, gateway, netmask))) {
			logmessage("route", "add failed(%d) '%s': %s dst %s mask %s gw %s",
			err, strerror(err),
			ifname, ipaddr, netmask, gateway);
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

		if (gateway && ip_addr(ipaddr) != INADDR_ANY) {
			dprintf("=> ");
			route_add(ifname, metric + 1, ipaddr, gateway, netmask);
		}
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
			dprintf("=> ");
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

	foreach(word, nvram_safe_get(strcat_r(prefix, var, tmp)), next)
	{
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

		/* TODO: use gateway from prefix? */
		if (ip_addr(gateway) == INADDR_ANY) 
			gateway = nvram_safe_get("wan0_xgateway");

		dprintf("=> ");
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

	if (nvram_get_int("udpxy_enable_x"))
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
			char tmp[100], prefix[WAN_PREFIX_SZ];
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

	/* set the packet size */
	if (nvram_match("jumbo_frame_enable", "1"))
		eval("et", "robowr", "0x40", "0x05", nvram_safe_get("jumbo_frame_size"));

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
	    nvram_invmatch("ipv6_proto", "tun6to4") &&
	    nvram_invmatch("ipv6_proto", "tun6rd") &&
	    !nvram_get_int("ipv6_lanauto_x")) {
		struct in6_addr addr;
		char addrstr[INET6_ADDRSTRLEN];
		int size;

		size = ipv6_addr(nvram_safe_get("lan_ipv6_addr"), &addr);
		inet_ntop(AF_INET6, &addr, addrstr, INET6_ADDRSTRLEN);
		if (size > 0)
			sprintf(addrstr, "%s/%d", addrstr, size);
		eval("ip", "-6", "addr", "add", addrstr, "dev", lan_ifname);
		nvram_set("lan_ipv6_addr", addrstr);

#ifdef __CONFIG_RADVD__
		/* Notify radvd of change */
		if (nvram_get_int("ipv6_radvd_enable"))
			killall_s("radvd", SIGHUP);
#endif
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

int _wan_proto(const char *prefix, char *buffer)
{
	char *wan_proto = nvram_get(strcat_r(prefix, "proto", buffer));

	if (wan_proto && *wan_proto) {
		if (strcmp(wan_proto, "static") == 0)
			return WAN_STATIC;
		else if (strcmp(wan_proto, "dhcp") == 0)
			return WAN_DHCP;
		else if (strcmp(wan_proto, "pppoe") == 0)
			return WAN_PPPOE;
		else if (strcmp(wan_proto, "pptp") == 0)
			return WAN_PPTP;
		else if (strcmp(wan_proto, "l2tp") == 0)
			return WAN_L2TP;
		else if (strcmp(wan_proto, "bigpond") == 0)
			return WAN_BIGPOND;
#ifdef __CONFIG_MADWIMAX__
		else if (strcmp(wan_proto, "wimax") == 0)
			return WAN_WIMAX;
#endif
#ifdef __CONFIG_MODEM__
		else if (strcmp(wan_proto, "usbmodem") == 0)
			return WAN_USBMODEM;
#endif
#ifdef __CONFIG_USBNET__
		else if (strcmp(wan_proto, "usbnet") == 0)
			return WAN_USBNET;
#endif
	}
	return -1;
}

int wan_proto(const char *prefix)
{
	char tmp[100];

	return _wan_proto(prefix, tmp);
}

#if defined(__CONFIG_USBNET__)
void prepare_wan_unit(int unit)
{
	char tmp[100], prefix[WAN_PREFIX_SZ];
	char wan_ifname[10];
	int wan_proto;
	char name[80], *next;
	int found = 0;

	snprintf(prefix, sizeof(prefix), "wan%d_", unit);

	wan_proto = _wan_proto(prefix, tmp);

	dprintf("unit: %d, proto: %d\n", unit, wan_proto);

	if (wan_proto != WAN_USBNET)
		return;

	snprintf(wan_ifname, sizeof(wan_ifname), "wan%d", unit);

	nvram_set(strcat_r(prefix, "ifname", tmp), wan_ifname);
	nvram_set(strcat_r(prefix, "ifnames", tmp), wan_ifname);

	foreach(name, nvram_safe_get("wan_ifnames"), next)
		if (!strcmp(wan_ifname, name))
			found = 1;

	if (!found) {
		sprintf(tmp, "%s %s", nvram_safe_get("wan_ifnames"), wan_ifname );
		nvram_set("wan_ifnames", tmp);
	}

	eval("brctl", "addbr", wan_ifname, "stp", "0" );
	ifconfig(wan_ifname, IFUP, NULL, NULL);

	usbnet_load_drivers(prefix);

	dprintf("done\n");
}
#endif

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
	else for (unit = 0; unit < MAX_NVPARSE; unit ++) {
		sprintf(prefix, "wan%d_", unit);
		if (!nvram_match(strcat_r(prefix, "ifname", tmp), wan_ifname))
			continue;

		switch (_wan_proto(prefix, tmp)) {
		case WAN_DHCP:
		case WAN_BIGPOND:
		case WAN_STATIC:
#ifdef __CONFIG_USBNET__
		case WAN_USBNET:
#endif
			return unit;
		}
	}
	return -1;

 found:
	sprintf(prefix, "wan%d_", unit);
	return unit;
}

int wanx_prefix(const char *wan_ifname, char *prefix)
{
	int unit, xunit = -1;
	char tmp[100];

	if ((unit = ppp_ifunit(wan_ifname)) >= 0)
		goto found;
#ifdef __CONFIG_MADWIMAX__
	else if ((unit = wimax_ifunit(wan_ifname)) >= 0)
		goto found;
#endif
	else for (unit = 0; unit < MAX_NVPARSE; unit ++) {
		sprintf(prefix, "wan%d_", unit);
		if (!nvram_match(strcat_r(prefix, "ifname", tmp), wan_ifname))
			continue;

		switch (_wan_proto(prefix, tmp)) {
		case WAN_DHCP:
		case WAN_BIGPOND:
		case WAN_STATIC:
#ifdef __CONFIG_USBNET__
		case WAN_USBNET:
#endif
			return unit;
		}
		if (xunit < 0)
			xunit = unit;
	}
	if (xunit >= 0)
		sprintf(prefix, "wan%d_x", xunit);
	return xunit;

 found:
	sprintf(prefix, "wan%d_", unit);
	return unit;
}

int wans_prefix(const char *wan_ifname, char *prefix, char *xprefix)
{
	int unit;

	if ((unit = wanx_prefix(wan_ifname, xprefix)) >= 0)
		sprintf(prefix, "wan%d_", unit);
	return unit;
}

static int add_wan_routes(const char *wan_ifname)
{
	char prefix[WAN_PREFIX_SZ];

	/* Figure out nvram variable name prefix for this i/f */
	if (wan_prefix(wan_ifname, prefix) < 0)
		return -1;

	return add_routes(prefix, "route", wan_ifname);
}

static int del_wan_routes(const char *wan_ifname)
{
	char prefix[WAN_PREFIX_SZ];

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
	int wan_proto;
	char tmp[100], prefix[WAN_PREFIX_SZ];
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

		wan_proto = _wan_proto(prefix, tmp);
		if (wan_proto < 0)
			continue;
#ifdef DEBUG
		dprintf("%s %s\n\n\n\n\n",
			wan_ifname, nvram_safe_get(strcat_r(prefix, "proto", tmp)));
#endif
		/* disable the connection if the i/f is not in wan_ifnames */
		if (!wan_valid(wan_ifname)) {
			nvram_set(strcat_r(prefix, "proto", tmp), "disabled");
			continue;
		}

		switch (wan_proto) {
		/* proto which doesn't need ethernet */
#if defined(__CONFIG_MADWIMAX__)
		case WAN_WIMAX: break;
#endif
#if defined(__CONFIG_USBNET__)
		case WAN_USBNET: break;
#endif
		default:
		{
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
		if (memcmp(eabuf, ifr.ifr_hwaddr.sa_data, ETHER_ADDR_LEN)) {
			/* current hardware address is different than user specified */
			ifconfig(wan_ifname, 0, NULL, NULL);
		}

		/* Configure i/f only once, specially for wireless i/f shared by multiple connections */
		if (ioctl(s, SIOCGIFFLAGS, &ifr)) {
			close(s);
			continue;
		} else if (!(ifr.ifr_flags & IFF_UP) ) {
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
			} else {
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
		if (nvram_match(strcat_r(prefix, "primary", tmp), "1")) {
			setup_ethernet(wan_ifname);
			start_pppoe_relay(wan_ifname);
		}
#endif
		} /* default */
		} /* switch (wan_proto) */

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
#endif /* ASUS_EXT */

		switch (wan_proto) {
		/* 
		* Configure PPPoE connection. The PPPoE client will run 
		* ip-up/ip-down scripts upon link's connect/disconnect.
		*/
		case WAN_PPPOE:
		case WAN_PPTP:
		case WAN_L2TP:
		{
			char *ipaddr = nvram_get(strcat_r(prefix, "pppoe_ipaddr", tmp));
			char *netmask = nvram_get(strcat_r(prefix, "pppoe_netmask", tmp));
			int demand = nvram_get_int(strcat_r(prefix, "pppoe_idletime", tmp)) &&
				wan_proto != WAN_L2TP /* L2TP does not support idling */;

			/* update demand option */
			nvram_set(strcat_r(prefix, "pppoe_demand", tmp), demand ? "1" : "0");

			/* Bring up WAN interface */
			ifconfig(wan_ifname, IFUP, ipaddr, netmask);

#ifdef __CONFIG_IPV6__
			/* Start IPv6 on MAN */
			if (nvram_get_int(strcat_r(prefix, "ipv6_if_x", tmp)) &&
			    (nvram_match("ipv6_proto", "native") ||
			     nvram_match("ipv6_proto", "dhcp6")))
				wan6_up(wan_ifname, unit);
#endif
			/* start firewall */
			start_firewall_ex(nvram_safe_get(strcat_r(prefix, "pppoe_ifname", tmp)),
				"0.0.0.0", "br0", nvram_safe_get("lan_ipaddr"));

			/* launch dhcp client and wait for lease forawhile */
			if (ipaddr && ip_addr(ipaddr) == INADDR_ANY) {
				/* Start dhcp daemon */
				start_dhcpc(wan_ifname, unit);
			} else {
				/* We are done configuration */
				wan_up(wan_ifname);
			}

			/* launch pppoe client daemon */
			start_pppd(prefix);

			/* ppp interface name is referenced from this point on */
			wan_ifname = nvram_safe_get(strcat_r(prefix, "pppoe_ifname", tmp));
			
			/* Pretend that the WAN interface is up */
			if (demand) {
				if (!wait_for_ifup(prefix, wan_ifname, &ifr)) continue;
			}
#ifdef ASUS_EXT
			nvram_set("wan_ifname_t", wan_ifname);
#endif
			/* fall through to dhcp/static, firewall2 */
			break;
		}

#ifdef __CONFIG_MADWIMAX__
		case WAN_WIMAX:
		{
			/* wait for usb-device initializing */
			sleep(1);
			/* launch wimax daemon */
			start_wimax(prefix);

			/* wimax interface name is referenced from this point on */
			wan_ifname = nvram_safe_get(strcat_r(prefix, "wimax_ifname", tmp));
#ifdef ASUS_EXT
			nvram_set("wan_ifname_t", wan_ifname);
#endif
			/* why not fall through to dhcp/static, firewall2? */
			continue;
		}
#endif /* __CONFIG_MADWIMAX__ */

#ifdef __CONFIG_USBNET__
		/* Start previously manually disconnected USB interface */
		case WAN_USBNET:
		{
			//char *usb_ifname = nvram_safe_get(strcat_r(prefix, "usb_ifname", tmp));

			//if (strncmp(usb_ifname, "eth", 3) != 0 &&
			//    strncmp(usb_ifname, "usb", 3) != 0)
			//	continue;

			/* override wan_proto */
			wan_proto = nvram_match(strcat_r(prefix, "pppoe_ipaddr", tmp),
				"0.0.0.0") ? WAN_DHCP : WAN_STATIC;
			/* fall through to dhcp/static, firewall2 */

			usbnet_connect(prefix, unit, wan_ifname);

			break;
		}
#endif // __CONFIG_USBNET__

#ifdef __CONFIG_MODEM__
		/* 
		* Configure PPP connection. The PPP client will run 
		* ip-up/ip-down scripts upon link's connect/disconnect.
		*/
		case WAN_USBMODEM:
		{
			int demand = nvram_get_int(strcat_r(prefix, "modem_idletime", tmp));
			
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
			/* fall through to dhcp/static, firewall2 */
			break;
		}
#endif /* __CONFIG_MODEM__ */
		} /* switch(wan_proto) */

		/*
		 * Configure DHCP connection. The DHCP client will run 
		 * 'udhcpc.script bound'/'udhcpc.script deconfig' upon finishing IP address 
		 * renew and release.
		 */
		switch (wan_proto) {
		case WAN_DHCP:
		case WAN_BIGPOND:
		{
#ifdef __CONFIG_IPV6__
			/* Start IPv6 */
			if (nvram_match("ipv6_proto", "native") ||
			    nvram_match("ipv6_proto", "dhcp6"))
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
			break;
		}

		/* Configure static IP connection. */
		case WAN_STATIC:
		{
			/* Assign static IP address to i/f */
			ifconfig(wan_ifname, IFUP,
				 nvram_safe_get(strcat_r(prefix, "ipaddr", tmp)),
				 nvram_safe_get(strcat_r(prefix, "netmask", tmp)));
#ifdef __CONFIG_IPV6__
			/* Start IPv6 */
			if (nvram_match("ipv6_proto", "native") ||
			    nvram_match("ipv6_proto", "dhcp6"))
				wan6_up(wan_ifname, -1);
#endif
			/* (Re)start pre-authenticator */
			start_auth(prefix, 0);
			/* We are done configuration */
			wan_up(wan_ifname);
#ifdef ASUS_EXT
			nvram_set("wan_ifname_t", wan_ifname);
#endif
			break;
		}
		} /* switch(wan_proto) */

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
#ifdef PPPD_AUTH_UNUSED
	symlink("/sbin/rc", "/tmp/ppp/auth-up");
	symlink("/sbin/rc", "/tmp/ppp/auth-down");
	symlink("/sbin/rc", "/tmp/ppp/auth-fail");
#endif
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
#if defined(__CONFIG_USBNET__)
		prepare_wan_unit(unit);
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
	char tmp[100], prefix[WAN_PREFIX_SZ];
	int dynamic_ip = 0;

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
		else
#endif
#ifdef __CONFIG_MODEM__
		/* Stop USB modem */
		if (strcmp(wan_proto, "usbmodem") == 0 )
		{
			nvram_unset(strcat_r(prefix, "prepared", tmp));
			stop_modem_dial(prefix);
			continue;
		}
		else
#endif
#ifdef __CONFIG_USBNET__
		/* Stop Ethernet over USB */
		if (strcmp(wan_proto, "usbnet") == 0 ) {
			dynamic_ip = nvram_match(strcat_r(prefix, "pppoe_ipaddr", tmp), "0.0.0.0");
			usbnet_disconnect(prefix, unit, wan_ifname);
		}
		else
#endif
		/* Stop VPN connection */
		if (strcmp(wan_proto, "pppoe") == 0 ||
		    strcmp(wan_proto, "pptp") == 0 ||
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

			dynamic_ip = nvram_match(strcat_r(prefix, "pppoe_ipaddr", tmp), "0.0.0.0");
		}

		/* Stop DHCP connection */
		else if (strcmp(wan_proto, "dhcp") == 0 ||
			 strcmp(wan_proto, "bigpond") == 0)
		{
#ifdef __CONFIG_IPV6__
			if (nvram_match("ipv6_proto", "native") ||
			    nvram_match("ipv6_proto", "dhcp6"))
				wan6_down(wan_ifname, unit);
#endif
			dynamic_ip = 1;
		}

		/* Stop Static */
		else if (strcmp(wan_proto, "static") == 0)
		{
#ifdef __CONFIG_IPV6__
			if (nvram_match("ipv6_proto", "native") ||
			    nvram_match("ipv6_proto", "dhcp6"))
				wan6_down(wan_ifname, unit);
#endif
		}

		if (dynamic_ip != 0) {
			snprintf(tmp, sizeof(tmp), "/var/run/udhcpc%d.pid", unit);
			kill_pidfile_s(tmp, SIGUSR2);
			usleep(10000);
			kill_pidfile(tmp);
		} else {
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
#ifdef PPPD_AUTH_UNUSED
	killall("auth-up");
	killall("auth-down");
	killall("auth-fail");
#endif
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
#ifdef PPPD_AUTH_UNUSED
	unlink("/tmp/ppp/auth-up");
	unlink("/tmp/ppp/auth-down");
	unlink("/tmp/ppp/auth-fail");
#endif
	rmdir("/tmp/ppp");

	dprintf("done\n");
}

/* TODO: Incompatible with multiwan yet */
int update_resolvconf(const char *ifname, int metric, int up)
{
	FILE *fp;
	char word[100], *next;
	char tmp[100], prefix[WAN_PREFIX_SZ];
	int unit;

	/* Figure out nvram variable name prefix for this i/f */
	unit = wans_prefix(ifname, prefix, tmp);
	dprintf("%s unit %d prefix %s metric %d up %d\n",
		ifname, unit, prefix, metric, up);
	if (unit < 0)
		return 0;

	/* Check if auto dns enabled */
	if (nvram_invmatch("wan_dnsenable_x", "1")
#ifdef __CONFIG_IPV6__
	&& !nvram_invmatch("ipv6_proto", "")
#endif
	)
		return 0;

	/* Metric can be obtained directly, do we need it?
	 * metric = nvram_get_int(strcat_r(wanprefix, "priority", tmp)); */

	if (!(fp = fopen("/tmp/resolv.conf", "w+"))) {
		perror("/tmp/resolv.conf");
		return errno;
	}

#ifdef __CONFIG_IPV6__
	if (nvram_invmatch("wan_dnsenable_x", "1")) {
		if (nvram_invmatch("wan_dns1_x",""))
			fprintf(fp, "nameserver %s\n", nvram_safe_get("wan_dns1_x"));
		if (nvram_invmatch("wan_dns2_x",""))
			fprintf(fp, "nameserver %s\n", nvram_safe_get("wan_dns2_x"));
	} else
#endif
	foreach(word, (*nvram_safe_get(strcat_r(prefix, "dns", tmp)) ?
	    nvram_safe_get(tmp) : nvram_safe_get(strcat_r(prefix, "xdns", tmp))), next) {
		fprintf(fp, "nameserver %s\n", word);
	}

#ifdef __CONFIG_IPV6__
	if (nvram_invmatch("wan_dnsenable_x", "1") ||
	    nvram_invmatch(strcat_r(prefix, "dns", tmp), "")) {
		foreach(word, nvram_safe_get(strcat_r(prefix, "ipv6_dns", tmp)), next) {
			fprintf(fp, "nameserver %s\n", word);
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
	char tmp[100], prefix[WAN_PREFIX_SZ];
	char xprefix[WAN_PREFIX_SZ];
	char *gateway;
	int unit, metric, wan_proto;

	/* Figure out nvram variable name prefix for this i/f */
	unit = wans_prefix(wan_ifname, prefix, xprefix);
	dprintf("%s unit %d prefix %s %s\n", wan_ifname, unit, prefix, xprefix);
	if (unit < 0)
		return;

	wan_proto = _wan_proto(prefix, tmp);
	metric = nvram_get_int(strcat_r(prefix, "priority", tmp));
	dprintf("%s unit %d proto %d metric %d\n", wan_ifname, unit, wan_proto, metric);

	/* Called for Static/DHCP+PPP, there should be no wanN_xifname */
	if (!nvram_get(strcat_r(xprefix, "ifname", tmp)))
	{
		/* re-start firewall with old ppp address or 0.0.0.0 */
		start_firewall_ex(
			nvram_safe_get(strcat_r(prefix, "pppoe_ifname", tmp)),
			nvram_safe_get(strcat_r(prefix, "ipaddr", tmp)),
			"br0", nvram_safe_get("lan_ipaddr"));

		/* setup static wan routes via physical device */
		add_routes(prefix, "mroute", wan_ifname);
		/* and one supplied via DHCP */
		add_wanx_routes(xprefix, wan_ifname, 0); /* why not 2 ? */

		gateway = nvram_safe_get(strcat_r(xprefix, "gateway", tmp));

		/* and default route with metric 1 */
		if (ip_addr(gateway) != INADDR_ANY)
		{
			char word[100], *next;
			in_addr_t addr = inet_addr(nvram_safe_get(strcat_r(xprefix, "ipaddr", tmp)));
			in_addr_t mask = inet_addr(nvram_safe_get(strcat_r(xprefix, "netmask", tmp)));

			if ((inet_addr(gateway) & mask) != (addr & mask))
			{
				/* the gateway is out of the local network */
				dprintf("=> ");
				route_add(wan_ifname, 2, gateway, NULL, "255.255.255.255");
			}

			/* default route via default gateway */
			dprintf("=> ");
			route_add(wan_ifname, 2, "0.0.0.0", gateway, "0.0.0.0");

			/* ... and to dns servers as well for demand ppp to work */
			if (nvram_match("wan_dnsenable_x", "1"))
				foreach(word, nvram_safe_get(strcat_r(xprefix, "dns", tmp)), next) 
			{
				if ((inet_addr(word) != inet_addr(gateway)) &&
				    (inet_addr(word) & mask) != (addr & mask))
				{
					dprintf("=> ");
					route_add(wan_ifname, 2, word, gateway, "255.255.255.255");
				}
			}
		}

		/* start multicast router */
		if (nvram_match(strcat_r(prefix, "primary", tmp), "1"))
			start_igmpproxy(wan_ifname);

		update_resolvconf(wan_ifname, metric, 1);

		return;
	}

	gateway = nvram_safe_get(strcat_r(prefix, "gateway", tmp));
	if (ip_addr(gateway) == INADDR_ANY)
		gateway = NULL;

	/* Set default route to gateway if specified */
	if (nvram_match(strcat_r(prefix, "primary", tmp), "1"))
	{

		switch (wan_proto) {
		case WAN_DHCP:
		case WAN_BIGPOND:
		case WAN_STATIC:
#ifdef __CONFIG_MADWIMAX__
		case WAN_WIMAX:
#endif
#ifdef __CONFIG_USBNET__
		case WAN_USBNET:
#endif
			/* the gateway is in the local network */
			if (gateway == NULL)
				break;
			dprintf("=> ");
			route_add(wan_ifname, 0, gateway, NULL, "255.255.255.255");
			/* fall through */
		default:
			/* default route via default gateway */
			dprintf("=> ");
			route_add(wan_ifname, metric, "0.0.0.0", gateway, "0.0.0.0");
		}

		/* hack: avoid routing cycles, when both peer and server has the same IP */
		switch (wan_proto) {
		case WAN_PPTP:
		case WAN_L2TP:
			/* delete gateway route as it's no longer needed */
			if (gateway == NULL)
				break;
			dprintf("=> ");
			route_del(wan_ifname, 0, gateway, "0.0.0.0", "255.255.255.255");
		}
	}

	/* Install interface dependent static routes */
	add_wan_routes(wan_ifname);

	/* Setup static wan routes via physical device */
	switch (wan_proto) {
	case WAN_DHCP:
	case WAN_BIGPOND:
	case WAN_STATIC:
#ifdef __CONFIG_MADWIMAX__
	case WAN_WIMAX:
#endif
#ifdef __CONFIG_USBNET__
	case WAN_USBNET:
#endif
		nvram_set(strcat_r(prefix, "xgateway", tmp), gateway ? : "0.0.0.0");
		add_routes(prefix, "mroute", wan_ifname);
	}

	/* Setup routes supplied via DHCP */
	switch (wan_proto) {
	case WAN_DHCP:
	case WAN_BIGPOND:
#ifdef __CONFIG_MADWIMAX__
	case WAN_WIMAX:
#endif
#ifdef __CONFIG_USBNET__
	case WAN_USBNET:
#endif
		add_wanx_routes(prefix, wan_ifname, 0);
	}

#ifdef __CONFIG_IPV6__
	/* Start IPv6 */
	if (nvram_invmatch("ipv6_proto", "") &&
	    nvram_invmatch("ipv6_proto", "native") &&
	    nvram_invmatch("ipv6_proto", "dhcp6"))
		wan6_up(wan_ifname, -1);
#endif

	/* Add dns servers to resolv.conf */
	update_resolvconf(wan_ifname, metric, 1);

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

	/* Start multicast router */
	switch (wan_proto) {
	case WAN_DHCP:
	case WAN_BIGPOND:
	case WAN_STATIC:
#ifdef __CONFIG_MADWIMAX__
	case WAN_WIMAX:
#endif
#ifdef __CONFIG_USBNET__
	case WAN_USBNET:
#endif
		if (!nvram_match(strcat_r(prefix, "primary", tmp), "1"))
			break;
		start_igmpproxy(wan_ifname);
	}

	dprintf("done\n");
}

void wan_down(const char *wan_ifname)
{
	char tmp[100], prefix[WAN_PREFIX_SZ];
	int unit, metric;

	/* Figure out nvram variable name prefix for this i/f */
	unit = wan_prefix(wan_ifname, prefix);
	dprintf("%s unit %d prefix %s\n", wan_ifname, unit, prefix);
	if (unit < 0)
		return;

	metric = nvram_get_int(strcat_r(prefix, "priority", tmp));
	dprintf("%s unit %d proto %d metric %d\n", wan_ifname, unit, _wan_proto(prefix, tmp), metric);

	/* Stop authenticator */
	stop_auth(prefix, 1);

#ifdef __CONFIG_IPV6__
	if (nvram_invmatch("ipv6_proto", "") &&
	    nvram_invmatch("ipv6_proto", "native") &&
	    nvram_invmatch("ipv6_proto", "dhcp6"))
		wan6_down(wan_ifname, -1);
#endif

	/* Remove default route to gateway if specified */
	if (nvram_match(strcat_r(prefix, "primary", tmp), "1")) {
		char *gateway = nvram_safe_get(strcat_r(prefix, "gateway", tmp));

		if (ip_addr(gateway) == INADDR_ANY)
			gateway = NULL;
		dprintf("=> ");
		route_del(wan_ifname, metric, "0.0.0.0", gateway, "0.0.0.0");
	}

	/* Remove interface dependent static routes */
	del_wan_routes(wan_ifname);

	/* Update resolv.conf -- leave as is if no dns servers left for demand to work */
	if (*nvram_safe_get(strcat_r(prefix, "xdns", tmp)))
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
	char tmp[100], prefix[WAN_PREFIX_SZ];
	const char *wan6_ifname = wan_ifname;
	char *wan6_ipaddr, *wan6_gateway;
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

	size = ipv6_addr(nvram_safe_get(strcat_r(prefix, "ipv6_addr", tmp)), &addr);
	addr4.s_addr = ip_addr(nvram_safe_get(strcat_r(prefix, "ipaddr", tmp)));

	/* Configure tunnel 6in4 & 6to4 & 6rd */
	if (nvram_match("ipv6_proto", "tun6in4") ||
	    nvram_match("ipv6_proto", "tun6to4") ||
	    nvram_match("ipv6_proto", "tun6rd"))
	{
		char netstr[sizeof("255.255.255.255/32")] = "0.0.0.0/0";

		wan6_ifname = "six0";

		if (nvram_match("ipv6_proto", "tun6in4")) {
			sprintf(addrstr, "::/0");
		} else
		if (nvram_match("ipv6_proto", "tun6to4")) {
			addr4masklen = 0;
			size = 16;

			addr.s6_addr16[0] = htons(0x2002);
			ipv6_network(&addr, size);
			inet_ntop(AF_INET6, &addr, addrstr, INET6_ADDRSTRLEN);
			sprintf(addrstr, "%s/%d", addrstr, size);

			addr4masklen = 0;

			ipv6_map6rd(&addr, size, &addr4, 0);
			addr.s6_addr16[7] |= htons(0x0001);
		} else
		if (nvram_match("ipv6_proto", "tun6rd")) {
			addr4masklen = nvram_get_int(strcat_r(prefix, "ipv6_ip4size", tmp));
			if (addr4masklen < 0 || addr4masklen > 32)
				addr4masklen = 0;

			ipv6_network(&addr, size);
			inet_ntop(AF_INET6, &addr, addrstr, INET6_ADDRSTRLEN);
			sprintf(addrstr, "%s/%d", addrstr, size);

			if (addr4masklen) {
				struct in_addr net;
				net.s_addr = addr4.s_addr &
				    htonl(0xffffffffUL << (32 - addr4masklen));
				sprintf(netstr, "%s/%d", inet_ntoa(net), addr4masklen);
			}

			ipv6_map6rd(&addr, size, &addr4, addr4masklen);
			addr.s6_addr16[7] |= htons(0x0001);
		}

		/* Instantiate tunnel */
		eval("ip", "tunnel", "add", wan6_ifname, "mode", "sit",
			"remote", nvram_match("ipv6_proto", "tun6in4") ? nvram_safe_get("ipv6_sit_remote") : "any",
			"local", inet_ntoa(addr4),
			"ttl", nvram_safe_get("ipv6_sit_ttl"));

		/* Chage local address if any */
		eval("ip", "tunnel", "change", wan6_ifname, "mode", "sit",
			"local", inet_ntoa(addr4));

		/* Setup domain */
		eval("ip", "tunnel", "6rd", "dev", wan6_ifname,
			"6rd-prefix", addrstr, "6rd-relay_prefix", netstr);

		/* Set MTU value and enable tunnel */
		eval("ip", "link", "set", "mtu", nvram_safe_get("ipv6_sit_mtu"), "dev", wan6_ifname, "up");
	}

	/* Configure WAN IPv6 address */
	if (nvram_match("ipv6_proto", "tun6to4") ||
	    nvram_match("ipv6_proto", "tun6rd") ||
	    !nvram_get_int("ipv6_wanauto_x")) {
		inet_ntop(AF_INET6, &addr, addrstr, INET6_ADDRSTRLEN);
		if (size > 0)
			sprintf(addrstr, "%s/%d", addrstr, size);

		/* Delete old WAN IPv6 address, if changed */
		wan6_ipaddr = nvram_safe_get(strcat_r(prefix, "ipv6_addr_t", tmp));
		if (*wan6_ipaddr && strcmp(addrstr, wan6_ipaddr) != 0)
			eval("ip", "-6", "addr", "del", wan6_ipaddr, "dev", wan6_ifname);

		/* Configure WAN IPv6 address */
		eval("ip", "-6", "addr", "add", addrstr, "dev", wan6_ifname);
		nvram_set(strcat_r(prefix, "ipv6_addr_t", tmp), addrstr);

		/* Configure WAN IPv6 specific routes */
		if (nvram_match("ipv6_proto", "tun6to4") ||
		    nvram_match("ipv6_proto", "tun6rd")) {
			sprintf(addrstr, "::%s", nvram_safe_get(strcat_r(prefix, "ipv6_relay", tmp)));
			wan6_gateway = addrstr;
		} else {
			wan6_gateway = nvram_safe_get(strcat_r(prefix, "ipv6_router", tmp));
			if (*wan6_gateway)
				eval("ip", "-6", "route", "add", wan6_gateway, "dev", wan6_ifname);
		}
	} else
		wan6_gateway = "";

	/* Configure WAN IPv6 default gateway */
	if (*wan6_gateway) {
		eval("ip", "-6", "route", "add", "default", "via", wan6_gateway, "metric", "1");
		nvram_set(strcat_r(prefix, "ipv6_router_t", tmp), wan6_gateway);
	} else {
		char name[64];

		/* Enable stateless autoconfiguration */
		sprintf(name, "/proc/sys/net/ipv6/conf/%s/accept_ra", wan6_ifname);
		fputs_ex(name, "2");
		sprintf(name, "/proc/sys/net/ipv6/conf/%s/forwarding", wan6_ifname);
		fputs_ex(name, "2");
	}

	/* Add IPv6 DNS servers */
	if (nvram_invmatch("ipv6_dnsenable_x", "1")) {
		char dnsstr[INET6_ADDRSTRLEN*3+3] = "";
		if (nvram_invmatch("ipv6_dns1_x", ""))
			snprintf(dnsstr, sizeof(dnsstr), "%s%s \n", dnsstr, nvram_safe_get("ipv6_dns1_x"));
		if (nvram_invmatch("ipv6_dns2_x", ""))
			snprintf(dnsstr, sizeof(dnsstr), "%s%s \n", dnsstr, nvram_safe_get("ipv6_dns2_x"));
		if (nvram_invmatch("ipv6_dns3_x", ""))
			snprintf(dnsstr, sizeof(dnsstr), "%s%s \n", dnsstr, nvram_safe_get("ipv6_dns3_x"));
		nvram_set(strcat_r(prefix, "ipv6_dns", tmp), dnsstr);
	}

	/* Start DHCPv6 client */
	if (nvram_match("ipv6_proto", "dhcp6"))
		start_dhcp6c(wan6_ifname);
	else
	/* (Re)configure LAN IPv6 address */
	if (nvram_match("ipv6_proto", "tun6to4") ||
	    nvram_match("ipv6_proto", "tun6rd")) {
		char *lan6_ifname = nvram_safe_get("lan_ifname");
		char *lan6_ipaddr;

		size += (32 - addr4masklen);
		if (size < 64)
			size = 64;

		inet_ntop(AF_INET6, &addr, addrstr, INET6_ADDRSTRLEN);
		sprintf(addrstr, "%s/%d", addrstr, size);

		/* Delete old LAN IPv6 address, if changed */
		lan6_ipaddr = nvram_safe_get("lan_ipv6_addr");
		if (*lan6_ipaddr && strcmp(addrstr, lan6_ipaddr) != 0)
			eval("ip", "-6", "addr", "del", lan6_ipaddr, "dev", lan6_ifname);

		/* Configure LAN IPv6 address */
		eval("ip", "-6", "addr", "add", addrstr, "dev", lan6_ifname);
		nvram_set("lan_ipv6_addr", addrstr);

#ifdef __CONFIG_RADVD__
		/* Notify radvd of change */
		if (nvram_get_int("ipv6_radvd_enable"))
			killall_s("radvd", SIGHUP);
#endif
	}
}

void wan6_down(const char *wan_ifname, int unit)
{
	char tmp[100], prefix[WAN_PREFIX_SZ];
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

	/* Delete WAN IPv6 default gateway */
	eval("ip", "-6", "route", "del", "default", "metric", "1");

	/* Delete WAN IPv6 specific routes */
	wan6_ipaddr = nvram_safe_get(strcat_r(prefix, "ipv6_router_t", tmp));
	if (*wan6_ipaddr)
		eval("ip", "-6", "route", "del", wan6_ipaddr, "dev", wan6_ifname);
	nvram_unset(strcat_r(prefix, "ipv6_router_t", tmp));

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
	dprintf("=> ");
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
// TODO: is it still requred?
	if (nvram_invmatch("ipv6_proto", "") &&
	    nvram_invmatch("ipv6_dnsenable_x", "1")) {
		if (nvram_invmatch("ipv6_dns1_x", ""))
			fprintf(fp, "nameserver %s\n", nvram_safe_get("ipv6_dns1_x"));
		if (nvram_invmatch("ipv6_dns2_x", ""))
			fprintf(fp, "nameserver %s\n", nvram_safe_get("ipv6_dns2_x"));
		if (nvram_invmatch("ipv6_dns3_x", ""))
			fprintf(fp, "nameserver %s\n", nvram_safe_get("ipv6_dns3_x"));
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
	dprintf("=> ");
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
	dprintf("=> ");
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
// TODO: is it still requred?
	if (nvram_invmatch("ipv6_proto", "") &&
	    nvram_invmatch("ipv6_dnsenable_x", "1")) {
		if (nvram_invmatch("ipv6_dns1_x", ""))
			fprintf(fp, "nameserver %s\n", nvram_safe_get("ipv6_dns1_x"));
		if (nvram_invmatch("ipv6_dns2_x", ""))
			fprintf(fp, "nameserver %s\n", nvram_safe_get("ipv6_dns2_x"));
		if (nvram_invmatch("ipv6_dns3_x", ""))
			fprintf(fp, "nameserver %s\n", nvram_safe_get("ipv6_dns3_x"));
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
	dprintf("=> ");
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
	char tmp[100], prefix[WAN_PREFIX_SZ];
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

	dprintf("%s %s\n", interface, action);

#ifdef __CONFIG_USBNET__
	if (!strncmp(interface, "usb", 3) || !strncmp(interface, "eth", 3)) {

		if (!strcmp(action, "add") || !strcmp(action, "remove")) {
			usbnet_check_and_act(interface, action);
		}

		return 0;
	}
#endif

	if (strncmp(interface, "wds", 3))
		return 0;

	if (!strcmp(action, "add")) {
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
	char tmp[100], prefix[WAN_PREFIX_SZ];

	/* Figure out nvram variable name prefix for this i/f */
	if (wan_prefix(wan_ifname, prefix) < 0)
		return -1;

	/* Set default route to gateway if specified */
	if (nvram_match(strcat_r(prefix, "primary", tmp), "1"))
	{
		dprintf("=> ");
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
		char tmp[100], prefix[WAN_PREFIX_SZ];

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

#if defined(__CONFIG_MADWIMAX__) || defined(__CONFIG_MODEM__) || defined(__CONFIG_USBNET__)
void hotplug_network_device(const char *interface, const char *action, const char *product, const char *device)
{
	int wan_proto;
	char *dev_vidpid;
	int unit;
	int found = 0;
	char tmp[100], prefix[WAN_PREFIX_SZ];
	char str_devusb[100];

	dprintf( "%s %s %s\n", interface, action, product );

	int action_add = (strcmp(action, "add") == 0);

	snprintf(str_devusb, sizeof(str_devusb), "%s : %s", product, device);

	hotplug_sem_open();
	hotplug_sem_lock();

	// prevent multiple processing of the same device
	if (action_add) for (unit = 0; !found && unit < MAX_NVPARSE; unit ++)
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

		if ((wan_proto = _wan_proto(prefix, tmp)) < 0)
			continue;

		dprintf("%d \n\n", wan_proto);

		if (!found) {
			if (action_add) {
				switch(wan_proto) {
#ifdef __CONFIG_MADWIMAX__
				case WAN_WIMAX:
					found = hotplug_check_wimax(interface, product, prefix);
					break;
#endif
#ifdef __CONFIG_USBNET__
				case WAN_USBNET:
					found = hotplug_usbnet_check(interface, product, device, prefix);
					break;
#endif
#ifdef __CONFIG_MODEM__
				case WAN_USBMODEM:
					found = hotplug_check_modem(interface, product, device, prefix);
					break;
#endif
				}
			} else { // action remove
				dev_vidpid = nvram_get(strcat_r(prefix, "usb_device", tmp));
				if (dev_vidpid)
					found = (strcmp(dev_vidpid, str_devusb) == 0);
			}
		}
		if (found) {
		    if (action_add) {
			dev_vidpid = nvram_get( strcat_r(prefix, "usb_device", tmp) );
			if ( !dev_vidpid || !*dev_vidpid ||
			     !(dev_vidpid && strncmp(dev_vidpid, "zerocd", 6 == 0) ) )
			{
				dprintf("set: %s - %d - %s\n", prefix, wan_proto, str_devusb );
				switch (wan_proto) {
#ifdef __CONFIG_MADWIMAX__
				case WAN_WIMAX:
					nvram_set(strcat_r(prefix, "usb_device", tmp), str_devusb );
#ifdef HOTPLUG_DEV_START
					start_wimax( prefix );
#endif
					break;
#endif

#ifdef __CONFIG_USBNET__
				// Do nothing. Real processing by hotplug_net.
/*				case WAN_USBNET:
					nvram_set(strcat_r(prefix, "usb_device", tmp), str_devusb );
					break; */
#endif

#ifdef __CONFIG_MODEM__
				case WAN_USBMODEM:
					nvram_set(strcat_r(prefix, "usb_device", tmp), str_devusb );
#ifdef HOTPLUG_DEV_START
					// now starts from hotplug usb-serial
					//usb_modem_check(prefix);
#endif
					break;
#endif
				}
			}
		    } else { // action remove
			nvram_unset(strcat_r(prefix, "usb_device", tmp) );
			nvram_unset(strcat_r(prefix, "usb_device_name", tmp) );

			switch (wan_proto) {
#ifdef __CONFIG_MODEM__
			case WAN_USBMODEM:
				nvram_unset(strcat_r(prefix, "usb_device", tmp) );
				nvram_unset(strcat_r(prefix, "usb_device_name", tmp) );
				stop_modem_dial(prefix);
				break;
#endif
			}
			break;
		    }
		}
	}
	hotplug_sem_unlock();
	hotplug_sem_close();

	dprintf("done\n");
}
#endif /* defined(__CONFIG_MADWIMAX__) || defined(__CONFIG_MODEM__) || defined(__CONFIG_USBNET__)*/
