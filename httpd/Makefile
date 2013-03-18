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

CFLAGS	+= -Wall -O2 $(GLOBAL_OPTIONS) $(EXTRACFLAGS)
CFLAGS	+= -I. -I$(TOP)/shared -I$(SRCBASE)/include -Wall -DFLASH_EMULATOR -DLinux
#CFLAGS	+= -g -DDEBUG -DDEBUG_TO_SYSLOG
LDFLAGS	+= $(EXTRALDFLAGS)
LDFLAGS	+= -L$(TOP)/nvram -L$(TOP)/shared -L$(TOP)/netconf

OBJS = httpd.o ej.o cgi.o web_ex.o common.o nvram_f.o broadcom.o prninfo.o

ifneq ($(USE_JSON),)
OBJS += json.o web_ex_json.o
CFLAGS += -DUSE_JSON
endif

#vpath %.c $(TOP)/shared

all: httpd

clean:
	rm -f *.o *~ httpd

install: all
	install -D httpd $(INSTALLDIR)/usr/sbin/httpd
	$(STRIP) $(INSTALLDIR)/usr/sbin/httpd

common.o: common.c variables.c common.h
web_ex.o: web_ex.c common.h

httpd: $(OBJS)
	$(CC) -o $@ $^ $(LDFLAGS) -lnvram -lshared -lnetconf
