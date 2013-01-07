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
	int sid;

	for (sid = 0; svcLinks[sid].serviceId != NULL; sid++) {
		if (strcmp(serviceId, svcLinks[sid].serviceId) == 0)
			return sid;
	}

	return -1;
}

const struct variable *GetVariables(int sid)
{
	return svcLinks[sid].variables;
}

const struct group_variable *LookupGroup(const char *groupName)
{
	const struct group_variable *gv;

	/* Find group */
	for (gv = grpList; gv->name != NULL; gv++) {
		if (strcmp(groupName, gv->name) == 0)
			return gv;
	}

	return NULL;
}
