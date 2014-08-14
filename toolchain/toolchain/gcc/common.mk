# Makefile for to build a gcc/uClibc toolchain
#
# Copyright (C) 2002-2003 Erik Andersen <andersen@uclibc.org>
# Copyright (C) 2004 Manuel Novoa III <mjn3@uclibc.org>
# Copyright (C) 2005-2006 Felix Fietkau <nbd@openwrt.org>
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

include $(TOPDIR)/rules.mk

PKG_NAME:=gcc
PKG_VERSION:=$(call qstrip,$(CONFIG_GCC_VERSION))

ifeq ($(PKG_VERSION),3.4.6)
  PKG_MD5SUM:=4a21ac777d4b5617283ce488b808da7b
endif
ifeq ($(PKG_VERSION),4.4.7)
  PKG_MD5SUM:=295709feb4441b04e87dea3f1bab4281
endif
ifeq ($(PKG_VERSION),4.6.4)
  PKG_MD5SUM:=b407a3d1480c11667f293bfb1f17d1a4
endif
ifeq ($(PKG_VERSION),4.8.3)
  PKG_MD5SUM:=7c60f24fab389f77af203d2516ee110f
endif

PATCH_DIR=../patches/$(PKG_VERSION)
PKG_SOURCE_URL:=ftp://ftp.fu-berlin.de/unix/languages/gcc/releases/gcc-$(PKG_VERSION) \
	http://mirrors.rcn.net/pub/sourceware/gcc/releases/gcc-$(PKG_VERSION) \
	ftp://ftp.gnu.org/gnu/gcc/releases/gcc-$(PKG_VERSION)
PKG_SOURCE:=gcc-$(PKG_VERSION).tar.bz2
STAGING_DIR_HOST:=$(TOOLCHAIN_DIR)
BUILD_DIR_HOST:=$(BUILD_DIR_TOOLCHAIN)

include $(INCLUDE_DIR)/host-build.mk

PKG_SOURCE_DIR:=$(PKG_BUILD_DIR)
GCC_BUILD_DIR:=$(PKG_BUILD_DIR)-$(GCC_VARIANT)
ifneq ($(GCC_VARIANT),minimal)
	PKG_BUILD_DIR:=$(GCC_BUILD_DIR)
endif

STAMP_PREPARED:=$(PKG_BUILD_DIR)/.prepared
STAMP_BUILT:=$(GCC_BUILD_DIR)/.built
STAMP_CONFIGURED:=$(GCC_BUILD_DIR)/.configured
STAMP_INSTALLED:=$(TOOLCHAIN_DIR)/stamp/.gcc_$(GCC_VARIANT)_installed

SEP:=,
TARGET_LANGUAGES:="c$(if $(CONFIG_INSTALL_LIBSTDCPP),$(SEP)c++)"

EXTRA_TARGET=$(if $(CONFIG_EXTRA_TARGET_ARCH),--enable-biarch --enable-targets=$(call qstrip,$(CONFIG_EXTRA_TARGET_ARCH_NAME))-linux-uclibc)

export CONFIG_SITE=NONE
export libgcc_cv_fixed_point=no
export glibcxx_cv_c99_math_tr1=no

GCC_CONFIGURE:= \
	SHELL="$(BASH)" \
	$(PKG_SOURCE_DIR)/configure \
		--prefix=$(TOOLCHAIN_DIR) \
		--build=$(GNU_HOST_NAME) \
		--host=$(GNU_HOST_NAME) \
		--target=$(REAL_GNU_TARGET_NAME) \
		--with-gnu-ld \
		--enable-target-optspace \
		--disable-libgomp \
		--disable-libmudflap \
		--disable-multilib \
		--disable-nls \
		--disable-__cxa_atexit \
		$(SOFT_FLOAT_CONFIG_OPTION) \
		$(if $(CONFIG_arm),--with-arch=armv7-a --with-abi=aapcs-linux) \
		$(call qstrip,$(CONFIG_EXTRA_GCC_CONFIG_OPTIONS)) \

ifneq ($(CONFIG_GCC_VERSION_4_4)$(CONFIG_GCC_VERSION_4_6)$(CONFIG_GCC_VERSION_4_8),)
  GCC_BUILD_TARGET_LIBGCC:=y
  GCC_CONFIGURE+= \
		--with-gmp=$(TOPDIR)/staging_dir/host \
		--with-mpfr=$(TOPDIR)/staging_dir/host \
		--disable-decimal-float
endif

ifneq ($(CONFIG_GCC_VERSION_4_6)$(CONFIG_GCC_VERSION_4_8),)
  GCC_CONFIGURE+= \
		--with-mpc=$(TOPDIR)/staging_dir/host 
endif

ifneq ($(CONFIG_GCC_VERSION_4_4)$(CONFIG_GCC_VERSION_4_6)$(CONFIG_GCC_VERSION_4_8),)
  ifneq ($(CONFIG_mips)$(CONFIG_mipsel),)
    GCC_CONFIGURE += --with-mips-plt
  endif
endif

ifneq ($(CONFIG_SSP_SUPPORT),)
  GCC_CONFIGURE+= \
		--enable-libssp
else
  GCC_CONFIGURE+= \
		--disable-libssp
endif

ifneq ($(CONFIG_EXTRA_TARGET_ARCH),)
  GCC_CONFIGURE+= \
		--enable-biarch \
		--enable-targets=$(call qstrip,$(CONFIG_EXTRA_TARGET_ARCH_NAME))-linux-$(TARGET_SUFFIX)
endif

GCC_MAKE:= \
	export SHELL="$(BASH)"; \
	$(MAKE) \
		CFLAGS_FOR_TARGET="$(TARGET_CFLAGS)" \
		CXXFLAGS_FOR_TARGET="$(TARGET_CFLAGS)"

define Build/SetToolchainInfo
	$(SED) 's,TARGET_CROSS=.*,TARGET_CROSS=$(REAL_GNU_TARGET_NAME)-,' $(TOOLCHAIN_DIR)/info.mk
	$(SED) 's,GCC_VERSION=.*,GCC_VERSION=$(PKG_VERSION),' $(TOOLCHAIN_DIR)/info.mk
	$(SED) 's,REVISION=.*,REVISION=$(TOOLCHAINVERSION),' $(TOOLCHAIN_DIR)/info.mk
endef

define Build/Prepare
	mkdir -p $(GCC_BUILD_DIR)
endef

define Build/Configure
	(cd $(GCC_BUILD_DIR) && rm -f config.cache; \
		$(GCC_CONFIGURE) \
	);
endef

define Build/Clean
	rm -rf \
		$(STAGING_DIR_HOST)/stamp/.gcc_* \
		$(STAGING_DIR_HOST)/stamp/.binutils_* \
		$(GCC_BUILD_DIR) \
		$(TOOLCHAIN_DIR)/$(REAL_GNU_TARGET_NAME) \
		$(TOOLCHAIN_DIR)/bin/$(REAL_GNU_TARGET_NAME)-gc* \
		$(TOOLCHAIN_DIR)/bin/$(REAL_GNU_TARGET_NAME)-c*
endef
