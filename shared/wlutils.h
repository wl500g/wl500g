/*
 * Broadcom wireless network adapter utility functions
 *
 * Copyright 2005, Broadcom Corporation
 * All Rights Reserved.
 * 
 * THIS SOFTWARE IS OFFERED "AS IS", AND BROADCOM GRANTS NO WARRANTIES OF ANY
 * KIND, EXPRESS OR IMPLIED, BY STATUTE, COMMUNICATION OR OTHERWISE. BROADCOM
 * SPECIFICALLY DISCLAIMS ANY IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS
 * FOR A SPECIFIC PURPOSE OR NONINFRINGEMENT CONCERNING THIS SOFTWARE.
 *
 * $Id$
 */

#ifndef _wlutils_h_
#define _wlutils_h_

#include <typedefs.h>
#include <wlioctl.h>

/*
 * Pass a wlioctl request to the specified interface.
 * @param	name	interface name
 * @param	cmd	WLC_GET_MAGIC <= cmd < WLC_LAST
 * @param	buf	buffer for passing in and/or receiving data
 * @param	len	length of buf
 * @return	>= 0 if successful or < 0 otherwise
 */
extern int wl_ioctl(const char *name, int cmd, void *buf, int len);

/*
 * Get device type.
 * @param	name	interface name
 * @param	buf	buffer for passing in and/or receiving data
 * @param	len	length of buf
 * @return	>= 0 if successful or < 0 otherwise
 */
#define DEV_TYPE_LEN 3 /* Length for dev type 'et'/'wl' */
extern int wl_get_dev_type(const char *name, void *buf, int len);

/*
 * Get the MAC (hardware) address of the specified interface.
 * @param	name	interface name
 * @param	hwaddr	6-byte buffer for receiving address
 * @return	>= 0 if successful or < 0 otherwise
 */
extern int wl_hwaddr(const char *name, unsigned char *hwaddr);

/*
 * Probe the specified interface.
 * @param	name	interface name
 * @return	>= 0 if a Broadcom wireless device or < 0 otherwise
 */
extern int wl_probe(const char *name);

/*
 * Set/Get named variable.
 * @param	ifname		interface name
 * @param	iovar		variable name
 * @param	param		input param value/buffer
 * @param	paramlen	input param value/buffer length
 * @param	bufptr		io buffer
 * @param	buflen		io buffer length
 * @param	val		val or val pointer for int routines
 * @return	success == 0, failure != 0
 */
extern int wl_iovar_setbuf(const char *ifname, const char *iovar, const void *param, int paramlen, void *bufptr, int buflen);
extern int wl_iovar_getbuf(const char *ifname, const char *iovar, const void *param, int paramlen, void *bufptr, int buflen);
extern int wl_iovar_set(const char *ifname, const char *iovar, const void *param, int paramlen);
extern int wl_iovar_get(const char *ifname, const char *iovar, void *bufptr, int buflen);
extern int wl_iovar_setint(const char *ifname, const char *iovar, int val);
extern int wl_iovar_getint(const char *ifname, const char *iovar, int *val);

/*
 * Set/Get named variable indexed by BSS Configuration
 * @param	ifname		interface name
 * @param	iovar		variable name
 * @param	bssidx		bsscfg index
 * @param	param		input param value/buffer
 * @param	paramlen	input param value/buffer length
 * @param	bufptr		io buffer
 * @param	buflen		io buffer length
 * @param	val		val or val pointer for int routines
 * @return	success == 0, failure != 0
 */
extern int wl_bssiovar_setbuf(const char *ifname, const char *iovar, int bssidx, const void *param, int paramlen, void *bufptr, int buflen);
extern int wl_bssiovar_getbuf(const char *ifname, const char *iovar, int bssidx, const void *param, int paramlen, void *bufptr, int buflen);
extern int wl_bssiovar_get(const char *ifname, const char *iovar, int bssidx, void *outbuf, int len);
extern int wl_bssiovar_set(const char *ifname, const char *iovar, int bssidx, const void *param, int paramlen);
extern int wl_bssiovar_setint(const char *ifname, const char *iovar, int bssidx, int val);

#endif /* _wlutils_h_ */
