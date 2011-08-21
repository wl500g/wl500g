#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>
#include <errno.h>
#include <ctype.h>
#include <time.h>
#include <sys/stat.h>

#include <bcmnvram.h>
#include <netconf.h>
#include <shutils.h>
#include "rc.h"

#include <nvparse.h>

#define wimax_events "/tmp/madwimax.events"

extern int wan_valid(char *ifname);

void update_nvram_wmx(char * ifname, int isup);

inline static int
wimax_modem(char *prefix)
{
	char tmp[100];
	char * tmp1 = nvram_get(strcat_r(prefix, "usb_device", tmp));
	return ( tmp1!=0 && *tmp1!=0 );
}

inline static int
is_chk_con(void)
{
	return nvram_match("wan_wimax_check", "1");
}

inline static int
is_chk_log(void)
{
	return nvram_match("wan_wimax_syslog", "1");
}

inline static int
is_chk_restart_wimax(void)
{
	return nvram_match("wan_wimax_restart", "1");
}

inline static int
wmx_chk_interval(void)
{ 
	int result = atoi(nvram_safe_get("wan_wimax_interval"));
	if (result == 0)
		result = 30;

	return result;
};

inline static time_t
wmx_chk_get_last_time(char *prefix)
{
	char tmp[100];
	return atol(nvram_safe_get(strcat_r(prefix, "wimax_check_time", tmp)));
}

inline static void
wmx_chk_set_last_time(char *prefix, time_t cur)
{
	char tmp[100], buf[33];
	snprintf(buf, sizeof(buf), "%ld", (unsigned long) cur);
	nvram_set(strcat_r(prefix, "wimax_check_time", tmp), buf);
}

static void
init_wmx_variables(char *prefix)
{
	char tmp[100];

	int t_interval = atoi(nvram_safe_get("wan_wimax_interval"));
	if (t_interval < 20 || t_interval > 3600 ) 
		nvram_set("wan_wimax_interval", "60");

	// Timestamp of the last connection checking
	nvram_unset(strcat_r(prefix, "wimax_check_time", tmp));
}

int
wimax_ifunit(char *ifname)
{
	if (strncmp(ifname, "wmx", 3))
		return -1;
	if (!isdigit(ifname[3]))
		return -1;
	return atoi(&ifname[3]);
}

int
start_wimax(char *prefix)
{
	char tmp[100];
	int unit = atoi(nvram_safe_get(strcat_r(prefix, "unit", tmp)));
	char *wimax_ifname = nvram_safe_get(strcat_r(prefix, "wimax_ifname", tmp));
	char *wimax_ssid = nvram_safe_get(strcat_r(prefix, "wimax_ssid", tmp));
	int ret = 0;

	char *wmx_argv[] = {
		"/usr/sbin/madwimax", "-qofd",
		"-i", wimax_ifname,
		"-e", wimax_events,
		"-p", (sprintf(tmp, "/var/run/madwimax%d.pid", unit), tmp),
		"-s", "/tmp/madwimax.log",
		"--ssid", *wimax_ssid ? wimax_ssid : "@yota.ru",
		NULL};

	dprintf( "%s", prefix );

	init_wmx_variables(prefix);
	update_nvram_wmx(wimax_ifname, 0);

	if (wimax_modem(prefix))
	{
		insmod("tun", NULL);
		sleep(1);
		symlink("/sbin/rc", wimax_events);
		ret = _eval(wmx_argv, NULL, 0, NULL);
	}

	nvram_set(strcat_r(prefix, "wimax_enabled", tmp), "1");

	return ret;
}

int
stop_wimax(char *prefix)
{
	char tmp[100];

	nvram_set(strcat_r(prefix, "wimax_enabled", tmp), "0");

	dprintf( "%s", prefix );

	killall("madwimax", 0);
	sleep(1);
	unlink(wimax_events);
	rmmod("tun");

	return 0;
}

//time_t prev_time = 0;
// called from watchdog using the "madwimax-check" symlink to rc.
int
madwimax_check(char *prefix)
{
	char tmp[100], pid_fname[100], buf[256];
	FILE *fp;
	pid_t pid = 0;

	int unit = atoi(nvram_safe_get(strcat_r(prefix, "unit", tmp)));

	if (!wimax_modem(prefix))
		return -1;

	if (!nvram_match(strcat_r(prefix, "wimax_enabled", tmp), "1"))
		return -1;

	sprintf(pid_fname, "/var/run/madwimax%d.pid", unit);

	fp = fopen(pid_fname, "r");
	if (fp && fgets(buf, sizeof(buf), fp))
	{
		pid = strtoul(buf, NULL, 0);
		fclose(fp);
	}

	/* Check connection */
	if (pid > 0 && is_chk_con())
	{
		// TODO check the process state
		//dprintf( "wmx_chk_con enabled, madwimad pid %d", pid );
		time_t cur_time = time(NULL);
		time_t prev_time = wmx_chk_get_last_time(prefix);

		if ((cur_time > prev_time + wmx_chk_interval()) ||
		    (cur_time < prev_time))
		{
			//if( is_chk_log() ) logmessage( "[wmx_checker]", "Ping gateway\n" );
			int ping_pid;
			char s_pid[20];
			char * wchk_argv[] = {
				"/usr/sbin/wimax_check_connection.sh",
				(snprintf(s_pid, sizeof(s_pid), "%d", pid), s_pid),
				nvram_safe_get(strcat_r(prefix, "wimax_ifname", tmp)),
				nvram_safe_get(strcat_r(prefix, "gateway", tmp)),
				NULL};

			_eval( wchk_argv, NULL, 0, &ping_pid);
			//prev_time = cur_time;
			wmx_chk_set_last_time(prefix, cur_time);

			fp = fopen(pid_fname, "r");
			if (fp && fgets(buf, sizeof(buf), fp))
			{
				pid = strtoul(buf, NULL, 0);
				fclose(fp);
			}
		}
	}

	// Stop UDHCPC if there's no madwimax running
	if (pid == 0)
	{
		if (is_chk_log())
			logmessage(nvram_safe_get("wan_proto_t"), "Restarting madwimax\n" );

	 	// kill the udhcpc
		kill_pidfile_s((sprintf(tmp, "/var/run/udhcpc%d.pid", unit), tmp), SIGUSR2);
		kill_pidfile((sprintf(tmp, "/var/run/udhcpc%d.pid", unit), tmp));

		start_wimax(prefix);
	}
	return 0;
}

//=========================== madwimax events ==================================

// if-create
int
madwimax_create(char *ifname)
{
	//char tmp[100];
	char prefix[] = "wanXXXXXXXXXX_";
	int unit = 0;

	dprintf( "%s", ifname );

	if ((unit = wimax_ifunit(ifname)) < 0)
		return -1;
	snprintf(prefix, sizeof(prefix), "wan%d_", unit);

	wmx_chk_set_last_time(prefix, time(NULL));

	//if (nvram_match("wan_proto", "wimax"))
	//	nvram_set("wan_ifname", ifname);

	//if (!wan_valid(ifname))
	//{
	//	strcpy(tmp, nvram_safe_get("wan_ifnames"));
	//	add_to_list(ifname, tmp, sizeof(tmp));
	//	nvram_set("wan_ifnames", tmp);
	//}

#ifndef ASUS_EXT
	/* Start connection dependent firewall */
	start_firewall2(ifname);
#endif


	return 0;
}

//if-release
int
madwimax_release(char *ifname)
{
	//char tmp[100];
	//char prefix[] = "wanXXXXXXXXXX_";
	int unit;

	dprintf( "%s", ifname );

	if ((unit = wimax_ifunit(ifname)) < 0)
		return -1;
	//snprintf(prefix, sizeof(prefix), "wan%d_", unit);

	//if (nvram_match(strcat_r(prefix, "primary", tmp), "1"))
	//{
	//	stop_wan();
	//}

	//if (wan_valid(ifname))
	//{
	//	strcpy(tmp, nvram_safe_get("wan_ifnames"));
	//	remove_from_list(ifname, tmp, sizeof(tmp));
	//	nvram_set("wan_ifnames", tmp);
	//}

	return 0;
}

//if-up
static int madwimax_up(char *ifname)
{
	char tmp[100];
	char prefix[] = "wanXXXXXXXXXX_";
	int unit;

	dprintf( "%s", ifname );

	if ((unit = wimax_ifunit(ifname)) < 0)
		return -1;
	snprintf(prefix, sizeof(prefix), "wan%d_", unit);

	wmx_chk_set_last_time(prefix, time(NULL));

	if (nvram_match(strcat_r(prefix, "wimax_ipaddr", tmp), "0.0.0.0"))
	{
		/* Start firewall */
		start_firewall_ex(ifname, "0.0.0.0", "br0", nvram_safe_get("lan_ipaddr"));
		/* Start dhcp daemon */
		start_dhcpc(ifname, unit);
		/* Update wan information for null DNS server */
		update_wan_status(1);
		wanmessage("Can not get IP from server");
	} else {
		/* Assign static IP address to i/f */
		ifconfig(ifname, IFUP,
			nvram_get(strcat_r(prefix, "ipaddr", tmp)),
			nvram_get(strcat_r(prefix, "netmask", tmp)), NULL);
                /* We are done configuration */
		wan_up(ifname);
		wanmessage("");
	}

	update_nvram_wmx(ifname, 1);
	logmessage(nvram_safe_get("wan_proto_t"), "connected to ISP");

	return 0;
}

//if-down
static int madwimax_down(char *ifname)
{
	char tmp[100];
	int unit;

	dprintf( "%s", ifname );

	if ((unit = wimax_ifunit(ifname)) < 0)
		return -1;

	wan_down(ifname);
	update_nvram_wmx(ifname, 0);

	preset_wan_routes(ifname);

	// kill the udhcpc
	kill_pidfile_s((sprintf(tmp, "/var/run/udhcpc%d.pid", unit), tmp), SIGUSR2);
	kill_pidfile((sprintf(tmp, "/var/run/udhcpc%d.pid", unit), tmp));

	logmessage(nvram_safe_get("wan_proto_t"), "Disconnected");
	wanmessage("Disconnected");

	return 0;
}

void
update_nvram_wmx(char *ifname, int isup)
{
	dprintf( "ifname: %s, is up: %d", ifname, isup );

	if (is_chk_con())
		nvram_set("wan_wimax_ping_t", "No data available");
	else
		nvram_set("wan_wimax_ping_t", "Disabled");
}

int 
hotplug_check_wimax( char * interface, char * product, char * prefix )
{
	char tmp[100];

	dprintf( "%s %s %s", interface, product, prefix );
	if (strncmp(product, "4e8/6761", 8) == 0 ||
	    strncmp(product, "4e9/6761", 8) == 0 ||
	    strncmp(product, "4e8/6731", 8) == 0 ||
	    strncmp(product, "4e8/6780", 8) == 0) {
		nvram_set(strcat_r(prefix, "usb_device_name", tmp), "Samsung CMC-730 chip");
		return 1;
	}
	else
		return 0;
};

int madwimax_main(int argc, char **argv)
{

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
