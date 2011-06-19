GCC_VARIANT:=minimal

include ../common.mk

GCC_CONFIGURE += \
	--with-newlib \
	--without-headers \
	--enable-languages=c \
	--disable-libssp \
	--disable-shared \
	--disable-threads

define Build/Prepare
	$(call Build/SetToolchainInfo)
	$(call Build/Prepare/Default)
	$(CP) $(SCRIPT_DIR)/config.{guess,sub} $(PKG_BUILD_DIR)/
	$(SED) 's,^MULTILIB_OSDIRNAMES,# MULTILIB_OSDIRNAMES,' $(PKG_BUILD_DIR)/gcc/config/*/t-*
	#(cd $(PKG_BUILD_DIR)/libstdc++-v3; autoconf;);
	$(SED) 's,gcc_no_link=yes,gcc_no_link=no,' $(PKG_BUILD_DIR)/libstdc++-v3/configure
	mkdir -p $(GCC_BUILD_DIR)
endef

define Build/Compile
	$(GCC_MAKE) -C $(GCC_BUILD_DIR) all-gcc
endef

define Build/Install
	$(GCC_MAKE) -C $(GCC_BUILD_DIR) install-gcc
endef

define Build/Clean
	rm -rf \
		$(PKG_BUILD_DIR) \
		$(GCC_BUILD_DIR)
endef

$(eval $(call HostBuild))
