#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>
#include <errno.h>
#include <time.h>

#include <bcmnvram.h>
#include <netconf.h>
#include <shutils.h>
#include <rc.h>
#include <sys/stat.h>

#include <nvparse.h>

#define wimax_events "/tmp/madwimax.events"

extern int wan_valid(char *ifname);

void update_nvram_wmx(char * ifname, int isup);

void get_wimax_ifname(char *out, int unit)
{
	sprintf(out, "wmx%d", unit);
}

int
wimax_modem(void)
{
	//dprintf("%s\n", nvram_safe_get("usb_wimax_device"));
	return nvram_invmatch("usb_wimax_device", "");
}

inline int is_chk_con(){ return nvram_match("wmx_chk_con", "0" ) == 0; };
inline int is_chk_log(){ return nvram_match("wmx_chk_log", "1" ) != 0; };
inline int is_chk_restart_wimax(){ return nvram_match("wmx_chk_rst", "1" ) != 0; };
inline int wmx_chk_interval(){ 
	int result = atoi ( nvram_safe_get("wmx_chk_interval") );
	if ( result ) return result;
	else  return 30;
};

inline time_t wmx_chk_get_last_time(){ return atol ( nvram_safe_get("wmx_chk_tm_last") );}

inline void wmx_chk_set_last_time( time_t cur )
{
   char buf[0x20];
   snprintf( buf, sizeof(buf), "%ld", (unsigned long) cur );
   nvram_set("wmx_chk_tm_last", buf );
};

inline void wmx_nvram_check_default( char * name, char * def_val)
{
	if( nvram_match( name, "0" ) == 0 &&
	    nvram_match( name, "1" ) == 0 )
		nvram_set( name, def_val );
};

void init_wmx_variables()
{
	wmx_nvram_check_default("wmx_chk_con", "0" ); // ping connection
	wmx_nvram_check_default("wmx_chk_log", "0" ); // output to syslog
	wmx_nvram_check_default("wmx_chk_rst", "0" ); // kill madwimax if ping failed

	int t_interval = atoi ( nvram_safe_get("wmx_chk_interval") );
	if( t_interval<30 || t_interval>3600 ) 
		nvram_set("wmx_chk_interval", "60" );

	nvram_unset( "wmx_chk_tm_last" ); // timestamp of the last connection checking
};


int
start_wimax(char *prefix)
{
	char tmp[100];
	int unit = atoi(nvram_safe_get(strcat_r(prefix, "unit", tmp)));
	char *wimax_ifname = nvram_safe_get(strcat_r(prefix, "ifname", tmp));
	char *wimax_ssid = nvram_safe_get(strcat_r(prefix, "wimax_ssid", tmp));

	char *wmx_argv[] = {
		"/usr/sbin/madwimax", "-qofd",
		"-i", wimax_ifname,
		"-e", wimax_events,
		"-p", (sprintf(tmp, "/var/run/madwimax%d.pid", unit), tmp),
		"-s", "/tmp/madwimax.log",
		"--ssid", *wimax_ssid ? wimax_ssid : "@yota.ru",
		NULL};

	init_wmx_variables();
	update_nvram_wmx(wimax_ifname, 0);

	if (wimax_modem())
	{
		eval("insmod", "tun");
		sleep(1);
		symlink("/sbin/rc", wimax_events);
		return _eval(wmx_argv, NULL, 0, NULL);
	}

	return 0;
}

int
stop_wimax(void)
{
	eval("killall", "madwimax");
	sleep(1);
	unlink(wimax_events);
	eval("rmmod", "tun");

	return 0;
}

//time_t prev_time = 0;
// called from watchdog using the "madwimax-check" symlink to rc.
int
madwimax_check(void)
{
	char tmp[100], pid_fname[100];
	char prefix[] = "wanXXXXXXXXXX_";

	if (!wimax_modem()) {
		return -1;
	}

	//int unit = atoi(nvram_safe_get("wimax_unit"));
	int unit = 0; // !!! tobefixed
	snprintf(prefix, sizeof(prefix), "wan%d_", unit);
	sprintf(pid_fname, "/var/run/madwimax%d.pid", unit);

	// check the madwimax process
	pid_t pid = 0;

	FILE *fp = fopen(pid_fname, "r");
	char buf[256];
	if (fp && fgets(buf, sizeof(buf), fp)) {
		pid = strtoul(buf, NULL, 0);
		fclose(fp);
	}

	//*********** Connection checking section *****************
	if (pid > 0 && is_chk_con() ) {
		// TODO check the process state
		//dprintf( "wmx_chk_con enabled, madwimad pid %d", pid );
		time_t cur_time = time(NULL);
		time_t prev_time = wmx_chk_get_last_time();

		if(	(cur_time - prev_time) > wmx_chk_interval() || 
			cur_time < prev_time  )
		{
			//if( is_chk_log() ) logmessage( "[wmx_checker]", "Ping gateway\n" );
			get_wimax_ifname( buf, unit );
			char s_pid[20];
			snprintf( s_pid, sizeof(s_pid), "%d", pid  );
			char * wchk_argv[] = {
				"/usr/sbin/wimax_check_connection.sh",
				s_pid,					// madwimax pid
				buf,					// if_name
				//nvram_safe_get( "wmx_gateway_t" ),	// gateway ip
				nvram_safe_get( strcat_r(prefix, "gateway", tmp) ),
				NULL
			};
			int ping_pid;
			_eval( wchk_argv, NULL, 0, &ping_pid);
			//prev_time = cur_time;
			wmx_chk_set_last_time( cur_time );

			fp = fopen(pid_fname, "r");
			if (fp && fgets(buf, sizeof(buf), fp)) {
				pid = strtoul(buf, NULL, 0);
				fclose(fp);
			}
		}
	}
	//*********************************************************

	// if no madwimax, then STOP THE UDHCPC
	if (pid == 0) {
		if( is_chk_log() ) logmessage( "[wmx_checker]", "Restarting madwimax\n" );
	 	// kill the udhcpc
		kill_pidfile_s((sprintf(tmp, "/var/run/udhcpc%d.pid", unit), tmp), SIGUSR2);
		kill_pidfile((sprintf(tmp, "/var/run/udhcpc%d.pid", unit), tmp));
		//get_wimax_ifname( tmp, unit );
		start_wimax(prefix);
	}
	return 0;
}

//=========================== madwimax events ==================================

// if-create
int
madwimax_create(char *ifname)
{
	char tmp[100];
	char prefix[] = "wanXXXXXXXXXX_";
	int unit = 0;

	dprintf("ifname %s", ifname);

	if ((unit = wan_ifunit(ifname)) < 0)
		return -1;
	snprintf(prefix, sizeof(prefix), "wan%d_", unit);

	wmx_chk_set_last_time(time(NULL));

	if (nvram_match("wan_proto", "wimax"))
		nvram_set("wan_ifname", ifname);

	if (!wan_valid(ifname))
	{
		sprintf(tmp, "%s %s", ifname, nvram_safe_get("wan_ifnames"));
		nvram_set("wan_ifnames", tmp);
	}

	return 0;
}

//if-release
int
madwimax_release(char *ifname)
{
	char tmp[100];
	char prefix[] = "wanXXXXXXXXXX_";
	int unit;
	char name[100], *next;

	dprintf("ifname %s", ifname);

	if ((unit = wan_ifunit(ifname)) < 0)
		return -1;
	snprintf(prefix, sizeof(prefix), "wan%d_", unit);

	if (nvram_match("wan_proto", "wimax"))
	{
		stop_wan();
		nvram_set("wan_ifname","");
	}

	if (wan_valid(ifname))
	{
		foreach(name, nvram_safe_get("wan_ifnames"), next)
		{
			if (strcmp(name, ifname) != 0)
				sprintf(tmp, "%s %s", tmp, name);
		}
		nvram_set("wan_ifnames", tmp);
	}

	return 0;
}

//if-up
int
madwimax_up(char *ifname)
{
	char tmp[100];
	char prefix[] = "wanXXXXXXXXXX_";
	int unit;

	dprintf( "ifname %s", ifname );

	if ((unit = wan_ifunit(ifname)) < 0)
		return -1;
	snprintf(prefix, sizeof(prefix), "wan%d_", unit);

	wmx_chk_set_last_time(time(NULL));
	
	/* Start firewall */
	start_firewall_ex( ifname, "0.0.0.0",
			  "br0", nvram_safe_get("lan_ipaddr"));

	if (nvram_match(strcat_r(prefix, "ipaddr", tmp), "0.0.0.0"))
	{
		char *dhcp_argv[] = {"udhcpc",
				     "-i", ifname,
				     "-p", (sprintf(tmp, "/var/run/udhcpc%d.pid", unit), tmp),
				     "-s", "/tmp/udhcpc",
				     "-b",
				     NULL};
		/* Start dhcp daemon */
		_eval(dhcp_argv, NULL, 0, NULL);
	} else {
		ifconfig(ifname, IFUP,
			nvram_get(strcat_r(prefix, "ipaddr", tmp)),
			nvram_get(strcat_r(prefix, "netmask", tmp)));

		wan_up(ifname);
	}

	update_nvram_wmx(ifname, 1);

	return 0;
}

//if-down
int
madwimax_down(char *ifname)
{
	char tmp[100];
	int unit;

	dprintf("ifname %s", ifname);

	if ((unit = wan_ifunit(ifname)) < 0)
		return -1;

	wan_down(ifname);
	update_nvram_wmx(ifname, 0);

 	// kill the udhcpc
	kill_pidfile_s((sprintf(tmp, "/var/run/udhcpc%d.pid", unit), tmp), SIGUSR2);
	kill_pidfile((sprintf(tmp, "/var/run/udhcpc%d.pid", unit), tmp));

	return 0;
}

void update_nvram_wmx( char * ifname, int isup )
{
	dprintf( "ifname: %s, is up: %d", ifname, isup );
/*	// variables may be usefull in case of multiwan (web output)
	if( isup ){
		nvram_set("wmx_ifname_t", ifname );
		nvram_set("wmx_ipaddr_t",nvram_safe_get("wan_ipaddr_t"));
		nvram_set("wmx_netmask_t",nvram_safe_get("wan_netmask_t"));
		nvram_set("wmx_gateway_t",nvram_safe_get("wan_gateway_t"));
		nvram_set("wmx_dns_t",nvram_safe_get("wan_dns_t"));
	} else {
		nvram_set("wmx_ifname_t","");
		nvram_set("wmx_ipaddr_t","");
		nvram_set("wmx_netmask_t","");
		nvram_set("wmx_gateway_t","");
		nvram_set("wmx_dns_t","");
	}*/
	if( is_chk_con() )
		nvram_set("wmx_ping_t","");
	else
		nvram_set("wmx_ping_t","Switch on \"check connection\".");
};

int
madwimax_main(int argc, char **argv)
{

	dprintf("%s %s %s\n", argv[0], argv[1], argv[2]);

	if (argc != 3)
		return -1;

	if (!strcmp(argv[1], "if-create"))
		return madwimax_create(argv[2]);
	if (!strcmp(argv[1], "if-up"))
		return madwimax_up(argv[2]);
	if (!strcmp(argv[1], "if-down"))
		return madwimax_down(argv[2]);
	if (!strcmp(argv[1], "if-release"))
		return madwimax_release(argv[2]);

	return -1;
}
