# 
# Copyright (C) 2006 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#
include $(TOPDIR)/rules.mk

KERNEL_BUILD_DIR := $(BUILD_DIR_TOOLCHAIN)
BUILD_DIR := $(KERNEL_BUILD_DIR)

STAGING_DIR_HOST:=$(TOOLCHAIN_DIR)
BUILD_DIR_HOST:=$(BUILD_DIR_TOOLCHAIN)
override QUILT:=

include $(INCLUDE_DIR)/kernel-version.mk
include $(INCLUDE_DIR)/kernel.mk

PKG_NAME:=linux
PKG_VERSION:=$(LINUX_VERSION)
PKG_SOURCE:=$(LINUX_SOURCE)
ifeq ($(strip $(CONFIG_EXTERNAL_KERNEL_TREE)),"")
PKG_SOURCE_URL:=$(LINUX_SITE)
else
PKG_SOURCE_URL:=
endif
PKG_BUILD_DIR := $(KERNEL_BUILD_DIR)/linux-$(LINUX_VERSION)
PKG_MD5SUM:=$(LINUX_KERNEL_MD5SUM)
LINUX_DIR := $(PKG_BUILD_DIR)

include $(INCLUDE_DIR)/host-build.mk
include $(INCLUDE_DIR)/kernel-defaults.mk

KMAKE := $(MAKE) -C $(PKG_BUILD_DIR) \
	HOSTCFLAGS="$(HOST_CFLAGS) -Wall -Wmissing-prototypes -Wstrict-prototypes" \
	ARCH=$(LINUX_KARCH) \
	KBUILD_HAVE_NLS=no \
	CONFIG_SHELL=$(BASH)

define Build/Prepare
	$(call Kernel/Prepare/Default)
	mkdir -p $(LINUX_HEADERS_DIR)
	yes '' | $(KMAKE) oldconfig
ifeq ($(KERNEL),2.4)
	$(KMAKE) include/linux/version.h include/asm
	-mkdir $(LINUX_HEADERS_DIR)/include
	cp -pLR $(BUILD_DIR_TOOLCHAIN)/linux-$(LINUX_VERSION)/include/asm-$(LINUX_KARCH) $(LINUX_HEADERS_DIR)/include/
	cp -pLR $(BUILD_DIR_TOOLCHAIN)/linux-$(LINUX_VERSION)/include/asm-generic $(LINUX_HEADERS_DIR)/include/
	cp -pLR $(BUILD_DIR_TOOLCHAIN)/linux-$(LINUX_VERSION)/include/linux $(LINUX_HEADERS_DIR)/include/
	ln -sf  asm-$(LINUX_KARCH) $(LINUX_HEADERS_DIR)/include/asm
else
	$(KMAKE) include/linux/utsrelease.h
	$(KMAKE) INSTALL_HDR_PATH="$(LINUX_HEADERS_DIR)" headers_install
	find $(LINUX_HEADERS_DIR)/include \( -name .install -o -name ..install.cmd \) -delete
endif
endef

define Build/Configure
endef

define Build/Compile
endef

define Build/Install
	cp -pLR $(LINUX_HEADERS_DIR)/* $(TOOLCHAIN_DIR)/
endef

define Build/Clean
	rm -rf $(LINUX_HEADERS_DIR)
endef

$(eval $(call HostBuild))
