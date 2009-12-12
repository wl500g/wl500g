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
#include <arpa/inet.h>
#include <errno.h>
#include <ctype.h>

#include <bcmnvram.h>
#include <netconf.h>
#include <shutils.h>
#include <rc.h>

/*
* parse ifname to retrieve unit #
*/
int
ppp_ifunit(char *ifname)
{
	if (strncmp(ifname, "ppp", 3))
		return -1;
	if (!isdigit(ifname[3]))
		return -1;
	return atoi(&ifname[3]);
}

/*
 * Called when link comes up
 */
int
ipup_main(int argc, char **argv)
{
	FILE *fp;
	char *wan_ifname = safe_getenv("IFNAME");
	char *value;
	char buf[256];
	int unit;
	char tmp[100], prefix[] = "wanXXXXXXXXXX_";

	dprintf("%s\n", argv[0]);
	
	umask(022);

	if ((unit = ppp_ifunit(wan_ifname)) < 0)
		return -1;
	
	snprintf(prefix, sizeof(prefix), "wan%d_", unit);

	/* Touch connection file */
	if (!(fp = fopen(strcat_r("/tmp/ppp/link.", wan_ifname, tmp), "a"))) {
		perror(tmp);
		return errno;
	}
	fclose(fp);
	
	if (!nvram_get(strcat_r(prefix, "ifname", tmp)))
		return -1;

	if ((value = getenv("IPLOCAL"))) {
		ifconfig(wan_ifname, IFUP,
			 value, "255.255.255.255");
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
	char *wan_ifname = safe_getenv("IFNAME");
	int unit;
	char tmp[100], prefix[] = "wanXXXXXXXXXX_";

	umask(022);
	
	if ((unit = ppp_ifunit(wan_ifname)) < 0)
		return -1;
	
	snprintf(prefix, sizeof(prefix), "wan%d_", unit);

	if (!nvram_get(strcat_r(prefix, "ifname", tmp)))
		return -1;

	wan_down(wan_ifname);

	unlink(strcat_r("/tmp/ppp/link.", wan_ifname, tmp));
	
	preset_wan_routes(wan_ifname);

	logmessage(nvram_safe_get("wan_proto_t"), "Disconnected");
	wanmessage(pppstatus(tmp));

	return 0;
}

