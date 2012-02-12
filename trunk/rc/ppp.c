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

#include "rc.h"

/*
* parse ifname to retrieve unit #
*/
int ppp_ifunit(const char *ifname)
{
	if (strncmp(ifname, "ppp", 3))
		return -1;
	if (!isdigit(ifname[3]))
		return -1;
	return atoi(&ifname[3]);
}

static int ppp_prefix(char **wan_ifname, char *prefix)
{
	char tmp[100];
	int unit;

	*wan_ifname = safe_getenv("IFNAME");

	if ((unit = ppp_ifunit(*wan_ifname)) < 0)
		return -1;

	sprintf(prefix, "wan%d_", unit);

	if (!nvram_get(strcat_r(prefix, "ifname", tmp)))
		return -2;

	return unit;
}

/*
 * Called when link comes up
 */
int
ipup_main(int argc, char **argv)
{
	FILE *fp;
	char *wan_ifname;
	char *value;
	char buf[256];
	char tmp[100], prefix[sizeof("wanXXXXXXXXXX_")];

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
	if (getenv("DNS1"))
		sprintf(buf, "%s", getenv("DNS1"));
	if (getenv("DNS2"))
		sprintf(buf + strlen(buf), "%s%s", strlen(buf) ? " " : "", getenv("DNS2"));
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
	char *wan_ifname;
	char tmp[100], prefix[sizeof("wanXXXXXXXXXX_")];

	if (ppp_prefix(&wan_ifname, prefix) < 0)
		return -1;

	umask(022);

	wan_down(wan_ifname);

	unlink(strcat_r("/tmp/ppp/link.", wan_ifname, tmp));

	preset_wan_routes(wan_ifname);

	logmessage(nvram_safe_get("wan_proto_t"), "Disconnected");
	wanmessage(pppstatus(tmp));

	return 0;
}

#ifdef __CONFIG_IPV6__
int
ip6up_main(int argc, char **argv)
{
	char *wan_ifname;
	//char *value;
	int unit;
	char tmp[100], prefix[sizeof("wanXXXXXXXXXX_")];

	if ((unit = ppp_prefix(&wan_ifname, prefix)) < 0)
		return -1;

	if (nvram_invmatch(strcat_r(prefix, "ipv6_if_x", tmp), "0"))
		return -1;

	//if ((value = getenv("LLLOCAL")))
	//	eval("ip", "-6", "addr", "add", value, "dev", wan_ifname);
	//if ((value = getenv("LLREMOTE")))
	//	nvram_set(strcat_r(prefix, "ipv6_router", tmp), value);

	wan6_up(wan_ifname, unit);

	return 0;
}

int
ip6down_main(int argc, char **argv)
{
	char *wan_ifname;
	int unit;
	char tmp[100], prefix[sizeof("wanXXXXXXXXXX_")];

	if ((unit = ppp_prefix(&wan_ifname, prefix)) < 0)
		return -1;

	if (nvram_invmatch(strcat_r(prefix, "ipv6_if_x", tmp), "0"))
		return -1;

	wan6_down(wan_ifname, unit);

	return 0;
}
#endif

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
	char prefix[sizeof("wanXXXXXXXXXX_")];

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
	char prefix[sizeof("wanXXXXXXXXXX_")];

	if (wan_ifname == NULL ||
	    wan_prefix(wan_ifname, prefix) < 0)
		return EINVAL;

	return 0;
}
