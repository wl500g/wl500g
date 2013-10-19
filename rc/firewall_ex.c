
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
#include <string.h>
#include <signal.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <dirent.h>

#include <netconf.h>
#include <nvparse.h>
#include "rc.h"

#define foreach_x(x) for (i = 0; i < nvram_get_int(x); i++)

static char *g_buf;
#define G_BUF_TOTAL	1024

/* Forwards */
static int porttrigger_setting(FILE *fp, const char *lan_if);


static void g_buf_init()
{
	static char *g_buf_pool = NULL;

	if (g_buf_pool == NULL)
		g_buf_pool = malloc(G_BUF_TOTAL);

	g_buf = g_buf_pool;
}

static char *g_buf_alloc(char *g_buf_now)
{
	g_buf += strlen(g_buf_now) + 1;

	return (g_buf_now);
}

static char *proto_conv(char *proto_name, int idx)
{	
	char *proto;
	char itemname_arr[32];
	
	sprintf(itemname_arr,"%s%d", proto_name, idx);
	proto = nvram_get(itemname_arr);
		
	if (!strncasecmp(proto, "Both", 3)) strcpy(g_buf, "both");
	else if (!strncasecmp(proto, "TCP", 3)) strcpy(g_buf, "tcp");
	else if (!strncasecmp(proto, "UDP", 3)) strcpy(g_buf, "udp");
	else if (!strncasecmp(proto, "OTHER", 5)) strcpy(g_buf, "other");
	else strcpy(g_buf,"tcp");
	return (g_buf_alloc(g_buf));
}

static char *protoflag_conv(char *proto_name, int idx, int isFlag)
{	
	char *proto;
	char itemname_arr[32];
	
	sprintf(itemname_arr,"%s%d", proto_name, idx);
	proto = nvram_get(itemname_arr);

	strcpy(g_buf, "");
	
	if (!isFlag)
	{		
		if (strncasecmp(proto, "UDP", 3) == 0) strcpy(g_buf, "udp");
		else strcpy(g_buf, "tcp");
	}
	else
	{	
		if (strlen(proto)>3)
		{
			strcpy(g_buf, proto+4);
		}			
		else strcpy(g_buf,"");
	}	
	return (g_buf_alloc(g_buf));
}

static char *portrange_ex2_conv(char *port_name, int idx, int *start, int *end)
{
	char *port, *strptr;
	char itemname_arr[32];
	
	sprintf(itemname_arr,"%s%d", port_name, idx);
	port = nvram_get(itemname_arr);

	strcpy(g_buf, "");
	
	if (!strncmp(port, ">", 1)) 
	{
		sprintf(g_buf, "%d-65535", atoi(port+1) + 1);
		*start = atoi(port+1);
		*end = 65535;
	}
	else if (!strncmp(port, "=", 1)) 
	{
		sprintf(g_buf, "%d-%d", atoi(port+1), atoi(port+1));
		*start = *end = atoi(port+1);
	}
	else if (!strncmp(port, "<", 1)) 
	{
		sprintf(g_buf, "1-%d", atoi(port+1) - 1);
		*start = 1;
		*end = atoi(port+1);
	}
	else if ((strptr = strchr(port, ':')) != NULL)
	{		
		strcpy(itemname_arr, port);
		strptr = strchr(itemname_arr, ':');
		sprintf(g_buf, "%d-%d", atoi(itemname_arr), atoi(strptr+1));	
		*start = atoi(itemname_arr);
		*end = atoi(strptr+1);
	}
	else if (*port)
	{
		sprintf(g_buf, "%d-%d", atoi(port), atoi(port));
		*start = atoi(port);
		*end = atoi(port);
	}
	else
	{
		g_buf[0] = 0;
		*start = 0;
		*end = 0;
	}
	
	return(g_buf_alloc(g_buf));
}

static char *portrange_conv(char *port_name, int idx)
{
	char itemname_arr[32];
	
	sprintf(itemname_arr,"%s%d", port_name, idx);
	strcpy(g_buf, nvram_safe_get(itemname_arr));	
	
	return(g_buf_alloc(g_buf));
}

static char *iprange_ex_conv(char *ip_name, int idx)
{
	char *ip;
	char itemname_arr[32];
	char startip[16];
	int i, j, k;
	int mask;
	
	sprintf(itemname_arr,"%s%d", ip_name, idx);
	ip = nvram_safe_get(itemname_arr);
	strcpy(g_buf, "");
	
	// scan all ip string
	i = j = k = 0;
	mask = 32;
	
	while (*(ip+i))
	{
		if (*(ip+i) == '*') 
		{
			startip[j++] = '0';
			// 255 is for broadcast
			mask-=8;
		}
		else 
		{
			startip[j++] = *(ip+i);
		}
		i++;
	}	
	
	startip[j++] = 0;

	if (mask == 32)
		sprintf(g_buf, "%s", startip);
	else if (mask == 0)
		strcpy(g_buf, "");
	else sprintf(g_buf, "%s/%d", startip, mask);

	return(g_buf_alloc(g_buf));
}

static char *ip_conv(char *ip_name, int idx)
{	
	char itemname_arr[32];

	sprintf(itemname_arr,"%s%d", ip_name, idx);
	sprintf(g_buf, "%s", nvram_safe_get(itemname_arr));
	return(g_buf_alloc(g_buf));
}


static char *general_conv(char *ip_name, int idx)
{	
	char itemname_arr[32];

	sprintf(itemname_arr,"%s%d", ip_name, idx);
	sprintf(g_buf, "%s", nvram_safe_get(itemname_arr));
	return(g_buf_alloc(g_buf));
}

static char *filter_conv(char *proto, char *flag, char *srcip, char *srcport,
			 char *dstip, char *dstport)
{
	char newstr[64];

	//printf("filter : %s,%s,%s,%s,%s,%s\n", proto, flag, srcip, srcport, dstip, dstport);
	
	strcpy(g_buf, "");		
				                        			
        if (strcmp(proto, "")!=0)
	{
		sprintf(newstr, " -p %s", proto);
        	strcat(g_buf, newstr);
        }				
            			
        if (strcmp(flag, "")!=0)
        {
        	sprintf(newstr, " --tcp-flags %s RST", flag);
		strcat(g_buf, newstr);
	}			
                 
	if (strcmp(srcip, "")!=0)
	{
		if (strchr(srcip , '-'))
			sprintf(newstr, " --src-range %s", srcip);
		else	
			sprintf(newstr, " -s %s", srcip);
		strcat(g_buf, newstr);
	}				
            			
	if (strcmp(srcport, "")!=0)
	{
		sprintf(newstr, " --sport %s", srcport);
		strcat(g_buf, newstr);
        }			
            			
        if (strcmp(dstip, "")!=0)
        {
		if (strchr(dstip, '-'))
			sprintf(newstr, " --dst-range %s", dstip);
		else	
        		sprintf(newstr, " -d %s", dstip);
		strcat(g_buf, newstr);
        }
			
        if (strcmp(dstport, "")!=0)
        {
        	sprintf(newstr, " --dport %s", dstport);
		strcat(g_buf, newstr);
	}
	return(g_buf_alloc(g_buf));
	//printf("str: %s\n", g_buf);
}

static void timematch_conv(char *mstr, char *nv_date, char *nv_time)
{	
	char *datestr[] = { "Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat"};
	char timestart[6], timestop[6];
	char *time, *date;
	int i, head;

	date = nvram_safe_get(nv_date);
	time = nvram_safe_get(nv_time);

	if (strlen(date)!=7||strlen(time)!=8) goto no_match;

	if (strncmp(date, "1111111", 7) == 0 &&
	    strncmp(time, "00002359", 8) == 0) goto no_match;
	
	i = 0;
	strncpy(timestart, time, 2);
	i+=2;
	timestart[i++] = ':';
	strncpy(timestart+i, time+2, 2);
	i+=2;
	timestart[i]=0;
	i = 0;
	strncpy(timestop, time+4, 2);
	i+=2;
	timestop[i++] = ':';
	strncpy(timestop+i, time+6, 2);
	i+=2;
	timestop[i]=0;

	sprintf(mstr, "-m time --timestart %s --timestop %s --weekdays ",
			timestart, timestop);

	head = 1;

	for (i = 0; i < 7; i++)
	{
		if (*(date+i) == '1')
		{
			if (head)
			{
			    sprintf(mstr, "%s %s", mstr, datestr[i]);
			    head = 0;
			}
			else
			{	
			    sprintf(mstr, "%s,%s", mstr, datestr[i]);
			}
		}
	}
	return;
	
no_match:
	mstr[0] = 0;
	return;
}
/*
void p(int step)
{
	dprintf("P: %d %s\n", step, g_buf);
}
*/

#ifndef __CONFIG_MINIUPNPD__
static void write_upnp_forward(FILE *fp, char *wan_if, char *wan_ip,
				char *lan_if, char *lan_ip, char *lan_class,
				char *logaccept, char *logdrop)
{
	char name[NV_FORWARD_PORT_MAXLEN], value[512];
        char *wan_port0, *wan_port1, *lan_ipaddr, *lan_port0, *lan_port1, *proto;
        char *enable, *desc;
	int i;

	/* Set wan_port0-wan_port1>lan_ipaddr:lan_port0-lan_port1,proto,enable,desc */
	for (i = 0; i < MAX_NVPARSE; i++)
	{
		snprintf(name, sizeof(name), "forward_port%d", i);

		strncpy(value, nvram_safe_get(name), sizeof(value));

		/* Check for LAN IP address specification */
		lan_ipaddr = value;
		wan_port0 = strsep(&lan_ipaddr, ">");
		if (!lan_ipaddr)
			continue;

		/* Check for LAN destination port specification */
		lan_port0 = lan_ipaddr;
		lan_ipaddr = strsep(&lan_port0, ":");
		if (!lan_port0)
			continue;

		/* Check for protocol specification */
		proto = lan_port0;
		lan_port0 = strsep(&proto, ":,");
		if (!proto)
			continue;

		/* Check for enable specification */
		enable = proto;
		proto = strsep(&enable, ":,");
		if (!enable)
			continue;

		/* Check for description specification (optional) */
		desc = enable;
		enable = strsep(&desc, ":,");

		/* Check for WAN destination port range (optional) */
		wan_port1 = wan_port0;
		wan_port0 = strsep(&wan_port1, "-");
		if (!wan_port1)
			wan_port1 = wan_port0;

		/* Check for LAN destination port range (optional) */
		lan_port1 = lan_port0;

		lan_port0 = strsep(&lan_port1, "-");
	        if (!lan_port1)
			lan_port1 = lan_port0;

		/* skip if it's disabled */
		if ( strcmp(enable, "off") == 0)
			continue;

		/* -A PREROUTING -p tcp -m tcp --dport 823 -j DNAT 
		                 --to-destination 192.168.1.88:23  */
		if (!strcmp(proto,"tcp") || !strcmp(proto,"both"))
		{
	   		fprintf(fp, "-A VSERVER -p tcp -m tcp --dport %s "
				  "-j DNAT --to-destination %s:%s\n"
					, wan_port0, lan_ipaddr, lan_port0);
		}
		if (!strcmp(proto,"udp") || !strcmp(proto,"both")) {
	   		fprintf(fp, "-A VSERVER -p udp -m udp --dport %s "
				  "-j DNAT --to-destination %s:%s\n"
				  	, wan_port0, lan_ipaddr, lan_port0);
		}
	}
}
#endif /* !__CONFIG_MINIUPNPD__ */

static void nat_setting(const char *wan_if, const char *wan_ip, const char *lan_if, const char *lan_ip, const char *logaccept, const char *logdrop)
{
	FILE *fp;
        char lan_class[32];
	int i;
	int wan_port;

	if ((fp = fopen("/tmp/nat_rules", "w")) == NULL) return;

	fprintf(fp, "*nat\n"
	       	":PREROUTING ACCEPT [0:0]\n"
	       	":POSTROUTING ACCEPT [0:0]\n"
	  	":OUTPUT ACCEPT [0:0]\n"
	  	":VSERVER - [0:0]\n"
#ifdef __CONFIG_MINIUPNPD__
		":UPNP - [0:0]\n"
#endif
	);

	//Log	
   	//if (nvram_match("misc_natlog_x", "1"))
   	// 	fprintf(fp, "-A PREROUTING -i %s -j LOG --log-prefix ALERT --log-level 4\n", wan_if);

	/* VSERVER chain */
	if (ip_addr(wan_ip))
		fprintf(fp, "-A PREROUTING -d %s -j VSERVER\n", wan_ip);

	if (nvram_invmatch("wan0_ifname", wan_if) && ip_addr(nvram_safe_get("wan0_xipaddr")))
   		fprintf(fp, "-A PREROUTING -d %s -j VSERVER\n", nvram_get("wan0_xipaddr"));
      
   	if (nvram_match("misc_http_x", "1"))
	{
		wan_port = 8080;
   		if (nvram_invmatch("misc_httpport_x", ""))
      			wan_port = nvram_get_int("misc_httpport_x");
   		fprintf(fp, "-A VSERVER -p tcp -m tcp --dport %d -j DNAT --to-destination %s:%s\n",
			wan_port, lan_ip, nvram_safe_get("http_lanport"));
	}

   	if (nvram_match("wan_nat_x", "1") && nvram_invmatch("upnp_enable", "0"))
   	{
#ifdef __CONFIG_MINIUPNPD__
		/* Call UPNP chain */
		fprintf(fp, "-A VSERVER -j UPNP\n");
#else
		// upnp port forward
        	write_upnp_forward(fp, wan_if, wan_ip, lan_if, lan_ip, lan_class, logaccept, logdrop);
#endif
	}

	// Port forwarding or Virtual Server
   	if (nvram_match("wan_nat_x", "1") && nvram_match("vts_enable_x", "1"))
   	{     		
		char srcips[64], dstips[64];

   		g_buf_init();
   				     		
     		foreach_x("vts_num_x")
     		{
     			char *proto;
			char *protono;
			char *port;
			char *lport;
			char *dstip;
			char *srcip;

     			proto = proto_conv("vts_proto_x", i);
			protono = portrange_conv("vts_protono_x", i);
			port = portrange_conv("vts_port_x", i);
			lport = portrange_conv("vts_lport_x", i);
			dstip = ip_conv("vts_ipaddr_x", i);
			srcip = ip_conv("vts_sipaddr_x", i);

			if (srcip != NULL && strlen(srcip) != 0)
				snprintf(srcips, sizeof(srcips), "-s %s", srcip);
			else
				srcips[0] = '\0';

			if (lport != NULL && strlen(lport) != 0)
				snprintf(dstips, sizeof(dstips), "--to-destination %s:%s", dstip, lport);
			else
				snprintf(dstips, sizeof(dstips), "--to %s", dstip);

			if (strcmp(proto, "tcp") == 0 || strcmp(proto, "both") == 0)
			{
				fprintf(fp, "-A VSERVER -p tcp -m tcp %s --dport %s -j DNAT %s\n", 
					srcips, port, dstips);
			}
			if (strcmp(proto, "udp") == 0 || strcmp(proto, "both") == 0)
			{
				fprintf(fp, "-A VSERVER -p udp -m udp %s --dport %s -j DNAT %s\n", 
					srcips, port, dstips);
			}
			if (strcmp(proto, "other") == 0)
			{
				fprintf(fp, "-A VSERVER -p %s %s -j DNAT --to %s\n",
					protono, srcips, dstip);
			}
		}
	}

   	if (nvram_match("wan_nat_x", "1") && nvram_invmatch("sp_battle_ips", "0") && ip_addr(wan_ip))
	{
		#define BASEPORT 6112
		#define BASEPORT_NEW 10000

		ip2class(lan_ip, nvram_safe_get("lan_netmask"), lan_class);

		/* run starcraft patch anyway */
		fprintf(fp, "-A PREROUTING -p udp -d %s --sport %d -j NETMAP --to %s\n", wan_ip, BASEPORT, lan_class);

		fprintf(fp, "-A POSTROUTING -p udp -s %s --dport %d -j NETMAP --to %s\n", lan_class, BASEPORT, wan_ip);
	}

   	// Exposed station	
   	if (nvram_match("wan_nat_x", "1") && nvram_invmatch("dmz_ip", ""))
   	{   		
      		fprintf(fp, "-A VSERVER -j DNAT --to %s\n", nvram_safe_get("dmz_ip"));
      	}

	if (nvram_match("wan_nat_x", "1"))
	{
		if (ip_addr(wan_ip))
   			fprintf(fp, "-A POSTROUTING -o %s ! -s %s -j MASQUERADE\n", wan_if, wan_ip);

   		/* masquerade physical WAN port connection */
		if (nvram_invmatch("wan0_ifname", wan_if) && ip_addr(nvram_safe_get("wan0_xipaddr")))
			fprintf(fp, "-A POSTROUTING -o %s ! -s %s -j MASQUERADE\n", 
	   			nvram_get("wan0_ifname"), nvram_get("wan0_xipaddr"));

		/* masquerade VSERVER from LAN port connection */
		ip2class(lan_ip, nvram_safe_get("lan_netmask"), lan_class);
		fprintf(fp, "-A POSTROUTING -o %s -s %s -d %s -j MASQUERADE\n", lan_if, lan_class, lan_class);
	}

	/* Trigger port setting */
	porttrigger_setting(fp, lan_if);

	fprintf(fp, "COMMIT\n");
	
	fclose(fp);

	eval("iptables-restore", "/tmp/nat_rules");

#ifdef __CONFIG_MINIUPNPD__
	/* TODO: kick miniupnpd to reload lease file */
#endif
}

/* Rules for LW Filter and MAC Filter
 * MAC ACCEPT
 *     ACCEPT -> MACS
 *            	 -> LW Disabled
 *                  MACS ACCEPT
 *               -> LW Default Accept: 
 *                  MACS DROP in rules
 *                  MACS ACCEPT Default
 *               -> LW Default Drop: 
 *                  MACS ACCEPT in rules
 *                  MACS DROP Default
 *     DROP    -> FORWARD DROP 
 *
 * MAC DROP
 *     DROP -> FORWARD DROP
 *     ACCEPT -> FORWARD ACCEPT 
 */
static int filter_setting(const char *wan_if, const char *wan_ip,
			const char *lan_if, const char *lan_ip,
			const char *logaccept, const char *logdrop)
{

	FILE *fp, *fp1;
	char *proto, *flag, *srcip, *srcport, *dstip, *dstport;
	char *setting, line[256], s[64];
	int i;
#ifdef WEBSTRFILTER
        char nvname[36], timef[256], *filterstr;
#endif
	
	if ((fp = fopen("/tmp/filter_rules", "w")) == NULL) return -1;

	fprintf(fp, "*filter\n"
		":INPUT ACCEPT [0:0]\n"
		":FORWARD ACCEPT [0:0]\n"
		":OUTPUT ACCEPT [0:0]\n"
		":MACS - [0:0]\n"
		":SECURITY - [0:0]\n"
		":BRUTE - [0:0]\n"
#ifdef __CONFIG_MINIUPNPD__
		":UPNP - [0:0]\n"
#endif
		":logaccept - [0:0]\n"
		":logdrop - [0:0]\n");

	// FILTER from LAN to WAN Source MAC
	if (nvram_invmatch("macfilter_enable_x", "0") &&
		nvram_invmatch("macfilter_enable_x", "disabled"))
	{   		
		int blacklist = nvram_invmatch("macfilter_enable_x", "1");
		// LAN/WAN filter		
		foreach_x("macfilter_num_x")
		{	
			g_buf_init();
         		fprintf(fp, "-A MACS -m mac --mac-source %s -j %s\n", 
				mac_conv("macfilter_list_x", i, line), 
				(blacklist ? logdrop : "RETURN"));
		} 
		if (!blacklist)
			fprintf(fp, "-A MACS -j %s\n", logdrop);
		fprintf(fp,
			"-A INPUT -i %s -j MACS\n"
			"-A FORWARD -i %s -j MACS\n",
			lan_if, lan_if);
	} 

	// Security checks
	fprintf(fp,
	// sync-flood protection
		"-A SECURITY -p tcp --syn -m limit --limit 1/s -j RETURN\n"
	// furtive port scanner
		"-A SECURITY -p tcp --tcp-flags SYN,ACK,FIN,RST RST -m limit --limit 1/s -j RETURN\n"
	// udp flooding
		"-A SECURITY -p udp -m limit --limit 5/s -j RETURN\n"
	// ping of death
		"-A SECURITY -p icmp -m limit --limit 5/s -j RETURN\n");
#ifdef __CONFIG_IPV6__
	// pass ipv6 tunnel packets
	if (nvram_match("ipv6_proto", "tun6in4") ||
	    nvram_match("ipv6_proto", "tun6to4") ||
	    nvram_match("ipv6_proto", "tun6rd"))
		fprintf(fp, "-A SECURITY -p 41 -j RETURN\n");
#endif
	// drop attacks!!!
	fprintf(fp, "-A SECURITY -j %s\n", logdrop);

	/* Drop the wrong state, INVALID, packets */
	fprintf(fp, "-A INPUT -m conntrack --ctstate INVALID -j %s\n", logdrop);
	/* Accept related connections, skip rest of checks */
	fprintf(fp, "-A INPUT -m conntrack --ctstate ESTABLISHED,RELATED -j ACCEPT\n");
	/* From localhost and intranet, all traffic is accepted */
	fprintf(fp, "-A INPUT -i lo -m conntrack --ctstate NEW -j %s\n", logaccept);
	fprintf(fp, "-A INPUT -i %s -m conntrack --ctstate NEW -j %s\n", lan_if, logaccept);
	/* Pass multicast */
	if (nvram_match("mr_enable_x", "1") || nvram_invmatch("udpxy_enable_x", "0")) {
		fprintf(fp,
			"-A INPUT -p igmp -d 224.0.0.0/4 -j %s\n"
			"-A INPUT -p udp -d 224.0.0.0/4 ! --dport 1900 -j %s\n",
			 logaccept, logaccept);
	}
	/* Check internet traffic */
	if (nvram_match("fw_dos_x", "1"))
	{
		fprintf(fp, "-A INPUT -i %s -m conntrack --ctstate NEW -j SECURITY\n", wan_if);
		if (nvram_invmatch("wan0_ifname", wan_if))
			fprintf(fp, "-A INPUT -i %s -m conntrack --ctstate NEW -j SECURITY\n", nvram_get("wan0_ifname"));
	}

	if (nvram_match("fw_enable_x", "1"))
	{	
		/* enable incoming packets from broken dhcp servers, which are sending replies
		 * from addresses other than used for query, this could lead to lower level
		 * of security, but it does not work otherwise (conntrack does not work) :-(
		 */
		if (nvram_match("wan0_proto", "dhcp") || nvram_match("wan0_proto", "bigpond") ||
		    nvram_match("wan_ipaddr", "0.0.0.0"))
		{
			fprintf(fp, "-A INPUT -p udp --sport 67 --dport 68 -j %s\n", logaccept);
		}
	#ifdef __CONFIG_CONVEX__
		if (nvram_match("wan0_auth_x", "convex")
		&& (nvram_match("wan0_proto", "static") || nvram_match("wan0_proto", "dhcp")))
		{
			fprintf(fp, "-A INPUT -p tcp --dport 6326 -j %s\n", logaccept);
		}
	#endif

		// Firewall between WAN and Local
		if (nvram_match("ssh_enable", "1"))
			fprintf(fp, "-A INPUT -p tcp -m tcp --dport %s --syn -j %s\n", nvram_safe_get("ssh_port"),
				nvram_invmatch("recent_ssh_enable", "0") ? "BRUTE" : logaccept);

		if (nvram_match("usb_ftpenable_x", "1"))
			fprintf(fp, "-A INPUT -p tcp -m tcp --dport %s --syn -j %s\n", nvram_safe_get("usb_ftpport_x"),
				nvram_invmatch("recent_ftp_enable", "0") ? "BRUTE" : logaccept);

		if (nvram_invmatch("http_lanport", "80"))
			fprintf(fp, "-A INPUT -p tcp -m tcp -d %s --dport 80 -j %s\n", nvram_safe_get("lan_ipaddr"), logaccept);

		if (nvram_invmatch("udpxy_enable_x", "0") && nvram_invmatch("udpxy_wan_x", "0"))
			fprintf(fp, "-A INPUT -p tcp -m tcp --dport %s -j %s\n", nvram_safe_get("udpxy_enable_x"), logaccept);

		if (nvram_match("misc_http_x", "1"))
		{
            		fprintf(fp, "-A INPUT -p tcp -m tcp -d %s --dport %s -j %s\n", nvram_safe_get("lan_ipaddr"), nvram_safe_get("http_lanport"), logaccept);  
		}

		if (nvram_match("usb_webenable_x", "2"))
		{
			fprintf(fp, "-A INPUT -p tcp -m tcp --dport %s -j %s\n", nvram_safe_get("usb_webhttpport_x"), logaccept);
		}

		if (nvram_invmatch("misc_ping_x", "0"))
		{
			fprintf(fp, "-A INPUT -p icmp -j %s\n", logaccept);
			// Pass udp traceroute
			fprintf(fp, "-A INPUT -p udp -m udp --dport 33434:33534 -j %s\n", logaccept);
		}

#ifdef __CONFIG_IPV6__
		if (nvram_match("ipv6_proto", "tun6in4") ||
		    nvram_match("ipv6_proto", "tun6to4") ||
		    nvram_match("ipv6_proto", "tun6rd"))
		{
			if (nvram_match("ipv6_proto", "tun6in4") && !nvram_invmatch("misc_ping_x", "0")) {
				char *sit_remote = nvram_safe_get("ipv6_sit_remote");
				if (*sit_remote)
					fprintf(fp, "-A INPUT -p icmp -s %s -j %s\n", sit_remote, logaccept);
			}
			fprintf(fp, "-A INPUT -p 41 -j %s\n", logaccept);
		}
#endif

		if (nvram_invmatch("misc_lpr_x", "0"))
		{
			fprintf(fp, "-A INPUT -p tcp -m tcp --dport %d -j %s\n", 515, logaccept);
			fprintf(fp, "-A INPUT -p tcp -m tcp --dport %d -j %s\n", 9100, logaccept);

			fprintf(fp, "-A INPUT -p tcp -m tcp --dport %d -j %s\n", 3838, logaccept);
		}

		fprintf(fp, "-A INPUT -j %s\n", logdrop);
	}


	/* Accept the redirect, might be seen as INVALID, packets */
	fprintf(fp, "-A FORWARD -i %s -o %s -j %s\n", lan_if, lan_if, logaccept);	
	/* Drop the wrong state, INVALID, packets */
	fprintf(fp, "-A FORWARD -m conntrack --ctstate INVALID -j %s\n", logdrop);
	/* Pass multicast */
	if (nvram_match("mr_enable_x", "1"))
		fprintf(fp, "-A FORWARD -p udp -d 224.0.0.0/4 -j ACCEPT\n");

	/* Clamp TCP MSS to PMTU of WAN interface */
	if (nvram_match("wan0_proto", "pppoe") ||
	    nvram_match("wan0_proto", "pptp") || nvram_match("wan0_proto", "l2tp") 
#ifdef __CONFIG_MADWIMAX__
	 || nvram_match("wan0_proto", "wimax")
#endif
#ifdef __CONFIG_MODEM__
	 || nvram_match("wan0_proto", "usbmodem")
#endif
#ifdef __CONFIG_USBNET__
	 || nvram_match("wan0_proto", "usbnet")
#endif
        ) {
		fprintf(fp, "-A FORWARD -p tcp --tcp-flags SYN,RST SYN -j TCPMSS --clamp-mss-to-pmtu\n");
	}

	/* Accept related connections, skip rest of checks */
	fprintf(fp, "-A FORWARD -m conntrack --ctstate ESTABLISHED,RELATED -j ACCEPT\n");

	/* Filter out invalid WAN->WAN connections */
	fprintf(fp, "-A FORWARD -o %s ! -i %s -j %s\n", wan_if, lan_if, logdrop);
	if (nvram_invmatch("wan0_ifname", wan_if))
		fprintf(fp, "-A FORWARD -o %s ! -i %s -j %s\n", nvram_get("wan0_ifname"), lan_if, logdrop);

	/* Check internet traffic */
	if (nvram_match("fw_dos_x", "1"))
		fprintf(fp, "-A FORWARD ! -i %s -m conntrack --ctstate NEW -j SECURITY\n", lan_if);

	// FILTER from LAN to WAN
	if (nvram_match("fw_lw_enable_x", "1"))
	{   		
		char lanwan_timematch[128];
		char ptr[32], *icmplist;
		const char *ftype, *dtype;

		timematch_conv(lanwan_timematch, "filter_lw_date_x", "filter_lw_time_x");	   
 
		if (nvram_match("filter_lw_default_x", "DROP"))
		{
			dtype = logdrop;
			ftype = logaccept;
		}
		else
		{
			dtype = logaccept;
			ftype = logdrop;
		}
			
		// LAN/WAN filter		
		g_buf_init();

       		foreach_x("filter_lw_num_x")
       		{	               			
            		proto = protoflag_conv("filter_lw_proto_x", i, 0);
            		flag = protoflag_conv("filter_lw_proto_x", i, 1);
			srcip = iprange_ex_conv("filter_lw_srcip_x", i);
			srcport = portrange_conv("filter_lw_srcport_x", i);
			dstip = iprange_ex_conv("filter_lw_dstip_x", i);
			dstport = portrange_conv("filter_lw_dstport_x", i);	
			setting = filter_conv(proto, flag, srcip, srcport, dstip, dstport); 
         		fprintf(fp, "-A FORWARD %s -i %s %s -j %s\n", lanwan_timematch, lan_if, setting, ftype);
		}            

       		// ICMP       	       			       			
       		foreach(ptr, nvram_safe_get("filter_lw_icmp_x"), icmplist)
		{	   	 		
	        	fprintf(fp, "-A FORWARD %s -i %s -p icmp --icmp-type %s -j %s\n", lanwan_timematch, lan_if, ptr, ftype);
	        }	

		// Default
	        fprintf(fp, "-A FORWARD -i %s -j %s\n", lan_if, dtype);
	}

	// Filter from WAN to LAN
	if (nvram_match("fw_wl_enable_x", "1"))
	{   		
		char wanlan_timematch[128];
		char ptr[32], *icmplist;
		const char *ftype;

		timematch_conv(wanlan_timematch, "filter_wl_date_x", "filter_wl_time_x");
		g_buf_init();
	
		if (nvram_match("filter_wl_default_x", "DROP"))
		{
			ftype = logaccept;
		}
		else
		{
			ftype = logdrop;
		}
			
       		foreach_x("filter_wl_num_x")
       		{	               			
            		proto = protoflag_conv("filter_wl_proto_x", i, 0);
            		flag = protoflag_conv("filter_wl_proto_x", i, 1);
			srcip = iprange_ex_conv("filter_wl_srcip_x", i);
			srcport = portrange_conv("filter_wl_srcport_x", i);
			dstip = iprange_ex_conv("filter_wl_dstip_x", i);
			dstport = portrange_conv("filter_wl_dstport_x", i);	
			setting = filter_conv(proto, flag, srcip, srcport, dstip, dstport);

         		fprintf(fp, "-A FORWARD %s -o %s %s -j %s\n", wanlan_timematch, lan_if, setting, ftype);
		}            
       			 
       		// ICMP       	       			       			
       		foreach(ptr, nvram_safe_get("filter_wl_icmp_x"), icmplist)
		{	   	 		
	        	fprintf(fp, "-A FORWARD %s -o %s -p icmp --icmp-type %s -j %s\n", wanlan_timematch, lan_if, ptr, ftype);
	        }
       	}

	/* Enable Virtual Servers */
	fprintf(fp, "-A FORWARD -m conntrack --ctstate DNAT -j %s\n", (
		nvram_match("fw_wl_enable_x", "1") ?
		nvram_match("filter_vs_default_x", "DROP") : FALSE) ? logdrop : logaccept);

	if (nvram_match("fw_wl_enable_x", "1"))
	{   		
		// Default
		fprintf(fp, "-A FORWARD -o %s -j %s\n", lan_if, 
			nvram_match("filter_wl_default_x", "DROP") ? logdrop : logaccept);
	}

	/* BRUTE chain */
	// SSH and FTP servers bruteforce protection through ipt_recent module
	if ((nvram_match("ssh_enable", "1") && nvram_invmatch("recent_ssh_enable", "0")) ||
	    (nvram_match("usb_ftpenable_x", "1") && nvram_invmatch("recent_ftp_enable", "0"))) {
		// Evaluate incoming connections through white&blacklists stored in flashfs
		if ((fp1 = fopen("/usr/local/etc/ssh.allow", "r")) != NULL) {
			while (fgets(line, sizeof(line), fp1))
				if ((sscanf(line, "%s", s) == 1) && (s[0] != '#'))
					fprintf(fp, "-A BRUTE -s %s -j %s\n", s, logaccept);
					fclose(fp1);
		}
		if ((fp1 = fopen("/usr/local/etc/ssh.deny", "r")) != NULL) {
			while (fgets(line, sizeof(line), fp1))
				if ((sscanf(line, "%s", s) == 1) && (s[0] != '#'))
					fprintf(fp, "-A BRUTE -s %s -j %s\n", s, logdrop);
					fclose(fp1);
		}
		// Block annoying intruder's connection attemtps
		// Remember, that both successful and unsuccessful attempts are counted
		fprintf(fp, "-A BRUTE -m recent --name BRUTE --update --hitcount %s --seconds %s -j %s\n",
				nvram_safe_get("recent_hitcount"), nvram_safe_get("recent_seconds"), logdrop);
		fprintf(fp, "-A BRUTE -m recent --name BRUTE --set -j %s\n", logaccept);
	}

	/* logaccept chain */
	fprintf(fp, "-A logaccept -m conntrack --ctstate NEW -j LOG --log-prefix \"ACCEPT \" "
		  " --log-macdecode "
		  "--log-tcp-sequence --log-tcp-options --log-ip-options\n"
		  "-A logaccept -j ACCEPT\n");

	/* logdrop chain */
	fprintf(fp,"-A logdrop -m conntrack --ctstate NEW -j LOG --log-prefix \"DROP \" "
		  " --log-macdecode "
		  "--log-tcp-sequence --log-tcp-options --log-ip-options\n"
		  "-A logdrop -j DROP\n");
	
#ifdef WEBSTRFILTER
	if (nvram_match("url_enable_x", "1") && nvram_invmatch("url_date_x", "0000000")) {

		timematch_conv(timef, "url_date_x", "url_time_x");
		for (i = 0; i < nvram_get_int("url_num_x"); i++)
		{
			memset(nvname, 0, sizeof(nvname));
			sprintf(nvname, "url_keyword_x%d", i);
			filterstr = nvram_safe_get(nvname);
 			if (strncasecmp(filterstr, "http://", strlen("http://")) == 0)
				filterstr += strlen("http://");
			else
			if (strncasecmp(filterstr, "https://", strlen("https://")) == 0)
				filterstr += strlen("https://");
			if (*filterstr)
			{
				fprintf(fp,
					"-I FORWARD -p tcp %s -m webstr --url \"%s\" -j REJECT --reject-with tcp-reset\n",
					timef, filterstr);
			}
                }
        }
#endif

	fprintf(fp, "COMMIT\n\n");
       	fclose(fp);

	eval("iptables-restore", "/tmp/filter_rules");
	
#ifdef __CONFIG_IPV6__
	if (nvram_invmatch("ipv6_proto", "")) {
	// TODO: sync-flood protection, LAN/WAN filter tables, WAN/LAN filter tables
	if ((fp = fopen("/tmp/filter6_rules", "w")) == NULL) return -1;

	fprintf(fp, "*filter\n"
		    ":INPUT ACCEPT [0:0]\n"
		    ":FORWARD ACCEPT [0:0]\n"
		    ":OUTPUT ACCEPT [0:0]\n"
		    ":SECURITY - [0:0]\n"
		    ":logaccept - [0:0]\n:logdrop - [0:0]\n");

	/* INPUT chain */

	// Disable RH0 to block ping-pong of packets.
	fprintf(fp, "-A INPUT -m rt --rt-type 0 -j %s\n", logdrop);
	if (nvram_match("ipv6_proto", "dhcp6"))
		fprintf(fp, "-A INPUT -p udp --dport 546 -j %s\n", logaccept);
	// Allow ICMPv6
	fprintf(fp, "-A INPUT -p ipv6-icmp ! --icmpv6-type echo-request -j %s\n", logaccept);
#ifndef BROKEN_IPV6_CONNTRACK
        /* Drop the wrong state, INVALID, packets */
	fprintf(fp, "-A INPUT -m conntrack --ctstate INVALID -j %s\n", logdrop);
	/* Accept related connections, skip rest of checks */
	fprintf(fp, "-A INPUT -m conntrack --ctstate ESTABLISHED,RELATED -j ACCEPT\n");
	// From localhost and intranet, all traffic is accepted
	fprintf(fp, "-A INPUT -i lo -m conntrack --ctstate NEW -j %s\n", logaccept);
	fprintf(fp, "-A INPUT -i %s -m conntrack --ctstate NEW -j %s\n", lan_if, logaccept);
#else
	fprintf(fp, "-A INPUT -i lo -j %s\n", logaccept);
	fprintf(fp, "-A INPUT -i %s -j %s\n", lan_if, logaccept);
	// Pass Link-Local, is it superseded by ipv6-icmp? 
	fprintf(fp, "-A INPUT -s fe80::/10 -j %s\n", logaccept);
#endif
	// Pass multicast, should it depend on mr_enable_x?
	//if (nvram_match("mr_enable_x", "1"))
		fprintf(fp, "-A INPUT -s ff00::/8 -j %s\n", logaccept);
#ifndef BROKEN_IPV6_CONNTRACK
	// Check internet traffic
	if (nvram_match("fw_dos_x", "1")) {
		if (nvram_match("ipv6_proto", "tun6in4") ||
		    nvram_match("ipv6_proto", "tun6to4") ||
		    nvram_match("ipv6_proto", "tun6rd"))
			fprintf(fp, "-A INPUT -i six0 -m conntrack --ctstate NEW -j SECURITY\n");
		fprintf(fp, "-A INPUT -i %s -m conntrack --ctstate NEW -j SECURITY\n", wan_if);
		if (nvram_invmatch("wan0_ifname", wan_if))
			fprintf(fp, "-A INPUT -i %s -m conntrack --ctstate NEW -j SECURITY\n", nvram_get("wan0_ifname"));
	}
#endif
	// Firewall between WAN and Local
	if (nvram_match("fw_enable_x", "1")) {
		// Pass SSH
		if (nvram_match("ssh_enable", "1")) {
#ifndef BROKEN_IPV6_CONNTRACK
			fprintf(fp, "-A INPUT -p tcp -m tcp --dport %s --syn -j %s\n", nvram_safe_get("ssh_port"),
#else
			fprintf(fp, "-A INPUT -p tcp -m tcp --dport %s -j %s\n", nvram_safe_get("ssh_port"),
#endif
				logaccept);
		}
		// Pass FTP
		if (nvram_match("usb_ftpenable_x", "1")) {
#ifndef BROKEN_IPV6_CONNTRACK
			fprintf(fp, "-A INPUT -p tcp -m tcp --dport %s --syn -j %s\n", nvram_safe_get("usb_ftpport_x"),
#else
			fprintf(fp, "-A INPUT -p tcp -m tcp --dport %s -j %s\n", nvram_safe_get("usb_ftpport_x"),
#endif
				logaccept);
		}
		// Pass Web-UI
		if (nvram_match("misc_http_x", "1")) {
			fprintf(fp, "-A INPUT -p tcp -m tcp -d %s --dport %s -j %s\n",
				nvram_safe_get("ipv6_lan_addr"),
				nvram_safe_get("http_lanport"), logaccept);
		}
		// Pass ping, echo-reply should be handled by conntrack
		if (nvram_invmatch("misc_ping_x", "0")) {
			fprintf(fp, "-A INPUT -p ipv6-icmp --icmpv6-type echo-request -j %s\n", logaccept);
			// Pass udp traceroute
			fprintf(fp, "-A INPUT -p udp -m udp --dport 33434:33534 -j %s\n", logaccept);
		}
		// Drop everything else
		fprintf(fp, "-A INPUT -j %s\n", logdrop);
	}

	/* FORWARD chain */

	// Disable RH0 to block ping-pong of packets.
	fprintf(fp, "-A FORWARD -m rt --rt-type 0 -j %s\n", logdrop);
	// Accept the redirect, might be seen as INVALID, packets
	fprintf(fp, "-A FORWARD -i %s -o %s -j %s\n", lan_if, lan_if, logaccept);
#ifndef BROKEN_IPV6_CONNTRACK
	/* Drop the wrong state, INVALID, packets */
	fprintf(fp, "-A FORWARD -m conntrack --ctstate INVALID -j %s\n", logdrop);
#endif
	// Pass multicast, should it depend on mr_enable_x?
	//if (nvram_match("mr_enable_x", "1"))
		fprintf(fp, "-A FORWARD -s ff00::/8 -j %s\n", logaccept);

	/* Clamp TCP MSS to PMTU of WAN interface */
	if (nvram_match("ipv6_proto", "tun6in4") ||
	    nvram_match("ipv6_proto", "tun6to4") ||
	    nvram_match("ipv6_proto", "tun6rd") ||
	    (!nvram_get_int("ipv6_if_x") &&
	     (nvram_match("wan0_proto", "pppoe") ||
	      nvram_match("wan0_proto", "pptp") || nvram_match("wan0_proto", "l2tp"))))
		fprintf(fp, "-A FORWARD -p tcp --tcp-flags SYN,RST SYN -j TCPMSS --clamp-mss-to-pmtu\n");
#ifndef BROKEN_IPV6_CONNTRACK
	/* Accept related connections, skip rest of checks */
	fprintf(fp, "-A FORWARD -m conntrack --ctstate ESTABLISHED,RELATED -j ACCEPT\n");
#endif
	// Allow ICMPv6
	// Should be only echo-req, dest-unreach, packet-too-big, time-exeed and parm-problem
	fprintf(fp, "-A FORWARD -p ipv6-icmp -j %s\n", logaccept);
#ifndef BROKEN_IPV6_CONNTRACK
#else
	// Pass Link-Local, is it superseded by ipv6-icmp?
	fprintf(fp, "-A FORWARD -s fe80::/10 -j %s\n", logaccept);
#endif

        // Filter out invalid WAN->WAN connections
	if (nvram_match("ipv6_proto", "tun6in4") ||
	    nvram_match("ipv6_proto", "tun6to4") ||
	    nvram_match("ipv6_proto", "tun6rd"))
		fprintf(fp, "-A FORWARD -o six0 ! -i %s -j %s\n", lan_if, logdrop);
	fprintf(fp, "-A FORWARD -o %s ! -i %s -j %s\n", wan_if, lan_if, logdrop);
	if (nvram_invmatch("wan0_ifname", wan_if))
		fprintf(fp, "-A FORWARD -o %s ! -i %s -j %s\n", nvram_get("wan0_ifname"), lan_if, logdrop);
#ifndef BROKEN_IPV6_CONNTRACK
	/* Check internet traffic */
	if (nvram_match("fw_dos_x", "1"))
		fprintf(fp, "-A FORWARD ! -i %s -m conntrack --ctstate NEW -j SECURITY\n", lan_if);
#endif

	/* OUTPUT chain */

	// Disable RH0 to block ping-pong of packets.
	fprintf(fp, "-A OUTPUT -m rt --rt-type 0 -j %s\n", logdrop);

	/* logaccept chain */
#ifndef BROKEN_IPV6_CONNTRACK
	fprintf(fp, "-A logaccept -m conntrack --ctstate NEW -j LOG --log-prefix \"ACCEPT \" "
#else
	fprintf(fp, "-A logaccept -j LOG --log-prefix \"ACCEPT \" "
#endif
		  " --log-macdecode "
		  "--log-tcp-sequence --log-tcp-options --log-ip-options\n"
		  "-A logaccept -j ACCEPT\n");

	/* logdrop chain */
#ifndef BROKEN_IPV6_CONNTRACK
	fprintf(fp,"-A logdrop -m conntrack --ctstate NEW -j LOG --log-prefix \"DROP \" "
#else
	fprintf(fp,"-A logdrop -j LOG --log-prefix \"DROP \" "
#endif
		  " --log-macdecode "
		  "--log-tcp-sequence --log-tcp-options --log-ip-options\n"
		  "-A logdrop -j DROP\n");

	fprintf(fp, "COMMIT\n\n");
	fclose(fp);

	eval("ip6tables-restore", "/tmp/filter6_rules");
	}
#endif
	return 0;
}

static int porttrigger_setting(FILE *fp, const char *lan_if)
{
	netconf_app_t apptarget, *app;
	int i;
	char *out_proto, *in_proto;
//	char *out_port, *in_port;
	char *desc;
	int  out_start, out_end, in_start, in_end;

	if (nvram_invmatch("wan_nat_x", "1") ||
	    nvram_invmatch("autofw_enable_x", "1")) 
		return -1;
	
	g_buf_init();
   				     		
     	foreach_x("autofw_num_x")
     	{	
     		out_proto = proto_conv("autofw_outproto_x", i);
     		/*out_port = */portrange_ex2_conv("autofw_outport_x", i, &out_start, &out_end);
     		in_proto = proto_conv("autofw_inproto_x", i);
		/*in_port = */portrange_ex2_conv("autofw_inport_x", i, &in_start, &in_end);
		desc = general_conv("autofw_desc_x", i);

		app = &apptarget;
		memset(app, 0, sizeof(netconf_app_t));

		/* Parse outbound protocol */
		if (!strncasecmp(out_proto, "tcp", 3))
			app->match.ipproto = IPPROTO_TCP;
		else if (!strncasecmp(out_proto, "udp", 3))
			app->match.ipproto = IPPROTO_UDP;
		else continue;

		/* Parse outbound port range */
		app->match.dst.ports[0] = htons(out_start);
		app->match.dst.ports[1] = htons(out_end);

		/* Parse related protocol */
		if (!strncasecmp(in_proto, "tcp", 3))
			app->proto = IPPROTO_TCP;
		else if (!strncasecmp(in_proto, "udp", 3))
			app->proto = IPPROTO_UDP;
		else continue;

		/* Parse related destination port range */
		app->dport[0] = htons(in_start);
		app->dport[1] = htons(in_end);

		/* Parse mapped destination port range */
		app->to[0] = htons(in_start);
		app->to[1] = htons(in_end);

		/* Parse description */
		if (desc)
			strncpy(app->desc, desc, sizeof(app->desc));

		/* Set interface name (match packets entering LAN interface) */
		strncpy(app->match.in.name, nvram_safe_get("lan_ifname"), IFNAMSIZ);

		/* Set LAN source port range (match packets from any source port) */
		app->match.src.ports[1] = htons(0xffff);

		/* Set target (application specific port forward) */
		app->target = NETCONF_APP;

		if (valid_autofw_port(app))
		{
			fprintf(fp, "-A PREROUTING -p %s -i %s --dport %d:%d -j autofw"
			    " --related-proto %s --related-dport %d-%d --related-to %d-%d\n",
				(app->match.ipproto == IPPROTO_TCP) ? "tcp" : "udp", lan_if, 
				ntohs(app->match.dst.ports[0]), ntohs(app->match.dst.ports[1]), 
				(app->proto == IPPROTO_TCP) ? "tcp" : "udp", 
				ntohs(app->dport[0]), ntohs(app->dport[1]), 
				ntohs(app->to[0]), ntohs(app->to[1]));
			/*netconf_add_fw((netconf_fw_t *)app); */
		}
	}
	return 0;
}

int start_firewall_ex(const char *wan_if, const char *wan_ip, const char *lan_if, const char *lan_ip)
{
	DIR *dir;
	struct dirent *file;
	char name[NAME_MAX];
	char logaccept[32], logdrop[32];
	char *mcast_ifname = nvram_get("wan0_ifname");
	
	/* mcast needs rp filter to be turned off only for non default iface */
	if (!(nvram_match("mr_enable_x", "1") || nvram_invmatch("udpxy_enable_x", "0")) ||
	 	strcmp(wan_if, mcast_ifname) == 0) mcast_ifname = NULL;
	
	/* Block obviously spoofed IP addresses */
	if (!(dir = opendir("/proc/sys/net/ipv4/conf")))
		perror("/proc/sys/net/ipv4/conf");
	while (dir && (file = readdir(dir))) {
		if (strncmp(file->d_name, ".", NAME_MAX) != 0 &&
		    strncmp(file->d_name, "..", NAME_MAX) != 0) {
			sprintf(name, "/proc/sys/net/ipv4/conf/%s/rp_filter", file->d_name);
			if (!fputs_ex(name,
				(mcast_ifname &&
				 strncmp(file->d_name, mcast_ifname, NAME_MAX) == 0)
				? "0" : "1"))
				break;
		}
	}
	closedir(dir);


	/* Determine the log type */
	if (nvram_match("fw_log_x", "accept") || nvram_match("fw_log_x", "both"))
		strcpy(logaccept, "logaccept");
	else strcpy(logaccept, "ACCEPT");

	if (nvram_match("fw_log_x", "drop") || nvram_match("fw_log_x", "both"))
		strcpy(logdrop, "logdrop");
	else strcpy(logdrop, "DROP");
		
	/* nat setting */
	nat_setting(wan_if, wan_ip, lan_if, lan_ip, logaccept, logdrop);

	/* Filter setting */
	filter_setting(wan_if, wan_ip, lan_if, lan_ip, logaccept, logdrop);

	if (nvram_invmatch("misc_conntrack_x", "")) {
		fputs_ex("/proc/sys/net/netfilter/nf_conntrack_max",
				nvram_safe_get("misc_conntrack_x"));
	}
#ifdef XBOX_SUPPORT
	fputs_ex("/proc/sys/net/ipv4/ip_conntrack_udp_timeouts", "65 180");
#endif

	fputs_ex("/proc/sys/net/ipv4/ip_forward", "1");

	if (nvram_invmatch("wan0_ifname", wan_if))
		eval("/usr/local/sbin/post-firewall", wan_if, wan_ip, lan_if, lan_ip, 
			nvram_get("wan0_ifname"), nvram_get("wan0_xipaddr"));
	else
		eval("/usr/local/sbin/post-firewall", wan_if, wan_ip, lan_if, lan_ip);

	return 0;
}

void convert_routes(int unit)
{
	int i;
	char *ip, *netmask, *gateway, *matric, *interface;
	char wroutes[1024], lroutes[1024], mroutes[1024];
	char tmp[100], prefix[WAN_PREFIX_SZ];

	wroutes[0] = 0;
	lroutes[0] = 0;
	mroutes[0] = 0;

	g_buf_init();

	if (nvram_match("sr_enable_x", "1")) foreach_x("sr_num_x")
	{
		char *routes;
		ip = general_conv("sr_ipaddr_x", i);
		netmask = general_conv("sr_netmask_x", i);
		gateway = general_conv("sr_gateway_x", i);
		matric = general_conv("sr_matric_x", i);
		interface = general_conv("sr_if_x", i);

		dprintf("%x %s %s %s %s %s\n", i, ip, netmask, gateway, matric, interface);

		if (strcmp(interface, "WAN") == 0)
			routes = wroutes;
		else if (strcmp(interface, "LAN") == 0)
			routes = lroutes;
		else if (strcmp(interface, "MAN") == 0)
			routes = mroutes;
		else
			continue;

		sprintf(routes, "%s %s:%s:%s:%d", routes, ip, netmask, gateway, atoi(matric)+1);
	}

	nvram_set("lan_route", lroutes);

	snprintf(prefix, sizeof(prefix), "wan%d_", unit);
	nvram_set(strcat_r(prefix, "route", tmp), wroutes);
	nvram_set(strcat_r(prefix, "mroute", tmp), mroutes);
}
