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

#include <bcmnvram.h>
#include <netconf.h>
#include <shutils.h>
#include <wlutils.h>
#include <nvparse.h>
#include <rc.h>
#include <bcmutils.h>

#ifdef REMOVE
int start_pppoe(void)
{

	int timeout = 5;
	char pppunit[] = "XXXXXXXXXXXX";

	/* Add optional arguments */
	for (arg = pppoe_argv; *arg; arg++);
	if (nvram_invmatch(strcat_r(prefix, "pppoe_service", tmp), "")) {
		*arg++ = "-s";
				*arg++ = nvram_safe_get(strcat_r(prefix, "pppoe_service", tmp));
			}
			if (nvram_invmatch(strcat_r(prefix, "pppoe_ac", tmp), "")) {
				*arg++ = "-a";
				*arg++ = nvram_safe_get(strcat_r(prefix, "pppoe_ac", tmp));
			}
			if (nvram_match(strcat_r(prefix, "pppoe_demand", tmp), "1") || 
			    nvram_match(strcat_r(prefix, "pppoe_keepalive", tmp), "1"))
				*arg++ = "-k";
			snprintf(pppunit, sizeof(pppunit), "%d", unit);
			*arg++ = "-U";
			*arg++ = pppunit;

			/* launch pppoe client daemon */


			/* ppp interface name is referenced from this point on */
			wan_ifname = nvram_safe_get(strcat_r(prefix, "pppoe_ifname", tmp));
			
			/* Pretend that the WAN interface is up */
			if (nvram_match(strcat_r(prefix, "pppoe_demand", tmp), "1")) {
				/* Wait for pppx to be created */
				while (ifconfig(wan_ifname, IFUP, NULL, NULL) && timeout--)
					sleep(1);

				/* Retrieve IP info */
				if ((s = socket(AF_INET, SOCK_RAW, IPPROTO_RAW)) < 0)
					continue;
				strncpy(ifr.ifr_name, wan_ifname, IFNAMSIZ);

				/* Set temporary IP address */
				if (ioctl(s, SIOCGIFADDR, &ifr))
					perror(wan_ifname);
				nvram_set(strcat_r(prefix, "ipaddr", tmp), inet_ntoa(sin_addr(&ifr.ifr_addr)));
				nvram_set(strcat_r(prefix, "netmask", tmp), "255.255.255.255");

				/* Set temporary P-t-P address */
				if (ioctl(s, SIOCGIFDSTADDR, &ifr))
					perror(wan_ifname);
				nvram_set(strcat_r(prefix, "gateway", tmp), inet_ntoa(sin_addr(&ifr.ifr_dstaddr)));

				close(s);

				/* 
				* Preset routes so that traffic can be sent to proper pppx even before 
				* the link is brought up.
				*/
				preset_wan_routes(wan_ifname);
}
#endif

int start_pptp(char *prefix)
{
	int ret;
	FILE *fp;
	char *pptp_argv[] = { "pppd", NULL};
	char username[32],passwd[32];
	char tmp[100];

	strcpy(username, nvram_safe_get(strcat_r(prefix, "pppoe_username", tmp)));
	strcpy(passwd, nvram_safe_get(strcat_r(prefix, "pppoe_passwd", tmp)));

	/* Generate options file */
	if (!(fp = fopen("/tmp/ppp/options", "w"))) {
       	        perror("/tmp/ppp/options");
       	        return -1;
	}
	
	if (nvram_invmatch(strcat_r(prefix, "pppoe_idletime", tmp), ""))
		fprintf(fp, "idle %s\n", nvram_safe_get(strcat_r(prefix, "pppoe_idletime", tmp)));
	
       	fprintf(fp, "noauth\n");	
       	fprintf(fp, "user '%s'\n",username); 
	fprintf(fp, "demand\n");
	fprintf(fp, "connect true\n");    

	if(nvram_invmatch("wan_heartbeat_x", ""))
	{	
       		fprintf(fp, "pty '/usr/sbin/pptp %s --nolaunchpppd'\n",nvram_safe_get("wan_heartbeat_x")); 
	}
	else
	{
       		fprintf(fp, "pty '/usr/sbin/pptp %s --nolaunchpppd'\n",nvram_safe_get(strcat_r(prefix, "pppoe_gateway", tmp))); 
	}

	fprintf(fp, "defaultroute\n"); 

	//if (nvram_invmatch(strcat_r(prefix, "pppoe_mtu", tmp), ""))	
       	//	fprintf(fp, "mtu %s\n", nvram_safe_get(strcat_r(prefix, "pppoe_mtu", tmp)));

      	if (nvram_invmatch(strcat_r(prefix, "dnsenable_x", tmp), "0"))
		fprintf(fp, "usepeerdns\n");   

       	fprintf(fp, "persist\n");        
       	fprintf(fp, "ipcp-accept-remote\n");  
       	fprintf(fp, "ipcp-accept-local\n");        
       	fprintf(fp, "noipdefault\n");  
       	fprintf(fp, "ktune\n");      
    	fprintf(fp, "default-asyncmap\n"); 
	fprintf(fp, "nopcomp\n");	
	fprintf(fp, "noaccomp\n");	
       	fprintf(fp, "noccp\n");  
       	fprintf(fp, "novj\n");       
       	fprintf(fp, "nobsdcomp\n");   
       	fprintf(fp, "nodeflate\n");     
	fprintf(fp, "lcp-echo-interval 10\n");
	fprintf(fp, "lcp-echo-failure 3\n");	
       	fprintf(fp, "lock\n");

       	fclose(fp);

       	/* Generate pap-secrets file */
       	if (!(fp = fopen("/tmp/ppp/pap-secrets", "w"))) {
       	        perror("/tmp/ppp/pap-secrets");
        	return -1;
       	}
       	fprintf(fp, "\"%s\" * \"%s\" *\n",
		username,
		passwd);
       	fclose(fp);
	chmod("/tmp/ppp/pap-secrets", 0600);

       	/* Generate chap-secrets file */
       	if (!(fp = fopen("/tmp/ppp/chap-secrets", "w"))) {
        	perror("/tmp/ppp/chap-secrets");
               	return -1;
       	}

       	fprintf(fp, "\"%s\" * \"%s\" *\n",
		username,
		passwd);
       	fclose(fp);

	chmod("/tmp/ppp/chap-secrets", 0600);

	/* Enable Forwarding */
	if ((fp = fopen("/proc/sys/net/ipv4/ip_forward", "r+"))) {
		fputc('1', fp);
		fclose(fp);
	} else
	{	
		perror("/proc/sys/net/ipv4/ip_forward");
	}
	
	/* Bring up  WAN interface */
	ifconfig(nvram_safe_get(strcat_r(prefix, "ifname", tmp)), 
		 IFUP,
		 nvram_safe_get("wanx_ipaddr"), 
		 nvram_safe_get("wanx_netmask"));

	ret = _eval(pptp_argv, NULL, 0, NULL);
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
	//if (nvram_invmatch("wan_etherspeed_x", "auto"))
	{
		eval("et", "-i", wan_if, "speed", nvram_safe_get("wan_etherspeed_x"));	
	}	
} 
