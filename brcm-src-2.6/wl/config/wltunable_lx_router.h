/*
 * Broadcom 802.11abgn Networking Device Driver Configuration file
 *
 * Copyright (C) 2009, Broadcom Corporation
 * All Rights Reserved.
 * 
 * THIS SOFTWARE IS OFFERED "AS IS", AND BROADCOM GRANTS NO WARRANTIES OF ANY
 * KIND, EXPRESS OR IMPLIED, BY STATUTE, COMMUNICATION OR OTHERWISE. BROADCOM
 * SPECIFICALLY DISCLAIMS ANY IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS
 * FOR A SPECIFIC PURPOSE OR NONINFRINGEMENT CONCERNING THIS SOFTWARE.
 *
 * $Id: wltunable_lx_router.h,v 1.7.5 2011/10/10 16:01:02 Exp $
 *
 * wl driver tunables
 */

#include <linux/autoconf.h>

// 3.90.38		- 2,4,5
// 4.108.9		- 4,5,11,13
// 4.150.10		- 5,9,11,13,14,15
// 5.10.56		- 4,5,9,11,13,14,15,16,17,19,20,21
// 5.60.127		- 5,7,9,11,12,13,15,16,17,18,21,23,24,25,26,28,29
// 5.100.138	- 4,5,9,11,13,14,15,16,17,19,20,21,22,24,25,26,27,29,30,32,33

//#define D11CONF	 D11 Core Rev
					/* 5 (4306C0), 7 (4712), 9 (4318b0, 5352),
					 * 11 (4321a1), 12 (4321b/c), 13 (5354), 15(4312),
					 * 16 (4322), 17 (4716), 18 (43224a0), 21 (5356),
					 * 23 (43224b0), 24 (4313), 25 (5357a0), 26 (4331a0),
					 * 28 (5357b0)r, 29 (4331b0), 30 (43228).
					 */

//#define D11CONF		0x123bea30	/* 4,5,9,11,13,14,15,16,17,19,20,21,25,28 */

#ifndef CONFIG_CPU_MIPS32_R2
		/* MIPS32r1 */
#define D11CONF		0x0000aa20	/* 5,9,11,13,15 */

#define ACONF		0x000c0	/* 6 (4318), 7 (5352) */
#define GCONF		0x00080	/* 7 (4318, 5352) */
#define NCONF		0x00007 /* 0-2 (4321) */
#define HTCONF		0		/* No HT-Phy */
#define LPCONF		0x00001	/* 0 (5354) */
#define SSLPNCONF	0		/* No SSLPN-Phy */
#define LCNCONF		0		/* No LCN-Phy */

#else	/* MIPS32r2 */

#define D11CONF		0x133b0000	/* 16,17,19,20,21,24,25,28 */

#define ACONF		0		/* No A-Phy */
#define GCONF		0		/* No G-Phy */
#define NCONF		0x20360 /* 5 (4716), 6 (43222a0, 43224a0), 8-9 (5357, 43236a0), 17 (53572a0) */
#define HTCONF		0		/* No HT-Phy */
#define LPCONF		0		/* No LP-Phy */
#define SSLPNCONF	0x0000b /* 0-1 (4329), 3 (5356) */
#define LCNCONF		0		/* No LCN-Phy */

#endif	/* MIPS32r2 */


#define NRXBUFPOST	64	/* # rx buffers posted */
#define RXBND		24	/* max # rx frames to process */
//#define CTFPOOLSZ       64	/* max buffers in ctfpool */

#define WME_PER_AC_TX_PARAMS 1
#define WME_PER_AC_TUNING 1
