/*
 * Minimal debug/trace/assert driver definitions for
 * Broadcom Home Networking Division 10/100 Mbit/s Ethernet
 * Device Driver.
 *
 * Copyright (C) 2009, Broadcom Corporation
 * All Rights Reserved.
 * 
 * This is UNPUBLISHED PROPRIETARY SOURCE CODE of Broadcom Corporation;
 * the contents of this file may not be disclosed to third parties, copied
 * or duplicated in any form, in whole or in part, without the prior
 * written permission of Broadcom Corporation.
 * $Id: et_dbg.h,v 1.23.1 2010/11/22 08:59:20 Exp $
 */

#ifndef _et_dbg_
#define _et_dbg_

#define	ET_ERROR(args)		if (!(et_msg_level & 1)) ; else printf args
#ifdef	BCMDBG
struct ether_header;
extern void etc_prhdr(char *msg, struct ether_header *eh, uint len, int unit);
extern void etc_prhex(char *msg, uchar *buf, uint nbytes, int unit);
/*
 * et_msg_level is a bitvector:
 *	0	errors
 *	1	function-level tracing
 *	2	one-line frame tx/rx summary
 *	3	complex frame tx/rx in hex
 */
#define	ET_TRACE(args)	if (!(et_msg_level & 2)) ; else printf args
#define	ET_PRHDR(msg, eh, len, unit)	if (!(et_msg_level & 4)) ; else etc_prhdr(msg, eh, len, unit)
#define	ET_PRPKT(msg, buf, len, unit)	if (!(et_msg_level & 8)) ; else etc_prhex(msg, buf, len, unit)

/* et_dump_level: dump log levels */
#define ET_DUMP_DMA	0x1	/* Dump DMA stats */
#else	/* BCMDBG */
#define	ET_TRACE(args)
#define	ET_PRHDR(msg, eh, len, unit)
#define	ET_PRPKT(msg, buf, len, unit)
#endif	/* BCMDBG */

extern uint32 et_msg_level;

#define	ET_LOG(fmt, a1, a2)

/* include port-specific tunables */
#ifdef NDIS
#include <et_ndis.h>
#elif defined(__ECOS)
#include <et_ecos.h>
#elif defined(linux)
#include <et_linux.h>
#elif defined(PMON)
#include <et_pmon.h>
#elif defined(_CFE_)
#include <et_cfe.h>
#elif defined(_HNDRTE_)
#include <et_rte.h>
#else
#error
#endif

#endif /* _et_dbg_ */
