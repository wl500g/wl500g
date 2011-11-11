#
# Makefile for the Broadcom EMF Layer
#
# Copyright (C) 2008, Broadcom Corporation
# All Rights Reserved.
# 
# THIS SOFTWARE IS OFFERED "AS IS", AND BROADCOM GRANTS NO WARRANTIES OF ANY
# KIND, EXPRESS OR IMPLIED, BY STATUTE, COMMUNICATION OR OTHERWISE. BROADCOM
# SPECIFICALLY DISCLAIMS ANY IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS
# FOR A SPECIFIC PURPOSE OR NONINFRINGEMENT CONCERNING THIS SOFTWARE.
#
# $Id: Makefile,v 1.3 2008/08/13 20:20:16 Exp $
#

EMFSRCDIR := ../../../../../router/emf/emf
EMFCSRCDIR := ../../../../../emf/emf
EMFDIR := ../../../../../router/emf

EMF_OBJS := $(EMFCSRCDIR)/emfc.o $(EMFSRCDIR)/emf_linux.o

# Search for sources under src/router/emf/emf
ifneq ($(wildcard $(src)/$(EMFSRCDIR)),)
	EXTRA_CFLAGS += -I../../router/emf/emf -I../../router/emf/igs
	EXTRA_CFLAGS += -I../../include/emf/emf -I../../include/emf/igs
	emf-objs := $(EMF_OBJS)
else
	emf-objs := $(EMFDIR)/emf.o
endif

obj-$(CONFIG_EMF) := emf.o

ifneq ($(KERNELVERSION),)
# kbuild part of makefile
else
# Normal makefile
KERNELDIR := ../../../../../linux/linux-2.6
all:
	$(MAKE) -C $(KERNELDIR) M=`pwd`
clean:
	$(MAKE) -C $(KERNELDIR) M=`pwd` clean
endif

clean-files += $(EMFCSRCDIR)/*.o $(EMFSRCDIR)/*.o $(EMFSRCDIR)/.*.o.cmd
