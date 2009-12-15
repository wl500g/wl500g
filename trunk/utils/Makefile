#
# Miscellaneous utilities Makefile
#
# Copyright 2004, Broadcom Corporation
# All Rights Reserved.
# 
# THIS SOFTWARE IS OFFERED "AS IS", AND BROADCOM GRANTS NO WARRANTIES OF ANY
# KIND, EXPRESS OR IMPLIED, BY STATUTE, COMMUNICATION OR OTHERWISE. BROADCOM
# SPECIFICALLY DISCLAIMS ANY IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS
# FOR A SPECIFIC PURPOSE OR NONINFRINGEMENT CONCERNING THIS SOFTWARE.
#
# $Id$
#

UTILS := wl nas mii-tool robocfg
CFLAGS += -s -O2 -I$(SRCBASE)/include $(EXTRACFLAGS)

vpath %.c $(SRCBASE)/tools/misc

ifneq ($(wildcard $(SRCBASE)/et/linux/et.c),)
UTILS += et
vpath %.c $(SRCBASE)/et/linux
endif


all: $(UTILS)

wl: $(SRCBASE)/wl/mipsel-uclibc/wl_exe.o
	$(CC) $(CCFLAGS) -o $@ $<

nas: $(SRCBASE)/wl/mipsel-uclibc/nas_exe.o
	$(CC) $(CCFLAGS) -o $@ $< -L$(TOP)/shared  -L$(TOP)/nvram -lnvram -lshared

clean:
	rm -f *.o *~ $(UTILS)

install: all
	install -d $(INSTALLDIR)/usr/sbin
	install $(UTILS) $(INSTALLDIR)/usr/sbin
	$(STRIP) $(foreach file,$(UTILS),$(INSTALLDIR)/usr/sbin/$(file))
	ln -sf nas $(INSTALLDIR)/usr/sbin/nas4not
