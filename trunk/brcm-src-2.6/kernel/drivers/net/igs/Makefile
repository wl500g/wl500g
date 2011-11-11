#
# Makefile for the Broadcom IGS Layer
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

IGSSRCDIR := ../../../../../router/emf/igs
IGSCSRCDIR := ../../../../../emf/igs
EMFDIR := ../../../../../router/emf

IGS_OBJS := $(IGSCSRCDIR)/igsc.o $(IGSCSRCDIR)/igsc_sdb.o $(IGSSRCDIR)/igs_linux.o \
	    $(IGSCSRCDIR)/osl_linux.o

# Search for sources under src/router/emf/igs
ifneq ($(wildcard $(src)/$(IGSSRCDIR)),)
	EXTRA_CFLAGS += -I../../router/emf/igs -I../../router/emf/emf
	EXTRA_CFLAGS += -I../../include/emf/igs -I../../include/emf/emf
	igs-objs := $(IGS_OBJS)
else
	igs-objs := $(EMFDIR)/igs.o
endif

obj-$(CONFIG_EMF) := igs.o

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

clean-files += $(IGSCSRCDIR)/*.o $(IGSSRCDIR)/*.o $(IGSSRCDIR)/.*.o.cmd
