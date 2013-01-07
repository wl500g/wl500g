/*
 * Required functions exported by the port-specific (os-dependent) driver
 * to common (os-independent) driver code.
 *
 * Copyright (C) 2008, Broadcom Corporation
 * All Rights Reserved.
 * 
 * This is UNPUBLISHED PROPRIETARY SOURCE CODE of Broadcom Corporation;
 * the contents of this file may not be disclosed to third parties, copied
 * or duplicated in any form, in whole or in part, without the prior
 * written permission of Broadcom Corporation.
 * $Id: et_export.h,v 1.13.96.1 2008/07/26 00:30:46 Exp $
 */

#ifndef _et_export_h_
#define _et_export_h_

typedef struct et_info et_info_t;

/* misc callbacks */
void et_init(et_info_t *et, uint options);
void et_reset(et_info_t *et);
void et_link_up(et_info_t *et);
void et_link_down(et_info_t *et);
int et_up(et_info_t *et);
void et_down(et_info_t *et, int reset);
void et_dump(et_info_t *et, struct bcmstrbuf *b);
void et_intrson(et_info_t *et);

/* for BCM5222 dual-phy shared mdio contortion */
et_info_t *et_phyfind(et_info_t *et, uint coreunit);
uint16 et_phyrd(et_info_t *et, uint phyaddr, uint reg);
void et_phywr(et_info_t *et, uint reg, uint phyaddr, uint16 val);

#endif	/* _et_export_h_ */
