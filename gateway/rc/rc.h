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

#if defined(__UCLIBC__)
#include <crypt.h>
#endif

#include <bcmconfig.h>
#include <bcmnvram.h>
#include <shutils.h>
#include <netinet/in.h>

#define IFUP (IFF_UP | IFF_RUNNING | IFF_BROADCAST | IFF_MULTICAST)

#define sin_addr(s) (((struct sockaddr_in *)(s))->sin_addr)

#if !defined(__UCLIBC_MAJOR__) \
 || __UCLIBC_MAJOR__ > 0 \
 || __UCLIBC_MINOR__ > 9 \
 || (__UCLIBC_MINOR__ == 9 && __UCLIBC_SUBLEVEL__ >= 32)
    #define RC_SEMAPHORE_ENABLED
#endif

#define DEV_RTC	"/dev/misc/rtc"

enum {
	WAN_STATIC = 0,
	WAN_DHCP,
	WAN_PPPOE,
	WAN_PPTP,
	WAN_L2TP,
	WAN_BIGPOND,
	WAN_WIMAX,
	WAN_USBMODEM,
	WAN_USBNET,
};

#define WAN_PREFIX_SZ	sizeof("wanXXXXXXXXXX_")

/* common */
in_addr_t ip_addr(const char *cp);
void ip2class(const char *lan_ip, const char *netmask, char *buf);
#ifdef __CONFIG_IPV6__
int ipv6_addr(const char *str, struct in6_addr *addr6);
int ipv6_network(struct in6_addr *addr6, int netsize);
int ipv6_host(struct in6_addr *addr6, int netsize);
int ipv6_map6rd(struct in6_addr *addr6, int netsize, const struct in_addr *addr4, int ip4size);
#endif
char *trim_r(char *str);
int router_totalram();
void logmessage(const char *logheader, const char *fmt, ...);
void wanmessage(const char *fmt, ...);
void update_lan_status(int isup);
void update_wan_status(int isup);
char *pppstatus(char *buf);
void convert_asus_values();
void convert_country();
void convert_routes(int unit);
char *mac_conv(const char *mac_name, int idx, char *buf);
int fputs_ex(const char *name, const char *value);
int fgets_ex(const char *name, char *value, int size);
int insmod(const char *module, ...);
int insmod_cond(const char *module, ...);
int rmmod(const char *module);
int killall_w(const char *program, unsigned sig, int timeout);
int killall_s(const char *program, unsigned sig);
int killall(const char *program);
int killall_tk(const char *program);
void setenv_tz();
time_t update_tztime(int is_resettm);
int proc_check_pid(const char *pidfile);
int wsrom_main(const char *devname, unsigned int pos, unsigned short val);
int rsrom_main(const char *devname, unsigned int pos, int pflag);

/* dhcp/zcip scripts */
int udhcpc_main(int argc, char **argv);
int start_dhcpc(const char *wan_ifname, int unit);
int udhcpc_ex_main(int argc, char **argv);
int zcip_main(int argc, char **argv);
#ifdef __CONFIG_IPV6__
int dhcp6c_main(int argc, char **argv);
int start_dhcp6c(const char *wan_ifname);
void stop_dhcp6c(void);
#endif

/* ppp scripts */
#ifdef PPPD_AUTH_UNUSED
int authup_main(int argc, char **argv);
int authdown_main(int argc, char **argv);
int authfail_main(int argc, char **argv);
#endif
int ipup_main(int argc, char **argv);
int ipdown_main(int argc, char **argv);
#ifdef __CONFIG_IPV6__
int ip6up_main(int argc, char **argv);
int ip6down_main(int argc, char **argv);
#endif
int ppp_ifunit(const char *ifname);

/* init */
pid_t run_shell(int timeout, int nowait);
void signal_init(void);
void preshutdown_system(void);
void sysinit(void);
void child_reap(int sig);

/* interface */
int _ifconfig(const char *ifname, int flags, const char *addr, const char *netmask, const char *peer);
int ifconfig(const char *ifname, int flags, const char *addr, const char *netmask);
int route_add(const char *name, int metric, const char *dst, const char *gateway, const char *genmask);
int route_del(const char *name, int metric, const char *dst, const char *gateway, const char *genmask);
void config_loopback(void);
int config_vlan(void);
int start_vlan(void);
int stop_vlan(void);

/* network */
void start_lan(void);
void stop_lan(void);
void start_wan(void);
void stop_wan(void);
void start_wan_unit(int unit);
void stop_wan_unit(int unit);
void wan_up(const char *ifname);
void wan_down(const char *ifname);
#ifdef __CONFIG_IPV6__
void wan6_up(const char *ifname, int unit);
void wan6_down(const char *ifname, int unit);
#endif
void lan_up_ex(const char *lan_ifname);
void lan_down_ex(const char *lan_ifname);
int wan_prefix(const char *ifname, char *prefix);
int wanx_prefix(const char *ifname, char *prefix);
int wans_prefix(const char *ifname, char *prefix, char *xprefix);
int wan_valid(const char *ifname);
int _wan_proto(const char *prefix, char *buffer);
int wan_proto(const char *prefix);

int hotplug_net(void);
int wan_primary_ifunit(void);
int start_bpalogin(void);
int stop_bpalogin(void);
void start_qos(char *wan_ipaddr);
void setup_ethernet(const char *wan_if);
int ethernet_port(const char *wan_if);
int write_mac(const char *devname, const char *mac);
int bpa_connect_main(int argc, char **argv);
int bpa_disconnect_main(int argc, char **argv);
void stop_igmpproxy(void);
int update_resolvconf(const char *ifname, int metric, int up);
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
#ifdef __CONFIG_RADVD__
int start_radvd(void);
int stop_radvd(void);
#endif
int stop_snmpd(void);
int start_pppd(const char *prefix);
int start_pppoe_relay(char *wan_if);
int start_dns(void);
int stop_dns(void);
int start_upnp(void);
int stop_upnp(void);
int start_ddns(int type);
int stop_ddns(void);
int start_ntpc(void);
int stop_ntpc(void);
int start_nas(const char *type);
int stop_nas(void);
int start_usb(void);
int stop_usb(void);
int start_rcamd(void);
int stop_rcamd(void);
int start_lltd(void);
int start_services(void);
int stop_services(void);
int start_logger(void);
int stop_logger(void);
int start_misc(void);
int stop_misc(void);
int hotplug_usb(void);
int hotplug_usb_mass(const char *product);
int hotplug_usb_webcam(const char *product);
int remove_usb_webcam(const char *product);
int remove_usb_mass(const char *product, int scsi_host_no);
int restart_ftpd();
int restart_smbd();
int ddns_updated_main();
int sendalarm_main(int argc, char *argv[]);
int service_handle(void);
size_t fappend(const char *name, FILE *f);
int mkdir_if_none(const char *dir);
void diag_PaN(void);
int wlan_update();

/* auth */
int start_auth(const char *prefix, int wan_up);
int stop_auth(const char *prefix, int wan_down);
#ifdef __CONFIG_EAPOL__
int wpacli_main(int argc, char **argv);
#endif

/* firewall */
#ifdef __CONFIG_NETCONF__
int start_firewall(void);
int start_firewall2(const char *ifname);
int start_firewall_ex(const char *wan_if, const char *wan_ip, const char *lan_if, const char *lan_ip);
#else
#define start_firewall() do {} while (0)
#define stop_firewall() do {} while (0)
#define start_firewall2(ifname) do {} while (0)
#define stop_firewall2(ifname) do {} while (0)
#endif

/* routes */
int preset_wan_routes(const char *ifname);

/* watchdog */
int refresh_wave(void);
int poweron_main(int argc, char *argv[]);
int watchdog_main();

#ifdef __CONFIG_MADWIMAX__
int start_wimax(const char *prefix);
int stop_wimax(const char *prefix);
int madwimax_check(const char *prefix);
int madwimax_main(int argc, char **argv);
int wimax_ifunit(const char *ifname);
int hotplug_check_wimax(const char *interface, const char *product, const char *prefix);
#endif

#ifdef __CONFIG_MODEM__
int start_modem_dial(const char *prefix);
int stop_modem_dial(const char *prefix);
int usb_modem_check(const char *prefix);
int hotplug_check_modem(const char *interface, const char *product, const char *device, const char *prefix);
int lsmodem_main(int argc, char **argv);
void modem_load_drivers();
#endif

#ifdef __CONFIG_USBNET__
int  hotplug_usbnet_check(const char *interface, const char *product, const char *device, const char *prefix);
void usbnet_check_and_act(char *ifname, char *action);
void usbnet_load_drivers(const char *prefix);

void usbnet_connect(const char *prefix, int unit, const char *ifname);
void usbnet_disconnect(const char *prefix, int unit, const char *ifname);
#endif

#if defined(__CONFIG_MADWIMAX__) || defined(__CONFIG_MODEM__)
void hotplug_network_device(const char *interface, const char *action, const char *product, const char *device);
void hotplug_usb_modeswitch(const char *interface, const char *action, const char *product, const char *device);
int usb_communication_device_processcheck(int wait_flag);

void hotplug_sem_open();
void hotplug_sem_close();
void hotplug_sem_lock();
int  hotplug_sem_trylock();
void hotplug_sem_unlock();
#endif

#endif /* _rc_h_ */
