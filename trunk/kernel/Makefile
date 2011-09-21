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

export KERNEL_DIR := $(ROOT)/linux/linux

define patches_list
    $(shell ls -1 $(1)/[0-9][0-9][0-9]-*.patch 2>/dev/null)
endef

MIPS_Kernel_Patches:=$(call patches_list,linux-mips)
OPENWRT_Kernel_Patches:=$(call patches_list,openwrt)
OPENWRT_Brcm_Patches:=$(call patches_list,openwrt/brcm)
OUR_Kernel_Patches:=$(call patches_list,.)

patch:
	@echo Preparing kernel ...
	[ -d $(KERNEL_DIR)/arch/mips/bcm947xx ] || tar -C $(KERNEL_DIR) -xvjf brcm-boards.tar.bz2
	$(MAKE) -C $(KERNEL_DIR)/arch/mips/bcm947xx/compressed/ clean
	@$(PATCHER) -Z $(KERNEL_DIR) buildhost.patch
	$(MAKE) -C $(KERNEL_DIR) mrproper
	@echo Patching kernel...
	@$(PATCHER) -Z $(KERNEL_DIR) $(MIPS_Kernel_Patches)
	@$(PATCHER) -Z $(KERNEL_DIR) $(OPENWRT_Kernel_Patches)
	@$(PATCHER) -Z $(KERNEL_DIR) $(OPENWRT_Brcm_Patches)
	@$(PATCHER) -Z $(KERNEL_DIR) $(OUR_Kernel_Patches)

extra-drivers:
	tar -C drivers/ov51x-1.65 $(TAR_EXCL_SVN) -cf - . | tar -C $(KERNEL_DIR)/drivers/usb -xf -
	tar -C drivers/pwc-9.0.2 $(TAR_EXCL_SVN) -cf - . | tar -C $(KERNEL_DIR)/drivers/usb -xf -
	if [ ! -d $(KERNEL_DIR)/fs/fuse ]; then \
	  tar -C $(KERNEL_DIR)/fs -xvjf drivers/fuse-2.5.3.tar.bz2 fuse-2.5.3/kernel/ \
	   && mv $(KERNEL_DIR)/fs/fuse-2.5.3/kernel $(KERNEL_DIR)/fs/fuse && rmdir $(KERNEL_DIR)/fs/fuse-2.5.3; \
	  $(PATCHER) -Z $(KERNEL_DIR)/fs/fuse drivers/fuse-2.5.3.patch; \
	fi

config:
	cp -p kernel.config $(KERNEL_DIR)/arch/mips/defconfig-bcm947xx

version:
	$(MAKE) -C $(KERNEL_DIR) include/linux/version.h

%:
	$(MAKE) -C $(KERNEL_DIR) $*

.PHONY: patch extra-drivers config version
