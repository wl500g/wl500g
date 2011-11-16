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

#include <wlutils.h>
#include <bcmutils.h>
#include "rc.h"


int start_pppd(char *prefix)
{
	int ret;
	FILE *fp;
	char options[80];
	char *pppd_argv[] = { "pppd", "file", options, NULL};
	char tmp[100];
	mode_t mask;
	
	sprintf(options, "/tmp/ppp/options.wan%s", 
		nvram_safe_get(strcat_r(prefix, "unit", tmp)));

	mask = umask(066);
	
	/* Generate options file */
	if (!(fp = fopen(options, "w"))) {
       	        perror(options);
		umask(mask);
       	        return -1;
	}
	
	umask(mask);
	
	/* do not authenticate peer and do not use eap */
	fprintf(fp, "noauth refuse-eap\n");
	fprintf(fp, "user '%s'\n", 
       		nvram_safe_get(strcat_r(prefix, "pppoe_username", tmp))); 
	fprintf(fp, "password '%s'\n", 
		nvram_safe_get(strcat_r(prefix, "pppoe_passwd", tmp)));
       	       	
       	if (nvram_match(strcat_r(prefix, "proto", tmp), "pptp")) {
#ifdef __CONFIG_ACCEL_PPTP__
	    {
		fprintf(fp, "plugin pptp.so\n");
		fprintf(fp, "pptp_server %s\n", 
   			nvram_invmatch("wan_heartbeat_x", "") ?
   			nvram_safe_get("wan_heartbeat_x") : 
   			nvram_safe_get(strcat_r(prefix, "pppoe_gateway", tmp))); 
 	    }
#endif
	    /* see KB Q189595 -- historyless & mtu */
	    fprintf(fp, "nomppe-stateful %s mtu 1400\n",
			nvram_safe_get(strcat_r(prefix, "pptp_options_x", tmp)));
	} else {
		fprintf(fp, "nomppe nomppc\n");
	}

       	if (nvram_match(strcat_r(prefix, "proto", tmp), "pppoe")) 
       	{
		fprintf(fp, "plugin rp-pppoe.so");

		if (nvram_invmatch(strcat_r(prefix, "pppoe_service", tmp), "")) {
			fprintf(fp, " rp_pppoe_service '%s'", 
				nvram_safe_get(strcat_r(prefix, "pppoe_service", tmp)));
		}
	
		if (nvram_invmatch(strcat_r(prefix, "pppoe_ac", tmp), "")) {
			fprintf(fp, " rp_pppoe_ac '%s'", 
				nvram_safe_get(strcat_r(prefix, "pppoe_ac", tmp)));
		}

		fprintf(fp, " nic-%s\n", nvram_safe_get(strcat_r(prefix, "ifname", tmp)));
		
		fprintf(fp, "mru %s mtu %s\n", 
			nvram_safe_get(strcat_r(prefix, "pppoe_mru", tmp)),
			nvram_safe_get(strcat_r(prefix, "pppoe_mtu", tmp)));
	}
	
	if (atoi(nvram_safe_get(strcat_r(prefix, "pppoe_idletime", tmp))) &&
	     nvram_match(strcat_r(prefix, "pppoe_demand", tmp), "1")) 
	{
		fprintf(fp, "idle %s ", nvram_safe_get(strcat_r(prefix, "pppoe_idletime", tmp)));
		if (nvram_invmatch(strcat_r(prefix, "pppoe_txonly_x", tmp), "0")) {
			fprintf(fp, "tx_only ");
		}
		fprintf(fp, "demand\n");
	}

	fprintf(fp, "maxfail 0\n");

      	if (nvram_invmatch(strcat_r(prefix, "dnsenable_x", tmp), "0"))
		fprintf(fp, "usepeerdns\n");   

       	if (nvram_invmatch(strcat_r(prefix, "proto", tmp), "l2tp")) 
		fprintf(fp, "persist\n");
       	
       	fprintf(fp, "ipcp-accept-remote ipcp-accept-local noipdefault\n");
       	fprintf(fp, "ktune\n");
       	
       	/* pppoe set these options automatically */
       	/* looks like pptp also likes them */
    	fprintf(fp, "default-asyncmap nopcomp noaccomp\n");

	/* pppoe disables "vj bsdcomp deflate" automagically */
	/* ccp should still be enabled - mppe/mppc requires this */
       	fprintf(fp, "novj nobsdcomp nodeflate\n");
       	
       	/* echo failures */
	fprintf(fp, "lcp-echo-interval 10\n");
	fprintf(fp, "lcp-echo-failure 6\n");

	fprintf(fp, "unit %s\n", 
		nvram_get(strcat_r(prefix, "unit", tmp)) ? : "0");

#ifdef __CONFIG_IPV6__
	/* Enable IPv6 and IPv6CP */
	if (nvram_match("ipv6_proto", "ppp"))
		fprintf(fp, "+ipv6\n"); 
#endif

	/* user specific options */
	fprintf(fp, "%s\n", 
		nvram_safe_get(strcat_r(prefix, "pppoe_options_x", tmp)));
		
       	fclose(fp);

       	if (nvram_match(strcat_r(prefix, "proto", tmp), "l2tp")) 
       	{
#ifdef __CONFIG_XL2TPD__
		if (!(fp = fopen("/etc/xl2tpd.conf", "w"))) {
	       	        perror(options);
	       	        return -1;
		}

		fprintf(fp,
			"[global]\n"
			"access control = yes\n\n"
			"[lac l2tp]\n"
			"pppoptfile = %s\n"
			"lns = %s\n"
			"redial = yes\n"
			"max redials = %s\n"
			"redial timeout = %s\n"
			"autodial = yes\n",
			options,
			nvram_invmatch("wan_heartbeat_x", "") ?
				nvram_safe_get("wan_heartbeat_x") : 
				nvram_safe_get(strcat_r(prefix, "pppoe_gateway", tmp)),
			nvram_invmatch(strcat_r(prefix, "pppoe_maxfail", tmp), "") ?
				nvram_safe_get(strcat_r(prefix, "pppoe_maxfail", tmp)) : "32767",
			nvram_invmatch(strcat_r(prefix, "pppoe_holdoff", tmp), "") ?
				nvram_safe_get(strcat_r(prefix, "pppoe_holdoff", tmp)) : "30");
		fclose(fp);
		
		/* launch xl2tp */
		ret = eval("xl2tpd");
#else
       		mkdir("/etc/l2tp", 0755);
       		
		if (!(fp = fopen("/etc/l2tp/l2tp.conf", "w"))) {
	       	        perror(options);
	       	        return -1;
		}
		
		fprintf(fp, "# automagically generated\n"
			"global\n\n"
			"load-handler \"sync-pppd.so\"\n"
			"load-handler \"cmd.so\"\n\n"
			"section sync-pppd\n\n"
			"lac-pppd-opts \"file %s\"\n\n"
			"section peer\n"
			"peername %s\n"
			"lac-handler sync-pppd\n"
			"persist yes\n"
			"maxfail %s\n"
			"holdoff %s\n"
			"section cmd\n\n",
			options,
			nvram_invmatch("wan_heartbeat_x", "") ?
   				nvram_safe_get("wan_heartbeat_x") : 
   				nvram_safe_get(strcat_r(prefix, "pppoe_gateway", tmp)),
			nvram_invmatch(strcat_r(prefix, "pppoe_maxfail", tmp), "") ?
				nvram_safe_get(strcat_r(prefix, "pppoe_maxfail", tmp)) : "32767",
			nvram_invmatch(strcat_r(prefix, "pppoe_holdoff", tmp), "") ?
				nvram_safe_get(strcat_r(prefix, "pppoe_holdoff", tmp)) : "30");
			
		fclose(fp);
		
		/* launch l2tp */
		eval("l2tpd");
		
		sleep(1);
		
		/* start-session */
		ret = eval("l2tp-control", "start-session 0.0.0.0");
#endif
		
		/* pppd sync nodetach noaccomp nobsdcomp nodeflate */
		/* nopcomp novj novjccomp file /tmp/ppp/options.l2tp */
		
       	} else
		ret = _eval(pppd_argv, NULL, 0, NULL);
		
	return ret;
}

void start_pppoe_relay(char *wan_if)
{
	if (nvram_match("wan_pppoe_relay_x", "1"))
	{
		char *pppoerelay_argv[] = {"pppoe-relay", "-C", "br0", "-S", wan_if, "-F", NULL};
		int ret;
		pid_t pid;

		ret = _eval(pppoerelay_argv, NULL, 0, &pid);
	}
}

void setup_ethernet(char *wan_if)
{
	if (nvram_invmatch("wan_etherspeed_x", "auto"))
	{
		char *speed = nvram_safe_get("wan_etherspeed_x");
		
		if (strncmp(wan_if, "vlan", 4) == 0)
		{
			char tmp[16];
			char *wanport;
			char *mii_argv[] = { "mii-tool", "eth0", "-p", "0",
				 "-A", !strcmp(speed, "100full") ? "100baseTx-FD" :
					!strcmp(speed, "100half") ? "100baseTx-HD" :
					!strcmp(speed, "10full") ? "10baseT-FD" : "10baseT-HD",
				 NULL };

			if ((wanport = nvram_get(strcat_r(wan_if, "ports", tmp))))
			{
				tmp[0] = wanport[0]; // copy single digit
				tmp[1] = '\0';
				mii_argv[3] = tmp;
			}
			_eval(mii_argv, ">/dev/null", 0, NULL);
		}
		else eval("et", "-i", wan_if, "speed", speed);
	}
}

int ethernet_port(char *wan_if)
{
	char tmp[16];
	int ret = -1;

	if (strncmp(wan_if, "vlan", 4) == 0)
	{
		char *ports = nvram_safe_get(strcat_r(wan_if, "ports", tmp));
		tmp[0] = ports[0]; // copy single digit
		tmp[1] = '\0';
		ret = atoi(tmp);
	} /* ... */

	return ret;
}
