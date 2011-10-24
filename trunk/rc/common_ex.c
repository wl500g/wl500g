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


#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <signal.h>
#include <sys/time.h>
#include <sys/sysinfo.h>
#include <syslog.h>
#include <stdarg.h>
#include <unistd.h>
#include <errno.h>

#include <bcmnvram.h>
#include <shutils.h>
#include <netconf.h>
#include <wlioctl.h>
#include "nvparse.h"
#include "rc.h"
#include "mtd.h"

#define XSTR(s) STR(s)
#define STR(s) #s


/* remove space in the end of string */
char *trim_r(char *str)
{
	char *end = NULL;

	if (str)
		end = str + strlen(str) - 1;
	while (end && end >= str && strchr("\x20\r\n", *end))
		*end-- = '\0';
	return str;
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
		for (i=0; i<12; i++)
		{		
			if (i!=0&&i%2==0) buf[j++] = ':';
			buf[j++] = mac[i];
		}
	}
	buf[j] = 0;

	dprintf("mac: %s\n", buf);

	return(buf);
}

/* load kernel module, trailing NULL in arguments list required! */
int insmod(char *module, ...)
{
        char *argv[8] = { "insmod", module, NULL };
	int i = 2;
	va_list ap;

	va_start(ap, module);
	while (i < 7 && (argv[i] = va_arg(ap, char *)) != NULL) {
		++i;
	}
	va_end(ap);
	argv[i] = NULL;
        return _eval(argv, ">/dev/null", 0, NULL);
}

int rmmod(char *module)
{
	return eval("rmmod", module);
}

int killall_w(char *program, unsigned sig, int wait)
{
	char sigstr[sizeof("-65535")];
	char *argv[] = { "killall",
				NULL,   /* -w */
				NULL,   /* signal */
				NULL,   /* program */
				NULL };
	int i = 1;

#ifdef KILLALL_HAS_WAIT
	if (wait)
		argv[i++] = "-w";
#endif
	if (sig) {
		snprintf(sigstr, sizeof(sigstr), "-%u", sig);
		argv[i++] = sigstr;
	}
	argv[i++] = program;

#ifdef KILLALL_HAS_WAIT
	return _eval(argv, NULL, 0, NULL);
#else
	while (!_eval(argv, NULL, 0, NULL) && wait)
		sleep(1);
	return 0;
#endif
}

int killall_s(char *program, unsigned sig)
{
	return killall_w(program, sig, 0);
}

int killall(char *program)
{
	return killall_w(program, 0, 0);
}

void getsyspara(void)
{
	char productid[13];
	char fwver[12];

	get_fw_ver(productid, fwver);

	// its a ugle solution for Product ID
	if (strstr(productid, "WL500gx"))
		nvram_set("productid", "WL500g.Deluxe");
	else
		nvram_set("productid", trim_r(productid));
	nvram_set("firmver", trim_r(fwver));
}

void setenv_tz()
{
	static char TZ_env[64];

	snprintf(TZ_env, sizeof(TZ_env), "TZ=%s", nvram_safe_get("time_zone"));
	TZ_env[sizeof(TZ_env)-1] = '\0';
	putenv(TZ_env);
}

time_t update_tztime(int is_resettm)
{
	time_t now;
	struct tm gm, local;
	struct timezone tz;
	struct timeval tv = { 0 };

	/* Update kernel timezone and time */
	setenv_tz();
	time(&now);
	gmtime_r(&now, &gm);
	localtime_r(&now, &local);
	tz.tz_minuteswest = (mktime(&gm) - mktime(&local)) / 60;
	settimeofday(is_resettm ? &tv : NULL, &tz);

	return now;
}

/* This function is used to map nvram value from asus to Broadcom */
void convert_asus_values()
{	
	char macbuf[36];
	char ifnames[36];
	char sbuf[64];
	int i, num;
	char *list;

	list = malloc(2048);
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
                        nvram_set("wl_akm", "psk psk2");
                        nvram_set("wl0_akm", "psk psk2");
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
		nvram_set("wl0_nmcsidx", nvram_get("wl_nmcsidx"));
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
#ifdef __CONFIG_BCMWL5__
	nvram_set("wl0_mimo_preamble", nvram_safe_get("wl_mimo_preamble"));
	nvram_set("wl0_wme_apsd", nvram_safe_get("wl_wme_apsd"));
	nvram_set("wl0_vlan_prio_mode", nvram_safe_get("wl_vlan_prio_mode"));
	nvram_set("wl0_obss_coex", (nvram_match("wl_nbw", "40") && nvram_invmatch("wl_reg_mode", "off")) ? "1" : "0");
	nvram_set("wl0_wmf_bss_enable", nvram_safe_get("wl_wmf_bss_enable"));
#endif
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
#ifndef __CONFIG_BCMWL5__
		int wepidx=atoi(nvram_safe_get("wl0_key"));
		char wepname[16];
#endif

		sprintf(sbuf, "wl join \"%s\"", nvram_safe_get("wl0_ssid"));

#ifndef __CONFIG_BCMWL5__
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
#endif
		sprintf(sbuf, "%s imode bss", sbuf);

#ifndef __CONFIG_BCMWL5__
		if (nvram_match("wl_auth_mode", "psk"))
			sprintf(sbuf, "%s amode wpapsk", sbuf);
#else
		if (nvram_match("wl_auth_mode", "psk") && nvram_match("wl_wpa_mode", "1"))
			sprintf(sbuf, "%s amode wpapsk", sbuf);

		else if (nvram_match("wl_auth_mode", "psk") && nvram_match("wl_wpa_mode", "2"))
			sprintf(sbuf, "%s amode wpa2psk", sbuf);

		else if (nvram_match("wl_auth_mode", "wpa"))
			sprintf(sbuf, "%s amode wpa", sbuf);

		else if (nvram_match("wl_auth_mode", "wpa2"))
			sprintf(sbuf, "%s amode wpa2", sbuf);
#endif
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

		for (i=0;i<num;i++)
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

		for (i=0;i<num;i++)
		{
			sprintf(list, "%s %s", list, mac_conv("wl_maclist_x", i, macbuf));
			
		}

		//printf("mac list %s %x\n", list, num);

		nvram_set("wl0_maclist", list);
	}

	/* Power values
	 * 400mW = 26dBm, defaults 50mW = 17 dBm */
	//nvram_set("pa0maxpwr", "400");
	nvram_set("wl0_radio_power_x", nvram_safe_get("wl_radio_power_x"));

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

#if defined( __CONFIG_MADWIMAX__ ) || defined ( __CONFIG_MODEM__ )
	char tmp[100], prefix[] = "wanXXXXXXXXXX_";
	int unit;
	for (unit = 0; unit < MAX_NVPARSE; unit ++) 
	{
		snprintf(prefix, sizeof(prefix), "wan%d_", unit);
		nvram_unset(strcat_r(prefix, "dial_enabled", tmp));
		nvram_unset(strcat_r(prefix, "wimax_enabled", tmp));
		nvram_unset(strcat_r(prefix, "prepared", tmp));
		nvram_unset(strcat_r(prefix, "usb_device", tmp));
		nvram_unset(strcat_r(prefix, "usb_device_name", tmp));
	}
#endif

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
			ip_addr(nvram_safe_get("wan_ipaddr")) && 
			ip_addr(nvram_safe_get("wan_netmask")) ? 
				nvram_get("wan_netmask") : NULL);
		nvram_set("wan0_pppoe_gateway", nvram_get("wan_gateway"));
		
		/* current interface address (dhcp + firewall) */
		nvram_set("wanx_ipaddr", nvram_safe_get("wan_ipaddr"));
	}
#ifdef __CONFIG_MADWIMAX__
	else if (nvram_match("wan_proto", "wimax"))
	{
		nvram_set("wan0_wimax_ifname", "wmx0");
		//nvram_set("upnp_wan_proto", "dhcp");
		nvram_set("wan0_wimax_ipaddr", nvram_safe_get("wan_ipaddr"));
		nvram_set("wan0_wimax_netmask", 
			ip_addr(nvram_safe_get("wan_ipaddr")) && 
			ip_addr(nvram_safe_get("wan_netmask")) ? 
				nvram_get("wan_netmask") : NULL);
		nvram_set("wan0_wimax_gateway", nvram_get("wan_gateway"));
		nvram_set("wan0_wimax_ssid", nvram_safe_get("wan_wimax_ssid"));

		/* current interface address (dhcp + firewall) */
		nvram_set("wanx_ipaddr", nvram_safe_get("wan_ipaddr"));
	}
#endif
#ifdef __CONFIG_MODEM__
	else if (nvram_match("wan_proto", "usbmodem"))
	{
		nvram_set("wan0_pppoe_ifname", "ppp0");
		//nvram_set("upnp_wan_proto", "dhcp");
		nvram_set("wan0_modem_ipaddr", nvram_safe_get("wan_ipaddr"));
		nvram_set("wan0_modem_netmask", 
			ip_addr(nvram_safe_get("wan_ipaddr")) && 
			ip_addr(nvram_safe_get("wan_netmask")) ? 
				nvram_get("wan_netmask") : NULL);
		nvram_set("wan0_modem_gateway", nvram_get("wan_gateway"));
		/* current interface address (dhcp + firewall) */
		nvram_set("wanx_ipaddr", nvram_safe_get("wan_ipaddr"));
		nvram_set("wan0_dial_enabled", "1");
	}
#endif

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

	nvram_set("wan0_auth_x", nvram_safe_get("wan_auth_x"));
	nvram_set("wan0_auth_username", nvram_safe_get("wan_pppoe_username"));
	nvram_set("wan0_auth_passwd", nvram_safe_get("wan_pppoe_passwd"));

	convert_routes();

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
#if defined(MODEL_WL700G)
	/* force mounting (boot_local and wl-hdd) */
	nvram_set("usb_storage_device", "ide");
#endif
	nvram_set("usb_web_device", "");
	nvram_set("usb_audio_device", "");
	nvram_set("usb_webdriver_x", "");
#ifdef __CONFIG_MADWIMAX__
	nvram_set("usb_wimax_device", "");
#endif
	nvram_set("no_br", "0");

	if (nvram_invmatch("sp_battle_ips", "0"))
	{
		insmod("ip_nat_starcraft", NULL);
		insmod("ipt_NETMAP", NULL);
	}

#ifdef WEBSTRFILTER
	if (nvram_match("url_enable_x", "1")) {
		#ifdef LINUX26
		insmod("xt_webstr", NULL);
		#else
		insmod("ipt_webstr", NULL);
		#endif
	}
#endif

	if (nvram_invmatch("usb_smbcset_x", "utf8"))
	{
		char *cpname = nvram_safe_get("usb_smbcset_x");

		sprintf(sbuf, "nls_%s%s", (atoi(cpname) > 0) ? "cp" : "", cpname);
		insmod(sbuf, NULL);
	}

        //2005/09/22 insmod FTP module
        if (nvram_match("usb_ftpenable_x", "1") && atoi(nvram_get("usb_ftpport_x"))!=21)
        {
                char ports[32];

                sprintf(ports, "ports=21,%d", atoi(nvram_get("usb_ftpport_x")));
		#ifdef LINUX26
                insmod("nf_conntrack_ftp", ports, NULL);
                insmod("nf_nat_ftp", ports, NULL);
		#else
                insmod("ip_conntrack_ftp", ports, NULL);
                insmod("ip_nat_ftp", ports, NULL);
		#endif
        }
        else
        {
		#ifdef LINUX26
                insmod("nf_conntrack_ftp", NULL);
                insmod("nf_nat_ftp", NULL);
		#else
                insmod("ip_conntrack_ftp", NULL);
                insmod("ip_nat_ftp", NULL);
		#endif
        }

	if ((nvram_match("ssh_enable", "1") && nvram_invmatch("recent_ssh_enable", "0")) ||
	    (nvram_match("usb_ftpenable_x", "1") && nvram_invmatch("recent_ftp_enable", "0")))
	{
#if defined(LINUX26)
		insmod("xt_recent", NULL);
#else
		insmod("ipt_recent", NULL);
#endif
	}

#ifdef __CONFIG_IPV6__
	nvram_unset("lan_ipv6_addr");
	nvram_unset("wan0_ipv6_addr");
	nvram_unset("wan0_ipv6_dns");
	nvram_set("wan0_ipv6_router", nvram_safe_get("ipv6_wan_router"));
	nvram_set("wan0_ipv6_relay", nvram_match("ipv6_proto", "tun6rd") ?
	    nvram_safe_get("ipv6_6rd_router") :
	    nvram_safe_get("ipv6_sit_relay"));
	nvram_set("wan0_ipv6_ip4size", nvram_safe_get("ipv6_6rd_ip4size"));

	nvram_unset("wan_ipv6_addr_t");

	if (nvram_invmatch("ipv6_proto", ""))
	{
		char addrstr[INET6_ADDRSTRLEN];

		sprintf(addrstr, "%s/%s",
		    nvram_safe_get("ipv6_wan_addr"),
		    nvram_safe_get("ipv6_wan_netsize"));
		nvram_set("wan0_ipv6_addr", addrstr);

#if !defined(BROKEN_IPV6_CONNTRACK) && !defined(LINUX26)
		insmod("ip6_conntrack", NULL);
//		insmod("ip6t_state", NULL);
//		insmod("ip6t_TCPMSS", NULL);
#endif
	} else {
		fputs_ex("/proc/sys/net/ipv6/conf/all/disable_ipv6", "1");
		fputs_ex("/proc/sys/net/ipv6/conf/default/disable_ipv6", "1");
	}
#endif

#ifdef __CONFIG_EMF__
	/* Force IGMPv2 due EMF limitations */
	if (nvram_match("emf_enable", "1"))
	{
		fputs_ex("/proc/sys/net/ipv4/conf/all/force_igmp_version", "2");
		fputs_ex("/proc/sys/net/ipv4/conf/default/force_igmp_version", "2");
	}
#endif

#if defined(LINUX26) && defined(QOS)
	/* Kinda smart fast nat management */
	fputs_ex("/proc/sys/net/ipv4/netfilter/ip_conntrack_fastnat",
	    nvram_match("misc_fastnat_x", "0") || !nvram_match("wan_nat_x", "1") ||
#ifdef __CONFIG_MADWIMAX__
	    nvram_match("wan_proto", "wimax") ||
#endif
#ifdef __CONFIG_MODEM__
	    nvram_match("wan_proto", "usbmodem") ||
#endif
	    nvram_match("qos_enable_x", "1") ? "0" :
#ifdef WEBSTRFILTER
	    nvram_match("url_enable_x", "1") ? "2" :
#endif
	    "1");
#endif

	update_lan_status(1);

	free(list);
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
#ifdef __CONFIG_MODEM__
	else if (!strcmp(proto, "usbmodem")) nvram_set("wan_proto_t", "3G USB Modem");
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

  va_start(args, fmt);
  openlog(logheader, 0, 0);
  vsyslog(0, fmt, args);
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

int fputs_ex(char *name, char *value)
{
	FILE *fp;

	if ((fp = fopen(name, "r+")))
	{
		fputs(value, fp);
		fclose(fp);
	} else
		perror(name);

	return errno;
}

int router_totalram()
{
	struct sysinfo si;

	sysinfo(&si);
	return si.totalram;
}
