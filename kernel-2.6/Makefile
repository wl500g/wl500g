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

export KERNEL_DIR := $(ROOT)/linux/linux-2.6

define patches_list
    $(shell ls -1 $(1)/[0-9][0-9][0-9]-*.patch 2>/dev/null)
endef

MIPS_Kernel_Patches:=$(call patches_list,linux-mips)
OPENWRT_Kernel_Patches:=$(call patches_list,openwrt)
OUR_Kernel_Patches:=$(call patches_list,.)

patch:
	@echo Preparing kernel ...
	@$(PATCHER) -c -Z $(KERNEL_DIR) buildhost.patch
	$(MAKE) -C $(KERNEL_DIR) mrproper
	@echo Patching kernel...
	@$(PATCHER) -c -Z $(KERNEL_DIR) $(MIPS_Kernel_Patches)
#	@$(PATCHER) -c -Z $(KERNEL_DIR) $(OPENWRT_Kernel_Patches)
	@$(PATCHER) -c -Z $(KERNEL_DIR) $(OUR_Kernel_Patches)

extra-drivers:
	@for drv in drivers/* ; do \
	    if [ -d $$drv ]; then \
		tar -C $$drv $(TAR_EXCL_SVN) -cf - . | tar -C $(KERNEL_DIR) -xf - ; \
		echo " DRV `basename $$drv`"; \
	    fi; \
	done;

config:
	cp kernel.config $(KERNEL_DIR)/arch/mips/defconfig-bcm947xx

version:
	$(MAKE) -C $(KERNEL_DIR) include/linux/version.h

%:
	$(MAKE) -C $(KERNEL_DIR) $*

.PHONY: patch extra-drivers config version
