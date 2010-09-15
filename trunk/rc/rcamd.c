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
#ifdef USB_SUPPORT 
#include <stdio.h>
#include <signal.h>
#include <time.h>
#include <sys/time.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>

#include <bcmnvram.h>
#include <shutils.h>
#include <rc.h>
#include <stdarg.h>

int
start_rcamd(void)
{
	char *rcamd_argv[] = {"rcamdmain", NULL};
	pid_t pid;

	return _eval(rcamd_argv, NULL, 0, &pid);
}

int
stop_rcamd(void)
{
	eval("killall", "rcamdmain");
	eval("killall", "rcamd");
	return 0;
}

int
hotplug_usb_webcam(char *product, int webflag)
{
	char *rcamd_argv[]={"rcamd", 
				"-p", nvram_safe_get("usb_webactivex_x"),
				"-s", nvram_safe_get("usb_webfresh_x"),
				"-z", nvram_safe_get("time_zone"),
				"-a", nvram_safe_get("usb_websecurity_x"),
				"-r", "0",	// Record Time -r
				NULL, NULL,	// Model -t
				NULL, NULL, 	// Image Size -f
				NULL, NULL, 	// Sense Vlaue -m 
				NULL, NULL, 	// Sense Limit -c
				NULL, NULL,
				NULL};
	char **arg;
	pid_t pid;

	if (nvram_match("usb_webenable_x", "0") || strlen(product)==0 || webflag==0) return 1;
	for (arg = rcamd_argv; *arg; arg++);
	
	if (webflag == WEB_PWCWEB)
	{
		eval("insmod", "pwc", "power_save=0", "size=vga");
		eval("insmod", "pwcx");
		nvram_set("usb_webdriver_x", "0");

		*arg++ = "-t";
		*arg++ = "0";
	}
	else
	{
		eval("insmod", "i2c-core");
		eval("insmod", "ov51x");
		eval("insmod", "ov511_decomp");
		eval("insmod", "ov518_decomp");
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

	// image size
	if (nvram_match("usb_webimage_x", "0"))
	{
		*arg++ = "-f";

		*arg++="640x480";
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
	_eval(rcamd_argv, ">/dev/null", 0, &pid);

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

static void catch_sig(int sig)
{
	if (sig == SIGUSR1)
	{
		//printf("SIGUSR1 catched\n");
	}
}

int rcamd_main(void)
{
	FILE *fp;
	
	fp=fopen("/var/run/rcamdmain.pid","w");
	if (fp==NULL) exit(0);
	fprintf(fp, "%d", getpid());
	fclose(fp);
	
	signal(SIGUSR1, catch_sig);

	while(1)
	{
		hotplug_usb_webcam(nvram_safe_get("usb_web_device"), 
				   atoi(nvram_safe_get("usb_web_flag")));
		pause();
	}	
}
#endif

