###########################################################
#
# usbip
#
###########################################################

# You must replace "usbip" and "USBIP" with the lower case name and
# upper case name of your new package.  Some places below will say
# "Do not change this" - that does not include this global change,
# which must always be done to ensure we have unique names.

#
# USBIP_VERSION, USBIP_SITE and USBIP_SOURCE define
# the upstream location of the source code for the package.
# USBIP_DIR is the directory which is created when the source
# archive is unpacked.
# USBIP_UNZIP is the command used to unzip the source.
# It is usually "zcat" (for .gz) or "bzcat" (for .bz2)
#
# You should change all these variables to suit your package.
# Please make sure that you add a description, and that you
# list all your packages' dependencies, seperated by commas.
# 
# If you list yourself as MAINTAINER, please give a valid email
# address, and indicate your irc nick if it cannot be easily deduced
# from your name or email address.  If you leave MAINTAINER set to
# "NSLU2 Linux" other developers will feel free to edit.
#
USBIP_SITE=http://$(SOURCEFORGE_MIRROR)/sourceforge/usbip
USBIP_VERSION=0.1.7
USBIP_SOURCE=usbip-$(USBIP_VERSION).tar.gz
USBIP_DIR=usbip-$(USBIP_VERSION)
USBIP_UNZIP=zcat
USBIP_MAINTAINER=http://usbip.sourceforge.net
USBIP_DESCRIPTION=General USB device sharing system over IP network
USBIP_SECTION=utility
USBIP_PRIORITY=optional
USBIP_DEPENDS=glib,sysfsutils
USBIP_SUGGESTS=
USBIP_CONFLICTS=

#
# USBIP_IPK_VERSION should be incremented when the ipk changes.
#
USBIP_IPK_VERSION=1

#
# USBIP_CONFFILES should be a list of user-editable files
USBIP_CONFFILES=/opt/share/usbip/usb.ids

#
# USBIP_PATCHES should list any patches, in the the order in
# which they should be applied to the source code.
#
#USBIP_PATCHES=$(USBIP_SOURCE_DIR)/configure.patch

#
# If the compilation of the package requires additional
# compilation or linking flags, then list them here.
#
USBIP_CPPFLAGS=
USBIP_LDFLAGS=

#
# USBIP_BUILD_DIR is the directory in which the build is done.
# USBIP_SOURCE_DIR is the directory which holds all the
# patches and ipkg control files.
# USBIP_IPK_DIR is the directory in which the ipk is built.
# USBIP_IPK is the name of the resulting ipk files.
#
# You should not change any of these variables.
#
USBIP_BUILD_DIR=$(BUILD_DIR)/usbip
USBIP_SOURCE_DIR=$(SOURCE_DIR)/usbip
USBIP_IPK_DIR=$(BUILD_DIR)/usbip-$(USBIP_VERSION)-ipk
USBIP_IPK=$(BUILD_DIR)/usbip_$(USBIP_VERSION)-$(USBIP_IPK_VERSION)_$(TARGET_ARCH).ipk

.PHONY: usbip-source usbip-unpack usbip usbip-stage usbip-ipk usbip-clean usbip-dirclean usbip-check

#
# This is the dependency on the source code.  If the source is missing,
# then it will be fetched from the site using wget.
#
$(DL_DIR)/$(USBIP_SOURCE):
	$(WGET) -P $(@D) $(USBIP_SITE)/$(@F) || \
	$(WGET) -P $(@D) $(SOURCES_NLO_SITE)/$(@F)

#
# The source code depends on it existing within the download directory.
# This target will be called by the top level Makefile to download the
# source code's archive (.tar.gz, .bz2, etc.)
#
usbip-source: $(DL_DIR)/$(USBIP_SOURCE) $(USBIP_PATCHES)

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
# If the package uses  GNU libtool, you should invoke $(PATCH_LIBTOOL) as
# shown below to make various patches to it.
#
$(USBIP_BUILD_DIR)/.configured: $(DL_DIR)/$(USBIP_SOURCE) $(USBIP_PATCHES) make/usbip.mk
	$(MAKE) glib-stage sysfsutils-stage
	rm -rf $(BUILD_DIR)/$(USBIP_DIR) $(@D)
	$(USBIP_UNZIP) $(DL_DIR)/$(USBIP_SOURCE) | tar -C $(BUILD_DIR) -xvf -
	if test -n "$(USBIP_PATCHES)" ; \
		then cat $(USBIP_PATCHES) | \
		patch -d $(BUILD_DIR)/$(USBIP_DIR) -p0 ; \
	fi
	if test "$(BUILD_DIR)/$(USBIP_DIR)" != "$(@D)" ; \
		then mv $(BUILD_DIR)/$(USBIP_DIR) $(@D) ; \
	fi
	(cd $(@D)/src; ./autogen.sh)
	(cd $(@D)/src; \
		$(TARGET_CONFIGURE_OPTS) \
		CPPFLAGS="$(STAGING_CPPFLAGS) $(USBIP_CPPFLAGS)" \
		LDFLAGS="$(STAGING_LDFLAGS) $(USBIP_LDFLAGS)" \
		ac_cv_func_malloc_0_nonnull=yes ac_cv_func_realloc_0_nonnull=yes \
		./configure \
		--build=$(GNU_HOST_NAME) \
		--host=$(GNU_TARGET_NAME) \
		--target=$(GNU_TARGET_NAME) \
		--prefix=/opt \
		--disable-nls \
		--disable-static \
	)
#	$(PATCH_LIBTOOL) $(@D)/libtool
	touch $@

usbip-unpack: $(USBIP_BUILD_DIR)/.configured

#
# This builds the actual binary.
#
$(USBIP_BUILD_DIR)/.built: $(USBIP_BUILD_DIR)/.configured
	rm -f $@
	$(MAKE) -C $(@D)/src
	touch $@

#
# This is the build convenience target.
#
usbip: $(USBIP_BUILD_DIR)/.built

#
# If you are building a library, then you need to stage it too.
#
$(USBIP_BUILD_DIR)/.staged: $(USBIP_BUILD_DIR)/.built
	rm -f $@
	$(MAKE) -C $(@D)/src DESTDIR=$(STAGING_DIR) install
	touch $@

usbip-stage: $(USBIP_BUILD_DIR)/.staged

#
# This rule creates a control file for ipkg.  It is no longer
# necessary to create a seperate control file under sources/usbip
#
$(USBIP_IPK_DIR)/CONTROL/control:
	@install -d $(@D)
	@rm -f $@
	@echo "Package: usbip" >>$@
	@echo "Architecture: $(TARGET_ARCH)" >>$@
	@echo "Priority: $(USBIP_PRIORITY)" >>$@
	@echo "Section: $(USBIP_SECTION)" >>$@
	@echo "Version: $(USBIP_VERSION)-$(USBIP_IPK_VERSION)" >>$@
	@echo "Maintainer: $(USBIP_MAINTAINER)" >>$@
	@echo "Source: $(USBIP_SITE)/$(USBIP_SOURCE)" >>$@
	@echo "Description: $(USBIP_DESCRIPTION)" >>$@
	@echo "Depends: $(USBIP_DEPENDS)" >>$@
	@echo "Suggests: $(USBIP_SUGGESTS)" >>$@
	@echo "Conflicts: $(USBIP_CONFLICTS)" >>$@

#
# This builds the IPK file.
#
# Binaries should be installed into $(USBIP_IPK_DIR)/opt/sbin or $(USBIP_IPK_DIR)/opt/bin
# (use the location in a well-known Linux distro as a guide for choosing sbin or bin).
# Libraries and include files should be installed into $(USBIP_IPK_DIR)/opt/{lib,include}
# Configuration files should be installed in $(USBIP_IPK_DIR)/opt/etc/usbip/...
# Documentation files should be installed in $(USBIP_IPK_DIR)/opt/doc/usbip/...
# Daemon startup scripts should be installed in $(USBIP_IPK_DIR)/opt/etc/init.d/S??usbip
#
# You may need to patch your application to make it use these locations.
#
$(USBIP_IPK): $(USBIP_BUILD_DIR)/.built
	rm -rf $(USBIP_IPK_DIR) $(BUILD_DIR)/usbip_*_$(TARGET_ARCH).ipk
	$(MAKE) -C $(USBIP_BUILD_DIR)/src DESTDIR=$(USBIP_IPK_DIR) install-strip
#	install -d $(USBIP_IPK_DIR)/opt/etc/
#	install -m 644 $(USBIP_SOURCE_DIR)/usbip.conf $(USBIP_IPK_DIR)/opt/etc/usbip.conf
#	install -d $(USBIP_IPK_DIR)/opt/etc/init.d
#	install -m 755 $(USBIP_SOURCE_DIR)/rc.usbip $(USBIP_IPK_DIR)/opt/etc/init.d/SXXusbip
#	sed -i -e '/^#!/aOPTWARE_TARGET=${OPTWARE_TARGET}' $(USBIP_IPK_DIR)/opt/etc/init.d/SXXusbip
	$(MAKE) $(USBIP_IPK_DIR)/CONTROL/control
#	install -m 755 $(USBIP_SOURCE_DIR)/postinst $(USBIP_IPK_DIR)/CONTROL/postinst
#	sed -i -e '/^#!/aOPTWARE_TARGET=${OPTWARE_TARGET}' $(USBIP_IPK_DIR)/CONTROL/postinst
#	install -m 755 $(USBIP_SOURCE_DIR)/prerm $(USBIP_IPK_DIR)/CONTROL/prerm
#	sed -i -e '/^#!/aOPTWARE_TARGET=${OPTWARE_TARGET}' $(USBIP_IPK_DIR)/CONTROL/prerm
#	if test -n "$(UPD-ALT_PREFIX)"; then \
		sed -i -e '/^[ 	]*update-alternatives /s|update-alternatives|$(UPD-ALT_PREFIX)/bin/&|' \
			$(USBIP_IPK_DIR)/CONTROL/postinst $(USBIP_IPK_DIR)/CONTROL/prerm; \
	fi
	echo $(USBIP_CONFFILES) | sed -e 's/ /\n/g' > $(USBIP_IPK_DIR)/CONTROL/conffiles
	cd $(BUILD_DIR); $(IPKG_BUILD) $(USBIP_IPK_DIR)

#
# This is called from the top level makefile to create the IPK file.
#
usbip-ipk: $(USBIP_IPK)

#
# This is called from the top level makefile to clean all of the built files.
#
usbip-clean:
	rm -f $(USBIP_BUILD_DIR)/.built
	-$(MAKE) -C $(USBIP_BUILD_DIR)/src clean

#
# This is called from the top level makefile to clean all dynamically created
# directories.
#
usbip-dirclean:
	rm -rf $(BUILD_DIR)/$(USBIP_DIR) $(USBIP_BUILD_DIR) $(USBIP_IPK_DIR) $(USBIP_IPK)
#
#
# Some sanity check for the package.
#
usbip-check: $(USBIP_IPK)
	perl scripts/optware-check-package.pl --target=$(OPTWARE_TARGET) $^
