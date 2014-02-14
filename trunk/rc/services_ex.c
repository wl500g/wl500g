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

#ifdef ASUS_EXT
#define _GNU_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>
#include <errno.h>
#include <sys/fcntl.h>
#include <dirent.h>
#include <sys/mount.h>
#include <syslog.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <arpa/inet.h>
#include <ctype.h>
#include <mntent.h>

#include "rc.h"
#include "iboxcom.h"
#include "lp.h"


enum WEB_TYPE
{
	WEB_NONE = 0,
	WEB_CAMERA,
	WEB_AUDIO
};


static int umount_all_part(const char *product, int scsi_host_no);
static struct mntent *findmntent(const char *file);
static int stop_lltd(void);

size_t fappend(const char *name, FILE *f)
{
	size_t size = 0, count;
	
	FILE *fp = fopen(name, "r");

	if (fp != NULL) {
		char buf[4096];
		
		while ((count = fread(buf, 1, sizeof(buf), fp)) > 0)
			size += fwrite(buf, 1, count, f);
	
		fclose(fp);
	}
	
	return size;
}

int mkdir_if_none(const char *dir)
{
	DIR *dp;

	if (!(dp = opendir(dir)))
		return mkdir(dir, 0777);
	closedir(dp);
	return 0;
}

int
start_dns(void)
{
	FILE *fp;
	char *dnsmasq_argv[] = {"dnsmasq", NULL};
	pid_t pid;

	size_t ethers = 0;
	
	int ret;

	/* Create resolv.conf with empty nameserver list */
	if (!(fp = fopen("/tmp/resolv.conf", "a"))) {
		perror("/tmp/resolv.conf");
		return errno;
	}

	// if user want to set dns server by himself
	if (nvram_invmatch("wan_dnsenable_x", "1"))
	{
		/* Write resolv.conf with upstream nameservers */
		if (nvram_invmatch("wan_dns1_x",""))
			fprintf(fp, "nameserver %s\n", nvram_safe_get("wan_dns1_x"));
		if (nvram_invmatch("wan_dns2_x",""))
			fprintf(fp, "nameserver %s\n", nvram_safe_get("wan_dns2_x"));
	}

	/* append custom nameservers */
	fappend("/usr/local/etc/resolv.conf", fp);

	fclose(fp);

	/* create /etc/hosts */
	if (!(fp = fopen("/etc/hosts", "a"))) {
		perror("/etc/hosts");
		return errno;
	}

	fprintf(fp,
		"127.0.0.1 localhost.localdomain localhost\n"
		"%s %s my.router my.%s\n",
		nvram_safe_get("lan_ipaddr"), nvram_safe_get("lan_hostname"),
		nvram_safe_get("productid"));
#ifdef __CONFIG_IPV6__
	if (nvram_invmatch("ipv6_proto", "")) {
		fprintf(fp, "::1 localhost.localdomain localhost\n");
		if (nvram_invmatch("ipv6_proto", "tun6to4") &&
		    nvram_invmatch("ipv6_proto", "tun6rd") &&
		    !nvram_get_int("ipv6_lanauto_x")) {
			fprintf(fp, "%s %s my.router my.%s\n", nvram_safe_get("ipv6_lan_addr"),
				nvram_safe_get("lan_hostname"), nvram_safe_get("productid"));
		}
	}
#endif
	if (nvram_match("dhcp_static_x","1"))
	{	
		int i;
		char ip[32], name[32];
		for (i = 0; i < nvram_get_int("dhcp_staticnum_x"); i++) 
		{
			sprintf(ip, "dhcp_staticip_x%d", i);
			sprintf(name, "dhcp_staticname_x%d", i);

			if (ip_addr(nvram_safe_get(ip)) != INADDR_ANY &&
				nvram_invmatch(name, "")) 
			{
				fprintf(fp, "%s %s\n", nvram_get(ip), nvram_get(name));
			}
		}
	}
	
	fappend("/usr/local/etc/hosts", fp);
	fclose(fp);

	if (nvram_match("router_disable", "1"))
		return 0;

	/* create /etc/ethers */
	if (!(fp = fopen("/etc/ethers", "w"))) {
		perror("/etc/ethers");
	} else {
		if (nvram_match("dhcp_static_x","1"))
		{	
			int i;
			char buf[32], *mac;
			for (i = 0; i < nvram_get_int("dhcp_staticnum_x"); i++) 
			{
				sprintf(buf, "dhcp_staticmac_x%d", i);
				
				if (strlen(mac = nvram_safe_get(buf)) == 12) {
					sprintf(buf, "dhcp_staticip_x%d", i);
					ethers += fprintf(fp, "%c%c:%c%c:%c%c:%c%c:%c%c:%c%c %s\n", 
						mac[0], mac[1], mac[2], mac[3], mac[4], mac[5], 
						mac[6], mac[7], mac[8], mac[9], mac[10], mac[11],
						nvram_safe_get(buf));
				}
				
			}
		}
	
		ethers += fappend("/usr/local/etc/ethers", fp);
		fclose(fp);
	}

	/* start playing with conf file */
	if (!(fp = fopen("/etc/dnsmasq.conf", "w"))) {
		perror("/etc/dnsmasq.conf");
		return errno;
	}

	fprintf(fp, "user=nobody\n"
		    "resolv-file=/tmp/resolv.conf\n"
		    "no-poll\n"
		    "interface=%s\n", nvram_safe_get("lan_ifname"));

	if (nvram_invmatch("lan_domain", "")) {
		fprintf(fp, "domain=%s\n"
			    "expand-hosts\n", nvram_get("lan_domain"));
	}
	
	fprintf(fp, "no-negcache\n"
		    "cache-size=512\n"
		    "dhcp-leasefile=/tmp/dnsmasq.log\n");
	if (nvram_match("lan_proto", "dhcp")) {
		fprintf(fp,
			"dhcp-range=lan,%s,%s,%s\n",
			nvram_safe_get("dhcp_start"), nvram_safe_get("dhcp_end"),
			nvram_safe_get("dhcp_lease"));
		if (nvram_invmatch("dhcp_dns1_x",""))
			fprintf(fp, "dhcp-option=lan,6,%s,0.0.0.0\n", nvram_safe_get("dhcp_dns1_x"));
		if (nvram_invmatch("dhcp_wins_x",""))
			fprintf(fp, "dhcp-option=lan,44,%s\n"
				    "dhcp-option=lan,46,8\n", nvram_safe_get("dhcp_wins_x"));
		if (nvram_invmatch("lan_domain", ""))
			fprintf(fp, "dhcp-option=lan,15,%s\n", nvram_safe_get("lan_domain"));
		if (nvram_invmatch("dhcp_gateway_x",""))
			fprintf(fp, "dhcp-option=lan,3,%s\n", nvram_safe_get("dhcp_gateway_x"));
		if (ethers)
			fprintf(fp, "read-ethers\n");
		fprintf(fp, "dhcp-authoritative\n");
	}
#ifdef __CONFIG_IPV6__
	if (nvram_invmatch("ipv6_proto", "") &&
	    nvram_get_int("ipv6_radvd_enable")) {
#ifdef __CONFIG_RADVD__
	if (nvram_get_int("ipv6_radvd_enable") == 2)
		fprintf(fp, "dhcp-range=lan,::,static,%d\n", 600);
	else
#endif
		fprintf(fp, "quiet-ra\n"
			    "ra-param=%s,%d,%d\n"
			    "dhcp-range=lan,::,constructor:%s,ra-stateless,ra-names,%d,%d\n",
			nvram_safe_get("lan_ifname"), 10, 600*3,
			nvram_safe_get("lan_ifname"), 64, 600);
		fprintf(fp, "dhcp-option=lan,option6:23,");
		if (nvram_invmatch("ipv6_radvd_dns1_x", ""))
			fprintf(fp, "%s,", nvram_safe_get("ipv6_radvd_dns1_x"));
		fprintf(fp, "[::]\n");
		if (nvram_invmatch("lan_domain", ""))
			fprintf(fp, "dhcp-option=lan,option6:24,%s\n", nvram_safe_get("lan_domain"));
	}
#endif

	fappend("/usr/local/etc/dnsmasq.conf", fp);
	fclose(fp);

	/* launch it */		
	ret = _eval(dnsmasq_argv, NULL, 0, &pid);

	dprintf("done\n");
	return ret;
}	

int
stop_dns(void)
{
	return killall("dnsmasq");
}

int start_dhcpd(void)
{
	return 0;
}

int stop_dhcpd(void)
{
	return 0;
}

#ifdef __CONFIG_RADVD__
int
start_radvd(void)
{
	FILE *fp;
	pid_t pid;
	char *radvd_argv[] = {"radvd", NULL};
	struct in6_addr addr;
	int size, ret;
	char addrstr[INET6_ADDRSTRLEN];
	const char *dnsstr = NULL;

	if (!nvram_invmatch("ipv6_proto", "") ||
	    nvram_get_int("ipv6_radvd_enable") != 2)
		return 0;

	/* Create radvd.conf */
	if (!(fp = fopen("/etc/radvd.conf", "w"))) {
		perror("/etc/radvd.conf");
		return errno;
	}

	if (nvram_match("ipv6_proto", "tun6to4") ||
	    nvram_match("ipv6_proto", "tun6rd") ||
	    nvram_get_int("ipv6_lanauto_x")) {
		size = 64;
		sprintf(addrstr, "::");
	} else {
		size = ipv6_addr(nvram_safe_get("lan_ipv6_addr"), &addr);
		inet_ntop(AF_INET6, &addr, addrstr, INET6_ADDRSTRLEN);
		dnsstr = addrstr;
	}

	/* Write out to config file */
	fprintf(fp,
		"interface %s {\n"
		    "IgnoreIfMissing on;\n"
		    "AdvSendAdvert on;\n"
		    "AdvOtherConfigFlag on;\n",
		nvram_safe_get("lan_ifname"));

#ifdef BROKEN_IPV6_CONNTRACK
	/* Advertise tunnel MTU to avoid large packet issue */
	if (nvram_match("ipv6_proto", "tun6in4") ||
	    nvram_match("ipv6_proto", "tun6to4") ||
	    nvram_match("ipv6_proto", "tun6rd"))
		fprintf(fp,
		    "AdvLinkMTU %s;", nvram_safe_get("ipv6_sit_mtu"));
#endif

	/* Prefix */
	fprintf(fp,
		    "prefix %s/%d {\n"
			"AdvOnLink on;\n"
			"AdvAutonomous on;\n"
		    "};\n", addrstr, size);

	/* RDNSS */
	if (nvram_invmatch("ipv6_radvd_dns1_x", ""))
		dnsstr = nvram_safe_get("ipv6_radvd_dns1_x");
	if (dnsstr && *dnsstr) {
		fprintf(fp,
		    "RDNSS %s {};\n", dnsstr);
	}

	fprintf(fp, "};\n");
	fclose(fp);

	/* Enable IPv6 forwarding */
	fputs_ex("/proc/sys/net/ipv6/conf/all/forwarding", "1");

	/* Then start the radvd */
	ret = _eval(radvd_argv, NULL, 0, &pid);

	dprintf("done\n");
	return ret;
}

int
stop_radvd(void)
{
	return killall("radvd");
}
#endif /* __CONFIG_RADVD__ */

int
ddns_updated_main()
{
	FILE *fp;
	char buf[64];

	if (!(fp = fopen("/var/run/inadyn/inadyn.cache", "r")))
		return 0;

	fgets(buf, sizeof(buf), fp);
	fclose(fp);

	if (!nvram_match("ddns_ipaddr", buf)) {
		nvram_set("ddns_ipaddr", buf);
		nvram_set("ddns_status", "1");
		nvram_commit();
	}

	logmessage("ddns", "ddns update ok");

	dprintf("done\n");

	return 0;
}

static struct ddns_service {
	char *name;
	char *alias;
} ddns_services[] = {
	{ "update@asus.com", "WWW.ASUS.COM" },
	{ "default@dnsexit.com", NULL },
	{ "default@dnsomatic.com", "WWW.DNSOMATIC.COM" },
	{ "default@dyndns.org", "WWW.DYNDNS.ORG" },
	{ "default@dynsip.org", NULL },
	{ "default@easydns.com", "WWW.EASYDNS.COM" },
	{ "default@freedns.afraid.org", NULL },
	{ "ipv6tb@he.net", "WWW.TUNNELBROKER.NET" },
	{ "dyndns@he.net", "DNS.HE.NET" },
	{ "default@no-ip.com", "WWW.NO-IP.COM" },
	{ "default@sitelutions.com", NULL },
	{ "default@tzo.com", "WWW.TZO.COM" },
	{ "default@zoneedit.com", "WWW.ZONEEDIT.COM" },
	{ "default@duckdns.org", NULL },
	{ NULL, NULL }
};

int 
start_ddns(int type)
{
	FILE *fp;
	char word[32], *next;
	char *wan_ifname;
	char *ddns_argv[] = {
	    "/usr/sbin/inadyn",
	    "--background",
	    "--iterations", "1",
	    "--config", "/etc/ddns.conf",
	    "--exec", "/sbin/ddns_updated",
	    NULL };
	struct ddns_service *service;
	char services[64];
	char *server, *user, *passwd, *host;
	char *domain;
	int wild;

	if (nvram_match("router_disable", "1") ||
	    nvram_invmatch("ddns_enable_x", "1"))
		return -1;

	server = nvram_safe_get("ddns_server_x");
	for (service = &ddns_services[0]; service->name; service++) {
		if (strcmp(server, service->name) == 0)
			break;
		if (service->alias &&
		    strncmp(server, service->alias, strlen(service->alias)) == 0) {
			/* allow easy migration for web ui */
			nvram_set("ddns_server_x", service->name);
			break;
		}
	}
	if (service->name == NULL) {
		logmessage("ddns", "unknown server %s", server);
		return -1;
	}

	user = nvram_safe_get("ddns_username_x");
	passwd = nvram_safe_get("ddns_passwd_x");
	host = nvram_safe_get("ddns_hostname_x");
	domain = NULL;
	wild = nvram_match("ddns_wildcard_x", "1");

	if (strcmp(service->name, "update@asus.com") == 0) {
		strcpy(services, (type == 2) ? "register@asus.com " : "");
		strcat(services, "update@asus.com");
		user = nvram_safe_get("et0macaddr");
		passwd = nvram_safe_get("secret_code");
		if (strcasestr(host, ".asuscomm.com") == NULL)
			domain = ".asuscomm.com";
	} else
		strcpy(services, service->name);

	wan_ifname = nvram_safe_get("wan0_ifname");
	switch (_wan_proto("wan0_", word)) {
	case WAN_PPPOE:
	case WAN_PPTP:
	case WAN_L2TP:
		if (!nvram_match("ddns_realip_x", "2"))
			wan_ifname = nvram_safe_get("wan0_pppoe_ifname");
		break;
#ifdef __CONFIG_MADWIMAX__
	case WAN_WIMAX:
		wan_ifname = nvram_safe_get("wan0_wimax_ifname");
		break;
#endif
#ifdef __CONFIG_MODEM__
	case WAN_USBMODEM:
		wan_ifname = nvram_safe_get("wan0_pppoe_ifname");
		break;
#endif
	}

	if (!(fp = fopen("/etc/ddns.conf", "w"))) {
		perror("/etc/ddns.conf");
		return errno;
	}

	if (nvram_invmatch("ddns_realip_x", "0"))
		fprintf(fp, "iface %s\n", wan_ifname);

	foreach (word, services, next) {
		fprintf(fp, "system %s\n", word);
		if (*user)
			fprintf(fp, "username %s\n", user);
		if (*passwd)
			fprintf(fp, "password %s\n", passwd);
		fprintf(fp, "alias %s%s\n", host, domain ? : "");
		if (wild)
			fprintf(fp, "wildcard\n");
	}

	fappend("/usr/local/etc/ddns.conf", fp);
	fclose(fp);

	dprintf("ddns update %s %s\n", server, services);
	nvram_unset("ddns_ipaddr");
	nvram_unset("ddns_status");

	killall_tk("inadyn");
	if (type)
		unlink("/var/run/inadyn/inadyn.cache");

	_eval(ddns_argv, NULL, 0, NULL);

	return 0;
}

int 
stop_ddns(void)
{
	int ret = killall("inadyn");

	dprintf("done\n");
	return ret;
}

int start_logger(void)
{
	pid_t pid;
	int i;
	char *syslogd_argv[] = {"syslogd",
		"-m", "0",
		"-O", "/tmp/syslog.log",
		"-S", "-D",
		"-l", nvram_safe_get("log_level_x"),
		"-b", "1",
		NULL, NULL,	/* -LR addr */
		NULL, NULL,	/* -s size */
		NULL
	};
	char *klogd_argv[] = {"klogd", NULL};

#if 0
	if (nvram_match("router_disable", "1"))
		return 0;
#endif
	/* first empty arg */
	for (i = 0; syslogd_argv[i]; i++);

	/* -l argument */
	if (!*syslogd_argv[8])
		syslogd_argv[8] = "7";

	if (nvram_invmatch("log_ipaddr", ""))
	{
		syslogd_argv[i++] = "-LR";
		syslogd_argv[i++] = nvram_safe_get("log_ipaddr");
	}
	if (router_totalram() <= 16*1024*1024 /* 16MB */)
	{
		syslogd_argv[i++] = "-s";
		syslogd_argv[i++] = "32";
	}

	_eval(syslogd_argv, NULL, 0, &pid);
	usleep(500000);
	_eval(klogd_argv, NULL, 0, &pid);
	// remote log not easy to ok
	//usleep(500000);

	return 0;
}

int
stop_logger(void)
{
	int ret = killall("klogd");

	ret |= killall("syslogd");

	dprintf("done\n");
	return (ret);
}

int 
start_misc(void)
{
	pid_t pid;
	char *watchdog_argv[] = {"watchdog", NULL};
#ifdef __CONFIG_INFOSRV__
	char *infosrv_argv[] = {"infosrv", nvram_safe_get("lan_ifname"), NULL};
#endif
	const char *txpwr;

#ifdef __CONFIG_INFOSRV__
	if (!nvram_match("infosrv_disable", "1"))
		_eval(infosrv_argv, NULL, 0, &pid);
#endif
	_eval(watchdog_argv, NULL, 0, &pid);

	/* try to adjust wifi tx power */
	txpwr = nvram_safe_get("wl_radio_power_x");
	if (atoi(txpwr) > 0)
		eval("wl", "txpwr1", "-m", txpwr, "-o");

#if 1
	// for all product, fix antdiv
	if (eval("wl", "antdiv", "0") == 0)
		eval("wl", "txant", "0");
#endif

	return 0;
}

int
stop_misc(void)
{
	int ret;

	ret = killall("watchdog");
#ifdef __CONFIG_INFOSRV__
	killall("infosrv");
#endif
	stop_ntpc();
	stop_ddns();
	stop_lltd();

	dprintf("done\n");
	return(ret);
}

int
start_lltd(void)
{
#ifdef __CONFIG_LLTD__
	char *lltd_argv[] = {
		"lld2d",
		nvram_safe_get("lan_ifname"),
		nvram_safe_get("wl0_ifname"),
		NULL};
	pid_t pid;

	if (nvram_match("lltd_enable", "1") && nvram_invmatch("router_disable", "1"))
		return _eval(lltd_argv, NULL, 0, &pid);
	else
#endif
	return 0;
}

static int
stop_lltd(void)
{
#ifdef __CONFIG_LLTD__
	killall("lld2d");
#endif
	return 0;
}

#ifndef USB_SUPPORT
int start_usb(void)
{
	return 0;
}

int stop_usb(void)
{
	return 0;
}

int hotplug_usb(void)
{
	return 0;
}
#else /* USB_SUPPORT */

static int start_nfsd(void)
{
	struct stat	st_buf;
	FILE 		*fp;

	/* create directories/files */
	mkdir("/var/lib", 0755);
	mkdir("/var/lib/nfs", 0755);
	mkdir("/var/lib/nfs/v4recovery", 0755);
	mount("nfsd", "/proc/fs/nfsd", "nfsd", MS_MGC_VAL, NULL);
	close(creat("/var/lib/nfs/etab", 0644));
	close(creat("/var/lib/nfs/xtab", 0644));
	close(creat("/var/lib/nfs/rmtab", 0644));
	
	/* create /etc/exports, if it does not exists yet */
	if (stat("/etc/exports", &st_buf) != 0) 
	{
		int i, count;
		char tmp[sizeof("usb_nfslist_xXXXXX")];
		
		if ((fp = fopen("/etc/exports", "w")) == NULL) {
			perror("/etc/exports");
			return 1;
		}
		
		fprintf(fp, "# automagically generated\n");

		for (i = 0, count = nvram_get_int("usb_nfsnum_x"); i < count; i++) 
		{
			sprintf(tmp, "usb_nfslist_x%d", i);
			if (nvram_safe_get(tmp)[0] == '/')
				fprintf(fp, "%s\n", nvram_safe_get(tmp));
			else	fprintf(fp, "/tmp/harddisk/%s\n", nvram_safe_get(tmp));
		}
		fappend("/usr/local/etc/exports", fp);
		fclose(fp);
	}

	eval("/usr/sbin/portmap");
	eval("/usr/sbin/statd");
	eval("/usr/sbin/nfsd");
	eval("/usr/sbin/mountd");
	sleep(1);
	eval("/usr/sbin/exportfs", "-a");

	return 0;	
}

static int restart_nfsd(void)
{
	eval("/usr/sbin/exportfs", "-au");
	eval("/usr/sbin/exportfs", "-a");

	return 0;	
}

static int stop_nfsd(void)
{
	killall("mountd");
	killall_s("nfsd", SIGKILL);
	killall("statd");
	killall("portmap");

	umount("/proc/fs/nfsd");

	return 0;
}

int 
start_usb(void)
{
	insmod("usbcore", NULL);
	if (nvram_invmatch("usb20_disable_x", "1"))
	{
		insmod("ehci-hcd", NULL);
	}
	if (nvram_invmatch("usb20_disable_x", "2"))
	{
		insmod("ohci-hcd", NULL);
		insmod("uhci-hcd", NULL);
	}

	/* mount usbfs */
	mount("usbfs", "/proc/bus/usb", "usbfs", MS_MGC_VAL, NULL);

	insmod("usblp", NULL);
	if (!nvram_invmatch("lpr_enable", "1"))
	{
		char *lpd_argv[] = {"lpd", NULL};
		pid_t pid;

		sleep(1);
		_eval(lpd_argv, NULL, 0, &pid);	
	}
	if (!nvram_invmatch("raw_enable", "1"))
	{
		eval("p910nd", "-f", LP_DEV(0), "0");
	}
	if (!nvram_invmatch("audio_enable", "1"))
	{
		insmod("soundcore", NULL);
		insmod("audio", NULL);
	}
#ifdef __CONFIG_RCAMD__
	if (nvram_invmatch("usb_webenable_x", "0"))
	{	
		start_rcamd();
	}
#endif
	mkdir("/tmp/mnt", 0755);

	if (!nvram_match("usb_storage_x", "0"))
	{
		insmod("scsi_mod", NULL);
		insmod("sd_mod", NULL);
		insmod("usb-storage", NULL);
#ifdef __CONFIG_NTFS3G__
		if (nvram_match("usb_ntfs3g_enable", "1"))
		{
			insmod("fuse", NULL);
		}
		else
#endif
		if (nvram_match("usb_ntfs3g_enable", "0"))
		{
			insmod("ntfs", NULL);
		}
	}	

	if (nvram_match("usb_nfsenable_x", "1"))
	{	
		insmod("sunrpc", NULL);
		insmod("lockd", NULL);
		insmod("exportfs", NULL);
		insmod("nfsd", NULL);
		
		start_nfsd();
	}
	return 0;
}

int
stop_usb(void)
{
	if (nvram_match("usb_nfsenable_x", "1"))
	{	
		stop_nfsd();

		rmmod("nfsd");
		rmmod("exportfs");
		rmmod("lockd");
		rmmod("sunrpc");
	}
	

	if (!nvram_match("usb_storage_x", "0"))
	{
		killall("vsftpd");
		killall("smbd");
		killall("nmbd");
		killall("ntfs-3g");
		umount_all_part(NULL, -1);
		rmmod("usb-storage");
		rmmod("sd_mod");
		rmmod("scsi_mod");
#ifdef __CONFIG_NTFS3G__
		if (nvram_match("usb_ntfs3g_enable", "1"))
		{
			rmmod("fuse");
		}
		else
#endif
		if (nvram_match("usb_ntfs3g_enable", "0"))
		{
			rmmod("ntfs");
		}
	}

#ifdef __CONFIG_RCAMD__
	if (nvram_invmatch("usb_webenable_x", "0"))
	{
		stop_rcamd();	
	}
#endif
	rmmod("audio");
	rmmod("soundcore");
	killall("lpd");
	killall("p910nd");
	rmmod("usblp");

	umount("/proc/bus/usb");

	rmmod("uhci-hcd");
	rmmod("ohci-hcd");
	rmmod("ehci-hcd");
	rmmod("usbcore");
	return 0;
}

static void start_script(void)
{
//	pid_t pid;
	FILE *fp;
	char *script;
	char runbuf[512];

	script = nvram_safe_get("usb_ftpscript_x");
	if (strlen(script) == 0) return;
	sprintf(runbuf, "/tmp/harddisk/%s", script);
	fp = fopen(runbuf, "r");
	
	if (fp)
	{
		fclose(fp);

		//char *script_argv[] = {"/tmp/harddisk/init.usb", NULL};
		//_eval(script_argv, NULL, 0, &pid);

		logmessage("USB storage", "start user specified script");
		eval(runbuf);
	}	
}

/* get full storage path */
static char *nvram_storage_path(const char *var)
{
	static char buf[256];
	char *val = nvram_safe_get(var);
	int len = sprintf(buf, "%s%s", 
		val[0] == '/' ? "" : "/tmp/harddisk/", val);
	
	if (len > 1 && buf[len - 1] == '/')
		buf[len - 1] = 0;

	return buf;
}

int restart_ftpd()
{
	const char vsftpd_users[] = "/etc/vsftpd.users";
	const char vsftpd_passwd[] = "/etc/vsftpd.passwd";
	int i, count;
	int anonymous_mode;
	char tmp[256];
	FILE *fp, *f;

	killall("vsftpd");

	mkdir_if_none(vsftpd_users);

	if ((fp = fopen("/etc/vsftpd.conf", "w")) == NULL)
		return 1;

	if (nvram_match("usb_ftpsuper_x", "1"))
	{
		/* rights */
		sprintf(tmp, "%s/%s", vsftpd_users,
			nvram_safe_default_get("http_username"));
		if ((f = fopen(tmp, "w")) != NULL)
		{
			fprintf(f, 
				"dirlist_enable=yes\n"
				"write_enable=yes\n"
				"download_enable=yes\n");
			fclose(f);
		}
	}

	anonymous_mode = nvram_get_int("usb_ftpanonymous_x");
	if (anonymous_mode > 0)
	{
		fprintf(fp, 
			"anon_world_readable_only=no\n"
			"anon_umask=022\n");
		
		/* rights */
		sprintf(tmp, "%s/ftp", vsftpd_users);
		if ((f = fopen(tmp, "w")))
		{
			if (nvram_match("usb_ftpdirlist_x", "0"))
				fprintf(f, "dirlist_enable=yes\n");
			if (anonymous_mode == 1 || anonymous_mode == 3)
				fprintf(f, "write_enable=yes\n");
			if (anonymous_mode == 1 || anonymous_mode == 2)
				fprintf(f, "download_enable=yes\n");
			fclose(f);
		}
		if (anonymous_mode == 1 || anonymous_mode == 3)
		{
			fprintf(fp, 
				"anon_upload_enable=yes\n"
				"anon_mkdir_write_enable=yes\n"
				"anon_other_write_enable=yes\n");
		}
	} else {
		fprintf(fp, "anonymous_enable=no\n");
	}
	
	/* Passive mode */
	if (nvram_match("usb_ftp_pasvenable_x", "1"))
		fprintf(fp,
			"pasv_enable=yes\n"
			"pasv_min_port=%d\n"
			"pasv_max_port=%d\n",
			nvram_get_int("usb_ftp_pasvminport_x"),
			nvram_get_int("usb_ftp_pasvmaxport_x"));

	fprintf(fp,
		"dirmessage_enable=yes\n"
		"download_enable=no\n"
		"dirlist_enable=no\n"
		"hide_ids=yes\n"
		"syslog_enable=yes\n"
		"local_enable=yes\n"
		"local_umask=022\n"
		"chmod_enable=no\n"
		"chroot_local_user=yes\n"
		"allow_writable_root=yes\n"
		"check_shell=no\n"
		"isolate=no\n"
		"user_config_dir=%s\n"
		"passwd_file=%s\n",
		vsftpd_users, vsftpd_passwd);

	fprintf(fp,
		"listen%s=yes\n"
		"listen_port=%s\n"
		"background=yes\n"
		"max_clients=%s\n"
		"idle_session_timeout=%s\n",
#ifdef __CONFIG_IPV6__
		nvram_invmatch("ipv6_proto", "") ? "_ipv6" :
#endif
		"",
		nvram_safe_default_get("usb_ftpport_x"),
		nvram_get("usb_ftpmax_x") ? : "0",
		nvram_get("usb_ftpstaytimeout_x") ? : "300");

	if (nvram_match("usb_smbcset_x", "utf8"))
		fprintf(fp, "utf8=yes\n");

	/* ntfs(fuse), cifs does not support sendfile at the moment? */
	fprintf(fp, "use_sendfile=no\n");

	/* bandwidth */
	fprintf(fp,
		"anon_max_rate=%d\n"
		"local_max_rate=%d\n",
		nvram_get_int("usb_ftpanonrate_x") * 1024,
		nvram_get_int("usb_ftprate_x") * 1024);

	fclose(fp);

	/* prepare passwd file and default users */
	if ((fp = fopen(vsftpd_passwd, "w")) == NULL)
		return 2;

	fprintf(fp, /* anonymous, admin, nobody */
		"ftp:x:0:0:ftp:%s:/sbin/nologin\n"
		"%s:%s:0:0:root:/tmp/mnt/:/sbin/nologin\n"
		"nobody:x:99:99:nobody:/usr/share/empty:/sbin/nologin\n",
		nvram_storage_path("usb_ftpanonroot_x"),
		nvram_safe_default_get("http_username"),
		nvram_match("usb_ftpsuper_x", "1") ? 
			crypt(nvram_safe_default_get("http_passwd"), "$1$") : "x");

	for (i = 0, count = nvram_get_int("usb_ftpnum_x"); i < count; i++) 
	{
		const char *user = (sprintf(tmp, "usb_ftpusername_x%d", i), nvram_get(tmp));
		const char *pass = (sprintf(tmp, "usb_ftppasswd_x%d", i), nvram_get(tmp));
		const char *rights = (sprintf(tmp, "usb_ftprights_x%d", i), nvram_get(tmp));

		if (user && pass && rights)
		{
			/* directory */
			if (strncmp(rights, "Private", 7) == 0)
			{
				sprintf(tmp, "%s/%s", nvram_storage_path("usb_ftppvtroot_x"), user);
				mkdir_if_none(tmp);
			}
			else	sprintf(tmp, "%s", nvram_storage_path("usb_ftppubroot_x"));


			fprintf(fp, "%s:%s:0:0:%s:%s:/sbin/nologin\n",
				user, crypt(pass, "$1$"), user, tmp);

			/* rights */
			sprintf(tmp, "%s/%s", vsftpd_users, user);
			if ((f = fopen(tmp, "w")))
			{
				if (nvram_invmatch("usb_ftpdirlist_x", "1"))
					fprintf(f, "dirlist_enable=yes\n");
				if (strstr(rights, "Read") || !strcmp(rights, "Private"))
					fprintf(f, "download_enable=yes\n");
				if (strstr(rights, "Write") || !strncmp(rights, "Private", 7))
					fprintf(f, "write_enable=yes\n");
				fclose(f);
			}
		}
	}
	
	fclose(fp);
	
	eval("/usr/sbin/vsftpd");
	return 0;
}

int restart_smbd()
{
	FILE *fp;
	DIR *dir = NULL;
	struct dirent *dp;
	
	kill_pidfile("/var/lock/nmbd.pid");
	if (!kill_pidfile("/var/lock/smbd.pid"))
		sleep(2);	/* wait for smbd to finish */

	if ((fp = fopen("/etc/smb.conf", "w")) == NULL) {
		perror("/etc/smb.conf");
		return 1;
	}
		
	fprintf(fp, "[global]\n"
		"\tinterfaces = %s\n"
		"\tbind interfaces only = yes\n"
		"\tworkgroup = %s\n"
		"\tserver string = %s\n"
		"\tguest account = nobody\n"
		"\tsecurity = share\n"
		"\tbrowseable = yes\n"
		"\tguest ok = yes\n"
		"\tguest only = yes\n"
		"\tlog level = 1\n"
		"\tmax log size = 100\n"
		"\tencrypt passwords = no\n"
		"\tpreserve case = yes\n"
		"\tshort preserve case = yes\n",
		nvram_safe_default_get("lan_ifname"),
		nvram_safe_default_get("usb_smbwrkgrp_x"),
		nvram_get("productid") ? : "Samba");

	if (nvram_invmatch("usb_smbcpage_x", ""))
		fprintf(fp, "\tclient code page = %s\n", nvram_get("usb_smbcpage_x"));
	if (nvram_match("usb_smbcset_x", "utf8"))
		fprintf(fp, "\tcoding system = utf8\n");
	else if (nvram_invmatch("usb_smbcset_x", ""))
		fprintf(fp, "\tcharacter set = %s\n", nvram_get("usb_smbcset_x"));

	if (nvram_match("usb_smbenable_x", "1"))
	{
		fprintf(fp, "\n[share]\n"
			"\tpath = /tmp/harddisk\n");
		
		if (nvram_match("usb_smbhidden_x", "1"))
			fprintf(fp, "\n[share$]\n"
				"\tpath = /tmp/harddisk\n"
				"\tbrowseable = no\n");
		
		if (nvram_match("usb_smbhidden_x", "1") || 
		    nvram_match("usb_smbhidden_x", "2"))
			fprintf(fp, 
				"\twritable = yes\n"
				"\tforce user = %s\n",
				nvram_safe_default_get("http_username"));
	}

	/* share everything below /tmp/mnt */
	if (nvram_match("usb_smbenable_x", "2") && (dir = opendir("/tmp/mnt")))
		while ((dp = readdir(dir)))
	{
		if (strcmp(dp->d_name, ".") && strcmp(dp->d_name, ".."))
		{
			fprintf(fp, "\n[%s]\n"
				"\tpath = /tmp/mnt/%s\n", dp->d_name, dp->d_name);

			if (nvram_match("usb_smbhidden_x", "1"))
				fprintf(fp, "\n[%s$]\n"
					"\tpath = /tmp/mnt/%s\n"
					"\tbrowseable = no\n", dp->d_name, dp->d_name);
		
			if (nvram_match("usb_smbhidden_x", "1") || nvram_match("usb_smbhidden_x", "2"))
				fprintf(fp, 
					"\twritable = yes\n"
					"\tforce user = %s\n",
					nvram_safe_default_get("http_username"));
		}
	}
	if (dir) closedir(dir);

	if (nvram_match("usb_smbenable_x", "3"))
	{
		int i, count;
		char tmp[sizeof("usb_smblist_xXXXXX")];
		
		for (i = 0, count = nvram_get_int("usb_smbnum_x"); i < count; i++) 
		{
			/* share name */
			sprintf(tmp, "usb_smbshare_x%d", i);
			if (!nvram_invmatch(tmp, ""))
				continue;
			fprintf(fp, "\n[%s]\n", nvram_get(tmp));
		
			/* path */
			sprintf(tmp, "usb_smbdir_x%d", i);
			fprintf(fp, "\tpath = %s\n", 
				nvram_storage_path(tmp));

			/* access level */
			sprintf(tmp, "usb_smblevel_x%d", i);
			if (nvram_match(tmp, "Read/Write"))
				fprintf(fp, 
					"\twritable = yes\n"
					"\tforce user = %s\n",
					nvram_safe_default_get("http_username"));
			/* comment */
			sprintf(tmp, "usb_smbdesc_x%d", i);
			if (nvram_invmatch(tmp, ""))
				fprintf(fp, "\tcomment = %s\n", nvram_get(tmp));
		}
	}
	fappend("/usr/local/etc/smb.conf", fp);
	fclose(fp);

	eval("/usr/sbin/nmbd", "-D");
	eval("/usr/sbin/smbd", "-D");
	return 0;
}


/* unmount partitions
 *  scsi_host_no = -1	- all partitions (regardless product)
 *               = -2	- partitions related to device specified by product (2.6 only)
 *              >=  0	- partitions on device with specified scsi host
 */
static int umount_all_part(const char *product, int scsi_host_no)
{
	DIR *dir_to_open, *usb_dev_disc;
	struct dirent *dp;
	struct stat sd;
	char umount_dir[128];

	if (scsi_host_no != -1)
	{
	    // umount all partitions from specific scsi host only
	    char discs_path[128], parts[128];
	    char scsi_dev_link[256];
	    char buf[256];
	    FILE *part_fp;
	    struct mntent *mnt;
	    int t_host_no;
	    int len;

	/* /sys/bus/scsi/devices/<host_no>:x:x:x/block:[sda|sdb|...] */
	    if ((usb_dev_disc = opendir("/sys/bus/scsi/devices")))
    	    {
		DIR *dir_disc;
		FILE *prod_fp;

		while ((dp = readdir(usb_dev_disc))) {
		    if (sscanf(dp->d_name, "%d:%*s:%*s:%*s", &t_host_no) != 1)
			continue;

		    /* check for scsi host number */
		    if (scsi_host_no >= 0) {
			if (scsi_host_no != t_host_no)
			    continue;
			else
			    goto dev_found;
		    }

		    /* check for product id */
		    snprintf(scsi_dev_link, sizeof(scsi_dev_link), "%s/%s/product", "/sys/bus/scsi/devices", dp->d_name);
		    if ((prod_fp = fopen(scsi_dev_link, "r")) == NULL)
			continue;
		    if (fgets(buf, sizeof(buf), prod_fp)) {
			if (strncasecmp(product, buf, strlen(product)) != 0) {
			    fclose(prod_fp);
			    continue;
			}
		    }
		    fclose(prod_fp);

dev_found:
		    /* find corresponding block device */
		    len = 0;
		    snprintf(discs_path, sizeof(discs_path), "%s/%s", "/sys/bus/scsi/devices", dp->d_name);
		    if ((dir_disc = opendir(discs_path))) {
			while ((dp = readdir(dir_disc))) {
				if (strncmp(dp->d_name, "block:", 6) == 0)
                                             break;
			}
			strncpy(scsi_dev_link, dp->d_name + 6, sizeof(scsi_dev_link));
			len = strlen(scsi_dev_link);
			closedir(dir_disc);
		    }
		    if (len == 0)
			continue;

		    /* We have found a disc that is on this controller.
            	     * Loop thru all the partitions on this disc.
	             */
		    if ((part_fp = fopen("/proc/partitions", "r")))
		    {
			while (fgets(buf, sizeof(buf) - 1, part_fp))
			{
			    if (sscanf(buf, " %*s %*s %*s %s", parts) == 1)
			    {
				if (strncmp(parts, scsi_dev_link, len) == 0)
				{
				    snprintf(umount_dir, sizeof(umount_dir), "/dev/%s", parts);
				    if ((mnt = findmntent(umount_dir)))
				    {
					if (!umount(mnt->mnt_dir))
					    unlink(mnt->mnt_dir);
				    }
				    else
					umount(umount_dir);
				}
			    }
			}
			fclose(part_fp);
		    } /* partitions loop */

		}
		closedir(usb_dev_disc);
	    }

	}
	else if ((dir_to_open = opendir("/tmp/mnt")))
	{
		while ((dp = readdir(dir_to_open)))
		{
		    if (strncmp(dp->d_name, "disc", 4) == 0)
		    {
			sprintf(umount_dir, "/tmp/mnt/%s", dp->d_name);
			if (!umount(umount_dir))
				unlink(umount_dir);
		    }
		}
		closedir(dir_to_open);
	}

	/* We just unmounted first disk? */
	if (stat("/tmp/harddisk", &sd) != 0)
		unlink("/tmp/harddisk");

	return 0;
}

/* remove usb mass storage */
int remove_usb_mass(const char *product, int scsi_host_no)
{
	if ((product == NULL && scsi_host_no < 0)
		|| (product != NULL && nvram_match("usb_ftp_device", product)))
	{
		if (nvram_invmatch("usb_ftpenable_x", "0")) {
			killall("vsftpd");
		}
		if (nvram_invmatch("usb_smbenable_x", "0")) {
			killall("smbd");
			killall("nmbd");
		}
		nvram_set("usb_ftp_device", "");

		sleep(1);
	}

	umount_all_part(product, scsi_host_no);
	logmessage("USB storage", "removed");
	return 0;
}

/* stollen from the e2fsprogs/ismounted.c */
static struct mntent *findmntent(const char *file)
{
	struct mntent 	*mnt;
	struct stat	st_buf;
	dev_t		file_dev = 0, file_rdev = 0;
	ino_t		file_ino = 0;
	FILE 		*f;
	
	if ((f = setmntent ("/proc/mounts", "r")) == NULL)
		return NULL;

	if (stat(file, &st_buf) == 0) {
		if (S_ISBLK(st_buf.st_mode)) {
			file_rdev = st_buf.st_rdev;
		} else {
			file_dev = st_buf.st_dev;
			file_ino = st_buf.st_ino;
		}
	}
	while ((mnt = getmntent (f)) != NULL) {
		if (strcmp(file, mnt->mnt_fsname) == 0)
			break;
		if (stat(mnt->mnt_fsname, &st_buf) == 0) {
			if (S_ISBLK(st_buf.st_mode)) {
				if (file_rdev && (file_rdev == st_buf.st_rdev))
					break;
			} else {
				if (file_dev && ((file_dev == st_buf.st_dev) &&
						 (file_ino == st_buf.st_ino)))
					break;
			}
		}
	}

	fclose(f);
	return mnt;
}

struct ext2_super_block {
	uint32_t	s_inodes_count;
	uint32_t	s_blocks_count;
	uint32_t	s_r_blocks_count;
	uint32_t	s_free_blocks_count;
	uint32_t	s_free_inodes_count;
	uint32_t	s_first_data_block;
	uint32_t	s_log_block_size;
	uint32_t	s_dummy3[7];
	uint8_t		s_magic[2];
	uint16_t	s_state;
	uint32_t	s_dummy5[8];
	uint32_t	s_feature_compat;
	uint32_t	s_feature_incompat;
	uint32_t	s_feature_ro_compat;
	uint8_t		s_uuid[16];
	uint8_t		s_volume_name[16];
} __attribute__ ((__packed__));

static const char *detect_fs_type(const char *device)
{
	int fd;
	uint8_t buf[4096] __attribute__((aligned(4)));
	
	if ((fd = open(device, O_RDONLY)) < 0)
		return NULL;
		
	if (read(fd, buf, sizeof(buf)) != sizeof(buf))
	{
		close(fd);
		return NULL;
	}
	
	close(fd);
	
	/* first check for mbr */
	if (*device && device[strlen(device) - 1] > '9' &&
	    buf[510] == 0x55 && buf[511] == 0xAA && /* signature */
	    ((buf[0x1be] | buf[0x1ce] | buf[0x1de] | buf[0x1ee]) & 0x7f) == 0) /* boot flags */
	{
		return "mbr";
	} else
	/* detect swap */
	if (memcmp(buf + 4086, "SWAPSPACE2", 10) == 0 ||
	    memcmp(buf + 4086, "SWAP-SPACE", 10) == 0)
	{
		return "swap";
	} else
	/* detect ext2/3/4 */
	if (buf[0x438] == 0x53 && buf[0x439] == 0xEF)
	{
#define EXT3_FEATURE_INCOMPAT_JOURNAL_DEV	0x0008
#define EXT3_FEATURE_COMPAT_HAS_JOURNAL		0x0004

#define EXT2_FEATURE_INCOMPAT_UNSUPPORTED	(~0x0012)

#define EXT3_FEATURE_INCOMPAT_UNSUPPORTED	(~0x0016)
#define EXT3_FEATURE_RO_COMPAT_UNSUPPORTED	(~0x0007)
		struct ext2_super_block *es = (struct ext2_super_block *)(buf + 0x400);

		if ((le32_to_cpu(es->s_feature_compat) & EXT3_FEATURE_COMPAT_HAS_JOURNAL)
		  || (le32_to_cpu(es->s_feature_incompat) & EXT2_FEATURE_INCOMPAT_UNSUPPORTED))
		{
			/* Distinguish from jbd */
			if (le32_to_cpu(es->s_feature_incompat) & EXT3_FEATURE_INCOMPAT_JOURNAL_DEV)
				return "jbd";

			/* Ext4 has at least one feature which ext3 doesn't understand */
			if ((le32_to_cpu(es->s_feature_incompat) & EXT3_FEATURE_INCOMPAT_UNSUPPORTED)
			  || (le32_to_cpu(es->s_feature_ro_compat) & EXT3_FEATURE_RO_COMPAT_UNSUPPORTED))
				return	"ext4";
			else
				return	"ext3";
		}
		else
			return	"ext2";
	} else 
	/* detect ntfs */
	if (buf[510] == 0x55 && buf[511] == 0xAA && /* signature */
	    memcmp(buf + 3, "NTFS    ", 8) == 0)
	{
		return "ntfs";
	} else
	/* detect vfat */
	if (buf[510] == 0x55 && buf[511] == 0xAA && /* signature */
	    buf[11] == 0 && buf[12] >= 1 && buf[12] <= 8 /* sector size 512 - 4096 */ &&
	    buf[13] != 0 && (buf[13] & (buf[13] - 1)) == 0) /* sectors per cluster */
	{
	 	return "vfat";
	}

	return NULL;
}

#define MOUNT_VAL_FAIL 	0
#define MOUNT_VAL_RONLY	1
#define MOUNT_VAL_RW 	2
#define MOUNT_VAL_DUP 	3

static int mount_r(const char *mnt_dev, char *mnt_dir)
{
	struct mntent *mnt = findmntent(mnt_dev);
	const char *type;
	
	if (mnt) {
		if (strcmp(mnt->mnt_dir, mnt_dir) == 0)
			return MOUNT_VAL_RW;
		strcpy(mnt_dir, mnt->mnt_dir);
		return MOUNT_VAL_DUP;
	}
	
	if ((type = detect_fs_type(mnt_dev))) 
	{
		char options[64];
		const char *nls_cp;
		unsigned long flags = MS_NOATIME;

		options[0] = '\0';
		if (nvram_invmatch("usb_smbcset_x", ""))
			nls_cp = nvram_get("usb_smbcset_x");
		else
			nls_cp = NULL;

		if (strcmp(type, "swap") == 0 || strcmp(type, "mbr") == 0)
			flags = 0; /* not a mountable partition */
		else if (strcmp(type, "vfat") == 0)
		{
			int l = 0;

			if (nvram_invmatch("usb_vfat_options", ""))
				l = snprintf(options, 64, "%s", nvram_get("usb_vfat_options"));
			if (nls_cp)
				l += snprintf(options + l, 64-l, ",iocharset=%s%s" + ((l>0) ? 0 : 1),
					isdigit(nls_cp[0]) ? "cp" : "", nls_cp);
			if (nvram_invmatch("usb_smbcpage_x", ""))
				l += snprintf(options + l, 64-l, ",codepage=%s" + ((l>0) ? 0 : 1), 
					nvram_get("usb_smbcpage_x"));
			options[l+1] = '\0';
		}
		else if (strcmp(type, "ntfs") == 0)
		{
			flags = MS_RDONLY;
			if (nls_cp)
				sprintf(options, "nls=%s%s", 
					isdigit(nls_cp[0]) ? "cp" : "", nls_cp);
#ifdef __CONFIG_NTFS3G__
			if (nvram_match("usb_ntfs3g_enable", "1")) {
				flags = MS_NOATIME;
				mkdir_if_none(mnt_dir);
				eval("ntfs-3g", "-onoatime,big_writes", mnt_dev, mnt_dir);
				logmessage("USB storage", "%s%s fs at %s mounted to %s", 
					type, (flags & MS_RDONLY) ? "(ro)" : "", mnt_dev, mnt_dir);
				return (flags & MS_RDONLY) ? MOUNT_VAL_RONLY : MOUNT_VAL_RW;
			}
#endif
		}

		if (flags && !mkdir_if_none(mnt_dir) &&
			!mount(mnt_dev, mnt_dir, type, flags, options[0] ? options : NULL)) 
		{
			logmessage("USB storage", "%s%s fs at %s mounted to %s", 
				type, (flags & MS_RDONLY) ? "(ro)" : "", mnt_dev, mnt_dir);
			return (flags & MS_RDONLY) ? MOUNT_VAL_RONLY : MOUNT_VAL_RW;
		}

		rmdir(mnt_dir);
	}
	
	return MOUNT_VAL_FAIL;
}

/* insert usb mass storage */
int hotplug_usb_mass(const char *product)
{	
	DIR *dir_to_open, *usb_dev_disc, *usb_dev_part;
	char usb_disc[128], mnt_dev[128], mnt_dir[128];
	struct dirent *dp, *dp_disc;

	dir_to_open = usb_dev_disc = usb_dev_part = NULL;
	
	// Mount USB to system
	if ((usb_dev_disc = opendir("/dev/discs")))
	{
		eval("/usr/local/sbin/pre-mount", product);

		if (exists("/etc/fstab")) {
			eval("swapon", "-a");
			eval("mount", "-a");
		}
		
		while ((dp = readdir(usb_dev_disc)))
		{
			if (!strcmp(dp->d_name, "..") || !strcmp(dp->d_name, "."))
				continue;

			sprintf(usb_disc, "/dev/discs/%s", dp->d_name);

			if ((usb_dev_part = opendir(usb_disc)))
			{
				while ((dp_disc = readdir(usb_dev_part)))
				{
					/* assume disc is the first entry */
					int disc = !strcmp(dp_disc->d_name, "disc");
					
					if (!strcmp(dp_disc->d_name, "..") || !strcmp(dp_disc->d_name, "."))
						continue;
					
					sprintf(mnt_dev, "/dev/discs/%s/%s", dp->d_name, dp_disc->d_name);

					if (disc) sprintf(mnt_dir, "/tmp/mnt/%s", dp->d_name);
					else sprintf(mnt_dir, "/tmp/mnt/%s_%s", dp->d_name, 
						dp_disc->d_name + (strncmp(dp_disc->d_name, "part", 4) ? 0 : 4));

					if (mount_r(mnt_dev, mnt_dir))
					{
						if (disc) break; /* no mbr -- no partitions */
					}
				}
			}
		}
		
		/* create /tmp/harddisk pointing to first partition */
		usb_disc[0] = 0;	/* alphabetical sort minimum */
		for (dir_to_open = opendir("/tmp/mnt");
			dir_to_open && (dp = readdir(dir_to_open));)
		{
			if (!strncmp(dp->d_name, "disc", 4) &&
			    (usb_disc[0] == 0 || strcmp(dp->d_name, usb_disc) < 0))
				strcpy(usb_disc, dp->d_name);
		}
		
		if (usb_disc[0])
		{
			sprintf(mnt_dir, "/tmp/mnt/%s", usb_disc);
			symlink(mnt_dir, "/tmp/harddisk");

			nvram_set("usb_ftp_device", product);
			if (nvram_invmatch("usb_ftpenable_x", "0")) {
				restart_ftpd();
			}

			if (nvram_invmatch("usb_smbenable_x", "0") && nvram_invmatch("lan_hostname", "")) {
				restart_smbd();
			}

			if (nvram_match("usb_nfsenable_x", "1"))
			{	
				restart_nfsd();
			}
			
			//logmessage("USB storage", "attached");
			start_script();
		}

		//run script if any
		eval("/usr/local/sbin/post-mount", product, mnt_dir);
	}

	if (usb_dev_disc)
		closedir(usb_dev_disc);
	if (usb_dev_part)
		closedir(usb_dev_part);
	if (dir_to_open)
		closedir(dir_to_open);

	return 0;
}

/* plugging or removing usb device */
/* usbcore, usb-ohci, usb-ehci, printer are always there */
/* usb-storage, sd_mod, scsi_mod, videodev are there if functions are enabled */
/* pwc, ov511 i2c, depends on current status */

static const char *UVCLIST[] = {"46d/8c1/", "46d/8c2/", "46d/8c3/", "46d/8c5/", "46d/8c6/", "46d/8c7/", NULL};

int
hotplug_usb(void)
{
	char *action, *interface, *product, *device;
	int i;
	int isweb;

	if (!(device = getenv("DEVICE"))) device = "";
#ifdef DEBUG
	dprintf("%s-%s-%s. Dev:%s\n", getenv("ACTION"), getenv("INTERFACE"),
		getenv("PRODUCT"), device);
#endif
	if (!(interface = getenv("INTERFACE")) || !(action = getenv("ACTION")))
		return EINVAL;

	if ((product = getenv("PRODUCT")))
	{
		// see http://www.usb.org/developers/defined_class
#if defined(__CONFIG_MADWIMAX__) || defined(__CONFIG_MODEM__)
		/* communication device */
		if (strncmp(interface, "255/", 4) == 0 || // Vendor specific
			strncmp(interface, "2/", 2) == 0) // Communications and CDC Control
		{
			hotplug_network_device(interface, action, product, device);
		}
#endif
		/* usb storage */
		if (strncmp(interface, "8/", 2) == 0)
		{
			int scsi_host_no = -2;
#if defined(__CONFIG_MODEM__)
			hotplug_usb_modeswitch(interface, action, product, device);
#endif
			if (strcmp(action, "add") == 0)
				nvram_set("usb_storage_device", product);
			else
				remove_usb_mass(product, scsi_host_no);
			return 0;
		}

		if (strncmp(interface, "1/1", 3) == 0)
		{
			// if the audio device is the same with web cam,
			// just skip it
			if (nvram_match("usb_web_device", product)) 
				return 0;
			isweb = WEB_AUDIO;
			goto usbhandler;
		}
		else if (strncmp(interface, "1/", 2) == 0)
			return 0;

		/* ignore hubs */
		if (strncmp(interface, "9/", 2) == 0)
			return 0;

		i = 0;
		isweb = WEB_NONE;
		if (strcmp(interface, "14/1/0") == 0) { // Video
			isweb = WEB_CAMERA;
			goto usbhandler;
		}
		if (strcmp(interface, "255/1/0") == 0) { // Vendor specific
			while (UVCLIST[i] != NULL) {
				if (strncmp(product, UVCLIST[i], strlen(UVCLIST[i])) == 0) {
					isweb = WEB_CAMERA;
					goto usbhandler;
				}
				i++;
			}
		}
	}
	else return 0;

usbhandler:	
	if (!strncasecmp(action, "add", 3))
	{		
		if (isweb == WEB_NONE)
		{
			/* old usb-storage handler */
		}
		else if (isweb == WEB_AUDIO)
		{
			if (nvram_match("usb_audio_device", ""))
				logmessage("USB audio", "attached");
			nvram_set("usb_audio_device", product);
		}
		else
		{
			if (nvram_match("usb_web_device", ""))	
				logmessage("USB webcam", "attached");

			nvram_set("usb_web_device", product);
			nvram_set("usb_webdriver_x", "");
		}
	}
	else 
	{
		if (isweb == WEB_NONE) // Treat it as USB Storage
		{
			/* old usb-storage handler */
		}
		else if (isweb == WEB_AUDIO)
		{
			nvram_set("usb_audio_device", "");
		}
		else
		{
			if (nvram_invmatch("usb_web_device", ""))
			{	
				logmessage("USB webcam", "removed");
				nvram_set("usb_web_device", "");
			}
#ifdef __CONFIG_RCAMD__
			remove_usb_webcam(product);
#endif
		}
	}
	return 0;
}
#endif /* USB_SUPPORT */

/* stop necessary services for firmware upgrade */	
int
stop_service_main()
{
	stop_misc();

	preshutdown_system();
	stop_usb();

	/* nas is still needed for upgrade over WiFI with WPA enabled */
	/* stop_nas();*/

	stop_igmpproxy();
	stop_upnp();
	stop_snmpd();
#ifdef __CONFIG_IPV6__
	stop_dhcp6c();
#endif
#ifdef __CONFIG_RADVD__
	stop_radvd();
#endif
	stop_dhcpd();
	stop_dns();
	stop_logger();

	dprintf("done\n");
	return 0;
}

int service_handle(void)
{
	char *service = nvram_get("rc_service");
	char *value;

	if (!service)
		kill(1, SIGHUP);
	if (!(value = strdup(service)))
		return 0;

	service = strsep(&value, ",");
	value = strsep(&value, ",");

	if (strcmp(service,"wan_disconnect") == 0)
	{
		int unit = (value && *value) ? atoi(value) : -1;

		cprintf("wan disconnect, unit %d\n", unit);
		logmessage("wan", "disconnected manually (unit %d)", unit);
		if (unit < 0)
			stop_wan();
		else
			stop_wan_unit(unit);
	} else
	if (strcmp(service,"wan_connect") == 0)
	{
		int unit = (value && *value) ? atoi(value) : -1;

		cprintf("wan connect, unit %d\n", unit);
		logmessage("wan", "connected manually (unit %d)", unit);

		if (unit < 0)
		{
			stop_wan();
			sleep(3);
	    		start_wan();
	    		sleep(2);
		} else {
			char tmp[100], prefix[WAN_PREFIX_SZ];
			char *ping_argv[] = { "ping", "-c2", "140.113.1.1", NULL};
			char *str;
			pid_t pid;

			stop_wan_unit(unit);
			sleep(3);
	    		start_wan_unit(unit);
			sleep(2);

			/* trigger connect */
			sprintf(prefix, "wan%d_", unit);
			str = nvram_get(strcat_r(prefix, "gateway", tmp));
			if (str)
				ping_argv[2] = str;
			_eval(ping_argv, NULL, 0, &pid);
		}
	}

	nvram_unset("rc_service");
	return 0;
}

#endif /* ASUS_EXT */
