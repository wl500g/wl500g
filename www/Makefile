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

ifneq ($(MAKECMDGOALS),clean)
include $(TOP)/.config
export CONFIG_IPV6 CONFIG_MADWIMAX CONFIG_MODEM CONFIG_LLTD CONFIG_NTFS3G
endif

all: web_tmp

web_tmp:
	./mkweb tmp

install:
	rm -rf $(INSTALLDIR)/www && install -d $(INSTALLDIR)/www
	install -m 644 web_tmp/*.asp $(INSTALLDIR)/www
	install -m 644 web_tmp/*.inc $(INSTALLDIR)/www
	install -m 644 web_tmp/*.js $(INSTALLDIR)/www
	install -m 644 web_tmp/*.css $(INSTALLDIR)/www
	install -m 644 web_tmp/*.svg $(INSTALLDIR)/www
	install -m 644 web_tmp/*.ico $(INSTALLDIR)/www
	install -d $(INSTALLDIR)/www/graph
	install -m 644 web_tmp/graph/* $(INSTALLDIR)/www/graph

clean:
	rm -rf web_tmp
