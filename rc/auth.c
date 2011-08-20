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
	return 0;
}

int
stop_wpa_supplicant(void)
{
	return 0;
}
#endif

#ifdef __CONFIG_TELENET__
int
start_lanauth(char *prefix)
{
	char tmp[100];
	char *lanauth_argv[] = {
		"lanauth",
	    /*	"-v", protocol,	   */
	    /*	"-l", acces level, */
		"-p", nvram_safe_get(strcat_r(prefix, "pppoe_passwd", tmp)),
		nvram_invmatch("wan_heartbeat_x", "") ? "-s" : NULL,
		nvram_safe_get("wan_heartbeat_x"),
   		NULL
	};

	return _eval(lanauth_argv, NULL, 0, NULL);
}

int
stop_lanauth(void)
{
	return killall("lanauth", 0);
}
#endif
