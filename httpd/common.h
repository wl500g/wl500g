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

struct variable {
	const char *name;
	const char *longname;
	const char **argv;
};

struct svcLink
{
      const char *serviceId;
      const struct variable *variables;
};

#define ARGV(args...) ((const char *[]) { (const char *) args, NULL })

enum {
	NOTHING,
	REBOOT,
	RESTART,
};


#ifdef __cplusplus
extern "C" {
#endif

/* API export for UPnP function */
void InitVariables(void);
int LookupServiceId(const char *serviceId);
const char *GetServiceId(int sid);
const struct variable *GetVariables(int sid);
int CheckVariables(int sid, char *name, char *var);
char *GetVariable(int sid, char *name);
void SetVariable(int sid, char *name, char *value);

const struct variable *LookupGroupVariables(int sid, char *groupName);
int CheckGroupVariables(int sid, struct variable *gvs, char *name, char *var);
void SetGroupVariable(int sid, struct variable *gvs, char *name, char *value, char *action);
char *GetGroupVariable(int sid, struct variable *gvs, char *name);


#ifdef __cplusplus
}
#endif

#endif /* _COMMON_H_ */
