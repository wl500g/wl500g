/*
 * Router rc control script
 *
 * Copyright 2004, Broadcom Corporation
 * All Rights Reserved.
 * 
 * THIS SOFTWARE IS OFFERED "AS IS", AND BROADCOM GRANTS NO WARRANTIES OF ANY
 * KIND, EXPRESS OR IMPLIED, BY STATUTE, COMMUNICATION OR OTHERWISE. BROADCOM
 * SPECIFICALLY DISCLAIMS ANY IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS
 * FOR A SPECIFIC PURPOSE OR NONINFRINGEMENT CONCERNING THIS SOFTWARE.
 *
 * $Id$
 */

#ifndef _rc_h_
#define _rc_h_

#include <bcmconfig.h>
#include <netinet/in.h>

#define IFUP (IFF_UP | IFF_RUNNING | IFF_BROADCAST | IFF_MULTICAST)

#define sin_addr(s) (((struct sockaddr_in *)(s))->sin_addr)

/* common */
in_addr_t inet_addr_(const char *cp);
char *trim_r(char *str);
void logmessage(char *logheader, char *fmt, ...);
void wanmessage(char *fmt, ...);
int kill_pidfile_s(char *pidfile, int sig);
void update_lan_status(int isup);
void update_wan_status(int isup);
char *pppstatus(char *buf);
void convert_asus_values();
char *mac_conv(char *mac_name, int idx, char *buf);

/* dns */
void dns_query(char *svrip, char *hostname, char qtype, char *ret);

/* udhcpc scripts */
int udhcpc_main(int argc, char **argv);

/* ppp scripts */
int ipup_main(int argc, char **argv);
int ipdown_main(int argc, char **argv);
int ppp_ifunit(char *ifname);

/* http functions */
int http_get(const char *server, char *buf, size_t count, off_t offset);
int http_post(const char *server, char *buf, size_t count);
int http_stats(const char *url);
int http_check(const char *server, char *buf, size_t count, off_t offset);

/* init */
int console_init(void);
pid_t run_shell(int timeout, int nowait);
void signal_init(void);
void fatal_signal(int sig);
void preshutdown_system(void);

/* interface */
int ifconfig(char *ifname, int flags, char *addr, char *netmask);
int route_add(char *name, int metric, char *dst, char *gateway, char *genmask);
int route_del(char *name, int metric, char *dst, char *gateway, char *genmask);
void config_loopback(void);
int config_vlan(void);
int start_vlan(void);
int stop_vlan(void);

/* network */
void start_lan(void);
void stop_lan(void);
void start_wan(void);
void stop_wan(void);
void stop_wan2(void);
void wan_up(char *ifname);
void wan_down(char *ifname);
void lan_up_ex(char *lan_ifname);
void lan_down_ex(char *lan_ifname);
int hotplug_net(void);
int wan_ifunit(char *ifname);
int wan_primary_ifunit(void);
int start_bpalogin(void);
int stop_bpalogin(void);
void start_qos(char *wan_ipaddr);
void setup_ethernet(char *wan_if);

/* services */
int start_httpd(void);
int start_dhcpd(void);
int stop_dhcpd(void);
int stop_snmpd(void);
int start_pppd(char *prefix);
void start_pppoe_relay(char *wan_if);
int start_dns(void);
int stop_dns(void);
int start_upnp(void);
int stop_upnp(void);
int start_ddns(void);
int stop_ddns(void);
int start_ntpc(void);
int stop_ntpc(void);
int start_nas(char *type);
int stop_nas(void);
int start_usb(void);
int stop_usb(void);
int start_rcamd(void);
int stop_rcamd(void);
int start_audio(void);
int stop_audio(void);
int start_services(void);
int stop_services(void);
int start_logger(void);
int stop_logger(void);
int start_misc(void);
int stop_misc(void);
int hotplug_usb(void);
int hotplug_usb_mass(char *product);
int hotplug_usb_webcam(char *product, int webflag);
int hotplug_usb_audio(char *product);
int service_handle(void);
int mkdir_if_none(char *dir);

/* firewall */
#ifdef __CONFIG_NETCONF__
int start_firewall(void);
int start_firewall2(char *ifname);
int start_firewall_ex(char *wan_if, char *wan_ip, char *lan_if, char *lan_ip);
#else
#define start_firewall() do {} while (0)
#define stop_firewall() do {} while (0)
#define start_firewall2(ifname) do {} while (0)
#define stop_firewall2(ifname) do {} while (0)
#endif

/* routes */
int preset_wan_routes(char *ifname);

#endif /* _rc_h_ */
