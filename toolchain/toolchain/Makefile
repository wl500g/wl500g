# 
# Copyright (C) 2007 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#
# Main makefile for the toolchain
#
# Steps:
# 1) toolchain/binutils/install
#    build & install binutils
# 2) toolchain/gcc/prepare
#    build & install a minimal gcc, needed for steps 3 & 4
# 3) toolchain/kernel-headers/install
#    install kernel headers, needed for step 4
# 4) toolchain/libc/prepare
#    build & install libc headers & support files, needed for step 5
# 5) toolchain/gcc/compile
#    build & install an initial gcc, needed for step 6
# 6) toolchain/libc/compile
#    build & install the final libc
# 7) toolchain/gcc/install
#    build & install the final gcc
# 8) toolchain/libc/install
#    build & install libc utilities
#
curdir:=toolchain

# subdirectories to descend into
$(curdir)/builddirs := kernel-headers $(if $(CONFIG_GDB),gdb) binutils gcc/minimal gcc/initial gcc/final uClibc
$(curdir)/builddirs-compile:=. $(filter-out kernel-headers,$($(curdir)/builddirs))
$(curdir)/builddirs-install:=$($(curdir)/builddirs-compile)

# builddir dependencies
$(curdir)/uClibc/prepare:=$(curdir)/kernel-headers/install
$(curdir)/gcc/minimal/compile:=$(curdir)/binutils/install
$(curdir)/kernel-headers/install:=$(curdir)/gcc/minimal/install
$(curdir)/gcc/initial/compile:=$(curdir)/uClibc/prepare
$(curdir)/gcc/initial/prepare:=$(curdir)/gcc/minimal/prepare
$(curdir)/gcc/final/prepare:=$(curdir)/gcc/initial/prepare
$(curdir)/uClibc/compile:=$(curdir)/gcc/initial/install
$(curdir)/gcc/final/compile:=$(curdir)/uClibc/compile
$(curdir)/uClibc/install:=$(curdir)/gcc/final/install

$(TOOLCHAIN_DIR)/info.mk: .config
	@for dir in $(TOOLCHAIN_DIR); do ( \
		set -x; \
		mkdir -p "$$dir"; \
		cd "$$dir"; \
		mkdir -p bin lib include stamp; \
	); done
	@grep GCC_VERSION $@ >/dev/null 2>&1 || $(INSTALL_DATA) $(TOPDIR)/toolchain/info.mk $@
	@touch $@

# prerequisites for the individual targets
$(curdir)/ := .config prereq
$(curdir)//prepare = $(STAGING_DIR)/.prepared $(TOOLCHAIN_DIR)/info.mk
$(curdir)//compile = $(1)/prepare
$(curdir)//install = $(1)/compile

$(TOOLCHAIN_DIR)/stamp/.gcc-initial_installed:

$(eval $(call stampfile,$(curdir),toolchain,install,$(TOOLCHAIN_DIR)/stamp/.gcc-initial_installed))
$(eval $(call subdir,$(curdir)))
