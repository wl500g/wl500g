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
#include <fcntl.h>
#include <net/if.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <net/if_arp.h>
#include <signal.h>

#include <wlutils.h>
#include <bcmutils.h>
#include "rc.h"


int start_pppd(const char *prefix)
{
	int i, fd, ret;
	FILE *fp;
	char options[80];
	char *pppd_argv[] = { "pppd", "file", options, NULL};
	char tmp[100];
	
	sprintf(options, "/tmp/ppp/options.wan%s", 
		nvram_safe_get(strcat_r(prefix, "unit", tmp)));

	/* Generate options file */
	fd = open(options, O_CREAT|O_WRONLY, S_IRUSR|S_IWUSR);
	if ((fd < 0) || !(fp = fdopen(fd, "w"))) {
       	        perror(options);
       	        return -1;
	}

	/* do not authenticate peer and do not use eap */
	fprintf(fp,
		"noauth refuse-eap\n"
		"user '%s'\n"
		"password '%s'\n",
		nvram_safe_get(strcat_r(prefix, "pppoe_username", tmp)),
		nvram_safe_get(strcat_r(prefix, "pppoe_passwd", tmp)));

	if (nvram_match(strcat_r(prefix, "proto", tmp), "pptp")) {
#ifdef __CONFIG_ACCEL_PPTP__
	    {
		fprintf(fp,
			"plugin pptp.so\n"
			"pptp_server %s\n",
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
		const char *var;

		fprintf(fp, "plugin rp-pppoe.so");

		var = strcat_r(prefix, "pppoe_service", tmp);
		if (nvram_invmatch(var, "")) {
			fprintf(fp, " rp_pppoe_service '%s'", nvram_get(var));
		}

		var = strcat_r(prefix, "pppoe_ac", tmp);
		if (nvram_invmatch(var, "")) {
			fprintf(fp, " rp_pppoe_ac '%s'", nvram_get(var));
		}

		fprintf(fp,
			" nic-%s\n"
			"mru %s mtu %s\n",
			nvram_safe_get(strcat_r(prefix, "ifname", tmp)),
			nvram_safe_get(strcat_r(prefix, "pppoe_mru", tmp)),
			nvram_safe_get(strcat_r(prefix, "pppoe_mtu", tmp)));
	}
	
	i = atoi(nvram_safe_get(strcat_r(prefix, "pppoe_idletime", tmp)));
	if (i > 0 && nvram_match(strcat_r(prefix, "pppoe_demand", tmp), "1")) 
	{
		fprintf(fp, "idle %d ", i);
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
       	
	fprintf(fp,
		"ipcp-accept-remote ipcp-accept-local noipdefault\n"
		"ktune\n");
       	
       	/* pppoe set these options automatically */
       	/* looks like pptp also likes them */
    	fprintf(fp, "default-asyncmap nopcomp noaccomp\n");

	/* pppoe disables "vj bsdcomp deflate" automagically */
	/* ccp should still be enabled - mppe/mppc requires this */
       	fprintf(fp, "novj nobsdcomp nodeflate\n");
       	
       	/* echo failures */
	fprintf(fp,
		"lcp-echo-interval 10\n"
		"lcp-echo-failure 6\n");

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
	close(fd);


	if (nvram_match(strcat_r(prefix, "proto", tmp), "l2tp")) 
	{
		int maxfail, holdoff;

		maxfail = strtoul(nvram_safe_get(strcat_r(prefix, "pppoe_maxfail", tmp)), NULL, 10);
		maxfail = (errno == 0) ? maxfail : 32767;
		holdoff = strtoul(nvram_safe_get(strcat_r(prefix, "pppoe_holdoff", tmp)), NULL, 10);
		holdoff = (errno == 0) ? holdoff : 30;

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
			"max redials = %d\n"
			"redial timeout = %d\n"
			"autodial = yes\n",
			options,
			nvram_invmatch("wan_heartbeat_x", "") ?
				nvram_safe_get("wan_heartbeat_x") : 
				nvram_safe_get(strcat_r(prefix, "pppoe_gateway", tmp)),
			maxfail,
			holdoff);
		fclose(fp);
		
		/* launch xl2tp */
		ret = eval("xl2tpd");

#else /* __CONFIG_XL2TPD__ */
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
			"maxfail %d\n"
			"holdoff %d\n"
			"section cmd\n\n",
			options,
			nvram_invmatch("wan_heartbeat_x", "") ?
   				nvram_safe_get("wan_heartbeat_x") : 
   				nvram_safe_get(strcat_r(prefix, "pppoe_gateway", tmp)),
			maxfail,
			holdoff);
			
		fclose(fp);
		
		/* launch l2tp */
		eval("l2tpd");
		
		sleep(1);
		
		/* start-session */
		ret = eval("l2tp-control", "start-session 0.0.0.0");
#endif /* !__CONFIG_XL2TPD__ */
		
		/* pppd sync nodetach noaccomp nobsdcomp nodeflate */
		/* nopcomp novj novjccomp file /tmp/ppp/options.l2tp */
		
    } else {
		ret = _eval(pppd_argv, NULL, 0, NULL);
	}

	return ret;
}

int start_pppoe_relay(char *wan_if)
{
	if (nvram_match("wan_pppoe_relay_x", "1"))
	{
		char *pppoerelay_argv[] = {"pppoe-relay", "-C", "br0", "-S", wan_if, "-F", NULL};
		pid_t pid;

		return _eval(pppoerelay_argv, NULL, 0, &pid);
	}

	return 0;
}

void setup_ethernet(const char *wan_if)
{
	if (nvram_invmatch("wan_etherspeed_x", "auto"))
	{
		const char *speed = nvram_safe_get("wan_etherspeed_x");
		
		if (strncmp(wan_if, "vlan", 4) == 0)
		{
			char tmp[32];
			const char *wanport;
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
			_eval(mii_argv, NULL, 0, NULL);
		}
		else eval("et", "-i", wan_if, "speed", speed);
	}
}

int ethernet_port(const char *wan_if)
{
	char tmp[32];
	int ret = -1;

	if (strncmp(wan_if, "vlan", 4) == 0)
	{
		const char *ports = nvram_safe_get(strcat_r(wan_if, "ports", tmp));

		tmp[0] = ports[0]; // copy single digit
		tmp[1] = '\0';
		ret = atoi(tmp);
	} /* ... */

	return ret;
}
