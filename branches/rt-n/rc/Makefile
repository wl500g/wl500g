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

CFLAGS	+= -I. -I$(TOP)/shared -I$(SRCBASE)/include -idirafter$(LINUXDIR)/include -Wall -DASUS_EXT $(GLOBAL_OPTIONS)
#CFLAGS	+= -g -DDEBUG
CFLAGS	+= -s -O2 $(EXTRACFLAGS)
LDFLAGS	+= -L$(TOP)/nvram -lnvram -L$(TOP)/shared -lshared -lcrypt -Wl,-rpath /lib

OBJS := rc.o init.o interface.o network.o services.o udhcpc.o ppp.o \
    crc.o mtd.o common_ex.o network_ex.o firewall_ex.o services_ex.o \
    manu_ex.o http_ex.o watchdog.o sendalarm.o udhcpc_ex.o \
    bigpond.o qos.o rcamd.o waveserver.o

ifeq ($(CONFIG_NETCONF),y)
OBJS += firewall.o
CFLAGS += -DWEBSTRFILTER
LDFLAGS += -L$(TOP)/netconf -lnetconf
endif

ifeq ($(CONFIG_SENTRY5),y)
include $(TOP)/rc/Makefiles5.mk
endif

vpath %.c $(SRCBASE)/rts/src

all: rc

clean:
	rm -f *.o rc

install: all
	install -d $(INSTALLDIR)/sbin
	install rc $(INSTALLDIR)/sbin
	$(STRIP) $(INSTALLDIR)/sbin/rc
	cd $(INSTALLDIR)/sbin && ln -sf rc init
	cd $(INSTALLDIR)/sbin && ln -sf rc erase
	cd $(INSTALLDIR)/sbin && ln -sf rc write
	cd $(INSTALLDIR)/sbin && ln -sf rc flash
	cd $(INSTALLDIR)/sbin && ln -sf rc hotplug
	cd $(INSTALLDIR)/sbin && ln -sf rc halt
	cd $(INSTALLDIR)/sbin && ln -sf rc reboot
	cd $(INSTALLDIR)/sbin && ln -sf rc poweron

	# Added by ASUS
	cd $(INSTALLDIR)/sbin && ln -sf rc stopservice
	cd $(INSTALLDIR)/sbin && ln -sf rc ddns_updated
	cd $(INSTALLDIR)/sbin && ln -sf rc watchdog
	cd $(INSTALLDIR)/sbin && ln -sf rc sendalarm
	cd $(INSTALLDIR)/sbin && ln -sf rc rmwebcam
	cd $(INSTALLDIR)/sbin && ln -sf rc rmstorage
	cd $(INSTALLDIR)/sbin && ln -sf rc wsrom
	cd $(INSTALLDIR)/sbin && ln -sf rc rsrom
	cd $(INSTALLDIR)/sbin && ln -sf rc wmac
	cd $(INSTALLDIR)/sbin && ln -sf rc ntp	
	cd $(INSTALLDIR)/sbin && ln -sf rc rcamdmain
	cd $(INSTALLDIR)/sbin && ln -sf rc waveservermain
	cd $(INSTALLDIR)/sbin && ln -sf rc bpa_connect	
	cd $(INSTALLDIR)/sbin && ln -sf rc bpa_disconnect
	cd $(INSTALLDIR)/sbin && ln -sf rc restore
	cd $(INSTALLDIR)/sbin && ln -sf rc start_ddns
	cd $(INSTALLDIR)/sbin && ln -sf rc start_ftpd
	cd $(INSTALLDIR)/sbin && ln -sf rc wlan_update


rc: $(OBJS)
	$(CC) -o $@ $^ $(LDFLAGS)
	
