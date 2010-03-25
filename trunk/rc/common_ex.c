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


#include<stdlib.h>
#include<stdio.h>
#include <string.h>
#include<time.h>
#include<signal.h>
#include<bcmnvram.h>
#include<shutils.h>
#include<netconf.h>
#include<wlioctl.h>
#include<sys/time.h>
#include<syslog.h>
#include <stdarg.h>
#include <errno.h>

#include "rc.h"

#define XSTR(s) STR(s)
#define STR(s) #s

static char list[2048];

/* remove space in the end of string */
char *trim_r(char *str)
{
	int i;

	i=strlen(str);

	while(i>=1)
	{
		if (*(str+i-1) == ' ' || *(str+i-1) == 0x0a || *(str+i-1) == 0x0d) *(str+i-1)=0x0;
		else break;
		i--;
	}
	return(str);
}

/* convert mac address format from XXXXXXXXXXXX to XX:XX:XX:XX:XX:XX */
char *mac_conv(char *mac_name, int idx, char *buf)
{
	char *mac, name[32];
	int i, j;

	if (idx!=-1)	
		sprintf(name, "%s%d", mac_name, idx);
	else sprintf(name, "%s", mac_name);

	mac = nvram_safe_get(name);

	j=0;
	if (strlen(mac)!=0) 
	{
		for(i=0; i<12; i++)
		{		
			if (i!=0&&i%2==0) buf[j++] = ':';
			buf[j++] = mac[i];
		}
	}
	buf[j] = 0;

	dprintf("mac: %s\n", buf);

	return(buf);
}

void getsyspara(void)
{
	FILE *fp;
	unsigned long *imagelen;
	char dataPtr[4];
	char verPtr[64];
	char productid[13];
	char fwver[12];

	strcpy(productid, "WLHDD");
	strcpy(fwver, "0.1.0.1");

	if ((fp = fopen("/dev/mtd/1", "rb"))!=NULL)
	{
		if (fseek(fp, 4, SEEK_SET)!=0) goto write_ver;
		fread(dataPtr, 1, 4, fp);
		imagelen = (unsigned long *)dataPtr;

		dprintf("image len %x\n", *imagelen);
		if (fseek(fp, *imagelen - 64, SEEK_SET)!=0) goto write_ver;
		dprintf("seek\n");
		if (!fread(verPtr, 1, 64, fp)) goto write_ver;
		dprintf("ver %x %x", verPtr[0], verPtr[1]);
		strncpy(productid, verPtr + 4, 12);
		productid[12] = 0;
		sprintf(fwver, "%d.%d.%d.%d", verPtr[0], verPtr[1], verPtr[2], verPtr[3]);

		dprintf("get fw ver: %s\n", productid);
		fclose(fp);
	}
write_ver:
	// its a ugle solution for Product ID
	if (strstr(productid, "WL500gx"))
		nvram_set("productid", "WL500g.Deluxe");
	else
		nvram_set("productid", trim_r(productid));
	nvram_set("firmver", trim_r(fwver));
}

/* This function is used to map nvram value from asus to Broadcom */
void convert_asus_values()
{	
	char macbuf[36];
	char servers[64];
	char ifnames[36];
	char sbuf[64];
	int i, num;

	getsyspara();
	/* convert country code from regulation_domain */
	convert_country();

#ifdef WOB
	// add for 4712/5350 which have no eeprom
	nvram_set("il0macaddr", nvram_safe_get("et0macaddr"));
#endif

	if (nvram_match("productid", "WL500b") || 
	    nvram_match("productid", "WL500bv2"))
	{
		nvram_set("wl_gmode", "0");
	}

	cprintf("read from nvram\n");

	/* Wireless Section */
	nvram_set("wl0_bss_enabled", "1");
	
	/* No wsc support yet -- this also fixes nas WPA/WPA-PSK problems */
	nvram_set("wl0_wsc_mode", "disabled");

	/* Country Code */
	nvram_set("wl0_country_code", nvram_safe_get("wl_country_code"));

	/* GMODE */
	nvram_set("wl0_gmode", nvram_safe_get("wl_gmode"));
	nvram_set("wl0_nmode", nvram_safe_get("wl_nmode"));

	if (nvram_match("wl_gmode_protection_x", "1"))
	{
		//cprintf("set to auto\n");
		nvram_set("wl_gmode_protection", "auto");
		nvram_set("wl0_gmode_protection", "auto");
		nvram_set("wl_nmode_protection", "auto");
		nvram_set("wl0_nmode_protection", "auto");
	}
	else
	{
		//cprintf("set to off");
		nvram_set("wl_gmode_protection", "off");
		nvram_set("wl0_gmode_protection", "off");
		nvram_set("wl_nmode_protection", "off");
		nvram_set("wl0_nmode_protection", "off");
	}	

	if (nvram_match("wl_wep_x", "0"))
		nvram_set("wl0_wep", "disabled");
	else nvram_set("wl0_wep", "enabled");

	if (nvram_match("wl_auth_mode", "shared"))
		nvram_set("wl0_auth", "1");
	else nvram_set("wl0_auth", "0");


#ifdef WPA2_WMM
	if (nvram_match("wl_auth_mode", "wpa"))
	{
                nvram_set("wl_akm", "wpa");
                nvram_set("wl0_akm", "wpa");
	}
        else if (nvram_match("wl_auth_mode", "wpa2"))
        {
                nvram_set("wl_akm", "wpa2");
                nvram_set("wl0_akm", "wpa2");
        }
	else if (nvram_match("wl_auth_mode", "psk"))
	{
                if (nvram_match("wl_wpa_mode", "0"))
                {
                        if (nvram_match("wl_crypto", "tkip") || nvram_match("wl_crypto", "0"))
                        {
                                nvram_set("wl_akm", "psk");
                                nvram_set("wl0_akm", "psk");
                        }
                        else if (nvram_match("wl_crypto", "aes"))
                        {
                                nvram_set("wl_akm", "psk2");
                                nvram_set("wl0_akm", "psk2");
                        }
                        else
                        {
                                nvram_set("wl_akm", "psk psk2");
                                nvram_set("wl0_akm", "psk psk2");
                        }
                }
                else if (nvram_match("wl_wpa_mode", "1"))
                {
                        nvram_set("wl_akm", "psk");
                        nvram_set("wl0_akm", "psk");
                }
                else if (nvram_match("wl_wpa_mode", "2"))
                {
                        nvram_set("wl_akm", "psk2");
                        nvram_set("wl0_akm", "psk2");
                }

	}	
	else 
	{
		nvram_set("wl_akm", "");
		nvram_set("wl0_akm", "");
	}

	nvram_set("wl0_auth_mode", 
		nvram_match("wl_auth_mode", "radius") ? "radius" : "none");
	nvram_set("wl0_preauth", nvram_safe_get("wl_preauth"));
	nvram_set("wl0_net_reauth", nvram_safe_get("wl_net_reauth"));
	nvram_set("wl0_wme", nvram_safe_get("wl_wme"));
	nvram_set("wl0_wme_no_ack", nvram_safe_get("wl_wme_no_ack"));
	nvram_set("wl0_wme_sta_bk", nvram_safe_get("wl_wme_sta_bk"));
	nvram_set("wl0_wme_sta_be", nvram_safe_get("wl_wme_sta_be"));
	nvram_set("wl0_wme_sta_vi", nvram_safe_get("wl_wme_sta_vi"));
	nvram_set("wl0_wme_sta_vo", nvram_safe_get("wl_wme_sta_vo"));
	nvram_set("wl0_wme_ap_bk", nvram_safe_get("wl_wme_ap_bk"));
	nvram_set("wl0_wme_ap_be", nvram_safe_get("wl_wme_ap_be"));
	nvram_set("wl0_wme_ap_vi", nvram_safe_get("wl_wme_ap_vi"));
	nvram_set("wl0_wme_ap_vo", nvram_safe_get("wl_wme_ap_vo"));
#else
	nvram_set("wl0_auth_mode", nvram_safe_get("wl_auth_mode"));
	nvram_set("wl_akm", "");
	nvram_set("wl0_akm", "");
	nvram_set("wl0_wme", "off");
#endif

	nvram_set("wl0_rate", nvram_safe_get("wl_rate"));
	nvram_set("wl0_gmode", nvram_get("wl_gmode"));
	nvram_set("wl0_nreqd", "0");	/* off */

	/* BUGBUG: nmcsidx == -1 causes wlconf to ignore wl_rate */
	/* setting completely (resets to auto), so we use something < 0 */
	nvram_set("wl0_nmcsidx", nvram_match("wl_nmcsidx", "-1") ? 
		"-2" : nvram_get("wl_nmcsidx"));
	
	/* override settings for 11n */
	if (nvram_match("wl_gmode", "6"))	/* 11n */
	{
		nvram_set("wl0_rate", "0");
		nvram_set("wl0_nmode", "1");	/* on */
		nvram_set("wl0_gmode", "1");	/* auto */
		nvram_set("wl0_nreqd", "1");	/* on */
	}
	else if (nvram_match("wl_gmode","1"))	/* auto */
	{	
		nvram_set("wl0_nmode", "-1");	/* auto */
	} else {
		nvram_set("wl0_nmode", "0");	/* off */
	}

	nvram_set("wl0_nbw", nvram_safe_get("wl_nbw"));
	/* 0 - 20Mhz, 1 - 40 Mhz, 2 - 20Mhz in 2.4G/40Mhz in 5G */
	nvram_set("wl0_nbw_cap", nvram_match("wl_nbw", "20") ? "0" : "1");
	nvram_set("wl0_nctrlsb", nvram_safe_get("wl_nctrlsb"));
	nvram_set("wl0_nband", nvram_safe_get("wl_nband"));
	nvram_set("wl0_reg_mode", nvram_safe_get("wl_reg_mode"));

	nvram_set("wl0_ssid", nvram_safe_get("wl_ssid"));
	nvram_set("wl0_channel", nvram_safe_get("wl_channel"));
	nvram_set("wl0_country_code", nvram_safe_get("wl_country_code"));
	nvram_set("wl0_rate", nvram_safe_get("wl_rate"));
	nvram_set("wl0_mrate", nvram_safe_get("wl_mrate"));
	nvram_set("wl0_rateset", nvram_safe_get("wl_rateset"));
	nvram_set("wl0_frag", nvram_safe_get("wl_frag"));
	nvram_set("wl0_rts", nvram_safe_get("wl_rts"));
	nvram_set("wl0_dtim", nvram_safe_get("wl_dtim"));
	nvram_set("wl0_bcn", nvram_safe_get("wl_bcn"));
	nvram_set("wl0_plcphdr", nvram_safe_get("wl_plcphdr"));
	nvram_set("wl0_crypto", nvram_safe_get("wl_crypto"));
	nvram_set("wl0_wpa_psk", nvram_safe_get("wl_wpa_psk"));
	nvram_set("wl0_key", nvram_safe_get("wl_key"));
	nvram_set("wl0_key1", nvram_safe_get("wl_key1"));
	nvram_set("wl0_key2", nvram_safe_get("wl_key2"));
	nvram_set("wl0_key3", nvram_safe_get("wl_key3"));
	nvram_set("wl0_key4", nvram_safe_get("wl_key4"));
	nvram_set("wl0_closed", nvram_safe_get("wl_closed"));
	nvram_set("wl0_frameburst", nvram_safe_get("wl_frameburst"));
	nvram_set("wl0_afterburner", nvram_safe_get("wl_afterburner"));
	nvram_set("wl0_ap_isolate", nvram_safe_get("wl_ap_isolate"));
	nvram_set("wl0_radio", nvram_safe_get("wl_radio_x"));
	nvram_set("wl0_radius_ipaddr", nvram_safe_get("wl_radius_ipaddr"));
	nvram_set("wl0_radius_port", nvram_safe_get("wl_radius_port"));
	nvram_set("wl0_radius_key", nvram_safe_get("wl_radius_key"));
	nvram_set("wl0_wpa_gtk_rekey", nvram_safe_get("wl_wpa_gtk_rekey"));


	if (nvram_invmatch("wl_mode_ex", "ap"))
	{
		int wepidx=atoi(nvram_safe_get("wl0_key"));
		char wepname[16];

		sprintf(sbuf, "wl join \"%s\"", nvram_safe_get("wl0_ssid"));

		// key ??
		if (nvram_match("wl0_auth_mode", "psk"))
		{
			sprintf(sbuf, "%s key %s", sbuf, nvram_safe_get("wl0_wpa_psk"));
		}
		else if (nvram_match("wl0_wep", "enabled") && wepidx>=1 && wepidx<=4)
		{
			sprintf(wepname, "wl0_key%d", wepidx);
			sprintf(sbuf, "%s key %s", sbuf, nvram_safe_get(wepname));
		}
		
		sprintf(sbuf, "%s imode bss", sbuf);
		
		if (nvram_match("wl_auth_mode", "psk"))
			sprintf(sbuf, "%s amode wpapsk", sbuf);
		else if (nvram_match("wl_auth_mode", "shared"))
			sprintf(sbuf, "%s amode shared", sbuf);
		else sprintf(sbuf, "%s amode open", sbuf);

		nvram_set("wl0_join", sbuf);
		/* Station or Ethernet Bridge Mode */
		nvram_set("wl0_mode", nvram_safe_get("wl_mode_ex"));
	}
	else
	{
		/* WDS control */
		if (nvram_match("wl_mode_x", "1")) 
			nvram_set("wl0_mode", "wds");
		else nvram_set("wl0_mode", "ap");

		nvram_set("wl0_lazywds", nvram_safe_get("wl_lazywds"));
	}

	if (nvram_match("wl_wdsapply_x", "1"))
	{
		num = atoi(nvram_safe_get("wl_wdsnum_x"));
		list[0]=0;

		for(i=0;i<num;i++)
		{
			sprintf(list, "%s %s", list, mac_conv("wl_wdslist_x", i, macbuf));

		}

		dprintf("wds list %s %x\n", list, num);

		nvram_set("wl0_wds", list);
	}
	else nvram_set("wl0_wds", "");

	/* Mac filter */
	nvram_set("wl0_macmode", nvram_safe_get("wl_macmode"));

	if (nvram_invmatch("wl_macmode", "disabled"))
	{
		num = atoi(nvram_safe_get("wl_macnum_x"));
		list[0]=0;

		for(i=0;i<num;i++)
		{
			sprintf(list, "%s %s", list, mac_conv("wl_maclist_x", i, macbuf));
			
		}

		//printf("mac list %s %x\n", list, num);

		nvram_set("wl0_maclist", list);
	}

	/* Direct copy value */
	/* LAN Section */
	if (nvram_match("dhcp_enable_x", "1"))
		nvram_set("lan_proto", "dhcp");
	else nvram_set("lan_proto", "static");


	nvram_set("wan0_proto", nvram_safe_get("wan_proto"));
	nvram_set("wan0_ipaddr", nvram_safe_get("wan_ipaddr"));
	nvram_set("wan0_netmask", nvram_safe_get("wan_netmask"));
	nvram_set("wan0_gateway", nvram_safe_get("wan_gateway"));

	nvram_set("wan_ipaddr_t", "");
	nvram_set("wan_netmask_t", "");
	nvram_set("wan_gateway_t", "");
	nvram_set("wan_dns_t", "");
	nvram_set("wan_status_t", "Disconnected");

	if (nvram_match("wan_proto", "pppoe") || nvram_match("wan_proto", "pptp") ||
		nvram_match("wan_proto", "l2tp"))
	{
		nvram_set("wan0_pppoe_ifname", "ppp0");
		nvram_set("upnp_wan_proto", "pppoe");
		nvram_set("wan0_pppoe_username", nvram_safe_get("wan_pppoe_username"));
		nvram_set("wan0_pppoe_passwd", nvram_safe_get("wan_pppoe_passwd"));
		nvram_set("wan0_pppoe_idletime", nvram_safe_get("wan_pppoe_idletime"));
		nvram_set("wan0_pppoe_txonly_x", nvram_safe_get("wan_pppoe_txonly_x"));
		nvram_set("wan0_pppoe_mtu", nvram_safe_get("wan_pppoe_mtu"));
		nvram_set("wan0_pppoe_mru", nvram_safe_get("wan_pppoe_mru"));
		nvram_set("wan0_pppoe_service", nvram_safe_get("wan_pppoe_service"));
		nvram_set("wan0_pppoe_ac", nvram_safe_get("wan_pppoe_ac"));
		nvram_set("wan0_pppoe_options_x", nvram_safe_get("wan_pppoe_options_x"));
		nvram_set("wan0_pptp_options_x", nvram_safe_get("wan_pptp_options_x"));
#ifdef REMOVE
		nvram_set("wan0_pppoe_demand", "1");
		nvram_set("wan0_pppoe_keepalive", "1");
#endif
		nvram_set("wan0_pppoe_ipaddr", nvram_safe_get("wan_ipaddr"));
		nvram_set("wan0_pppoe_netmask", 
			inet_addr_(nvram_safe_get("wan_ipaddr")) && 
			inet_addr_(nvram_safe_get("wan_netmask")) ? 
				nvram_get("wan_netmask") : NULL);
		nvram_set("wan0_pppoe_gateway", nvram_get("wan_gateway"));
		
		/* current interface address (dhcp + firewall) */
		nvram_set("wanx_ipaddr", nvram_safe_get("wan_ipaddr"));
	}
	nvram_set("wan0_hostname", nvram_safe_get("wan_hostname"));

	if (nvram_invmatch("wan_hwaddr_x", ""))
	{
		char *wan_mac = mac_conv("wan_hwaddr_x", -1, macbuf);

		nvram_set("wan_hwaddr", wan_mac);
		nvram_set("wan0_hwaddr", wan_mac);
	}
	else
	{
		nvram_unset("wan_hwaddr");
		nvram_unset("wan0_hwaddr");
	}

	nvram_set("wan0_dnsenable_x", nvram_safe_get("wan_dnsenable_x"));
	nvram_unset("wan0_dns");
	nvram_unset("wanx_dns");

	convert_routes();

	memset(servers, 0, sizeof(servers));

	if (nvram_invmatch("ntp_server0", ""))
		sprintf(servers, "%s%s ", servers, nvram_safe_get("ntp_server0"));
	if (nvram_invmatch("ntp_server1", ""))
		sprintf(servers, "%s%s ", servers, nvram_safe_get("ntp_server1"));

	nvram_set("ntp_servers", servers);

	if (nvram_match("wan_nat_x", "0") && nvram_match("wan_route_x", "IP_Bridged"))
	{
		sprintf(ifnames, "%s", nvram_safe_get("lan_ifnames"));
		sprintf(ifnames, "%s %s", ifnames, nvram_safe_get("wan_ifname"));
		nvram_set("router_disable", "1");
		nvram_set("vlan_enable", "0");
	}
	else if (nvram_invmatch("wl_mode_ex", "ap")) 
	{
		char name[80], *next;
		char *wl_ifname = nvram_safe_get("wl0_ifname");

		strcpy(ifnames, nvram_safe_get("wan_ifname"));
		/* remove wl_ifname from the ifnames */
		foreach(name, nvram_safe_get("lan_ifnames"), next) {
			if (strcmp(name, wl_ifname)) {
				sprintf(ifnames, "%s %s", ifnames, name);
			}
		}

		nvram_set("router_disable", "0");
		nvram_set("vlan_enable", "0");
	}
	else 
	{ 
		strcpy(ifnames, nvram_safe_get("lan_ifnames"));
		nvram_set("router_disable", "0");
		nvram_set("vlan_enable", "1");
	}
	
	nvram_set("lan_ifnames_t", ifnames);

	// clean some temp variables
	nvram_set("usb_ftp_device", "");
	/* force mounting (boot_local and wl-hdd) */
	nvram_set("usb_storage_device", "");
	nvram_set("usb_web_device", "");
	nvram_set("usb_audio_device", "");
	nvram_set("usb_webdriver_x", "");
	nvram_set("usb_web_flag", "");
#ifdef __CONFIG_MADWIMAX__
	nvram_set("usb_wimax_device", "");
#endif
	nvram_set("no_br", "0");

	if(nvram_invmatch("sp_battle_ips", "0"))
	{
		eval("insmod", "ip_nat_starcraft");
		eval("insmod", "ipt_NETMAP");
	}

#ifdef WEBSTRFILTER
	if (nvram_match("url_enable_x", "1")) {
		eval("insmod", "ipt_webstr");
	}
#endif

        //2005/09/22 insmod FTP module
        if (nvram_match("usb_ftpenable_x", "1") && atoi(nvram_get("usb_ftpport_x"))!=21)
        {
                char ports[32];

                sprintf(ports, "ports=21,%d", atoi(nvram_get("usb_ftpport_x")));
                eval("insmod", "ip_conntrack_ftp", ports);
                eval("insmod", "ip_nat_ftp", ports);
        }
        else
        {
                eval("insmod", "ip_conntrack_ftp");
                eval("insmod", "ip_nat_ftp");
        }

	if ((nvram_match("ssh_enable", "1") && nvram_invmatch("recent_ssh_enable", "0")) ||
	    (nvram_match("usb_ftpenable_x", "1") && nvram_invmatch("recent_ftp_enable", "0")))
	{
		eval("insmod", "ipt_recent");
	}

#ifdef __CONFIG_IPV6__
	if (nvram_invmatch("ipv6_proto", ""))
	{
#ifndef BROKEN_IPV6_CONNTRACK
		eval("insmod", "ip6_conntrack");
		eval("insmod", "ip6t_state");
//		eval("insmod", "ip6t_TCPMSS");
#endif
	} else {
		/* FIXME: Move it to the right place */
		FILE *fp;

		if ((fp = fopen("/proc/sys/net/ipv6/conf/all/disable_ipv6", "r+")))
		{
			fputc('1', fp);
			fclose(fp);
		} else
			perror("/proc/sys/net/ipv6/conf/all/disable_ipv6");
	}
#endif

	update_lan_status(1);

	dprintf("end map\n");
}

void update_lan_status(int isup)
{
	if (isup)
	{	
		nvram_set("lan_ipaddr_t", nvram_safe_get("lan_ipaddr"));
		nvram_set("lan_netmask_t", nvram_safe_get("lan_netmask"));

		if (nvram_match("wan_route_x", "IP_Routed"))
		{
			if (nvram_match("lan_proto", "dhcp"))
			{
				if (nvram_invmatch("dhcp_gateway_x", ""))
					nvram_set("lan_gateway_t", nvram_safe_get("dhcp_gateway_x"));
				else nvram_set("lan_gateway_t", nvram_safe_get("lan_ipaddr"));
			}
			else nvram_set("lan_gateway_t", nvram_safe_get("lan_ipaddr"));
		}
		else nvram_set("lan_gateway_t", nvram_safe_get("lan_gateway"));
	}
	else
	{
		nvram_set("lan_ipaddr_t", "");
		nvram_set("lan_netmask_t", "");
		nvram_set("lan_gateway_t", "");
	}
}


void update_wan_status(int isup)
{
	char *proto;
	char dns_str[36];

	proto = nvram_safe_get("wan_proto");

	if (!strcmp(proto, "static")) nvram_set("wan_proto_t", "Static");
	else if (!strcmp(proto, "dhcp")) nvram_set("wan_proto_t", "Automatic IP");
	else if (!strcmp(proto, "pppoe")) nvram_set("wan_proto_t", "PPPoE");	
	else if (!strcmp(proto, "pptp")) nvram_set("wan_proto_t", "PPTP");
	else if (!strcmp(proto, "l2tp")) nvram_set("wan_proto_t", "L2TP");
	else if (!strcmp(proto, "bigpond")) nvram_set("wan_proto_t", "BigPond");
#ifdef __CONFIG_MADWIMAX__
	else if (!strcmp(proto, "wimax")) nvram_set("wan_proto_t", "WiMAX");
#endif

	if (!isup)
	{
		nvram_set("wan_ipaddr_t", "");
		nvram_set("wan_netmask_t", "");
		nvram_set("wan_gateway_t", "");
		nvram_set("wan_dns_t", "");
		nvram_set("wan_ifname_t", "");
		nvram_set("wan_status_t", "Disconnected");
	}	
	else
	{
		nvram_set("wan_ipaddr_t", nvram_safe_get("wan0_ipaddr"));
		nvram_set("wan_netmask_t", nvram_safe_get("wan0_netmask"));
		nvram_set("wan_gateway_t", nvram_safe_get("wan0_gateway"));


		if (nvram_invmatch("wan_dnsenable_x", "1"))	
		{		
			dns_str[0] = '\0';
			if (nvram_invmatch("wan_dns1_x",""))
				sprintf(dns_str, "%s", nvram_safe_get("wan_dns1_x"));		
			if (nvram_invmatch("wan_dns2_x",""))
				sprintf(dns_str+strlen(dns_str), " %s", nvram_safe_get("wan_dns2_x"));
			nvram_set("wan_dns_t", dns_str);
		}
		else nvram_set("wan_dns_t", nvram_safe_get("wan0_dns"));
		nvram_set("wan_status_t", "Connected");
	}
}

/*
 * logmessage
 *
 */
void logmessage(char *logheader, char *fmt, ...)
{
  va_list args;
  char buf[512];

  va_start(args, fmt);

  vsnprintf(buf, sizeof(buf), fmt, args);
  openlog(logheader, 0, 0);
  syslog(0, buf);
  closelog();
  va_end(args);
}


/*
 * wanmessage
 *
 */
void wanmessage(char *fmt, ...)
{
  va_list args;
  char buf[512];

  va_start(args, fmt);
  vsnprintf(buf, sizeof(buf), fmt, args);
  nvram_set("wan_reason_t", buf);
  va_end(args);
}

/* 
 * pppstatus
 * 
 */
char *pppstatus(char *buf)
{
   FILE *fp;
   char sline[128], *p;

   if ((fp=fopen("/tmp/wanstatus.log", "r")) && fgets(sline, sizeof(sline), fp))
   {
	p = strstr(sline, ",");
	strcpy(buf, p+1);
	fclose(fp);
   }
   else
   {
	strcpy(buf, "unknown reason");
   }	
   return buf;
}


