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
#include <sys/types.h>

#include <bcmnvram.h>
#include <shutils.h>
#include <rc.h>
#include <stdarg.h>
#include <wlioctl.h>

#if 0 
#define DEBUG printf
#else
#define DEBUG(format, args...) 
#endif

#define BCM47XX_SOFTWARE_RESET  0x40		/* GPIO 6 */
#define RESET_WAIT		3		/* seconds */
#define RESET_WAIT_COUNT	RESET_WAIT * 10 /* 10 times a second */

#define NORMAL_PERIOD		1		/* second */
#define URGENT_PERIOD		100 * 1000	/* microsecond */	
						/* 1/10 second */
#define STACHECK_PERIOD_CONNECT 60/5		/* 30 seconds */
#define STACHECK_PERIOD_DISCONNECT 5/5		/* 5 seconds */

#define GPIO0 0x0001
#define GPIO1 0x0002
#define GPIO2 0x0004
#define GPIO3 0x0008
#define GPIO4 0x0010
#define GPIO5 0x0020
#define GPIO6 0x0040
#define GPIO7 0x0080

#define LED_ON 	0
#define LED_OFF 1

#define LED_POWER	GPIO0
#define BTN_RESET	GPIO6
#define LED_CONTROL(led,flag) gpio_write("/dev/gpio/out", led, flag)

struct itimerval itv; 
int watchdog_period=0;
static int btn_pressed=0;
static int btn_count = 0;
long sync_interval=-1; // every 30 seconds a unit
int sync_flag=0;
long timestamp_g=0;
int stacheck_interval=-1;


void gpio_write(char *dev, int gpio, int flag)
{
	unsigned int val;
	FILE *fp;	

	val = gpio_read(dev, 0xffff);

	fp=fopen(dev, "w");
	if (fp!=NULL)
	{
		if (flag) val|=gpio;
		else val&=~gpio;

		fwrite(&val, 4, 1, fp);
		fclose(fp);
	}	
}

int gpio_read(char *dev, int gpio)
{
	unsigned int val;
	FILE *fp;

	fp=fopen(dev, "r");

	if (fp!=NULL)
	{
		fread(&val, 4, 1, fp);
		fclose(fp);
		return(val&gpio);		
	}
	return 0;
}


/* Functions used to control led and button */
gpio_init()
{
	unsigned int outen;

	// gpio 0 as output
	// gpio 6 as input
	gpio_write("/dev/gpio/outen", LED_POWER, 1);
	gpio_write("/dev/gpio/outen", BTN_RESET, 0);
}

static void
alarmtimer(unsigned long sec,unsigned long usec)
{
	itv.it_value.tv_sec  = sec;
	itv.it_value.tv_usec = usec;
	itv.it_interval = itv.it_value;
	setitimer(ITIMER_REAL, &itv, NULL);
}

void btn_check(void)
{
	//printf("btn :  %d %d\n", btn_pressed, btn_count);	
	if (gpio_read("/dev/gpio/in", BTN_RESET))
	{
		if (!btn_pressed)
		{
			btn_pressed=1;
			btn_count=0;
			alarmtimer(0, URGENT_PERIOD);
		}
		else {	/* Whenever it is pushed steady */
			if( ++btn_count > RESET_WAIT_COUNT )
			{
				btn_pressed=2;
			}

			if (btn_pressed==2)
			{
				/* 0123456789 */
				/* 0011100111 */
				if ((btn_count%10)<1 ||
				    ((btn_count%10)>4 && (btn_count%10)<7)) LED_CONTROL(LED_POWER, LED_OFF);
				else LED_CONTROL(LED_POWER, LED_ON);


			}
		}
	}
	else
	{
		if(btn_pressed==1)
		{
			btn_count = 0;
			btn_pressed = 0;			
			LED_CONTROL(LED_POWER, LED_ON);
			alarmtimer(NORMAL_PERIOD, 0);
		}
		else if(btn_pressed==2)
		{
			alarmtimer(0, 0);
			eval("erase", "/dev/mtd/3");
			kill(1, SIGTERM);
		}
	}
}

void refresh_ntpc(void)
{
	eval("killall","ntpclient");
	kill_pidfile_s("/var/run/ntp.pid", SIGUSR1);	
	//printf("Sync time %dn", sync_interval);
}

int ntp_timesync(void)
{
	time_t now;
	struct tm tm;	
	struct tm gm, local;
	struct timezone tz;

	//if (nvram_match("router_disable", "1")) return 0;
	
	if (sync_interval!=-1)
	{
        	sync_interval--;

	    	if (sync_interval==0)
		{
			/* Update kernel timezone */
			setenv("TZ", nvram_safe_get("time_zone"), 1);
			time(&now);
			gmtime_r(&now, &gm);
			localtime_r(&now, &local);
			tz.tz_minuteswest = (mktime(&gm) - mktime(&local)) / 60;
			settimeofday(NULL, &tz);
		   	memcpy(&tm, localtime(&now), sizeof(struct tm));

		   	if (tm.tm_year>100) // More than 2000 
		   	{	 
		      		sync_interval=60*60/5;
			  	logmessage("ntp client", "time is synchronized to %s", nvram_safe_get("ntp_servers"));

				stop_upnp();
				start_upnp();
		   	}	
		  	else sync_interval=1;
	
			refresh_ntpc();	
		}
	}	
}

enum 
{
	URLACTIVE=0,
	WEBACTIVE,
	RADIOACTIVE,
	ACTIVEITEMS
} ACTIVE;

int svcStatus[ACTIVEITEMS] = { -1, -1, -1};
int extStatus[ACTIVEITEMS] = { 0, 0, 0};
char svcDate[ACTIVEITEMS][10];
char *svcTime[ACTIVEITEMS][20];

int timecheck_item(char *activeDate, char *activeTime)
{
	#define DAYSTART (0)
	#define DAYEND (60*60*23+60*59+59) //86399
	int current, active, activeTimeStart, activeTimeEnd, i;
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

int svc_timecheck(void)
{
	int activeFlag, activeNow;

	activeFlag = 0;

	/* Initialize */
	if (svcStatus[URLACTIVE]==-1 && nvram_invmatch("url_enable_x", "0"))
	{
		strcpy(svcDate[URLACTIVE], nvram_safe_get("url_date_x"));
		strcpy(svcTime[URLACTIVE], nvram_safe_get("url_time_x"));
		svcStatus[URLACTIVE] = -2;
	}	

	if (svcStatus[URLACTIVE]!=-1)
	{
		activeNow = timecheck_item(svcDate[URLACTIVE], svcTime[URLACTIVE]);	
		if (activeNow!=svcStatus[URLACTIVE])
		{
			//printf("url time change: %d\n", activeNow);
			svcStatus[URLACTIVE] = activeNow;
			stop_dns();
			start_dns();
		}
	}

	if (svcStatus[WEBACTIVE]==-1 && 
		nvram_invmatch("usb_webenable_x", "0") &&
		nvram_invmatch("usb_websecurity_x", "0"))
	{	
		strcpy(svcDate[WEBACTIVE], nvram_safe_get("usb_websecurity_date_x"));
		strcpy(svcTime[WEBACTIVE], nvram_safe_get("usb_websecurity_time_x"));
		svcStatus[WEBACTIVE] = -2;
	}

	if (svcStatus[WEBACTIVE]!=-1)
	{
		activeNow = timecheck_item(svcDate[WEBACTIVE], svcTime[WEBACTIVE]);
		if (activeNow!=svcStatus[WEBACTIVE])
		{
			svcStatus[WEBACTIVE] = activeNow;

			if (!notice_rcamd(svcStatus[WEBACTIVE])) svcStatus[WEBACTIVE]=-1;
		}	
	}

	if (svcStatus[RADIOACTIVE]==-1 && nvram_invmatch("wl_radio_x", "0"))
	{	
		strcpy(svcDate[RADIOACTIVE], nvram_safe_get("wl_radio_date_x"));
		strcpy(svcTime[RADIOACTIVE], nvram_safe_get("wl_radio_time_x"));
		svcStatus[RADIOACTIVE] = -2;
	}


	if (svcStatus[RADIOACTIVE]!=-1)
	{
		activeNow = timecheck_item(svcDate[RADIOACTIVE], svcTime[RADIOACTIVE]);
		if (activeNow!=svcStatus[RADIOACTIVE])
		{
			svcStatus[RADIOACTIVE] = activeNow;

			if (activeNow) eval("wl", "radio", "on");
			else eval("wl", "radio", "off");
		}
	}

	//printf("svc : %d %d %d\n", svcStatus[0], svcStatus[1], svcStatus[2]);
	return 0;
}	
	
/* Sometimes, httpd becomes inaccessible, try to re-run it */
int http_processcheck(void)
{
	char http_cmd[32];
	char buf[256];

	//printf("http check\n");
	sprintf(http_cmd, "http://127.0.0.1/");
	if (!http_check(http_cmd, buf, sizeof(buf), 0))
	{
		dprintf("http rerun\n");
		kill_pidfile("/var/run/httpd.pid");
		//stop_httpd();
		start_httpd();
	}

	if (nvram_invmatch("usb_webdriver_x", ""))
	{						
		sprintf(http_cmd, "http://127.0.0.1:%s/", nvram_safe_get("usb_webhttpport_x"));
		//logmessage("webcam", "webcam httpd die checking %s\n", http_cmd);

		if (!http_check(http_cmd, buf, sizeof(buf), 0))
		{
			dprintf("http rerun\n");
			sprintf(buf, "/var/run/httpd-%s.pid", nvram_safe_get("usb_webhttpport_x"));
			kill_pidfile(buf);
			//logmessage("webcam", "webcam httpd rerun\n");

			chdir("/tmp/webcam");
			eval("httpd", nvram_safe_get("wan0_ifname"), nvram_safe_get("usb_webhttpport_x"));
			chdir("/");
		}
	}
	return 0;
}

#ifdef USB_SUPPORT
char usbinterrupt[128];

int rcamd_processcheck()
{
	FILE *fp;
	char buf[128];
	int flag=1;

	fp = fopen("/proc/interrupts", "r");

	if (fp!=NULL)
	{
		while(fgets(buf, sizeof(buf), fp))
		{
#ifdef WL500GX
			if (strstr(buf, "ehci"))
#else
			if (strstr(buf, "ohci"))
#endif
			{
				//logmessage("web camera", buf);

				if (strcmp(usbinterrupt, buf)==0) flag=0;
				strcpy(usbinterrupt, buf);
				break;
			}				
		}
		fclose(fp);
	}
	return flag;		
}
#endif

int notice_rcamd(int flag)
{
	 int rcamdpid=-1;
	 //printf("Send signal : %d %d\n", rcamdpid, flag);
	 if (rcamdpid==-1)
	 {
			FILE *fp;

			if ((fp=fopen("/var/run/rcamd.pid","r"))!=NULL)
			{
				fscanf(fp,"%d", &rcamdpid);
				fclose(fp);
			}			
	 }
	 if (rcamdpid!=-1) 
	 {
		 if (flag)
			kill(rcamdpid, SIGUSR1);
		 else
			kill(rcamdpid, SIGUSR2);	

		 return 1;
	 }
	 return 0;
}

int refresh_rcamd(void)
{
	FILE *fp;
	int rcamdpid=-1;

	if ((fp=fopen("/var/run/rcamd.pid","r"))!=NULL)
	{
		fscanf(fp,"%d", &rcamdpid);
		fclose(fp);
		unlink("/var/run/rcamd.pid");
		kill(rcamdpid, SIGUSR1);
	}			
	else 
	{	
		eval("killall", "rcamd");
	}

	if ((fp=fopen("/var/run/rcamdmain.pid","r"))!=NULL)
	{
		fscanf(fp,"%d", &rcamdpid);
		fclose(fp);
		kill(rcamdpid, SIGUSR1);
	}		
	return 0;
}

int refresh_wave(void)
{
	FILE *fp;
	int wavepid=-1;

	eval("killall", "waveserver");

	if ((fp=fopen("/var/run/waveservermain.pid","r"))!=NULL)
	{
		fscanf(fp,"%d", &wavepid);
		fclose(fp);
		kill(wavepid, SIGUSR1);
	}			
	return 0;
}

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
		FILE *fp;
		char command[256], *cmd_ptr;

		dprintf("Get Signal: %d %d %d\n", svcStatus[WEBACTIVE], extStatus[WEBACTIVE], sig);

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
	}	
}

void sta_check(void)
{
	int ret, i;
	char *wl_ifname=nvram_safe_get("wl0_ifname");
	char bssid[32];
	int val;

	if (stacheck_interval==-1)
	{
		if (nvram_invmatch("wl0_mode", "sta") && 
            		nvram_invmatch("wl0_mode", "wet")) return;

		stacheck_interval=STACHECK_PERIOD_DISCONNECT;
	}
	
	stacheck_interval--;

	if (stacheck_interval) return;

	// Get bssid
	ret=wl_ioctl(wl_ifname, WLC_GET_BSSID, bssid, sizeof(bssid));

	if (ret==0 && !(bssid[0]==0&&bssid[1]==0&&bssid[2]==0
		&&bssid[3]==0&&bssid[4]==0&&bssid[5]==0)) 	
	{
		dprintf("connected\n");
		stacheck_interval=STACHECK_PERIOD_CONNECT;
	}
	else 
	{
		char sbuf[128];
		dprintf("disconnected\n");
		stacheck_interval=STACHECK_PERIOD_DISCONNECT;

		dprintf("connect: [%s]\n", nvram_safe_get("wl0_join"));
		system(nvram_safe_get("wl0_join"));
		//eval("wl", "join", nvram_safe_get("wl0_ssid"));
	}
	return;
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
void watchdog(void)
{
	time_t now;
	/* handle button */
	btn_check();

	/* if timer is set to less than 1 sec, then bypass the following */
	if (itv.it_value.tv_sec==0) return;

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
	/* web cam process */
	if (nvram_invmatch("usb_web_device", ""))
	{	
		if (nvram_invmatch("usb_webdriver_x", ""))
		{
			if (!rcamd_processcheck())
			{
				refresh_rcamd();
			}
		}				
		else
		{	
			//hotplug_usb_webcam(nvram_safe_get("usb_web_device"), 
			//	   atoi(nvram_safe_get("usb_web_flag")));
			//nvram_set("usb_web_device", "");
			//nvram_set("usb_web_flag", "");
			// reset WEBCAM status	
			refresh_rcamd();
			svcStatus[WEBACTIVE] = -1;
		}
	}

	/* storage process */
	if (nvram_invmatch("usb_storage_device", ""))
	{
		hotplug_usb_mass(nvram_safe_get("usb_storage_device"));
		nvram_set("usb_storage_device", "");
	}
#endif

	/* station or ethernet bridge handler */
	sta_check();
}

int 
watchdog_main(int argc, char *argv[])
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
	if ((fp=fopen("/var/run/watchdog.pid", "w"))!=NULL)
	{
		fprintf(fp, "%d", getpid());
		fclose(fp);
	}

	/* set the signal handler */
	signal(SIGUSR1, catch_sig);
	signal(SIGUSR2, catch_sig);
	signal(SIGALRM, watchdog);

	/* set timer */
	alarmtimer(NORMAL_PERIOD, 0);

	/* Start GPIO function */
	gpio_init();

	/* Start POWER LED */
	LED_CONTROL(LED_POWER, LED_ON);

	/* Start sync time */
	sync_interval=1;
	start_ntpc();

	/* Most of time it goes to sleep */
	while(1)
	{
		pause();
	}
	
	return 0;
}
