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

#ifdef WEBS
#include <webs.h>
#include <uemf.h>
#include <ej.h>
#else /* !WEBS */
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
#include <httpd.h>
#endif /* WEBS */

#include <fcntl.h>
#include <signal.h>
#include <time.h>
#include <sys/klog.h>
#include <sys/wait.h>
#include <dirent.h>
#include <wlutils.h>
#include "typedefs.h"
#include "shutils.h"
#include "bcmconfig.h"
#include "bcmutils.h"
#include "bcmnvram.h"
#include "bcmnvram_f.h"
#include "common.h"

#define sys_forcereboot() kill(1, SIGABRT)

#ifdef WEBS
#define init_cgi(query)
#define do_file(webs, file)
#endif

#ifdef LINUX26
 #define        MTD_DEV(arg)            "/dev/mtd"#arg
#else
 #define        MTD_DEV(arg)            "/dev/mtd/"#arg
#endif

#define sys_upgrade(image) eval("write", image, MTD_DEV(1))
#define sys_upload(image) eval("nvram", "restore", image)
#define sys_download(file) eval("nvram", "save", file)
#define sys_restore(sid) eval("nvram_x","get",(sid))
#define sys_commit(sid) nvram_commit();
#define sys_default()   eval("erase", MTD_DEV(3))
#define sys_nvram_set(param) eval("nvram", "set", param)

#define UPNP_E_SUCCESS 0
#define UPNP_E_INVALID_ARGUMENT -1

#define GROUP_FLAG_REFRESH 	0
#define GROUP_FLAG_DELETE 	1
#define GROUP_FLAG_ADD 		2
#define GROUP_FLAG_REMOVE 	3

#define IMAGE_HEADER 	"HDR0"
#define PROFILE_HEADER 	"HDR1"

extern void getSharedEntry(int index);
extern void setSharedEntry(int index);

static int apply_cgi_group(webs_t wp, int sid, struct variable *var, char *groupName, int flag);
static int nvram_generate_table(webs_t wp, char *serviceId, char *groupName);

static int ej_select_country(int eid, webs_t wp, int argc, char_t **argv);
static int wl_channels_in_country(char *abbrev, int channels[]);
static int wl_channels_in_country_asus(char *abbrev, int channels[]);

extern char ibuf2[WLC_IOCTL_MAXLEN];


#define ACTION_UPGRADE_OK   0
#define ACTION_UPGRADE_FAIL 1

int action;

char *serviceId;
#define MAX_GROUP_ITEM 10
#define MAX_GROUP_COUNT 300
#define MAX_LINE_SIZE 1024
char *groupItem[MAX_GROUP_ITEM];
char urlcache[128];
char *next_host;
int delMap[MAX_GROUP_COUNT];
char SystemCmd[128];
char UserID[32]="";
char UserPass[32]="";
char ProductID[32]="";

extern int redirect;


char *
rfctime(const time_t *timep)
{
	static char s[201];
	struct tm tm;

	setenv("TZ", nvram_safe_get_x("", "time_zone"), 1);
	memcpy(&tm, localtime(timep), sizeof(struct tm));
	strftime(s, 200, "%a, %d %b %Y %H:%M:%S %z", &tm);
	return s;
}

char *
reltime(unsigned int seconds)
{
	static char s[] = "XXXXX days, XX hours, XX minutes, XX seconds";
	char *c = s;

#if 1 //def SHOWALL
	if (seconds > 60*60*24) {
		c += sprintf(c, "%d days, ", seconds / (60*60*24));
		seconds %= 60*60*24;
	}
	if (seconds > 60*60) {
		c += sprintf(c, "%d hours, ", seconds / (60*60));
		seconds %= 60*60;
	}
	if (seconds > 60) {
		c += sprintf(c, "%d minutes, ", seconds / 60);
		seconds %= 60;
	}
	c += sprintf(c, "%d seconds", seconds);
#else
	c += sprintf(c, "%d secs", seconds);
#endif

	return s;
}

#ifndef WEBS
/******************************************************************************/
/*
 *	Redirect the user to another webs page
 */

void websRedirect(webs_t wp, char_t *url)
{	
	//printf("Redirect to : %s\n", url);	
	websWrite(wp, T("<html><head>\r\n"));
	if (next_host && *next_host)
        	websWrite(wp, T("<meta http-equiv=\"refresh\" content=\"0; url=http://%s/%s\">\r\n"), next_host, url);
        else
        	websWrite(wp, T("<meta http-equiv=\"refresh\" content=\"0; url=%s\">\r\n"), url);
        websWrite(wp, T("<meta http-equiv=\"Content-Type\" content=\"text/html\">\r\n"));
        websWrite(wp, T("</head></html>\r\n"));      
	
#ifdef REMOVE_WL600	
	websWrite(wp, T("HTTP/1.0 302 Redirect\r\n"));
	websWrite(wp, T("Server: Asus Server\r\n"));
	websWrite(wp, T("Pragma: no-cache\r\n"));
	websWrite(wp, T("Cache-Control: no-cache\r\n"));
	websWrite(wp, T("Content-Type: text//html\r\n"));
	websWrite(wp, T("Location: http://%s/%s"), /*websGetVar(wp, T("HTTP_HOST"), "")*/"192.168.123.249", url);			

	websWrite(wp, T("<html><head></head><body>\r\n\
		This document has moved to a new <a href=\"http://%s/%s\">location</a>.\r\n\
		Please update your documents to reflect the new location.\r\n\
		</body></html>\r\n"), /*websGetVar(wp, T("HTTP_HOST"), "")*/"192.168.123.249", url);
        websWrite(wp, T("HTTP/1.0 200 OK\r\n"));	
#endif        
        websDone(wp, 200);	
}
#endif

void sys_script(char *name)
{
     char scmd[64];
	
     sprintf(scmd, "/tmp/%s", name);
     //printf("run %s %d %s\n", name, strlen(name), scmd);
     
     //handle special scirpt first

     if (strcmp(name,"syscmd.sh")==0)
     {
	    char *Cmd_argv[] = { "/bin/sh", "-c", SystemCmd, NULL };

	   if (strcmp(SystemCmd, "")==0)
		Cmd_argv[2] = "echo None";
	   _eval(Cmd_argv, ">/tmp/syscmd.log", 10, NULL);
     }
     else if (strcmp(name, "syslog.sh")==0)
     {
	   // to nothing
     }	
     else if (strcmp(name, "wan.sh")==0 || strcmp(name, "printer.sh")==0)
     {
#ifdef __CONFIG_INFOSVR__
	   // update status of printer
	   kill_pidfile_s("/var/run/infosvr.pid", SIGUSR1);
#else
	    //TODO
#endif
     }
     else if (strcmp(name, "lpr_remove")==0)
     {
	   kill_pidfile_s("/var/run/lpdparent.pid", SIGUSR2);
	   eval("killall","p910nd");
     }
     else if (strcmp(name, "madwimax.sh")==0)
     {	
	   // update status of madwimax
	   kill_pidfile_s("/var/run/madwimax.pid", SIGUSR1);
     }
     else if (strcmp(name, "wlan11a.sh")==0 || strcmp(name,"wlan11b.sh")==0)
     {
	  // do nothing	
     }
     else if (strcmp(name,"dnsmasq.sh")==0)
     {
	   kill_pidfile_s("/var/run/dnsmasq.pid", SIGALRM);
	   sleep(1);
     }
     else if (strcmp(name,"iptable.sh")==0) 
     {
		// TODO	
     }
     else if (strcmp(name,"route.sh")==0)
     {
		// TODO
     }
     else if (strcmp(name,"radio_disable")==0)
     {
		eval("wl", "radio", "off");	
     }
     else if (strcmp(name,"radio_enable")==0)
     {
		eval("wl", "radio", "on");	
     }
     else if (strcmp(name,"dhcpc_renew")==0)
     {
		sys_renew();
		sleep(1);
     }
     else if (strcmp(name,"dhcpc_release")==0)
     {
		sys_release();
		sleep(1);
     }
     else if (strcmp(name,"eject-usb.sh")==0)
     {
		eval("rmstorage");
     }
     else if (strcmp(name,"ddnsclient")==0)
     {
		eval("start_ddns");
     }
     else if (strstr(scmd, " ")==0) // no parameter, run script with eval
     {	
         	eval(scmd);
     }
     else system(scmd);  
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
char checkVersion(char *version, unsigned char major, unsigned char minor)
{
        #define VINT(ver) (((((ver)[0])&0xff)<<8)|(((ver)[1])&0xff))        
        int i;
        unsigned int ver, min, max;
        char *clist;
        
        clist = version+16;
        ver = major << 8 | minor;
        
        //printf("ver: %d %d %d\n", major, minor, ver);
        i = 0;
        
        while(VINT(clist+i) && i<16)
        {
            min = VINT(clist+i); 	
            max = VINT(clist+i+2);
                        
            //printf("List: %x %x %x %x %x %x %x %x\n", i, ver, min, max, clist[i], clist[i+1], clist[i+2], clist[i+3]);
           
            if (ver>=min && (max==0 || ver<=max)) return 1; 
            i+=4;	
        }         
        if (i==0 || ver==0) return 1;                              	
        else return 0;
}

void websScan(char_t *str)
{
	unsigned int i, flag;
        char_t *v1, *v2, *v3, *sp;
        char_t groupid[64];
        char_t value[MAX_LINE_SIZE];
        char_t name[MAX_LINE_SIZE];
        
        v1 = strchr(str, '?');
                        
        i = 0;
        flag = 0;
                                     		
	while(v1!=NULL)
	{	   	    	
	    v2 = strchr(v1+1, '=');
	    v3 = strchr(v1+1, '&');
	    
	    if (v3!=NULL)
	    {
	       strncpy(value, v2+1, v3-v2-1);
	       value[v3-v2-1] = 0;  
	    }  
	    else
	    {
	       strcpy(value, v2+1);
	    }
	    
	    strncpy(name, v1+1, v2-v1-1);
	    name[v2-v1-1] = 0;
	    /*printf("Value: %s %s\n", name, value);*/
	    
	    if (v2 != NULL && ((sp = strchr(v1+1, ' ')) == NULL || (sp > v2))) 
	    {	    	
	       if (flag && strncmp(v1+1, groupid, strlen(groupid))==0)
	       {	    		    	   
	           delMap[i] = atoi(value);
	           /*printf("Del Scan : %x\n", delMap[i]);*/
	           if (delMap[i]==-1)  break;	           	           
	           i++;
	       }	
               else if (strncmp(v1+1,"group_id", 8)==0)
               {            	                               
                   sprintf(groupid, "%s_s", value);
                   flag = 1;
               }   
            }
	    v1 = strchr(v1+1, '&');
	} 
	delMap[i] = -1;
	return;
}


void websApply(webs_t wp, char_t *url)
{	
     FILE *fp;
     char buf[MAX_LINE_SIZE];

     fp = fopen(url, "r");
     
     if (fp==NULL) return;
     
     while(fgets(buf, sizeof(buf), fp))
     {
	websWrite(wp, buf);
     } 
     
     websDone(wp, 200);	
     fclose(fp);
}

int websWriteEscape(webs_t wp, char *buf)
{
	static char *escapes = "\"&<>";
	char *c;
	int ret = 0;

        for (c = buf; *c; c++) {
		if (strchr(escapes, *c))
			ret += websWrite(wp, "&#%d", *c);
		else
			ret += websWrite(wp, "%c", *c);
	}
	return ret;
}

/*
 * Example: 
 * lan_ipaddr=192.168.1.1
 * <% nvram_get_x("lan_ipaddr"); %> produces "192.168.1.1"
 * <% nvram_get_x("undefined"); %> produces ""
 */
static int
ej_nvram_get_x(int eid, webs_t wp, int argc, char_t **argv)
{
	char *sid, *name;

	if (ejArgs(argc, argv, "%s %s", &sid, &name) < 2) {
		websError(wp, 400, "Insufficient args\n");
		return -1;
	}

	return websWriteEscape(wp, nvram_safe_get_x(sid, name));
}

/*
 * Example: 
 * lan_ipaddr=192.168.1.1
 * <% nvram_get_x("lan_ipaddr"); %> produces "192.168.1.1"
 * <% nvram_get_x("undefined"); %> produces ""
 */
static int
ej_nvram_get_f(int eid, webs_t wp, int argc, char_t **argv)
{
	char *file, *field;

	if (ejArgs(argc, argv, "%s %s", &file, &field) < 2) {
		websError(wp, 400, "Insufficient args\n");
		return -1;
	}

	return websWriteEscape(wp, nvram_safe_get_f(file, field));
}

/*
 * Example: 
 * wan_proto=dhcp
 * <% nvram_match("wan_proto", "dhcp", "selected"); %> produces "selected"
 * <% nvram_match("wan_proto", "static", "selected"); %> does not produce
 */
static int
ej_nvram_match_x(int eid, webs_t wp, int argc, char_t **argv)
{
	char *sid, *name, *match, *output;

	if (ejArgs(argc, argv, "%s %s %s %s", &sid, &name, &match, &output) < 4) {
		websError(wp, 400, "Insufficient args\n");
		return -1;
	}
	
	if (nvram_match_x(sid, name, match))
	{
		return websWrite(wp, output);
	}	

	return 0;
}	

static int
ej_nvram_double_match_x(int eid, webs_t wp, int argc, char_t **argv)
{
	char *sid, *name, *match, *output;
	char *sid2, *name2, *match2;

	if (ejArgs(argc, argv, "%s %s %s %s %s %s %s", &sid, &name, &match, &sid2, &name2, &match2, &output) < 7) {
		websError(wp, 400, "Insufficient args\n");
		return -1;
	}
	
	if (nvram_match_x(sid, name, match) && nvram_match_x(sid2, name2, match2))
	{
		return websWrite(wp, output);
	}	

	return 0;
}

/*
 * Example: 
 * wan_proto=dhcp
 * <% nvram_match("wan_proto", "dhcp", "selected"); %> produces "selected"
 * <% nvram_match("wan_proto", "static", "selected"); %> does not produce
 */
static int
ej_nvram_match_both_x(int eid, webs_t wp, int argc, char_t **argv)
{
	char *sid, *name, *match, *output, *output_not;

	if (ejArgs(argc, argv, "%s %s %s %s %s", &sid, &name, &match, &output, &output_not) < 5) 
        {
		websError(wp, 400, "Insufficient args\n");
		return -1;
	}
	
	if (nvram_match_x(sid, name, match))
	{
		return websWrite(wp, output);
	}
        else
        {
                return websWrite(wp, output_not);
        }	
}

#ifdef REMOVE
/*
 * Example: 
 * wan_proto=dhcp
 * <% nvram_match_both("wan_proto", "dhcp", "selected", "not"); %> produces "selected"
 * <% nvram_match_both("wan_proto", "static", "selected", "not"); %> produces "not"
 */
int
ej_nvram_match_list_both_x(int eid, webs_t wp, int argc, char_t **argv)
{
	char *sid, *name, *match, *output, *output_ex;
	int index;

	if (ejArgs(argc, argv, "%s %s %s %s %s %d", &sid, &name, &match, &output, &output_ex, &index) < 6) {
		websError(wp, 400, "Insufficient args\n");
		return -1;
	}

	if (nvram_match_list_x(sid, name, match, index))
		return websWrite(wp, output);
	else
	        return websWrite(wp, output_ex);		
}	
#endif

/*
 * Example: 
 * lan_ipaddr=192.168.1.1 192.168.39.248
 * <% nvram_get_list("lan_ipaddr", 0); %> produces "192.168.1.1"
 * <% nvram_get_list("lan_ipaddr", 1); %> produces "192.168.39.248"
 */
static int
ej_nvram_get_list_x(int eid, webs_t wp, int argc, char_t **argv)
{
	char *sid, *name;
	int which;	
	int ret = 0;

	if (ejArgs(argc, argv, "%s %s %d", &sid, &name, &which) < 3) {
		websError(wp, 400, "Insufficient args\n");
		return -1;
	}

	ret += websWrite(wp, nvram_get_list_x(sid, name, which));
	return ret;
}

/*
 * Example: 
 * lan_ipaddr=192.168.1.1 192.168.39.248
 * <% nvram_get_list("lan_ipaddr", 0); %> produces "192.168.1.1"
 * <% nvram_get_list("lan_ipaddr", 1); %> produces "192.168.39.248"
 */
static int
ej_nvram_get_buf_x(int eid, webs_t wp, int argc, char_t **argv)
{
	char *sid, *name;
	int which;		

	if (ejArgs(argc, argv, "%s %s %d", &sid, &name, &which) < 3) {
		websError(wp, 400, "Insufficient args\n");
		return -1;
	}

	
	return 0;
}

/*
 * Example: 
 * lan_ipaddr=192.168.1.1 192.168.39.248
 * <% nvram_get_table_x("lan_ipaddr"); %> produces "192.168.1.1"
 * <% nvram_get_table_x("lan_ipaddr"); %> produces "192.168.39.248"
 */
static int
ej_nvram_get_table_x(int eid, webs_t wp, int argc, char_t **argv)
{
	char *sid, *name;
	int ret = 0;

	if (ejArgs(argc, argv, "%s %s", &sid, &name) < 2) {
		websError(wp, 400, "Insufficient args\n");
		return -1;
	}
		        
	ret += nvram_generate_table(wp, sid, name);	
	return ret;
}

/*
 * Example: 
 * wan_proto=dhcp;dns
 * <% nvram_match_list("wan_proto", "dhcp", "selected", 0); %> produces "selected"
 * <% nvram_match_list("wan_proto", "static", "selected", 1); %> does not produce
 */
static int
ej_nvram_match_list_x(int eid, webs_t wp, int argc, char_t **argv)
{
	char *sid, *name, *match, *output;
	int which;

	if (ejArgs(argc, argv, "%s %s %s %s %d", &sid, &name, &match, &output, &which) < 5) {
		websError(wp, 400, "Insufficient args\n");
		return -1;
	}
	
	if (nvram_match_list_x(sid, name, match, which))
		return websWrite(wp, output);
	else
	        return 0;		
}	


static int
ej_select_channel(int eid, webs_t wp, int argc, char_t **argv)
{
	char *sid, chstr[32];
	int ret = 0;	
	int i, channel, channels[32];
	
	if (ejArgs(argc, argv, "%s", &sid) < 1) {
		websError(wp, 400, "Insufficient args\n");
		return -1;
	}
         
        channel=atoi(nvram_safe_get_x("", "wl_channel"));

	if (!wl_channels_in_country(nvram_safe_get_x("", "wl_country_code"), channels))
	{
		wl_channels_in_country_asus(nvram_safe_get_x("", "wl_country_code"), channels);
	}

	i = 0;

	while(channels[i]!=-1)
	{
		if (channels[i] == 0) strcpy(chstr, "Auto");
		else sprintf(chstr, "%d", channels[i]);

        	if (channel==channels[i])
            		ret += websWrite(wp, "<option value=\"%d\" selected>%s</option>", channels[i], chstr);
        	else
            		ret += websWrite(wp, "<option value=\"%d\">%s</option>", channels[i], chstr);
		i++;
	} 
	return ret;
}


static int
ej_urlcache(int eid, webs_t wp, int argc, char_t **argv)
{
	int flag;
		
	if (strcmp(urlcache, "Main_Operation.asp")==0)
	   flag = 2;
	else if (strcmp(urlcache, "Main_Content.asp")==0)
	   flag = 1;		  		
	else
	   flag = 0;
	   
	cprintf("Url:%s %d\n", urlcache, flag);

	if (strcmp(nvram_safe_get_x("IPConnection","wan_route_x"), "IP_Routed")==0)
	{	
	   if (strcmp(nvram_safe_get_x("IPConnection", "wan_nat_x"), "1")==0)
	   {	   
	   	/* disable to redirect page */
	   	if (redirect)
	   	{	
			websWrite(wp, "Basic_GOperation_Content.asp");
			redirect=0;
           	}
	   	else if (flag==2)
			websWrite(wp, "Basic_GOperation_Content.asp");
	   	else if (flag==1)
			websWrite(wp, "Basic_HomeGateway_SaveRestart.asp");
	   	else
	   		websWrite(wp, "Main_Index_HomeGateway.asp");
	    }
	    else
	    {	
	   	/* disable to redirect page */
	   	if (redirect)
	   	{	
			websWrite(wp, "Basic_ROperation_Content.asp");
			redirect=0;
           	}
	   	else if (flag==2)
			websWrite(wp, "Basic_ROperation_Content.asp");
	   	else if (flag==1)
			websWrite(wp, "Basic_Router_SaveRestart.asp");
	   	else
	   		websWrite(wp, "Main_Index_Router.asp");
            }	
	}      
	else
	{
	    if (flag==2)
		websWrite(wp, "Basic_AOperation_Content.asp");
	    else if (flag==1)	
		websWrite(wp, "Basic_AccessPoint_SaveRestart.asp");
	    else	
	    	websWrite(wp, "Main_Index_AccessPoint.asp");
	}		
	strcpy(urlcache,"");		                             	

	return flag;
}


/* Report sys up time */
static int
ej_uptime(int eid, webs_t wp, int argc, char_t **argv)
{
	char buf[MAX_LINE_SIZE];
	int ret;
	char *str = file2str("/proc/uptime");
	time_t tm;

	time(&tm);
	sprintf(buf, rfctime(&tm));

	if (str) {
		unsigned int up = atoi(str);
		free(str);
		sprintf(buf, "%s (%s since boot)", buf, reltime(up));
	}

	ret = websWrite(wp, buf);  
	return ret;	    
}

#ifdef REMOVE
/* Dump firewall log */
int
ej_dumplog(int eid, webs_t wp, int argc, char_t **argv)
{
	#define MAX_LOG_BUF 2048
	#define MAX_LOG_SIZE 16384
	char buf[MAX_LOG_BUF], *line;
	char *s;
	int len, ret = 0;
	long filelen;
	time_t tm;
	char *verdict, *src, *dst, *proto, *spt, *dpt;
	FILE *fpOrig, *fp;

        if ((fp = fopen("/etc/linuxigd/firewall.log","a+"))==NULL)
	{	
		cprintf("Open file fail\n");
		goto fail;
	}
	
	if ((fpOrig = fopen("/etc/linuxigd/firewallOrig.log","r"))!=NULL)
	{
	   /* Find size of orignal file */
	   fseek(fpOrig, 0, SEEK_END);
	   filelen = ftell(fp);
	
	   if (filelen>MAX_LOG_SIZE)
	      fseek(fpOrig, -(filelen-MAX_LOG_SIZE), SEEK_END);
	   else
	      fseek(fpOrig, 0, SEEK_SET);     
	      
	   while((len=fread(buf, 1, MAX_LOG_BUF, fpOrig)))
	   {	    	
	       fwrite(buf, 1, len, fp);
	   }  	  							
	   fclose(fpOrig);	   
	   unlink("/etc/linuxigd/firewallOrig.log");
	}   
			
	while((len=klogctl(4, buf, MAX_LOG_BUF))>0)
	{
	    //printf("W:%s\n", buf);
	    fwrite(buf, 1, len, fp);
	}
	
	fclose(fp);
	
	if ((fp = fopen("/etc/linuxigd/firewall.log","r"))==NULL)
	{	
		cprintf("Open file fail\n");
		goto fail;
	}
	
	//fseek(fp, 0, SEEK_SET);
		
#ifdef REMOVE_WL600	
	for (next = buf; (line = strsep(&next, "\n"));) 
#else
	while(fgets(buf, MAX_LOG_BUF, fp))
#endif	
	{
#ifndef REMOVE_WL600		
		line = buf;
		if (!strncmp(line, "<4>DROP", 7))
			verdict = "denied";
		else if (!strncmp(line, "<4>ACCEPT", 9))
			verdict = "accepted";
		else
			continue;

		/* Parse into tokens */
		s = line;
		len = strlen(s);
		while (strsep(&s, " "));

		/* Initialize token values */
		time(&tm);
		src = dst = proto = spt = dpt = "n/a";

		/* Set token values */
		for (s = line; s < &line[len] && *s; s += strlen(s) + 1) {
			if (!strncmp(s, "TIME=", 5))
				tm = strtoul(&s[5], NULL, 10);
			else if (!strncmp(s, "SRC=", 4))
				src = &s[4];
			else if (!strncmp(s, "DST=", 4))
				dst = &s[4];
			else if (!strncmp(s, "PROTO=", 6))
				proto = &s[6];
			else if (!strncmp(s, "SPT=", 4))
				spt = &s[4];
			else if (!strncmp(s, "DPT=", 4))
				dpt = &s[4];
		}

		ret += websWrite(wp, "%s %s connection %s to %s:%s from %s:%s\n",
				 rfctime(&tm), proto, verdict, dst, dpt, src, spt);
#else
		ret += websWrite(wp, "%s\n", buf);				 
		cprintf("L:%s\n",buf);
#endif				 
	}	
	fclose(fp);	
	return ret;
fail:
	ret += websWrite(wp, "");
	return ret;	
}

struct lease_t {
	unsigned char chaddr[16];
	u_int32_t yiaddr;
	u_int32_t expires;
	char hostname[64];
};

int
websWriteCh(webs_t wp, char *ch, int count)
{
   int i, ret;
   
   ret = 0;
   for(i=0; i<count; i++)
      ret+=websWrite(wp, "%s", ch);
   return(ret);   
} 

/* Dump leases in <tr><td>MAC</td><td>IP</td><td>expires</td></tr> format */
static int
#ifdef REMOVE_WL600
ej_dumpleases(int eid, webs_t wp, int argc, char_t **argv)
#else
ej_dumpleases(int eid, webs_t wp, char *lease_file)
#endif
{
	FILE *fp;
	struct lease_t lease;
	int i;
	struct in_addr addr;
	unsigned long expires;
#ifdef REMOVE_WL600	
	char *format;
	char f[]="";
	char sigusr1[] = "-XX";
#endif	
	int ret = 0;

#ifdef REMOVE_WL600
	if (ejArgs(argc, argv, "%s", &format) < 1) {
		websError(wp, 400, "Insufficient args\n");
		return -1;
	}

	/* Write out leases file */
	sprintf(sigusr1, "-%d", SIGUSR1);
	eval("killall", sigusr1, "udhcpd");
#endif	

    ret +=websWrite(wp,"Mac Address       IP Address      Lease Time\n");	
        
	/* Parse leases file */
	if (!(fp = fopen(lease_file, "r"))) 
	{
		//websWrite(wp, "No leases\n");
		return -1;
	}
					    	           	
	while (fread(&lease, sizeof(lease), 1, fp)) {
		//ret += websWrite(wp, "%s", format);
		for (i = 0; i < 6; i++) {
			ret += websWrite(wp, "%02x", lease.chaddr[i]);
			if (i != 5) ret += websWrite(wp, ":");
		}
		addr.s_addr = lease.yiaddr;
		ret += websWrite(wp, " %s", inet_ntoa(addr));
		ret += websWriteCh(wp," ", 16-strlen(inet_ntoa(addr)));
		expires = ntohl(lease.expires);
		
		if (expires==0xffffffff)	
			ret += websWrite(wp, "manual");
		else if (!expires)
			ret += websWrite(wp, "expired");
		else {
			if (expires > 60*60*24) {
				ret += websWrite(wp, "%ld days, ", expires / (60*60*24));
				expires %= 60*60*24;
			}
			if (expires > 60*60) {
				ret += websWrite(wp, "%ld hours, ", expires / (60*60));
				expires %= 60*60;
			}
			if (expires > 60) {
				ret += websWrite(wp, "%ld minutes, ", expires / 60);
				expires %= 60;
			}
			ret += websWrite(wp, "%ld seconds", expires);
		}
		ret += websWrite(wp, "\n");
	}
	fclose(fp);

	return ret;
}

/* Dump leases in <tr><td>MAC</td><td>IP</td><td>expires</td></tr> format */
static int
ej_dumpiptable(int eid, webs_t wp, char *iptable_file)
{	
	/* Format 											    */
	/* DNAT       tcp  --  anywhere             0.0.0.0            tcp dpts:6970:32000 to:192.168.122.2 */
	char buf[MAX_LINE_SIZE], *line, *s;
	char *dst, *prot, *port, *rdst;
	int ret, i, len;
	FILE *fp;

        ret +=websWrite(wp,"Destination     Prot. Port Range  Redirect to\n");	
        
	/* Parse leases file */
	if (!(fp = fopen(iptable_file, "r"))) 
	{
		//websWrite(wp, "No leases\n");
		return -1;
	}
					    	           	
	while (fgets(buf, MAX_LINE_SIZE, fp)) 
	{
	     line = buf;	
	     /* Parse into tokens */
	     s = line;
	     len = strlen(s);
	     
	     //printf("Line: %d %s\n", len, line);
	     
	     while (strsep(&s, " "));

	     
	     
	     /* Initialize token values */	     
	     dst = rdst = "n/a";
	     prot = port = "all";

	     i = 0;
	     /* Set token values */
  	     /* destination protocol:...  proto: */
	     for (s = line; s < &line[len]; s += strlen(s) + 1)
	     {
		if (i==1) /* Destination */
	           dst = &s[0];
	     	else if (!strncmp(s, "tcp", 3)) /* Proto */
	     	   prot = &s[0];
	     	else if (!strncmp(s, "udp", 3)) /* Proto */
	     	   prot = &s[0];   
	     	else if (!strncmp(s, "all", 3)) /* Proto */
	     	   prot = &s[0];   
	     	else if (!strncmp(s, "dpts:", 5))
	           port = &s[5];	
	        else if (!strncmp(s, "dpt:", 4))
	           port = &s[4];	   		    
		else if (!strncmp(s, "to:", 3))
		   rdst = &s[3];
		else if (!strncmp(s, "protox:", 7))
		   prot = &s[7];

		if (strlen(s)!=0) i++;
		//printf("Token: %d %s\n", i, s);
	     }
	     
	     ret +=websWrite(wp, "%s", dst);
	     ret +=websWriteCh(wp, " ", 16-strlen(dst));
	     ret +=websWrite(wp, "%s", prot);
	     ret +=websWriteCh(wp, " ", 6-strlen(prot));
	     ret +=websWrite(wp, "%s", port);
	     ret +=websWriteCh(wp, " ", 12-strlen(port));
	     ret +=websWrite(wp, "%s\n", rdst);	    
	}
	fclose(fp);

	return ret;
}
#endif

static int dump_file(webs_t wp, char *filename)
{
	FILE *fp;
	char buf[MAX_LINE_SIZE];
	int ret = 0;

	//printf("dump: %s\n", filename);
	
	fp = fopen(filename, "r");
		
	if (fp==NULL) 
	{
		ret+=websWrite(wp, "");
		return(ret);
	}

	while (fgets(buf, MAX_LINE_SIZE, fp)!=NULL)
	{	 	
	    //printf("Read time: %s\n", buf);	    	    	    	   
	    ret += websWrite(wp, "%s", buf);
	}		    	                             		
	 
	fclose(fp);		
	
	return(ret);
}

static int
ej_dump(int eid, webs_t wp, int argc, char_t **argv)
{	
	char filename[32];
	char *file,*script;
	int ret;
	
	if (ejArgs(argc, argv, "%s %s", &file, &script) < 2) {
		websError(wp, 400, "Insufficient args\n");
		return -1;
	}
	
		
	cprintf("Script : %s, File: %s\n", script, file);
	
	// run scrip first to update some status
	if(strcmp(script,"")!=0) sys_script(script); 
 
	if (strcmp(file, "wlan11b.log")==0)
	{
		if (nvram_match_x("", "nobr", "1"))
			return(ej_wl_status_nobr(eid, wp, 0, NULL));
		else
			return(ej_wl_status(eid, wp, 0, NULL));
	}
	else if(strcmp(file, "leases.log")==0) 
		return(ej_lan_leases(eid, wp, 0, NULL));
	else if (strcmp(file, "iptable.log")==0) 
		return(ej_nat_table(eid, wp, 0, NULL));
	else if (strcmp(file, "route.log")==0)
		return(ej_route_table(eid, wp, 0, NULL));
			   
	ret = 0;
			   
	if (strcmp(file, "syslog.log")==0)
	{
           	sprintf(filename, "/tmp/%s-1", file);
	   	ret+=dump_file(wp, filename); 
	}
	   			   
	sprintf(filename, "/tmp/%s", file);
	ret+=dump_file(wp, filename);					
	   
	return ret;	    
}	

static int
ej_load(int eid, webs_t wp, int argc, char_t **argv)
{	
	char *script;
	
	if (ejArgs(argc, argv, "%s", &script) < 1) {
		websError(wp, 400, "Insufficient args\n");
		return -1;
	}
	 	  
	sys_script(script);
	return(websWrite(wp,""));						    
}	


static void
validate_cgi(webs_t wp, int sid, int groupFlag)
{
    struct variable *v;    
    char *value;
    char name[64];
        
    /* Validate and set variables in table order */
    for (v = GetVariables(sid); v->name != NULL; v++) 
    {
    	 //printf("Name: %s %d\n", v->name, sid);
    	 sprintf(name, "%s", v->name);    
    	         	 
    	 if ((value = websGetVar(wp, name, NULL)))
	 { 
	      if (strcmp(v->longname, "Group")==0)
    	      {
    	      	
    	      }
    	      else 
    	      {
		 //printf("set: %s %s\n", v->name, value);
    	         nvram_set_x(GetServiceId(sid), v->name, value);
    	      }   
	 }     
	 							 	
#ifdef REMOVE_WL600				   
	 if ((!*value && v->nullok) || !v->validate)
         { 
	    nvram_set_x(GetServiceId(sid), v->name, value);
         } 
  	 else
         {
	    if (v->validate(value, v) == UPNP_E_SUCCESS)
	       nvram_set_x(FindSvcLink(sid), v->name, value);
	    else
	       websWrite(wp, "Invalid %s = %s\n", v->name, value);
         }
#endif        	
    }
}



static const char * const apply_header =
"<head>"
"<title>ASUS Wireless AP/Router: Apply</title>"
"<meta http-equiv=\"Content-Type\" content=\"text/html; charset=utf-8\">"
"<style type=\"text/css\">"
"body { background: white; color: black; font-family: arial, sans-serif; font-size: 9pt }"
".title	{ font-family: arial, sans-serif; font-size: 13pt; font-weight: bold }"
".subtitle { font-family: arial, sans-serif; font-size: 11pt }"
".label { color: #306498; font-family: arial, sans-serif; font-size: 7pt }"
"</style>"
"</head>"
"<body>"
"<p>"
"<span class=\"title\">APPLY</span><br>"
"<span class=\"subtitle\">This screen notifies you of any errors "
"that were detected while changing the router's settings.</span>"
"<form method=\"get\" action=\"apply.cgi\">"
"<p>"
;

static const char * const apply_footer =
"<p>"
"<input type=\"button\" name=\"action\" value=\"Continue\" OnClick=\"history.go(-1)\">"
"</form>"
"<p class=\"label\">&#169;2001 ASUSTeK COMPUTER INC. All rights reserved.</p>"
"</body>"
;

enum {
	NOTHING,
	REBOOT,
	RESTART,
};

char *svc_pop_list(char *value, char key)
{    
    char *v, *buf;
    int i;
               
    if (value==NULL || *value=='\0')
       return(NULL);      
            
    buf = value;
    v = strchr(buf, key);

    i = 0;
    
    if (v!=NULL)
    {    	
        *v = '\0';  	
        return(buf);    	   
    }    
    return(NULL);
}


static int
apply_cgi(webs_t wp, char_t *urlPrefix, char_t *webDir, int arg, 
	  char_t *url, char_t *path, char_t *query)
{
    
    int sid;
    char *value; 
    char *current_url;
    char *next_url;
    char *sid_list;
    char *value1; 
    char *script;
    char groupId[64];
    char urlStr[64];
    char *groupName;
//    char *host;
    
    if (!query)
	goto footer;
            
    urlStr[0] = 0;
                
    value = websGetVar(wp, "action", "");         
    
    //if (strcmp(value, "")==0)
    value = websGetVar(wp, "action_mode","");
          
    next_host = websGetVar(wp, "next_host", "");   
    cprintf("host:%s\n", next_host);   
    current_url = websGetVar(wp, "current_page", "");
    next_url = websGetVar(wp, "next_page", "");
    script = websGetVar(wp, "action_script",""); 
   
    
    //printf("Script: %s\n", script);
    cprintf("Apply: %s %s %s %s\n", value, current_url, next_url, websGetVar(wp, "group_id", ""));

    if (!strcmp(value," Refresh "))
    {   
    	strcpy(SystemCmd, websGetVar(wp,"SystemCmd","")); 	        	
    	websRedirect(wp, current_url);
        return 0;	
    }
    else if (!strcmp(value," Clear "))
    {   
	// current only syslog implement this button
	unlink("/tmp/syslog.log-1");
	unlink("/tmp/syslog.log");	
    	websRedirect(wp, current_url);
        return 0;	
    }
    else if (!strcmp(value,"NEXT"))
    {
    	websRedirect(wp, next_url);
        return 0;	
    }
    else if (!strcmp(value, "Save&Restart "))
    {    	
        /*action = ACTION_SAVE_RESTART;*/
        /*websRedirect(wp, "Restarting.asp");*/
        websApply(wp, "Restarting.asp");
	nvram_set_f("General", "x_Setting", "1");
        sys_commit(NULL);	
        //sys_script("bcm_set");
        //system("/web/script/eject-usb.sh");
        sys_reboot();    	
    	return(0);
    }
    else if(!strcmp(value, "Restore"))
    {             	
     	/* action = ACTION_RESET_DEFAULT;*/
        /* websRedirect(wp, "Restarting.asp");*/
        websApply(wp, "Restarting.asp");	
        //system("/web/script/eject-usb.sh");
        sys_default();       
        sys_reboot();      
        return(0);	
    }
    else if (!strcmp(value, "WlanUpdate "))
    {    	
        /*action = ACTION_SAVE_RESTART;*/
        /*websRedirect(wp, "Restarting.asp");*/
    	if (strcmp(script, "")!=0)
    	{    	      	   
        	system(script);
	}
        websApply(wp, "Restarting.asp");
        sys_reboot();    	
    	return(0);
    }
    else 
    {    	
    	sid_list = websGetVar(wp, "sid_list", "");
    	    	        
    	while((serviceId = svc_pop_list(sid_list, ';')))
    	{    		    	   	    	    	
            sid = 0;		
    	    while(GetServiceId(sid)!=0)
    	    {    	    	
    	    	if (!strcmp(GetServiceId(sid), serviceId))
    	    	   break;
    	    	sid++;   
    	    }	
    	   
    	    if (serviceId!=NULL)
    	    {
    	    	if (!strcmp(value, " Restore "))
    	    	{
    	    	    //sys_restore(serviceId);
    		}
    		else if(!strcmp(value, "  Save  ") || !strcmp(value, " Apply "))
    		{  			
    		    validate_cgi(wp, sid, TRUE);
    		} 
    		else if(!strcmp(value, "Set") || !strcmp(value, "Unset") || !strcmp(value, "Update") || !strcmp(value, "Eject"))
    		{
    		    validate_cgi(wp, sid, TRUE);
    		} 
    		else if(!strcmp(value," Finish "))
    		{
    		    validate_cgi(wp, sid, TRUE);    		   	    
    	    	}
    	    	else 
    	    	{    
		     cprintf("group ?\n");	    	 	    	    	    	         	      	    	    	
    	    	     strcpy(groupId,websGetVar(wp, "group_id", ""));
            
   		     if ((value1 = websGetVar(wp, "action_mode", NULL)))
   		     {    		     	
       			groupName = groupId;

       			//if (!strcmp(GetServiceId(sid), groupId))
       			{
       			   if (!strncmp(value1, " Delete ", 8))
       			   {
       			       apply_cgi_group(wp, sid, NULL, groupName, GROUP_FLAG_DELETE);
       			   }    
       			   else if (!strncmp(value1, " Add ", 5))
       			   {
       			       apply_cgi_group(wp, sid, NULL, groupName, GROUP_FLAG_ADD);  
       			   }    
       			   else if (!strncmp(value1, " Del ", 5))    
       			   { 
       			       apply_cgi_group(wp, sid, NULL, groupName, GROUP_FLAG_REMOVE);   
       			   }    
       			   else if (!strncmp(value1, " Refresh ", 9))  
       			   {
    	    	    	       apply_cgi_group(wp, sid, NULL, groupName, GROUP_FLAG_REFRESH);    	    	    	       	    	    	   
    	    	    	   }        	    	    
//#ifdef SHARED
       			   else if (!strncmp(value1, " Edit ", 6))  
       			   {
    	    	    	       //apply_cgi_group(wp, sid, NULL, groupName, GROUP_FLAG_REFRESH);    	    	    	       	    	    	  
				cprintf("edit %d\n", delMap[0]); 
    				if (delMap[0]==-1) websRedirect(wp, current_url);
				else 
				{
					getSharedEntry(delMap[0]);
					websRedirect(wp, "Advanced_StorageUser_Content.asp");
				}
				return 0;

    	    	    	   }        	    	    
//#endif	   
    	    	    	   sprintf(urlStr, "%s#%s", current_url, groupName);    	    	    	   
    	    	    	   
    	    	    	   validate_cgi(wp, sid, FALSE);	
    	    	    	}      
    	    	     }    	    	      	    	    	         	    	    
    		}       	    		    	            	    	
    	    }    	       
    	    sid_list = sid_list + strlen(serviceId)+1;
    	}    	
    
	printf("apply????\n");
	
    	/* Add for EMI Test page */
    	if (strcmp(script, "")!=0)
    	{    	      	   
	   	sys_script(script);
	}   
	
        if (!strcmp(value, "  Save  "))
        {
           strcpy(urlcache, next_url);
           websRedirect(wp, next_url);
        } else 
        if (!strcmp(value, " Apply "))
        {
           strcpy(urlcache, current_url);
           websRedirect(wp, current_url);
        } 
        
    	else if(!strcmp(value, " Finish ")) websRedirect(wp, "SaveRestart.asp");

    	else if (urlStr[0]==0) websRedirect(wp, current_url);

    	else websRedirect(wp, urlStr);

	cprintf("apply ok\n");
        return 0;           
    }  
     	        	    
  
      
footer:
	websWrite(wp, (char_t *) apply_footer);
	websFooter(wp);
	websDone(wp, 200);

#ifndef FLASH_EMULATOR
	if (action == RESTART)
		sys_restart();
	else if (action == REBOOT)
		sys_reboot();
#endif

	return 1;
}


#ifdef REMOVE_WL600
void nvram_add_group_list(webs_t wp, struct variable *v, int count)
{
    struct variable *gv;
    char *value;
    char name[64];
    int i;
    
    if (v->argv[0]==NULL) 
    {
       return;
    }
    
    i = 0;
    for (gv = (struct variable *)v->argv[0]; gv->name!=NULL; gv++)
    {    		    	
    	sprintf(name, "%s_%s_%d", serviceId, gv->name, count+1);    	
        if ((value=websGetVar(wp, name, NULL)))
        {           	
            if (groupItem[i]==0) {
    		groupItem[i]=(char*)malloc(MAX_LINE_SIZE);
    		sprintf(groupItem[i],"%s;",value);
    	    }  	
    	    else sprintf(groupItem[i],"%s%s;",groupItem[i], value);
            /* Don't check validate. Left validate to JavaScript */                        
        }   
        else
        {
            if (i==0) return;	
            if (groupItem[i]==0) {
    		groupItem[i]=(char*)malloc(MAX_LINE_SIZE);
    		sprintf(groupItem[i],";");
    	    }	
            else sprintf(groupItem[i],"%s;",groupItem[i]);
        }   
        i++;
    }   
    return;
}

void nvram_apply_group_list(webs_t wp, struct variable *v)
{
    struct variable *gv;
    int i;
        
    if (v->argv[0]==NULL) 
    {
       return;
    }

    i=0;
    for (gv = (struct variable *)v->argv[0]; gv->name!=NULL; gv++)
    {
    	if (groupItem[i]!=NULL)
    	{
    	   nvram_set_x(serviceId, gv->name, groupItem[i]);
    	   free(groupItem[i]);
    	   groupItem[i]=NULL;
    	}   
    	i++;
    }   
    return;
}
#endif

void nvram_add_group_item(webs_t wp, struct variable *v, int sid)
{
    struct variable *gv;
    char *value;
    char name[64], cstr[5];    
    int count;
    
    if (v->argv[0]==NULL) 
    {
       return;
    }
                
    count = atoi(nvram_safe_get_x(serviceId, v->argv[3]));        
    cprintf("count: %d\n", count);
    
    for (gv = (struct variable *)v->argv[0]; gv->name!=NULL; gv++)
    {    	    		    	    	
    	sprintf(name, "%s_0", gv->name);    	
    	    	    	    	    	    	
        if ((value=websGetVar(wp, name, NULL)))
        {                    
            /*SetGroupVariable(sid, v, gv->name, value, "Add");*/
            nvram_add_lists_x(serviceId, gv->name, value, count);            
            cprintf("Add: %s %s\n", gv->name, value);
        }   
        else
        {        	
            /*SetGroupVariable(sid, v, gv->name, "", "Add");  */
            nvram_add_lists_x(serviceId, gv->name, "", count);             
        }                      
    }   
    
    count++;    
    sprintf(cstr, "%d", count);       
    nvram_set_x(serviceId, v->argv[3], cstr);
    return;
}


void nvram_remove_group_item(webs_t wp, struct variable *v, int sid, int *delMap)
{
    struct variable *gv;
    char cstr[5];
    int i, count;
    
    if (v->argv[0]==NULL) 
    {
       return;
    }
    
    count = atoi(nvram_safe_get_x(serviceId, v->argv[3]));    
    for (gv = (struct variable *)v->argv[0]; gv->name!=NULL; gv++)
    {    	    		    	     	    	
    	nvram_del_lists_x(serviceId, gv->name, delMap);    		    	       
    }           
    
    i=0;
    while(delMap[i]!=-1) i++;
    
    count-=i;
    sprintf(cstr, "%d\n", count);
    nvram_set_x(serviceId, v->argv[3], cstr);
    return;
}

/* Rule for table: 
 * 1. First field can not be NULL, or it is treat as empty
 * 2. 
 */

static int 
nvram_add_group_table(webs_t wp, char *serviceId, struct variable *v, int count)
{
    struct variable *gv;
    char buf[MAX_LINE_SIZE+MAX_LINE_SIZE];
    char bufs[MAX_LINE_SIZE+MAX_LINE_SIZE];    
    int i, j, fieldLen, rowLen, fieldCount, value;
    
    
    if (v->argv[0]==NULL) 
    {
       return 0;
    }
    
    bufs[0] = 0x0;
    rowLen = atoi(v->argv[2]);      
    
    if (count==-1)
    {    
       for(i=0;i<rowLen;i++)
       {     
       	   bufs[i] = ' ';
       }
       value = -1;	       	       	             	
       bufs[i] = 0x0;             	
    	
       goto ToHTML;	
    }
              
                    
    fieldCount = 0;
    
    value = count;
            
    for (gv = (struct variable *)v->argv[0]; gv->name!=NULL; gv++)
    {    		    	    	    	
    	strcpy(buf, nvram_get_list_x(serviceId, gv->name, count));    
    	fieldLen = atoi(gv->longname);
    	    	    	       	        	    
    	if (fieldLen!=0)
    	{    	
    	   if (strlen(buf)>fieldLen)
    	   {    	     
    	      buf[fieldLen] = 0x0; 	
    	   }   
    	   else
    	   {
    	      i = strlen(buf);	
    	      j = i;
    	      for(;i<fieldLen;i++)
    	      {
    	         buf[j++] = ' ';	
    	      }       	      
    	      buf[j] = 0x0;
    	   }      
    	}   
    	
    	if (fieldCount==0)
    	   sprintf(bufs,"%s", buf);
    	else
    	   sprintf(bufs,"%s%s",bufs, buf);
    	       	   
    	fieldCount++;
    }                                              
    
    if (strlen(bufs)> rowLen)
       bufs[rowLen] = 0x0;
       
ToHTML:       
    /* Replace ' ' to &nbsp; */
    buf[0] = 0;
    j = 0;
    
    for (i=0; i<strlen(bufs);i++)
    {
    	if (bufs[i] == ' ')
    	{
    	   buf[j++] = '&';
    	   buf[j++] = 'n';
    	   buf[j++] = 'b';
    	   buf[j++] = 's';
    	   buf[j++] = 'p';
    	   buf[j++] = ';';    	   	
    	} 
    	else buf[j++] = bufs[i];
    }          
    buf[j] = 0x0;
       
    return(websWrite(wp, "<option value=\"%d\">%s</option>", value, buf));
}

static int
apply_cgi_group(webs_t wp, int sid, struct variable *var, char *groupName, int flag)
{	
   struct variable *v;
   int groupCount;
         
       
   cprintf("This group limit is %d %d\n", flag, sid);

   if (var!=NULL) v = var;
   else
   {         
       /* Validate and set vairables in table order */              
       for (v = GetVariables(sid); v->name != NULL; v++) 
       {
       	   //printf("Find group : %s %s\n", v->name, groupName);
       	   if(!strcmp(groupName, v->name)) break;
       }	                    
   }    
   
   if (v->name == NULL) return 0;    
   
   groupCount = atoi(v->argv[1]);

   if (flag == GROUP_FLAG_ADD)/* if (!strcmp(value, " Refresh ")) || Save condition */
   {    
   	nvram_add_group_item(wp, v, sid);
   }	  
   else if (flag == GROUP_FLAG_REMOVE)/* if (!strcmp(value, " Refresh ")) || Save condition */
   {    
   	/*nvram_remove_group_item(wp, v, sid); 	*/
   	/*sprintf(item, "%s_s", websGetVar(wp, "group_id", ""));*/
        /*value1 = websGetVar(wp, item, NULL);*/
        /* TO DO : change format from 'x=1&x=2&...' to 'x=1 2 3 ..'*/
   	nvram_remove_group_item(wp, v, sid, delMap);   	
   }	     
   return(1);
}

static int
nvram_generate_table(webs_t wp, char *serviceId, char *groupName)
{	
   struct variable *v;
   int i, groupCount, ret, r, sid;
     
   
   sid = LookupServiceId(serviceId);
         
   if (sid==-1) return 0;
    
   /* Validate and set vairables in table order */              
   for (v = GetVariables(sid); v->name != NULL; v++) 
   {
      /* printf("Find group : %s %s\n", v->name, groupName);*/
      if(!strcmp(groupName, v->name)) break;                       
   }    
   
   if (v->name == NULL) return 0;    
            
   groupCount = atoi(nvram_safe_get_x(serviceId, v->argv[3]));               
            
   if (groupCount==0) ret = nvram_add_group_table(wp, serviceId, v, -1);
   else
   {
   
      ret = 0;
   
      for (i=0; i<groupCount; i++)
      {       	            	
        r = nvram_add_group_table(wp, serviceId, v, i);
        if (r!=0)
           ret += r;
        else break;
      }    
   }   
   
   return(ret);
}


#ifdef WEBS
mySecurityHandler(webs_t wp, char_t *urlPrefix, char_t *webDir, int arg, char_t *url, char_t *path, char_t *query)
{
	char_t *user, *passwd, *digestCalc;
	int flags, nRet;


	user = websGetRequestUserName(wp);
	passwd = websGetRequestPassword(wp);
	flags = websGetRequestFlags(wp);


	nRet = 0;

	if (user && *user)
	{
		if (strcmp(user, "admin")!=0)
		{
			websError(wp, 401, T("Wrong User Name"));
			nRet = 1;
		}
		else if (passwd && *passwd)
		{
			if (strcmp(passwd, websGetPassword())!=0)
			{
				websError(wp, 401, T("Wrong Password"));
				nRet = 1;
			}
		}
#ifdef DIGEST_ACCESS_SUPPORT		
		else if (flags & WEBS_AUTH_DIGEST)
		{
			wp->password = websGetPassword();

			a_assert(wp->digest);
			a_assert(wp->nonce);
			a_assert(wp->password);

			digestCalc = websCalcDigest(wp);
			a_assert(digestCalc);		

			if (gstrcmp(wp->digest, digestCalc)!=0)
			{
				websError(wp, 401, T("Wrong Password"));
				nRet = 1;
			}
			bfree(B_L, digestCalc);
		}
#endif	
	}
	else 
	{
#ifdef DIGEST_ACCESS_SUPPORT
		wp->flags |= WEBS_AUTH_DIGEST;
#endif
		websError(wp, 401, T(""));
		nRet = 1;
	}
	return nRet;

}
#else
static void
do_auth(char *userid, char *passwd, char *realm)
{	
//	time_t tm;
					
	if (strcmp(ProductID,"")==0)
	{
	        strncpy(ProductID, nvram_safe_get_x("general.log", "productid"), 32);
	}
	if (strcmp(UserID,"")==0)
	{	   
	   	strncpy(UserID, nvram_safe_get_x("General","http_username"), 32);
	}
   	if (strcmp(UserPass,"")==0)
   	{
		strncpy(UserPass, nvram_safe_get_x("General","http_passwd"), 32);
	}	
	//time(&tm);
	//printf("do_auth %s\n", rfctime(&tm));
	//printf("User: %s:%s%s\n", ProductID, UserID, UserPass);
	strncpy(userid, UserID, AUTH_MAX);

	if (redirect || !is_auth())
	{
		//printf("Disable password checking!!!\n");
		strcpy(passwd, "");
	}
	else
	{
		strncpy(passwd, UserPass, AUTH_MAX);
	}
	strncpy(realm, ProductID, AUTH_MAX);	
}
#endif


static void
do_apply_cgi(char *url, FILE *stream)
{
	char *path, *query;
        
        cprintf(" Before Apply : %s\n", url);
        
        websScan(url);        
	query = url;
	path = strsep(&query, "?") ? : url;	
#ifndef HANDLE_POST	
	init_cgi(query);
#endif	
	apply_cgi(stream, NULL, NULL, 0, url, path, query);
#ifndef HANDLE_POST	
	init_cgi(NULL);
#endif	
}

#ifdef HANDLE_POST
static char post_buf[10000] = { 0 };
#endif

static void
do_apply_cgi_post(char *url, FILE *stream, int len, char *boundary)
{
	//printf("In : %s %s\n", url, boundary);
#ifdef HANDLE_POST	
	if (!fgets(post_buf, MIN(len+1, sizeof(post_buf)), stream)) return;
	len -= strlen(post_buf);
	init_cgi(post_buf);
	while(len--) (void) fgetc(stream);
#endif	
}


#ifdef REMOVE_WL600
static void
do_internal_cgi(char *url, FILE *stream)
{
	char *path, *query;

	query = url;
	path = strsep(&query, "?") ? : url;
	init_cgi(query);
	internal_cgi(stream, NULL, NULL, 0, url, path, query);
}
#endif

#if defined(linux)


static void
do_webcam_cgi(char *url, FILE *stream)
{
	#define MAX_RECORD_IMG 12
	char *query, *path;
	char pic[32];
	int query_idx, last_idx;
	        
	query = url;
	path = strsep(&query, "?") ? : url;
	cprintf("WebCam CGI\n");
	//ret = fcntl(fileno(stream), F_GETOWN, 0);
	query_idx = atoi(query);
	last_idx = atoi(nvram_get_f("webcam.log", "WebPic"));
	//pic = nvram_get_f("webcam.log","WebPic");
			
	if (query_idx==0) sprintf(pic, "../var/tmp/display.jpg");
	else sprintf(pic, "../var/tmp/record%d.jpg", (query_idx>last_idx+1) ? (last_idx+1+MAX_RECORD_IMG-query_idx):(last_idx+1-query_idx));
	
	cprintf("\nWebCam: %s\n", pic);
	do_file(pic, stream);
}

static void
do_upgrade_post(char *url, FILE *stream, int len, char *boundary)
{
	#define MAX_VERSION_LEN 64
	char upload_fifo[] = "/tmp/linux.trx";
	FILE *fifo = NULL;
	/*char *write_argv[] = { "write", upload_fifo, "linux", NULL };*/
	char buf[1024];
	int count, ret = EINVAL, ch;
	int cnt;
	long filelen, *filelenptr;
	int hwmajor=0, hwminor=0;
	char version[MAX_VERSION_LEN], cmpHeader;
	
	cprintf("Start upgrade!!!\n");
	eval("stopservice");
	
	/* Look for our part */
	while (len > 0) 
	{
	     if (!fgets(buf, MIN(len + 1, sizeof(buf)), stream))
	     {			
			goto err;
	     }			
		
	     len -= strlen(buf);
		
	     if (!strncasecmp(buf, "Content-Disposition:", 20) &&
		    strstr(buf, "name=\"file\""))
			break;			
	}
	
	/* Skip boundary and headers */
	while (len > 0) {
		if (!fgets(buf, MIN(len + 1, sizeof(buf)), stream))
		{
			goto err;
		}		
		len -= strlen(buf);
		if (!strcmp(buf, "\n") || !strcmp(buf, "\r\n"))
		{
			break;
		}
	}

	if (!(fifo = fopen(upload_fifo, "a+"))) goto err;
				
        filelen = len;    
        cnt = 0;
        
	/* Pipe the rest to the FIFO */
	//printf("Upgrading %d\n", len);
	cmpHeader = 0;
	
	while (len > 0 && filelen>0) 
	{				
		if (waitfor(fileno(stream), 10) <= 0)
		{
			/*printf("Break while len=%x filelen=%x\n", len, filelen);*/
			break;
		}
                				
		count = fread(buf, 1, MIN(len, sizeof(buf)), stream);
		
		if (cnt == 0 && count>8) 
		{		  
		    if (strncmp(buf, IMAGE_HEADER, 4)!=0) 
		    {
		       /*printf("Header %x %x %x %x\n", buf[0], buf[1], buf[2], buf[3]);*/
		       len-=count;
		       goto err;
		    }
		    else cmpHeader = 1;	   
		    		   
		    filelenptr = (long *)(buf+4);
		    filelen = *filelenptr;		  
		    /*printf("Filelen: %x %x %x %x %x %x\n", filelen, count, (unsigned long)(buf+4), (unsigned long)(buf+7), buf[5], buf[4]);*/
		    cnt ++;
		}
		
		filelen-=count; 
		len-=count;	
										
		fwrite(buf, 1, count, fifo);
		/*printf(".");*/								   		
	}		 		
	
	if (!cmpHeader) goto err;		

	/* Slurp anything remaining in the request */
	while (len-- > 0)
	{
		ch = fgetc(stream);
		
		if (filelen>0)
		{
		   fwrite(&ch, 1, 1, fifo);
		   filelen--;
		}   
	}	
	
	for(cnt=0;cnt<MAX_VERSION_LEN;cnt++) version[cnt] = 0;
	   		
	fseek(fifo, -MAX_VERSION_LEN, SEEK_END);
	fread(version, 1, MAX_VERSION_LEN, fifo);
	
	sscanf(nvram_get_x("general.log", "HardwareVer"), "%d.%d", &hwmajor, &hwminor);
	cprintf("Hardware : %d.%d %s", hwmajor, hwminor, version+4);
	
	if ((strncmp(ProductID, version+4, strlen(ProductID))==0 &&
	     strncmp(version+4, "WL500gx", 7)!=0) || 
	    (strncmp(ProductID, "WL500g.Deluxe", 13)==0 &&
	     strncmp(version+4, "WL500gx", 7)==0)
	    && checkVersion(version,hwmajor,hwminor)) ret = 0;
	
		
	fseek(fifo, 0, SEEK_END);
	fclose(fifo);
	fifo = NULL;
	/*printf("done\n");*/
 	
 err:
	if (fifo)
	   fclose(fifo);
		
	/* Slurp anything remaining in the request */
	while (len-- > 0)	
		ch = fgetc(stream);				
		
#ifdef REMOVE_WL600
	unlink(upload_fifo);
#endif	
	//printf("Error : %d\n", ret);	
	fcntl(fileno(stream), F_SETOWN, -ret);
}

static void
do_upgrade_cgi(char *url, FILE *stream)
{
	int ret;
	
	//printf("Upgrade CGI\n");

	ret = fcntl(fileno(stream), F_GETOWN, 0);
	
	/* Reboot if successful */
	if (ret == 0)
	{
                websApply(stream, "Updating.asp"); 
		sys_upgrade("/tmp/linux.trx");
		sys_forcereboot();
	}	
	else    
	{
	   	websApply(stream, "UpdateError.asp");
	   	unlink("/tmp/linux.trx");
	}   	
	  
}

static void
do_upload_post(char *url, FILE *stream, int len, char *boundary)
{
	#define MAX_VERSION_LEN 64
	char upload_fifo[] = "/tmp/settings_u.prf";
	FILE *fifo = NULL;
	/*char *write_argv[] = { "write", upload_fifo, "linux", NULL };*/
	char buf[1024];
	int count, ret = EINVAL, ch;
	int cnt;
	long filelen, *filelenptr;
//	char version[MAX_VERSION_LEN];
	char cmpHeader;
	
	cprintf("Start upload\n");
	eval("stopservice");
	
	/* Look for our part */
	while (len > 0) 
	{
	     if (!fgets(buf, MIN(len + 1, sizeof(buf)), stream))
	     {			
			goto err;
	     }			
		
	     len -= strlen(buf);
		
	     if (!strncasecmp(buf, "Content-Disposition:", 20) &&
		    strstr(buf, "name=\"file\""))
			break;			
	}
	
	/* Skip boundary and headers */
	while (len > 0) {
		if (!fgets(buf, MIN(len + 1, sizeof(buf)), stream))
		{
			goto err;
		}		
		len -= strlen(buf);
		if (!strcmp(buf, "\n") || !strcmp(buf, "\r\n"))
		{
			break;
		}
	}

	if (!(fifo = fopen(upload_fifo, "a+"))) goto err;
				
        filelen = len;    
        cnt = 0;
        
	/* Pipe the rest to the FIFO */
	cprintf("Upgrading %d\n", len);
	cmpHeader = 0;
	
	while (len > 0 && filelen>0) 
	{				
		if (waitfor(fileno(stream), 10) <= 0)
		{
			//printf("Break while len=%x filelen=%x\n", len, filelen);
			break;
		}
                				
		count = fread(buf, 1, MIN(len, sizeof(buf)), stream);
		
		if (cnt == 0 && count>8) 
		{		  
		    if (strncmp(buf, PROFILE_HEADER, 4)!=0) 
		    {
		       //printf("Header %x %x %x %x\n", buf[0], buf[1], buf[2], buf[3]);
		       len-=count;
		       goto err;
		    }
		    else cmpHeader = 1;	   
		    		   
		    filelenptr = (long *)(buf+4);
		    filelen = *filelenptr;		  
		    //printf("Filelen: %x %x %x %x %x %x\n", filelen, count, (unsigned long)(buf+4), (unsigned long)(buf+7), buf[5], buf[4]);
		    cnt ++;
		}
		
		filelen-=count; 
		len-=count;	
										
		fwrite(buf, 1, count, fifo);
		//printf(".");								   
	}		 		
	
	if (!cmpHeader) goto err;		

	/* Slurp anything remaining in the request */
	while (len-- > 0)
	{
		ch = fgetc(stream);
		
		if (filelen>0)
		{
		   fwrite(&ch, 1, 1, fifo);
		   filelen--;
		}   
	}	
	
#ifdef REMOVE_WL600	
	for(cnt=0;cnt<MAX_VERSION_LEN;cnt++) version[cnt] = 0;
	   		
	fseek(fifo, -MAX_VERSION_LEN, SEEK_END);
	fread(version, 1, MAX_VERSION_LEN, fifo);
	
	sscanf(nvram_get_x("general.log", "HardwareVer"), "%d.%d", &hwmajor, &hwminor);
	cprintf("Hardware : %d.%d", hwmajor, hwminor);
	
	if (checkVersion(version,hwmajor,hwminor)) ret = 0;
#else
	ret = 0;	
#endif
		
	fseek(fifo, 0, SEEK_END);
	fclose(fifo);
	fifo = NULL;
	/*printf("done\n");*/
 	
 err:
	if (fifo)
	   fclose(fifo);
		
	/* Slurp anything remaining in the request */
	while (len-- > 0)	
		ch = fgetc(stream);
		
#ifdef REMOVE_WL600
	unlink(upload_fifo);
#endif	
	//printf("Error : %d\n", ret);
	fcntl(fileno(stream), F_SETOWN, -ret);
}

static void
do_upload_cgi(char *url, FILE *stream)
{
	int ret;
	
	//printf("Upgrade CGI\n");

	ret = fcntl(fileno(stream), F_GETOWN, 0);
	
	/* Reboot if successful */
	if (ret == 0)
	{
                websApply(stream, "Uploading.asp"); 
		sys_upload("/tmp/settings_u.prf");
		sys_commit();
		sys_reboot();
	}	
	else    
	{
	   	websApply(stream, "UploadError.asp");
	   	//unlink("/tmp/settings_u.prf");
	}   	
	  
}


static void
do_prf_file(char *url, FILE *stream)
{
	sys_commit(NULL);
	sys_download("/tmp/settings");
	do_file("/tmp/settings", stream);
	//unlink("/tmp/settings");
}


static void
do_upload_file(char *upload_file, char *url, FILE *stream, int len, char *boundary)
{
	FILE *fifo = NULL;
	char buf[1024];
	int count, ret = EINVAL;

	cprintf("Start upload\n");
	/* Look for our part */
	while (len > 0) 
	{
	     if (!fgets(buf, MIN(len + 1, sizeof(buf)), stream))
	     {			
			goto err;
	     }			
		
	     len -= strlen(buf);
		
	     if (!strncasecmp(buf, "Content-Disposition:", 20) &&
		    strstr(buf, "name=\"file\""))
			break;			
	}

	/* Skip boundary and headers */
	while (len > 0) {
		if (!fgets(buf, MIN(len + 1, sizeof(buf)), stream))
		{
			goto err;
		}		
		len -= strlen(buf);
		if (!strcmp(buf, "\n") || !strcmp(buf, "\r\n"))
		{
			break;
		}
	}

	if (!(fifo = fopen(upload_file, "w"))) goto err;
				
	len -= strlen(boundary)+8;
	cprintf("Upgrading %d\n", len);
	while (len > 0)
	{
		count = fread(buf, 1, MIN(len, sizeof(buf)), stream);
		fwrite(buf, 1, count, fifo);
		len -= count;
	}
	fread(buf, 1, strlen(boundary)+8, stream);
	fflush(fifo);
	ret = 0;
 err:
	if (fifo)
	   fclose(fifo);
		
	//printf("Error : %d\n", ret);
	fcntl(fileno(stream), F_SETOWN, -ret);
}

static void
do_uploadflashfs_post(char *url, FILE *stream, int len, char *boundary)
{
  char upload_file[]="/tmp/flash.tar.gz";
  
  do_upload_file(upload_file, url, stream, len, boundary);
  
  if(eval("tar","ztf",upload_file) != 0)
   fcntl(fileno(stream), F_SETOWN, -EINVAL);
}

static void
do_uploadflashfs_cgi(char *url, FILE *stream)
{
	int ret;
	
	//printf("Upgrade CGI\n");
	ret = fcntl(fileno(stream), F_GETOWN, 0);
	/* Reboot if successful */
	if (ret == 0)
	{
		websApply(stream, "Uploading.asp"); 
		eval("flashfs","commit");
		eval("flashfs","enable");
		sys_reboot();
	}	
	else    
	{
	   	websApply(stream, "UploadError.asp");
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

#elif defined(vxworks)

static void
do_upgrade_post(char *url, FILE *stream, int len, char *boundary)
{
}

static void
do_upgrade_cgi(char *url, FILE *stream)
{
}

#endif

static char no_cache[] =
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

#ifdef WEBS
void
initHandlers(void)
{                
	websAspDefine("nvram_get_x", ej_nvram_get_x);
	websAspDefine("nvram_get_f", ej_nvram_get_f);
	websAspDefine("nvram_get_list_x", ej_nvram_get_list_x);	
	websAspDefine("nvram_get_buf_x", ej_nvram_get_buf_x);	
	websAspDefine("nvram_get_table_x", ej_nvram_get_table_x);	
	websAspDefine("nvram_match_x", ej_nvram_match_x);
	websAspDefine("nvram_double_match_x", ej_nvram_double_match_x);
    	websAspDefine("nvram_match_both_x", ej_nvram_match_both_x);	
	websAspDefine("nvram_match_list_x", ej_nvram_match_list_x);
	websAspDefine("select_channel", ej_select_channel);
	websAspDefine("select_country", ej_select_country);
    	websAspDefine("urlcache", ej_urlcache);	
	websAspDefine("uptime", ej_uptime);
	websAspDefine("nvram_dump", ej_dump);	
    	websAspDefine("load_script", ej_load);	
	websSecurityDelete();
	websUrlHandlerDefine("", NULL, 0, mySecurityHandler, WEBS_HANDLER_FIRST);	
	websUrlHandlerDefine("/apply.cgi", NULL, 0, apply_cgi, 0);	
	websSetPassword(nvram_safe_get_x("General", "x_Password"));
	websSetRealm(nvram_safe_get_x("general.log", "ProductID"));
}


#else
struct mime_handler mime_handlers[] = {
	{ "**.asp", "text/html", no_cache, NULL, do_ej, do_auth },
	{ "**.htm", "text/html", no_cache, NULL, do_ej, do_auth },
	{ "**.css", "text/css", NULL, NULL, do_file, do_auth },
	{ "**.gif", "image/gif", NULL, NULL, do_file, do_auth },
	{ "**.jpg", "image/jpeg", NULL, NULL, do_file, do_auth },
	{ "**.js",  "text/javascript", NULL, NULL, do_file, do_auth },
	{ "**.cab", "text/txt", NULL, NULL, do_file, do_auth },
	{ "**.CFG", "text/txt", NULL, NULL, do_prf_file, do_auth },
	{ "**.tar.gz", "text/txt", NULL, NULL, do_flashfs_file, do_auth },
	{ "apply.cgi*", "text/html", no_cache, do_apply_cgi_post, do_apply_cgi, do_auth },	
	{ "upgrade.cgi*", "text/html", no_cache, do_upgrade_post, do_upgrade_cgi, do_auth},
	{ "upload.cgi*", "text/html", no_cache, do_upload_post, do_upload_cgi, do_auth },
	{ "uploadflashfs.cgi*", "text/html", no_cache, do_uploadflashfs_post, do_uploadflashfs_cgi, do_auth },
 	{ "syslog.cgi*", "text/txt", no_cache, NULL, do_log_cgi, do_auth },
	{ "webcam.cgi*", "text/html", no_cache, NULL, do_webcam_cgi, do_auth },
	{ NULL, NULL, NULL, NULL, NULL, NULL }
};


static int
ej_select_folder(int eid, webs_t wp, int argc, char_t **argv)
{
	char *sid;
	int ret = 0;	
	char out[64];	
	int i;
	DIR *dir;
	struct dirent *entry;

	if (ejArgs(argc, argv, "%s", &sid) < 1) {
		websError(wp, 400, "Insufficient args\n");
		return -1;
	}
         
        i=0;  
              
	/* display folder in first level */
	dir = opendir("/");
	if (dir!=NULL)
	{
		i=0;

		while((entry=readdir(dir))!=NULL)
		{	
             	 	sprintf(out, "<option value=\"%s\"", entry->d_name);
                 	sprintf(out,"%s>%s</option>", out, entry->d_name);                 
                 	websWrite(wp, out);
       		} 
        	closedir(dir);
	}     
	return ret;
}


static int
ej_select_list(int eid, webs_t wp, int argc, char_t **argv)
{
	char *id;
	int ret = 0;	
	char out[64], idxstr[12], tmpstr[64], tmpstr1[64];
	int i, curr, hit, noneFlag;
	char *ref1, *ref2, *refnum;

	if (ejArgs(argc, argv, "%s", &id) < 1) {
		websError(wp, 400, "Insufficient args\n");
		return -1;
	}

	//printf("id: %s\n", id);

        if (strcmp(id, "Storage_x_SharedPath")==0)
        {
		ref1 = "sh_path_x";
		ref2 = "sh_path";
		refnum = "sh_num";

		curr = atoi(nvram_get(ref1));
		noneFlag = 0;
        }
	else if (strncmp(id, "Storage_x_AccUser", 17)==0)
	{
		sprintf(tmpstr, "sh_accuser_x%s", id + 17);
		ref2 = "acc_username";
		refnum = "acc_num";

		curr = atoi(nvram_get(tmpstr));
		noneFlag =1;
	}
	else if (strcmp(id, "Storage_x_AccAny")==0)
	{
		
                 ret = websWrite(wp, "<option value=\"Guest\">Guest</option>");
		 return ret;
	}
	else 
	{
		 return ret;     
	}
        

	//printf("out %s\n", curr);
	hit = 0;
 
	for(i=0;i<atoi(nvram_get(refnum));i++)
	{     
 		 sprintf(idxstr, "%d", i);
		 strcpy(tmpstr1, nvram_get(strcat_r(ref2, idxstr, tmpstr)));
             	 sprintf(out, "<option value=\"%d\"", i);

                 if (i==curr) 
		 {
			hit = 1;
			sprintf(out, "%s selected", out);
		 }
                 sprintf(out,"%s>%s</option>", out, tmpstr1);       
 
		 //printf("out: %s\n", out);

                 ret += websWrite(wp, out);
	}     

	if (noneFlag)
	{
		cprintf("hit : %d\n", hit);
		if (!hit) sprintf(out, "<option value=\"99\" selected>None</option>");
		else sprintf(out, "<option value=\"99\">None</option>");

		ret += websWrite(wp, out);
	}	
	return ret;
}


struct ej_handler ej_handlers[] = {
	{ "nvram_get_x", ej_nvram_get_x},
	{ "nvram_get_f", ej_nvram_get_f},
	{ "nvram_get_list_x", ej_nvram_get_list_x},
	{ "nvram_get_buf_x", ej_nvram_get_buf_x},
	{ "nvram_get_table_x", ej_nvram_get_table_x},
	{ "nvram_match_x", ej_nvram_match_x},
	{ "nvram_double_match_x", ej_nvram_double_match_x},
        { "nvram_match_both_x", ej_nvram_match_both_x},
	{ "nvram_match_list_x", ej_nvram_match_list_x},
	{ "select_channel", ej_select_channel},
	{ "select_country", ej_select_country},
	{ "urlcache", ej_urlcache},     
	{ "uptime", ej_uptime},   
	{ "nvram_dump", ej_dump},
	{ "load_script", ej_load},
	{ "select_folder", ej_select_folder},
	{ "select_list", ej_select_list},
	{ NULL, NULL }
};

#endif /* !WEBS */

void websSetVer(void)
{
	FILE *fp;
	unsigned long *imagelen;
	char dataPtr[4];
	char verPtr[64];
	char productid[13];
	char fwver[12];

	strcpy(productid, "WLHDD");
	strcpy(fwver, "0.1.0.1");

	if ((fp = fopen(MTD_DEV(1), "rb"))!=NULL)
	{
		if (fseek(fp, 4, SEEK_SET)!=0) goto write_ver;
		fread(dataPtr, 1, 4, fp);
		imagelen = (unsigned long *)dataPtr;

		cprintf("image len %x\n", *imagelen);
		if (fseek(fp, *imagelen - 64, SEEK_SET)!=0) goto write_ver;
		cprintf("seek\n");
		if (!fread(verPtr, 1, 64, fp)) goto write_ver;
		cprintf("ver %x %x", verPtr[0], verPtr[1]);
		strncpy(productid, verPtr + 4, 12);
		productid[12] = 0;
		sprintf(fwver, "%d.%d.%d.%d", verPtr[0], verPtr[1], verPtr[2], verPtr[3]);

		cprintf("get fw ver: %s\n", productid);
		fclose(fp);
	}
write_ver:
	nvram_set_f("general.log", "productid", productid);
	nvram_set_f("general.log", "firmver", fwver);	
}

/*
 * Country names and abbreviations from ISO 3166
 */
typedef struct {
	char *name;     /* Long name */
	char *abbrev;   /* Abbreviation */
} country_name_t;
country_name_t country_names[];     /* At end of this file */


/*
 * Country names and abbreviations from ISO 3166
 */
country_name_t country_names[] = {

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
ej_select_country(int eid, webs_t wp, int argc, char_t **argv)
{
	char *country, *sid;	
	country_name_t *cntry;
	int ret=0;

	if (ejArgs(argc, argv, "%s", &sid) < 1) {
		websError(wp, 400, "Insufficient args\n");
		return -1;
	}
	
        country=nvram_safe_get_x("", "wl_country_code");
                                
	for(cntry = country_names; cntry->name; cntry++)
	{ 
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

	i=0;
	
	if (nvram_match_x("", "wl_chan_list", "")) {
		for(i=0;i<14;i++)
		{
			channels[i] = i+1;
			channels[i+1] = -1;
		}	
	}
	else {
		foreach(var, nvram_safe_get_x("", "wl_chan_list"), next) {
			channels[i]=atoi(var);
			channels[i+1]=-1;
			i++;
		}
	}
	return i;
}

static int
wl_channels_in_country(char *abbrev, int channels[])
{
	int i, j, unit;
	wl_channels_in_country_t *cic = (wl_channels_in_country_t *)ibuf2;
	char tmp[100], prefix[] = "wlXXXXXXXXXX_";
	char *name;

	if ((unit = atoi(nvram_safe_get_x("", "wl_unit"))) < 0)
		return -1;

	snprintf(prefix, sizeof(prefix), "wl%d_", unit);
	name = nvram_safe_get_x("", strcat_r(prefix, "ifname", tmp));		
	

	i = 0;
	channels[i++] = 0;
	channels[i] = -1;

	if (strlen(abbrev)==0) return 0;

	cic->buflen = sizeof(ibuf2);
	strcpy(cic->country_abbrev, abbrev);
	cic->band = WLC_BAND_2G;
	cic->count = 0;


	if (wl_ioctl(name, WLC_GET_CHANNELS_IN_COUNTRY, cic, cic->buflen) == 0) 	{
		if (cic->count == 0)
			return 0;

		for(j = 0; j < cic->count; j++)
		{
			channels[i++] = cic->channel[j];
		}

		channels[i] = -1;
	}
	return cic->count;

}
