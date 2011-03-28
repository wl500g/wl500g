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
#include <arpa/inet.h>
#include <errno.h>
#include <time.h>
#include <unistd.h>

#include <bcmnvram.h>
#include <netconf.h>
#include <shutils.h>
#include <rc.h>

static char udhcpstate[12];

static int
expires(char *lan_ifname, unsigned int in)
{
	time_t now;
	FILE *fp;
	char tmp[100];

	time(&now);
	snprintf(tmp, sizeof(tmp), "/tmp/udhcpc%d.expires", 0); 
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
static int
deconfig(void)
{
	char *lan_ifname = safe_getenv("interface");

	ifconfig(lan_ifname, IFUP,
		 nvram_safe_get("lan_ipaddr"),
		 nvram_safe_get("lan_netmask"), NULL);

	expires(lan_ifname, 0);

	lan_down_ex(lan_ifname);

	logmessage("dhcp client", "%s: lease is lost", udhcpstate);
	//wanmessage("lost IP from server");

	dprintf("done\n");
	return 0;
}

/*
 * bound: This argument is used when udhcpc moves from an unbound, to
 * a bound state. All of the paramaters are set in enviromental
 * variables, The script should configure the interface, and set any
 * other relavent parameters (default gateway, dns server, etc).
*/
static int
bound(void)
{
	char *lan_ifname = safe_getenv("interface");
	char *value;
	char tmp[100], prefix[] = "lanXXXXXXXXXX_";

	snprintf(prefix, sizeof(prefix), "lan_");
	
	if ((value = getenv("ip")))
		nvram_set(strcat_r(prefix, "ipaddr_t", tmp), value);
	if ((value = getenv("subnet")))
		nvram_set(strcat_r(prefix, "netmask_t", tmp), value);
        if ((value = getenv("router")))
		nvram_set(strcat_r(prefix, "gateway_t", tmp), value);
	if ((value = getenv("dns")))
		nvram_set(strcat_r(prefix, "dns_t", tmp), value);
	if ((value = getenv("wins")))
		nvram_set(strcat_r(prefix, "wins_t", tmp), value);
#if 0
	if ((value = getenv("hostname")))
		sethostname(value, strlen(value) + 1);
#endif
	if ((value = getenv("domain")))
		nvram_set(strcat_r(prefix, "domain_t", tmp), value);
	if ((value = getenv("lease"))) {
		nvram_set(strcat_r(prefix, "lease_t", tmp), value);
		expires(lan_ifname, atoi(value));
	}

	ifconfig(lan_ifname, IFUP,
		 nvram_safe_get(strcat_r(prefix, "ipaddr_t", tmp)),
		 nvram_safe_get(strcat_r(prefix, "netmask_t", tmp)), NULL);

	lan_up_ex(lan_ifname);

	logmessage("dhcp client", "%s IP : %s from %s", 
		udhcpstate, 
		nvram_safe_get(strcat_r(prefix, "ipaddr_t", tmp)), 
		nvram_safe_get(strcat_r(prefix, "gateway_t", tmp)));

	//wanmessage("");

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
static int
renew(void)
{
	bound();

	dprintf("done\n");
	return 0;
}

int
udhcpc_ex_main(int argc, char **argv)
{
	if (argc<2 || !argv[1])
		return EINVAL;

	strcpy(udhcpstate, argv[1]);

	if (!strcmp(argv[1], "deconfig"))
		return deconfig();
	else if (!strcmp(argv[1], "bound"))
		return bound();
	else if (!strcmp(argv[1], "renew"))
		return renew();
	else if (!strcmp(argv[1], "leasefail"))
		return 0;
	else return deconfig();
}
