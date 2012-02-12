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

#include "rc.h"

static char udhcpstate[12];

static int start_zcip(const char *wan_ifname);
static void stop_zcip(void);


static int expires(const char *wan_ifname, unsigned int in)
{
	time_t now;
	FILE *fp;
	char tmp[100], prefix[sizeof("wanXXXXXXXXXX_")];
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
	char tmp[100], prefix[sizeof("wanXXXXXXXXXX_")];
	char wanprefix[sizeof("wanXXXXXXXXXX_")];

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
	char tmp[100], prefix[sizeof("wanXXXXXXXXXX_")];
	char wanprefix[sizeof("wanXXXXXXXXXX_")];
	char route[sizeof("255.255.255.255/255")];
	int changed = 0;
	int gateway = 0;

	stop_zcip();

	if (wans_prefix(wan_ifname, wanprefix, prefix) < 0)
		return EINVAL;

	if ((value = getenv("ip"))) {
		changed = nvram_invmatch(strcat_r(prefix, "ipaddr", tmp), value);
		nvram_set(strcat_r(prefix, "ipaddr", tmp), trim_r(value));
	}
	if ((value = getenv("subnet")))
		nvram_set(strcat_r(prefix, "netmask", tmp), trim_r(value));
        if ((value = getenv("router"))) {
		gateway = 1;
		nvram_set(strcat_r(prefix, "gateway", tmp), trim_r(value));
	}
	if ((value = getenv("dns")))
		nvram_set(strcat_r(prefix, "dns", tmp), trim_r(value));
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
	if ((value = getenv("domain")))
		nvram_set(strcat_r(prefix, "domain", tmp), trim_r(value));
	if ((value = getenv("lease"))) {
		nvram_set(strcat_r(prefix, "lease", tmp), trim_r(value));
		expires(wan_ifname, atoi(value));
	}

#ifdef __CONFIG_IPV6__
	if ((value = getenv("ip6rd"))) {
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
	char tmp[100], prefix[sizeof("wanXXXXXXXXXX_")];
	char wanprefix[sizeof("wanXXXXXXXXXX_")];
	int metric;
	int changed = 0;

	stop_zcip();

	if (wans_prefix(wan_ifname, wanprefix, prefix) < 0)
		return EINVAL;

	if (!(value = getenv("subnet")) || nvram_invmatch(strcat_r(prefix, "netmask", tmp), trim_r(value)))
		return bound(wan_ifname);
	if (!(value = getenv("router")) || nvram_invmatch(strcat_r(prefix, "gateway", tmp), trim_r(value)))
		return bound(wan_ifname);

	if ((value = getenv("dns"))) {
		changed = nvram_invmatch(strcat_r(prefix, "dns", tmp), trim_r(value));
		nvram_set(strcat_r(prefix, "dns", tmp), trim_r(value));
	}
	if ((value = getenv("wins")))
		nvram_set(strcat_r(prefix, "wins", tmp), trim_r(value));

#if 0
	if ((value = getenv("hostname")))
		sethostname(trim_r(value), strlen(value) + 1);
#endif

	if ((value = getenv("domain")))
		nvram_set(strcat_r(prefix, "domain", tmp), trim_r(value));
	if ((value = getenv("lease"))) {
		nvram_set(strcat_r(prefix, "lease", tmp), trim_r(value));
		expires(wan_ifname, atoi(value));
	}

	if (changed) {
		metric = nvram_get_int(strcat_r(wanprefix, "priority", tmp));
		update_resolvconf(wan_ifname, metric, 1);
	}

	if (changed &&
	    nvram_invmatch(strcat_r(wanprefix, "proto", tmp), "l2tp") &&
	    nvram_invmatch(strcat_r(wanprefix, "proto", tmp), "pptp") &&
	    nvram_invmatch(strcat_r(wanprefix, "proto", tmp), "pppoe"))
		update_wan_status(1);

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
	char tmp[100], prefix[sizeof("wanXXXXXXXXXX_")];

	if (wanx_prefix(wan_ifname, prefix) < 0)
		return EINVAL;

	if ((ip_addr(nvram_safe_get(strcat_r(prefix, "ipaddr", tmp))) &
	     ip_addr(nvram_safe_get(strcat_r(prefix, "netmask", tmp)))) ==
	     ip_addr("169.254.0.0"))
		return 0;

	return start_zcip(wan_ifname);
}

int udhcpc_main(int argc, char **argv)
{
	const char *wan_ifname;

	if (argc<2 || !argv[1])
		return EINVAL;

	wan_ifname = safe_getenv("interface");
	strcpy(udhcpstate, argv[1]);

	if (!strcmp(argv[1], "deconfig"))
		return deconfig(wan_ifname, 0);
	else if (!strcmp(argv[1], "bound"))
		return bound(wan_ifname);
	else if (!strcmp(argv[1], "renew"))
		return renew(wan_ifname);
	else if (!strcmp(argv[1], "leasefail"))
		return leasefail(wan_ifname);
	/* nak */
	else
		return 0;
}

int start_dhcpc(const char *wan_ifname, int unit)
{
	char tmp[100], prefix[sizeof("wanXXXXXXXXXX_")];
	char pid[sizeof("/var/run/udhcpcXXXXXXXXXX.pid")];
	char *wan_hostname;
	char *dhcp_argv[] = {
		"/sbin/udhcpc",
		"-i", (char *)wan_ifname,
		"-p", (snprintf(pid, sizeof(pid), "/var/run/udhcpc%d.pid", unit), pid),
		"-bN61",
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
	if (nvram_match("ipv6_proto", "tun6rd")) {
		dhcp_argv[index++] = "-O212";	/* "6rd" */
		dhcp_argv[index++] = "-O150";	/* "comcast6rd" */
	}
#endif
#ifdef DEBUG
	dhcp_argv[index++] = "-vvS";
#endif

	return _eval(dhcp_argv, NULL, 0, NULL);
}

static int config(const char *wan_ifname)
{
	char *value;
	char tmp[100], prefix[sizeof("wanXXXXXXXXXX_")];
	char wanprefix[sizeof("wanXXXXXXXXXX_")];
	int changed = 0;

	if (wans_prefix(wan_ifname, wanprefix, prefix) < 0)
		return EINVAL;

	if ((value = getenv("ip"))) {
		changed = nvram_invmatch(strcat_r(prefix, "ipaddr", tmp), value);
		nvram_set(strcat_r(prefix, "ipaddr", tmp), trim_r(value));
	}
	nvram_set(strcat_r(prefix, "netmask", tmp), "255.255.0.0");
	nvram_set(strcat_r(prefix, "gateway", tmp), "");
	nvram_set(strcat_r(prefix, "dns", tmp), "");

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

	if (argc<2 || !argv[1])
		return EINVAL;

	wan_ifname = safe_getenv("interface");
	strcpy(udhcpstate, argv[1]);

	if (!strcmp(argv[1], "deconfig"))
		return deconfig(wan_ifname, 1);
	else if (!strcmp(argv[1], "config"))
		return config(wan_ifname);
	/* init */
	else return 0;
}

static int start_zcip(const char *wan_ifname)
{
	char *zcip_argv[] = { "/sbin/zcip", "-q", (char *)wan_ifname, "/tmp/zcip.script", NULL };

	return _eval(zcip_argv, NULL, 0, NULL);
}

static void stop_zcip(void)
{
	killall_s("zcip.script", SIGTERM);
	killall("zcip");
}

#ifdef __CONFIG_IPV6__
int dhcp6c_main(int argc, char **argv)
{
	char *wan_ifname = safe_getenv("interface");
	char *dns6 = getenv("new_domain_name_servers");

	if (dns6) {
//TODO: Process new DNS records
		update_resolvconf(wan_ifname, 2, 1);
	}
	// notify radvd of possible change
	killall_s("radvd", SIGHUP);

	return 0;
}

int start_dhcp6c(const char *wan_ifname)
{
	FILE *fp;
	pid_t pid;
	int sla_len, ret, is_wan6_valid;
	struct in6_addr wan6_addr;
	char *dhcp6c_argv[] = { "/sbin/dhcp6c", "-v", "-D", "LL", (char *)wan_ifname, NULL };

	if (!nvram_match("ipv6_proto", "dhcp6")) return 1;

	stop_dhcp6c();

	sla_len = 64 - atoi(nvram_safe_get("ipv6_lan_netsize"));
	if (sla_len <= 0)
		sla_len = 0;
	else if (sla_len > 16)
		sla_len = 16;
	is_wan6_valid = ipv6_addr(nvram_safe_get("wan0_ipv6_addr"), &wan6_addr);

	if ((fp = fopen("/etc/dhcp6c.conf", "w")) == NULL) {
		perror("/etc/dhcp6c.conf");
                return 2;
        }
	fprintf(fp, "interface %s {\n"
		    " send ia-pd 0;\n"
		    "%s"
		    " send rapid-commit;\n"		/* May cause unexpected advertise in case of server don't support rapid-commit */
		    " request domain-name-servers;\n"
		    " script \"/tmp/dhcp6c.script\";\n"
		    "};\n"
		    "id-assoc pd 0 {\n"
		    " prefix-interface %s {\n"
		    "  sla-id 0;\n"
		    "  sla-len %d;\n"
		    " };\n"
		    "};\n"
		    "id-assoc na 0 { };\n",
		    wan_ifname,
		    (is_wan6_valid > 0) ? "" : " send ia-na 0;\n",
		    nvram_safe_get("lan_ifname"), sla_len
		);
        fclose(fp);
	ret = _eval(dhcp6c_argv, NULL, 0, &pid);

        return ret;
}

void stop_dhcp6c(void)
{
	killall_s("dhcp6c.script", SIGTERM);
	kill_pidfile("/var/run/dhcp6c.pid");
}

#endif
