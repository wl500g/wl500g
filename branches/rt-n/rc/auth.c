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
start_wpa_supplicant(void)
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
start_lanauth(void)
{
	return 0;
}

int
stop_lanauth(void)
{
	return 0;
}
#endif
