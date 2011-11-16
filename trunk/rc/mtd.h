/*
 * MTD utility functions
 *
 * Copyright 2006, Broadcom Corporation
 * All Rights Reserved.
 * 
 * THIS SOFTWARE IS OFFERED "AS IS", AND BROADCOM GRANTS NO WARRANTIES OF ANY
 * KIND, EXPRESS OR IMPLIED, BY STATUTE, COMMUNICATION OR OTHERWISE. BROADCOM
 * SPECIFICALLY DISCLAIMS ANY IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS
 * FOR A SPECIFIC PURPOSE OR NONINFRINGEMENT CONCERNING THIS SOFTWARE.
 *
 * $Id: mtd.h,v 1.1.1.1 2008/07/21 09:17:42 james26_jang Exp $
 */

#ifndef _mtd_h_
#define _mtd_h_

/*
 * Erase an MTD device
 * @param	mtd	path to or partition name of MTD device
 * @return	0 on success and errno on failure
 */
int mtd_erase(const char *mtd);

/*
 * Flash a file to an MTD device
 * @param       path    file to write or a URL
 * @param       mtd     path to or partition name of MTD device.
 * @return      0 on success and errno on failure
 */
int mtd_flash(const char *path, const char *mtd);

/*
 * Write a file or a URL to an MTD device
 * @param	path	file to write or a URL
 * @param	mtd	path to or partition name of MTD device 
 * @return	0 on success and errno on failure
 */
int mtd_write(const char *path, const char *mtd);

#endif /* _mtd_h_ */
