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
#include <errno.h>
#include <syslog.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <signal.h>

#include <shutils.h>
#include "rc.h"

int start_bpalogin(void)
{
	FILE *fp;
	const char *authserver;
	const char *authdomain = NULL;
	char *bpalogin_argv[5] = { "bpalogin", "-c", "/tmp/bpalogin.conf", NULL, NULL };

	if (nvram_invmatch("wan_heartbeat_x", ""))
	{
		authserver = nvram_safe_get("wan_heartbeat_x");
	}
	else if (nvram_invmatch("wan0_domain", " ") && nvram_invmatch("wan0_domain", ""))
	{
		authserver = "sm-server";
		authdomain = nvram_get("wan0_domain");
	}
	else
	{
		authserver = nvram_safe_get("wan0_gateway");
	}

	if (!(fp = fopen("/tmp/bpalogin.conf", "w"))) {
		return errno;
	}
	fprintf(fp,
		"username %s\n"
		"password %s\n"
		"authserver %s\n",
		nvram_safe_get("wan_pppoe_username"),
		nvram_safe_get("wan_pppoe_passwd"),
		authserver);
	if (authdomain != NULL) {
		fprintf(fp, "authdomain %s\n", authdomain);
	}
	fprintf(fp,
		"localport 5050\n"
		"logging syslog\n"
		"debuglevel 0\n"
		"minheartbeatinterval 60\n"
		"maxheartbeatinterval 420\n"
		"connectedprog bpa_connect\n"
		"disconnectedprog bpa_disconnect\n");
	fclose(fp);
	
	//mkdir("/tmp/ppp", 0777);
	if (exists("/tmp/bpa_connect_success")) {
		bpalogin_argv[3] = "-t";
	}

	return _eval(bpalogin_argv, NULL, 0, NULL);
}

int
stop_bpalogin(void)
{
	int ret;
	
	ret = killall("bpalogin");
	usleep(10000);
	ret += killall_s("bpalogin", SIGKILL);
	dprintf("done\n");
	
	return ret;
}

int bpa_connect_main(int argc, char **argv)
{
	char buf[254];
	
	nvram_set("wan_auth_t", "OK");

	snprintf(buf, sizeof(buf),
		"iptables -I INPUT -d %s -i %s -p udp --dport %d -j %s",
		   nvram_safe_get("wan0_ipaddr"), 
		   nvram_safe_get("wan0_ifname"),
		   5050, 
		   "ACCEPT");

	system(buf);
	return TRUE;
}

int bpa_disconnect_main(int argc, char **argv)
{
#if 1
	/* never play with dhcp leases, they're working
	 idependently of authentication and you could 
	 erroneously release lease, which was just aquired */
	nvram_set("wan_auth_t", "FAIL");
#else
	char tmp[100];

	nvram_set("wan_auth_t", "FAIL");

	sprintf(tmp, "/var/run/udhcpc%d.pid", 0);
	kill_pidfile_s(tmp, SIGUSR2);
#endif
	return TRUE;
}
