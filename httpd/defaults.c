/*
 * Router default NVRAM values
 *
 * Copyright 2001, Broadcom Corporation
 * All Rights Reserved.
 *
 * This is UNPUBLISHED PROPRIETARY SOURCE CODE of Broadcom Corporation;
 * the contents of this file may not be disclosed to third parties, copied or
 * duplicated in any form, in whole or in part, without the prior written
 * permission of Broadcom Corporation.
 *
 * $Id: defaults.c,v 1.1.1.1 2003/11/19 11:14:40 Cheni_Shen Exp $
 */

#include <epivers.h>
#include <bcmnvram.h>

struct nvram_tuple router_defaults[] = {
	/* OS parameters */
	{ "lan_ifname", "", 0 },		/* LAN interface name */
	{ "lan_ifnames", "", 0 },		/* Enslaved LAN interfaces */
	{ "lan_hwnames", "", 0 },		/* LAN driver names (e.g. et0) */
	{ "lan_hwaddr", "", 0 },		/* LAN interface MAC address */
	{ "wan_ifname", "", 0 },		/* WAN interface name */
	{ "wan_hwname", "", 0 },		/* WAN driver name (e.g. et1) */
	{ "wan_hwaddr", "", 0 },		/* WAN interface MAC address */
	{ "os_name", "", 0 },			/* OS name string */
	{ "os_version", EPI_VERSION_STR, 0 },	/* OS revision */

	/* Miscellaneous parameters */
	{ "timer_interval", "3600", 0 },	/* Timer interval in seconds */
	{ "ntp_server", "", 0 },		/* NTP server */
	{ "log_level", "0", 0 },		/* Bitmask 0:off 1:denied 2:accepted */
	{ "time_zone", "PST8PDT", 0 },		/* Time zone (GNU TZ format) */
	{ "upnp_enable", "0", 0 },		/* Start UPnP */
	{ "os_server", "", 0 },			/* URL for getting upgrades */
	{ "stats_server", "", 0 },		/* URL for posting stats */
	{ "console_loglevel", "1", 0 },		/* Kernel panics only */

	/* TCP/IP parameters */
	{ "lan_proto", "dhcp", 0 },		/* [static|dhcp] */
	{ "lan_ipaddr", "192.168.1.1", 0 },	/* LAN IP address */
	{ "lan_netmask", "255.255.255.0", 0 },	/* LAN netmask */
	{ "lan_stp", "1", 0 },			/* LAN spanning tree protocol */
	{ "wan_proto", "dhcp", 0 },		/* [static|dhcp|pppoe] */
	{ "wan_ipaddr", "0.0.0.0", 0 },		/* WAN IP address */
	{ "wan_netmask", "0.0.0.0", 0 },	/* WAN netmask */
	{ "wan_gateway", "0.0.0.0", 0 },	/* WAN gateway */
	{ "wan_dns", "", 0 },			/* x.x.x.x x.x.x.x ... */
	{ "wan_wins", "", 0 },			/* x.x.x.x x.x.x.x ... */
	{ "wan_hostname", "", 0 },		/* WAN hostname */
	{ "wan_domain", "", 0 },		/* WAN domain name */
	{ "wan_lease", "864000", 0 },		/* WAN lease time in seconds */
	{ "static_route", "", 0 },		/* Static routes (ipaddr:netmask:gateway:metric:ifname ...) */

	/* LAN filters */
	{ "filter_ip", "", 0 },			/* lan_ipaddr0-lan_ipaddr1 ... */
	{ "filter_tcp", "", 0 },		/* [lan_ipaddr|*]:lan_port0-lan_port1 */
	{ "filter_udp", "", 0 },		/* [lan_ipaddr|*]:lan_port0-lan_port1 */
	{ "filter_mac", "", 0 },		/* xx:xx:xx:xx:xx:xx ... */

	/* Port forwards */
	{ "forward_tcp", "", 0 },		/* wan_port0-wan_port1>lan_ipaddr:lan_port0-lan_port1 */
	{ "forward_udp", "", 0 },		/* wan_port0-wan_port1>lan_ipaddr:lan_port0-lan_port1 */
	{ "dmz_ipaddr", "", 0 },		/* x.x.x.x (equivalent to 0-60999>dmz_ipaddr:0-60999) */

	/* DHCP server parameters */
	{ "dhcp_start", "192.168.1.100", 0 },	/* First assignable DHCP address */
	{ "dhcp_end", "192.168.1.150", 0 },	/* Last assignable DHCP address */

	/* Web server parameters */
	{ "http_username", "", 0 },		/* Username */
	{ "http_passwd", "admin", 0 },		/* Password */
	{ "http_wanport", "", 0 },		/* WAN port to listen on */
	{ "http_lanport", "80", 0 },		/* LAN port to listen on */

	/* PPPoE parameters */
	{ "pppoe_ifname", "", 0 },		/* PPPoE enslaved interface */
	{ "pppoe_username", "", 0 },		/* PPP username */
	{ "pppoe_passwd", "", 0 },		/* PPP password */
	{ "pppoe_idletime", "60", 0 },		/* Dial on demand max idle time (seconds) */
	{ "pppoe_keepalive", "0", 0 },		/* Restore link automatically */
	{ "pppoe_demand", "0", 0 },		/* Dial on demand */
	{ "pppoe_mru", "1500", 0 },		/* Negotiate MRU to this value */
	{ "pppoe_mtu", "1500", 0 },		/* Negotiate MTU to the smaller of this value or the peer MRU */

	/* Wireless parameters */
	{ "wl_ssid", "Broadcom", 0 },		/* Service set ID (network name) */
	{ "wl_country", "USA", 0 },		/* Country (USA|Canada|Europe|Japan) */
	{ "wl_closed", "0", 0 },		/* Closed (hidden) network */
	{ "wl_wep", "off", 0 },			/* WEP data encryption (off|on|restricted) */
	{ "wl_auth", "0", 0 },			/* Authentication (0|1) */
	{ "wl_key", "1", 0 },			/* Current WEP key */
	{ "wl_key1", "", 0 },			/* 5/13 char ASCII or 10/26 char hex */
	{ "wl_key2", "", 0 },			/* 5/13 char ASCII or 10/26 char hex */
	{ "wl_key3", "", 0 },			/* 5/13 char ASCII or 10/26 char hex */
	{ "wl_key4", "", 0 },			/* 5/13 char ASCII or 10/26 char hex */
	{ "wl_mac", "", 0 },			/* xx:xx:xx:xx:xx:xx ... */
	{ "d11b_channel", "11", 0 },		/* Channel number (1-14 depending on country) */
	{ "d11b_rate", "0", 0 },		/* Rate (bps, 0 for auto) */
	{ "d11b_rateset", "default", 0 },	/* "default" (1/2/5.5/11) or "min" (1/2) */
	{ "d11b_frag", "1536", 0 },		/* Fragmentation threshold */
	{ "d11b_rts", "1536", 0 },		/* RTS threshold */
	{ "d11b_dtim", "3", 0 },		/* DTIM period */
	{ "d11b_bcn", "100", 0 },		/* Beacon interval */

	/* Restore defaults */
	{ "restore_defaults", "0", 0 },		/* Set to 0 to not restore defaults on boot */

	{ 0, 0, 0 }
};
