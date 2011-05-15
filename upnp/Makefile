#
# Linux upnp Makefile
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

UPNP	:= igd/linux
#DEBUG	:= 1
ifeq ($(DEBUG),1)
CFLAGS += -g -DDEBUG
else
CFLAGS	+= -s -O2
endif
CFLAGS +=  -Dlinux $(EXTRACFLAGS) -Wall -Wunused
#CFLAGS += --trace-includes
export CFLAGS DEBUG

all: $(UPNP)/upnp

install: all
	install -d $(INSTALLDIR)/usr/sbin
	install -m 755 $(UPNP)/upnp $(INSTALLDIR)/usr/sbin
	$(STRIP) $(INSTALLDIR)/usr/sbin/upnp

clean:
	$(MAKE) -C $(UPNP) clean
	rm -f $(UPNP)/upnp

distclean: clean
	$(MAKE) -C $(UPNP) distclean

$(UPNP)/upnp:
	$(MAKE) -C $(UPNP) SRCBASE=$(SRCBASE) TOP=$(TOP)
