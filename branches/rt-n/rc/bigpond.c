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
#include <sys/stat.h>
#include <sys/types.h>
#include <signal.h>
#include <shutils.h>
#include <bcmnvram.h>
#include <rc.h>

int
start_bpalogin(void)
{
	FILE *fp;
	int ret;
	char authserver[20];
	char authdomain[80];
	char buf[254];

	if(nvram_invmatch("wan_heartbeat_x", "")){
		snprintf(authserver, sizeof(authserver), "%s", nvram_safe_get("wan_heartbeat_x"));
		snprintf(authdomain, sizeof(authdomain), "%s", "");
	}
	else if(nvram_invmatch("wan0_domain", " ") && nvram_invmatch("wan0_domain", ""))
	{	
		snprintf(authserver, sizeof(authserver), "%s", "sm-server");
		snprintf(authdomain, sizeof(authdomain), "%s", nvram_safe_get("wan0_domain"));
	}
#ifdef REMOVE
	else if(nvram_match("wan0_domain", "nsw.bigpond.net.au")){            // NSW
		snprintf(authserver, sizeof(authserver), "%s", "spr3");
		snprintf(authdomain, sizeof(authdomain), "%s", "nsw-remote.bigpond.net.au");
	}
	else if(nvram_match("wan0_domain", "vic.bigpond.net.au")){           // Victoria
		snprintf(authserver, sizeof(authserver), "%s", "mer3");
		snprintf(authdomain, sizeof(authdomain), "%s", "vic-remote.bigpond.net.au");
	}
	else if(nvram_match("wan0_domain","qld.bigpond.net.au")){            // Queensland
		snprintf(authserver, sizeof(authserver), "%s", "bcr3");
		snprintf(authdomain, sizeof(authdomain), "%s", "qld-remote.bigpond.net.au");
	}
	else if(nvram_match("wan0_domain", "sa.bigpond.net.au")){            // South Austrialia
		snprintf(authserver, sizeof(authserver), "%s", "afr3");
		snprintf(authdomain, sizeof(authdomain), "%s", "sa-remote.bigpond.net.au");
	}
	else if(nvram_match("wan0_domain", "wa.bigpond.net.au")){            // Western Austrialia
		snprintf(authserver, sizeof(authserver), "%s", "pwr3");
		snprintf(authdomain, sizeof(authdomain), "%s", "wa-remote.bigpond.net.au");
	}
#endif
	else{
		snprintf(authserver, sizeof(authserver), "%s", nvram_safe_get("wan0_gateway"));
		snprintf(authdomain, sizeof(authdomain), "%s", "");
	}

	snprintf(buf, sizeof(buf), "%s%s%s", authserver, !strcmp(authdomain,"") ? "" : ".", authdomain);

	nvram_set("hb_server_name", buf);
	
	if (!(fp = fopen("/tmp/bpalogin.conf", "w"))) {
		perror("/tmp/bpalogin.conf");
		return errno;
	}
	fprintf(fp, "username %s\n", nvram_safe_get("wan_pppoe_username"));
	fprintf(fp, "password %s\n", nvram_safe_get("wan_pppoe_passwd"));
	fprintf(fp, "authserver %s\n", authserver);
	if (strcmp(authdomain,"")) {
	  fprintf(fp, "authdomain %s\n", authdomain);
	}
	fprintf(fp, "localport 5050\n");
	fprintf(fp, "logging syslog\n");
	fprintf(fp, "debuglevel 0\n");
	fprintf(fp, "minheartbeatinterval 60\n");
	fprintf(fp, "maxheartbeatinterval 420\n");
	fprintf(fp, "connectedprog bpa_connect\n"); 
	fprintf(fp, "disconnectedprog bpa_disconnect\n");
	fclose(fp);
	
	//mkdir("/tmp/ppp", 0777);
	if((fp = fopen("/tmp/bpa_connect_success", "r"))){
		ret = eval("bpalogin", "-c", "/tmp/bpalogin.conf", "-t");
		fclose(fp);	
	}
	else
		ret = eval("bpalogin", "-c", "/tmp/bpalogin.conf");

	return ret;
}

int
stop_bpalogin(void)
{
	int ret;
	
	ret = killall("bpalogin", 0);
	usleep(10000);
	ret += killall("bpalogin", -9);
	dprintf("done\n");
	
	return ret;
}

int
bpa_connect_main(int argc, char **argv)
{
	char buf[254];
	
	nvram_set("wan_auth_t", "OK");

	snprintf(buf, sizeof(buf), "iptables -I INPUT -d %s -i %s -p udp --dport %d -j %s", 
		   nvram_safe_get("wan0_ipaddr"), 
		   nvram_safe_get("wan0_ifname"),
		   5050, 
		   "ACCEPT");

	system(buf);
	return TRUE;	
}

int
bpa_disconnect_main(int argc, char **argv)
{
#if 1
	/* never play with dhcp leases, they're working
	 idependently of authentication and you could 
	 erroneously release lease, which was just aquired */
	nvram_set("wan_auth_t", "FAIL");
#else
	char tmp[100], *str;
	int pid;

	nvram_set("wan_auth_t", "FAIL");

	snprintf(tmp, sizeof(tmp), "/var/run/udhcpc%d.pid", 0);
	if ((str = file2str(tmp))) {
		pid = atoi(str);
		free(str);	
		kill(pid, SIGUSR2);
	}
#endif
	return TRUE;
}
