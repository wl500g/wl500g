#
# Copyright (C) 2010 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=libelf
PKG_VERSION:=0.8.13
PKG_MD5SUM:=4136d7b4c04df68b686570afa26988ac
PKG_RELEASE:=1

PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.gz
PKG_SOURCE_URL:=http://www.mr511.de/software/

include $(INCLUDE_DIR)/host-build.mk

HOST_CONFIGURE_ARGS += \
	--disable-shared \
	--enable-elf64

define Build/Configure
	$(call Build/Configure/Default,\
	    $(HOST_CONFIGURE_ARGS) \
	)
endef

define Build/Compile
	$(MAKE) -C $(PKG_BUILD_DIR)/lib/ libelf.a
endef

define Build/Install
	$(INSTALL_DIR) $(STAGING_DIR_HOST)/{lib,include/libelf}
	$(CP) $(PKG_BUILD_DIR)/lib/{elf_repl.h,gelf.h,libelf.h,nlist.h,sys_elf.h} \
						$(STAGING_DIR_HOST)/include/libelf/
	$(CP) $(PKG_BUILD_DIR)/lib/libelf.a $(STAGING_DIR_HOST)/lib/
endef

define Build/Clean
	rm -rf $(STAGING_DIR_HOST)/include/libelf
	rm -f $(STAGING_DIR_HOST)/lib/libelf.a
	$(call Build/Clean/Default)
endef

$(eval $(call HostBuild))
