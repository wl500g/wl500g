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
* $Id: common.c,v 1.2 2004/01/05 05:39:18 Cheni_Shen Exp $
*/

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <fcntl.h>
#include <signal.h>
#include <time.h>
#include <syslog.h>
#include <sys/klog.h>
#include <sys/wait.h>
#include <sys/socket.h>
#include <linux/if.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include <shutils.h>
#include <bcmconfig.h>

#include "bcmnvram_f.h"
#include "common.h"

#define MAX_LINE_SIZE 512
#define UPNP_E_SUCCESS 0
#define UPNP_E_INVALID_ARGUMENT -1


static int
validate_portrange(const char *value, const struct variable *v)
{
	return UPNP_E_SUCCESS;
}

static int
validate_wlchannel(const char *value, const struct variable *v)
{
	return UPNP_E_SUCCESS;
}

static int
validate_wlwep(const char *value, const struct variable *v)
{
	return UPNP_E_SUCCESS;
}

static int
validate_wlkey(const char *value, const struct variable *v)
{
	return UPNP_E_SUCCESS;
}


static int
validate_ipaddr(const char *value, const struct variable *v)
{
	struct in_addr ipaddr;

	if (!inet_aton(value, &ipaddr)) {
		return UPNP_E_INVALID_ARGUMENT;
	}

	return UPNP_E_SUCCESS;
}

static int
validate_choice(const char *value, const struct variable *v)
{
	const char **choice;

	for (choice = v->argv; *choice; choice++) {
		if (!strcmp(value, *choice))
			return UPNP_E_SUCCESS;
	}

	return UPNP_E_INVALID_ARGUMENT;
}

static int
validate_range(const char *value, const struct variable *v)
{
	int n, start, end;

	n = atoi(value);
	start = atoi(v->argv[0]);
	end = atoi(v->argv[1]);

	if (n < start || n > end) {
		return UPNP_E_INVALID_ARGUMENT;
	}

	return UPNP_E_SUCCESS;
}


static int
validate_string(const char *value, const struct variable *v)
{
	int n, max;

	n = atoi(value);
	max = atoi(v->argv[0]);
	syslog(LOG_INFO, "Validate string: %s %x %x\n", value, strlen(value), max);
	if (strlen(value) > max) {
		return UPNP_E_INVALID_ARGUMENT;
	}

	return UPNP_E_SUCCESS;
}

static int
validate_group(const char *value, const struct variable *v)
{
	return(UPNP_E_SUCCESS);
}

static int
validate_hwaddr(const char *value, const struct variable *v)
{
	unsigned int hwaddr[6];

	/* Make exception for "NOT IMPLELEMENTED" string */
	if (!strcmp(value,"NOT_IMPLEMENTED")) 
		return(UPNP_E_SUCCESS);

	/* Check for bad, multicast, broadcast, or null address */

	if (sscanf(value, "%x:%x:%x:%x:%x:%x",
		&hwaddr[0], &hwaddr[1], &hwaddr[2],
		&hwaddr[3], &hwaddr[4], &hwaddr[5]) != 6 ||
		(hwaddr[0] & 1) ||
		(hwaddr[0] & hwaddr[1] & hwaddr[2] & hwaddr[3] & hwaddr[4] & hwaddr[5]) == 0xff ||
		(hwaddr[0] | hwaddr[1] | hwaddr[2] | hwaddr[3] | hwaddr[4] | hwaddr[5]) == 0x00) {
			return UPNP_E_INVALID_ARGUMENT;
	}

	return UPNP_E_SUCCESS;
}

#include "variables.c"

#ifdef REMOVE_NVRAM
void InitVariables(void)
{
	/*nvram_init("all");*/
}
#endif

/* API export for UPnP function */
int LookupServiceId(const char *serviceId)
{
	int sid = 0;

	while (svcLinks[sid].serviceId!=NULL) {      
		if( strcmp(serviceId, svcLinks[sid].serviceId) == 0)
			break;
		sid++;
	} 

	if (svcLinks[sid].serviceId == NULL)
		return -1;
	else
		return sid;
}   

const char *GetServiceId(int sid)
{        
	return (svcLinks[sid].serviceId);   
} 

const struct variable *GetVariables(int sid)
{        
	return (svcLinks[sid].variables);   
}

const struct action *GetActions(int sid)
{        
	return (svcLinks[sid].actions);   
}

const char *GetServiceType(int sid)
{
	return (svcLinks[sid].serviceType);
}

#ifdef REMOVE_CHECK
const struct action *CheckActions(int sid, char *name)
{
	const struct action *a;

	for (a = GetActions(sid); a != NULL && a->name != NULL; a++) {
		if (strcmp(a->name, name)==0) {
			return(a);
		}
	}
	return NULL;
}

int CheckVariables(int sid, char *name, char *var)
{
	const struct variable *v;

	for (v = GetVariables(sid); v->name != NULL; v++) {
		dprintf("Check variables: %s %s\n", v->name, name);
		if (strcmp(v->name, name)==0) {
			if (v->validate != NULL) { 
				if ((v->validate(var, v)) == UPNP_E_SUCCESS)
					return 1;
				else return 0;
			}      
			else return 1;
		}
	}
	return 0;
}

const struct variable *LookupGroupVariables(int sid, char *groupName)
{
	const struct variable *v;

	/* Find group */
	for (v = GetVariables(sid); v->name != NULL; v++) {
		if (strcmp(v->name, groupName)==0) {
			break;
		}
	}

	if (v->name==NULL||v->argv[0]==NULL) return NULL; 

	return(v);
}   

int CheckGroupVariables(int sid, struct variable *gvs, char *name, char *var)
{  
	const struct variable *gv;

	/* Find member of group */
	for (gv = (struct variable *)gvs->argv[0]; gv->name!=NULL; gv++) {
		if (strcmp(gv->name, name)==0) {
			if (gv->validate!=NULL)	{ 
				if ((gv->validate(var, gv))==UPNP_E_SUCCESS)
					return 1;
				else return 0;
			}
		}
	}
	return 0;
}
#endif /* REMOVE_CHECK */

#ifdef REMOVE_NVRAM
char *GetVariable(int sid, char *name)
{
	const struct variable *v;
	char buf[MAX_LINE_SIZE];

	for (v = GetVariables(sid); v->name != NULL; v++) {
		if (strcmp(v->name, name)==0) {
			if (strcmp(v->longname, "Status")!=0) {
				/* syslog(LOG_INFO, "Get variables: %s %s\n", v->name, name, nvram_safe_get_x(svcLinks[sid].serviceId, name));	*/
				return(nvram_safe_get_x(svcLinks[sid].serviceId, name));
			} else {
				strcpy(buf, nvram_safe_get_f(v->argv[0], v->argv[1]));
				/*syslog(LOG_INFO, "Get variables from file: %s %s %s\n", v->argv[0], v->argv[1], buf);	      	    */
				return(buf);
			}
		}
	}
	return("");

}

void SetVariable(int sid, char *name, char *value)
{
	nvram_set_x(svcLinks[sid].serviceId, name, value);
}

void SetGroupVariable(int sid, struct variable *gvs, char *name, char *value, char *action)
{
	int groupCount;
	int i;
	char buf[MAX_LINE_SIZE];

	if (strcmp(action, "Add")==0) {
		nvram_add_list_x(svcLinks[sid].serviceId, name, value);
	} else if (strcmp(action, "Del")==0) {  
		for (i=0; i< groupCount; i++) {
			strcpy(buf, nvram_get_list_x(svcLinks[sid].serviceId, name, i+1));      
			if (strcmp(buf,value)==0) {
				syslog(LOG_INFO, "Del group variables: %d %d %s %s\n", i, groupCount, buf, value);      	    	      	                   	         	
				nvram_del_list_x(svcLinks[sid].serviceId, name, i);  
				break;
			}
		}
	}
}

char *GetGroupVariable(int sid, struct variable *gvs, char *name)
{
	const struct variable *gv;

	for (gv = gvs->argv[0]; gv->name != NULL; gv++) {      
		if (strcmp(gv->name, name)==0) {
			if (strcmp(gv->longname, "Status")!=0) {
				/*syslog(LOG_INFO, "Get group variables: %s %s\n", gv->name, name);		*/
				return(nvram_safe_get_x(svcLinks[sid].serviceId, name));
			} else {
				/*syslog(LOG_INFO, "Get group variables from file: %s %s\n", gv->argv[0], gv->argv[1]);		*/
				return(nvram_safe_get_f(gv->argv[0], gv->argv[1]));
			}
		}
	}
	return("");
}
#endif /* REMOVE_NVRAM */
