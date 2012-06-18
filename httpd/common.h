/*
    Copyright 2001, Broadcom Corporation
    All Rights Reserved.
    
    This is UNPUBLISHED PROPRIETARY SOURCE CODE of Broadcom Corporation;
    the contents of this file may not be disclosed to third parties, copied or
    duplicated in any form, in whole or in part, without the prior written
    permission of Broadcom Corporation.
*/
/*
 * NVRAM variable manipulation
 *
 * Copyright (C) 2001 Broadcom Corporation
 *
 * $Id: common.h,v 1.2 2004/01/05 05:39:18 Cheni_Shen Exp $
 */

#ifndef _COMMON_H_
#define _COMMON_H_

#ifndef TRUE
#define TRUE 1
#endif

#ifndef FALSE
#define FALSE 0
#endif

#define MAX_LINE_SIZE 1024

struct variable {
	const char *name;
	short fldLen;
};

struct group_variable {
	const char *name;
	const struct variable *variables;
	short rowLen;
	const char *counter_name;
};

struct svcLink
{
      const char *serviceId;
      const struct variable *variables;
};


enum {
	NOTHING,
	REBOOT,
	RESTART,
};


#ifdef __cplusplus
extern "C" {
#endif

/* API export */
int LookupServiceId(const char *serviceId);
const struct variable *GetVariables(int sid);
const struct group_variable *LookupGroup(const char *groupName);

#ifdef __cplusplus
}
#endif

#endif /* _COMMON_H_ */
