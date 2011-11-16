#ifdef QOS
#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <time.h>
#include <unistd.h>
#include <errno.h>
#include <syslog.h>
#include <signal.h>
#include <string.h>
#include <sys/klog.h>
#include <sys/types.h>
#include <sys/mount.h>
#include <sys/reboot.h>
#include <sys/stat.h>
#include <sys/sysmacros.h>
#include <sys/time.h>
#include <sys/wait.h>
#include <sys/socket.h>
#include <net/if_arp.h>
#include <dirent.h>
                                                                                                                                               
#include <epivers.h>
#include <shutils.h>
#include "rc.h"

static char *Ch_conv(char *proto_name, int idx)
{
	char qos_name_x[32];

	snprintf(qos_name_x, sizeof(qos_name_x), "%s%d", proto_name, idx);
	if (nvram_match(qos_name_x,""))
	{
		return NULL;
	}
	else 
	{
		return nvram_get(qos_name_x);
	}
}

static void tc_qdisc(char *move, char *network, int idx)
{
//	printf("Making qdisc for %s ...\n", network);
	char flowid[32];

	snprintf(flowid, sizeof(flowid), "%s%d", "1", idx);
	eval("tc","qdisc",move,"dev",network,"root","handle","1:","htb", "default",flowid);
}

static void tc_class(char *network, int idx, char *minBW, char *maxBW)
{
	char flowid[32];
	char max_bw[32];
	char min_bw[32];

	if (maxBW==NULL)
	{
		maxBW="100000";
	}
	else if (minBW==NULL)
	{
		minBW=maxBW;
	}
	snprintf(flowid, sizeof(flowid), "%s%d", "1:1", idx);
	sprintf(max_bw, "%s%s", maxBW, "kbit");
	sprintf(min_bw, "%s%s", minBW, "kbit");
	eval("tc","class","add","dev",network,"parent","1:1","classid",flowid,"htb","rate",min_bw,"ceil",max_bw);
}

static void tc_filter_D(char *network, char *ipaddr, char *port, int idx)
{
	char flowid[32];
	char flowid_lan[32];
	char lan_ipaddr[32];
	
	int rulenum=atoi(nvram_safe_get("qos_rulenum_x"));
	int urulenum=atoi(nvram_safe_get("qos_urulenum_x")); 
	int idx_class_D80=rulenum+urulenum;

	snprintf(flowid, sizeof(flowid), "%s%d", "1:1", idx);
	snprintf(flowid_lan, sizeof(flowid_lan), "%s%d", "1:1", idx_class_D80);
	sprintf(lan_ipaddr, "%s%s", (nvram_get("lan_ipaddr")), "/24");

	if ((ipaddr==NULL)&&(port==NULL)) //Make default class if ip/port both are NULL
	{
		eval("tc","filter","add","dev",network,"protocol","ip","parent","1:0","prio","3",\
			"u32","match","ip","dst",lan_ipaddr,"flowid",flowid);
	}
	else if (ipaddr==NULL) //Make filter for NULL ip
	{
		if (atoi(port)==80) //solve when port=80, the translation rate on WL500gx will be slow
		{
			tc_class(network, idx_class_D80, "10000", "10000");
			eval("tc","filter","add","dev",network,"protocol","ip","parent","1:0","prio","2",\
				"u32","match","ip","sport",port,"0xffff","match","ip","dst",lan_ipaddr,"flowid",flowid);
			eval("tc","filter","add","dev",network,"protocol","ip","parent","1:0","prio","1",\
				"u32","match","ip","src",nvram_get("lan_ipaddr"),"flowid", flowid_lan);
		}
		else
		{
			eval("tc","filter","add","dev",network,"protocol","ip","parent","1:0","prio","2",\
				"u32","match","ip","sport",port,"0xffff","match","ip","dst",lan_ipaddr,"flowid",flowid);
		}
	}
	else if (port==NULL) //Make filter for NULL port
	{
		eval("tc","filter","add","dev",network,"protocol","ip","parent","1:0","prio","2",\
			"u32","match","ip","dst",ipaddr,"flowid",flowid);
	}
	else 
	{
		eval("tc","filter","add","dev",network,"protocol","ip","parent","1:0","prio","2","u32",\
		"match","ip","dst",ipaddr,"match","ip","sport",port,"0xffff","flowid",flowid);
	}
}

static void tc_filter_U(char *wan_ipaddr, char *network, char *port, int idx)
{
	char flowid[32];

	snprintf(flowid, sizeof(flowid), "%s%d", "1:1", idx);
	if (port==NULL)
	{
		eval("tc","filter","add","dev",network,"protocol","ip","parent","1:0","prio","3","u32","match","ip","src",wan_ipaddr,"flowid",flowid);
	}
	else 
	{
		eval("tc","filter","add","dev",network,"protocol","ip","parent","1:0","prio","2","u32","match","ip","src",wan_ipaddr,"match","ip","sport",port,"0xffff","flowid",flowid);
	}
}


void start_qos(char *wan_ipaddr)
//void main(void)
{
	//printf("starting QoS service ...\n");
	int rulenum=atoi(nvram_safe_get("qos_rulenum_x")); //Download stream number
	int urulenum=atoi(nvram_safe_get("qos_urulenum_x")); //Upload stream number
	int idx_class_D=0,idx_class_U=0, idx_filter_D=0, idx_filter_U=0; //control index
	int def=rulenum+urulenum+1; //default Down/Up stream or total stream(rule) number
	int idx=0;
	char net_name[32];
	
	if (nvram_match("qos_enable_x", "1"))
	{
		for (idx=0;idx<=2;idx++)
		{
		
		sprintf(net_name, "%s%d", "eth", idx);
		//printf("Begin to initialize qdisc for Qos ...\n"); 
		//initialize qdisc
		tc_qdisc("del", net_name, def);
		
		//printf("starting Qdisc action ...\n");
		//start making qdisc for QoS
		tc_qdisc("add", net_name, def);

		//printf("starting partation classes ... \n");
		//make the top class 
		eval("tc","class","add","dev",net_name,"parent","1:","classid","1:1","htb"\
			,"rate","100Mbps","ceil","100Mbps");

		//printf("starting making children classes ...\n");
		for (idx_class_D=0; idx_class_D<=rulenum-1; idx_class_D++) //Download classes
		{
			tc_class(net_name, idx_class_D,\
				Ch_conv("qos_minbw_x", idx_class_D),Ch_conv("qos_maxbw_x", idx_class_D));
		}
		for (idx_class_U=0; idx_class_U<=urulenum-1; idx_class_U++) //Upload classes
		{
			tc_class(net_name, idx_class_U+idx_class_D,\
				Ch_conv("qos_uminbw_x", idx_class_U), Ch_conv("qos_umaxbw_x", idx_class_U));
		}
		tc_class(net_name, def,"100000","100000"); //default class

		//make filters
		for (idx_filter_D=0; idx_filter_D<=rulenum-1; idx_filter_D++) //fit to download classes
		{
			tc_filter_D(net_name, Ch_conv("qos_ipaddr_x", idx_filter_D), \
				Ch_conv("qos_port_x", idx_filter_D), idx_filter_D);
		}
		for (idx_filter_U=0; idx_filter_U<=urulenum-1; idx_filter_U++) //fit to upload classes
		{
			tc_filter_U(wan_ipaddr, net_name, \
				Ch_conv("qos_uport_x", idx_filter_U), idx_filter_U+idx_filter_D);
		}
		}
	}
}
#endif
