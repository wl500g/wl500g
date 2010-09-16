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


void modem_get_script_name(char *filename)
{
    char * script_fn = nvram_get( "wan_modem_scriptfn" );
    if( script_fn && *script_fn )
	strcpy( filename, script_fn );
    else
	strcpy( filename, "/usr/ppp/dial" );
};

int
start_modem_dial(char *prefix)
{
    int ret=0;
    pid_t pid;
    FILE * file;
    char sfn[200], tmp[200];
    char *argv[] = {
	(modem_get_script_name(sfn), sfn),
	NULL
    };

    if( nvram_match( strcat_r(prefix, "dial_enabled", tmp), "1" ) &&
	nvram_match( strcat_r(prefix, "prepared", tmp), "1" ) &&
	nvram_get( strcat_r(prefix, "usb_device", tmp) ) )
    {
	dprintf( "%s", sfn );

	if (nvram_match( strcat_r(prefix, "proto", tmp), "usbmodem") )
	{
		nvram_set( strcat_r(prefix, "ifname", tmp) , nvram_safe_get(strcat_r(prefix, "pppoe_ifname", tmp)) );
		nvram_set( strcat_r(prefix, "dnsenable_x", tmp), "1");
	}
	ret = _eval(argv, NULL, 0, &pid);
	if (pid) {
	    sprintf(tmp, "/var/run/%s", prefix );
	    if (tmp[strlen(tmp)-1]=='_') tmp[strlen(tmp)-1]=0;
	    file=fopen(tmp, "w");
	    if (file) {
		fprintf(file, "%d", pid);
		fclose(file);
	    }
	}
    }

    dprintf("done\n");
    return ret;
}

int
stop_modem_dial(char *prefix)
{
    int ret, pid;
    char tmp[200];
    int unit;
    FILE *file;
    char *argv[] = {
	"kill",
	tmp+100,
	NULL
    };

    unit = atoi(nvram_safe_get(strcat_r(prefix, "unit", tmp)));
    ret=0;

    sprintf(tmp, "/var/run/%s", prefix);
    tmp[strlen(tmp)-1]=0;
    file=fopen(tmp,"r");
    if(file){
        fgets(tmp+100, 10, file);
        fclose(file);
        ret=_eval(argv, NULL, 0, &pid);
    }
    unlink(tmp);

    sprintf(tmp, "/var/run/ppp%d.pid", unit);
    file=fopen(tmp,"r");
    if(file){
        fgets(tmp+100, 10, file);
        fclose(file);
        ret=_eval(argv, NULL, 0, &pid);
    }
//    nvram_set( "wan0_dial_enabled", "0" );

    dprintf("%s done\n", prefix);
    return ret;
}

int
parse_product_string( char *product, int *vid, int *pid )
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
hotplug_check_modem( char *interface, char *product, char *prefix )
{
    int ret=0;
    char *str1, *str2;
    char tmp[200];
    char stored_product[40];
    int vid, pid;
    int autodetect = nvram_match("wan_modem_autodetect", "1");

    str1 = nvram_safe_get( "wan_modem_vid" );
    sscanf( str1, "%x", &vid );
    str2 = nvram_safe_get( "wan_modem_pid" );
    sscanf( str2, "%x", &pid );

    sprintf( stored_product, "%x/%x", vid, pid );

    dprintf( "stored: %s, found: %s, autodetect: %d", stored_product, product, autodetect );

    if( autodetect==0 &&
	strncmp( product, stored_product, strlen(stored_product) )==0 ){
	ret=1;
    } else {
	if( !*str1 || autodetect ){
	    if(autodetect && parse_product_string( product, &vid, &pid )){
		sprintf( stored_product, "0x%04x", vid );
//		nvram_set(strcat_r(prefix, "modem_vid", tmp), stored_product );
		nvram_set( "wan_modem_vid",  stored_product );
		sprintf( stored_product, "0x%04x", pid );
//		nvram_set(strcat_r(prefix, "modem_pid", tmp), stored_product );
		nvram_set( "wan_modem_pid",  stored_product );

		dprintf( "wrote: %04x:%04x", vid, pid );
	    }
	    ret=1;
	}
    }

    if( ret ) nvram_set(strcat_r(prefix, "usb_device_name", tmp), "3G Modem");

    dprintf("done. ret %d", ret );

    return ret;
}

void
hotplug_usb_modeswitch( char *interface, char *action, char *product )
{
    int vid, pid;

    int dev_vid, dev_pid;
    int i = 0;
    char sFileName[0x7F];
    char sPath[0x7F];

    char * sMaList[]={
	"Option",
	"AnyDATA",
	"SAMSUNG",
	"Vertex"
    };

    char * argv_usb_modeswitch[] = {
	"usb_modeswitch",
	"-c",
	sPath,
	NULL
    };
    *sPath = 0;

    if( strcmp(action, "add") == 0 ){
	if( nvram_match("wan_modem_zerocd_mode", "UserDefined" ) ){
	    strcpy( sPath, "/usr/local/etc/usb_modeswitch.conf" );
	} else if( nvram_match("wan_modem_zerocd_mode", "Auto" ) )
	{
	    if( parse_product_string( product, &vid, &pid ) ){
		sprintf( sFileName, "%04x:%04x", vid, pid );
		if( (vid==0x05c6 && pid==0x1000) ||
		    (vid==0x1a8d && pid==0x1000) )
		{
		    FILE * file = fopen( "/proc/bus/usb/devices", "rt");
		    char str[0xFF];
		    int i_size;
		    char * str1;
		    int added=0;
		    if( file ){
			int ready=0;
			char *templ[]={	"Vendor=",
					"ProdID=",
					"Manufacturer=",
					"Product="};
			char sManufacturer[0xFF];

			while( !feof(file) )
			{
			    fgets( str, sizeof(str)-1, file);
			    if( (str1 = strstr( str, templ[0] )) ){
				sscanf( str1+strlen(templ[0]), "%x", &dev_vid );
				if( (str1 = strstr( str1, templ[1] )) ){
				    sscanf( str1+strlen(templ[1]), "%x", &dev_pid );
				}
				ready=0;
			    } else if( (str1 = strstr( str, templ[2] )) ){
				strncpy( sManufacturer, str1+strlen(templ[2]), sizeof(sManufacturer)-1);
				ready=1;
			    } else if( (str1 = strstr( str, templ[3] )) ){
				strncpy( sManufacturer, str1+strlen(templ[3]), sizeof(sManufacturer)-1);
				ready=2;
			    }

			    if( ready ){
				dprintf( "lsUSB: %04x:%04x", dev_vid, dev_pid );
				if( dev_vid==vid && dev_pid==pid ){
				    i_size = sizeof(sMaList)/sizeof(char*);
				    if( ready==1 ){
					for( i=0; i<i_size; i++ ){
					    if( strncmp( sManufacturer, sMaList[i], strlen(sMaList[i]) )==0 ){
						if(i==0) strcat ( sFileName, ":sVe=" );
						else strcat ( sFileName, ":uMa=" );

						strcat ( sFileName, sMaList[i] );
						added=1;
						break;
					    }
					}
				    } else {
					if( strncmp( sManufacturer, "5G", 2 ) == 0 ){
					    strcat ( sFileName, ":uPr=5G" );
					    added=1;
					    break;
					}
				    }
				}
				ready=0;
			    }
			}
			fclose( file );
		    }
		    if( added==0 && vid==0x05c6 )
			strcat (sFileName, ":uMa=AnyDATA");
		}
	    };
	    sprintf( sPath, "/usr/share/usb_modeswitch.d/%s", sFileName );
	}
	
	if( *sPath ){
	    dprintf("Check usb_modeswitch data file %s",sPath);
	    FILE * file = fopen( sPath, "r" );
	    if( file ){
		fclose( file );
		dprintf( "usb_modeswitch -c %s", sPath );
		_eval(argv_usb_modeswitch, NULL, 0, NULL);
	    }
	}

    }
    dprintf("done");
}

int 
usb_modem_check(char *prefix)
{
    int ret=0;
    char tmp[200];
    FILE *file;

    if( nvram_match( strcat_r(prefix, "dial_enabled", tmp), "1" ) &&
	nvram_match( strcat_r(prefix, "prepared", tmp), "1" ) &&
	nvram_get( strcat_r(prefix, "usb_device", tmp) ) )
    {
	sprintf(tmp, "/var/run/%s", prefix);
	tmp[strlen(tmp)-1]=0;
	file=fopen(tmp,"r");
	if(file){
	    fgets(tmp+100, 99, file);
	    fclose(file);

	    sprintf(tmp, "/proc/%s/status", tmp+100);
	    file=fopen(tmp, "r");
	    if(file){
		fclose(file);
	    } else
		ret=start_modem_dial(prefix);
	}else
	    ret=start_modem_dial(prefix);
    }
    return ret;
}
