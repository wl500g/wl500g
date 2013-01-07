#
# Makefile for Broadcom Home Networking Division (HND) shared driver code
#
# Copyright (C) 2008, Broadcom Corporation
# All Rights Reserved.
# 
# THIS SOFTWARE IS OFFERED "AS IS", AND BROADCOM GRANTS NO WARRANTIES OF ANY
# KIND, EXPRESS OR IMPLIED, BY STATUTE, COMMUNICATION OR OTHERWISE. BROADCOM
# SPECIFICALLY DISCLAIMS ANY IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS
# FOR A SPECIFIC PURPOSE OR NONINFRINGEMENT CONCERNING THIS SOFTWARE.
#
# $Id: Makefile,v 1.5 2008/05/02 22:49:54 Exp $
#

SHARED   := ../../../../../shared
WLSHARED := wlconfig_lx_shared
WLCFGDIR := $(src)/../../../../../wl/config
WLSYSDIR := $(src)/../../../../../wl/sys

ifneq ($(wildcard $(WLCFGDIR)),)
include $(WLCFGDIR)/$(WLSHARED)
include $(WLCFGDIR)/wl.mk
endif

HND_OBJS := $(sort $(WLFILES:.c=.o))

EXTRA_CFLAGS += $(WLFLAGS) -DBCM5354 -DLINUX_HYBRID_POSTPROCESSING_REMOVAL

ifneq ($(CONFIG_BCM947XX),y)
HND_OBJS += nvramstubs.o
endif

ifeq ($(CONFIG_ET_47XX),y)
HND_OBJS += bcmrobo.o
else
ifeq ($(CONFIG_BCM57XX),y)
HND_OBJS += bcmrobo.o
endif
endif

ifdef CONFIG_USB_RNDIS
EXTRA_CFLAGS += -DCONFIG_USBRNDIS_RETAIL
endif

ifeq ($(BCMDBG_MEM),1)
EXTRA_CFLAGS += -DBCMDBG_MEM
endif


ifdef BCMDBG
EXP_OBJS := $(HND_OBJS)
else
EXP_OBJS := $(filter-out aiutils.o sbutils.o nicpci.o ,$(HND_OBJS))
endif

# Search for sources under src/shared/
ifeq ($(wildcard $(src)/$(SHARED)/*.c),)
PREFIX:=$(SHARED)/linux/
else
PREFIX:=$(SHARED)/
endif
EXP_OBJS := $(addprefix $(src)/$(PREFIX), $(EXP_OBJS))
hnd-objs := $(addprefix $(PREFIX), $(HND_OBJS))

obj-$(CONFIG_HND) := hnd.o shared_ksyms.o

clean-files := $(hnd-objs) shared_ksyms.c

quiet_cmd_shared_ksyms = GEN     $@
      cmd_shared_ksyms = sh -e $< $(EXP_OBJS) > $@

$(obj)/shared_ksyms.c: $(src)/shared_ksyms.sh $(EXP_OBJS)
	$(call cmd,shared_ksyms)
