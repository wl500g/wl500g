/*
 * Broadcom Home Gateway Reference Design
 * Web Page Configuration Support Routines
 *
 * Copyright 2004, Broadcom Corporation
 * All Rights Reserved.
 * 
 * THIS SOFTWARE IS OFFERED "AS IS", AND BROADCOM GRANTS NO WARRANTIES OF ANY
 * KIND, EXPRESS OR IMPLIED, BY STATUTE, COMMUNICATION OR OTHERWISE. BROADCOM
 * SPECIFICALLY DISCLAIMS ANY IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS
 * FOR A SPECIFIC PURPOSE OR NONINFRINGEMENT CONCERNING THIS SOFTWARE.
 * $Id$
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <errno.h>
#include <unistd.h>
#include <limits.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <assert.h>
#include <httpd.h>


#include <typedefs.h>
#include <proto/ethernet.h>
#include <bcmnvram.h>
#include <bcmutils.h>
#include <shutils.h>
#include <netconf.h>
#include <nvparse.h>
#include <wlutils.h>

extern char *reltime(unsigned int seconds);

#define wan_prefix(unit, prefix)	snprintf(prefix, sizeof(prefix), "wan%d_", unit)


#define XSTR(s) STR(s)
#define STR(s) #s

#define EZC_FLAGS_READ		0x0001
#define EZC_FLAGS_WRITE		0x0002
#define EZC_FLAGS_CRYPT		0x0004

#define EZC_CRYPT_KEY		"620A83A6960E48d1B05D49B0288A2C1F"

#define EZC_SUCCESS	 	0
#define EZC_ERR_NOT_ENABLED 	1
#define EZC_ERR_INVALID_STATE 	2
#define EZC_ERR_INVALID_DATA 	3


#if defined(linux)

#include <fcntl.h>
#include <signal.h>
#include <time.h>
#include <sys/klog.h>
#include <sys/wait.h>
#include <sys/ioctl.h>
#include <net/if.h>

typedef u_int64_t u64;
typedef u_int32_t u32;
typedef u_int16_t u16;
typedef u_int8_t u8;
#include <linux/types.h>
#include <linux/ethtool.h>
#include <linux/sockios.h>
#include <net/if_arp.h>


#define MIN_BUF_SIZE	4096

struct lease_t {
	unsigned char chaddr[16];
	u_int32_t yiaddr;
	u_int32_t expires;
	char hostname[64];
};

/* Dump leases in <tr><td>hostname</td><td>MAC</td><td>IP</td><td>expires</td></tr> format */
int
ej_lan_leases(int eid, webs_t wp, int argc, char_t **argv)
{
	FILE *fp = NULL;
	struct lease_t lease;
	int i;
	struct in_addr addr;
	unsigned long expires = 0;
	int ret = 0;

        ret += websWrite(wp, "Host Name       Mac Address       IP Address      Lease\n");
			                                                  
	/* Write out leases file */
	if (!(fp = fopen("/tmp/udhcpd-br0.leases", "r")))
		return ret;

	while (fread(&lease, sizeof(lease), 1, fp)) {
		/* Do not display reserved leases */
		if (ETHER_ISNULLADDR(lease.chaddr))
			continue;

		//printf("lease: %s %d\n", lease.hostname, strlen(lease.hostname));
		ret += websWrite(wp, "%-16s", lease.hostname);
		for (i = 0; i < 6; i++) {
			ret += websWrite(wp, "%02X", lease.chaddr[i]);
			if (i != 5) ret += websWrite(wp, ":");
		}
		addr.s_addr = lease.yiaddr;
		ret += websWrite(wp, " %-15s ", inet_ntoa(addr));
		expires = ntohl(lease.expires);

		if (expires==0xffffffff) ret += websWrite(wp, "Manual\n");
		else if (!expires) ret += websWrite(wp, "Expired\n");
		else ret += websWrite(wp, "%s\n", reltime(expires));
	}
	fclose(fp);

	return ret;
}

/* Renew lease */
int
sys_renew(void)
{
	int unit;
	char tmp[100];

	if ((unit = nvram_get_int("wan_unit")) < 0)
		unit = 0;

#ifdef REMOVE	
	char *str;
	int pid;

	snprintf(tmp, sizeof(tmp), "/var/run/udhcpc%d.pid", unit);
	if ((str = file2str(tmp))) {
		pid = atoi(str);
		free(str);
		return kill(pid, SIGUSR1);
	}	
	return -1;
#else
	snprintf(tmp, sizeof(tmp), "wan_connect,%d", unit);
	nvram_set("rc_service", tmp);
	return kill(1, SIGUSR1);
#endif
}

/* Release lease */
int
sys_release(void)
{
	int unit;
	char tmp[100];

	if ((unit = nvram_get_int("wan_unit")) < 0)
		unit = 0;
	
#ifdef REMOVE
	char *str;
	int pid;

	snprintf(tmp, sizeof(tmp), "/var/run/udhcpc%d.pid", unit);
	if ((str = file2str(tmp))) {
		pid = atoi(str);
		free(str);
		return kill(pid, SIGUSR2);
	}	
	return -1;
#else	
	snprintf(tmp, sizeof(tmp), "wan_disconnect,%d", unit);
	nvram_set("rc_service", tmp);
	return kill(1, SIGUSR1);
#endif
}

#ifdef REMOVE
/* Display IP Address lease */
int
ej_wan_lease(int eid, webs_t wp, int argc, char_t **argv)
{
	unsigned long expires = 0;
	int ret = 0;
	int unit;
	char tmp[100], prefix[] = "wanXXXXXXXXXX_";

	if ((unit = nvram_get_int("wan_unit")) < 0)
		unit = 0;
	wan_prefix(unit, prefix);
	
	if (nvram_match(strcat_r(prefix, "proto", tmp), "dhcp")) {
		char *str;
		time_t now;

		snprintf(tmp, sizeof(tmp), "/tmp/udhcpc%d.expires", unit); 
		if ((str = file2str(tmp))) {
			expires = atoi(str);
			free(str);
		}
		time(&now);
		if (expires <= now)
			ret += websWrite(wp, "Expired");
		else
			ret += websWrite(wp, "%s", reltime(expires - now));
	} else
		ret += websWrite(wp, "N/A");

	return ret;
}


/* Return a list of wan interfaces (eth0/eth1/eth2/eth3) */
static int
ej_wan_iflist(int eid, webs_t wp, int argc, char_t **argv)
{
	char name[IFNAMSIZ], *next;
	int ret = 0;
	int unit;
	char tmp[100], prefix[] = "wanXXXXXXXXXX_";
	char ea[64];
	int s;
	struct ifreq ifr;

	/* current unit # */
	if ((unit = nvram_get_int("wan_unit")) < 0)
		unit = 0;
	wan_prefix(unit, prefix);
	
	if ((s = socket(AF_INET, SOCK_RAW, IPPROTO_RAW)) < 0)
		return errno;
	
	/* build wan interface name list */
	foreach(name, nvram_safe_get("wan_ifnames"), next) {
		strncpy(ifr.ifr_name, name, IFNAMSIZ);
		if (ioctl(s, SIOCGIFHWADDR, &ifr))
			continue;
		ret += websWrite(wp, "<option value=\"%s\" %s>%s (%s)</option>", name,
				 nvram_match(strcat_r(prefix, "ifname", tmp), name) ? "selected" : "",
				 name, ether_etoa(ifr.ifr_hwaddr.sa_data, ea));
	}

	close(s);

	return ret;
}
#endif // REMOVE


#endif // linux

static int
ej_wl_sta_status(int eid, webs_t wp, char *ifname)
{
	int ret;
	unsigned char bssid[32];

	// Get bssid
	ret = wl_ioctl(ifname, WLC_GET_BSSID, bssid, sizeof(bssid));

	if (ret == 0 && memcmp(bssid, "\x00\x00\x00\x00\x00", 6))
	{
		uint32 rssi;
		
		if (wl_ioctl(ifname, WLC_GET_RSSI, &rssi, sizeof(rssi)))
			return(websWrite(wp, "Status	: Connected to %s\n"
				     "AP	: %02x:%02x:%02x:%02x:%02x:%02x\n", 
				nvram_safe_get("wl0_ssid"), 
				bssid[0], bssid[1], bssid[2], bssid[3], bssid[4], bssid[5]));
		else	return(websWrite(wp, "Status	: Connected to %s\n"
				     "AP	: %02x:%02x:%02x:%02x:%02x:%02x\n"
				     "Signal	: %d dBm\n",  nvram_safe_get("wl0_ssid"), 
				bssid[0], bssid[1], bssid[2], bssid[3], bssid[4], bssid[5], rssi));
	}
	return(websWrite(wp, "Status	: Connecting to %s\n", nvram_safe_get("wl0_ssid")));
}


int
ej_wl_status(int eid, webs_t wp, int argc, char_t **argv)
{
	int unit;
	char tmp[100], prefix[sizeof("wlXXXXXXXXXX_")];
	char *name;
	struct maclist *auth, *assoc, *authorized;
	int max_sta_count, maclist_size;
	int i, j, val;
	int ret = 0;
	channel_info_t ci;
	char chanspec[16]; /* sizeof("255 + 255") */

	if ((unit = nvram_get_int("wl_unit")) < 0)
		return -1;

	snprintf(prefix, sizeof(prefix), "wl%d_", unit);
	name = nvram_safe_get(strcat_r(prefix, "ifname", tmp));		
	
	wl_ioctl(name, WLC_GET_RADIO, &val, sizeof(val));

	if (val==1) 
	{
		ret+=websWrite(wp, "Radio is disabled\n");
		return 0;
	}
	
	/* query wl for chanspec */
	if (wl_ioctl(name, WLC_GET_VAR, &chanspec, sizeof(chanspec)) == 0)
	{
		val = *(chanspec_t *) &chanspec;
		snprintf(chanspec, sizeof(chanspec),
		    CHSPEC_IS40(val) ? "%d + %d" : "%d",
		    CHSPEC_IS40(val) ? CHSPEC_CTL_CHAN(val)
				     : CHSPEC_CHANNEL(val),
		    CHSPEC_SB_LOWER(val) ? UPPER_20_SB(CHSPEC_CHANNEL(val))
					 : LOWER_20_SB(CHSPEC_CHANNEL(val)));
	} else {
		wl_ioctl(name, WLC_GET_CHANNEL, &ci, sizeof(ci));
		snprintf(chanspec, sizeof(chanspec), "%d", ci.target_channel);
	}

	if (nvram_match(strcat_r(prefix, "mode", tmp), "ap"))
	{
		if (nvram_match("wl_lazywds", "1") ||
			nvram_match("wl_wdsapply_x", "1"))
			ret+=websWrite(wp, "Mode	: Hybrid\n");
		else    ret+=websWrite(wp, "Mode	: AP Only\n");

		ret+=websWrite(wp, "Channel	: %s\n", chanspec);

	}
	else if (nvram_match(strcat_r(prefix, "mode", tmp), "wds"))
	{
		ret+=websWrite(wp, "Mode	: WDS Only\n");
		ret+=websWrite(wp, "Channel	: %s\n", chanspec);
	}
	else if (nvram_match(strcat_r(prefix, "mode", tmp), "sta"))
	{
		ret+=websWrite(wp, "Mode	: Station\n");
		ret+=websWrite(wp, "Channel	: %s\n", chanspec);
		ret+=ej_wl_sta_status(eid, wp, name);
		return ret;
	}
	else if (nvram_match(strcat_r(prefix, "mode", tmp), "wet"))
	{
		ret+=websWrite(wp, "Mode	: Ethernet Bridge\n");
		ret+=websWrite(wp, "Channel	: %s\n", chanspec);
		ret+=ej_wl_sta_status(eid, wp, name);
		return ret;
	}	

	/* buffers and length */
	max_sta_count = 256;
	maclist_size = sizeof(auth->count) + max_sta_count * sizeof(struct ether_addr);

	auth = malloc(maclist_size);
	assoc = malloc(maclist_size);
	authorized = malloc(maclist_size);

	if (!auth || !assoc || !authorized)
		goto exit;

	/* query wl for authenticated sta list */
	strcpy((char*)auth, "authe_sta_list");
	if (wl_ioctl(name, WLC_GET_VAR, auth, maclist_size))
		goto exit;

	/* query wl for associated sta list */
	assoc->count = max_sta_count;
	if (wl_ioctl(name, WLC_GET_ASSOCLIST, assoc, maclist_size))
		goto exit;

	/* query wl for authorized sta list */
	strcpy((char*)authorized, "autho_sta_list");
	if (wl_ioctl(name, WLC_GET_VAR, authorized, maclist_size))
		goto exit;


	websWrite(wp, "\n");
	websWrite(wp, "Stations List                           \n");
	websWrite(wp, "----------------------------------------\n");
	//             00:00:00:00:00:00 associated authorized

	/* build authenticated/associated/authorized sta list */
	for (i = 0; i < auth->count; i ++) {
		char ea[ETHER_ADDR_STR_LEN];

		websWrite(wp, "%s ", ether_etoa((void *)&auth->ea[i], ea));

		for (j = 0; j < assoc->count; j ++) {
			if (!memcmp((void *)&auth->ea[i], (void *)&assoc->ea[j], ETHER_ADDR_LEN)) {
				websWrite(wp, " associated");
				break;
			}
		}

		for (j = 0; j < authorized->count; j ++) {
			if (!memcmp((void *)&auth->ea[i], (void *)&authorized->ea[j], ETHER_ADDR_LEN)) {
				websWrite(wp, " authorized");
				break;
			}
		}
		websWrite(wp, "\n");
	}

	/* error/exit */
exit:
	if (auth) free(auth);
	if (assoc) free(assoc);
	if (authorized) free(authorized);
	
	return 0;
}


/* Dump NAT table <tr><td>destination</td><td>MAC</td><td>IP</td><td>expires</td></tr> format */
int
ej_nat_table(int eid, webs_t wp, int argc, char_t **argv)
{
	int needlen = 0, listlen, i, ret = 0;
	netconf_nat_t *nat_list = NULL;
	char line[256], tstr[32];

	ret += websWrite(wp, "Source          Destination     Proto.  Port range  Redirect to     Local port\n");
	/*                   "255.255.255.255 255.255.255.255 ALL     65535:65535 255.255.255.255 65535:65535" */

	netconf_get_nat(NULL, &needlen);
	if (needlen <= 0)
		return ret;

	nat_list = (netconf_nat_t *) malloc(needlen);
	if (nat_list == NULL)
		return ret;

	memset(nat_list, 0, needlen);
	listlen = needlen;
	if (netconf_get_nat(nat_list, &listlen) == 0 && needlen == listlen)
	{
		listlen = needlen/sizeof(netconf_nat_t);
		for (i = 0; i < listlen; i++)
		{
			//printf("%d %d %d\n", nat_list[i].target,
		        //		nat_list[i].match.ipproto,
			//		nat_list[i].match.dst.ipaddr.s_addr);	
			if (nat_list[i].target == NETCONF_DNAT)
			{
				/* Source */
				if (nat_list[i].match.src.ipaddr.s_addr == 0)
					sprintf(line, "%-15s", "ALL");
				else
					sprintf(line, "%-15s", inet_ntoa(nat_list[i].match.src.ipaddr));

				/* Destination */
				if (nat_list[i].match.dst.ipaddr.s_addr == 0)
					sprintf(line, "%s %-15s", line, "ALL");
				else
					sprintf(line, "%s %-15s", line, inet_ntoa(nat_list[i].match.dst.ipaddr));

                                    /* Proto. */
				if (ntohs(nat_list[i].match.dst.ports[0]) == 0)
					sprintf(line, "%s %-7s", line, "ALL");
				else if (nat_list[i].match.ipproto == IPPROTO_TCP)
					sprintf(line, "%s %-7s", line, "TCP");
				else if (nat_list[i].match.ipproto == IPPROTO_UDP)
					sprintf(line, "%s %-7s", line, "UDP");
				else
					sprintf(line, "%s %-7d", line, nat_list[i].match.ipproto);

				/* Port range */
				if (nat_list[i].match.dst.ports[0] == nat_list[i].match.dst.ports[1])
				{
					if (ntohs(nat_list[i].match.dst.ports[0]) == 0)
						sprintf(line, "%s %-11s", line, "ALL");
					else
						sprintf(line, "%s %-11d", line, ntohs(nat_list[i].match.dst.ports[0]));
				} else {
					sprintf(tstr, "%d:%d",
						ntohs(nat_list[i].match.dst.ports[0]),
						ntohs(nat_list[i].match.dst.ports[1]));
					sprintf(line, "%s %-11s", line, tstr);
				}

				/* Redirect to */
				sprintf(line, "%s %-15s", line, inet_ntoa(nat_list[i].ipaddr));

				/* Local port */
				if (nat_list[i].ports[0] == nat_list[i].ports[1])
				{
					if (ntohs(nat_list[i].ports[0]) == 0)
						sprintf(line, "%s %-11s", line, "ALL");
					else
						sprintf(line, "%s %-11d", line, ntohs(nat_list[i].ports[0]));
				} else {
					sprintf(tstr, "%d:%d",
						ntohs(nat_list[i].ports[0]),
						ntohs(nat_list[i].ports[1]));
					sprintf(line, "%s %-11s", line, tstr);
				}

				sprintf(line, "%s\n", line);
				ret += websWrite(wp, line);
			}
	    	}
	}
	free(nat_list);

	return ret;
}

int
ej_route_table(int eid, webs_t wp, int argc, char_t **argv)
{
	char buff[256];
	int  nl = 0 ;
	struct in_addr dest;
	struct in_addr gw;
	struct in_addr mask;
	int flgs, ref, use, metric;
	int ret = 0;
	char flags[4];
	unsigned long int d,g,m;
	char sdest[16], sgw[16];
	FILE *fp;

        ret += websWrite(wp, "Destination     Gateway         Genmask         Flags Metric Ref    Use Iface\n");

	if (!(fp = fopen("/proc/net/route", "r"))) return 0;

	while (fgets(buff, sizeof(buff), fp) != NULL ) 
	{
		if (nl) 
		{
			int ifl = 0;
			while (buff[ifl]!=' ' && buff[ifl]!='\t' && buff[ifl]!='\0')
				ifl++;
			buff[ifl]=0;    /* interface */
			if (sscanf(buff+ifl+1, "%lx%lx%d%d%d%d%lx",
			   &d, &g, &flgs, &ref, &use, &metric, &m)!=7) {
				//error_msg_and_die( "Unsuported kernel route format\n");
				//continue;
			}

			ifl = 0;        /* parse flags */
			if (flgs&1)
				flags[ifl++]='U';
			if (flgs&2)
				flags[ifl++]='G';
			if (flgs&4)
				flags[ifl++]='H';
			flags[ifl]=0;
			dest.s_addr = d;
			gw.s_addr   = g;
			mask.s_addr = m;
			strcpy(sdest,  (dest.s_addr==0 ? "default" :
					inet_ntoa(dest)));
			strcpy(sgw,    (gw.s_addr==0   ? "*"       :
					inet_ntoa(gw)));

			if (strstr(buff, "br0"))
			{
				ret += websWrite(wp, "%-16s%-16s%-16s%-6s%-6d %-2d %7d LAN %s\n",
				sdest, sgw,
				inet_ntoa(mask),
				flags, metric, ref, use, buff);
			}
			else if(!strstr(buff, "lo"))
			{
				ret += websWrite(wp, "%-16s%-16s%-16s%-6s%-6d %-2d %7d WAN %s\n",
				sdest, sgw,
				inet_ntoa(mask),
				flags, metric, ref, use, buff);
			}
		}
		nl++;
	}
	fclose(fp);

	return 0;
}
