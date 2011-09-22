#
# Broadcom web pages
#
# Copyright 2003, Broadcom Corporation
# All Rights Reserved.                
#                                     
# This is UNPUBLISHED PROPRIETARY SOURCE CODE of Broadcom Corporation;   
# the contents of this file may not be disclosed to third parties, copied
# or duplicated in any form, in whole or in part, without the prior      
# written permission of Broadcom Corporation.                            
#
# $Id: Makefile,v 1.10 2003/05/30 01:30:42 mhuang Exp $
#

PAGES := *
PAGES_DIR := graph/*
MODEL := WL500gp
LANGUAGE := EN

ifneq ($(MAKECMDGOALS),clean)
include $(TOP)/.config
export CONFIG_IPV6 CONFIG_MADWIMAX CONFIG_MODEM CONFIG_LLTD CONFIG_NTFS3G
endif

all: pages.mk

pages.mk:
	echo "PAGES := $(PAGES)" > $@

install:
	./mkweb $(MODEL) "$(LANGUAGE)"
	rm -rf $(INSTALLDIR)/www && install -d $(INSTALLDIR)/www
	install -m 644 web_$(MODEL)/*.asp $(INSTALLDIR)/www
	install -m 644 web_$(MODEL)/*.js $(INSTALLDIR)/www
	install -m 644 web_$(MODEL)/*.css $(INSTALLDIR)/www
	install -m 644 web_$(MODEL)/*.svg $(INSTALLDIR)/www
	install -m 644 web_$(MODEL)/*.ico $(INSTALLDIR)/www
	install -d $(INSTALLDIR)/www/graph
	install -m 644 web_$(MODEL)/$(PAGES_DIR) $(INSTALLDIR)/www/graph
	rm -rf web
	rm -rf web_$(MODEL)

install-%:
	./mkweb $* "$(LANGUAGE)"
	rm -rf www_$*	
	install -d www_$*
	install -m 644 web_$*/*.asp www_$*
	install -m 644 web_$*/*.js www_$*
	install -m 644 web_$*/*.css www_$*
	install -m 644 web_$*/*.svg www_$*
	install -m 644 web_$*/*.ico www_$*
	install -d www_$*/graph
	install -m 644 web_$*/$(PAGES_DIR) www_$*/graph

	ln -sf /var/tmp/display.jpg www_$*/display.jpg

	rm -rf web
	rm -rf web_$*

pages-%:
	./mkweb $* "$(LANGUAGE)"
	rm -rf $(INSTALLDIR)/www && install -d $(INSTALLDIR)/www
	install -m 644 web_$*/*.asp $(INSTALLDIR)/www
	install -m 644 web_$*/*.js $(INSTALLDIR)/www
	install -m 644 web_$*/*.css $(INSTALLDIR)/www
	install -m 644 web_$*/*.svg $(INSTALLDIR)/www
	install -m 644 web_$*/*.ico $(INSTALLDIR)/www
	install -d $(INSTALLDIR)/www/graph
	install -m 644 web_$*/$(PAGES_DIR) $(INSTALLDIR)/www/graph

	ln -sf /var/tmp/display.jpg $(INSTALLDIR)/www/display.jpg

	rm -rf web
	rm -rf web_$*

clean:
	rm -f pages.mk
