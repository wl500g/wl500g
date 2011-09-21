#
#
# Copyright (C) 2011 by wl500g.googlecode.com project
#
# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 2 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
# General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program; if not, write to the Free Software
# Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
#

WL=wl-5.10.56.46
NAS=nas-5.10.56.46
LIBBCMCRYPTO=libbcmcrypto-5.20.41

define patches_list
    $(shell ls -1 $(1)/[0-9][0-9][0-9]-*.patch 2>/dev/null)
endef

wl:
	tar -C $(ROOT) --recursive-unlink -xjf wl/$(WL$).tar.bz2
	tar -C $(ROOT)/wl/mipsel-uclibc -xjf wl/$(NAS).tbz2

brcm_Patches := $(call patches_list,.)

brcm-shared:
	tar -C . $(TAR_EXCL_SVN) -cf - include rts shared emf bcm57xx et | tar -C $(ROOT) --recursive-unlink -xf -
	$(PATCHER) -Z $(ROOT) $(brcm_Patches)

libbcmcrypto: router/libbcmcrypto/$(LIBBCMCRYPTO$).tar.gz
	tar -zxf $^ -C $(TOP)
	$(PATCHER) $(TOP)/libbcmcrypto router/libbcmcrypto/$(LIBBCMCRYPTO).patch

wlconf:
	tar -C router $(TAR_EXCL_SVN) -cf - wlconf | tar -C $(TOP) --recursive-unlink -xf -

.PHONY: wl brcm-shared libbcmcrypto wlconf
