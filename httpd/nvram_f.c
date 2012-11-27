/*
Copyright 2001, Broadcom Corporation
All Rights Reserved.

This is UNPUBLISHED PROPRIETARY SOURCE CODE of Broadcom Corporation;
the contents of this file may not be disclosed to third parties, copied or
duplicated in any form, in whole or in part, without the prior written
permission of Broadcom Corporation.
*/
/*
* This module creates an array of name, value pairs
* and supports updating the nvram space. 
*
*/

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <syslog.h>

#include <bcmnvram.h>
#include <shutils.h>

/*
* Get the value of an NVRAM variable list 
* @param	name	name of variable to get
*              index   index of the list, start from 1
* @return	value of variable or NULL if undefined
*/

char *nvram_get_list(const char *name, int index)
{
	char new_name[128];

	snprintf(new_name, sizeof(new_name), "%s%d", name, index);

	return nvram_safe_get(new_name);
}

/*
* Add the value into the end an NVRAM variable list 
* @param	name	name of variable to get
* @return	0 on success and errno on failure
*/
int nvram_add_lists(const char *name, const char *value, int count)
{
	char name2[64];

	if (name[0] != '\0') {
		snprintf(name2, sizeof(name2), "%s%d", name, count);
		nvram_set(name2, value);
	}

	return 0;
}


/*
* Delete the value from an NVRAM variable list 
* @param	name	name of variable list
*              index   index of variable list
* @return	0 on success and errno on failure
*/
int nvram_del_lists(const char *name, int *delMap)
{
	char oname[64], nname[64], *oval, *nval;
	int oi, ni, di;

	if (name[0] != '\0') {
		oi=0;
		ni=0;
		di=0;
		while (1) {
			snprintf(oname, sizeof(oname), "%s%d", name, oi);
			snprintf(nname, sizeof(nname), "%s%d", name, ni);

			oval = nvram_get(oname);
			nval = nvram_get(nname);

			if (oval==NULL) break;

			dprintf("d: %d %d %d %d\n", oi, ni, di, delMap[di]);
			if (delMap[di]!=-1 && delMap[di]==oi) {
				oi++;
				di++;
			} else {
				nvram_set(nname, oval);
				ni++;
				oi++;
			}
		}
	}

	return (0);
}
