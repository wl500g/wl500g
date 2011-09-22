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

CFLAGS	+= -s -O2 $(GLOBAL_OPTIONS) $(EXTRACFLAGS)
CFLAGS	+= -I. -I$(SRCBASE)/include -I../httpd -Wall
#CFLAGS	+= -g -DDEBUG
#CFLAGS	+= -g -DDEBUG_TO_SYSLOG
CFLAGS	+= -fPIC
LDFLAGS += -L.

OBJS := nvparse.o shutils.o wl.o wl_linux.o linux_timer.o defaults.o utils.o

ifeq ($(CONFIG_BCMWL5),y)
OBJS += wlif_utils.o UdpLib.o
endif

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

nvparse.o: nvparse.c nvparse.h netconf.h
shutils.o: shutils.c shutils.h
wl_linux.o: wl_linux.c wlutils.h
wlif_utils.o: wlif_utils.c wlif_utils.h wlutils.h
