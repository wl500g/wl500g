# 
# Copyright (C) 2007 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#
# Main makefile for the host tools
#
curdir:=tools

# subdirectories to descend into
tools-y := pkg-config
tools-$(CONFIG_GCC_VERSION_4_3)$(CONFIG_GCC_VERSION_4_4)$(CONFIG_GCC_VERSION_4_5)$(CONFIG_GCC_VERSION_4_6) += gmp mpfr
tools-$(CONFIG_GCC_VERSION_4_5)$(CONFIG_GCC_VERSION_4_6) += mpc libelf
tools-$(CONFIG_CCACHE) += ccache
#tools-y += sstrip ipkg-utils
$(curdir)/builddirs := $(tools-y)
$(curdir)/builddirs-compile := $($(curdir)/builddirs)
$(curdir)/builddirs-install := $($(curdir)/builddirs-compile)

# builddir dependencies
$(curdir)/mpfr/compile := $(curdir)/gmp/install
$(curdir)/mpc/compile := $(curdir)/mpfr/install $(curdir)/gmp/install

# preparatory work
$(STAGING_DIR)/.prepared: $(TMP_DIR)/.build
	@for dir in $(STAGING_DIR) $(STAGING_DIR_HOST); do ( \
		set -x; \
		mkdir -p "$$dir"; \
		cd "$$dir"; \
		mkdir -p bin lib include stamp; \
	); done
	mkdir -p $(BUILD_DIR_HOST)/stamp $(BUILD_DIR)/stamp
	$(INSTALL_DATA) $(TOPDIR)/tools/include/*.h $(STAGING_DIR_HOST)/include/
	touch $@


# prerequisites for the individual targets
$(curdir)/ := .config prereq
$(curdir)//prepare = $(STAGING_DIR)/.prepared
$(curdir)//compile = $(1)/prepare
$(curdir)//install = $(1)/compile

$(eval $(call stampfile,$(curdir),tools,install))
$(eval $(call subdir,$(curdir)))
