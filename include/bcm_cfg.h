/*
 * BCM common config options
 *
 *
 * Copyright (C) 2009, Broadcom Corporation
 * All Rights Reserved.
 * 
 * THIS SOFTWARE IS OFFERED "AS IS", AND BROADCOM GRANTS NO WARRANTIES OF ANY
 * KIND, EXPRESS OR IMPLIED, BY STATUTE, COMMUNICATION OR OTHERWISE. BROADCOM
 * SPECIFICALLY DISCLAIMS ANY IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS
 * FOR A SPECIFIC PURPOSE OR NONINFRINGEMENT CONCERNING THIS SOFTWARE.
 *
 * $Id: bcm_cfg.h,v 1.1.1.1 2010/11/22 09:05:02 Exp $
 */


#if defined(__NetBSD__) || defined(__FreeBSD__)
#if defined(_KERNEL)
#include <opt_bcm.h>
#endif /* defined(_KERNEL) */
#endif /* defined(__NetBSD__) || defined(__FreeBSD__) */
