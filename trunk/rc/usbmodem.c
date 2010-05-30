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
    int ret=0;
    char sfn[200];
    char dialparam[] = "dial_param_XXXXXXXXX";
    pid_t pid;
    char tmp[200];
    char *argv[] = {
	(modem_get_script_name(sfn), sfn),
	NULL
    };

    dprintf( "%s", sfn );

    if( nvram_match( strcat_r(prefix, "dial_enabled", tmp), "1" ) &&
	nvram_match( strcat_r(prefix, "prepared", tmp), "1" ) &&
	nvram_get( strcat_r(prefix, "usb_device", tmp) ) ){

	snprintf(dialparam, 20, "dial_param_%s", nvram_safe_get("wan_modem_mode_x"));
	nvram_unset(dialparam);
	nvram_unset("wan_modem_dial_param_1");

	if (nvram_match( strcat_r(prefix, "proto", tmp), "usbmodem") &&
		nvram_invmatch("wan_modem_mode_x", "-1"))
	{
		nvram_set( strcat_r(prefix, "ifname", tmp) , nvram_safe_get(strcat_r(prefix, "modem_ifname", tmp)) );
		nvram_set( strcat_r(prefix, "dnsenable_x", tmp), "1");
	}
	ret = _eval(argv, NULL, 0, &pid);
    }

    dprintf("done\n");
    return ret;
}

int
stop_modem_dial(void)
{
    int ret;
    char sfn[200], * tmp;
    //eval("killall", "dial");
    modem_get_script_name( sfn );
    tmp = strrchr( sfn, '/' );
    if( tmp ) tmp++;
    else tmp = sfn;

    eval("killall", tmp);
    ret = eval("killall", "pppd");

    dprintf("done\n");
    return ret;
}

int hotplug_check_modem( char * interface, char * product, char * prefix )
{
    int ret=0;
    char tmp[200], *str1, *str2;
    char stored_product[40];
    int vid, pid;
    char *argv[] = {
	"/usr/ppp/zerocd",
	NULL
    };

    str1 = nvram_safe_get( "wan_modem_vid" );
    sscanf( str1, "%x", &vid );
    str2 = nvram_safe_get( "wan_modem_pid" );
    sscanf( str2, "%x", &pid );

    sprintf( stored_product, "%x/%x", vid, pid );

    dprintf( "stored: %s, found: %s", stored_product, product );

    if( strncmp( product, stored_product, strlen(stored_product) )==0 ){
	ret=1;
    } else {
	if( !*str1 ){
	    strcpy( stored_product, product );
	    str1 = strchr( stored_product, '/' );
	    if( str1 ){
		*str1=0; str1++;
		str2 = strchr( str1, '/' );
		if( str2 ) *str2=0;
		sscanf( stored_product, "%x", &vid ); sscanf( str1, "%x", &pid );
		sprintf( stored_product, "0x%04x", vid );
//		nvram_set(strcat_r(prefix, "modem_vid", tmp), stored_product );
		nvram_set( "wan_modem_vid",  stored_product );
		sprintf( stored_product, "0x%04x", pid );
//		nvram_set(strcat_r(prefix, "modem_pid", tmp), stored_product );
		nvram_set( "wan_modem_pid",  stored_product );
		
		dprintf( "wrote: %04x:%04x", vid, pid );
		ret=1;
	    }
	}
    }

    if( ret==0 ){
	// check zerocd mode
	_eval(argv, NULL, 0, &pid);
    }

    dprintf("done. ret %d", ret );

    return ret;
}
