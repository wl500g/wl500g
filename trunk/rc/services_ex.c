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
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>
#include <errno.h>
#include <sys/fcntl.h>
#include <dirent.h>
#include <sys/mount.h>
#include <bcmnvram.h>
#include <netconf.h>
#include <shutils.h>
#include <rc.h>
#include <syslog.h>
#include "iboxcom.h"
#include "lp.h"

#define logs(s) syslog(LOG_NOTICE, s)

#ifdef USB_SUPPORT
#define WEBCAM_SUPPORT 1
#define PRINTER_SUPPORT 1
#define MASSSTORAGE_SUPPORT 1
#define AUDIO_SUPPORT 1
#endif

//#define PARPORT_SUPPORT 
//#define USBCOPY_SUPPORT 1

enum
{
	WEB_NONE = 0,
	WEB_PWCWEB,
	WEB_OVWEB,
	WEB_AUDIO
} WEBTYPE;

char *PWCLIST[] = {"471","69a","46d","55d","41e","4cc","d81", NULL};
char *OVLIST[] = {"5a9","813","b62", NULL};
char buf_g[512];
	
void diag_PaN(void)
{
	FILE *fp;
	char *token;
    	char                                mfr[32];
    	char                                model[64];
    	int                                 fd;
        int  i = 0;
        
    	struct parport_splink_device_info   prn_info;

	/* dump pci device */
	fp=fopen("/proc/pci", "r");
	if (fp!=NULL)
	{
		while(fgets(buf_g, sizeof(buf_g), fp))
		{
			if(strstr(buf_g, "PCI device"))
				fprintf(stderr, buf_g);
		}
		fclose(fp);
	}


#ifdef RT2400_SUPPORT
	if (nvram_match("nobr", "1"))
		fprintf(stderr, "dumping 0x4301 for manu.\n");
#endif

#ifdef USB_SUPPORT
	if (nvram_match("usb_device", "1"))
	{
		fprintf(stderr, "new USB device\r\n");
	}
#endif

#ifdef PRINTER_SUPPORT
#ifdef PARPORT_SUPPORT	
    if( (fd=open("/dev/lp0", O_RDWR)) < 0 ) //Someone is opening the lp0
    {
        fp=fopen("/proc/sys/dev/parport/parport0/devices/lp/deviceid","r");

        if( fp != NULL)
        {    
            while ( fgets(buf_g, sizeof(buf_g), fp) != NULL )  
            {
                if(buf_g[0] == '\n')
                {
                    continue;
                }
                if(strncmp(buf_g , "Model: " , strlen("Model: ")) == 0)
                {
                    token = buf_g + strlen("Model: ");
		    sprintf(buf_g, "PARPORT: %s", token);
		    fprintf(stderr, "%s\r\n", buf_g);
		    //logmessage("PARPORT", buf_g);
                }                
	    }	
	    fclose(fp);
	}
	//else logmessage("NO", "No printer found");
    }
    else
    {
        if( ioctl(fd, LPGETID, &prn_info) <0 )
        {
	    //logmessage("PaN", "No printer found");
            //PRINT("ioctl error\n");
        }
        else
        {
            memccpy(mfr , prn_info.mfr , 1 , 32);
            memccpy(model , prn_info.model , 1 , 32);
        
            sprintf(buf_g, "PARPORT: %s", model);
	    fprintf(stderr, "%s\r\n", buf_g);
            //logmessage("PARPORT", buf_g);
        }
	close(fd);
   }
#endif
#endif
   fprintf(stderr, "echo for PaN ::: &&&PaN\r\n");
}

int
start_dhcpd(void)
{
	FILE *fp;
	char *dhcpd_argv[] = {"udhcpd", "/tmp/udhcpd.conf", NULL, NULL};
	char *slease = "/tmp/udhcpd-br0.sleases";
	pid_t pid;

	if (nvram_match("router_disable", "1") || nvram_invmatch("lan_proto", "dhcp"))
		return 0;

	dprintf("%s %s %s %s\n",
		nvram_safe_get("lan_ifname"),
		nvram_safe_get("dhcp_start"),
		nvram_safe_get("dhcp_end"),
		nvram_safe_get("lan_lease"));

	if (!(fp = fopen("/tmp/udhcpd-br0.leases", "a"))) {
		perror("/tmp/udhcpd-br0.leases");
		return errno;
	}
	fclose(fp);

	/* Write configuration file based on current information */
	if (!(fp = fopen("/tmp/udhcpd.conf", "w"))) {
		perror("/tmp/udhcpd.conf");
		return errno;
	}
	
	fprintf(fp, "pidfile /var/run/udhcpd-br0.pid\n");
	fprintf(fp, "start %s\n", nvram_safe_get("dhcp_start"));
	fprintf(fp, "end %s\n", nvram_safe_get("dhcp_end"));
	fprintf(fp, "interface %s\n", nvram_safe_get("lan_ifname"));
	fprintf(fp, "remaining yes\n");
	fprintf(fp, "lease_file /tmp/udhcpd-br0.leases\n");
	fprintf(fp, "option subnet %s\n", nvram_safe_get("lan_netmask"));
	
	if (nvram_invmatch("dhcp_gateway_x",""))
	    fprintf(fp, "option router %s\n", nvram_safe_get("dhcp_gateway_x"));	else	
	    fprintf(fp, "option router %s\n", nvram_safe_get("lan_ipaddr"));	
	
	if (nvram_invmatch("dhcp_dns1_x",""))		
		fprintf(fp, "option dns %s\n", nvram_safe_get("dhcp_dns1_x"));		
	fprintf(fp, "option dns %s\n", nvram_safe_get("lan_ipaddr"));
	fprintf(fp, "option lease %s\n", nvram_safe_get("dhcp_lease"));

	if (nvram_invmatch("dhcp_wins_x",""))		
		fprintf(fp, "option wins %s\n", nvram_safe_get("dhcp_wins_x"));		
	if (nvram_invmatch("lan_domain", ""))
		fprintf(fp, "option domain %s\n", nvram_safe_get("lan_domain"));
	fclose(fp);

	
	if (nvram_match("dhcp_static_x","1"))
	{	
		write_static_leases(slease);
		dhcpd_argv[2] = slease;
	}
	else
	{
		dhcpd_argv[2] = NULL;
	}

	_eval(dhcpd_argv, NULL, 0, &pid);


	dprintf("done\n");
	return 0;
}

int
stop_dhcpd(void)
{
	char sigusr1[] = "-XX";
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
	ret = eval("killall", "udhcpd");

	dprintf("done\n");
	return ret;
}

int
start_dns(void)
{
	FILE *fp;
	char *dproxy_argv[] = {"dproxy", "-d", "-c", "/tmp/dproxy.conf", NULL};
	pid_t pid;
	char word[100], *tmp;
	int ret, active;

	if (nvram_match("router_disable", "1"))
		return 0;


	/* Create resolv.conf with empty nameserver list */
	if (!(fp = fopen("/tmp/resolv.conf", "r")))
	{
		if (!(fp = fopen("/tmp/resolv.conf", "w"))) 
		{
			perror("/tmp/resolv.conf");
			return errno;
		}
		else fclose(fp);
	}
	else fclose(fp);

	if (!(fp = fopen("/tmp/dproxy.conf", "w"))) {
		perror("/tmp/dproxy.conf");
		return errno;
	}

	fprintf(fp, "name_server=140.113.1.1\n");
	fprintf(fp, "ppp_detect=no\n");
	fprintf(fp, "purge_time=1200\n");
	fprintf(fp, "deny_file=/tmp/dproxy.deny\n");
	fprintf(fp, "cache_file=/tmp/dproxy.cache\n");
	fprintf(fp, "hosts_file=/tmp/hosts\n");
	fprintf(fp, "dhcp_lease_file=\n");
	fprintf(fp, "ppp_dev=/var/run/ppp0.pid\n");
	fclose(fp);

	// if user want to set dns server by himself
	if (nvram_invmatch("wan_dnsenable_x", "1"))	
	{
		/* Write resolv.conf with upstream nameservers */
		if (!(fp = fopen("/tmp/resolv.conf", "w"))) {
			perror("/tmp/resolv.conf");
			return errno;
		}
	
		if (nvram_invmatch("wan_dns1_x",""))
			fprintf(fp, "nameserver %s\n", nvram_safe_get("wan_dns1_x"));		
		if (nvram_invmatch("wan_dns2_x",""))
			fprintf(fp, "nameserver %s\n", nvram_safe_get("wan_dns2_x"));
		fclose(fp);
	}

	active = timecheck_item(nvram_safe_get("url_date_x"), 
				nvram_safe_get("url_time_x"));

	if (nvram_match("url_enable_x", "1") && active) 
	{
		int i;
		
		if (!(fp = fopen("/tmp/dproxy.deny", "w"))) {
			perror("/tmp/dproxy.deny");
			return errno;
		}

		for(i=0; i<atoi(nvram_safe_get("url_num_x")); i++) {
			sprintf(word, "url_keyword_x%d", i);
			fprintf(fp, "%s\n", nvram_safe_get(word));
		}
	
		fclose(fp);	
	}
	else unlink("/tmp/dproxy.deny");


	if (!(fp = fopen("/tmp/hosts", "w"))) {
		perror("/tmp/hosts");
		return errno;
	}

	fprintf(fp, "127.0.0.1 localhost.localdomain localhost\n");
	fprintf(fp, "%s	my.router\n", nvram_safe_get("lan_ipaddr"));
	fprintf(fp, "%s	my.%s\n", nvram_safe_get("lan_ipaddr"), nvram_safe_get("productid"));
	
	if (nvram_invmatch("lan_hostname", ""))
	{
		fprintf(fp, "%s %s.%s %s\n", nvram_safe_get("lan_ipaddr"),
					nvram_safe_get("lan_hostname"),
					nvram_safe_get("lan_domain"),
					nvram_safe_get("lan_hostname"));
	}	
	fclose(fp);	
		
	_eval(dproxy_argv, NULL, 0, &pid);

	dprintf("done\n");
	return ret;
}	

int
stop_dns(void)
{
	int ret = eval("killall", "dproxy");
	unlink("/tmp/dproxy.deny");		
	dprintf("done\n");
	return ret;
}

int
ddns_updated_main(int argc, char *argv[])
{
	FILE *fp;
	char buf[64], *ip;

	if (!(fp=fopen("/tmp/ddns.cache", "r"))) return 0;
	
	fgets(buf, sizeof(buf), fp);
	fclose(fp);

	if (!(ip=strchr(buf, ','))) return 0;
	
	nvram_set("ddns_cache", buf);
	nvram_set("ddns_ipaddr", ip+1);
	nvram_set("ddns_status", "1");
	nvram_commit();

	logmessage("ddns", "ddns update ok");

	dprintf("done\n");

	return 0;
}
	

int 
start_ddns(void)
{
	FILE *fp;
	char buf[64];
	char *wan_ip, *ddns_cache;
	char server[32];
	char user[32];
	char passwd[32];
	char host[64];
	char service[32];
	char usrstr[64];
	char wan_ifname[16];
	int  wild=nvram_match("ddns_wildcard_x", "1");


	if (nvram_match("router_disable", "1")) return -1;
	
	if (nvram_invmatch("ddns_enable_x", "1")) return -1;
	
	if ((wan_ip = nvram_safe_get("wan_ipaddr_t"))==NULL) return -1;

	if (nvram_match("ddns_ipaddr", wan_ip)) return -1;

	if (inet_addr(wan_ip)==inet_addr(nvram_safe_get("ddns_ipaddr"))) return -1;

	// TODO : Check /tmp/ddns.cache to see current IP in DDNS
	// update when,
	// 	1. if ipaddr!= ipaddr in cache
	// 	
        // update
	// * nvram ddns_cache, the same with /tmp/ddns.cache


	if ((fp=fopen("/tmp/ddns.cache", "r"))==NULL && 
	     (ddns_cache=nvram_get("ddns_cache"))!=NULL)
	{
		if ((fp = fopen("/tmp/ddns.cache", "w+"))!=NULL)
		{
			fprintf(fp, "%s", ddns_cache);
			fclose(fp);
		}
	}

	strcpy(server, nvram_safe_get("ddns_server_x"));
	strcpy(user, nvram_safe_get("ddns_username_x"));
	strcpy(passwd, nvram_safe_get("ddns_passwd_x"));
	strcpy(host, nvram_safe_get("ddns_hostname_x"));
	strcpy(service, "");
			
	if (strcmp(server, "WWW.DYNDNS.ORG")==0)
		strcpy(service, "dyndns");			
	else if (strcmp(server, "WWW.DYNDNS.ORG(CUSTOM)")==0)
		strcpy(service, "dyndns");			
	else if (strcmp(server, "WWW.DYNDNS.ORG(STATIC)")==0)
		strcpy(service, "dyndns");			
	else if (strcmp(server, "WWW.TZO.COM")==0)
		strcpy(service, "tzo");			
	else if (strcmp(server, "WWW.ZONEEDIT.COM")==0)
		strcpy(service, "zoneedit");
	else if (strcmp(server, "WWW.JUSTLINUX.COM")==0)
		strcpy(service, "justlinux");
	else if (strcmp(server, "WWW.EASYDNS.COM")==0)
		strcpy(service, "easydns");
	else strcpy(service, "dyndns");
			
	sprintf(usrstr, "%s:%s", user, passwd);
	
	if (nvram_match("wan_proto", "pppoe") || nvram_match("wan_proto", "pptp"))
	{
		strcpy(wan_ifname, nvram_safe_get("wan0_pppoe_ifname"));
	}
	else
	{
		strcpy(wan_ifname, nvram_safe_get("wan0_ifname"));
	}	

	dprintf("wan_ifname: %s\n\n\n\n", wan_ifname);

	if (strlen(service)>0)
	{
		char *ddns_argv[] = {"ez-ipupdate", 
		"-S", service,
	        "-i", wan_ifname,
 		"-u", usrstr,
		"-h", host,
		"-e", "/sbin/ddns_updated",
		"-b", "/tmp/ddns.cache",
		wild ? "-w" : NULL,
		NULL};	
		pid_t pid;

		dprintf("ddns update %s %s\n", server, service);
		nvram_unset("ddns_cache");
		nvram_unset("ddns_ipaddr");
		nvram_unset("ddns_status");

		eval("killall", "ez-ipupdate");
		_eval(ddns_argv, NULL, 0, &pid);
	}
	return 0;
}

int 
stop_ddns(void)
{
	int ret = eval("killall", "ez-ipupdate");

	dprintf("done\n");
	return ret;
}

int 
start_logger(void)
{		
	pid_t pid;

	if (nvram_match("router_disable", "1"))
		return 0;

	if (nvram_invmatch("log_ipaddr", ""))
	{
		char *syslogd_argv[] = {"syslogd", "-m", "0", "-t", nvram_safe_get("time_zone"), "-O", "/tmp/syslog.log", "-R", nvram_safe_get("log_ipaddr"), "-L", NULL};
#ifdef KERNEL_DBG
		char *klogd_argv[] = {"klogd", "-d", NULL};
#else
		char *klogd_argv[] = {"klogd", NULL};
#endif

		//while(syslogd_argv[i]) printf("log: %s\n", syslogd_argv[i++]);
	
		_eval(syslogd_argv, NULL, 0, &pid);
		_eval(klogd_argv, NULL, 0, &pid);
	}
	else
	{
		char *syslogd_argv[] = {"syslogd", "-m", "0", "-t", nvram_safe_get("time_zone"), "-O", "/tmp/syslog.log", NULL};

#ifdef KERNEL_DBG
		char *klogd_argv[] = {"klogd", "-d", NULL};
#else
		char *klogd_argv[] = {"klogd", NULL};
#endif

		_eval(syslogd_argv, NULL, 0, &pid);
		_eval(klogd_argv, NULL, 0, &pid);
	}
	// remote log not easy to ok
	sleep(1);
	return 0;
}

int
stop_logger(void)
{
	int ret1 = eval("killall", "klogd");
	int ret2 = eval("killall", "syslogd");

	dprintf("done\n");
	return (ret1|ret2);
}

int 
start_misc(void)
{ 
	char *infosvr_argv[] = {"infosvr", "br0", NULL};
	char *watchdog_argv[] = {"watchdog", NULL};
	pid_t pid;
	
	_eval(infosvr_argv, NULL, 0, &pid);
	_eval(watchdog_argv, NULL, 0, &pid);

	/* adjust some special parameters here */
	/* tx power */
	if (nvram_invmatch("wl_radio_power_x", "19"))
	{
		eval("wl", "txpwr", nvram_safe_get("wl_radio_power_x"));
	}


	// for all product, fix antdiv
	eval("wl", "antdiv", "0");
	eval("wl", "txant", "0");

	return 0;
}

int
stop_misc(void)
{
	int ret1 = eval("killall", "infosvr");
	int ret2 = eval("killall", "watchdog");
	int ret3 = eval("killall", "ntp");

	dprintf("done\n");
	return(ret1);
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
#else
int 
start_usb(void)
{
	eval("insmod", "usbcore");
	eval("insmod", "usb-ohci");
	eval("insmod", "uhci");
	eval("insmod", "ehci-hcd");

#ifdef PRINTER_SUPPORT
#ifdef PARPORT_SUPPORT	
	symlink("/dev/printers/0", "/dev/lp0");
	symlink("/dev/lp0", "/tmp/lp0");
	eval("insmod", "parport.o");
	eval("insmod", "parport_splink.o");
	eval("insmod", "lp.o");
#endif		
	eval("insmod", "printer.o");
	mkdir("/var/state", 0777);
	mkdir("/var/state/parport", 0777);
	mkdir("/var/state/parport/svr_statue", 0777);
	{
		char *lpd_argv[]={"lpd", NULL};
		pid_t pid;

		sleep(1);
		_eval(lpd_argv, ">/dev/null", 0, &pid);	
	}
#endif	
#ifdef AUDIO_SUPPORT
	eval("insmod", "soundcore.o");
	eval("insmod", "audio.o");
	start_audio();
#endif
#ifdef WEBCAM_SUPPORT	
	if (nvram_invmatch("usb_webenable_x", "0"))
	{	
		eval("insmod", "videodev.o");

		// link video 
		symlink("/dev/v4l/video0", "/dev/video");
		start_rcamd();
	}
#endif
#ifdef MASSSTORAGE_SUPPORT
	if (nvram_invmatch("usb_ftpenable_x", "0"))
	{
		eval("insmod", "scsi_mod.o");
		eval("insmod", "sd_mod.o");
		eval("insmod", "usb-storage.o");
		mkdir("/tmp/harddisk", 0444);
	}	
#endif
}

int
stop_usb(void)
{
#ifdef MASSSTORAGE_SUPPORT
	if (nvram_invmatch("usb_ftpenable_x", "0"))
	{
		eval("killall", "stupid-fptd");
		eval("rmmod", "usb-storage");
		eval("rmmod", "sd_mod");
		eval("rmmod", "scsi_mod");
	}
#endif
#ifdef WEBCAM_SUPPORT	
	if (nvram_invmatch("usb_webenable_x", "0"))
	{
		stop_rcamd();	
		eval("killall", "rcamd");
		eval("killall", "sendmail");
		eval("rmmod", "pwc");
		eval("rmmod", "ov511_decomp");
		eval("rmmod", "ov518_decomp");
		eval("rmmod", "ov51x");
		eval("rmmod", "i2c-core");
		eval("rmmod", "videodev");
	}
#endif
#ifdef AUDIO_SUPPORT
	eval("rmmod", "audio");
	eval("rmmod", "soundcore");
	stop_audio();
#endif
#ifdef PRINTER_SUPPORT	
	eval("killall", "lpd");
	eval("rmmod", "printer");
#ifdef PARPORT_SUPPORT
	eval("rmmod", "lp.o");
	eval("rmmod", "parport_splink.o");
	eval("rmmod", "parport.o");
#endif
#endif	
	eval("rmmod", "usb-ehci");
	eval("rmmod", "usb-ohci");
	eval("rmmod", "usbcore");
}

void start_script(void)
{
	pid_t pid;
	FILE *fp;
	char *script;
	char runbuf[512];

	script = nvram_safe_get("usb_ftpscript_x");
	if (strlen(script)==0) return;
	sprintf(runbuf, "/tmp/harddisk/%s", script);
	fp=fopen(runbuf, "r");
	
	if (fp)
	{
		fclose(fp);

		//char *script_argv[]={"/tmp/harddisk/init.usb", NULL};
		//_eval(script_argv, NULL, 0, &pid);

		logmessage("USB storage", "start user specified script");
		eval(runbuf);
	}	
}

void start_ftpd()
{
	FILE *fp;
	char *ftpd_argv[] = {"stupid-ftpd", NULL};
	char user[32], user1[32], password[32], path[32];
	char tmpstr[32];
	char rright[128], wright[128], maxuser[16];
	int snum, unum, i, j;
	pid_t pid;


	fp=fopen("/tmp/stupid-ftpd.mtd", "w");
	if (fp==NULL) return;	
	fprintf(fp, "Welcom to My FTP Site:\n");
	fclose(fp);
		
	fp=fopen("/tmp/stupid-ftpd.bye", "w");
	if (fp==NULL) return;	
	fprintf(fp, "Thanks for your coming. Byte Byte!!\n");
	fclose(fp);

	fp=fopen("/tmp/stupid-ftpd.conf", "w");
	if (fp==NULL) return;
	
	fprintf(fp, "mode=daemon\n");
	fprintf(fp, "serverroot=/tmp/harddisk\n");
	fprintf(fp, "changeroottype=real\n");

	fprintf(fp, "motd=/tmp/stupid-ftpd.motd\n");
	fprintf(fp, "byemsg=/tmp/stupid-ftpd.bye\n");
	fprintf(fp, "banmsg=You have no permission\n");
	fprintf(fp, "log=/tmp/stupid-ftpd.log\n");
	fprintf(fp, "port=%s\n", nvram_safe_get("usb_ftpport_x"));
	fprintf(fp, "maxusers=%s\n", nvram_safe_get("usb_ftpmax_x"));	
	fprintf(fp, "login-timeout=%s\n", nvram_safe_get("usb_ftptimeout_x"));
	fprintf(fp, "timeout=%s\n", nvram_safe_get("usb_ftpstaytimeout_x"));


	if (nvram_match("usb_ftpanonymous_x", "1"))
	{
		fprintf(fp, "user=anonymous * // 0 A\n");
	}
	if (nvram_match("usb_ftpsuper_x", "1"))
	{
		fprintf(fp, "user=%s %s // 0 A\n", nvram_safe_get("http_username"), nvram_safe_get("http_passwd"));
	}

	write_ftp_banip(fp);
	write_ftp_userlist(fp);
	fclose(fp);

	//_eval(ftpd_argv, NULL, 0, &pid); 
	eval("stupid-ftpd");
}	

int
hotplug_usb_webcam(char *product, int webflag)
{
	char *rcamd_argv[]={"rcamd", 
				"-p", nvram_safe_get("usb_webactivex_x"),
				"-s", "0",
				"-z", nvram_safe_get("time_zone"),
				"-a", nvram_safe_get("usb_websecurity_x"),
				NULL, NULL,	// Model -t
				NULL, NULL,	// Record Time -r
				NULL, NULL, 	// Image Size -f
				NULL, NULL, 	// Sense Vlaue -m 
				NULL, NULL, 	// Sense Limit -c
				NULL, NULL,
				NULL};
	char **arg;
	pid_t pid;

	if (nvram_match("usb_webenable_x", "0") || strlen(product)==0 || webflag==0) return;
	for (arg = rcamd_argv; *arg; arg++);
	
	if (webflag == WEB_PWCWEB)
	{
		eval("insmod", "pwc.o", "power_save=0");
		nvram_set("usb_webdriver_x", "0");

		*arg++ = "-t";
		*arg++ = "0";
	}
	else
	{
		eval("insmod", "i2c-core.o");
		eval("insmod", "ov51x.o");
		eval("insmod", "ov511_decomp.o");
		eval("insmod", "ov518_decomp.o");
		nvram_set("usb_webdriver_x", "1");

		if (strstr(product, "8519"))
		{
			*arg++ = "-t";
			*arg++ = "2";
		}
		else
		{			
			*arg++ = "-t";
			*arg++ = "1";
		}
	}

	// start web cam
	if (nvram_match("usb_webmode_x", "0")) // ActiveX only
	{
		*arg++ = "-r";
		*arg++ = "60";
	}
	else 
	{
		*arg++ = "-r";
		*arg++ = "0";
	}
		
	// image size
	if (nvram_match("usb_webimage_x", "0"))
	{
		*arg++ = "-f";
		if (webflag==WEB_PWCWEB) *arg++="320x240";
		else *arg++="640x480";
	}
	else if (nvram_match("usb_webimage_x", "1"))
	{
		*arg++ = "-f";
		*arg++="320x240";
	}
	else if (nvram_match("usb_webimage_x", "2"))
	{
		*arg++ = "-f";
		if (webflag==WEB_PWCWEB) *arg++="160x120";
		else *arg++="176x144";
	}
	else
	{
		*arg++ = "-f";
		*arg++ = "80x60";
	}


	if (nvram_match("usb_websense_x", "0"))
	{	
		*arg++ = "-m";
		*arg++ = "150";
		*arg++ = "-c";
		*arg++ = "100";
	}
	else if (nvram_match("usb_websense_x", "1"))
	{
		*arg++ = "-m";
		*arg++ = "100";
		*arg++ = "-c";
		*arg++ = "100";
	}
	else
	{	
		*arg++ = "-m";
		*arg++ = "50";
		*arg++ = "-c";
		*arg++ = "100";
	}
	

	//*arg++="-d";
	//*arg++="7";
	mkdir("/tmp/webcam", 0777);	
	chdir("/tmp/webcam");

	symlink("/www/Advanced_ShowTime_Widzard.asp", "/tmp/webcam/index.asp");
	symlink("/www/Advanced_ShowTime_Widzard.asp", "/tmp/webcam/ShowWebCam.asp");
	symlink("/www/ShowWebCamPic.asp", "/tmp/webcam/ShowWebCamPic.asp");
	symlink("/www/graph", "/tmp/webcam/graph");
	symlink("/www/general.js", "/tmp/webcam/general.js");
	symlink("/www/overlib.js", "/tmp/webcam/overlib.js");
	symlink("/www/style.css", "/tmp/webcam/style.css");
	symlink("/www/netcam_mfc_activeX.cab", "/tmp/webcam/netcam_mfc_activeX.cab");
	symlink("/var/tmp/display.jpg", "/tmp/webcam/display.jpg");

	//char *httpd_argv[]={"httpd", 
	//			nvram_safe_get("wan0_ifname"), 
	//			nvram_safe_get("usb_webhttpport_x"),
	//			NULL};
	//_eval(httpd_argv, NULL, 0, &pid);
	eval("httpd", nvram_safe_get("wan0_ifname"), nvram_safe_get("usb_webhttpport_x"));
	chdir("/");
	_eval(rcamd_argv, ">/dev/null", 0, NULL);

	return 0;
}

int
remove_webcam_main(int webflag)
{
	if (webflag == WEB_PWCWEB)
	{
		eval("rmmod", "pwc");
	}
	else
	{
		eval("rmmod", "i2c-core");
		eval("rmmod", "ov511_decomp");
		eval("rmmod", "ov518_decomp");
		eval("rmmod", "ov51x");
	}
	nvram_set("usb_webdriver_x", "");
	return 0;
}


int
remove_usb_webcam(char *product, int webflag)
{
	char pidfile[32];
	sprintf(pidfile, "/var/run/httpd-%s.pid", nvram_safe_get("usb_webhttpport_x"));

	kill_pidfile(pidfile);
	kill_pidfile("/var/run/rcamd.pid");

	if (webflag == WEB_PWCWEB)
	{
		eval("rmmod", "pwc");
	}
	else
	{
		eval("rmmod", "i2c-core");
		eval("rmmod", "ov511_decomp");
		eval("rmmod", "ov518_decomp");
		eval("rmmod", "ov51x");
	}
	nvram_set("usb_webdriver_x", "");

	return 0;
}


int
start_rcamd(void)
{
	char *rcamd_argv[] = {"rcamdmain", NULL};
	pid_t pid;

	_eval(rcamd_argv, NULL, 0, &pid);
	return 0;
}

int
stop_rcamd(void)
{
	int ret = eval("killall", "rcamdmain");
}


/*to unmount all partitions*/
int
umount_all_part(char *usbdevice)
{
	int n;
	DIR *dir_to_open;
	struct dirent *dp;
	char umount_dir[32];
	

	if(!(dir_to_open = opendir("/tmp/harddisk")))
		perror("***cannot open /tmp/harddisk\n");

	while(dir_to_open && (dp=readdir(dir_to_open)))
	{
		if(strncmp(dp->d_name, "..", NAME_MAX) == 0 ||
		strncmp(dp->d_name, ".", NAME_MAX) == 0 ||
		strncmp(dp->d_name, "part", 4) != 0)
			continue;
		
		sprintf(umount_dir, "/tmp/harddisk/%s", dp->d_name);
		if(umount(umount_dir))
		{
			//perror("umount failed");
		}
		//eval("umount", umount_dir);
	}

	if (dir_to_open)
		closedir(dir_to_open);
	
	sprintf(umount_dir, "/tmp/harddisk");

	//logs(umount_dir);

	// retry 3 times
	if(umount(umount_dir))
	{
		//perror("umount failed");
	}
	//eval("umount", umount_dir);
	return 0;
}

/* remove usb mass storage */
int
remove_usb_mass(char *product)
{
#ifdef REMOVE
	if (product!=NULL) 
	   logmessage("USB storage", product);
	else
    	   logmessage("USB storage", "NULL");
#endif

	if (product==NULL || nvram_match("usb_ftp_device", product))
	{
		eval("killall", "stupid-ftpd");
		sleep(1);
		umount_all_part("usb");
		nvram_set("usb_ftp_device", "");
		logmessage("USB storage", "removed");
	}
	return 0;
}

int mkdir_if_none(char *dir)
{
	DIR *dp;
	if(!(dp=opendir(dir)))
		return(mkdir(dir, 0777));
	closedir(dp);
	return 0;
}

int
remove_storage_main(void)
{
	remove_usb_mass(NULL);
	return 0;
}

#define MOUNT_VAL_FAIL 	0
#define MOUNT_VAL_RONLY	1
#define MOUNT_VAL_RW 	2

int
mount_r(char *usb_part, char *usb_file_part)
{
	char msg[64];

	if(!mount(usb_part, usb_file_part, "ext2", MS_SYNCHRONOUS, NULL)) 
	{
		sprintf(msg, "ext2 fs mounted to %s\n", usb_file_part);		
		logmessage("USB storage", msg);
		return MOUNT_VAL_RW;
	}
	if(!mount(usb_part, usb_file_part, "vfat", MS_SYNCHRONOUS, "codepage=950,iocharset=cp950")) 
	{
		sprintf(msg, "vfat fs mounted to %s\n", usb_file_part);		
		logmessage("USB storage", msg);
		return MOUNT_VAL_RW;
	}
	if(!mount(usb_part, usb_file_part, "msdos", MS_SYNCHRONOUS, NULL))
	{
		sprintf(msg, "msdos fs mounted to %s\n", usb_file_part);	
		logmessage("USB storage", msg);
		return MOUNT_VAL_RW;
	}
	if(!mount(usb_part, usb_file_part, "ntfs", MS_SYNCHRONOUS, "iocharset=cp950"))
	{
		sprintf(msg, "ntfs(ro) fs mounted to %s\n", usb_file_part);	
		logmessage("USB storage", msg);
		return MOUNT_VAL_RONLY;
	}
	return MOUNT_VAL_FAIL;
}

/* insert usb mass storage */
hotplug_usb_mass(char *product)
{	
	DIR *dir_to_open, *dir_of_usb, *usb_dev_disc, *usb_dev_part;
	char usb_disc[128], usb_part[128], usb_file_part[128];
	int n = 0, m;
	struct dirent *dp, *dp_disc, **dpopen;

	if (nvram_match("usb_ftpenable_x", "0")) return;

	dir_to_open=dir_of_usb=usb_dev_disc=usb_dev_part=NULL;
	
	// Mount USB to system
	if((usb_dev_disc = opendir("/dev/discs")))
	{
		while(usb_dev_disc && (dp=readdir(usb_dev_disc)))
		{
			if(!strncmp(dp->d_name, "..", NAME_MAX) || !strncmp(dp->d_name, ".", NAME_MAX) /*|| !strncmp(dp->d_name, "disc0", NAME_MAX)*/)
				continue;

			sprintf(usb_disc, "/dev/discs/%s", dp->d_name);

			//logs(usb_disc);

			if((usb_dev_part = opendir(usb_disc)))
			{
				m = 0;

				while(usb_dev_part && (dp_disc=readdir(usb_dev_part)))
				{
					//logs(dp_disc->d_name);

					if(!strncmp(dp_disc->d_name, "..", NAME_MAX) || !strncmp(dp_disc->d_name, ".", NAME_MAX) ||
!strncmp(dp_disc->d_name, "disc", NAME_MAX))
						continue;
					
					sprintf(usb_part, "/dev/discs/%s/%s", dp->d_name, dp_disc->d_name);

					if (n==0)
						sprintf(usb_file_part, "/tmp/harddisk");
					else 
						sprintf(usb_file_part, "/tmp/harddisk/part%d", n);

					mkdir_if_none(usb_file_part);
#if MOUNTALL
					eval("mount", usb_part, usb_file_part);
#else
					if (mount_r(usb_part, usb_file_part))
					{
						n++;
						m++;
					}
#endif	
				}

				if (!m) // There is no other partition
				{
										
					sprintf(usb_part, "/dev/discs/%s/disc", dp->d_name);
					if (n==0)
						sprintf(usb_file_part, "/tmp/harddisk");
					else 
						sprintf(usb_file_part, "/tmp/harddisk/part%d", n++);
#ifdef MOUNTALL
					eval("mount", usb_part, usb_file_part);
#else
					if(mount_r(usb_part, usb_file_part))
					{
						n++;	
					}
#endif
				}
			}
				
		}
	}

	if (n) 
	{
		nvram_set("usb_ftp_device", product);
		start_ftpd();
		//logmessage("USB storage", "attached");
		start_script();
		//run script if any
	}
	else
	{
		
	}

#ifdef USBCOPY_SUPPORT
	n = 1;
	if((dir_to_open = opendir("/tmp/harddisk")))
	{
		while(dir_to_open && (dp=readdir(dir_to_open)))
		{
			if(!strncmp(dp->d_name, "..", NAME_MAX) || !strncmp(dp->d_name, ".", NAME_MAX) || !strncmp(dp->d_name, "part", 4))
				continue;
			sprintf(usb_part, "/tmp/harddisk/%s", dp->d_name);
			if(scandir(usb_part, &dpopen, 0, alphasort) <= 2)
				continue;
			while(1)
			{
				sprintf(path_copy_to, "/tmp/harddisk/part1/USBpart%03d", n);
				if(!opendir(path_copy_to))
				{
					if(mkdir(path_copy_to, 0777))
					{
						perror("error on creating usb directory");
					}
					eval("echo", path_copy_to);
					break;
				}
				else
					n++;
			}
			if((dir_of_usb = opendir(usb_part)))
			{
				while(dir_of_usb && (dp_disc=readdir(dir_of_usb)))
				{
					if(!strncmp(dp_disc->d_name, "..", NAME_MAX) || !strncmp(dp_disc->d_name, ".", NAME_MAX))
						continue;
					sprintf(path_to_copy, "/tmp/harddisk/%s/%s", dp->d_name, dp_disc->d_name);
					eval("cp", "-Rf", path_to_copy, path_copy_to);
					sync();
				}
			}
			n++;
		}
	}
#endif
	if(usb_dev_disc)
		closedir(usb_dev_disc);
	if(usb_dev_part)
		closedir(usb_dev_part);
	if(dir_to_open)
		closedir(dir_to_open);

	return 0;
}

/* plugging or removing usb device */
/* usbcore, usb-ohci, usb-ehci, printer are always there */
/* usb-storage, sd_mod, scsi_mod, videodev are there if functions are enabled */
/* pwc, ov511 i2c, depends on current status */

int
hotplug_usb(void)
{
	char *action, *interface, *product, *type;
	int i;
	int isweb;
	char flag[6];

	if( !(interface = getenv("INTERFACE")) || !(action = getenv("ACTION")))
		return EINVAL;

	if ((product=getenv("PRODUCT")))
	{
		if (strncmp(interface, "1/1", 3)==0)
		{
			// if the audio device is the same with web cam,
			// just skip it
			if (nvram_match("usb_web_device", product)) 
				return 0;
			isweb = WEB_AUDIO;
			goto usbhandler;
		}
		else if(strncmp(interface, "1/", 2)==0)
			return 0;
		i=0;
		isweb = WEB_NONE;
		while(PWCLIST[i]!=NULL)
		{
			if (strstr(product, PWCLIST[i]))
			{
				isweb = WEB_PWCWEB;
				goto usbhandler;
			}
			i++;
		}
		i=0;
		while(OVLIST[i]!=NULL)
		{
			if (strstr(product, OVLIST[i]))
			{
				isweb = WEB_OVWEB;
				goto usbhandler; 
			}
			i++;
		}
	}
	else return 0;

usbhandler:	
	if (strstr(action, "add"))
	{		
		nvram_set("usb_device", "1");
		if (isweb==WEB_NONE)
		{
			if (nvram_match("usb_ftp_device", "")) // Treat it as USB storage
			{
				nvram_set("usb_storage_device", product);
			}
		}
		else if (isweb==WEB_AUDIO)
		{
			if (nvram_match("usb_audio_device", ""))
				logmessage("USB audio", "attached");
			nvram_set("usb_audio_device", product);
			refresh_wave();
		}
		else
		{
			if (nvram_match("usb_web_device", ""))	
				logmessage("USB webcam", "attached");

			sprintf(flag, "%d", isweb);
			nvram_set("usb_web_device", product);
			nvram_set("usb_web_flag", flag);
			nvram_set("usb_webdriver_x", "");
		}
	}
	else 
	{
		if (isweb==WEB_NONE) // Treat it as USB Storage
		{
			remove_usb_mass(product);
		}
		else if(isweb==WEB_AUDIO)
		{
			remove_usb_audio(product);
			nvram_set("usb_audio_device", "");
		}
		else
		{
			if (nvram_invmatch("usb_web_device", ""))
			{	
				logmessage("USB webcam", "removed");
				nvram_set("usb_web_device", "");
				nvram_set("usb_web_flag", "");
			}
			remove_usb_webcam(product, isweb);
		}
	}
	return 0;
}
#endif	

/* stop necessary services for firmware upgrade */	
int
stop_service_main(int argc, char *argv[])
{
	stop_misc();
	stop_logger();
	stop_usb();

	stop_nas();
	stop_upnp();
	stop_dhcpd();
	stop_dns();

	dprintf("done\n");
	return 0;
}

int service_handle(void)
{
	char *service;
	char tmp[100], *str;
	int unit;
	int pid;
	char *ping_argv[] = { "ping", "140.113.1.1", NULL};
	FILE *fp;

	service = nvram_get("rc_service");

	if(!service)
		kill(1, SIGHUP);

	if(strstr(service,"wan_disconnect")!=NULL)
	{
		cprintf("wan disconnect\n");

		logmessage("wan", "disconnected manually");

		if (nvram_match("wan0_proto", "dhcp") ||
			nvram_match("wan0_proto", "bigpond"))
		{		
			snprintf(tmp, sizeof(tmp), "/var/run/udhcpc%d.pid", 0);
			if ((str = file2str(tmp))) {
				pid = atoi(str);
				free(str);			
				kill(pid, SIGUSR2);
			}
		}
		else 
		{			
			stop_wan2();
		}
	}
	else if (strstr(service,"wan_connect")!=NULL)
	{
		cprintf("wan connect\n");
		logmessage("wan", "connected manually");
		setup_ethernet(nvram_safe_get("wan_ifname"));

		if (nvram_match("wan0_proto", "dhcp") ||
			nvram_match("wan0_proto", "bigpond"))
		{
			snprintf(tmp, sizeof(tmp), "/var/run/udhcpc%d.pid", 0);
			if ((str = file2str(tmp))) {
				pid = atoi(str);
				free(str);
				kill(pid, SIGUSR1);
			}
		}
		else 
		{
			// pppoe or ppptp, check if /tmp/ppp exist
			if (nvram_invmatch("wan0_proto", "static") && (fp=fopen("/tmp/ppp/ip-up", "r"))!=NULL)
			{
				fclose(fp);
				_eval(ping_argv, NULL, 0, &pid);

			}
			else
			{
				stop_wan();
				start_wan();
				_eval(ping_argv, NULL, 0, &pid);
			}
		}
	}
	nvram_unset("rc_service");
	return 0;
}

int hotplug_usb_audio(char *product)
{					
	char *wave_argv[]={"waveserver", NULL};
	pid_t pid;

	if (strlen(product)==0) return;
	_eval(wave_argv, ">/dev/null", 0, NULL);
}

int remove_usb_audio(char *product)
{
	eval("killall", "waveserver");
}

int
start_audio(void)
{
	char *wave_argv[] = {"waveservermain", NULL};
	pid_t pid;

	_eval(wave_argv, NULL, 0, &pid);
	return 0;
}

int
stop_audio(void)
{
	int ret = eval("killall", "waveserver");
}


#endif
