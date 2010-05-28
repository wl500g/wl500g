#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>
#include <errno.h>
#include <ctype.h>
#include <time.h>

#include <bcmnvram.h>
#include <netconf.h>
#include <shutils.h>
#include <rc.h>
#include <sys/stat.h>

#include <nvparse.h>


void modem_get_script_name( char * filename )
{
    char * script_fn = nvram_get( "wan_modem_scriptfn" );
    if( script_fn && *script_fn )
	strcpy( filename, script_fn );
    else
	strcpy( filename, "/usr/ppp/dial" );
};

int
start_modem_dial( char * prefix )
{
    int ret;
    char sfn[200];
    char *argv[] = {
	(modem_get_script_name(sfn), sfn),
	NULL
    };

    dprintf( "%s", sfn );

    char dialparam[] = "dial_param_XXXXXXXXX";
    pid_t pid;
    char tmp[200];

    snprintf(dialparam, 20, "dial_param_%s", nvram_safe_get("wan_modem_mode_x"));
    nvram_unset(dialparam);
    nvram_unset("wan_modem_dial_param_1");

    //nvram_set("wan0_primary", "1");
    //nvram_set("wan1_primary", "0");

    if (nvram_match("wan_proto", "usbmodem") &&
	nvram_invmatch("wan_modem_mode_x", "-1"))
    {
		nvram_set("wan0_ifname", nvram_safe_get(strcat_r(prefix, "modem_ifname", tmp)) );
		nvram_set("wan0_dnsenable_x", "1");
    }
    ret = _eval(argv, NULL, 0, &pid);

    dprintf("done\n");
    return ret;
}

int
stop_modem_dial(void)
{
	int ret = eval("killall", "dial");
	ret = eval("killall", "pppd");

	dprintf("done\n");
	return ret;
}
