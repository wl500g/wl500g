/*
 * ppp scripts
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
#include <unistd.h>
#include <string.h>
#include <net/route.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <net/if.h>
#include <arpa/inet.h>
#include <errno.h>
#include <ctype.h>

#include <netconf.h>
#include <nvparse.h>
#include "rc.h"

/*
* parse ifname to retrieve unit #
*/
int ppp_ifunit(const char *ifname)
{
	char tmp[100], prefix[WAN_PREFIX_SZ];
	int unit;

	for (unit = 0; unit < MAX_NVPARSE; unit ++) {
		snprintf(prefix, sizeof(prefix), "wan%d_", unit);
		if (nvram_match(strcat_r(prefix, "pppoe_ifname", tmp), ifname))
			return unit;
	}

	return -1;
}

static int
ppp_linkunit(char *linkname)
{
	if (strncmp(linkname, "wan", 3))
		return -1;
	if (!isdigit(linkname[3]))
		return -1;
	return atoi(&linkname[3]);
}

static int ppp_prefix(char **wan_ifname, char *prefix)
{
	int unit;

	*wan_ifname = safe_getenv("IFNAME");

	if ((unit = ppp_linkunit(safe_getenv("LINKNAME"))) < 0)
		return -1;

	sprintf(prefix, "wan%d_", unit);

	return unit;
}

/*
 * Called when link comes up
 */
int
ipup_main(int argc, char **argv)
{
	FILE *fp;
	char tmp[100], prefix[WAN_PREFIX_SZ];
	char buf[100], *value;
	char *wan_ifname;

	if (ppp_prefix(&wan_ifname, prefix) < 0)
		return -1;

	umask(022);

	/* Touch connection file */
	if (!(fp = fopen(strcat_r("/tmp/ppp/link.", wan_ifname, tmp), "a"))) {
		perror(tmp);
		return errno;
	}
	fclose(fp);

	if ((value = getenv("IPLOCAL"))) {
		if (nvram_invmatch(strcat_r(prefix, "ipaddr", tmp), value))
			ifconfig(wan_ifname, IFUP, "0.0.0.0", NULL);
		_ifconfig(wan_ifname, IFUP,
			 value, "255.255.255.255", getenv("IPREMOTE"));
		nvram_set(strcat_r(prefix, "ipaddr", tmp), value);
		nvram_set(strcat_r(prefix, "netmask", tmp), "255.255.255.255");
	}

        if ((value = getenv("IPREMOTE")))
		nvram_set(strcat_r(prefix, "gateway", tmp), value);

	strcpy(buf, "");
	if ((value = getenv("DNS1")))
		sprintf(buf, "%s", value);
	if ((value = getenv("DNS2")))
		sprintf(buf + strlen(buf), "%s%s", *buf ? " " : "", value);
	if (strlen(buf) == 0 && !nvram_get_int(strcat_r(prefix, "dnsenable_x", tmp))) {
		value = nvram_safe_get(strcat_r(prefix, "dns1_x", tmp));
		if (*value && inet_addr(value) != INADDR_ANY)
			sprintf(buf, "%s", value);
		value = nvram_safe_get(strcat_r(prefix, "dns2_x", tmp));
		if (*value && inet_addr(value) != INADDR_ANY)
			sprintf(buf + strlen(buf), "%s%s", *buf ? " " : "", value);
	}
	nvram_set(strcat_r(prefix, "dns", tmp), buf);

	wan_up(wan_ifname);

	logmessage(nvram_safe_get("wan_proto_t"), "connected to ISP");
	wanmessage("");

	dprintf("done\n");
	return 0;
}

/*
 * Called when link goes down
 */
int
ipdown_main(int argc, char **argv)
{
	char tmp[100], prefix[WAN_PREFIX_SZ];
	char *wan_ifname;

	if (ppp_prefix(&wan_ifname, prefix) < 0)
		return -1;

	umask(022);

	/* leave as is if no dns servers left for demand to work */
	nvram_unset(strcat_r(prefix, "dns", tmp));

	wan_down(wan_ifname);

	unlink(strcat_r("/tmp/ppp/link.", wan_ifname, tmp));

	preset_wan_routes(wan_ifname);

	logmessage(nvram_safe_get("wan_proto_t"), "Disconnected");
	wanmessage(pppstatus(tmp));

	return 0;
}

/*
 * Called before link comes up
 */
int
ippreup_main(int argc, char **argv)
{
	char tmp[100], prefix[WAN_PREFIX_SZ];
	char *wan_ifname;

	if (ppp_prefix(&wan_ifname, prefix) < 0)
		return -1;

	nvram_set(strcat_r(prefix, "pppoe_ifname", tmp), wan_ifname);

	dprintf("done\n");
	return 0;
}

#ifdef __CONFIG_IPV6__
int
ip6up_main(int argc, char **argv)
{
	char *wan_ifname;
	//char *value;
	int unit;
	char tmp[100], prefix[WAN_PREFIX_SZ];

	if ((unit = ppp_prefix(&wan_ifname, prefix)) < 0)
		return -1;

	if (nvram_get_int(strcat_r(prefix, "ipv6_if_x", tmp)))
		return 0;

	//if ((value = getenv("LLLOCAL")))
	//	eval("ip", "-6", "addr", "add", value, "dev", wan_ifname);
	//if ((value = getenv("LLREMOTE")))
	//	nvram_set(strcat_r(prefix, "ipv6_router", tmp), value);

	/* Start IPv6 */
	if (nvram_match("ipv6_proto", "native") ||
	    nvram_match("ipv6_proto", "dhcp6"))
		wan6_up(wan_ifname, unit);

	return 0;
}

int
ip6down_main(int argc, char **argv)
{
	char *wan_ifname;
	int unit;
	char tmp[100], prefix[WAN_PREFIX_SZ];

	if ((unit = ppp_prefix(&wan_ifname, prefix)) < 0)
		return -1;

	if (nvram_get_int(strcat_r(prefix, "ipv6_if_x", tmp)))
		return 0;

	wan6_down(wan_ifname, unit);

	return 0;
}
#endif

#ifdef PPPD_AUTH_UNUSED
/*
 * Called after the remote/local system successfully authenticates itself.
 * It is executed with the parameters:
 * interface-name peer-name user-name tty-device speed
 * Note: that this script is not executed if the peer doesn't authenticate itself,
 * for example when the noauth option is used.
 */
int authup_main(int argc, char **argv)
{
	const char *wan_ifname = argv[1];
	char prefix[WAN_PREFIX_SZ];

	if (wan_ifname == NULL ||
	    wan_prefix(wan_ifname, prefix) < 0)
		return EINVAL;

	return 0;
}

/*
 * Called when link goes down, if auth-up was previously executed
 */
int authdown_main(int argc, char **argv)
{
	const char *wan_ifname = argv[1];
	char prefix[WAN_PREFIX_SZ];

	if (wan_ifname == NULL ||
	    wan_prefix(wan_ifname, prefix) < 0)
		return EINVAL;

	return 0;
}

/*
 * Called when authentication is required, but failed
 */
int authfail_main(int argc, char **argv)
{
	const char *wan_ifname = argv[1];
	char prefix[WAN_PREFIX_SZ];

	if (wan_ifname == NULL ||
	    wan_prefix(wan_ifname, prefix) < 0)
		return EINVAL;

	return 0;
}
#endif
