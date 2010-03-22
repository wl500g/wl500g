###########################################################
#
# vsftpd+
#
###########################################################

# You must replace "vsftpd" and "VSFTPD+" with the lower case name and
# upper case name of your new package.  Some places below will say
# "Do not change this" - that does not include this global change,
# which must always be done to ensure we have unique names.

#
# VSFTPD+_VERSION, VSFTPD+_SITE and VSFTPD+_SOURCE define
# the upstream location of the source code for the package.
# VSFTPD+_DIR is the directory which is created when the source
# archive is unpacked.
# VSFTPD+_UNZIP is the command used to unzip the source.
# It is usually "zcat" (for .gz) or "bzcat" (for .bz2)
#
# You should change all these variables to suit your package.
#
VSFTPD+_SITE=http://vsftpd.devnet.ru/files/2.1.2/ext.1
VSFTPD+_VERSION=2.1.2-ext1
VSFTPD+_SOURCE=vsftpd-$(VSFTPD+_VERSION).tgz
VSFTPD+_DIR=$(shell (echo vsftpd-$(VSFTPD+_VERSION) | awk -Fext '{print $$1 "ext." $$2}'))
VSFTPD+_UNZIP=zcat
VSFTPD+_MAINTAINER=NSLU2 Linux <nslu2-linux@yahoogroups.com>
VSFTPD+_DESCRIPTION=ftp daemon with an emphasis on speed and security with extensions including charset conversion
VSFTPD+_SECTION=net
VSFTPD+_PRIORITY=optional
ifneq (0.9.7,$(OPENSSL_LIB_VERSION))
VSFTPD+_DEPENDS=openssl
endif
VSFTPD+_SUGGESTS=
VSFTPD+_CONFLICTS=

#
# VSFTPD+_IPK_VERSION should be incremented when the ipk changes.
#
VSFTPD+_IPK_VERSION=1


# VSFTPD+_CONFFILES should be a list of user-editable files
VSFTPD+_CONFFILES=/opt/etc/vsftpd.conf

#
# VSFTPD+_PATCHES should list any patches, in the the order in
# which they should be applied to the source code.
#
#VSFTPD+_PATCHES=

#
# If the compilation of the package requires additional
# compilation or linking flags, then list them here.
#
VSFTPD+_CPPFLAGS=
VSFTPD+_LDFLAGS=-lcrypt

ifneq (0.9.7,$(OPENSSL_LIB_VERSION))
VSFTPD+_CPPFLAGS+=-I$(STAGING_INCLUDE_DIR)/openssl
VSFTPD+_LDFLAGS+=-lssl
endif

#
# VSFTPD+_BUILD_DIR is the directory in which the build is done.
# VSFTPD+_SOURCE_DIR is the directory which holds all the
# patches and ipkg control files.
# VSFTPD+_IPK_DIR is the directory in which the ipk is built.
# VSFTPD+_IPK is the name of the resulting ipk files.
#
# You should not change any of these variables.
#
VSFTPD+_BUILD_DIR=$(BUILD_DIR)/vsftpd+
VSFTPD+_SOURCE_DIR=$(SOURCE_DIR)/vsftpd+
VSFTPD+_IPK_DIR=$(BUILD_DIR)/vsftpd+-$(VSFTPD+_VERSION)-ipk
VSFTPD+_IPK=$(BUILD_DIR)/vsftpd+_$(VSFTPD+_VERSION)-$(VSFTPD+_IPK_VERSION)_$(TARGET_ARCH).ipk

.PHONY: vsftpd+-source vsftpd+-unpack vsftpd+ vsftpd+-stage vsftpd+-ipk vsftpd+-clean vsftpd+-dirclean vsftpd+-check

#
# This is the dependency on the source code.  If the source is missing,
# then it will be fetched from the site using wget.
#
$(DL_DIR)/$(VSFTPD+_SOURCE):
	$(WGET) -P $(@D) $(VSFTPD+_SITE)/$(@F) || \
	$(WGET) -P $(@D) $(SOURCES_NLO_SITE)/$(@F)
#
# The source code depends on it existing within the download directory.
# This target will be called by the top level Makefile to download the
# source code's archive (.tar.gz, .bz2, etc.)
#
vsftpd+-source: $(DL_DIR)/$(VSFTPD+_SOURCE) $(VSFTPD+_PATCHES)

#
# This target unpacks the source code in the build directory.
# If the source archive is not .tar.gz or .tar.bz2, then you will need
# to change the commands here.  Patches to the source code are also
# applied in this target as required.
#
# This target also configures the build within the build directory.
# Flags such as LDFLAGS and CPPFLAGS should be passed into configure
# and NOT $(MAKE) below.  Passing it to configure causes configure to
# correctly BUILD the Makefile with the right paths, where passing it
# to Make causes it to override the default search paths of the compiler.
#
# If the compilation of the package requires other packages to be staged
# first, then do that first (e.g. "$(MAKE) <bar>-stage <baz>-stage").
#
$(VSFTPD+_BUILD_DIR)/.configured: $(DL_DIR)/$(VSFTPD+_SOURCE) $(VSFTPD+_PATCHES) make/vsftpd.mk
ifneq (0.9.7,$(OPENSSL_LIB_VERSION))
	$(MAKE) openssl-stage
endif
	rm -rf $(BUILD_DIR)/$(VSFTPD+_DIR) $(@D)
	$(VSFTPD+_UNZIP) $(DL_DIR)/$(VSFTPD+_SOURCE) | tar -C $(BUILD_DIR) -xvf -
	if test -n "$(VSFPD_PATCHES)"; then \
		cat $(VSFTPD+_PATCHES) | patch -d $(BUILD_DIR)/$(VSFTPD+_DIR) -p1; \
	fi
	mv $(BUILD_DIR)/$(VSFTPD+_DIR) $(@D)
	sed -i -e '/VSFTP_DEFAULT_CONFIG/s|/etc/vsftpd/vsftpd.conf|/opt/etc/vsftpd.conf|' $(@D)/defs.h
	sed -i -e 's|/etc/vsftpd|/opt&|' $(@D)/tunables.c
	sed -i -e 's|/var/log|/opt&|' $(@D)/tunables.c
	sed -i -e 's|/usr/share/empty|/opt/var/empty|' $(@D)/tunables.c
	sed -i -e 's|/usr/share/ssl|/opt/share/openssl|' $(@D)/tunables.c
	sed -i -e 's|$$(CC) -c|& $$(CPPFLAGS)|' $(@D)/Makefile
ifneq (0.9.7,$(OPENSSL_LIB_VERSION))
	sed -i -e '/VSF_BUILD_SSL/s|#undef|#define|' $(@D)/builddefs.h
endif
ifeq ($(OPTWARE_TARGET), $(filter slugosbe slugosle slugos5be slugos5le, $(OPTWARE_TARGET)))
	sed -i -e '/pam_start/s/.*/if false; then/' $(@D)/vsf_findlibs.sh
	sed -i -e '/VSF_BUILD_PAM/s/#define/#undef/' $(@D)/builddefs.h
endif
#	(cd $(VSFTPD+_BUILD_DIR); \
		$(TARGET_CONFIGURE_OPTS) \
		CPPFLAGS="$(STAGING_CPPFLAGS) $(VSFTPD+_CPPFLAGS)" \
		LDFLAGS="$(STAGING_LDFLAGS) $(VSFTPD+_LDFLAGS)" \
		./configure \
		--build=$(GNU_HOST_NAME) \
		--host=$(GNU_TARGET_NAME) \
		--target=$(GNU_TARGET_NAME) \
		--prefix=/opt \
	)
	touch $@

vsftpd+-unpack: $(VSFTPD+_BUILD_DIR)/.configured

#
# This builds the actual binary.  You should change the target to refer
# directly to the main binary which is built.
#
$(VSFTPD+_BUILD_DIR)/.built: $(VSFTPD+_BUILD_DIR)/.configured
	rm -f $@
	$(MAKE) -C $(@D) \
		$(TARGET_CONFIGURE_OPTS) \
		CPPFLAGS="$(STAGING_CPPFLAGS) $(VSFTPD+_CPPFLAGS)" \
		LIBS="$(STAGING_LDFLAGS) $(VSFTPD+_LDFLAGS)"
	touch $@

#
# You should change the dependency to refer directly to the main binary
# which is built.
#
vsftpd+: $(VSFTPD+_BUILD_DIR)/.built

#
# If you are building a library, then you need to stage it too.
#
#$(VSFTPD+_BUILD_DIR)/.staged: $(VSFTPD+_BUILD_DIR)/.built
#	install -d $(STAGING_DIR)/opt/include
#	install -m 644 $(VSFTPD+_BUILD_DIR)/vsftpd.h $(STAGING_DIR)/opt/include
#	install -d $(STAGING_DIR)/opt/lib
#	install -m 644 $(VSFTPD+_BUILD_DIR)/libvsftpd.a $(STAGING_DIR)/opt/lib
#	install -m 644 $(VSFTPD+_BUILD_DIR)/libvsftpd.so.$(VSFTPD+_VERSION) $(STAGING_DIR)/opt/lib
#	cd $(STAGING_DIR)/opt/lib && ln -fs libvsftpd.so.$(VSFTPD+_VERSION) libvsftpd.so.1
#	cd $(STAGING_DIR)/opt/lib && ln -fs libvsftpd.so.$(VSFTPD+_VERSION) libvsftpd.so
#
#vsftpd+-stage: $(VSFTPD+_BUILD_DIR)/.staged

$(VSFTPD+_IPK_DIR)/CONTROL/control:
	@install -d $(@D)
	@rm -f $@
	@echo "Package: vsftpd+" >>$@
	@echo "Architecture: $(TARGET_ARCH)" >>$@
	@echo "Priority: $(VSFTPD+_PRIORITY)" >>$@
	@echo "Section: $(VSFTPD+_SECTION)" >>$@
	@echo "Version: $(VSFTPD+_VERSION)-$(VSFTPD+_IPK_VERSION)" >>$@
	@echo "Maintainer: $(VSFTPD+_MAINTAINER)" >>$@
	@echo "Source: $(VSFTPD+_SITE)/$(VSFTPD+_SOURCE)" >>$@
	@echo "Description: $(VSFTPD+_DESCRIPTION)" >>$@
	@echo "Depends: $(VSFTPD+_DEPENDS)" >>$@
	@echo "Suggests: $(VSFTPD+_SUGGESTS)" >>$@
	@echo "Conflicts: $(VSFTPD+_CONFLICTS)" >>$@

#
# This builds the IPK file.
#
# Binaries should be installed into $(VSFTPD+_IPK_DIR)/opt/sbin or $(VSFTPD+_IPK_DIR)/opt/bin
# (use the location in a well-known Linux distro as a guide for choosing sbin or bin).
# Libraries and include files should be installed into $(VSFTPD+_IPK_DIR)/opt/{lib,include}
# Configuration files should be installed in $(VSFTPD+_IPK_DIR)/opt/etc/vsftpd/...
# Documentation files should be installed in $(VSFTPD+_IPK_DIR)/opt/doc/vsftpd/...
# Daemon startup scripts should be installed in $(VSFTPD+_IPK_DIR)/opt/etc/init.d/S??vsftpd
#
# You may need to patch your application to make it use these locations.
#
$(VSFTPD+_IPK): $(VSFTPD+_BUILD_DIR)/.built
	rm -rf $(VSFTPD+_IPK_DIR) $(BUILD_DIR)/vsftpd+_*_$(TARGET_ARCH).ipk
	install -d $(VSFTPD+_IPK_DIR)/opt/sbin
	$(STRIP_COMMAND) $(VSFTPD+_BUILD_DIR)/vsftpd -o $(VSFTPD+_IPK_DIR)/opt/sbin/vsftpd
	install -d $(VSFTPD+_IPK_DIR)/opt/etc
	install -m 644 $(VSFTPD+_SOURCE_DIR)/vsftpd.conf $(VSFTPD+_IPK_DIR)/opt/etc/vsftpd.conf
	install -d $(VSFTPD+_IPK_DIR)/opt/share/man/man5
	install -m 644 $(VSFTPD+_BUILD_DIR)/vsftpd.conf.5 $(VSFTPD+_IPK_DIR)/opt/share/man/man5
	install -d $(VSFTPD+_IPK_DIR)/opt/share/man/man8
	install -m 644 $(VSFTPD+_BUILD_DIR)/vsftpd.8 $(VSFTPD+_IPK_DIR)/opt/share/man/man8
	$(MAKE) $(VSFTPD+_IPK_DIR)/CONTROL/control
	install -m 644 $(VSFTPD+_SOURCE_DIR)/postinst $(VSFTPD+_IPK_DIR)/CONTROL/postinst
	echo $(VSFTPD+_CONFFILES) | sed -e 's/ /\n/g' > $(VSFTPD+_IPK_DIR)/CONTROL/conffiles
	cd $(BUILD_DIR); $(IPKG_BUILD) $(VSFTPD+_IPK_DIR)

#
# This is called from the top level makefile to create the IPK file.
#
vsftpd+-ipk: $(VSFTPD+_IPK)

#
# This is called from the top level makefile to clean all of the built files.
#
vsftpd+-clean:
	-$(MAKE) -C $(VSFTPD+_BUILD_DIR) clean

#
# This is called from the top level makefile to clean all dynamically created
# directories.
#
vsftpd+-dirclean:
	rm -rf $(BUILD_DIR)/$(VSFTPD+_DIR) $(VSFTPD+_BUILD_DIR) $(VSFTPD+_IPK_DIR) $(VSFTPD+_IPK)

#
## Some sanity check for the package.
#
vsftpd+-check: $(VSFTPD+_IPK)
	perl scripts/optware-check-package.pl --target=$(OPTWARE_TARGET) $^
