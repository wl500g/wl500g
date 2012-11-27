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
 * $Id: bcmnvram_f.h,v 1.1 2003/12/04 02:13:03 Cheni_Shen Exp $
 */

#ifndef _bcmnvram_f_h_
#define _bcmnvram_f_h_

extern char *nvram_get_list(const char *name, int index);

/*
 * Match an NVRAM variable
 * @param	name	name of variable to match
 * @param	match	value to compare against value of variable
 * @return	TRUE if variable is defined and its value is string equal to match or FALSE otherwise
 */
#define nvram_match_list(name, match, index) ({ \
	const char *value = nvram_get_list(name, index); \
	(value && !strcmp(value, match)); \
})


extern int nvram_add_list_x(const char *sid, const char *name, const char *value);
extern int nvram_del_list_x(const char *sid, const char *name, int index);

/*
 * Add the value into the end an NVRAM variable list
 * @param       name    name of variable to get
 * @return      0 on success and errno on failure
 */
int nvram_add_lists(const char *name, const char *value, int count);
/*
 * Delete the value from an NVRAM variable list
 * @param       name    name of variable list
 *              index   index of variable list
 * @return      0 on success and errno on failure
 */
int nvram_del_lists(const char *name, int *delMap);

#endif /* _bcmnvram_f_h_ */
