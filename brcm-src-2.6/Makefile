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

define patches_list
    $(shell ls -1 $(1)/[0-9][0-9][0-9]-*.patch 2>/dev/null)
endef

wl:
	tar -C . $(TAR_EXCL_SVN) -cf - wl | tar -C $(ROOT) --recursive-unlink -xf -
	-bzip2 -d $(ROOT)/wl/linux/*.bz2

brcm_Patches := $(call patches_list,.)

brcm-shared:
	tar -C . $(TAR_EXCL_SVN) -cf - include shared emf bcm57xx et | tar -C $(ROOT) --recursive-unlink -xf -
	$(PATCHER) -Z $(ROOT) $(brcm_Patches)

brcm-kernel:
	tar -C kernel $(TAR_EXCL_SVN) -cf - arch drivers include sound | tar -C $(KERNEL_DIR) -xf -

libbcmcrypto: router/libbcmcrypto
	tar -C router $(TAR_EXCL_SVN) -cf - $@ | tar -C $(TOP) --recursive-unlink -xf -

wlconf:
	tar -C router $(TAR_EXCL_SVN) -cf - wlconf | tar -C $(TOP) --recursive-unlink -xf -

.PHONY: wl brcm-shared libbcmcrypto wlconf
