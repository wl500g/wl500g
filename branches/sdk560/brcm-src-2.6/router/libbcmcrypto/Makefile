#
# Linux router libbcmcrypto Makefile
#
# Copyright (C) 2008, Broadcom Corporation
# All Rights Reserved.
# 
# This is UNPUBLISHED PROPRIETARY SOURCE CODE of Broadcom Corporation;
# the contents of this file may not be disclosed to third parties, copied
# or duplicated in any form, in whole or in part, without the prior
# written permission of Broadcom Corporation.
#
# $Id: Makefile,v 1.4 2007/12/13 23:22:58 Exp $
#

include $(TOP)/.config

CFLAGS	+= -fPIC -Wsign-compare -I. -I../httpd -I$(SRCBASE)/include -Wall
#CFLAGS	+= -g -DDEBUG
CFLAGS	+= -s -O2
LDFLAGS += -L.

vpath %.c $(SRCBASE)/bcmcrypto
vpath %.o $(TOP)/libbcmcrypto/prebuilt

OBJS := aes.o aeskeywrap.o rijndael-alg-fst.o dh.o bn.o sha1.o passhash.o prf.o md5.o hmac.o rc4.o random.o
ifeq ($(CONFIG_WSCCMD), y)
OBJS += sha256.o hmac_sha256.o 
endif

# Find all ELF binaries (including extra libraries)
BINARIES := $(shell find $(TARGETDIR) -path $(TARGETDIR)/lib -prune -o -type f -print | file -f - | grep ELF | cut -d':' -f1)
MAP := .map
UNR := .unresolved
SYM := .symbols

all: libbcmcrypto.so

install: all
	rm -rf $(INSTALLDIR)
	install -d $(INSTALLDIR)/usr/lib
	install -m 755 libbcmcrypto.so $(INSTALLDIR)/usr/lib/libbcmcrypto.so
	$(STRIP) $(INSTALLDIR)/usr/lib/libbcmcrypto.so

clean:
	rm -f *.o *.so *.a $(MAP) $(UNR) $(SYM)

libbcmcrypto.so: $(OBJS)
	$(LD) -shared -o $@ $^

libbcmcrypto.a: $(OBJS)
	$(AR) cr $@ $^

# rule for removing unneeded symbols in the shared library
optimize: libbcmcrypto.so libbcmcrypto.a
	$(NM) -o --defined-only --no-sort libbcmcrypto.so | cut -d' ' -f3 > $(MAP)
	$(NM) --dynamic -u --no-sort $(BINARIES) | sort -u > $(UNR)
	rm -rf $(SYM)
	for symbol in `cat $(UNR)` ; do \
	if grep -q "^$${symbol}$$" $(MAP) ; then echo "-u $${symbol}" >> $(SYM) ;  \
	fi ; done
	# if no symbols are needed then delete the shared lib
	if ls $(SYM) ; then \
	xargs -t $(LD) -shared -o lib.so libbcmcrypto.a < $(SYM) ; \
	install lib.so $(TARGETDIR)/usr/lib/libbcmcrypto.so ; \
	$(STRIP) $(TARGETDIR)/usr/lib/libbcmcrypto.so ; \
	else \
	rm $(TARGETDIR)/usr/lib/libbcmcrypto.so ; fi
