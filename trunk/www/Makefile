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

all: pages.mk

pages.mk:
	echo "PAGES := $(PAGES)" > $@

install:
	./mkweb WL500gx "EN"	
	install -d $(INSTALLDIR)/www
	install -m 644 web_WL500gx/*.asp $(INSTALLDIR)/www
	install -m 644 web_WL500gx/*.js $(INSTALLDIR)/www
	install -m 644 web_WL500gx/*.css $(INSTALLDIR)/www
	install -d $(INSTALLDIR)/www/graph
	install -m 644 web_WL500gx/$(PAGES_DIR) $(INSTALLDIR)/www/graph
	rm -rf web
	rm -rf web_WL500gx

install-%:
	echo $(LANGUAGE)
	./mkweb $* "$(LANGUAGE)"
	rm -rf www_$*	
	install -d www_$*
	install -m 644 web_$*/*.asp www_$*
	install -m 644 web_$*/*.js www_$*
	install -m 644 web_$*/*.css www_$*
	( if [ "$*" != "WL500gl" ] && [ "$*" != "WL500bl" ] && [ "$*" != "WL331gl" ]; then \
		install -m 644 web_$*/*.cab www_$*;\
	fi )
	install -d www_$*/graph
	install -m 644 web_$*/$(PAGES_DIR) www_$*/graph

	ln -sf /tmp/settings www_$*/WL500g.Deluxe.CFG
	ln -sf /tmp/settings www_$*/WL500gl.CFG
	ln -sf /tmp/settings www_$*/WL500bl.CFG
	ln -sf /tmp/settings www_$*/WL500g.CFG
	ln -sf /tmp/settings www_$*/WL500b.CFG
	ln -sf /tmp/settings www_$*/WL500bv2.CFG
	ln -sf /tmp/settings www_$*/WL300g.CFG
	ln -sf /tmp/settings www_$*/WL331g.CFG
	ln -sf /tmp/settings www_$*/WL331gl.CFG
	ln -sf /var/tmp/display.jpg www_$*/display.jpg

	rm -rf web
	rm -rf web_$*

clean:
	rm -f pages.mk
