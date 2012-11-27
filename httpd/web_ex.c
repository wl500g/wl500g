/*
* Broadcom Home Gateway Reference Design
* Web Page Configuration Support Routines
*
* Copyright 2001, Broadcom Corporation
* All Rights Reserved.
*
* This is UNPUBLISHED PROPRIETARY SOURCE CODE of Broadcom Corporation;
* the contents of this file may not be disclosed to third parties, copied or
* duplicated in any form, in whole or in part, without the prior written
* permission of Broadcom Corporation.
*
* $Id: web_ex.c,v 1.5 2004/01/05 05:39:18 Cheni_Shen Exp $
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <errno.h>
#include <unistd.h>
#include <limits.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <signal.h>
#include <time.h>

#include <wlutils.h>
#include <typedefs.h>
#include <shutils.h>
#include <bcmconfig.h>
#include <bcmutils.h>
#include <bcmnvram.h>

#include "bcmnvram_f.h"
#include "common.h"
#include "httpd.h"

#define sys_upgrade(image)	eval("write", image, "linux")
#define sys_upload(image)	eval("nvram", "restore", image)
#define sys_download(file)	eval("nvram", "save", file)
//#define sys_restore(sid)	eval("nvram_x", "get", (sid))
#define sys_commit()		(flashfs_commit(), nvram_commit())
#define sys_default()		eval("erase", "nvram")

#define GROUP_FLAG_REFRESH 	0
#define GROUP_FLAG_DELETE 	1
#define GROUP_FLAG_ADD 		2
#define GROUP_FLAG_REMOVE 	3

#define IMAGE_HEADER 	"HDR0"
#define PROFILE_HEADER 	"HDR1"

extern void readPrnID();

static int apply_cgi_group(webs_t wp, int sid, const char *groupName, int flag);
static int nvram_add_group_table(webs_t wp, const struct group_variable *gv, int count);

static int ej_select_country(int eid, webs_t wp, int argc, char **argv);
static int wl_channels_in_country(char *abbrev, int channels[]);
static int wl_channels_in_country_asus(char *abbrev, int channels[]);

static int do_upload_file(const char *upload_file, const char *url,
			 FILE *stream, int len, const char *boundary_t,
			 int (*validate)(FILE *fifo, int len));


#define MAX_GROUP_COUNT 300

static char urlcache[128];
static const char *next_host;
static int delMap[MAX_GROUP_COUNT];
static char SystemCmd[128];
static char UserID[32]="";
static char UserPass[32]="";
static char ProductID[32]="";

extern int redirect;


#define ROOT_SSH_DIR     "/usr/local/root/.ssh"
#define SSH_AUTHORIZED_KEYS  ROOT_SSH_DIR"/authorized_keys"


static int
reltime(unsigned int seconds, char *buf)
{
	int off = 0;

	if (seconds > 60*60*24) {
		off += sprintf(buf + off, "%d days, ", seconds / (60*60*24));
		seconds %= 60*60*24;
	}
	if (seconds > 60*60) {
		off += sprintf(buf + off, "%d hours, ", seconds / (60*60));
		seconds %= 60*60;
	}
	if (seconds > 60) {
		off += sprintf(buf + off, "%d minutes, ", seconds / 60);
		seconds %= 60;
	}
	off += sprintf(buf + off, "%d seconds", seconds);

	return off;
}

/******************************************************************************/
/*
*	Redirect the user to another webs page
*/

static void websRedirect(webs_t wp, const char *url)
{
	//dprintf("Redirect to : %s\n", url);
	websWrite(wp, "<html><head>\r\n");
	if (next_host && *next_host)
		websWrite(wp, "<meta http-equiv=\"refresh\" content=\"0; url=http://%s/%s\">\r\n", next_host, url);
	else
		websWrite(wp, "<meta http-equiv=\"refresh\" content=\"0; url=%s\">\r\n", url);
	websWrite(wp, "<meta http-equiv=\"Content-Type\" content=\"text/html\">\r\n");
	websWrite(wp, "</head></html>\r\n");      

	websDone(wp, 200);
}

static void sys_script(const char *name)
{
	char scmd[64] = "/tmp/";

	strncat(scmd, name, sizeof(scmd)-1);
	dprintf("run '%s'\n", scmd);

	//handle special scripts first
	if (strcmp(name,"syscmd.sh")==0) {
		char *Cmd_argv[] = { "/bin/sh", "-c", SystemCmd, NULL };

		if (strcmp(SystemCmd, "")==0)
			Cmd_argv[2] = "echo None";
		_eval(Cmd_argv, ">/tmp/syscmd.log", 10, NULL);
		*SystemCmd = 0;
	} else if (strcmp(name, "syslog.sh")==0) {
		// to nothing
	} else if (strcmp(name, "printer.sh")==0) {
		readPrnID();
	} else if (strcmp(name, "lpr_remove")==0) {
		kill_pidfile_s("/var/run/lpdparent.pid", SIGUSR2);
		eval("killall","p910nd");
	}
#ifdef __CONFIG_MADWIMAX__
	else if (strcmp(name, "madwimax.sh")==0) {
		// update status of madwimax
		kill_pidfile_s("/var/run/madwimax0.pid", SIGUSR1);
		usleep(100);
	}
#endif
	else if (strcmp(name,"dnsmasq.sh")==0) {
		kill_pidfile_s("/var/run/dnsmasq.pid", SIGALRM);
		sleep(1);
	} else if (strcmp(name,"iptable.sh")==0) {
		// TODO	
	} else if (strcmp(name,"route.sh")==0) {
		// TODO
	} else if (strcmp(name,"radio_disable")==0) {
		eval("wl", "radio", "off");
	} else if (strcmp(name,"radio_enable")==0) {
		eval("wl", "radio", "on");
	} else if (strcmp(name,"dhcpc_renew")==0) {
		sys_renew();
		sleep(1);
	} else if (strcmp(name,"dhcpc_release")==0) {
		sys_release();
		sleep(1);
	} else if (strcmp(name,"eject-usb.sh")==0) {
		eval("rmstorage");
	} else if (strcmp(name,"ddnsclient")==0) {
		eval("start_ddns", "1");
	} else if (strstr(scmd, " ")==0) {
		// no parameter, run script with eval
		eval(scmd);
	} else
		system(scmd);
}


/* 
*   Byte 0 : Firmware Major
*   Byte 1 : Firmware Minor 
*   Byte 2 : File system Major
*   Byte 3 : File system Minor
*   Byte 4~15 : Product ID
*   Byte 16~31: Hardware compatible list( 4 pairs)
*      16+x*4: Minimum Hardware Major
*      17+x*4: Minimum Hardware Minor
*      18+x*4: Maximum Hardware Major
*      19+x*4: Maximum Hardware Minor
*      x= 0~3
*/
static char checkVersion(const char *version, unsigned char major, unsigned char minor)
{
#define VINT(ver) (((((ver)[0])&0xff)<<8)|(((ver)[1])&0xff))        
	int i;
	unsigned int ver, min, max;
	const char *clist;

	clist = version+16;
	ver = major << 8 | minor;

	//dprintf("ver: %d %d %d\n", major, minor, ver);
	i = 0;

	while (VINT(clist+i) && i<16)
	{
		min = VINT(clist+i); 	
		max = VINT(clist+i+2);

		//dprintf("List: %x %x %x %x %x %x %x %x\n", i, ver, min, max, clist[i], clist[i+1], clist[i+2], clist[i+3]);

		if (ver>=min && (max==0 || ver<=max))
			return 1; 
		i += 4;
	}
	if (i==0 || ver==0)
		return 1;
	else
		return 0;
}

static void websScan(const char *str)
{
	unsigned int i, flag;
	char *v1, *v2, *v3, *sp;
	char groupid[64];
	char value[MAX_LINE_SIZE];
	char name[MAX_LINE_SIZE];

	v1 = strchr(str, '?');

	i = 0;
	flag = 0;

	while (v1!=NULL) {
		v2 = strchr(v1+1, '=');
		v3 = strchr(v1+1, '&');

		if (v3!=NULL) {
			strncpy(value, v2+1, v3-v2-1);
			value[v3-v2-1] = 0;  
		} else {
			strcpy(value, v2+1);
		}

		strncpy(name, v1+1, v2-v1-1);
		name[v2-v1-1] = 0;
		/*dprintf("Value: %s %s\n", name, value);*/

		if (v2 != NULL && ((sp = strchr(v1+1, ' ')) == NULL || (sp > v2))) {
			if (flag && strncmp(v1+1, groupid, strlen(groupid))==0) {
				delMap[i] = atoi(value);
				//dprintf("Del Scan : %x\n", delMap[i]);
				if (delMap[i]==-1)  break;
				i++;
			} else if (strncmp(v1+1,"group_id", 8)==0) {
				sprintf(groupid, "%s_s", value);
				flag = 1;
			}   
		}
		v1 = strchr(v1+1, '&');
	} 
	delMap[i] = -1;
	return;
}

static int websWriteEscape(webs_t wp, char *buf)
{
	static const char *escapes = "\"&'<>";
	char *c;
	int ret = 0;

	for (c = buf; *c; c++) {
		if (strchr(escapes, *c))
			ret += websWrite(wp, "&#%d;", *c);
		else
			ret += websWrite(wp, "%c", *c);
	}
	return ret;
}

/*
* Example: 
* lan_ipaddr=192.168.1.1
* <% nvram_get("lan_ipaddr"); %> produces "192.168.1.1"
* <% nvram_get("undefined"); %> produces ""
*/
static int
ej_nvram_get(int eid, webs_t wp, int argc, char **argv)
{
	const char *name;

	if (ejArgs(argc, argv, "%s", &name) < 1) {
		websError(wp, 400, "Insufficient args\n");
		return -1;
	}

	return websWriteEscape(wp, nvram_safe_get(name));
}

/*
* Example: 
* wan_proto=dhcp
* <% nvram_match("wan_proto", "dhcp", "selected"); %> produces "selected"
* <% nvram_match("wan_proto", "static", "selected"); %> does not produce
*/
static int
ej_nvram_match(int eid, webs_t wp, int argc, char **argv)
{
	const char *name, *match, *output;

	if (ejArgs(argc, argv, "%s %s %s", &name, &match, &output) < 3) {
		websError(wp, 400, "Insufficient args\n");
		return -1;
	}

	if (nvram_match(name, match))
		return websWrite(wp, output);

	return 0;
}

static int
ej_nvram_double_match(int eid, webs_t wp, int argc, char **argv)
{
	const char *name, *match, *name2, *match2, *output;

	if (ejArgs(argc, argv, "%s %s %s %s %s", &name, &match, &name2, &match2, &output) < 5) {
		websError(wp, 400, "Insufficient args\n");
		return -1;
	}

	if (nvram_match(name, match) && nvram_match(name2, match2))
		return websWrite(wp, output);

	return 0;
}

static int
ej_nvram_match_both(int eid, webs_t wp, int argc, char **argv)
{
	const char *name, *match, *output, *output_not;

	if (ejArgs(argc, argv, "%s %s %s %s", &name, &match, &output, &output_not) < 4) {
		websError(wp, 400, "Insufficient args\n");
		return -1;
	}

	if (nvram_match(name, match))
		return websWrite(wp, output);
	else
		return websWrite(wp, output_not);
}

static int
ej_nvram_get_table(int eid, webs_t wp, int argc, char **argv)
{
	const char *name;
	const struct group_variable *gv;
	int i, groupCount, ret, r;

	if (ejArgs(argc, argv, "%s", &name) < 1) {
		websError(wp, 400, "Insufficient args\n");
		return -1;
	}

	gv = LookupGroup(name);
	if (gv == NULL)
		return 0;

	groupCount = nvram_get_int(gv->counter_name);
	if (groupCount==0)
		ret = nvram_add_group_table(wp, gv, -1);
	else {
		ret = 0;

		for (i=0; i<groupCount; i++) {
			r = nvram_add_group_table(wp, gv, i);
			if (r != 0)
				ret += r;
			else
				break;
		}
	}

	return ret;
}

/*
* Example: 
* wan_proto=dhcp;dns
* <% nvram_match_list("wan_proto", "dhcp", "selected", 0); %> produces "selected"
* <% nvram_match_list("wan_proto", "static", "selected", 1); %> does not produce
*/
static int
ej_nvram_match_list(int eid, webs_t wp, int argc, char **argv)
{
	const char *name, *match, *output;
	int which;

	if (ejArgs(argc, argv, "%s %s %s %d", &name, &match, &output, &which) < 4) {
		websError(wp, 400, "Insufficient args\n");
		return -1;
	}

	if (nvram_match_list(name, match, which))
		return websWrite(wp, output);
	else
		return 0;
}


static int
ej_select_channel(int eid, webs_t wp, int argc, char **argv)
{
	char chstr[32];
	int ret = 0;	
	int i, channel, channels[32];

	channel=nvram_get_int("wl_channel");

	if (!wl_channels_in_country(nvram_safe_get("wl_country_code"), channels)) {
		wl_channels_in_country_asus(nvram_safe_get("wl_country_code"), channels);
	}

	i = 0;

	while (channels[i]!=-1) {
		if (channels[i] == 0)
			strcpy(chstr, "Auto");
		else
			sprintf(chstr, "%d", channels[i]);

		if (channel==channels[i])
			ret += websWrite(wp, "<option value=\"%d\" selected>%s</option>", channels[i], chstr);
		else
			ret += websWrite(wp, "<option value=\"%d\">%s</option>", channels[i], chstr);
		i++;
	} 
	return ret;
}


static int
ej_urlcache(int eid, webs_t wp, int argc, char **argv)
{
	int flag;

	if (strcmp(urlcache, "Main_Operation.asp")==0)
		flag = 2;
	else if (strcmp(urlcache, "Main_Content.asp")==0)
		flag = 1;
	else
		flag = 0;

	dprintf("Url:%s %d\n", urlcache, flag);

	if (nvram_match("wan_route_x", "IP_Routed")) {
		if (nvram_match("wan_nat_x", "1")) {
			/* disable to redirect page */
			if (redirect) {
				websWrite(wp, "Basic_GOperation_Content.asp");
				redirect=0;
			} else {
				switch(flag) {
				case 2:
					websWrite(wp, "Basic_GOperation_Content.asp");
					break;
				case 1:
					websWrite(wp, "Basic_HomeGateway_SaveRestart.asp");
					break;
				default:
					websWrite(wp, "Main_Index_HomeGateway.asp");
					break;
				}
			}
		} else {
			/* disable to redirect page */
			if (redirect) {
				websWrite(wp, "Basic_ROperation_Content.asp");
				redirect=0;
			} else {
				switch(flag) {
				case 2:
					websWrite(wp, "Basic_ROperation_Content.asp");
					break;
				case 1:
					websWrite(wp, "Basic_Router_SaveRestart.asp");
					break;
				default:
					websWrite(wp, "Main_Index_Router.asp");
					break;
				}
			}
		}
	} else {
		switch(flag) {
		case 2:
			websWrite(wp, "Basic_AOperation_Content.asp");
			break;
		case 1:
			websWrite(wp, "Basic_AccessPoint_SaveRestart.asp");
			break;
		default:
			websWrite(wp, "Main_Index_AccessPoint.asp");
			break;
		}
	}
	strcpy(urlcache, "");

	return flag;
}


/* Report sys up time */
static int
ej_uptime(int eid, webs_t wp, int argc, char **argv)
{
	char buf[MAX_LINE_SIZE];
	char *str = file2str("/proc/uptime");
	time_t tm;
	struct tm tm1;
	int len;

	time(&tm);
	setenv("TZ", nvram_safe_get("time_zone"), 1);
	memcpy(&tm1, localtime(&tm), sizeof(tm1));
	len = strftime(buf, sizeof(buf)-1, "%a, %d %b %Y %H:%M:%S %z", &tm1);

	if (str != NULL) {
		unsigned long up = atol(str);
		free(str);

		strcpy(buf + len, " ("); len += 2;
		len += reltime(up, buf + len);
		strcpy(buf + len, " since boot)");
	}

	return websWrite(wp, buf);
}

static int
dump_file(webs_t wp, const char *filename)
{
	int  fd, n;
	char buf[2048 + 1];	/* aaa2ppp: imho size must be (n * minimum_io_size + 1) */
	int  ret = 0;

	fd = open(filename, O_RDONLY);
	if (fd == -1) {
		ret = -errno;
		dprintf("Can't open %s\n", filename);
		return 0; /* aaa2ppp: or return ret ? */
	}

	while ((n = read(fd, buf, (sizeof buf) - 1)) > 0) {
		ret += websWriteData(wp, buf, n);
	}
	close(fd);
	websDone(wp, 200);

	return ret;
}

static int
ej_dump(int eid, webs_t wp, int argc, char **argv)
{
	char filename[64] = "/tmp/";
	const char *file, *script;
	int ret;

	if (ejArgs(argc, argv, "%s %s", &file, &script) < 2) {
		websError(wp, 400, "Insufficient args\n");
		return -1;
	}
	dprintf("Script : %s, File: %s\n", script, file);

	// run scrip first to update some status
	if (strcmp(script, "") != 0)
		sys_script(script);

	ret = 0;

	if (strcmp(file, "wlan11a.log")==0)
		return ej_wl_status(eid, wp, 0, NULL);
	else if (strcmp(file, "iptable.log")==0) 
		return ej_nat_table(eid, wp, 0, NULL);
	else if (strcmp(file, "route.log")==0)
		return ej_route_table(eid, wp, 0, NULL);
	else if (strcmp(file, "syslog.log") == 0)
		ret += dump_file(wp, "/tmp/syslog.log-1");
	else if (strcmp(file, "ssh_keys.log") == 0)
		return dump_file(wp, SSH_AUTHORIZED_KEYS);

	strncat(filename, file, sizeof(filename)-1);
	ret += dump_file(wp, filename);

	return ret;
}

static int
ej_load(int eid, webs_t wp, int argc, char **argv)
{
	char *script;

	if (ejArgs(argc, argv, "%s", &script) < 1) {
		websError(wp, 400, "Insufficient args\n");
		return -1;
	}

	sys_script(script);
	return (websWrite(wp, "%s", ""));
}

static int
ej_include(int eid, webs_t wp, int argc, char **argv)
{
	char *file;

	if (ejArgs(argc, argv, "%s", &file) < 1) {
		websError(wp, 400, "Insufficient args\n");
		return -1;
	}

	do_ej(file, wp);
	return 0;
}

static void
validate_cgi(webs_t wp, int sid, int groupFlag)
{
	const struct variable *v;
	char *value;
	char name[64];

	/* Validate and set variables in table order */
	for (v = GetVariables(sid); v->name != NULL; v++) {
		//dprintf("Name: %s %d\n", v->name, sid);
		strcpy(name, v->name);

		if ((value = websGetVar(wp, name, NULL))) {
			nvram_set(v->name, value);
		}
	}
}


static char *svc_pop_list(char *value)
{
	char *v;

	if (value == NULL || (*value) == '\0')
		return NULL;

	v = strchr(value, ';');
	if (v != NULL)
	{
		*v = '\0';
		return value;
	}

	return NULL;
}


/* aaa2ppp: I would like so flashfs_update() to be */
#define flashfs_update() (nvram_set("flashfs_updated", "1"))

static int
flashfs_commit(void)
{
	if (nvram_match("flashfs_updated", "1")) {
		nvram_unset("flashfs_updated");	/* aaa2ppp: It must be unset on start device too! */
		eval("flashfs", "save");
		eval("flashfs", "commit");
		eval("flashfs", "enable");
	}

	return 0;
}

static int
update_authorized_keys(webs_t wp)
{
	char *flag, *keys;
	struct stf_opts  f_opts;
	int f_flags;
	int ret = 0;

	/* if user not wants to change keys then do nothing */
	flag = websGetVar(wp, "ssh_keys_change", "");
	if (strcmp (flag, "1") != 0)
		return 0;

	/* if keys is't changed then do nothing */
	flag = websGetVar(wp, "ssh_keys_changed", "");
	if (strcmp(flag, "1") != 0)
		return 0;

	keys = websGetVar(wp, "ssh_keys", NULL);

	/* if keys not defined then do nothing */
	if (keys == NULL)
		return 0;

	/* if keys is empty then delete authorized_keys file */
	if (*keys == '\0') {
		if (unlink(SSH_AUTHORIZED_KEYS) == 0)
			ret = flashfs_update();
		return ret;
	}

	/* else [re]write authorized_keys file */

	mkdir(ROOT_SSH_DIR, 0700);

	memset(&f_opts, 0, sizeof f_opts);
	f_flags = STF_CHMOD|STF_TRIM|STF_SKIP_0LINE;
	f_opts.mode = 0600;

	if ((ret = save_text_to_file(keys, SSH_AUTHORIZED_KEYS, f_flags, &f_opts)) == 0)
		ret = flashfs_update();

	return ret;
}


static int
apply_cgi(webs_t wp, const char *url, const char *path, const char *query)
{
	const char *value, *value1; 
	const char *current_url, *next_url;
	const char *script, *groupName;
	char urlStr[128];
	//    char *host;

	if (!query)
		goto footer;

	urlStr[0] = 0;

	value = websGetVar(wp, "action", "");         

	//if (strcmp(value, "")==0)
	value = websGetVar(wp, "action_mode","");

	next_host = websGetVar(wp, "next_host", "");   
	current_url = websGetVar(wp, "current_page", "");
	next_url = websGetVar(wp, "next_page", "");
	script = websGetVar(wp, "action_script",""); 


	dprintf("Apply: %s %s %s %s\n", value, current_url, next_url, websGetVar(wp, "group_id", ""));

	if ( !strcmp (current_url, "Advanced_Services_Content.asp") &&
		(!strcmp (value, " Apply ") || !strcmp (value, " Finish ")) ) {
		/* aaa2ppp: How to handle an error here? */
		update_authorized_keys(wp);
	}

	if (!strcmp(value," Refresh ")) {
		strcpy(SystemCmd, websGetVar(wp,"SystemCmd",""));
		websRedirect(wp, current_url);
		return 0;
	} else if (!strcmp(value," Clear ")) {
		// current only syslog implement this button
		unlink("/tmp/syslog.log-1");
		unlink("/tmp/syslog.log");
		websRedirect(wp, current_url);
		return 0;
	} else if (!strcmp(value,"NEXT")) {
		websRedirect(wp, next_url);
		return 0;
	} else if (!strcmp(value, "Save&Restart ")) {
		/*action = ACTION_SAVE_RESTART;*/
		/*websRedirect(wp, "Restarting.asp");*/
		dump_file(wp, "Restarting.asp");
		nvram_set("x_Setting", "1");
		sys_commit();
		sys_reboot();
		return (0);
	} else if(!strcmp(value, "Restore")) {
		/* action = ACTION_RESET_DEFAULT;*/
		/* websRedirect(wp, "Restarting.asp");*/
		dump_file(wp, "Restarting.asp");
		sys_default();
		sys_reboot();
		return (0);
	} else if (!strcmp(value, "WlanUpdate ")) {
		/*action = ACTION_SAVE_RESTART;*/
		/*websRedirect(wp, "Restarting.asp");*/
		if (strcmp(script, "")!=0) {
			system(script);
		}
		dump_file(wp, "Restarting.asp");
		sys_reboot();
		return (0);
	} else {
		int sid;
		char *sid_list;
		const char *serviceId;

		sid_list = websGetVar(wp, "sid_list", "");

		while ((serviceId = svc_pop_list(sid_list))) {
			sid_list += strlen(serviceId)+1;
			sid = LookupServiceId(serviceId);
			if (sid == -1)
				continue;

			if (!strcmp(value, " Restore ")) {
				//sys_restore(serviceId);
			} else if(!strcmp(value, "  Save  ") ||
				  !strcmp(value, " Apply ")) {
				validate_cgi(wp, sid, TRUE);
			} else if(!strcmp(value, "Set") ||
				  !strcmp(value, "Unset") ||
				  !strcmp(value, "Update") ||
				  !strcmp(value, "Eject")) {
				validate_cgi(wp, sid, TRUE);
			} else if(!strcmp(value," Finish ")) {
				validate_cgi(wp, sid, TRUE);
			} else {
				char groupId[64];

				dprintf("group ?\n");
				strcpy(groupId, websGetVar(wp, "group_id", ""));

				if ((value1 = websGetVar(wp, "action_mode", NULL))) {
					groupName = groupId;

					if (!strncmp(value1, " Delete ", 8)) {
						apply_cgi_group(wp, sid, groupName, GROUP_FLAG_DELETE);
					} else if (!strncmp(value1, " Add ", 5)) {
						apply_cgi_group(wp, sid, groupName, GROUP_FLAG_ADD);  
					} else if (!strncmp(value1, " Del ", 5)) {
						apply_cgi_group(wp, sid, groupName, GROUP_FLAG_REMOVE);
					} else if (!strncmp(value1, " Refresh ", 9)) {
						apply_cgi_group(wp, sid, groupName, GROUP_FLAG_REFRESH);
					}
					sprintf(urlStr, "%s#%s", current_url, groupName);

					validate_cgi(wp, sid, FALSE);
					break;
				}
			}
		}

		/* dprintf("apply????\n"); */

		/* Add for EMI Test page */
		if (strcmp(script, "")!=0) {
			sys_script(script);
		}   

		if (!strcmp(value, "  Save  ")) {
			strcpy(urlcache, next_url);
			websRedirect(wp, next_url);
		} else if (!strcmp(value, " Apply ")) {
			strcpy(urlcache, current_url);
			websRedirect(wp, current_url);
		} else if(!strcmp(value, " Finish ")) {
			websRedirect(wp, "SaveRestart.asp");
		} else if (urlStr[0]==0) {
			websRedirect(wp, current_url);
		} else
			websRedirect(wp, urlStr);

		dprintf("apply ok\n");
		return 0;
	}  



footer:
	websError(wp, 400, "Bad Request\n");
	websDone(wp, 400);

#ifndef FLASH_EMULATOR
	if (action == RESTART)
		sys_restart();
	else if (action == REBOOT)
		sys_reboot();
#endif

	return 1;
}

int
save_text_to_file(char *text, char *file, int flags, struct stf_opts *opts)
{
	int ret = 0;

	char real_path[PATH_MAX];
	char temp_file[PATH_MAX];

	int   fd;
	struct stat sb;
	mode_t mode;
	uid_t  uid;
	gid_t  gid;

	FILE *fs;
	size_t len;
	char *line, *line_end;
	int i;
	const char *eol = "\n";

	/* get real path of file if exists */
	if (realpath(file, real_path) != NULL)
		file = real_path;

	/* Make temp-file in the same directory */

	len = PATH_MAX-1;
	temp_file[len] = '\0';    
	strncpy(temp_file, file, len);

	len = PATH_MAX-1 - strlen(temp_file);
	strncat(temp_file, ".XXXXXX", len);

	fd = mkstemp(temp_file);
	if (fd == -1) {
		ret = errno;
		dprintf ("%s: Can't make temp file %s\n", __func__, temp_file);
		return ret;
	}

	/* mode/uid/gid by default */
	mode = 0600;
	uid  = geteuid();
	gid  = getegid();

	/* mode/uid/gid from options */
	if (opts != NULL) {    
		if (flags & STF_USE_MODE) mode = opts->mode;
		if (flags & STF_USE_UID)  uid  = opts->uid;
		if (flags & STF_USE_GID)  gid  = opts->gid;
	}

	/* mode/uid/gidif if old version of file is exists */ 
	if ((flags & STF_INHERIT_PERM) && stat(file, &sb) == 0) {
		if (flags & STF_INHERIT_MODE) mode = sb.st_mode;
		if (flags & STF_INHERIT_UID)  uid  = sb.st_uid;
		if (flags & STF_INHERIT_GID)  gid  = sb.st_gid;
	}

	/* chmod */
	if (/*(flags & STF_CHMOD) &&*/ fchmod(fd, mode) == -1) {
		ret = errno;
		dprintf("%s: Can't chmod %04o for %s\n", __func__, mode, temp_file);
		goto fail;
	}

	/* chown/chgrp */
	if ((flags & STF_CHOWN) && fchown(fd, uid, gid) == -1) {
		ret = errno;
		dprintf ("%s: Can't chown %d:%d for %s\n", __func__, uid, gid, temp_file);
		if (ret != EPERM || (flags & STF_CHOWN_PERM_ERROR))
			goto fail;
		ret = 0;
	}

	/* Save new version to temp-file */

	fs = fdopen(fd, "w");
	if (fs == NULL) {
		ret = errno;
		dprintf("%s: Can't fdopen for %s\n", __func__, temp_file);
		goto fail;
	}	

	if ((flags & STF_USE_EOL) && opts != NULL)
		eol = opts->eol;

	for (line = line_end = text, i = 1; line_end != NULL && *line != '\0'; line = line_end + 1) {
		/* search end of line */
		line_end = strchr(line, '\n');
		len = (line_end == NULL) ? strlen(line) : line_end - line;

		/* trim left blanks */ 
		if (flags & STF_LTRIM)
			for (; len > 0 && isspace(line[0]); len--, line++);

		/* trim rigth blanks */	    
		if (flags & STF_RTRIM)
			for (; len > 0 && isspace(line[len-1]); len--);

		/* skip empty line */	    
		if (flags & STF_SKIP_0LINE && len == 0)
			continue;

		/* print line number */	
		if (flags & STF_LINE_NUM) {
			if (fprintf(fs, "%d: ", i++) < 0) {
				ret = ferror(fs);
				goto fail_write;
			}
		}

		/* print the line contents */
		if (len > 0) {
			if (fwrite(line, len, 1, fs) == 0) {
				ret = ferror(fs);
				goto fail_write;
			}
		}

		/* finaly append eol */	
		if (fputs(eol, fs) < 0) {
			ret = ferror(fs);
			goto fail_write;
		}
	}

	if (fclose(fs) == EOF) {
		ret = errno;
		goto fail_close;
	}

	/* Atomically replace a file with the new version */    

	if (rename(temp_file, file) != 0) {
		ret = errno;
		dprintf("%s: Can't replace file %s", __func__, file);
		goto fail;
	}

	return ret;

fail_write:
	fclose(fs);        
fail_close:
	dprintf("%s: Write error to %s\n", __func__, temp_file);
fail:
	unlink(temp_file);
	return ret;
}


static void nvram_add_group_item(webs_t wp, const struct group_variable *gv, int sid)
{
	const struct variable *v;
	const char *value;
	char name[64];
	int count;

	if (gv->variables == NULL) {
		return;
	}

	count = nvram_get_int(gv->counter_name);
	dprintf("Grp count: %d\n", count);

	for (v = gv->variables; v->name != NULL; v++) {
		sprintf(name, "%s_0", v->name);

		if ((value = websGetVar(wp, name, NULL))) {
			nvram_add_lists(v->name, value, count);
			dprintf("Grp add: %s %s\n", v->name, value);
		} else {
			nvram_add_lists(v->name, "", count);
		}
	}

	nvram_set_int(gv->counter_name, ++count);
}


static void nvram_remove_group_item(webs_t wp, const struct group_variable *gv, int *delMap)
{
	const struct variable *v;
	int i, count;

	if (gv->variables == NULL) {
		return;
	}

	count = nvram_get_int(gv->counter_name);
	for (v = gv->variables; v->name != NULL; v++) {
		nvram_del_lists(v->name, delMap);
	}

	i = 0;
	while (delMap[i] != -1)
		i++;
	count -= i;

	nvram_set_int(gv->counter_name, count);
}

/* Rule for table: 
* 1. First field can not be NULL, or it is treat as empty
* 2. 
*/

static int 
nvram_add_group_table(webs_t wp, const struct group_variable *gv, int count)
{
	const struct variable *v;
	char buf[MAX_LINE_SIZE*2];
	char bufs[MAX_LINE_SIZE*2];
	int i, j, fieldLen, fieldCount, value;
	const char hard_space[] = "&nbsp;";

	if (gv->variables == NULL)
		return 0;

	bufs[0] = 0x0;

	if (count == -1) {
		memset(bufs, ' ', gv->rowLen);
		value = -1;
		bufs[gv->rowLen] = 0x0;

		goto ToHTML;
	}

	fieldCount = 0;
	value = count;

	for (v = gv->variables; v->name != NULL; v++) {
		strcpy(buf, nvram_get_list(v->name, count));
		fieldLen = v->fldLen;

		if (fieldLen > 0) {
			i = strlen(buf);
			if (i < fieldLen) {
				memset(buf+i, ' ', fieldLen-i);
			}
			buf[fieldLen] = 0x0;
		}   

		if (fieldCount == 0)
			strcpy(bufs, buf);
		else
			strcat(bufs, buf);

		fieldCount++;
	}

	if (strlen(bufs) > gv->rowLen)
		bufs[gv->rowLen] = 0x0;

ToHTML:
	/* Replace ' ' to &nbsp; */
	for (i=0,j=0; i<strlen(bufs); i++) {
		if (bufs[i] == ' ') {
			memcpy(&buf[j], hard_space, sizeof(hard_space)-1);
			j += sizeof(hard_space)-1;
		} else
			buf[j++] = bufs[i];
	}
	buf[j] = 0x0;

	return (websWrite(wp, "<option value=\"%d\">%s</option>", value, buf));
}

static int
apply_cgi_group(webs_t wp, int sid, const char *groupName, int flag)
{
	const struct group_variable *gv;

	dprintf("This group '%s' limit is %d (sid=%d)\n", groupName, flag, sid);

	gv = LookupGroup(groupName);
	if (gv == NULL)
		return 0;

	if (flag == GROUP_FLAG_ADD)/* if (!strcmp(value, " Refresh ")) || Save condition */
	{    
		nvram_add_group_item(wp, gv, sid);
	} else if (flag == GROUP_FLAG_REMOVE)/* if (!strcmp(value, " Refresh ")) || Save condition */
	{
		/*nvram_remove_group_item(wp, v, sid); 	*/
		/*sprintf(item, "%s_s", websGetVar(wp, "group_id", ""));*/
		/*value1 = websGetVar(wp, item, NULL);*/
		/* TO DO : change format from 'x=1&x=2&...' to 'x=1 2 3 ..'*/
		nvram_remove_group_item(wp, gv, delMap);
	}
	return 1;
}

static void
do_auth(char *userid, char *passwd, char *realm)
{
	if (strcmp(ProductID,"")==0) {
		strncpy(ProductID, nvram_safe_get("productid"), 32);
	}
	if (strcmp(UserID,"")==0) {
		strncpy(UserID, nvram_safe_get("http_username"), 32);
	}
	if (strcmp(UserPass,"")==0) {
		strncpy(UserPass, nvram_safe_get("http_passwd"), 32);
	}
	//dprintf("User: %s:%s%s\n", ProductID, UserID, UserPass);
	strncpy(userid, UserID, AUTH_MAX);

	if (redirect || !is_auth()) {
		//dprintf("Disable password checking!!!\n");
		strcpy(passwd, "");
	} else {
		strncpy(passwd, UserPass, AUTH_MAX);
	}
	strncpy(realm, ProductID, AUTH_MAX);
}


static void
do_apply_cgi(char *url, FILE *stream)
{
	char *path, *query;

	dprintf(" Before Apply : %s\n", url);

	websScan(url);
	query = url;
	path = strsep(&query, "?") ? : url;
#ifndef HANDLE_POST
	init_cgi(query);
#endif
	apply_cgi(stream, url, path, query);
#ifndef HANDLE_POST
	init_cgi(NULL);
#endif
}

static void
do_apply_cgi_post(const char *url, FILE *stream, int len, const char *boundary)
{
	//dprintf("In : %s %s\n", url, boundary);
#ifdef HANDLE_POST
	static char post_buf[10000];

	if (!fgets(post_buf, MIN(len+1, sizeof(post_buf)), stream))
		return;
	len -= strlen(post_buf);
	init_cgi(post_buf);
	while (len--)
		fgetc(stream);
#endif
}


static void
do_webcam_cgi(char *url, FILE *stream)
{
#define MAX_RECORD_IMG 12
	char *query, *path;
	char pic[32];
	int query_idx, last_idx;

	query = url;
	path = strsep(&query, "?") ? : url;
	dprintf("WebCam CGI\n");
	//ret = fcntl(fileno(stream), F_GETOWN, 0);
	query_idx = atoi(query);
	last_idx = nvram_get_int("WebPic");

	if (query_idx==0)
		sprintf(pic, "../var/tmp/display.jpg");
	else
		sprintf(pic, "../var/tmp/record%d.jpg", (query_idx>last_idx+1) ? (last_idx+1+MAX_RECORD_IMG-query_idx):(last_idx+1-query_idx));

	dprintf("\nWebCam: %s\n", pic);
	do_file(pic, stream);
}

#ifndef FLASH_DIRECT
static int chk_fw_image(FILE *fifo, int len)
{
	char hdr_buf[8];
	int rd;
#define MAX_VERSION_LEN 64
	char version[MAX_VERSION_LEN];
	int hwmajor=0, hwminor=0;

	fseek(fifo, 0, SEEK_SET);
	rd = fread(hdr_buf, 1, 4, fifo);
	if (rd != 4 || strncmp(hdr_buf, IMAGE_HEADER, 4) != 0)
		return -1;

	memset(version, 0, MAX_VERSION_LEN);
	fseek(fifo, -MAX_VERSION_LEN, SEEK_END);
	fread(version, 1, MAX_VERSION_LEN, fifo);
	sscanf(nvram_safe_get("HardwareVer"), "%d.%d", &hwmajor, &hwminor);
	cprintf("Hardware : %d.%d %s\n", hwmajor, hwminor, version+4);

	if (((strncmp(ProductID, version+4, strlen(ProductID))==0 &&
		strncmp(version+4, "WL500gx", 7)!=0) || 
		(strncmp(ProductID, "WL500g.Deluxe", 13)==0 &&
		strncmp(version+4, "WL500gx", 7)==0))
		&& checkVersion(version, hwmajor, hwminor))
	{
		cprintf("FW image ok\n");
		return 0;
	}

	return -2;
}
#endif

static void
do_upgrade_post(const char *url, FILE *stream, int len, const char *boundary)
{
	const char upload_file[] = "/tmp/linux.trx";
#ifdef FLASH_DIRECT
	char *argv[] = {"write", upload_file, "linux", NULL};
	pid_t pid;
	int ret;
#endif

	eval("stopservice");

#ifdef FLASH_DIRECT
	signal(SIGTERM, SIG_IGN);
	signal(SIGINT, SIG_IGN);
	signal(SIGHUP, SIG_IGN);
	signal(SIGQUIT, SIG_IGN);

	ret = mkfifo(upload_file, S_IRWXU);
	if (ret != 0) goto err;

	ret = _eval(argv, ">/dev/null", 0, &pid);
	if (ret != 0) goto err;

	cprintf("Flashing new FW image\n");
	ret = do_upload_file(upload_file, url, stream, len, boundary, NULL);

err:
	fcntl(fileno(stream), F_SETOWN, -ret);
	if (pid != -1) waitpid(pid, &ret, 0);
#else
	do_upload_file(upload_file, url, stream, len, boundary, chk_fw_image);
#endif
}

static void
do_upgrade_cgi(char *url, FILE *stream)
{
	int ret;

	ret = fcntl(fileno(stream), F_GETOWN, 0);

	/* Reboot if successful */
	if (ret == 0) {
#ifndef FLASH_DIRECT
		dump_file(stream, "Updating.asp"); 
		cprintf("Flashing new FW image\n");
		sys_upgrade("/tmp/linux.trx");
#endif
		sys_forcereboot();
	} else {
		dump_file(stream, "UpdateError.asp");
		unlink("/tmp/linux.trx");
	}
}

static int chk_profile_hdr(FILE *fifo, int len)
{
	char hdr_buf[8];
	int rd;

	fseek(fifo, 0, SEEK_SET);
	rd = fread(hdr_buf, 1, 4, fifo);
	if (rd != 4 || strncmp(hdr_buf, PROFILE_HEADER, 4) != 0)
		return -1;
	return 0;
}

static void
do_upload_post(const char *url, FILE *stream, int len, const char *boundary)
{
	const char upload_file[] = "/tmp/settings_u.prf";

	if (do_upload_file(upload_file, url, stream, len, boundary, chk_profile_hdr) == 0)
		eval("stopservice");
}

static void
do_upload_cgi(char *url, FILE *stream)
{
	int ret;

	//dprintf("Upgrade CGI\n");

	ret = fcntl(fileno(stream), F_GETOWN, 0);

	/* Reboot if successful */
	if (ret == 0) {
		dump_file(stream, "Uploading.asp"); 
		sys_upload("/tmp/settings_u.prf");
		sys_commit();
		sys_reboot();
	} else {
		dump_file(stream, "UploadError.asp");
		//unlink("/tmp/settings_u.prf");
	}
}


static void
do_prf_file(char *url, FILE *stream)
{
	sys_commit();
	sys_download("/tmp/settings");
	do_file("/tmp/settings", stream);
	//unlink("/tmp/settings");
}


static int
do_upload_file(const char *upload_file, const char *url, FILE *stream,
		int len, const char *boundary_t,
		int (*validate)(FILE *fifo, int len))
{
	FILE *fifo = NULL;
	char buf[1024];
	int total = 0, count, ret = EINVAL;
	int boundary = strlen(boundary_t)+8;

	dprintf("Start upload\n");
	/* Look for our part */
	while (len > 0) {
		if (!fgets(buf, MIN(len + 1, sizeof(buf)), stream)) {
			goto err;
		}

		len -= strlen(buf);

		if (!strncasecmp(buf, "Content-Disposition:", 20) &&
			strstr(buf, "name=\"file\""))
			break;
	}

	/* Skip boundary and headers */
	while (len > 0) {
		if (!fgets(buf, MIN(len + 1, sizeof(buf)), stream)) {
			goto err;
		}
		len -= strlen(buf);
		if (!strcmp(buf, "\n") || !strcmp(buf, "\r\n"))	{
			break;
		}
	}

	if (!(fifo = fopen(upload_file, "w+"))) goto err;

	len -= boundary;
	dprintf("Uploading %d\n", len);

	/* Pipe the rest to the FIFO */
	while (len > 0) {
		count = fread(buf, 1, MIN(len, sizeof(buf)), stream);
		if (fwrite(buf, 1, count, fifo) != count) goto err;
		len -= count;
		total += count;
	}
	fread(buf, 1, boundary, stream);
	fflush(fifo);

	if (validate)
		ret = (*validate)(fifo, total);
	else
		ret = 0;

err:
	if (fifo)
		fclose(fifo);

	//dprintf("Error : %d\n", ret);
	fcntl(fileno(stream), F_SETOWN, -ret);

	return ret;
}

static void
do_uploadflashfs_post(const char *url, FILE *stream, int len, const char *boundary)
{
	const char upload_file[] = "/tmp/flash.tar.gz";

	do_upload_file(upload_file, url, stream, len, boundary, NULL);

	if (eval("tar", "ztf", upload_file) != 0)
		fcntl(fileno(stream), F_SETOWN, -EINVAL);
}

static void
do_uploadflashfs_cgi(char *url, FILE *stream)
{
	int ret;

	//dprintf("Upgrade CGI\n");
	ret = fcntl(fileno(stream), F_GETOWN, 0);
	/* Reboot if successful */
	if (ret == 0) {
		dump_file(stream, "Uploading.asp"); 
		eval("flashfs","commit");
		eval("flashfs","enable");
		sys_reboot();
	} else {
		dump_file(stream, "UploadError.asp");
		//unlink("/tmp/settings_u.prf");
	}

}

static void
do_flashfs_file(char *url, FILE *stream)
{

	eval("flashfs", "load");
	eval("flashfs", "save");
	do_file("/tmp/flash.tar.gz", stream);
	//unlink("/tmp/settings");
}


static const char no_cache[] =
"Cache-Control: no-cache\r\n"
"Pragma: no-cache\r\n"
"Expires: 0"
;


static void 
do_log_cgi(char *path, FILE *stream)
{
	dump_file(stream, "/tmp/syslog.log-1");
	dump_file(stream, "/tmp/syslog.log");
	fputs("\r\n", stream); /* terminator */
	fputs("\r\n", stream); /* terminator */
}

struct cpu_jiff {
	/* Linux 2.4.x has only first four */
	unsigned long long user;
	unsigned long long nice;
	unsigned long long sys;
	unsigned long long idle;
	unsigned long long iowait;
	unsigned long long irq;
	unsigned long long softirq;
	unsigned long long steal;
	unsigned long long guest;
};

static void
do_cpustat(char *url, FILE *stream)
{
	FILE *fp;
	char line[80];
	struct cpu_jiff cpu;

	fp = fopen("/proc/stat", "rb");
	if (fp == NULL)
		return;
	memset(&cpu, 0, sizeof(cpu));

	if (fgets(line, sizeof(line), fp)
	 && sscanf(line, "cpu %llu %llu %llu %llu %llu %llu %llu %llu %llu",
	    &cpu.user, &cpu.nice, &cpu.sys, &cpu.idle,
	    &cpu.iowait, &cpu.irq, &cpu.softirq, &cpu.steal, &cpu.guest) >= 4)
	{
		/* procps 2.x does not count iowait as busy time */
		cpu.user += cpu.nice;
		cpu.sys += cpu.irq + cpu.softirq + cpu.steal + cpu.guest;
		cpu.idle += cpu.iowait;
		websWrite(stream, "%llu %llu %llu", cpu.user, cpu.sys, cpu.idle);
	}
	fclose(fp);
}

static void
do_fetchif(char *url, FILE *stream)
{
	FILE *in;
	char line[256];
	char buffer[256];
	char *path, *query;
	int buflen = 0;
	time_t tm;
	struct tm tm_time;

	query = url;
	if (query == NULL || strlen(query) == 0)
		return;

	path = strsep(&query, "?") ? : url;
	time(&tm);
	memcpy(&tm_time, localtime(&tm), sizeof(tm_time));
	buflen = strftime(buffer, sizeof(buffer)-1, "%a %b %e %H:%M:%S %Z %Y", &tm_time);
	buffer[buflen++] = '\n';

	in = fopen("/proc/net/dev", "rb");
	if (in == NULL)
		return;
	while (fgets(line, sizeof(line), in) != NULL) {
		if (!strchr(line, ':'))
			continue;
		if (strstr(line, query)) {
			int llen = strlen(line);
			if (buflen + llen < sizeof(buffer)) {
				strcpy(buffer + buflen, line);
				buflen += llen;
			}
			break;
		}
	}
	fclose(in);
	buffer[buflen] = '\0';

	websWriteData(stream, buffer, buflen);
	websDone(stream, 200);
}

static void
do_svgfile(char *url, FILE *stream)
{
	char *path, *query;

	query = url;
	path = strsep(&query, "?") ? : url;
	do_file(path, stream);
}


const struct mime_handler mime_handlers[] = {
#ifdef USE_JSON
	{ "**.asp", "text/html", no_cache, NULL, do_ej_ex, do_auth },
#else
	{ "**.asp", "text/html", no_cache, NULL, do_ej, do_auth },
#endif
	{ "**.htm", "text/html", no_cache, NULL, do_ej, do_auth },
	{ "**.css", "text/css", NULL, NULL, do_file, do_auth },
	{ "**.gif", "image/gif", NULL, NULL, do_file, do_auth },
	{ "**.jpg", "image/jpeg", NULL, NULL, do_file, do_auth },
	{ "**.ico", "image/x-icon", NULL, NULL, do_file, do_auth },
	{ "**.png", "image/png", NULL, NULL, do_file, do_auth },
	{ "**.js",  "application/x-javascript", NULL, NULL, do_file, do_auth },
	{ "**.cfg", "application/octet-stream", NULL, NULL, do_prf_file, do_auth },
	{ "**.tar.gz", "application/x-gzip", NULL, NULL, do_flashfs_file, do_auth },
	{ "apply.cgi*", "text/html", no_cache, do_apply_cgi_post, do_apply_cgi, do_auth },
	{ "upgrade.cgi*", "text/html", no_cache, do_upgrade_post, do_upgrade_cgi, do_auth},
	{ "upload.cgi*", "text/html", no_cache, do_upload_post, do_upload_cgi, do_auth },
	{ "uploadflashfs.cgi*", "text/html", no_cache, do_uploadflashfs_post, do_uploadflashfs_cgi, do_auth },
	{ "syslog.cgi*", "text/plain", no_cache, NULL, do_log_cgi, do_auth },
	{ "webcam.cgi*", "text/html", no_cache, NULL, do_webcam_cgi, do_auth },
	{ "**.svg*", "image/svg+xml", NULL, NULL, do_svgfile, do_auth },
	{ "fetchif.cgi*", "text/html", no_cache, NULL, do_fetchif, do_auth },
	{ "cpu.cgi*", "text/html", no_cache, NULL, do_cpustat, do_auth },
#ifdef USE_JSON
	{ "json_get.cgi", "application/json", no_cache, NULL, do_json_get, do_auth },
	{ "json_set.cgi", "application/json", no_cache, do_json_set, NULL, do_auth },
	{ "json.cgi", "application/json", no_cache, do_json_set, do_json_get, do_auth },
#endif
	{ NULL, NULL, NULL, NULL, NULL, NULL }
};


const struct ej_handler ej_handlers[] = {
#ifdef USE_JSON
	{ "nvram_get_json", ej_nvram_get_json },
	{ "nvram_get_n_json", ej_nvram_get_n_json },
#endif
	{ "nvram_get", ej_nvram_get },
	{ "nvram_get_table", ej_nvram_get_table },
	{ "nvram_match", ej_nvram_match },
	{ "nvram_double_match", ej_nvram_double_match },
	{ "nvram_match_both", ej_nvram_match_both },
	{ "nvram_match_list", ej_nvram_match_list },
	{ "select_channel", ej_select_channel },
	{ "select_country", ej_select_country },
	{ "urlcache", ej_urlcache },
	{ "uptime", ej_uptime },
	{ "nvram_dump", ej_dump },
	{ "load_script", ej_load },
	{ "include", ej_include },
	{ NULL, NULL }
};


/*
* Country names and abbreviations from ISO 3166
*/
typedef struct {
	char *name;     /* Long name */
	char *abbrev;   /* Abbreviation */
} country_name_t;


/*
* Country names and abbreviations from ISO 3166
*/
static const country_name_t country_names[] = {

	{"AFGHANISTAN",		 "AF"},
	{"ALBANIA",		 "AL"},
	{"ALGERIA",		 "DZ"},
	{"AMERICAN SAMOA", 	 "AS"},
	{"ANDORRA",		 "AD"},
	{"ANGOLA",		 "AO"},
	{"ANGUILLA",		 "AI"},
	{"ANTARCTICA",		 "AQ"},
	{"ANTIGUA AND BARBUDA",	 "AG"},
	{"ARGENTINA",		 "AR"},
	{"ARMENIA",		 "AM"},
	{"ARUBA",		 "AW"},
	{"AUSTRALIA",		 "AU"},
	{"AUSTRIA",		 "AT"},
	{"AZERBAIJAN",		 "AZ"},
	{"BAHAMAS",		 "BS"},
	{"BAHRAIN",		 "BH"},
	{"BANGLADESH",		 "BD"},
	{"BARBADOS",		 "BB"},
	{"BELARUS",		 "BY"},
	{"BELGIUM",		 "BE"},
	{"BELIZE",		 "BZ"},
	{"BENIN",		 "BJ"},
	{"BERMUDA",		 "BM"},
	{"BHUTAN",		 "BT"},
	{"BOLIVIA",		 "BO"},
	{"BOSNIA AND HERZEGOVINA","BA"},
	{"BOTSWANA",		 "BW"},
	{"BOUVET ISLAND",	 "BV"},
	{"BRAZIL",		 "BR"},
	{"BRITISH INDIAN OCEAN TERRITORY", 	"IO"},
	{"BRUNEI DARUSSALAM",	 "BN"},
	{"BULGARIA",		 "BG"},
	{"BURKINA FASO",	 "BF"},
	{"BURUNDI",		 "BI"},
	{"CAMBODIA",		 "KH"},
	{"CAMEROON",		 "CM"},
	{"CANADA",		 "CA"},
	{"CAPE VERDE",		 "CV"},
	{"CAYMAN ISLANDS",	 "KY"},
	{"CENTRAL AFRICAN REPUBLIC","CF"},
	{"CHAD",		 "TD"},
	{"CHILE",		 "CL"},
	{"CHINA",		 "CN"},
	{"CHRISTMAS ISLAND",	 "CX"},
	{"COCOS (KEELING) ISLANDS","CC"},
	{"COLOMBIA",		 "CO"},
	{"COMOROS",		 "KM"},
	{"CONGO",		 "CG"},
	{"CONGO, THE DEMOCRATIC REPUBLIC OF THE", "CD"},
	{"COOK ISLANDS",	 "CK"},
	{"COSTA RICA",		 "CR"},
	{"COTE D'IVOIRE",	 "CI"},
	{"CROATIA",		 "HR"},
	{"CUBA",		 "CU"},
	{"CYPRUS",		 "CY"},
	{"CZECH REPUBLIC",	 "CZ"},
	{"DENMARK",		 "DK"},
	{"DJIBOUTI",		 "DJ"},
	{"DOMINICA",		 "DM"},
	{"DOMINICAN REPUBLIC", 	 "DO"},
	{"ECUADOR",		 "EC"},
	{"EGYPT",		 "EG"},
	{"EL SALVADOR",		 "SV"},
	{"EQUATORIAL GUINEA",	 "GQ"},
	{"ERITREA",		 "ER"},
	{"ESTONIA",		 "EE"},
	{"ETHIOPIA",		 "ET"},
	{"FALKLAND ISLANDS (MALVINAS)",	"FK"},
	{"FAROE ISLANDS",	 "FO"},
	{"FIJI",		 "FJ"},
	{"FINLAND",		 "FI"},
	{"FRANCE",		 "FR"},
	{"FRENCH GUIANA",	 "GF"},
	{"FRENCH POLYNESIA",	 "PF"},
	{"FRENCH SOUTHERN TERRITORIES",	 "TF"},
	{"GABON",		 "GA"},
	{"GAMBIA",		 "GM"},
	{"GEORGIA",		 "GE"},
	{"GERMANY",		 "DE"},
	{"GHANA",		 "GH"},
	{"GIBRALTAR",		 "GI"},
	{"GREECE",		 "GR"},
	{"GREENLAND",		 "GL"},
	{"GRENADA",		 "GD"},
	{"GUADELOUPE",		 "GP"},
	{"GUAM",		 "GU"},
	{"GUATEMALA",		 "GT"},
	{"GUINEA",		 "GN"},
	{"GUINEA-BISSAU",	 "GW"},
	{"GUYANA",		 "GY"},
	{"HAITI",		 "HT"},
	{"HEARD ISLAND AND MCDONALD ISLANDS",	"HM"},
	{"HOLY SEE (VATICAN CITY STATE)", 	"VA"},
	{"HONDURAS",		 "HN"},
	{"HONG KONG",		 "HK"},
	{"HUNGARY",		 "HU"},
	{"ICELAND",		 "IS"},
	{"INDIA",		 "IN"},
	{"INDONESIA",		 "ID"},
	{"IRAN, ISLAMIC REPUBLIC OF",		"IR"},
	{"IRAQ",		 "IQ"},
	{"IRELAND",		 "IE"},
	{"ISRAEL",		 "IL"},
	{"ITALY",		 "IT"},
	{"JAMAICA",		 "JM"},
	{"JAPAN",		 "JP"},
	{"JORDAN",		 "JO"},
	{"KAZAKHSTAN",		 "KZ"},
	{"KENYA",		 "KE"},
	{"KIRIBATI",		 "KI"},
	{"KOREA, DEMOCRATIC PEOPLE'S REPUBLIC OF", "KP"},
	{"KOREA, REPUBLIC OF",	 "KR"},
	{"KUWAIT",		 "KW"},
	{"KYRGYZSTAN",		 "KG"},
	{"LAO PEOPLE'S DEMOCRATIC REPUBLIC", 	"LA"},
	{"LATVIA",		 "LV"},
	{"LEBANON",		 "LB"},
	{"LESOTHO",		 "LS"},
	{"LIBERIA",		 "LR"},
	{"LIBYAN ARAB JAMAHIRIYA","LY"},
	{"LIECHTENSTEIN",	 "LI"},
	{"LITHUANIA",		 "LT"},
	{"LUXEMBOURG",		 "LU"},
	{"MACAO",		 "MO"},
	{"MACEDONIA",	 "MK"},
	{"MADAGASCAR",		 "MG"},
	{"MALAWI",		 "MW"},
	{"MALAYSIA",		 "MY"},
	{"MALDIVES",		 "MV"},
	{"MALI",		 "ML"},
	{"MALTA",		 "MT"},
	{"MARSHALL ISLANDS",	 "MH"},
	{"MARTINIQUE",		 "MQ"},
	{"MAURITANIA",		 "MR"},
	{"MAURITIUS",		 "MU"},
	{"MAYOTTE",		 "YT"},
	{"MEXICO",		 "MX"},
	{"MICRONESIA", 		 "FM"},
	{"MOLDOVA, REPUBLIC OF", "MD"},
	{"MONACO",		 "MC"},
	{"MONGOLIA",		 "MN"},
	{"MONTSERRAT",		 "MS"},
	{"MOROCCO",		 "MA"},
	{"MOZAMBIQUE",		 "MZ"},
	{"MYANMAR",		 "MM"},
	{"NAMIBIA",		 "NA"},
	{"NAURU",		 "NR"},
	{"NEPAL",		 "NP"},
	{"NETHERLANDS",		 "NL"},
	{"NETHERLANDS ANTILLES", "AN"},
	{"NEW CALEDONIA",	 "NC"},
	{"NEW ZEALAND",		 "NZ"},
	{"NICARAGUA",		 "NI"},
	{"NIGER",		 "NE"},
	{"NIGERIA",		 "NG"},
	{"NIUE",		 "NU"},
	{"NORFOLK ISLAND",	 "NF"},
	{"NORTHERN MARIANA ISLANDS","MP"},
	{"NORWAY",		 "NO"},
	{"OMAN",		 "OM"},
	{"PAKISTAN",		 "PK"},
	{"PALAU",		 "PW"},
	{"PALESTINIAN TERRITORY, OCCUPIED", 	"PS"},
	{"PANAMA",		 "PA"},
	{"PAPUA NEW GUINEA",	 "PG"},
	{"PARAGUAY",		 "PY"},
	{"PERU",		 "PE"},
	{"PHILIPPINES",		 "PH"},
	{"PITCAIRN",		 "PN"},
	{"POLAND",		 "PL"},
	{"PORTUGAL",		 "PT"},
	{"PUERTO RICO",		 "PR"},
	{"QATAR",		 "QA"},
	{"REUNION",		 "RE"},
	{"ROMANIA",		 "RO"},
	{"RUSSIAN FEDERATION",	 "RU"},
	{"RWANDA",		 "RW"},
	{"SAINT HELENA",	 "SH"},
	{"SAINT KITTS AND NEVIS","KN"},
	{"SAINT LUCIA",		 "LC"},
	{"SAINT PIERRE AND MIQUELON",	 	"PM"},
	{"SAINT VINCENT AND THE GRENADINES", 	"VC"},
	{"SAMOA",		 "WS"},
	{"SAN MARINO",		 "SM"},
	{"SAO TOME AND PRINCIPE","ST"},
	{"SAUDI ARABIA",	 "SA"},
	{"SENEGAL",		 "SN"},
	{"SEYCHELLES",		 "SC"},
	{"SIERRA LEONE",	 "SL"},
	{"SINGAPORE",		 "SG"},
	{"SLOVAKIA",		 "SK"},
	{"SLOVENIA",		 "SI"},
	{"SOLOMON ISLANDS",	 "SB"},
	{"SOMALIA",		 "SO"},
	{"SOUTH AFRICA",	 "ZA"},
	{"SOUTH GEORGIA", "GS"},
	{"SPAIN",		 "ES"},
	{"SRI LANKA",		 "LK"},
	{"SUDAN",		 "SD"},
	{"SURINAME",		 "SR"},
	{"SVALBARD AND JAN MAYEN","SJ"},
	{"SWAZILAND",		 "SZ"},
	{"SWEDEN",		 "SE"},
	{"SWITZERLAND",		 "CH"},
	{"SYRIAN ARAB REPUBLIC", "SY"},
	{"TAIWAN, PROVINCE OF CHINA", 		"TW"},
	{"TAJIKISTAN",		 "TJ"},
	{"TANZANIA",		"TZ"},
	{"THAILAND",		 "TH"},
	{"TIMOR-LESTE",		 "TL"},
	{"TOGO",		 "TG"},
	{"TOKELAU",		 "TK"},
	{"TONGA",		 "TO"},
	{"TRINIDAD AND TOBAGO",	 "TT"},
	{"TUNISIA",		 "TN"},
	{"TURKEY",		 "TR"},
	{"TURKMENISTAN",	 "TM"},
	{"TURKS AND CAICOS ISLANDS",		"TC"},
	{"TUVALU",		 "TV"},
	{"UGANDA",		 "UG"},
	{"UKRAINE",		 "UA"},
	{"UNITED ARAB EMIRATES", "AE"},
	{"UNITED KINGDOM",	 "GB"},
	{"UNITED STATES",	 "US"},
	{"UNITED STATES MINOR OUTLYING ISLANDS","UM"},
	{"URUGUAY",		 "UY"},
	{"UZBEKISTAN",		 "UZ"},
	{"VANUATU",		 "VU"},
	{"VENEZUELA",		 "VE"},
	{"VIET NAM",		 "VN"},
	{"VIRGIN ISLANDS, BRITISH", "VG"},
	{"VIRGIN ISLANDS, U.S.", "VI"},
	{"WALLIS AND FUTUNA",	 "WF"},
	{"WESTERN SAHARA", 	 "EH"},
	{"YEMEN",		 "YE"},
	{"YUGOSLAVIA",		 "YU"},
	{"ZAMBIA",		 "ZM"},
	{"ZIMBABWE",		 "ZW"},
	{"ALL",		 	 "ALL"},
	{NULL, 			 NULL}
};

static int
ej_select_country(int eid, webs_t wp, int argc, char **argv)
{
	const char *country;
	const country_name_t *cntry;
	int ret = 0;

	country = nvram_safe_get("wl_country_code");

	for (cntry = country_names; cntry->name; cntry++) { 
		if (strcmp(country, cntry->abbrev)!=0)
			ret += websWrite(wp, "<option value=\"%s\">%s</option>", cntry->abbrev, cntry->name);
		else
			ret += websWrite(wp, "<option value=\"%s\" selected>%s</option>", cntry->abbrev, cntry->name);
	}
	return ret;
}

static int
wl_channels_in_country_asus(char *abbrev, int channels[])
{
	int i;
	char var[256], *next;

	i = 0;
	channels[i++] = 0;

	if (nvram_match("wl_chan_list", "")) {
		for ( ; i < 15; i++)
			channels[i] = i;
	} else {
		foreach(var, nvram_safe_get("wl_chan_list"), next) {
			channels[i++] = atoi(var);
		}
	}
	channels[i--] = -1;

	return i;
}

static int
wl_channels_in_country(char *abbrev, int channels[])
{
	char ibuf2[WLC_IOCTL_MEDLEN];
	int i, j, unit;
	wl_channels_in_country_t *cic = (wl_channels_in_country_t *)ibuf2;
	char tmp[100], prefix[sizeof("wlXXXXXXXXXX_")];
	char *name;

	if ((unit = nvram_get_int("wl_unit")) < 0)
		return -1;

	if (strlen(abbrev)==0) return 0;

	snprintf(prefix, sizeof(prefix), "wl%d_", unit);
	name = nvram_safe_get(strcat_r(prefix, "ifname", tmp));

	i = 0;
	channels[i++] = 0;
	channels[i] = -1;

	cic->buflen = sizeof(ibuf2);
	strcpy(cic->country_abbrev, abbrev);
	cic->band = WLC_BAND_2G;
	cic->count = 0;


	if (wl_ioctl(name, WLC_GET_CHANNELS_IN_COUNTRY, cic, cic->buflen) == 0) {
		if (cic->count == 0)
			return 0;

		for (j = 0; j < cic->count; j++) {
			channels[i++] = cic->channel[j];
		}

		channels[i] = -1;
	}
	return cic->count;

}
