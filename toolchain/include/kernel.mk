# 
# Copyright (C) 2006 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

#ifeq ($(__target_inc),)
#  include $(INCLUDE_DIR)/target.mk
#endif

ifeq ($(DUMP),1)
  KERNEL?=<KERNEL>
  BOARD?=<BOARD>
  LINUX_VERSION?=<LINUX_VERSION>
else
  export GCC_HONOUR_COPTS=s

  ifeq ($(KERNEL),2.6)
    LINUX_KMOD_SUFFIX=ko
  else
    LINUX_KMOD_SUFFIX=o
  endif

  ifneq (,$(findstring uml,$(BOARD)))
    KERNEL_CC?=$(HOSTCC)
    KERNEL_CROSS?=
  else
    KERNEL_CC?=$(TARGET_CC)
    KERNEL_CROSS?=$(TARGET_CROSS)
  endif

  PATCH_DIR ?= ./patches$(shell [ -d "./patches-$(KERNEL_PATCHVER)" ] && printf -- "-$(KERNEL_PATCHVER)" || true )
  FILES_DIR ?= ./files$(shell [ -d "./files-$(KERNEL_PATCHVER)" ] && printf -- "-$(KERNEL_PATCHVER)" || true )
  KERNEL_BUILD_DIR ?= $(BUILD_DIR_BASE)/linux-$(BOARD)$(if $(SUBTARGET),_$(SUBTARGET))
  LINUX_DIR ?= $(KERNEL_BUILD_DIR)/linux-$(LINUX_VERSION)

  MODULES_SUBDIR:=lib/modules/$(LINUX_VERSION)
  TARGET_MODULES_DIR := $(LINUX_TARGET_DIR)/$(MODULES_SUBDIR)

  LINUX_KERNEL:=$(KERNEL_BUILD_DIR)/vmlinux

  LINUX_SOURCE:=linux-$(LINUX_VERSION).tar.bz2
  TESTING:=$(if $(findstring -rc,$(LINUX_VERSION)),/testing,)
  LINUX_SITE:=@KERNEL/linux/kernel/v$(KERNEL)$(TESTING) \

  ifneq ($(TARGET_BUILD),1)
    PKG_BUILD_DIR ?= $(KERNEL_BUILD_DIR)/$(PKG_NAME)$(if $(PKG_VERSION),-$(PKG_VERSION))
  endif
endif

ifneq (,$(findstring uml,$(BOARD)))
  LINUX_KARCH:=um
else
  ifeq (,$(LINUX_KARCH))
    LINUX_KARCH:=$(shell echo $(ARCH) | sed -e 's/i[3-9]86/i386/' \
	  -e 's/mipsel/mips/' \
	  -e 's/mipseb/mips/' \
	  -e 's/sh[234]/sh/' \
	  -e 's/armeb/arm/' \
    )
  endif
endif

ifeq ($(DUMP)$(TARGET_BUILD),)
  -include $(LINUX_DIR)/.config
endif

define CompareKernelPatchVer
  $(shell [ $$(echo $(1) | tr . 0) -$(2) $$(echo $(3) | tr . 0) ] && echo 1 || echo 0)
endef

