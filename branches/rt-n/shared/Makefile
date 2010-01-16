#
# Linux router shared code Makefile
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

ifneq ($(MAKECMDGOALS),clean)
include $(TOP)/.config
endif

CFLAGS	+= -I. -I$(SRCBASE)/include -I../httpd -Wall $(EXTRACFLAGS)
#CFLAGS	+= -g -DDEBUG
CFLAGS	+= -s -O2
LDFLAGS += -L.

OBJS := nvparse.o shutils.o wl.o wl_linux.o linux_timer.o defaults.o wlif_utils.o UdpLib.o

ifeq ($(CONFIG_SENTRY5),y)
include $(TOP)/shared/Makefiles5.mk
endif

all: libshared.so

defaults.o: defaults.c flash.default

install: all
	install -d $(INSTALLDIR)/usr/lib
	install -m 755 libshared.so $(INSTALLDIR)/usr/lib
	$(STRIP) $(INSTALLDIR)/usr/lib/libshared.so

clean:
	rm -f *.o *.so

distclean: clean
	rm -f bcmconfig.h

libshared.so: $(OBJS)
	$(LD) -shared -o $@ $^
