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

CFLAGS	+= -Wall -O2 $(GLOBAL_OPTIONS) $(EXTRACFLAGS) -DFLASH_EMULATOR -DLinux
CFLAGS	+= -ffunction-sections -fdata-sections
CFLAGS	+= -I. -I$(TOP)/shared -I$(SRCBASE)/include -I$(TOP)/iptables/include -isystem$(LINUXDIR)/include
#CFLAGS	+= -g -DDEBUG -DDEBUG_TO_SYSLOG
LDFLAGS	+= -ffunction-sections -fdata-sections -Wl,--gc-sections
LDFLAGS	+= -L$(TOP)/nvram -lnvram -L$(TOP)/shared -lshared -L$(TOP)/iptables/libiptc/.libs/ -liptc

OBJS = httpd.o ej.o cgi.o web_ex.o common.o nvram_f.o broadcom.o prninfo.o netconf_linux.o

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
netconf_linux.o: netconf_linux.c $(TOP)/shared/netconf.h

httpd: $(OBJS)
	$(CC) $(LDFLAGS) -o $@ $^ $(LIBS)
