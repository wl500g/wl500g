/*
 * Copyright 2004, ASUSTek Inc.
 * All Rights Reserved.
 * 
 * THIS SOFTWARE IS OFFERED "AS IS", AND ASUS GRANTS NO WARRANTIES OF ANY
 * KIND, EXPRESS OR IMPLIED, BY STATUTE, COMMUNICATION OR OTHERWISE. BROADCOM
 * SPECIFICALLY DISCLAIMS ANY IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS
 * FOR A SPECIFIC PURPOSE OR NONINFRINGEMENT CONCERNING THIS SOFTWARE.
 *
 * $Id$
 */

#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <time.h>
#include <unistd.h>
#include <errno.h>
#include <syslog.h>
#include <signal.h>
#include <string.h>
#include <sys/klog.h>
#include <sys/types.h>
#include <sys/mount.h>
#include <sys/reboot.h>
#include <sys/stat.h>
#include <sys/sysmacros.h>
#include <sys/time.h>
#include <sys/resource.h>
#include <sys/utsname.h>
#include <sys/wait.h>
#include <sys/socket.h>
#include <net/if_arp.h>
#include <dirent.h>
#if defined(__UCLIBC__)
#include <crypt.h>
#endif

#include <epivers.h>
#include <bcmnvram.h>
#include <mtd.h>
#include <shutils.h>
#include <rc.h>
#include <netconf.h>
#include <nvparse.h>
#include <bcmdevs.h>
#include <wlutils.h>
#include <bcmparams.h>

static void restore_defaults(void);
static void sysinit(void);
static void rc_signal(int sig);

extern struct nvram_tuple router_defaults[];

//static int restore_defaults_g=0;
int router_model = MDL_UNKNOWN;


static int
build_ifnames(char *type, char *names, int *size)
{
	char name[32], *next;
	int len = 0;
	int s;

	/* open a raw scoket for ioctl */
	if ((s = socket(AF_INET, SOCK_RAW, IPPROTO_RAW)) < 0)
       		return -1;

	/*
	 * go thru all device names (wl<N> il<N> et<N> vlan<N>) and interfaces to 
	 * build an interface name list in which each i/f name coresponds to a device
	 * name in device name list. Interface/device name matching rule is device
	 * type dependant:
	 *
	 *	wl:	by unit # provided by the driver, for example, if eth1 is wireless
	 *		i/f and its unit # is 0, then it will be in the i/f name list if
	 *		wl0 is in the device name list.
	 *	il/et:	by mac address, for example, if et0's mac address is identical to
	 *		that of eth2's, then eth2 will be in the i/f name list if et0 is 
	 *		in the device name list.
	 *	vlan:	by name, for example, vlan0 will be in the i/f name list if vlan0
	 *		is in the device name list.
	 */
	foreach (name, type, next) {
		struct ifreq ifr;
		int i, unit;
		char var[32], *mac, ea[ETHER_ADDR_LEN];
		
		/* vlan: add it to interface name list */
		if (!strncmp(name, "vlan", 4)) {
			/* append interface name to list */
			len += snprintf(&names[len], *size - len, "%s ", name);
			continue;
		}

		/* others: proceed only when rules are met */
		for (i = 1; i <= DEV_NUMIFS; i ++) {
			/* ignore i/f that is not ethernet */
			ifr.ifr_ifindex = i;
			if (ioctl(s, SIOCGIFNAME, &ifr))
				continue;
			if (ioctl(s, SIOCGIFHWADDR, &ifr))
				continue;
			if (ifr.ifr_hwaddr.sa_family != ARPHRD_ETHER)
				continue;
			if (!strncmp(ifr.ifr_name, "vlan", 4))
				continue;
			/* wl: use unit # to identify wl */
			if (!strncmp(name, "wl", 2)) {
				if (wl_probe(ifr.ifr_name) ||
				    wl_ioctl(ifr.ifr_name, WLC_GET_INSTANCE, &unit, sizeof(unit)) ||
				    unit != atoi(&name[2]))
					continue;
			}
			/* et/il: use mac addr to identify et/il */
			else if (!strncmp(name, "et", 2) || !strncmp(name, "il", 2)) {
				snprintf(var, sizeof(var), "%smacaddr", name);
				if (!(mac = nvram_get(var)) || !ether_atoe(mac, ea) ||
				    bcmp(ea, ifr.ifr_hwaddr.sa_data, ETHER_ADDR_LEN))
					continue;

				// add by Chen-I to filter out wl interface here
				if (!wl_probe(ifr.ifr_name))
					continue;

			}
			/* mac address: compare value */
			else if (ether_atoe(name, ea) && !bcmp(ea, ifr.ifr_hwaddr.sa_data, ETHER_ADDR_LEN))
				;
			/* others: ignore */
			else
				continue;

			/* append interface name to list */
			len += snprintf(&names[len], *size - len, "%s ", ifr.ifr_name);
		}
	}
	
	close(s);

	*size = len;
	return 0;
}	

static void
stb_set(void)
{
	int stbport = atoi(nvram_safe_get("wan_stb_x"));

	if (stbport < 0 || stbport > 5)
		return;
	if (stbport == 0 && !nvram_match("vlan_set_x", "1"))
		return;

	switch (router_model)
	{
		case MDL_RTN16:
		case MDL_WNR3500L:
			{
			/* Set LAN ports */
			char *vlan1ports[] = {
					"1 2 3 4 8*",	// Defaults
					"1 2 3 8*",	// LAN 2 + LAN 3 + LAN 4
					"1 2 4 8*",	// LAN 1 + LAN 3 + LAN 4
					"1 3 4 8*",	// LAN 1 + LAN 2 + LAN 4
					"2 3 4 8*",	// LAN 1 + LAN 2 + LAN 3
					"3 4 8*"};	// LAN 1 + LAN 2
			/* Set WAN ports */
			char *vlan2ports[] = {
					"0 8",		// Defaults
					"0 4 8",	// WAN + LAN 1
					"0 3 8",	// WAN + LAN 2
					"0 2 8",	// WAN + LAN 3
					"0 1 8",	// WAN + LAN 4
					"0 1 2 8"};	// WAN + LAN 3 + LAN 4
			nvram_set("vlan1ports", vlan1ports[stbport]);
			nvram_set("vlan2ports", vlan2ports[stbport]);
			break;
			}
		case MDL_RTN12:
		case MDL_RTN10:
		case MDL_WL500GPV2:
			{
			/* Set LAN ports */
			char *vlan0ports[] = {
					"0 1 2 3 5*",	// Defaults
					"0 1 2 5*",	// LAN 2 + LAN 3 + LAN 4
					"0 1 3 5*",	// LAN 1 + LAN 3 + LAN 4
					"0 2 3 5*",	// LAN 1 + LAN 2 + LAN 4
					"1 2 3 5*",	// LAN 1 + LAN 2 + LAN 3
					"2 3 5*"};	// LAN 1 + LAN 2
			/* Set WAN ports */
			char *vlan1ports[] = {
					"4 5",		// Defaults
					"3 4 5",	// WAN + LAN 1
					"2 4 5",	// WAN + LAN 2
					"1 4 5",	// WAN + LAN 3
					"0 4 5",	// WAN + LAN 4
					"0 1 4 5"};	// WAN + LAN 3 + LAN 4
			nvram_set("vlan0ports", vlan0ports[stbport]);
			nvram_set("vlan1ports", vlan1ports[stbport]);
			break;
			}
		case MDL_WL330GE:
		case MDL_WL550GE:
		case MDL_WL520GU:
		case MDL_WL500GP:
		case MDL_DIR320:
			{
			/* Set LAN ports */
			char *vlan0ports[] = {
					"1 2 3 4 5*",	// Defaults
					"2 3 4 5*",	// LAN 2 + LAN 3 + LAN 4
					"1 3 4 5*",	// LAN 1 + LAN 3 + LAN 4
					"1 2 4 5*",	// LAN 1 + LAN 2 + LAN 4
					"1 2 3 5*",	// LAN 1 + LAN 2 + LAN 3
					"1 2 5*"};	// LAN 1 + LAN 2
			/* Set WAN ports */
			char *vlan1ports[] = {
					"0 5",		// Defaults
					"0 1 5",	// WAN + LAN 1
					"0 2 5",	// WAN + LAN 2
					"0 3 5",	// WAN + LAN 3
					"0 4 5",	// WAN + LAN 4
					"0 3 4 5"};	// WAN + LAN 3 + LAN4
			nvram_set("vlan0ports", vlan0ports[stbport]);
			nvram_set("vlan1ports", vlan1ports[stbport]);
			break;
			}
	}
	if (stbport == 0)
		nvram_unset("vlan_set_x");
	else
		nvram_set("vlan_set_x", "1");
}

static void
early_defaults(void)
{
	/* wl700ge -- boardflags are not set correctly */
	if (nvram_match("boardflags", "0x10") && nvram_get("default_boardflags")) 
	{
		nvram_set("boardflags", nvram_get("default_boardflags"));
	}

	if (nvram_match("wan_route_x", "IP_Bridged"))
	{
		switch (router_model)
		{
		    case MDL_RTN16:
		    case MDL_WNR3500L:
                        nvram_set("vlan1ports", "0 1 2 3 4 8*");
                        nvram_set("vlan2ports", "8");
                        break;

                    default:
			nvram_set("vlan0ports", "0 1 2 3 4 5*");
			nvram_set("vlan1ports", "5");
		}
		nvram_set("vlan_set_x", "1");
	}
	else { /* router mode, use vlans */

		/* fix Sentry5 config */
		if (router_model == MDL_WL500GX && !nvram_get("vlan0ports"))
		{
			nvram_set("lan_ifname", "br0");
			nvram_set("lan_ifnames", "vlan0 eth1");
			nvram_set("wan_ifname", "vlan1");
			nvram_set("wan_ifnames", "vlan1");
		
			nvram_set("vlan0hwname", "et0");
			nvram_set("vlan0ports", "1 2 3 4 5*");
			nvram_set("vlan1hwname", "et0");
			nvram_set("vlan1ports", "0 5");
		}

		/* bcm95350rg -- use vlans (wl550ge, wl500gp, wl700ge) vs wl320g - no vlans */
		if (nvram_match("wandevs", "et0") && 	/* ... wl500gpv2 */
		    (nvram_match("vlan1ports", "0 5u") || nvram_match("vlan1ports", "4 5u")) &&
		    (strtoul(nvram_safe_get("boardflags"), NULL, 0) & BFL_ENETVLAN) != 0)
		{
			nvram_set("wandevs", "vlan1");
			nvram_set("wan_ifname", "vlan1");
			nvram_set("wan_ifnames", "vlan1");
		
			/* data should be tagged for WAN port too */
			nvram_set("vlan1ports",
				nvram_match("vlan1ports", "0 5u") ? "0 5" : "4 5");
		}

		/* USE 0x04cf for N16(BCM4718) *
		 * RT-N16, WAN=0, LAN1~4=4~1 CPU port=8 */
		if (nvram_match("boardtype", "0x04cf"))
		{
			if (!nvram_get("wan_ifname") || !nvram_get("vlan2hwname"))
			{
	        		nvram_unset( "vlan0ports" );
	        		nvram_unset( "vlan0hwname" );
	        		nvram_unset( "br0_ifnames" );
				nvram_set("vlan1ports", "1 2 3 4 8*");
				nvram_set("vlan2ports", "0 8");
				nvram_set("vlan1hwname", "et0");
				nvram_set("vlan2hwname", "et0");
				nvram_set("landevs", "vlan1 wl0");
				nvram_set("lan_ifnames", "vlan1 eth1");
				nvram_set("wandevs", "vlan2");
				nvram_set("wan_ifname", "vlan2");
				nvram_set("wan_ifnames", "vlan2");
				nvram_set("wan0_ifname", "vlan2");
				nvram_set("wan0_ifnames", "vlan2");
				nvram_set("wlan_ifname", "eth1");
			}
		}

		/* fix RT-N12 / RT-N10 vlans */
		if (router_model == MDL_RTN12 || router_model == MDL_RTN10)
		{
			if (!nvram_get("wan_ifname") || !nvram_get("vlan1hwname"))
			{
				nvram_set("vlan0ports", "0 1 2 3 5*");
				nvram_set("vlan1ports", "4 5");
				nvram_set("vlan0hwname", "et0");
				nvram_set("vlan1hwname", "et0");
				nvram_set("landevs", "vlan0 wl0");
				nvram_set("lan_ifnames", "vlan0 eth1");
				nvram_set("wandevs", "vlan1");
				nvram_set("wan_ifname", "vlan1");
				nvram_set("wan_ifnames", "vlan1");
				nvram_set("wan0_ifname", "vlan1");
				nvram_set("wan0_ifnames", "vlan1");
				nvram_set("wlan_ifname", "eth1");
			}
		}

		/* fix DLINK DIR-320 vlans */
		if (router_model == MDL_DIR320 && !nvram_get("wan_ifname"))
		{
        		nvram_unset( "vlan2ports" );
          		nvram_unset( "vlan2hwname" );
			nvram_set("vlan1hwname", "et0");
			nvram_set("vlan1ports", "0 5");
			nvram_set("wandevs", "vlan1");
			nvram_set("wan_ifname", "vlan1");
			nvram_set("wan_ifnames", "vlan1");
			nvram_set("wan0_ifname", "vlan1");
			nvram_set("wan0_ifnames", "vlan1");
		}

		/* STB port setting */
		stb_set();

	} // vlans

	/* wl550ge -- missing wl0gpio values */
	if (router_model == MDL_WL550GE && !nvram_get("wl0gpio0"))
	{
		nvram_set("wl0gpio0", "2");
		nvram_set("wl0gpio1", "0");
		nvram_set("wl0gpio2", "0");
		nvram_set("wl0gpio3", "0");
	}
	
	/* fix AIR LED -- override default SROM setting and fix wl550ge config */
	if (!nvram_get("wl0gpio0") || nvram_match("wl0gpio0", "2"))
		nvram_set("wl0gpio0", "0x88");
	
	/* WL520gu/gc/gp/330ge */
	if (nvram_match("wl0gpio1", "0x02"))
		nvram_set("wl0gpio1", "0x88");

	/* wl500gp -- 16mb memory activated, 32 available */
	if (router_model == MDL_WL500GP &&
	    nvram_match("sdram_init", "0x000b") && nvram_match("sdram_config", "0x0062"))
	{
		nvram_set("sdram_init", "0x0009");
		nvram_set("sdram_ncdl", "0");
	}

	/* fix DIR-320 gpio */
	if (router_model == MDL_DIR320 && nvram_match("wl0gpio0", "255"))
	{
		nvram_set("wl0gpio0", "8");
		nvram_set("wl0gpio1", "0");
		nvram_set("wl0gpio2", "0");
		nvram_set("wl0gpio3", "0");
	}

	/* fix WL500W mac adresses for WAN port */
	if (nvram_match("et1macaddr", "00:90:4c:a1:00:2d")) 
		nvram_set("et1macaddr", nvram_get("et0macaddr"));

#if 0	/* leave it commented out until VLANs will work */
	/* WL500W could have vlans enabled */
	if (router_model == MDL_WL500W)
	{
		if (strtoul(nvram_safe_get("boardflags"), NULL, 0) & BFL_ENETVLAN)
		{
			if (!nvram_get("vlan0ports")) {
				nvram_set("vlan0hwname", "et0");
				nvram_set("vlan0ports", "0 1 2 3 5*");
				nvram_set("vlan1hwname", "et0");
				nvram_set("vlan1ports", "4 5");
			}
			nvram_set("lan_ifnames", "vlan0 eth2");
			nvram_set("wan_ifname", "vlan1");
		} else {
			nvram_set("lan_ifnames", "eth0 eth2");
			nvram_set("wan_ifname", "eth1");
		}
		nvram_set("lan_ifname", "br0");
		nvram_set("wan_ifnames", nvram_get("wan_ifname"));
	}
#endif

	/* set lan_hostname */
	if (!nvram_invmatch("lan_hostname", ""))
	{
		/* derive from et0 mac addr */
		char *mac = nvram_get("et0macaddr");
		if (mac && strlen(mac) == 17)
		{
			char hostname[16];
			sprintf(hostname, "WL-%c%c%c%c%c%c%c%c%c%c%c%c",
				mac[0], mac[1], mac[3], mac[4], mac[6], mac[7],
				mac[9], mac[10], mac[12], mac[13], mac[15], mac[16]);
			nvram_set("lan_hostname", hostname);
		}
	}
}

static void
restore_defaults(void)
{
	struct nvram_tuple generic[] = {
		{ "lan_ifname", "br0", 0 },
		{ "lan_ifnames", "eth0 eth2 eth3 eth4", 0 },
		{ "wan_ifname", "eth1", 0 },
		{ "wan_ifnames", "eth1", 0 },
		{ 0, 0, 0 }
	};
	struct nvram_tuple vlan[] = {
		{ "lan_ifname", "br0", 0 },
		{ "lan_ifnames", "vlan0 eth1 eth2 eth3", 0 },
		{ "wan_ifname", "vlan1", 0 },
		{ "wan_ifnames", "vlan1", 0 },
		{ 0, 0, 0 }
	};
	struct nvram_tuple dyna[] = {
		{ "lan_ifname", "br0", 0 },
		{ "lan_ifnames", "", 0 },
		{ "wan_ifname", "", 0 },
		{ "wan_ifnames", "", 0 },
		/* default with vlans disabled */
		{ "wan_nat_x", "0", 0},
		{ "wan_route_x", "IP_Bridged", 0},
		{ 0, 0, 0 }
	};


#ifdef CONFIG_SENTRY5
#include "rcs5.h"
#else
#define RC1_START() 
#define RC1_STOP()  
#define RC7_START()
#define RC7_STOP()
#define LINUX_OVERRIDES() 
#define EXTRA_RESTORE_DEFAULTS() 
#endif

	struct nvram_tuple *linux_overrides;
	struct nvram_tuple *t, *u;
	int restore_defaults, i;
	uint boardflags;
	char *landevs, *wandevs;
	char lan_ifnames[128], wan_ifnames[128];
	char wan_ifname[32], *next;
	int len;
	int ap = 0;

	/* Restore defaults if told to or OS has changed */
	restore_defaults = !nvram_match("restore_defaults", "0") || nvram_invmatch("os_name", "linux");

	if (restore_defaults)
		cprintf("Restoring defaults...");

	/* Delete dynamically generated variables */
	if (restore_defaults) {
		char tmp[100], prefix[] = "wlXXXXXXXXXX_";
		for (i = 0; i < MAX_NVPARSE; i++) {
			del_filter_client(i);
			del_forward_port(i);
			del_autofw_port(i);
			snprintf(prefix, sizeof(prefix), "wl%d_", i);
			for (t = router_defaults; t->name; t ++) {
				if (!strncmp(t->name, "wl_", 3))
					nvram_unset(strcat_r(prefix, &t->name[3], tmp));
			}
			snprintf(prefix, sizeof(prefix), "wan%d_", i);
			for (t = router_defaults; t->name; t ++) {
				if (!strncmp(t->name, "wan_", 4))
					nvram_unset(strcat_r(prefix, &t->name[4], tmp));
			}
		}
	}

	/* 
	 * Build bridged i/f name list and wan i/f name list from lan device name list
	 * and wan device name list. Both lan device list "landevs" and wan device list
	 * "wandevs" must exist in order to preceed.
	 */
	if ((landevs = nvram_get("landevs")) && (wandevs = nvram_get("wandevs"))) {
		/* build bridged i/f list based on nvram variable "landevs" */
		len = sizeof(lan_ifnames);
		if (!build_ifnames(landevs, lan_ifnames, &len) && len)
			dyna[1].value = lan_ifnames;
		else
			goto canned_config;
		/* build wan i/f list based on nvram variable "wandevs" */
		len = sizeof(wan_ifnames);
		if (!build_ifnames(wandevs, wan_ifnames, &len) && len) {
			dyna[3].value = wan_ifnames;
			foreach (wan_ifname, wan_ifnames, next) {
				dyna[2].value = wan_ifname;
				break;
			}
		}
		else
			ap = 1;
		
		/* if vlans enabled -- enable router mode */
		if ((strtoul(nvram_safe_get("boardflags"), 
			NULL, 0) & BFL_ENETVLAN) != 0) dyna[4].name = NULL;

		linux_overrides = dyna;
	}
	/* override lan i/f name list and wan i/f name list with default values */
	else {
canned_config:
		boardflags = strtoul(nvram_safe_get("boardflags"), NULL, 0);
		if (boardflags & BFL_ENETVLAN)
			linux_overrides = vlan;
		else
			linux_overrides = generic;

		/* override the above linux_overrides with a different table */
		LINUX_OVERRIDES();
	}

	if (router_model == MDL_WL500GX)
		linux_overrides = vlan;
	
	/* Restore defaults */
	for (t = router_defaults; t->name; t++) {
		if (restore_defaults || !nvram_get(t->name)) {
			for (u = linux_overrides; u && u->name; u++) {
				if (!strcmp(t->name, u->name)) {
					nvram_set(u->name, u->value);
					break;
				}
			}
			if (!u || !u->name)
				nvram_set(t->name, t->value);
		}
	}

	/* Force to AP */
	if (ap)
		nvram_set("router_disable", "1");

	/* Always set OS defaults */
	nvram_set("os_name", "linux");
	nvram_set("os_version", EPI_VERSION_STR);
	nvram_set("os_date", __DATE__);

	nvram_set("is_modified", "0");

	/* Commit values */
	if (restore_defaults) {
		EXTRA_RESTORE_DEFAULTS();
		/* default value of vlan */
		nvram_commit();		
		cprintf("done\n");
	}
}

static void
set_wan0_vars(void)
{
	int unit;
	char tmp[100], prefix[] = "wanXXXXXXXXXX_";
	
	/* check if there are any connections configured */
	for (unit = 0; unit < MAX_NVPARSE; unit ++) {
		snprintf(prefix, sizeof(prefix), "wan%d_", unit);
		if (nvram_get(strcat_r(prefix, "unit", tmp)))
			break;
	}
	/* automatically configure wan0_ if no connections found */
	if (unit >= MAX_NVPARSE) {
		struct nvram_tuple *t;
		char *v;

		/* Write through to wan0_ variable set */
		snprintf(prefix, sizeof(prefix), "wan%d_", 0);
		for (t = router_defaults; t->name; t ++) {
			if (!strncmp(t->name, "wan_", 4)) {
				if (nvram_get(strcat_r(prefix, &t->name[4], tmp)))
					continue;
				v = nvram_get(t->name);
				nvram_set(tmp, v ? v : t->value);
			}
		}
		nvram_set(strcat_r(prefix, "unit", tmp), "0");
		nvram_set(strcat_r(prefix, "desc", tmp), "Default Connection");
		nvram_set(strcat_r(prefix, "primary", tmp), "1");
	}
	/* reconfigure wan0 for client mode */
	if (nvram_invmatch("wl_mode_ex", "ap")) {
		nvram_set("wan0_ifname", nvram_safe_get("wl0_ifname"));
		nvram_set("wan0_ifnames", nvram_safe_get("wl0_ifname"));
	} else {
		nvram_set("wan0_ifname", nvram_get("wan_ifname"));
		nvram_set("wan0_ifnames", nvram_get("wan_ifnames"));
	}
	nvram_set("wan0_priority", "0");
}

/*
 * create /etc/passwd and /etc/group files
 */
 
static void 
make_etc(void)
{
	FILE *f;
	char *name, *pass;
	
	/* crypt using md5, no salt */
	name = nvram_get("http_username") ? : "admin";
	pass = crypt(nvram_get("http_passwd") ? : "admin", "$1$");
	
	if ((f = fopen("/etc/passwd", "w"))) {
		fprintf(f, "%s:%s:0:0:root:/usr/local/root:/bin/sh\n"
			"nobody:x:99:99:nobody:/:/sbin/nologin\n", name, pass);
		fclose(f);
	}
	
	if ((f = fopen("/etc/group", "w"))) {
		fprintf(f, "root:x:0:%s\nnobody:x:99:\n", name);
		fclose(f);
	}
	
	/* uClibc TZ */
	if ((f = fopen("/etc/TZ", "w"))) {
		fprintf(f, "%s\n", nvram_safe_get("time_zone"));
		fclose(f);
	}
	
	/* /etc/resolv.conf compatibility */
	symlink("/tmp/resolv.conf", "/etc/resolv.conf");
	
	/* hostname */
	if ((f = fopen("/proc/sys/kernel/hostname", "w"))) {
		fputs(nvram_safe_get("lan_hostname"), f);
		fclose(f);
	}

	/* crond */
	symlink("/etc/crontabs", "/var/spool/cron/crontabs");

	/* mtab compability */
	symlink("/proc/mounts", "/etc/mtab");
}

static int noconsole = 0;

static void
sysinit(void)
{
	char buf[PATH_MAX];
	struct utsname name;
	struct stat tmp_stat;
	
	time_t now;
	struct tm gm, local;
	struct timezone tz;
	struct timeval tv = { 0 };
	struct rlimit lim;

	/* set default hardlimit */
	getrlimit(RLIMIT_NOFILE, &lim);
	lim.rlim_max = 16384;
	setrlimit(RLIMIT_NOFILE, &lim);

	/* /proc */
	mount("proc", "/proc", "proc", MS_MGC_VAL, NULL);

#ifdef LINUX26
	mount("sysfs", "/sys", "sysfs", MS_MGC_VAL, NULL);
	mount("devfs", "/dev", "tmpfs", MS_MGC_VAL | MS_NOATIME, NULL);

	/* populate /dev */
	mknod("/dev/console", S_IFCHR | 0600, makedev(5, 1));
	mknod("/dev/null", S_IFCHR | 0666, makedev(1, 3));
	eval("/sbin/mdev", "-s");

	/* /dev/pts */
	mkdir("/dev/pts", 0755);
	mount("devpts", "/dev/pts", "devpts", MS_MGC_VAL, NULL);

	/* /dev/shm */
	mkdir("/dev/shm", S_ISVTX | 0755);
	
	/* extra links */
	symlink("/proc/self/fd", "/dev/fd");
	symlink("/proc/self/fd/0", "/dev/stdin");
	symlink("/proc/self/fd/1", "/dev/stdout");
	symlink("/proc/self/fd/2", "/dev/stderr");
#endif

	/* /tmp */
	mount("tmpfs", "/tmp", "tmpfs", MS_MGC_VAL | MS_NOATIME, NULL);

	/* /var */
	mkdir("/tmp/var", 0777);
	mkdir("/var/lock", 0777);
	mkdir("/var/log", 0777);
	mkdir("/var/run", 0777);
	mkdir("/var/spool", 0777);
	mkdir("/var/spool/cron", 0777);
	mkdir("/var/state", 0777);
	mkdir("/var/tmp", 0777);
	
	/* /usr/local */
	mkdir("/tmp/local", 0755);
	mkdir("/tmp/local/root", 0700);
	
	/* /etc contents */
	eval("cp", "-dpR", "/usr/etc", "/tmp");

	/* create /etc/{passwd,group,TZ} */
	make_etc();
	
	/* Setup console */
	if (console_init())
		noconsole = 1;
	klogctl(8, NULL, atoi(nvram_safe_get("console_loglevel")));

	/* load flashfs */
	if (!eval("flashfs", "start"))
		eval("/usr/local/sbin/pre-boot");

	/* Modules */
	uname(&name);
	snprintf(buf, sizeof(buf), "/lib/modules/%s", name.release);
	if (stat("/proc/modules", &tmp_stat) == 0 &&
	    stat(buf, &tmp_stat) == 0) {
		char module[80], *modules, *next;

		modules = nvram_get("kernel_mods") ? : 
#if defined(MODEL_WL700G)
		"ide-core aec62xx "
# ifdef LINUX26
		"ide-generic "
# else
		"ide-detect "
# endif
		"ide-disk "
#endif
#if defined(__CONFIG_EMF__)
		"emf igs "
#endif
		"et wl";
		foreach(module, modules, next)
			eval("insmod", module);
	}

	/* Update kernel timezone and time */
	setenv("TZ", nvram_safe_get("time_zone"), 1);
	time(&now);
	gmtime_r(&now, &gm);
	localtime_r(&now, &local);
	tz.tz_minuteswest = (mktime(&gm) - mktime(&local)) / 60;
	settimeofday(&tv, &tz);

#if defined(MODEL_WL700G)
	eval("insmod", "gpiortc", "sda_mask=0x04", "scl_mask=0x20");
	usleep(100000);
#endif

	if (exists("/dev/misc/rtc"))
		eval("/sbin/hwclock", "-s");

	dprintf("done\n");
}

/* States */
enum {
	RESTART,
	STOP,
	START,
	TIMER,
	IDLE,
	SERVICE,
};
static int state = START;
static int signalled = -1;


/* Signal handling */
static void
rc_signal(int sig)
{
	if (state == IDLE) {	
		if (sig == SIGHUP) {
			dprintf("signalling RESTART\n");
			signalled = RESTART;
		}
		else if (sig == SIGUSR2) {
			dprintf("signalling START\n");
			signalled = START;
		}
		else if (sig == SIGINT) {
			dprintf("signalling STOP\n");
			signalled = STOP;
		}
		else if (sig == SIGALRM) {
			dprintf("signalling TIMER\n");
			signalled = TIMER;
		}
		else if (sig == SIGUSR1) {
			dprintf("signalling USR1\n");
			signalled = SERVICE;
		}
	}
}

/* Timer procedure */
int
do_timer(void)
{
	time_t now;
	struct tm gm, local;
	struct timezone tz;

#ifndef ASUS_EXT
	int interval = atoi(nvram_safe_get("timer_interval"));

	dprintf("%d\n", interval);

	if (interval == 0)
		return 0;

	/* Sync time */
	start_ntpc();
#endif
	/* Update kernel timezone */
	setenv("TZ", nvram_safe_get("time_zone"), 1);
	time(&now);
	gmtime_r(&now, &gm);
	localtime_r(&now, &local);
	tz.tz_minuteswest = (mktime(&gm) - mktime(&local)) / 60;
	settimeofday(NULL, &tz);
#ifndef ASUS_EXT
	alarm(interval);
#endif
	return 0;
}

/* Main loop */
static void
main_loop(void)
{
	sigset_t sigset;
	pid_t shell_pid = 0;
	uint boardflags;

	/* Convert vital config before loading modules */
	early_defaults();

	/* Basic initialization */
	sysinit();

	/* Setup signal handlers */
	signal_init();
	signal(SIGHUP, rc_signal);
	signal(SIGUSR1, rc_signal);
	signal(SIGUSR2, rc_signal);
	signal(SIGINT, rc_signal);
	signal(SIGALRM, rc_signal);
	sigemptyset(&sigset);

	/* Give user a chance to run a shell before bringing up the rest of the system */

	if (!noconsole)
		run_shell(1, 0);

	/* Add vlan */
	boardflags = (router_model == MDL_WL500GX) ? BFL_ENETVLAN :
		strtoul(nvram_safe_get("boardflags"), NULL, 0);

	/* Add loopback */
	config_loopback();

	/* Convert deprecated variables */
	convert_deprecated();

	/* Restore defaults if necessary */
	restore_defaults();

#ifdef ASUS_EXT	
	convert_asus_values();
#endif

	/* Setup wan0 variables if necessary */
	set_wan0_vars();
	
	/* Loop forever */
	for (;;) {
		switch (state) {
		case SERVICE:
			dprintf("SERVICE\n");
			service_handle();
			state = IDLE;
			break;
		case RESTART:
			dprintf("RESTART\n");
			/* Fall through */
		case STOP:
			dprintf("STOP\n");

#ifdef ASUS_EXT
			stop_misc();
#endif
			stop_services();
			stop_wan(NULL);
			stop_lan();

			if (boardflags & BFL_ENETVLAN)
				stop_vlan();

			if (state == STOP) {
				state = IDLE;
				break;
			}
			/* Fall through */
		case START:
			dprintf("START\n");

			if (boardflags & BFL_ENETVLAN)
				start_vlan();

			start_lan();
			//if (restore_defaults_g) 
			//{
			//	goto retry;
			//}
			start_services();
			start_wan();
#ifndef __CONFIG_BCMWL5__
			start_nas("wan");
#endif

#ifdef ASUS_EXT
			start_misc();
#endif
			eval("/usr/local/sbin/post-boot");
#ifdef ASUS_EXT
			sleep(3);
			diag_PaN();
#endif
			/* Fall through */
		case TIMER:
			dprintf("TIMER\n");
			do_timer();			
			/* Fall through */
		case IDLE:
			dprintf("IDLE\n");
			state = IDLE;
			/* Wait for user input or state change */
			while (signalled == -1) {
				if (!noconsole && (!shell_pid || kill(shell_pid, 0) != 0))
					shell_pid = run_shell(0, 1);
				else
					sigsuspend(&sigset);
			}
			state = signalled;
			signalled = -1;
			break;
		default:
			dprintf("UNKNOWN\n");
			return;
		}
	}
}

static int
check_option(int argc, char * const argv[], int *index, const char *option) {
	int res;
	int found = 0;
	opterr = 0;
	if (strlen(option)==0) return 0;
	while ((res = getopt(argc,argv,option)) != -1) {
		if (!found)
			found = ((char)res == option[0]);
	}
	*index = optind;
	return found;
}

int
main(int argc, char **argv)
{
	char *base = strrchr(argv[0], '/');
	
	base = base ? base + 1 : argv[0];

	router_model = get_model();

	/* init */
	if (!strcmp(base, "init")) {
		main_loop();
		return 0;
	}

	/* Set TZ for all rc programs */
	setenv("TZ", nvram_safe_get("time_zone"), 1);

#ifdef DEBUG
	cprintf("rc applet: %s %s %s",
		 base, (argc>1) ? argv[1] : "", (argc>2) ? argv[2] : "");
#endif

	/* ppp */
	if (!strcmp(base, "ip-up"))
		return ipup_main(argc, argv);
	else if (!strcmp(base, "ip-down"))
		return ipdown_main(argc, argv);
#ifdef __CONFIG_IPV6__
	else if (!strcmp(base, "ipv6-up"))
		return ip6up_main(argc, argv);
	else if (!strcmp(base, "ipv6-down"))
		return ip6down_main(argc, argv);
#endif
	/* udhcpc.script [ deconfig bound renew ] */
	else if (!strcmp(base, "udhcpc.script"))
		return udhcpc_main(argc, argv);
#ifdef __CONFIG_IPV6__
	else if (!strcmp(base, "dhcp6c.script"))
		return dhcp6c_main(argc, argv);
#endif
#ifdef __CONFIG_MADWIMAX__
	/* madwimax [ if-create if-up if-down if-release ] */
	else if ( !strcmp(base, "madwimax.events" ) )
		return madwimax_main(argc, argv);
#endif
	/* restore default */
	else if (!strcmp(base, "restore"))
	{
		if (argc==2)
		{
			int step = atoi(argv[1]);
			if (step>=1)
			{
				nvram_set("vlan_enable", "1");
				restore_defaults();
			}
			/* Setup wan0 variables if necessary */
			if (step>=2)
				set_wan0_vars();
			if (step>=3)
				RC1_START();
			if (step>=4)
				start_lan();
		}
		return 0;
	}

	/* erase [device] */
	else if (!strcmp(base, "erase")) {
		if (argv[1])
			return mtd_erase(argv[1]);
		else {
			fprintf(stderr, "usage: erase [device]\n");
			return EINVAL;
		}
	}

	/* write [path] [device] */
	else if (!strcmp(base, "write")) {
		int index;
		int reboot = check_option(argc, argv, &index, "r");
		if (argc >= index+2) {
			if (reboot) preshutdown_system();
			int res = mtd_write(argv[index], argv[index+1]);
			if (reboot && !res) kill(1, SIGABRT);
			return res;
		} else {
			fprintf(stderr, "usage: write [-r] [path] [device]\n");
			fprintf(stderr, "	-r: reboot after write\n");
			return EINVAL;
		}
	}

	/* flash [path] [device] */
	else if (!strcmp(base, "flash")) {
		int index;
		int reboot = check_option(argc, argv, &index, "r");
		if (argc >= index+2) {
			if (reboot) preshutdown_system();
			int res = mtd_flash(argv[index], argv[index+1]);
			if (reboot && !res) kill(1, SIGABRT);
			return res;
		} else {
			fprintf(stderr, "usage: flash [-r] [path] [device]\n");
			fprintf(stderr, "	-r: reboot after flash\n");
			return EINVAL;
		}
	}

	/* hotplug [event] */
	else if (!strcmp(base, "hotplug")) {
		if (argc >= 2) {
		#ifdef LINUX26
			eval("/sbin/mdev");
		#endif
			if (!strcmp(argv[1], "net"))
				return hotplug_net();
#ifdef ASUS_EXT
		#ifdef LUNUX26
			else if (!strcmp(argv[1], "usb"))
		#else
			else if (!strcmp(argv[1], "usb") || !strcmp(argv[1], "block"))
		#endif
				return hotplug_usb();
#endif
		} else {
			fprintf(stderr, "usage: hotplug [event]\n");
			return EINVAL;
		}
	}

#ifdef ASUS_EXT
	/* ddns update ok */
	else if (!strcmp(base, "stopservice")) {
		return stop_service_main();
	}
	/* ddns update ok */
	else if (!strcmp(base, "ddns_updated")) 
	{
		return ddns_updated_main();
	}
	/* ddns update ok */
	else if (!strcmp(base, "start_ddns")) 
	{
		int forced = 0;
		if (argc >= 2)
			forced = atoi(argv[1]);
		return start_ddns(forced);
	}
	/* send alarm */
	else if (!strcmp(base, "sendalarm")) {
		return sendalarm_main(argc, argv);
	}
	/* invoke watchdog */
	else if (!strcmp(base, "watchdog")) {
		return(watchdog_main());
	}
	/* run ntp client */
	else if (!strcmp(base, "ntp")) {
		return (!start_ntpc());
	}
#ifdef USB_SUPPORT
#ifdef __CONFIG_RCAMD__
	/* remove webcam module */
	else if (!strcmp(base, "rmwebcam")) {
		if (argc >= 2)
			return (remove_usb_webcam(nvram_safe_get("usb_web_device")));
		else return EINVAL;
	}
	/* run rcamd */
	else if (!strcmp(base, "rcamdmain")) {
		return (hotplug_usb_webcam(nvram_safe_get("usb_web_device")));
	}
#endif
	/* remove usbstorage module */
	else if (!strcmp(base, "rmstorage")) {
		int scsi_host = -1;
		if (argc >= 2)
			scsi_host = atoi(argv[1]);
		return (remove_storage_main(scsi_host));
	}
#ifdef __CONFIG_WAVESERVER__
	/* run waveserver */
	else if (!strcmp(base, "waveservermain")) {
		return (waveserver_main());
	}
#endif
	/* run ftp server */
	else if (!strcmp(base, "start_ftpd")) {
		return (restart_ftpd());
	}
#endif //USB_SUPPORT
	/* write srom */
	else if (!strcmp(base, "wsrom")) 
	{
		do_timer();
		if (argc >= 4) 
			return wsrom_main(argv[1], atoi(argv[2]), atoi(argv[3]));
		else {
			fprintf(stderr, "usage: wsrom [dev] [position] [value in 2 bytes]\n");
			return EINVAL;
		}
	}
	/* write srom */
	else if (!strcmp(base, "rsrom")) 
	{
		if (argc >= 3)
		{	 
			rsrom_main(argv[1], atoi(argv[2]), 1);
			return 0;
		}
		else {
			fprintf(stderr, "usage: rsrom [dev] [position]\n");
			return EINVAL;
		}
	}
	/* write mac */
	else if (!strcmp(base, "wmac")) 
	{
		if (argc >= 3) 
			return write_mac(argv[1], argv[2]);
		else {
			fprintf(stderr, "usage: wmac [dev] [mac]\n");
			return EINVAL;
		}
	}
	/* wlan update */
	else if (!strcmp(base, "wlan_update")) 
	{
		wlan_update();
		return 0;
	}
	/* landhcpc [ deconfig bound renew ], for lan only */
	else if (!strcmp(base, "landhcpc"))
		return udhcpc_ex_main(argc, argv);
	else if (!strcmp(base, "bpa_connect"))
                 return bpa_connect_main(argc, argv);
        else if (!strcmp(base, "bpa_disconnect"))
                 return bpa_disconnect_main(argc, argv);
#endif

	/* rc [stop|start|restart ] */
	else if (!strcmp(base, "rc")) {
		if (argv[1]) {
			if (strncmp(argv[1], "start", 5) == 0)
				return kill(1, SIGUSR2);
			else if (strncmp(argv[1], "stop", 4) == 0)
				return kill(1, SIGINT);
			else if (strncmp(argv[1], "restart", 7) == 0)
				return kill(1, SIGHUP);
		} else {
			fprintf(stderr, "usage: rc [start|stop|restart]\n");
			return EINVAL;
		}
	} else if (!strcmp(base, "halt")) {
		kill(1, SIGQUIT);
	} else if (!strcmp(base, "reboot")) {
		kill(1, SIGTERM);
	} else if (!strcmp(base, "poweron")) {
		return poweron_main(argc, argv);
	}
	
	return EINVAL;
}
