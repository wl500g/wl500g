# 
# Copyright (C) 2007 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

ifneq ($(__quilt_inc),1)
__quilt_inc:=1

ifeq ($(TARGET_BUILD),1)
  PKG_BUILD_DIR:=$(LINUX_DIR)
endif
PATCH_DIR?=./patches
FILES_DIR?=./files

define filter_series
sed -e s,\\\#.*,, $(1) | grep -E \[a-zA-Z0-9\]
endef

define PatchDir/Default
	@if [ -d "$(1)" -a "$$$$(ls $(1) | wc -l)" -gt 0 ]; then \
		if [ -s "$(1)/series" ]; then \
			$(call filter_series,$(1)/series) | xargs -n1 \
				$(PATCH) $(PKG_BUILD_DIR) "$(1)"; \
		else \
			$(PATCH) $(PKG_BUILD_DIR) "$(1)"; \
		fi; \
	fi
endef

define PatchDir
$(call PatchDir/Default,$(strip $(1)),$(strip $(2)))
endef

define Build/Patch/Default
	$(call PatchDir,$(PATCH_DIR),)
endef

endif
