#
# Router rc control scripts Makefile
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
CFLAGS	+= -I. -I$(TOP)/shared -I$(SRCBASE)/include -idirafter$(LINUXDIR)/include -Wall
#CFLAGS	+= -g -DDEBUG
#CFLAGS	+= -g -DDEBUG -DDEBUG_TO_SYSLOG
CFLAGS += -DWEBSTRFILTER
LDFLAGS	+= -L$(TOP)/nvram -lnvram -L$(TOP)/shared -lshared -lcrypt -lpthread -Wl,-rpath /lib

MODEL := $(patsubst -DMODEL_%,%,$(filter -DMODEL_%,$(GLOBAL_OPTIONS)))
STAMP := .stamp-$(MODEL)

OBJS-y := rc.o init.o interface.o network.o services.o udhcpc.o ppp.o \
    crc.o mtd.o common_ex.o network_ex.o firewall.o services_ex.o \
    manu_ex.o http_ex.o watchdog.o sendalarm.o udhcpc_ex.o \
    bigpond.o qos.o auth.o

#CFLAGS += -DBROKEN_IPV6_CONNTRACK

OBJS-$(CONFIG_NETCONF) += firewall_ex.o
OBJS-$(CONFIG_MADWIMAX) += wimax.o
OBJS-$(CONFIG_MODEM) += usbmodem.o
OBJS-$(CONFIG_RCAMD) += rcamd.o


ifeq ($(CONFIG_UPNP),y)
LDFLAGS += -L$(TOP)/netconf -lnetconf
endif

ifeq ($(CONFIG_SENTRY5),y)
include $(TOP)/rc/Makefiles5.mk
endif

vpath %.c $(SRCBASE)/rts/src

all: rc

clean:
	rm -f *.o rc .stamp-*

install: all
	install -d $(INSTALLDIR)/sbin
	install rc $(INSTALLDIR)/sbin
	$(STRIP) $(INSTALLDIR)/sbin/rc
	ln -sf rc $(INSTALLDIR)/sbin/init
	ln -sf rc $(INSTALLDIR)/sbin/erase
	ln -sf rc $(INSTALLDIR)/sbin/write
	ln -sf rc $(INSTALLDIR)/sbin/flash
	ln -sf rc $(INSTALLDIR)/sbin/hotplug
	ln -sf rc $(INSTALLDIR)/sbin/halt
	ln -sf rc $(INSTALLDIR)/sbin/reboot
	ln -sf rc $(INSTALLDIR)/sbin/poweron

	# Added by ASUS
	ln -sf rc $(INSTALLDIR)/sbin/stopservice
	ln -sf rc $(INSTALLDIR)/sbin/ddns_updated
	ln -sf rc $(INSTALLDIR)/sbin/watchdog
	ln -sf rc $(INSTALLDIR)/sbin/sendalarm
	ln -sf rc $(INSTALLDIR)/sbin/rmstorage
	ln -sf rc $(INSTALLDIR)/sbin/wsrom
	ln -sf rc $(INSTALLDIR)/sbin/rsrom
	ln -sf rc $(INSTALLDIR)/sbin/wmac
	ln -sf rc $(INSTALLDIR)/sbin/ntp
ifeq ($(CONFIG_RCAMD),y)
	ln -sf rc $(INSTALLDIR)/sbin/rcamdmain
	ln -sf rc $(INSTALLDIR)/sbin/rmwebcam
endif
	ln -sf rc $(INSTALLDIR)/sbin/bpa_connect
	ln -sf rc $(INSTALLDIR)/sbin/bpa_disconnect
	ln -sf rc $(INSTALLDIR)/sbin/restore
	ln -sf rc $(INSTALLDIR)/sbin/start_ddns
	ln -sf rc $(INSTALLDIR)/sbin/start_ftpd
	ln -sf rc $(INSTALLDIR)/sbin/wlan_update
ifeq ($(CONFIG_MODEM),y)
	ln -sf rc $(INSTALLDIR)/sbin/lsmodem
endif


$(STAMP):
	$(MAKE) clean
	touch $(STAMP)

$(OBJS-y): $(STAMP)

rc: $(OBJS-y)
	$(CC) -o $@ $^ $(LDFLAGS)
