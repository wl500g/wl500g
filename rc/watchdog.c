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

 
#include <stdio.h>
#include <signal.h>
#include <time.h>
#include <sys/time.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <stdarg.h>
#include <sys/types.h>

#include <wlioctl.h>
#include <wlutils.h>
#include <netconf.h>
#include <nvparse.h>
#include "rc.h"

#ifdef LINUX26
 #define GPIOCTL
#else
 #undef GPIOCTL
#endif

#include "mtd.h"

extern int router_model;

#define RESET_WAIT		3		/* seconds */
#define RESET_STATE		5		/* seconds */
#define RESET_WAIT_COUNT	(RESET_WAIT * 10) /* 10 times a second */
#define RESET_STATE_COUNT	(RESET_STATE * 10)/* 10 times a second */
#define SETUP_WAIT_COUNT	3
#define SETUP_STATE_COUNT	5

#define NORMAL_PERIOD		1		/* second */
#define URGENT_PERIOD		100 * 1000	/* microsecond */	
						/* 1/10 second */
#define STACHECK_PERIOD_CONNECT 60/5		/* 30 seconds */
#define STACHECK_PERIOD_DISCONNECT 5/5		/* 5 seconds */

#define GPIO0  0x00000001
#define GPIO1  0x00000002
#define GPIO2  0x00000004
#define GPIO3  0x00000008
#define GPIO4  0x00000010
#define GPIO5  0x00000020
#define GPIO6  0x00000040
#define GPIO7  0x00000080
#define GPIO8  0x00000100
#define GPIO9  0x00000200
#define GPIO10 0x00000400
#define GPIO11 0x00000800
#define GPIO12 0x00001000
#define GPIO13 0x00002000
#define GPIO14 0x00004000
#define GPIO15 0x00008000
#define GPIO16 0x00010000
#define GPIO17 0x00020000
#define GPIO18 0x00040000
#define GPIO19 0x00080000
#define GPIO20 0x00100000
#define GPIO21 0x00200000
#define GPIO22 0x00400000
#define GPIO23 0x00800000

static int reset_mask = 0;
static int reset_value = 0;
static int ready_mask = 0;	/* Ready or Power LED */
static int ready_value = 0;
static int setup_mask = 0;	/* EZ-Setup/WPS button */
static int setup_value = 0;
static int power_mask = 0;	/* POWER button light */
static int power_value = 0;

#ifdef GPIOCTL
 #include <sys/ioctl.h>
 #include <linux_gpio.h>
 #define GPIO_DEV_IN	GPIO_IOC_IN
 #define GPIO_DEV_OUT	GPIO_IOC_OUT
 #define GPIO_DEV_OUTEN	GPIO_IOC_OUTEN
#else
 #define GPIO_DEV_IN	"/dev/gpio/in"
 #define GPIO_DEV_OUT	"/dev/gpio/out"
 #define GPIO_DEV_OUTEN	"/dev/gpio/outen"
#endif

#define LED_READY_ON 	(ready_value)
#define LED_READY_OFF	(~ready_value)
#define LED_READY	ready_mask

#define LED_CONTROL(led,flag) gpio_write(GPIO_DEV_OUT, led, flag)

static struct itimerval itv; 
static long sync_interval = -1; // every 30 seconds a unit
static int stacheck_interval = -1;

/* Private et.o ioctls */
#define SIOCGETCROBORD		(SIOCDEVPRIVATE + 14)
#define SIOCSETCROBOWR		(SIOCDEVPRIVATE + 15)
static int wan_phystatus = 1;	// assume it's was up before start
static int wan_phyport = -1;

/* forwards */
#ifdef __CONFIG_RCAMD__
static int notice_rcamd(int flag);
#endif


#ifdef GPIOCTL

static int tgpio_open()
{
	int fd = open("/dev/gpio", O_RDWR);
	if (fd < 0)
		printf ("Failed to open /dev/gpio\n");
	return fd;
}

static int tgpio_ioctl(int fd, int gpioreg, unsigned int mask , unsigned int value)
{
	struct gpio_ioctl gpio;

	gpio.val = value;
	gpio.mask = mask;

	if (ioctl(fd, gpioreg, &gpio) < 0) {
		printf ("Invalid gpioreg %d\n", gpioreg);
		return -1;
	}
	return gpio.val;
}

static unsigned int tgpio_multi(int fd, int gpioreg, unsigned int mask, unsigned int value)
{
	unsigned int bit, val = 0;
	int i;

	for (i = 0; mask >> i; i++) {
		if ((bit = mask & (1 << i)) && /* enabled */
		    tgpio_ioctl(fd, gpioreg, bit, value & bit) >= 0)
			val |= bit;
	}
	return val;
}

void gpio_write(int gpioreg, unsigned int mask, unsigned int value)
{
	int fd = tgpio_open();
	if (fd != -1)
	{
		tgpio_multi(fd, GPIO_IOC_RESERVE, mask, mask);
		tgpio_ioctl(fd, gpioreg, mask, value & mask);
		close(fd);
	}
}

unsigned int gpio_read(int gpioreg, unsigned int mask)
{
	unsigned int val = 0;
	int fd = tgpio_open();
	if (fd != -1)
	{
		val = tgpio_ioctl(fd, gpioreg, mask, 0);
		close(fd);
	}
	return val & mask;
}
#else /* GPIOCTL */

void gpio_write(const char *dev, unsigned int mask, unsigned int value)
{
	unsigned long val;
	int fd = open(dev, O_RDWR);
	if (fd != -1)
	{
		if (read(fd, &val, sizeof(val)) == sizeof(val))
		{
			val = (val & ~mask) | (value & mask);
			write(fd, &val, sizeof(val));
		}
		close(fd);
	}
}

unsigned int gpio_read(const char *dev, unsigned int mask)
{
	unsigned int val = 0;
	int fd = open(dev, O_RDONLY);
	if (fd != -1)
	{
		read(fd, &val, sizeof(val));
		close(fd);
	}
	return val & mask;
}
#endif /* !GPIOCTL */

/* Functions used to control led and button */
void gpio_init(void)
{
	// overrides based on nvram
	switch (router_model) {
		case MDL_MN700:
			reset_mask = GPIO7, reset_value = 0;
			ready_mask = GPIO6, ready_value = GPIO6;
			break;
		case MDL_WL500GX:
			reset_mask = GPIO6, reset_value = GPIO6;
			ready_mask = GPIO0, ready_value = 0;
			setup_mask = 0, setup_value = 0;
			break;
//		case MDL_WL320GE:
//		case MDL_WL320GP:
		case MDL_WL550GE:
			reset_mask = GPIO1, reset_value = GPIO1;
			ready_mask = GPIO2, ready_value = 0;
			setup_mask = GPIO15, setup_value = GPIO15;
			break;
		case MDL_WL500GP:
			reset_mask = GPIO0, reset_value = GPIO0;
			ready_mask = GPIO1, ready_value = 0;
			setup_mask = GPIO4, setup_value = GPIO4;
			break;
		case MDL_WL500W:
			reset_mask = GPIO6, reset_value = GPIO6;
			ready_mask = GPIO5, ready_value = 0;
			setup_mask = GPIO7, setup_value = GPIO7;
			/* -robo_reset is GPIO0 */
			break;
		case MDL_WL700G:
			/* copy is GPIO6, ez is GPIO4, pwr-btn is GPIO0 */
			/* hwpower is GPIO7, pwr is GPIO3, -ready is GPIO1 */
			/* GPIO2 is SDA, GPIO5 is SCL */
			reset_mask = GPIO0, reset_value = GPIO0;
			ready_mask = GPIO1, ready_value = 0;
			/* enable copy button too */
			setup_mask = GPIO4 | GPIO6, setup_value = GPIO4;
			power_mask = GPIO3, power_value = GPIO3;
			break;
		case MDL_WL520GU:
		case MDL_WL330GE:
		case MDL_WL500GPV2:
			reset_mask = GPIO2, reset_value = 0;
			ready_mask = GPIO0, ready_value = 0;
			setup_mask = GPIO3, setup_value = 0;
			break;
		case MDL_DIR320:
			reset_mask = GPIO7, reset_value = 0;
			ready_mask = GPIO1, ready_value = GPIO1;
			setup_mask = GPIO6, setup_value = 0;
			break;
		case MDL_RTN16:
			reset_mask = GPIO6, reset_value = 0;
			ready_mask = GPIO1, ready_value = 0;
			setup_mask = GPIO8, setup_value = 0;
			break;
		case MDL_RTN15U:
			reset_mask = GPIO5, reset_value = 0;
			ready_mask = GPIO6, ready_value = 0;
			setup_mask = GPIO8, setup_value = 0;
			break;
		case MDL_RTN12:
			reset_mask = GPIO1, reset_value = 0;
			ready_mask = GPIO2, ready_value = 0;
			setup_mask = GPIO0, setup_value = 0;
			break;
		case MDL_RTN12B1:
		case MDL_RTN12C1:
			reset_mask = GPIO22, reset_value = 0;
			ready_mask = GPIO18, ready_value = 0;
			setup_mask = GPIO23, setup_value = 0;
			break;
		case MDL_RTN10:
			reset_mask = GPIO3, reset_value = 0;
			ready_mask = GPIO1, ready_value = 0;
			setup_mask = GPIO2, setup_value = 0;
			break;
		case MDL_RTN10U:
			reset_mask = GPIO21, reset_value = 0;
			ready_mask = GPIO6, ready_value = 0;
			setup_mask = GPIO20, setup_value = 0;
			break;
		case MDL_WNR3500L:
			reset_mask = GPIO4, reset_value = 0;
			ready_mask = GPIO3, ready_value = GPIO3;
			setup_mask = GPIO6, setup_value = 0;
			break;
	}
	gpio_write(GPIO_DEV_OUTEN, ready_mask | power_mask |
		reset_mask | setup_mask, ready_mask | power_mask);
#ifndef GPIOCTL
	gpio_write("/dev/gpio/control", ready_mask | power_mask |
		reset_mask | setup_mask, 0);
#endif
}

static int phy_status(const char *wan_if, int port)
{
	struct ifreq ifr;
	int fd;
	int args[2];
	int ret = 0;

	memset(&ifr, 0, sizeof(ifr));
	strncpy(ifr.ifr_name, wan_if, sizeof(ifr.ifr_name));

	fd = socket(AF_INET, SOCK_DGRAM, 0);
	if (fd < 0)
		return ret;

	args[0] = 0x10000;
	ifr.ifr_data = (caddr_t) args;
	if (ioctl(fd, SIOCGETCROBORD, (caddr_t)&ifr) < 0) {
		perror("ioctl(SIOCGETCROBORD)");
		goto error;
	}

	if (args[1] & (1 << port))
		ret = 1;

error:
	close(fd);
	return ret;
}

static void alarmtimer(unsigned long sec, unsigned long usec)
{
	itv.it_value.tv_sec  = sec;
	itv.it_value.tv_usec = usec;
	itv.it_interval = itv.it_value;
	setitimer(ITIMER_REAL, &itv, NULL);
}

void btn_check(void)
{
	static int pressed;

	if (gpio_read(GPIO_DEV_IN, reset_mask) == reset_value)
	{
		/* Whenever it is pushed steady */
		if (++pressed < RESET_WAIT_COUNT) {
			if (pressed == 1) alarmtimer(0, URGENT_PERIOD);
		} else {
			switch ((pressed - RESET_WAIT_COUNT) / RESET_STATE_COUNT) {
			case 0: /* power off indication */
#if defined(MODEL_WL700G)
				LED_CONTROL(LED_READY, LED_READY_OFF);
				break;
#endif
			case 1:	/* restore to defaults indication */
				LED_CONTROL(LED_READY, (pressed & 1) ? LED_READY_OFF : LED_READY_ON);
				break;
			default: /* start over */
				pressed = 0;
				LED_CONTROL(LED_READY, LED_READY_ON);
				break;
			}
		}
	}
	else
	{
		if (pressed < RESET_WAIT_COUNT) {
			if (pressed) {
				alarmtimer(NORMAL_PERIOD, 0);
				LED_CONTROL(LED_READY, LED_READY_ON);
			}
		} else {
			switch ((pressed - RESET_WAIT_COUNT) / RESET_STATE_COUNT) {
			case 0: /* power off */
#if defined(MODEL_WL700G)
				alarmtimer(0, 0);
				kill(1, SIGQUIT);
				break;
#endif
			case 1:	/* restore to defaults */
				alarmtimer(0, 0);
				eval("erase", "nvram");
				kill(1, SIGTERM);
				break;
			}
		}		
		pressed = 0;
	}
}

void setup_check(void)
{
	static int pressed;

	if (gpio_read(GPIO_DEV_IN, setup_mask) == setup_value)
	{
		/* Whenever it is pushed steady */
		if (++pressed < SETUP_WAIT_COUNT) {
			//if (pressed == 1) alarmtimer(0, URGENT_PERIOD);
		} else {
			switch ((pressed - SETUP_WAIT_COUNT) / SETUP_STATE_COUNT) {
			case 0:	/* action indication */
				LED_CONTROL(LED_READY, (pressed & 1) ? LED_READY_OFF : LED_READY_ON);
				break;
			default: /* start over */
				pressed = 0;
				LED_CONTROL(LED_READY, LED_READY_ON);
				break;
			}
		}
	}
	else
	{
		if (pressed < SETUP_WAIT_COUNT) {
			if (pressed) {
				//alarmtimer(NORMAL_PERIOD, 0);
				LED_CONTROL(LED_READY, LED_READY_ON);
			}
		} else {
			switch ((pressed - SETUP_WAIT_COUNT) / SETUP_STATE_COUNT) {
			case 0: /* action */
				//alarmtimer(0, 0);
				LED_CONTROL(LED_READY, LED_READY_ON);
				eval("/usr/local/sbin/ez-setup");
				break;
			}
		}		
		pressed = 0;
	}
}

void inline refresh_ntpc(void)
{
	stop_ntpc();
	start_ntpc();
	//printf("Sync time %dn", sync_interval);
}

int ntp_timesync(void)
{
	//if (nvram_match("router_disable", "1")) return 0;
	
	if (sync_interval!=-1)
	{
        	sync_interval--;

	    	if (sync_interval==0)
		{
			time_t now;
			struct tm tm;

			now = update_tztime(0);
			localtime_r(&now, &tm);

			if (tm.tm_year>100) // More than 2000
			{
				sync_interval = (nvram_get_int("ntp_interval_x") ? : 2) * 360;
				logmessage("ntp client", "Synchronizing time with %s...", nvram_safe_get("ntp_server0"));

				//stop_upnp();
				//start_upnp();
			}
			else sync_interval = 6;		/* Once per 60s */

			refresh_ntpc();	
		}
	}
	return 0;
}

enum ACTIVE
{
	URLACTIVE=0,
	WEBACTIVE,
	RADIOACTIVE,
	ACTIVEITEMS
};

static int svcStatus[ACTIVEITEMS] = { -1, -1, -1 };
static int extStatus[ACTIVEITEMS] = { 0, 0, 0 };
static char svcDate[ACTIVEITEMS][10];
static char svcTime[ACTIVEITEMS][20];

static int timecheck_item(char *activeDate, char *activeTime)
{
	#define DAYSTART (0)
	#define DAYEND (60*60*23+60*59+59) //86399
	int current, active, activeTimeStart, activeTimeEnd;
	time_t now;
	struct tm *tm;

	time(&now);
	tm = localtime(&now);
	current = tm->tm_hour*60 + tm->tm_min;

	active=0;

	//printf("active: %s %s\n", activeDate, activeTime);

	activeTimeStart=((activeTime[0]-'0')*10+(activeTime[1]-'0'))*60 + (activeTime[2]-'0')*10 + (activeTime[3]-'0');
		
	activeTimeEnd=((activeTime[4]-'0')*10+(activeTime[5]-'0'))*60 + (activeTime[6]-'0')*10 + (activeTime[7]-'0');
				
	if (activeDate[tm->tm_wday] == '1')
	{
		if (activeTimeEnd<activeTimeStart)
		{
			if ((current>=activeTimeStart && current<=DAYEND) ||
			   (current>=DAYSTART && current<=activeTimeEnd))
			{
				active = 1;
			}
			else
			{
				active = 0;
			}
		}
		else
		{
			if (current>=activeTimeStart &&
		     	current<=activeTimeEnd)
			{	
				active = 1;
			}	
			else 
			{
				active = 0;
			}
		}			
	}
	return(active);
}

/* Check for time-dependent service 	*/
/* 1. URL filter 			*/
/* 2. WEB Camera Security filter 	*/

static int svc_timecheck(void)
{
	int activeNow;

#ifdef __CONFIG_RCAMD__
	if (svcStatus[WEBACTIVE] == -1 && 
		nvram_invmatch("usb_webenable_x", "0") &&
		nvram_invmatch("usb_websecurity_x", "0"))
	{	
		strcpy(svcDate[WEBACTIVE], nvram_safe_get("usb_websecurity_date_x"));
		strcpy(svcTime[WEBACTIVE], nvram_safe_get("usb_websecurity_time_x"));
		svcStatus[WEBACTIVE] = -2;
	}

	if (svcStatus[WEBACTIVE] != -1)
	{
		activeNow = timecheck_item(svcDate[WEBACTIVE], svcTime[WEBACTIVE]);
		if (activeNow != svcStatus[WEBACTIVE])
		{
			svcStatus[WEBACTIVE] = activeNow;

			if (!notice_rcamd(svcStatus[WEBACTIVE])) svcStatus[WEBACTIVE] = -1;
		}
	}
#endif /* __CONFIG_RCAMD__ */

	if (svcStatus[RADIOACTIVE] == -1 && nvram_invmatch("wl_radio_x", "0"))
	{
		strcpy(svcDate[RADIOACTIVE], nvram_safe_get("wl_radio_date_x"));
		strcpy(svcTime[RADIOACTIVE], nvram_safe_get("wl_radio_time_x"));
		svcStatus[RADIOACTIVE] = -2;
	}


	if (svcStatus[RADIOACTIVE] != -1)
	{
		activeNow = timecheck_item(svcDate[RADIOACTIVE], svcTime[RADIOACTIVE]);
		if (activeNow != svcStatus[RADIOACTIVE])
		{
			svcStatus[RADIOACTIVE] = activeNow;

			if (activeNow)
				eval("wl", "radio", "on");
			else
				eval("wl", "radio", "off");
		}
	}

	//printf("svc : %d %d %d\n", svcStatus[0], svcStatus[1], svcStatus[2]);
	return 0;
}	

/* Sometimes, httpd becomes inaccessible, try to re-run it */
static int http_processcheck(void)
{
//	char http_cmd[32];
//	char buf[256];
	const char *httpd_pid = "/var/run/httpd.pid";

	//printf("http check\n");

	/* just check httpd process existance */
	//sprintf(http_cmd, "http://127.0.0.1:%s/", nvram_safe_get("http_lanport"));
	//if (!http_check(http_cmd, buf, sizeof(buf), 0))
	if (!proc_check_pid(httpd_pid))
	{
		dprintf("http rerun\n");
		kill_pidfile(httpd_pid);
		//stop_httpd();
		start_httpd();
	}

	return 0;
}


#ifdef USB_SUPPORT

#if defined(__CONFIG_MADWIMAX__) || defined(__CONFIG_MODEM__)
int usb_communication_device_processcheck(int wait_flag)
{
	const char *wan_ifname, *wan_proto;
	char tmp[100], prefix[sizeof("wanXXXXXXXXXX_")];
	int unit, enable;

	hotplug_sem_open();
	if (wait_flag) {
		enable = 1;
		hotplug_sem_lock();
	} else {
		enable = hotplug_sem_trylock();
	}
	if (enable) {
		/* Start each configured and enabled wan connection and its undelying i/f */
		for ( unit=0; unit<MAX_NVPARSE; unit++) 
		{
			snprintf(prefix, sizeof(prefix), "wan%d_", unit);

			/* make sure the connection exists and is enabled */ 
			wan_ifname = nvram_get(strcat_r(prefix, "ifname", tmp));
			if (!wan_ifname)
				continue;

			wan_proto = nvram_get(strcat_r(prefix, "proto", tmp));
			if (!wan_proto || !strcmp(wan_proto, "disabled"))
				continue;
#ifdef __CONFIG_MADWIMAX__
			if (!strcmp(wan_proto, "wimax")) madwimax_check(prefix);
#endif
#ifdef __CONFIG_MODEM__
			if (!strcmp(wan_proto, "usbmodem")) usb_modem_check(prefix);
#endif
		}
		hotplug_sem_unlock();
	}
	hotplug_sem_close();

	return 0;
}
#endif

#ifdef __CONFIG_RCAMD__
static int notice_rcamd(int flag)
{
	int ret = 0;

//TODO: mjpg-streamer start/stop control
//	ret = kill_pidfile_s("/var/run/rcamd.pid", flag ? SIGUSR1 : SIGUSR2);

	return (ret == 0);
}

static int refresh_rcamd(void)
{
	if (kill_pidfile_s("/var/run/rcamd.pid", SIGUSR1) == 0)
	{
		unlink("/var/run/rcamd.pid");
	}
	else
	{
		killall("mjpg_streamer");
	}

	return hotplug_usb_webcam(nvram_safe_get("usb_web_device"));
}
#endif /* __CONFIG_RCAMD__ */


#endif /* USB_SUPPORT */

static void catch_sig(int sig)
{
	if (sig == SIGUSR1)
	{
		dprintf("Catch Reset to Default Signal\n");
		//sys_default();
		//set_pid(getpid());	
	}
	else if (sig == SIGUSR2)
	{
		dprintf("Get Signal: %d %d %d\n", svcStatus[WEBACTIVE], extStatus[WEBACTIVE], sig);

#ifdef __CONFIG_RCAMD__
		FILE *fp;
		char command[256], *cmd_ptr;

		if (!svcStatus[WEBACTIVE]) return;

		if (extStatus[WEBACTIVE]==0)
		{
			fp = fopen("/var/tmp/runshell", "r+");
			if (fp!=NULL)
			{
				cmd_ptr = fgets(command, 256, fp);

				if (cmd_ptr!=NULL) system(command);
			}
			fclose(fp);
			unlink("/tmp/runshell");
			notice_rcamd(0);
			extStatus[WEBACTIVE]=1;
		}
		else if (svcStatus[WEBACTIVE]==1)
		{
			notice_rcamd(1);
			extStatus[WEBACTIVE] = 0;
		}
#endif /* __CONFIG_RCAMD__ */
	}
}

static void sta_check(void)
{
	int ret;
	const char *wl_ifname = nvram_safe_get("wl0_ifname");
	char bssid[32];

	if (stacheck_interval == -1)
	{
		if (nvram_invmatch("wl0_mode", "sta") && 
            		nvram_invmatch("wl0_mode", "wet")) return;

		stacheck_interval = STACHECK_PERIOD_DISCONNECT;
	}
	
	stacheck_interval--;

	if (stacheck_interval) return;

	// Get bssid
	ret = wl_ioctl(wl_ifname, WLC_GET_BSSID, bssid, sizeof(bssid));

	if (ret==0 && memcmp(bssid, "\0\0\0\0\0\0", 6))
	{
		dprintf("connected\n");
		stacheck_interval = STACHECK_PERIOD_CONNECT;
	}
	else 
	{
		dprintf("disconnected\n");
		stacheck_interval = STACHECK_PERIOD_DISCONNECT;

		dprintf("connect: [%s]\n", nvram_safe_get("wl0_join"));
		system(nvram_safe_get("wl0_join"));
		//eval("wl", "join", nvram_safe_get("wl0_ssid"));
	}
	return;
}

static void link_check(void)
{
	int unit = 0;
	int status;

	/* Skip if wan port is unknown/not set */
	if (wan_phyport < 0)
		return;

	status = phy_status("eth0", wan_phyport);
	if (wan_phystatus == status)
		return;
	wan_phystatus = status;

	if (status)
	{
		char tmp[100], prefix[sizeof("wanXXXXXXXXXX_")];
		const char *wan_proto;

		sprintf(prefix, "wan%d_", unit);
		wan_proto = nvram_safe_get(strcat_r(prefix, "proto", tmp));

#ifdef __CONFIG_EAPOL__
		if (nvram_match(strcat_r(prefix, "auth_x", tmp), "eap-md5")
		&& (strcmp(wan_proto, "static") == 0 ||
		    strcmp(wan_proto, "dhcp") == 0))
			killall_s("wpa_supplicant", SIGUSR2);
		else
#endif
		if (strcmp(wan_proto, "dhcp") == 0 ||
		    strcmp(wan_proto, "bigpond") == 0)
		{
			snprintf(tmp, sizeof(tmp), "/var/run/udhcpc%d.pid", unit);
			kill_pidfile_s(tmp, SIGUSR1);
		}
	}

	logmessage("WAN port", "cable %s", status ? "connected" : "disconnected");
}

/* wathchdog is runned in NORMAL_PERIOD, 1 seconds
 * check in each NORMAL_PERIOD
 *	1. button
 *
 * check in each NORAML_PERIOD*5
 *
 *      1. ntptime, 
 *      2. time-dependent service
 *      3. http-process
 *      4. usb hotplug status
 */
static void watchdog(int signum)
{
	static int watchdog_period = 0;
	time_t now;

	/* handle button */
	if (reset_mask) btn_check();
	/* handle ezsetup */
	if (setup_mask) setup_check();

	/* if timer is set to less than 1 sec, then bypass the following */
	if (itv.it_value.tv_sec == 0)
		return;

	/* handle WAN port link */
	link_check();

	watchdog_period = (watchdog_period+1) %10;
	if (watchdog_period) return;

	time(&now);
	//printf("now: %d\n", (long )now);

	/* sync time to ntp server if necessary */
	ntp_timesync();

	/* check for time-dependent services */
	svc_timecheck();

	/* http server check */
	http_processcheck();

#ifdef USB_SUPPORT

#if defined(__CONFIG_MADWIMAX__) || defined(__CONFIG_MODEM__)
	/* madwimax and 3g/cdma process */
	usb_communication_device_processcheck(0);
#endif

#ifdef __CONFIG_RCAMD__
	/* web cam process */
	if (nvram_invmatch("usb_web_device", "") && nvram_invmatch("usb_webenable_x", "0"))
	{	
		if (nvram_invmatch("usb_webdriver_x", ""))
		{
			if (!proc_check_pid("/var/run/rcamd.pid"))
			{
				refresh_rcamd();
			}
		}				
		else
		{	
			// reset WEBCAM status	
			refresh_rcamd();
			svcStatus[WEBACTIVE] = -1;
		}
	}
#endif /* __CONFIG_RCAMD__ */

	/* storage process */
	if (nvram_invmatch("usb_storage_device", ""))
	{
		hotplug_usb_mass(nvram_safe_get("usb_storage_device"));
		nvram_set("usb_storage_device", "");
	}
#endif /* USB_SUPPORT */

	/* station or ethernet bridge handler */
	sta_check();
}

int poweron_main(int argc, char *argv[])
{
	/* Start GPIO function */
	gpio_init();
	
	if (power_mask) {
		gpio_write(GPIO_DEV_OUT, power_mask, power_value);
		/* sleep to allow hdd to spin up */
		sleep(2);
	}
	
	return 0;
}

static volatile int running = 1;

static void readyoff(int sig)
{
	gpio_write(GPIO_DEV_OUT, ready_mask, ~ready_value);
	running = 0;
}

int watchdog_main()
{
	FILE *fp;

#ifdef REMOVE
	/* Run it under background */
	switch (fork()) {
	case -1:
		exit(0);
		break;
	case 0:	
		// start in a new session	
		(void) setsid();
		break;
	default:	
		/* parent process should just die */
		_exit(0);
	}
#endif

	/* write pid */
	if ((fp=fopen("/var/run/watchdog.pid", "w")) != NULL)
	{
		fprintf(fp, "%d", getpid());
		fclose(fp);
	}

	/* Start GPIO function */
	gpio_init();

	/* set the signal handler */
	signal(SIGUSR1, catch_sig);
	signal(SIGUSR2, catch_sig);
	signal(SIGALRM, watchdog);
	signal(SIGTERM, readyoff);
	signal(SIGCHLD, child_reap);

	/* turn on POWER and READY LEDs */
	gpio_write(GPIO_DEV_OUT, power_mask | ready_mask,
		power_value | ready_value);

	/* Start sync time */
	sync_interval = 1;
	start_ntpc();

	/* Is WAN port link required */
	if (!nvram_match("router_disable", "1"))
		wan_phyport = ethernet_port(nvram_safe_get("wan_ifname"));

	/* set timer */
	alarmtimer(NORMAL_PERIOD, 0);

	/* Most of time it goes to sleep */
	while (running)
	{
		pause();
	}
	
	return 0;
}
