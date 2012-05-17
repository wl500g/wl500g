/*
 * Miscellaneous services
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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>
#include <errno.h>
#include <arpa/inet.h>

#include "rc.h"

#ifndef ASUS_EXT
int
start_dhcpd(void)
{
	FILE *fp;
	char name[100];

	if (nvram_match("router_disable", "1") || nvram_invmatch("lan_proto", "dhcp"))
		return 0;

	dprintf("%s %s %s %s\n",
		nvram_safe_get("lan_ifname"),
		nvram_safe_get("dhcp_start"),
		nvram_safe_get("dhcp_end"),
		nvram_safe_get("lan_lease"));

	/* Touch leases file */
	if (!(fp = fopen("/tmp/udhcpd.leases", "a"))) {
		perror("/tmp/udhcpd.leases");
		return errno;
	}
	fclose(fp);

	/* Write configuration file based on current information */
	if (!(fp = fopen("/tmp/udhcpd.conf", "w"))) {
		perror("/tmp/udhcpd.conf");
		return errno;
	}
	fprintf(fp, "pidfile /var/run/udhcpd.pid\n");
	fprintf(fp, "start %s\n", nvram_safe_get("dhcp_start"));
	fprintf(fp, "end %s\n", nvram_safe_get("dhcp_end"));
	fprintf(fp, "interface %s\n", nvram_safe_get("lan_ifname"));
	fprintf(fp, "remaining yes\n");
	fprintf(fp, "lease_file /tmp/udhcpd.leases\n");
	fprintf(fp, "option subnet %s\n", nvram_safe_get("lan_netmask"));
	fprintf(fp, "option router %s\n", nvram_safe_get("lan_ipaddr"));
	fprintf(fp, "option dns %s\n", nvram_safe_get("lan_ipaddr"));
	fprintf(fp, "option lease %s\n", nvram_safe_get("lan_lease"));
	snprintf(name, sizeof(name), "%s_wins", nvram_safe_get("dhcp_wins"));
	if (nvram_invmatch(name, ""))
		fprintf(fp, "option wins %s\n", nvram_get(name));
	snprintf(name, sizeof(name), "%s_domain", nvram_safe_get("dhcp_domain"));
	if (nvram_invmatch(name, ""))
		fprintf(fp, "option domain %s\n", nvram_get(name));
	fclose(fp);

	eval("udhcpd", "/tmp/udhcpd.conf");

	dprintf("done\n");
	return 0;
}

int
stop_dhcpd(void)
{
	int ret;

/*
* Process udhcpd handles two signals - SIGTERM and SIGUSR1
*
*  - SIGUSR1 saves all leases in /tmp/udhcpd.leases
*  - SIGTERM causes the process to be killed
*
* The SIGUSR1+SIGTERM behavior is what we like so that all current client
* leases will be honorred when the dhcpd restarts and all clients can extend
* their leases and continue their current IP addresses. Otherwise clients
* would get NAK'd when they try to extend/rebind their leases and they 
* would have to release current IP and to request a new one which causes 
* a no-IP gap in between.
*/
	killall_s("udhcpd", SIGUSR1);
	usleep(10000);
	ret = killall("udhcpd");

	dprintf("done\n");
	return ret;
}

int
start_dns(void)
{
	int ret;
	FILE *fp;
	
	if (nvram_match("router_disable", "1"))
		return 0;

	/* Create resolv.conf with empty nameserver list */
	if (!(fp = fopen("/tmp/resolv.conf", "w"))) {
		perror("/tmp/resolv.conf");
		return errno;
	}
	fclose(fp);

	/* launch dns relay */
	ret = eval("dnsmasq", "-h",
		   "-i", nvram_safe_get("lan_ifname"),
		   "-r", "/tmp/resolv.conf");

	dprintf("done\n");
	return ret;
}	

int
stop_dns(void)
{
	int ret = killall("dnsmasq");
	
	/* Remove resolv.conf */
	unlink("/tmp/resolv.conf");

	dprintf("done\n");
	return ret;
}	
#endif /* !ASUS_EXT */

static int start_telnetd(void)
{
	int ret;
	
	/* telnet is enabled by default */
	if (nvram_invmatch("telnet_enable", "1"))
		return 0;

	char *telnetd_argv[] = {"telnetd", NULL};
	pid_t pid;

	ret = _eval(telnetd_argv, NULL, 0, &pid);

	dprintf("done\n");
	return ret;
}

static int stop_telnetd(void)
{
	int ret = killall("telnetd");

	dprintf("done\n");
	return ret;
}

static int start_dropbear(void)
{
	char *dropbear_argv[] = {"dropbearstart", NULL, NULL, NULL, NULL, NULL};
	int i = 1;

	if (nvram_match("ssh_enable", "0"))
		return 0;

	if (nvram_match("ssh_password_logins", "0")) {
		dropbear_argv[i++] = "-s";
	}

	if (nvram_invmatch("ssh_port", "") && nvram_invmatch("ssh_port", "22"))
	{
		dropbear_argv[i++] = "-p";
		dropbear_argv[i++] = nvram_safe_get("ssh_port");
	}

#ifdef __CONFIG_IPV6__
	if (!nvram_invmatch("ipv6_proto", ""))
		dropbear_argv[i++] = "-4";
#endif

	int ret = _eval(dropbear_argv, NULL, 0, NULL);

	dprintf("done\n");
	return ret;
}

static int stop_dropbear(void)
{
	int ret = killall("dropbear");

	dprintf("done\n");
	return ret;
}

static int start_snmpd(void)
{
	const char *snmpd_conf = "/tmp/snmpd.conf";
	int ret;
	FILE *fp;
	
	if (!nvram_match("snmp_enable", "1"))
		return 0;
		
	/* create snmpd.conf  */
	if ((fp = fopen(snmpd_conf, "w")) == NULL) {
		perror(snmpd_conf);
		return 1;
	}

	fprintf(fp,
		"# automagically generated\n"
		"community %s\n"
		"syscontact %s\n"
		"syslocation %s\n",
		nvram_safe_default_get("snmp_community"),
		nvram_safe_default_get("snmp_contact"),
		nvram_safe_default_get("snmp_location"));
	fclose(fp);
	
	ret = eval("snmpd", "-c", snmpd_conf);

	dprintf("done\n");
	return ret;
}

int stop_snmpd(void)
{
	int ret = killall("snmpd");

	dprintf("done\n");
	return ret;
}

int start_httpd(void)
{
	int ret;

	chdir("/www");
#ifdef ASUS_EXT
	if (nvram_invmatch("router_disable", "1"))
		ret = eval("httpd", nvram_safe_get("wan0_ifname"));
	else
#endif
	ret = eval("httpd");

	chdir("/");

	dprintf("done\n");
	return ret;
}

static int stop_httpd(void)
{
	int ret = killall("httpd");

	dprintf("done\n");
	return ret;
}

int start_upnp(void)
{
	const char *wan_ifname;
	int ret;
	char var[100], prefix[sizeof("wanXXXXXXXXXX_")];
#ifdef __CONFIG_MINIUPNPD__
	FILE *fp;
	const char *lan_addr, *lan_mask, *lan_url;
	char lan_class[32];
	uint8_t lan_mac[16];
#endif

	if (!nvram_invmatch("upnp_enable", "0") || nvram_match("router_disable", "1"))
		return 0;

#ifdef __CONFIG_MINIUPNPD__
	ret = killall_s("miniupnpd", SIGUSR1);
#else
	ret = killall_s("upnp", SIGUSR1);
#endif
	if (ret != 0)
	{
		snprintf(prefix, sizeof(prefix), "wan%d_", wan_primary_ifunit());
		switch (_wan_proto(prefix, var)) {
		case WAN_PPPOE:
		case WAN_PPTP:
		case WAN_L2TP:
			wan_ifname = nvram_safe_get(strcat_r(prefix, "pppoe_ifname", var));
			break;
#ifdef __CONFIG_MADWIMAX__
		case WAN_WIMAX:
			wan_ifname = nvram_safe_get(strcat_r(prefix, "wimax_ifname", var));
			break;
#endif
		default:
			wan_ifname = nvram_safe_get(strcat_r(prefix, "ifname", var));
			break;
		}

#ifdef __CONFIG_MINIUPNPD__
		lan_addr = nvram_safe_get("lan_ipaddr");
		lan_mask = nvram_safe_get("lan_netmask");
		ip2class(lan_addr, lan_mask, lan_class);
		memset(lan_mac, 0, sizeof(lan_mac));
		ether_atoe(nvram_safe_get("lan_hwaddr"), lan_mac);

		lan_url = lan_addr;
		ret = nvram_get_int("http_lanport");
		if (ret && ret != 80) {
			sprintf(var, "%s:%d", lan_addr, ret);
			lan_url = var;
		}

		/* Touch leases file */
		if (!(fp = fopen("/tmp/upnp.leases", "a"))) {
			perror("/tmp/upnp.leases");
			return errno;
		}
		fclose(fp);

		/* Write configuration file */
		if (!(fp = fopen("/etc/miniupnpd.conf", "w"))) {
			perror("/etc/miniupnpd.conf");
			return errno;
		}

		/* General settings */
		fprintf(fp, "# automagically generated\n"
			"uuid=%02x%02x%02x%02x-%02x%02x-%02x%02x-%02x%02x-%02x%02x00000000\n"
                        "model_number=%s\n"
                        "presentation_url=http://%s/\n"
			"ext_ifname=%s\n"
			"listening_ip=%s/%s\n"
			"listening_ip=127.0.0.1/8\n"
			"port=0\n"
			"enable_upnp=%s\n"
			"enable_natpmp=%s\n"
			"lease_file=/tmp/upnp.leases\n"
			"system_uptime=yes\n",
			lan_mac[0], lan_mac[1], lan_mac[2], lan_mac[3], lan_mac[4], lan_mac[5],
			lan_mac[0], lan_mac[1], lan_mac[2], lan_mac[3], lan_mac[4], lan_mac[5],
			nvram_safe_get("productid"),
			lan_url,
			wan_ifname,
			lan_addr, lan_mask,
			nvram_match("upnp_proto", "2") ? "no" : "yes",
			nvram_match("upnp_proto", "1") ? "no" : "yes");

		/* Time-dependent settings */
		fprintf(fp,
			"notify_interval=%u\n"
			"clean_ruleset_interval=%u\n",
			nvram_get_int("upnp_notify") ? : 60,
			nvram_get_int("upnp_clean_ruleset") ? : 600);

		/* Security settings */
		fprintf(fp,
			"secure_mode=%s\n"
			"allow %u-65535 %s %u-65535\n"
			"deny 0-65535 0.0.0.0/0 0-65535\n",
			nvram_get_int("upnp_secure_mode") ? "yes" : "no",
			nvram_get_int("upnp_min_port") ? : 1024,
			lan_class,
			nvram_get_int("upnp_min_port") ? : 1024);

		fclose(fp);

		ret = eval("miniupnpd");
#else
		ret = eval("upnp", "-D",
			   "-L", nvram_safe_get("lan_ifname"),
			   "-W", wan_ifname);
#endif
	}
	dprintf("done\n");
	return ret;
}

int
stop_upnp(void)
{
	int ret = 0;

	if (nvram_invmatch("upnp_enable", "0"))
#ifdef __CONFIG_MINIUPNPD__
		ret = killall_w("miniupnpd", 0, 1);
#else
		ret = killall_w("upnp", 0, 1);
#endif

	dprintf("done\n");
	return ret;
}

int start_nas(const char *type)
{
	if ((nvram_match("wl0_radio", "0")) || (nvram_match("security_mode", "disabled")))
                return 1;

#ifdef __CONFIG_BCMWL5__
	setenv("UDP_BIND_IP", "127.0.0.1", 1);
        eval("eapd");
        unsetenv("UDP_BIND_IP");
        usleep(100000);
        eval("nas");
#else /* __CONFIG_BCMWL5__ */
	char cfgfile[64];
	char pidfile[64];
	pid_t pid;

	if (!type || !*type)
		type = "lan";

	snprintf(cfgfile, sizeof(cfgfile), "/tmp/nas.%s.conf", type);
	snprintf(pidfile, sizeof(pidfile), "/tmp/nas.%s.pid", type);
	{
		char *nas_argv[] = {"nas", cfgfile, pidfile, type, NULL};

		_eval(nas_argv, NULL, 0, &pid);
	}
#endif /* !__CONFIG_BCMWL5__ */
	dprintf("done\n");
	return 0;
}

int
stop_nas(void)
{
	int ret = killall("nas");

#ifdef __CONFIG_BCMWL5__
        killall("eapd");
#endif
	dprintf("done\n");
	return ret;
}

int
start_ntpc(void)
{
	pid_t pid = 0;
	char *server0 = nvram_safe_get("ntp_server0"), *server1;
	char *ntp_argv[] = {"ntpd", "-qt", "-p", server0, NULL, NULL, NULL};
	FILE *fp;

	if (strlen(server0) > 0) {
		server1 = nvram_safe_get("ntp_server1");
		if (strlen(server1) > 0) {
			ntp_argv[4] = "-p";
			ntp_argv[5] = server1;
		}
		_eval(ntp_argv, NULL, 0, &pid);

		/* write pid */
		if (pid && (fp = fopen("/var/run/ntpc.pid", "w")) != NULL)
		{
			fprintf(fp, "%d", pid);
			fclose(fp);
		}
	}

	return pid;
}

int
stop_ntpc(void)
{
	int ret = kill_pidfile("/var/run/ntpc.pid");
	if (ret == 0)
		unlink("/var/run/ntpc.pid");

	return ret;
}

int
start_services(void)
{
#ifdef ASUS_EXT
	start_logger();
#endif
	start_nas("lan");
	start_wl(); // Start WLAN
	start_telnetd();
	start_dropbear();
	start_httpd();
	start_dns();
	start_dhcpd();
#ifdef __CONFIG_RADVD__
	start_radvd();
#endif
	start_snmpd();
	start_upnp();
	start_lltd();
#ifdef ASUS_EXT
	start_usb();
#endif

	dprintf("done\n");
	return 0;
}

int
stop_services(void)
{
	preshutdown_system();

#ifdef ASUS_EXT
	stop_usb();
#endif
	stop_nas();
	stop_upnp();
	stop_snmpd();
#ifdef __CONFIG_RADVD__
	stop_radvd();
#endif
#ifdef __CONFIG_IPV6__
	stop_dhcp6c();
#endif
	stop_dhcpd();
	stop_dns();
	stop_httpd();
	stop_dropbear();
	stop_telnetd();
#ifdef ASUS_EXT
	stop_logger();
#endif
	dprintf("done\n");
	return 0;
}
