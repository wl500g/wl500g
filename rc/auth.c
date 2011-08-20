#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>
#include <errno.h>
#include <sys/stat.h>
#include <arpa/inet.h>

#include <bcmnvram.h>
#include <netconf.h>
#include <shutils.h>
#include "rc.h"

#ifdef __CONFIG_EAPOL__
int
start_wpa_supplicant(char *prefix)
{
	FILE *fp;
	char tmp[100];
	char *options = "/etc/wpa_supplicant.conf";
	char *wpa_argv[] = {
	    "/usr/sbin/wpa_supplicant",
	    "-B",
	    "-D", "roboswitch", /* RT-N16? */
	    "-i", nvram_safe_get(strcat_r(prefix, "ifname", tmp)),
	    "-c", options,
	    NULL
	};

	/* Generate options file */
	if ((fp = fopen(options, "w")) == NULL) {
		perror(options);
		return -1;
	}
	fprintf(fp,
		"ctrl_interface=/var/run/wpa_supplicant\n"
		"ap_scan=0\n"
		"fast_reauth=1\n"
		"network={\n"
		"	key_mgmt=IEEE8021X\n"
		"	eap=MD5\n"
		"	identity=\"%s\"\n"
		"	password=\"%s\"\n"
		"	eapol_flags=0\n"
		"}\n",
		nvram_safe_get(strcat_r(prefix, "pppoe_username", tmp)),
		nvram_safe_get(strcat_r(prefix, "pppoe_passwd", tmp)));
	fclose(fp);

	/* Start wpa_supplicant */
	return _eval(wpa_argv, NULL, 0, NULL);
}

int
stop_wpa_supplicant(void)
{
	return killall("wpa_supplicant", 0);
}
#endif

#ifdef __CONFIG_TELENET__
int
start_lanauth(char *prefix)
{
	char tmp[100];
	char *lanauth_argv[] = {
		"/usr/sbin/lanauth",
	    /*	"-v", protocol,	   */
	    /*	"-l", acces level, */
		"-p", nvram_safe_get(strcat_r(prefix, "pppoe_passwd", tmp)),
		nvram_invmatch("wan_heartbeat_x", "") ? "-s" : NULL,
		nvram_safe_get("wan_heartbeat_x"),
   		NULL
	};

	/* Start lanauth */
	return _eval(lanauth_argv, NULL, 0, NULL);
}

int
stop_lanauth(void)
{
	return killall("lanauth", 0);
}
#endif
