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

#ifndef _LANGUAGE_ASSEMBLY


extern char * nvram_get_x(const char *sid, const char *name);
extern char * nvram_get_f(const char *file, const char *field);

/* 
 * Get the value of an NVRAM variable
 * @param	name	name of variable to get
 * @return	value of variable or NUL if undefined
 */
/*#define nvram_safe_get(name)   (nvram_get(name) ? : "")*/
#define nvram_safe_get_x(sid, name) (nvram_get_x(sid, name) ? : "")
#define nvram_safe_get_f(file, field) (nvram_get_f(file, field) ? : "")

/*
 * Match an NVRAM variable
 * @param	name	name of variable to match
 * @param	match	value to compare against value of variable
 * @return	TRUE if variable is defined and its value is string equal to match or FALSE otherwise
 */
#define nvram_match_x(sid, name, match) ({ \
	const char *value = nvram_get_x(sid, name); \
	(value && !strcmp(value, match)); \
})


/*
 * Match an NVRAM variable
 * @param	name	name of variable to match
 * @param	match	value to compare against value of variable
 * @return	TRUE if variable is defined and its value is string equal to match or FALSE otherwise
 */
#define nvram_match_list_x(sid, name, match, index) ({ \
	const char *value = nvram_get_list_x(sid, name, index); \
	(value && !strcmp(value, match)); \
})

/*
 * Match an NVRAM variable
 * @param	name	name of variable to match
 * @param	match	value to compare against value of variable
 * @return	TRUE if variable is defined and its value is not string equal to invmatch or FALSE otherwise
 */
#define nvram_invmatch_x(sid, name, invmatch) ({ \
	const char *value = nvram_get_x(sid, name); \
	(value && strcmp(value, invmatch)); \
})


/*
 * Set the value of an NVRAM variable
 * @param	name	name of variable to set
 * @param	value	value of variable
 * @return	0 on success and errno on failure
 * NOTE: use nvram_commit to commit this change to flash.
 */
extern int nvram_set_x(const char *sid, const char *name, const char *value);
extern int nvram_set_f(const char *file, const char *field, const char *value);

/*
 * Get all NVRAM variables (format name=value\0 ... \0\0)
 * @param	buf	buffer to store variables
 * @param	count	size of buffer in bytes
 * @return	0 on success and errno on failure
 */
extern int nvram_getall_x(const char *sid);

extern char *nvram_get_list_x(const char *sid, const char *name, int index);
extern int nvram_add_list_x(const char *sid, const char *name, const char *value);
extern int nvram_del_list_x(const char *sid, const char *name, int index);
#endif /* _LANGUAGE_ASSEMBLY */
#endif /* _bcmnvram_f_h_ */
