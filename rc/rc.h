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
in_addr_t ip_addr(const char *cp);
void ip2class(char *lan_ip, char *netmask, char *buf);
#ifdef __CONFIG_IPV6__
int ipv6_addr(const char *str, struct in6_addr *addr6);
int ipv6_network(struct in6_addr *addr6, int netsize);
int ipv6_host(struct in6_addr *addr6, int netsize);
int ipv6_map6rd(struct in6_addr *addr6, int netsize, struct in_addr *addr4, int ip4size);
#endif
char *trim_r(char *str);
void logmessage(char *logheader, char *fmt, ...);
void wanmessage(char *fmt, ...);
int kill_pidfile_s(char *pidfile, int sig);
void update_lan_status(int isup);
void update_wan_status(int isup);
char *pppstatus(char *buf);
void convert_asus_values();
void convert_country();
void convert_routes();
char *mac_conv(char *mac_name, int idx, char *buf);
int fputs_ex(char *name, char *value);
int insmod(char *module, ...);
int rmmod(char *module);
int killall(char *program, int sig);
void setenv_tz();
time_t update_tztime(int is_resettm);

int wsrom_main(char *devname, unsigned int pos, unsigned short val);
int rsrom_main(char *devname, unsigned int pos, int pflag);

/* udhcpc scripts */
int udhcpc_main(int argc, char **argv);
int start_udhcpc(char *wan_ifname, int unit);
int udhcpc_ex_main(int argc, char **argv);
#ifdef __CONFIG_IPV6__
int dhcp6c_main(int argc, char **argv);
int start_dhcp6c(char *wan_ifname);
void stop_dhcp6c(void);
#endif

/* ppp scripts */
int authup_main(int argc, char **argv);
int authdown_main(int argc, char **argv);
int ipup_main(int argc, char **argv);
int ipdown_main(int argc, char **argv);
#ifdef __CONFIG_IPV6__
int ip6up_main(int argc, char **argv);
int ip6down_main(int argc, char **argv);
#endif
int ppp_ifunit(char *ifname);

/* http functions */
int http_get(const char *server, char *buf, size_t count, off_t offset);
int http_post(const char *server, char *buf, size_t count);
int http_stats(const char *url);
int http_check(const char *server, char *buf, size_t count, off_t offset);
int proc_check_pid(const char *pidfile);

/* init */
int console_init(void);
pid_t run_shell(int timeout, int nowait);
void signal_init(void);
void preshutdown_system(void);
void child_reap(int sig);

/* interface */
int ifconfig(char *ifname, int flags, char *addr, char *netmask, char *peer);
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
void stop_wan(char *ifname);
void wan_up(char *ifname);
void wan_down(char *ifname);
#ifdef __CONFIG_IPV6__
void wan6_up(char *ifname, int unit);
void wan6_down(char *ifname, int unit);
#endif
void lan_up_ex(char *lan_ifname);
void lan_down_ex(char *lan_ifname);
int wan_prefix(char *ifname, char *prefix);

int hotplug_net(void);
int wan_ifunit(char *ifname);
int wan_primary_ifunit(void);
int start_bpalogin(void);
int stop_bpalogin(void);
void start_qos(char *wan_ipaddr);
void setup_ethernet(char *wan_if);
int write_mac(char *devname, char *mac);
int bpa_connect_main(int argc, char **argv);
int bpa_disconnect_main(int argc, char **argv);
void stop_igmpproxy(void);
int update_resolvconf(char *ifname, int metric, int up);
#ifdef __CONFIG_BCMWL5__
extern void start_wl(void);
#else
static inline void start_wl(void) { };
#endif

/* services */
int stop_service_main();
int start_httpd(void);
int start_dhcpd(void);
int stop_dhcpd(void);
#ifdef __CONFIG_IPV6__
int start_radvd(void);
int stop_radvd(void);
#endif
int stop_snmpd(void);
int start_pppd(char *prefix);
void start_pppoe_relay(char *wan_if);
int start_dns(void);
int stop_dns(void);
int start_upnp(void);
int stop_upnp(void);
int start_ddns(int forced);
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
int start_lltd(void);
int start_services(void);
int stop_services(void);
int start_logger(void);
int stop_logger(void);
int start_misc(void);
int stop_misc(void);
int hotplug_usb(void);
int hotplug_usb_mass(char *product);
int hotplug_usb_webcam(char *product);
int remove_usb_webcam(char *product);
int hotplug_usb_audio(char *product);
int remove_storage_main(int scsi_host_no);
int restart_ftpd();
int ddns_updated_main();
int sendalarm_main(int argc, char *argv[]);
int service_handle(void);
int mkdir_if_none(char *dir);
void diag_PaN(void);
int wlan_update();

/* auth */
#ifdef __CONFIG_EAPOL__
int start_wpa_supplicant();
int stop_wpa_supplicant();
#endif
#ifdef __CONFIG_TELENET__
int start_lanauth();
int stop_lanauth();
#endif

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

/* watchdog */
int refresh_wave(void);
int poweron_main(int argc, char *argv[]);
int watchdog_main();

#ifdef __CONFIG_MADWIMAX__
int start_wimax(char *prefix);
int stop_wimax(char *prefix);
int madwimax_check(char *prefix);
int madwimax_main(int argc, char **argv);
int wimax_ifunit(char *ifname);
int hotplug_check_wimax(char *interface, char *product, char *prefix);
#endif

#ifdef __CONFIG_MODEM__
int start_modem_dial(char *prefix);
int stop_modem_dial(char *prefix);
int usb_modem_check(char * prefix);
int hotplug_check_modem(char *interface, char *product, char *device, char *prefix);
int lsmodem_main(int argc, char **argv);
#endif

#if defined(__CONFIG_MADWIMAX__) || defined(__CONFIG_MODEM__)
void hotplug_network_device(char *interface, char *action, char *product, char *device);
void hotplug_usb_modeswitch(char *interface, char *action, char *product, char *device);
#endif

#endif /* _rc_h_ */
