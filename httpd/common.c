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
#include <syslog.h>

#include <shutils.h>
#include <bcmconfig.h>

#include "bcmnvram_f.h"
#include "common.h"

#include "variables.c"

/* API export for UPnP function */
int LookupServiceId(const char *serviceId)
{
	int sid = 0;

	while (svcLinks[sid].serviceId!=NULL) {
		if (strcmp(serviceId, svcLinks[sid].serviceId) == 0)
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
