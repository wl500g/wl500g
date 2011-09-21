#
# Linux netconf Makefile
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

CFLAGS	+= -I$(TOP)/shared -I$(TOP)/iptables/include -I$(SRCBASE)/include -isystem$(LINUXDIR)/include -Wall $(EXTRACFLAGS) $(GLOBAL_OPTIONS)
#CFLAGS	+= -g -DDEBUG
CFLAGS	+= -fPIC -s -O2 -DWEBSTRFILTER
LDFLAGS += -L. -L$(TOP)/iptables/libiptc

all: libnetconf.so netconf

install: all
	install -D libnetconf.so $(INSTALLDIR)/usr/lib/libnetconf.so
	$(STRIP) $(INSTALLDIR)/usr/lib/libnetconf.so

clean:
	rm -f *.o *.a *.so netconf

libnetconf.so: netconf_linux.o
	$(LD) -shared -o $@ $^ $(LDFLAGS) -liptc

netconf: main.o libnetconf.so
	$(CC) -o $@ $< $(LDFLAGS) -lnetconf

netconf_linux.o: netconf_linux.c netconf_linux.h $(TOP)/shared/netconf.h
