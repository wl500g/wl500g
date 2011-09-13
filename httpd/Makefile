#
# milli_httpd Makefile
#
# Copyright (C) 2001 Broadcom Corporation
#
# $Id: Makefile,v 1.3 2004/01/05 05:39:17 Cheni_Shen Exp $
#

ifneq ($(MAKECMDGOALS),clean)
include $(TOP)/.config
endif

CFLAGS	+= -I. -I$(TOP)/shared -I$(SRCBASE)/include -Wall -DFLASH_EMULATOR -DLinux -DMIPS
#CFLAGS	+= -g -DDEBUG
CFLAGS	+= -g -O2 $(EXTRACFLAGS)
LDFLAGS	+= -s -L$(TOP)/nvram -L$(TOP)/shared -L$(TOP)/netconf

OBJS = httpd.o ej.o cgi.o web_ex.o common.o nvram_f.o nvmap.o broadcom.o ralink.o prninfo.o

OBJS += json.o
CFLAGS += -DUSE_JSON

#vpath %.c $(TOP)/shared

all: httpd

clean:
	rm -f *.o *~ httpd

install: all
	install -D httpd $(INSTALLDIR)/usr/sbin/httpd
	$(STRIP) $(INSTALLDIR)/usr/sbin/httpd

common.o: common.c variables.c

httpd: $(OBJS)
	$(CC) -o $@ $^ $(LDFLAGS) -lnvram -lshared -lnetconf
