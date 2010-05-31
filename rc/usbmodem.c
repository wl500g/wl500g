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

    if( nvram_match( strcat_r(prefix, "dial_enabled", tmp), "1" ) &&
	nvram_match( strcat_r(prefix, "prepared", tmp), "1" ) &&
	nvram_get( strcat_r(prefix, "usb_device", tmp) ) )
    {
	dprintf( "%s", sfn );

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

int
parse_product_string( char * product, int * vid, int * pid )
{
    int res=0;
    char tmp[0x200], *str1, *str2;
    *vid=0; *pid=0;
    strncpy( tmp, product, sizeof(tmp)-1 );
    str1 = strchr( tmp, '/' );
    if( str1 ){
	*str1=0; str1++;
	str2 = strchr( str1, '/' );
	if( str2 ) *str2=0;
	sscanf( tmp, "%x", vid ); sscanf( str1, "%x", pid );
	res=1;
    }
    return res;
}

int 
hotplug_check_modem( char * interface, char * product, char * prefix )
{
    int ret=0;
    char *str1, *str2;
    char tmp[200];
    char stored_product[40];
    int vid, pid;

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
	    if( parse_product_string( product, &vid, &pid ) ){
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

    if( ret ) nvram_set(strcat_r(prefix, "usb_device_name", tmp), "3G Modem");

    dprintf("done. ret %d", ret );

    return ret;
}

void
hotplug_usb_modeswitch( char * interface, char * action, char * product )
{
    int vid, pid;
//    char *argv[] = {
//	"/usr/ppp/zerocd",
//	NULL
//    };
    int dev_vid, dev_pid;
    int i = 0;
    char sFileName[0x7F];
    char sManufacturer[0x7F];
    char * sMaList[]={
	"Option",
	"AnyDATA",
	"SAMSUNG",
	"Vertex"
    };
    char * argv_usb_modeswitch[] = {
	"usb_modeswitch",
	"-c",
	sManufacturer,
	NULL
    };
    *sManufacturer = 0;

    if( strcmp(action, "add") == 0 ){
	if( nvram_match("wan_modem_zerocd_mode", "UserDefined" ) )
	{
	    strcpy( sManufacturer, "/usr/local/etc/usb_modeswitch.conf" );
	} else if( nvram_match("wan_modem_zerocd_mode", "Auto" ) )
	{
	    if( parse_product_string( product, &vid, &pid ) ){
		sprintf( sFileName, "%04x_%04x", vid, pid );
	
		if( vid==0x05c6 && pid==0x1000 )
		{
		    FILE * file = fopen( "/proc/bus/usb/devices", "rt");
		    char str[0xFF];
		    char * str1;
		    if( file )
		    {
			int ready=0;
			char templ1[]="Vendor";
			char templ2[]="ProdID";
			char templ3[]="Manufacturer";

			while( !feof(file) )
			{
			    fgets( str, sizeof(str)-1, file);
			    str1 = strstr( str, templ1 );
			    if( str1 ){
				sscanf( str1+sizeof(templ1), "%x", &dev_vid );
				str1 = strstr( str1, templ2 );
				if( str1 ){
				    sscanf( str1+sizeof(templ2), "%x", &dev_pid );
				}
				ready=0;
			    } else {
				str1 = strstr( str, templ3 );
				if( str1 ){
				    strcpy( sManufacturer, str1+sizeof(templ3) );
				    ready=1;
				}
			    }
			    if( ready )
			    {
				dprintf( "USB: %04x:%04x %s", dev_vid, dev_pid, sManufacturer );
				if( dev_vid==vid && dev_pid==pid ){
				    for( i=0; i<sizeof(sMaList)/sizeof(char*); i++ )
				    {
					if( strncmp( sManufacturer, sMaList[i], strlen(sMaList[i]) ) == 0 )
					{
					    if(i==0) strcat ( sFileName, "_sVe=" );
					    else strcat ( sFileName, "_uMa=" );

					    strcat ( sFileName, sMaList[i] );
					    break;
					}
				    }
				    break;
				}
				ready=0;
			    }
			}
			fclose( file );
		    }
		}
	    };
	    sprintf( sManufacturer, "/usr/share/usb_modeswitch.d/%s", sFileName );
	}
	
	if( *sManufacturer ){
	    FILE * file = fopen( sManufacturer, "r" );
	    if( file ){
		fclose( file );
		dprintf( "usb_modeswitch -c %s", sManufacturer );
		_eval(argv_usb_modeswitch, NULL, 0, &pid);
	    }
	}

	// check zerocd mode
	//_eval(argv, NULL, 0, &pid);
    }
    dprintf("done");
}
