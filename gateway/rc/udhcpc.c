/*
 * udhcpc scripts
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
#include <net/route.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <net/if.h>
#include <arpa/inet.h>
#include <errno.h>
#include <time.h>
#include <unistd.h>
#include <proto/ethernet.h>

#include "rc.h"

static char udhcpstate[12];

#ifdef __CONFIG_IPV6__
static char dhcp6state[12];
struct duid {
	uint16_t type;
	uint16_t hwtype;
	unsigned char ea[ETHER_ADDR_LEN];
} __attribute__ ((__packed__));

/* Generate DUID-LL */
int get_duid(struct duid *duid)
{
	if (!duid || !ether_atoe(nvram_safe_get("lan_hwaddr"), duid->ea))
		return 0;

	duid->type = htons(3);		/* DUID-LL */
	duid->hwtype = htons(1);	/* Ethernet */

	return ETHER_ADDR_LEN;
}

#ifdef __CONFIG_ODHCP6C__
/* returns: length of hex value
 * dest size must be lagre enough to accept n bytes from
   src in hex representation plus one \0 byte */
static int
bin2hex(char *dest, size_t size, const void *src, size_t n)
{
	unsigned char *sptr = (unsigned char *) src;
	unsigned char *send = sptr + n;
	char *dptr = dest;

	while (sptr < send && size > 2) {
		n = snprintf(dptr, size, "%02x", *sptr++);
		dptr += n;
		size -= n;
	}
	return dptr - dest;
}
#endif
#endif

static int expires(const char *wan_ifname, unsigned int in)
{
	time_t now;
	FILE *fp;
	char tmp[100], prefix[WAN_PREFIX_SZ];
	int unit;

	if ((unit = wan_prefix(wan_ifname, prefix)) < 0)
		return -1;

	time(&now);
	snprintf(tmp, sizeof(tmp), "/tmp/udhcpc%d.expires", unit); 
	if (!(fp = fopen(tmp, "w"))) {
		perror(tmp);
		return errno;
	}
	fprintf(fp, "%d", (unsigned int) now + in);
	fclose(fp);
	return 0;
}

/* 
 * deconfig: This argument is used when udhcpc starts, and when a
 * leases is lost. The script should put the interface in an up, but
 * deconfigured state.
*/
static int deconfig(const char *wan_ifname, int zcip)
{
	const char *client = zcip ? "zcip client" : "dhcp client";
	char tmp[100], prefix[WAN_PREFIX_SZ];
	char wanprefix[WAN_PREFIX_SZ];

	if (wans_prefix(wan_ifname, wanprefix, prefix) < 0)
		return EINVAL;

	if (nvram_match(strcat_r(wanprefix, "proto", tmp), "l2tp") ||
	    nvram_match(strcat_r(wanprefix, "proto", tmp), "pptp")) {
		/* fix kernel route-loop issue */
		logmessage(client, "skipping resetting IP address to 0.0.0.0");
	} else
		ifconfig(wan_ifname, IFUP, "0.0.0.0", NULL);

	expires(wan_ifname, 0);

	wan_down(wan_ifname);

	logmessage(client, "%s: lease is lost", udhcpstate);
	wanmessage("lost IP from server");

	dprintf("done\n");
	return 0;
}

/*
 * bound: This argument is used when udhcpc moves from an unbound, to
 * a bound state. All of the paramaters are set in enviromental
 * variables, The script should configure the interface, and set any
 * other relavent parameters (default gateway, dns server, etc).
*/
static int bound(const char *wan_ifname)
{
	char *value;
	char tmp[100], prefix[WAN_PREFIX_SZ];
	char wanprefix[WAN_PREFIX_SZ];
	char route[sizeof("255.255.255.255/255")];
	int unit, changed = 0;
	int gateway = 0;

	unit = wans_prefix(wan_ifname, wanprefix, prefix);
	if (unit < 0)
		return EINVAL;

	stop_zcip(unit);

	if ((value = getenv("ip"))) {
		changed = !nvram_match(strcat_r(prefix, "ipaddr", tmp), trim_r(value));
		nvram_set(strcat_r(prefix, "ipaddr", tmp), trim_r(value));
	}
	if ((value = getenv("subnet")))
		nvram_set(strcat_r(prefix, "netmask", tmp), trim_r(value));
        if ((value = getenv("router"))) {
		gateway = 1;
		nvram_set(strcat_r(prefix, "gateway", tmp), trim_r(value));
	}
	if (nvram_get_int(strcat_r(wanprefix, "dnsenable_x", tmp))) {
		if ((value = getenv("dns")) && nvram_get_int(strcat_r(wanprefix, "dnsenable_x", tmp)))
			nvram_set(strcat_r(prefix, "dns", tmp), trim_r(value));
		if ((value = getenv("domain")))
			nvram_set(strcat_r(prefix, "domain", tmp), trim_r(value));
	}
	if ((value = getenv("wins")))
		nvram_set(strcat_r(prefix, "wins", tmp), trim_r(value));

	/* classful static routes */
	nvram_set(strcat_r(prefix, "routes", tmp), getenv("routes"));
	/* ms classless static routes */
	nvram_set(strcat_r(prefix, "routes_ms", tmp), getenv("msstaticroutes"));
	/* rfc3442 classless static routes */
	nvram_set(strcat_r(prefix, "routes_rfc", tmp), getenv("staticroutes"));

	if (!gateway) {
		foreach(route, nvram_safe_get(strcat_r(prefix, "routes_rfc", tmp)), value) {
			if (gateway) {
				nvram_set(strcat_r(prefix, "gateway", tmp), route);
				break;
			} else
				gateway = !strcmp(route, "0.0.0.0/0");
		}
    	}

#if 0
	if ((value = getenv("hostname")))
		sethostname(trim_r(value), strlen(value) + 1);
#endif
	if ((value = getenv("lease"))) {
		nvram_set(strcat_r(prefix, "lease", tmp), trim_r(value));
		expires(wan_ifname, atoi(value));
	}

#ifdef __CONFIG_IPV6__
	if (nvram_match("ipv6_proto", "tun6rd") && nvram_get_int("ipv6_wanauto_x") &&
	    (value = getenv("ip6rd"))) {
		char ip6rd[sizeof("32 128 FFFF:FFFF:FFFF:FFFF:FFFF:FFFF:FFFF:FFFF 255.255.255.255 ")];
		char addrstr[INET6_ADDRSTRLEN];
		char *values[4];
		int i;

		value = strncpy(ip6rd, value, sizeof(ip6rd));
		for (i = 0; i < 4 && value; i++)
			values[i] = strsep(&value, " ");
		if (i == 4) {
			nvram_set(strcat_r(wanprefix, "ipv6_ip4size", tmp), values[0]);
			snprintf(addrstr, sizeof(addrstr), "%s/%s", values[2], values[1]);
			nvram_set(strcat_r(wanprefix, "ipv6_addr", tmp), addrstr);
			nvram_set(strcat_r(wanprefix, "ipv6_relay", tmp), values[3]);
		}
	}
#endif

	if (changed &&
	    nvram_invmatch(strcat_r(wanprefix, "proto", tmp), "l2tp") &&
	    nvram_invmatch(strcat_r(wanprefix, "proto", tmp), "pptp"))
		ifconfig(wan_ifname, IFUP, "0.0.0.0", NULL);
	ifconfig(wan_ifname, IFUP,
		 nvram_safe_get(strcat_r(prefix, "ipaddr", tmp)),
		 nvram_safe_get(strcat_r(prefix, "netmask", tmp)));
	wan_up(wan_ifname);

	logmessage("dhcp client", "%s IP : %s from %s", 
		udhcpstate, 
		nvram_safe_get(strcat_r(prefix, "ipaddr", tmp)), 
		nvram_safe_get(strcat_r(prefix, "gateway", tmp)));
	wanmessage("");

	dprintf("done\n");
	return 0;
}

/*
 * renew: This argument is used when a DHCP lease is renewed. All of
 * the paramaters are set in enviromental variables. This argument is
 * used when the interface is already configured, so the IP address,
 * will not change, however, the other DHCP paramaters, such as the
 * default gateway, subnet mask, and dns server may change.
 */
static int renew(const char *wan_ifname)
{
	char *value;
	char tmp[100], prefix[WAN_PREFIX_SZ];
	char wanprefix[WAN_PREFIX_SZ];
	int metric;
	int unit, changed = 0;

	unit = wans_prefix(wan_ifname, wanprefix, prefix);
	if (unit < 0)
		return EINVAL;

	if (!(value = getenv("subnet")) || nvram_invmatch(strcat_r(prefix, "netmask", tmp), trim_r(value)))
		return bound(wan_ifname);
	if (!(value = getenv("router")) || nvram_invmatch(strcat_r(prefix, "gateway", tmp), trim_r(value)))
		return bound(wan_ifname);

	stop_zcip(unit);

	if (nvram_get_int(strcat_r(wanprefix, "dnsenable_x", tmp))) {
		if ((value = getenv("dns"))) {
			changed += !nvram_match(strcat_r(prefix, "dns", tmp), trim_r(value));
			nvram_set(strcat_r(prefix, "dns", tmp), trim_r(value));
		}
		if ((value = getenv("domain"))) {
			changed += !nvram_match(strcat_r(prefix, "domain", tmp), trim_r(value));
			nvram_set(strcat_r(prefix, "domain", tmp), trim_r(value));
		}
	}
	if ((value = getenv("wins")))
		nvram_set(strcat_r(prefix, "wins", tmp), trim_r(value));

#if 0
	if ((value = getenv("hostname")))
		sethostname(trim_r(value), strlen(value) + 1);
#endif

	if ((value = getenv("lease"))) {
		nvram_set(strcat_r(prefix, "lease", tmp), trim_r(value));
		expires(wan_ifname, atoi(value));
	}

	if (changed) {
		metric = nvram_get_int(strcat_r(wanprefix, "priority", tmp));
		update_resolvconf(wan_ifname, metric);
	}

	if (changed &&
	    nvram_invmatch(strcat_r(wanprefix, "proto", tmp), "l2tp") &&
	    nvram_invmatch(strcat_r(wanprefix, "proto", tmp), "pptp") &&
	    nvram_invmatch(strcat_r(wanprefix, "proto", tmp), "pppoe"))
		update_wan_status(unit, WAN_STATUS_CONNECTED);

	//logmessage("dhcp client", "%s IP : %s from %s", 
	//		udhcpstate, 
	//		nvram_safe_get(strcat_r(prefix, "ipaddr", tmp)), 
	//		nvram_safe_get(strcat_r(prefix, "gateway", tmp)));
	wanmessage("");

	dprintf("done\n");
	return 0;
}

static int leasefail(const char *wan_ifname)
{
	char prefix[WAN_PREFIX_SZ];
	char pid[sizeof("/var/run/zcipXXXXXXXXXX.pid")];
	int unit;

	unit = wanx_prefix(wan_ifname, prefix);
	if (unit < 0)
		return EINVAL;

	snprintf(pid, sizeof(pid), "/var/run/zcip%d.pid", unit);
	if (kill_pidfile_s(pid, 0) == 0)
		return 0;

	return start_zcip(wan_ifname, unit);
}

int udhcpc_main(int argc, char **argv)
{
	const char *wan_ifname;

	if (argc < 2 || !argv[1])
		return EINVAL;

	wan_ifname = safe_getenv("interface");
	strlcpy(udhcpstate, argv[1], sizeof(udhcpstate));

	if (!strcmp(argv[1], "deconfig"))
		return deconfig(wan_ifname, 0);
	else if (!strcmp(argv[1], "bound"))
		return bound(wan_ifname);
	else if (!strcmp(argv[1], "renew"))
		return renew(wan_ifname);
	else if (!strcmp(argv[1], "leasefail"))
		return leasefail(wan_ifname);
	else /* if (!strcmp(argv[1], "nak")) */
		return 0;
}

int start_dhcpc(const char *wan_ifname, int unit)
{
	char tmp[100], prefix[WAN_PREFIX_SZ];
	char pid[sizeof("/var/run/udhcpcXXXXXXXXXX.pid")];
	char *wan_hostname;
	char *dhcp_argv[] = {
		"/sbin/udhcpc",
		"-i", (char *)wan_ifname,
		"-p", (snprintf(pid, sizeof(pid), "/var/run/udhcpc%d.pid", unit), pid),
		"-b",
		NULL, NULL,	/* -H wan_hostname	*/
		NULL,		/* -O routes		*/
		NULL,		/* -O staticroutes	*/
		NULL,		/* -O msstaticroutes	*/
#ifdef __CONFIG_IPV6__
		NULL,		/* -O 6rd		*/
		NULL,		/* -O comcast6rd	*/
#endif
#ifdef DEBUG
		NULL,		/* -vvS			*/
#endif
		NULL
	};
	int index = 6;		/* first NULL index	*/

	/* We have to trust unit */
	snprintf(prefix, sizeof(prefix), "wan%d_", unit);

	wan_hostname = nvram_safe_get(strcat_r(prefix, "hostname", tmp));
	if (*wan_hostname) {
		dhcp_argv[index++] = "-H";
		dhcp_argv[index++] = wan_hostname;
	}

	if (nvram_match("dr_enable_x", "1")) {
		dhcp_argv[index++] = "-O33";	/* "routes" */
		dhcp_argv[index++] = "-O121";	/* "staticroutes" */
		dhcp_argv[index++] = "-O249";   /* "msstaticroutes" */
	}

#ifdef __CONFIG_IPV6__
	if (nvram_match("ipv6_proto", "tun6rd") && nvram_get_int("ipv6_wanauto_x")) {
		dhcp_argv[index++] = "-O212";	/* "6rd" */
		dhcp_argv[index++] = "-O150";	/* "comcast6rd" */
	}
#endif
#ifdef DEBUG
	dhcp_argv[index++] = "-vvS";
#endif

	stop_zcip(unit);

	return _eval(dhcp_argv, NULL, 0, NULL);
}

void stop_dhcpc(int unit)
{
	char pid[sizeof("/var/run/udhcpcXXXXXXXXXX.pid")];

	stop_zcip(unit);

	snprintf(pid, sizeof(pid), "/var/run/udhcpc%d.pid", unit);
	kill_pidfile_s(pid, SIGUSR2);
	usleep(10000);
	kill_pidfile_s(pid, SIGTERM);
}

void renew_dhcpc(int unit)
{
	char pid[sizeof("/var/run/udhcpcXXXXXXXXXX.pid")];

	snprintf(pid, sizeof(pid), "/var/run/udhcpc%d.pid", unit);
	kill_pidfile_s(pid, SIGUSR1);
}

static int config(const char *wan_ifname)
{
	char *value;
	char tmp[100], prefix[WAN_PREFIX_SZ];
	char wanprefix[WAN_PREFIX_SZ];
	int changed = 0;

	if (wans_prefix(wan_ifname, wanprefix, prefix) < 0)
		return EINVAL;

	if ((value = getenv("ip"))) {
		changed = !nvram_match(strcat_r(prefix, "ipaddr", tmp), trim_r(value));
		nvram_set(strcat_r(prefix, "ipaddr", tmp), trim_r(value));
	}
	nvram_set(strcat_r(prefix, "netmask", tmp), "255.255.0.0");
	nvram_set(strcat_r(prefix, "gateway", tmp), "");
	if (nvram_get_int(strcat_r(wanprefix, "dnsenable_x", tmp))) {
		nvram_set(strcat_r(prefix, "dns", tmp), "");
		nvram_set(strcat_r(prefix, "domain", tmp), "");
	}

	if (changed &&
	    nvram_invmatch(strcat_r(wanprefix, "proto", tmp), "l2tp") &&
	    nvram_invmatch(strcat_r(wanprefix, "proto", tmp), "pptp"))
		ifconfig(wan_ifname, IFUP, "0.0.0.0", NULL);
	ifconfig(wan_ifname, IFUP,
		 nvram_safe_get(strcat_r(prefix, "ipaddr", tmp)),
		 nvram_safe_get(strcat_r(prefix, "netmask", tmp)));
	wan_up(wan_ifname);

	logmessage("zcip client", "%s IP : %s", 
		udhcpstate,
		nvram_safe_get(strcat_r(prefix, "ipaddr", tmp)));
	wanmessage("");

	dprintf("done\n");
	return 0;
}

int zcip_main(int argc, char **argv)
{
	const char *wan_ifname;

	if (argc < 2 || !argv[1])
		return EINVAL;

	wan_ifname = safe_getenv("interface");
	strlcpy(udhcpstate, argv[1], sizeof(udhcpstate));

	if (!strcmp(argv[1], "deconfig"))
		return deconfig(wan_ifname, 1);
	else if (!strcmp(argv[1], "config"))
		return config(wan_ifname);
	/* init */
	else return 0;
}

int start_zcip(const char *wan_ifname, int unit)
{
	char pid[sizeof("/var/run/zcipXXXXXXXXXX.pid")];
	char *zcip_argv[] = {
		"/sbin/zcip",
		"-p", (snprintf(pid, sizeof(pid), "/var/run/zcip%d.pid", unit), pid),
		(char *)wan_ifname,
		"/tmp/zcip.script",
		NULL
	};

	return _eval(zcip_argv, NULL, 0, NULL);
}

void stop_zcip(int unit)
{
	char pid[sizeof("/var/run/zcipXXXXXXXXXX.pid")];

	snprintf(pid, sizeof(pid), "/var/run/zcip%d.pid", unit);
	kill_pidfile_s(pid, SIGTERM);
}

#ifdef __CONFIG_IPV6__
#ifdef __CONFIG_ODHCP6C__
static int deconfig6(const char *wan_ifname)
{
	char tmp[100], prefix[WAN_PREFIX_SZ];
	char wanprefix[WAN_PREFIX_SZ];
	char *lan_ifname = nvram_safe_get("lan_ifname");
	char *value;
#ifdef __CONFIG_RADVD__
	int prefix_changed = 0;
#endif
	int dns_changed = 0;

	if (wans_prefix(wan_ifname, wanprefix, prefix) < 0)
		return EINVAL;

	/* Delete WAN address */
	if (nvram_get_int("ipv6_wanauto_x")) {
		value = nvram_safe_get(strcat_r(wanprefix, "ipv6_addr_t", tmp));
		if (*value)
			eval("ip", "-6", "addr", "del", value, "dev", wan_ifname);
		nvram_unset(strcat_r(wanprefix, "ipv6_addr", tmp));
		nvram_unset(strcat_r(wanprefix, "ipv6_addr_t", tmp));
	}

	/* Delete LAN address */
	if (nvram_get_int("ipv6_lanauto_x")) {
		value = nvram_safe_get("lan_ipv6_addr");
#ifdef __CONFIG_RADVD__
		prefix_changed = *value;
#endif
		if (*value)
			eval("ip", "-6", "addr", "del", value, "dev", lan_ifname);
		nvram_unset("lan_ipv6_addr");
	}

	if (!nvram_invmatch("ipv6_dnsenable_x", "1")) {
		value = nvram_safe_get(strcat_r(wanprefix, "ipv6_dns", tmp));
		dns_changed = *value;
		nvram_unset(strcat_r(wanprefix, "ipv6_dns", tmp));
	}

	if (dns_changed) {
		int metric = nvram_get_int(strcat_r(wanprefix, "priority", tmp));
		update_resolvconf(wan_ifname, metric);
	}

#ifdef __CONFIG_RADVD__
	/* Notify radvd of possible change */
	if (prefix_changed && nvram_get_int("ipv6_radvd_enable") == 2)
		killall_s("radvd", SIGHUP);
#endif

	logmessage("dhcp6 client", "%s: lease is lost", dhcp6state);
	//wanmessage("lost IPv6 from server");

	dprintf("done\n");
	return 0;
}

static int bound6(const char *wan_ifname, int bound)
{
	char tmp[100], prefix[WAN_PREFIX_SZ];
	char wanprefix[WAN_PREFIX_SZ];
	char *lan_ifname = nvram_safe_get("lan_ifname");
	char addrstr[INET6_ADDRSTRLEN];
	char word[100], *next;
	char *value, *valid = NULL, *preferred = NULL;
	int wanaddr_changed = 0, lanaddr_changed = 0, dns_changed = 0;

	if (wans_prefix(wan_ifname, wanprefix, prefix) < 0)
		return EINVAL;

	if (nvram_get_int("ipv6_wanauto_x")) {
		value = safe_getenv("ADDRESSES");
		if (*value) {
			foreach(word, value, next) {
				char *ptr = word;
				value = strsep(&ptr,",");
				preferred = strsep(&ptr,",");
				valid = strsep(&ptr,",");
				break; /* only first address at the moment */
			}
		}
		next = nvram_safe_get(strcat_r(wanprefix, "ipv6_addr_t", tmp));
		wanaddr_changed = strcmp(value, next);
		if (wanaddr_changed) {
			if (*next)
				eval("ip", "-6", "addr", "del", next, "dev", wan_ifname);
			nvram_set(strcat_r(wanprefix, "ipv6_addr", tmp), value);
			nvram_set(strcat_r(wanprefix, "ipv6_addr_t", tmp), value);
		}
		if (*value) {
			eval("ip", "-6", "addr", "add", value, "dev", wan_ifname);
			eval("ip", "-6", "addr", "change", value, "dev", wan_ifname,
			     valid ? : "valid_lft", valid,
			     preferred ? : "preferred_lft", preferred);
		}
	}

	if (nvram_get_int("ipv6_lanauto_x")) {
		value = safe_getenv("PREFIXES");
		if (*value) {
			struct in6_addr addr;
			int size;

			foreach(word, value, next) {
				char *ptr = word;
				value = strsep(&ptr,",");
				preferred = strsep(&ptr,",");
				valid = strsep(&ptr,",");
				break; /* only first prefix at the moment */
			}

			size = ipv6_addr(value, &addr);
			if (size > 0) {
				addr.s6_addr16[7] |= htons(0x0001);
				inet_ntop(AF_INET6, &addr, addrstr, INET6_ADDRSTRLEN);
				sprintf(addrstr, "%s/%d", addrstr, size);
				value = addrstr;
			}
		}
		next = nvram_safe_get("lan_ipv6_addr");
		lanaddr_changed = strcmp(value, next);
		if (lanaddr_changed) {
			if (*next)
				eval("ip", "-6", "addr", "del", next, "dev", lan_ifname);
			nvram_set("lan_ipv6_addr", value);
		}
		if (*value) {
			eval("ip", "-6", "addr", "add", value, "dev", lan_ifname);
			eval("ip", "-6", "addr", "change", value, "dev", lan_ifname,
			     valid ? : "valid_lft", valid,
			     preferred ? : "preferred_lft", preferred);
		}
	}

	if (!nvram_invmatch("ipv6_dnsenable_x", "1")) {
		value = safe_getenv("RDNSS");
		if (*value == '\0')
			value = safe_getenv("RA_DNS");
		dns_changed = !nvram_match(strcat_r(wanprefix, "ipv6_dns", tmp), value);
		nvram_set(strcat_r(wanprefix, "ipv6_dns", tmp), value);
	}

	if (dns_changed) {
		int metric = nvram_get_int(strcat_r(wanprefix, "priority", tmp));
		update_resolvconf(wan_ifname, metric);
	}

#ifdef __CONFIG_RADVD__
	/* Notify radvd of possible change */
	if (lanaddr_changed && nvram_get_int("ipv6_radvd_enable") == 2)
		killall_s("radvd", SIGHUP);
#endif

	if (bound == 1 || wanaddr_changed)
		logmessage("dhcp6 client", "%s %s IP : %s", 
			dhcp6state, "WAN",
			nvram_safe_get(strcat_r(wanprefix, "ipv6_addr", tmp)));
	if (bound == 1 || lanaddr_changed)
		logmessage("dhcp6 client", "%s %s IP : %s", 
			dhcp6state, "LAN",
			nvram_safe_get("lan_ipv6_addr"));
	//wanmessage("");

	dprintf("done\n");
	return 0;
}

static int ra_updated6(const char *wan_ifname)
{
	char tmp[100], prefix[WAN_PREFIX_SZ];
	char wanprefix[WAN_PREFIX_SZ];
	char *value;
	int dns_changed = 0;

	if (wans_prefix(wan_ifname, wanprefix, prefix) < 0)
		return EINVAL;

	if (!nvram_invmatch("ipv6_dnsenable_x", "1")) {
		value = safe_getenv("RDNSS");
		if (*value == '\0')
			value = safe_getenv("RA_DNS");
		dns_changed = !nvram_match(strcat_r(wanprefix, "ipv6_dns", tmp), value);
		nvram_set(strcat_r(wanprefix, "ipv6_dns", tmp), value);
	}

	if (dns_changed) {
		int metric = nvram_get_int(strcat_r(wanprefix, "priority", tmp));
		update_resolvconf(wan_ifname, metric);
	}

	dprintf("done\n");
	return 0;
}

int dhcp6c_main(int argc, char **argv)
{
	const char *wan_ifname;

	if (argc < 3 || !argv[1] || !argv[2])
		return EINVAL;

	wan_ifname = argv[1];
	strlcpy(dhcp6state, argv[2], sizeof(dhcp6state));

	if (strcmp(argv[2], "started") == 0 ||
	    strcmp(argv[2], "stopped") == 0 ||
	    strcmp(argv[2], "unbound") == 0)
		return deconfig6(wan_ifname);
	else if (strcmp(argv[2], "bound") == 0)
		return bound6(wan_ifname, 1);
	else if (strcmp(argv[2], "updated") == 0 ||
		 strcmp(argv[2], "rebound") == 0)
		return bound6(wan_ifname, 2);
	else if (strcmp(argv[2], "informed") == 0)
		return bound6(wan_ifname, 0);
	else if (strcmp(argv[2], "ra-updated") == 0)
		return ra_updated6(wan_ifname);

	return 0;
}

#else
int dhcp6c_main(int argc, char **argv)
{
	const char *wan_ifname = safe_getenv("interface");
	char *value;
	char tmp[100], prefix[WAN_PREFIX_SZ];
	char wanprefix[WAN_PREFIX_SZ];
	int metric, changed = 0;

	if (wans_prefix(wan_ifname, wanprefix, prefix) < 0)
		return EINVAL;

	if (!nvram_invmatch("ipv6_dnsenable_x", "1") &&
	    (value = getenv("new_domain_name_servers"))) {
		changed = !nvram_match(strcat_r(wanprefix, "ipv6_dns", tmp), trim_r(value));
		nvram_set(strcat_r(wanprefix, "ipv6_dns", tmp), trim_r(value));
	}

	if (changed) {
		metric = nvram_get_int(strcat_r(wanprefix, "priority", tmp));
		update_resolvconf(wan_ifname, metric);
	}

#ifdef __CONFIG_RADVD__
	/* Notify radvd of possible change */
	if (nvram_get_int("ipv6_radvd_enable") == 2)
		killall_s("radvd", SIGHUP);
#endif

	return 0;
}
#endif

int start_dhcp6c(const char *wan_ifname)
{
	char tmp[100], prefix[WAN_PREFIX_SZ];
	unsigned long iaid;
	struct duid duid;
#ifdef __CONFIG_ODHCP6C__
	char duid_arg[sizeof(duid)*2+1];
	char prefix_arg[sizeof("128:xxxxxxxx")];
	char *dhcp6c_argv[] = {
		"/usr/sbin/odhcp6c",
		"-dfR",
		"-s", "/tmp/dhcp6c.script",
		"-p", "/var/run/dhcp6c.pid",
		NULL, NULL,	/* -c duid		*/
		NULL, NULL,	/* -N mode		*/
		NULL, NULL,	/* -FP len:iaidhex	*/
		NULL,		/* -rdns		*/
		NULL, NULL, 	/* -rsolmaxrt -r infmaxrt */
#ifdef DEBUG
		NULL,		/* -v			*/
#endif
		NULL,		/* interface */
		NULL };
	int index = 6;		/* first NULL index	*/
#else
	FILE *fp;
	pid_t pid;
	char *dhcp6c_argv[] = {
		"/sbin/dhcp6c",
		"-D", "LL",
		"-v",
		(char *)wan_ifname,
		NULL
	};
#endif

	if (!nvram_match("ipv6_proto", "dhcp6"))
		return 1;

	if (wans_prefix(wan_ifname, prefix, tmp) < 0)
		return 1;

	stop_dhcp6c();

#ifdef __CONFIG_ODHCP6C__
	if (get_duid(&duid)) {
		bin2hex(duid_arg, sizeof(duid_arg), &duid, sizeof(duid));
		dhcp6c_argv[index++] = "-c";
		dhcp6c_argv[index++] = duid_arg;
	}

	dhcp6c_argv[index++] = "-N";
	dhcp6c_argv[index++] = nvram_get_int("ipv6_wanauto_x") ? "try" : "none";

	if (nvram_get_int("ipv6_lanauto_x")) {
		/* Generate IA_PD IAID from the last 7 digits of WAN MAC */
		iaid = ether_atoe(nvram_safe_get(strcat_r(prefix, "hwaddr", tmp)), duid.ea) ?
			((unsigned long)(duid.ea[3] & 0x0f) << 16) |
			((unsigned long)(duid.ea[4]) << 8) |
			((unsigned long)(duid.ea[5])) : 1;
		snprintf(prefix_arg, sizeof(prefix_arg), "%d:%lx", 0, iaid);
		dhcp6c_argv[index++] = "-FP";
		dhcp6c_argv[index++] = prefix_arg;
	}

	if (!nvram_invmatch("ipv6_dnsenable_x", "1"))
		dhcp6c_argv[index++] = "-r23";	/* dns */

	dhcp6c_argv[index++] = "-r82";	/* sol_max_rt */
	dhcp6c_argv[index++] = "-r83";	/* inf_max_rt */

#ifdef DEBUG
	dhcp6c_argv[index++] = "-v";
#endif
	dhcp6c_argv[index++] = (char *)wan_ifname;

	return _eval(dhcp6c_argv, NULL, 0, NULL);
#else
	/* Generate IAID from the last 7 digits of WAN MAC */
	iaid = ether_atoe(nvram_safe_get(strcat_r(prefix, "hwaddr", tmp)), duid.ea) ?
		((unsigned long)(duid.ea[3] & 0x0f) << 16) |
		((unsigned long)(duid.ea[4]) << 8) |
		((unsigned long)(duid.ea[5])) : 1;

	/* Create dhcp6c_duid */
	unlink("/var/state/dhcp6c_duid");
	if (get_duid(&duid) &&
	    (fp = fopen("/var/state/dhcp6c_duid", "w")) != NULL) {
		uint16 duid_len = sizeof(duid);
		fwrite(&duid_len, sizeof(duid_len), 1, fp);
		fwrite(&duid, sizeof(duid), 1, fp);
		fclose(fp);
	}

	/* Create config */
	if ((fp = fopen("/etc/dhcp6c.conf", "w")) == NULL) {
		perror("/etc/dhcp6c.conf");
		return 2;
	}
	fprintf(fp,
		"interface %s {\n"
		  "%ssend ia-pd %lu;\n"
		  "%ssend ia-na %lu;\n"
		    "send rapid-commit;\n"		/* May cause unexpected advertise in case of server don't support rapid-commit */
		    "request domain-name-servers;\n"
		    "script \"%s\";\n"
		"};\n"
		"id-assoc pd %lu {\n"
		    "prefix-interface %s {\n"
			"sla-id %lu;\n"
			"sla-len %d;\n"
		    "};\n"
		"};\n"
		"id-assoc na %lu { };\n",
		wan_ifname,
		nvram_get_int("ipv6_lanauto_x") ? "" : "#", iaid,
		nvram_get_int("ipv6_wanauto_x") ? "" : "#", /* wan iaid */ 1UL,
		"/tmp/dhcp6c.script",
		iaid, nvram_safe_get("lan_ifname"), 0UL, 0,
		/* wan iaid */ 1UL);
	fclose(fp);

	return _eval(dhcp6c_argv, NULL, 0, &pid);
#endif
}

void stop_dhcp6c(void)
{
	killall_s("dhcp6c.script", SIGTERM);
	kill_pidfile("/var/run/dhcp6c.pid");
}

#endif
