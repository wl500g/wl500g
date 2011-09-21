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

UTILS := wl nas eapd mii-tool robocfg pwrled hub-ctrl
CFLAGS += -s -Os -I$(SRCBASE)/include -I$(TOP)/shared -isystem$(LINUXDIR)/include -Wall -Wl,-rpath /lib $(EXTRACFLAGS)

vpath %.c $(SRCBASE)/tools/misc

ifneq ($(wildcard $(SRCBASE)/et/linux/et.c),)
UTILS += et
vpath %.c $(SRCBASE)/et/linux
endif


all: $(UTILS)

ifneq ($(wildcard $(SRCBASE)/wl/mipsel-uclibc/wl_exe.o),)
wl: $(SRCBASE)/wl/mipsel-uclibc/wl_exe.o
	$(CC) $(CFLAGS) -o $@ $<
else
wl: $(SRCBASE)/wl/mipsel-uclibc/wl
	cp -p $< $@
endif

ifneq ($(wildcard $(SRCBASE)/wl/mipsel-uclibc/nas_exe.o),)
nas: $(SRCBASE)/wl/mipsel-uclibc/nas_exe.o
	$(CC) $(CFLAGS) -o $@ -L$(TOP)/shared -lshared -L$(TOP)/nvram -lnvram -L$(TOP)/libbcmcrypto -lbcmcrypto $<
else
nas: $(SRCBASE)/wl/mipsel-uclibc/nas
	cp -p $< $@
endif

ifneq ($(wildcard $(SRCBASE)/wl/mipsel-uclibc/eapd_exe.o),)
eapd: $(SRCBASE)/wl/mipsel-uclibc/eapd_exe.o
	$(CC) $(CFLAGS) -o $@ -L$(TOP)/shared -lshared -L$(TOP)/nvram -lnvram $<
else
eapd: $(SRCBASE)/wl/mipsel-uclibc/eapd
	cp -p $< $@
endif

mii-tool: mii-tool.c mii.h

robocfg: robocfg.c etc53xx.h

pwrled: pwrled.c
	$(CC) $(CFLAGS) -DHAVE_GETMODEL -o $@ -L$(TOP)/shared -lshared -L$(TOP)/nvram -lnvram $<

hub-ctrl: hub-ctrl.c
	$(CC) $(CFLAGS) -o $@ -I$(TOP)/libusb10/libusb -I$(TOP)/libusb10/ -L$(TOP)/nvram -lnvram -L$(TOP)/libusb10/libusb/.libs -lusb-1.0 $<

clean:
	rm -f *.o *~ $(UTILS)

install: all
	install -d $(INSTALLDIR)/usr/sbin
	install $(UTILS) $(INSTALLDIR)/usr/sbin
	$(STRIP) $(foreach file,$(UTILS),$(INSTALLDIR)/usr/sbin/$(file))
	ln -sf nas $(INSTALLDIR)/usr/sbin/nas4not
