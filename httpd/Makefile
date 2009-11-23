#
# milli_httpd Makefile
#
# Copyright (C) 2001 Broadcom Corporation
#
# $Id: Makefile,v 1.3 2004/01/05 05:39:17 Cheni_Shen Exp $
#

CFLAGS	+= -I. -I$(TOP)/shared -I$(SRCBASE)/include -Wall -DFLASH_EMULATOR -DLinux -DMIPS
#CFLAGS	+= -g -DDEBUG
CFLAGS	+= -s -O2
LDFLAGS	+= -L$(TOP)/nvram -L$(TOP)/shared -L$(TOP)/netconf

vpath %.c $(TOP)/shared
vpath %.c $(TOP)/shared_common

all: httpd
	$(STRIP) httpd

clean:
	rm -f *.o *~ httpd

install: all
	install -D httpd $(INSTALLDIR)/usr/sbin/httpd
	$(STRIP) $(INSTALLDIR)/usr/sbin/httpd

httpd: httpd.o ej.o cgi.o web_ex.o shutils.o common.o nvram_f.o nvmap.o broadcom.o ralink.o
	$(CC) -o $@ $^ $(LDFLAGS) -lnvram -lshared -lnetconf
