###########################################################
#
# fakeidentd
#
###########################################################

# You must replace "fakeidentd" and "FAKEIDENTD" with the lower case name and
# upper case name of your new package.  Some places below will say
# "Do not change this" - that does not include this global change,
# which must always be done to ensure we have unique names.

#
# FAKEIDENTD_VERSION, FAKEIDENTD_SITE and FAKEIDENTD_SOURCE define
# the upstream location of the source code for the package.
# FAKEIDENTD_DIR is the directory which is created when the source
# archive is unpacked.
# FAKEIDENTD_UNZIP is the command used to unzip the source.
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
FAKEIDENTD_SITE=http://www.guru-group.fi/~too/sw/releases/
FAKEIDENTD_VERSION=2.5
FAKEIDENTD_SOURCE=identd.c
FAKEIDENTD_DIR=fakeidentd-$(FAKEIDENTD_VERSION)
FAKEIDENTD_UNZIP=
FAKEIDENTD_MAINTAINER=NSLU2 Linux <nslu2-linux@yahoogroups.com>
FAKEIDENTD_DESCRIPTION=Stanalone 'fake' ident deamon.
FAKEIDENTD_SECTION=net
FAKEIDENTD_PRIORITY=optional
FAKEIDENTD_DEPENDS=
FAKEIDENTD_SUGGESTS=
FAKEIDENTD_CONFLICTS=

#
# FAKEIDENTD_IPK_VERSION should be incremented when the ipk changes.
#
FAKEIDENTD_IPK_VERSION=7

#
# FAKEIDENTD_CONFFILES should be a list of user-editable files
FAKEIDENTD_CONFFILES=/opt/etc/init.d/S55fakeidentd /opt/etc/default/fakeidentd

#
# FAKEIDENTD_PATCHES should list any patches, in the the order in
# which they should be applied to the source code.
#
FAKEIDENTD_PATCHES= \
	$(FAKEIDENTD_SOURCE_DIR)/100_unix.patch \
	$(FAKEIDENTD_SOURCE_DIR)/200_pid.patch \
	$(FAKEIDENTD_SOURCE_DIR)/300_target.patch \
	$(FAKEIDENTD_SOURCE_DIR)/400_ipv6.patch

#
# If the compilation of the package requires additional
# compilation or linking flags, then list them here.
#
FAKEIDENTD_CPPFLAGS=
FAKEIDENTD_LDFLAGS=

#
# FAKEIDENTD_BUILD_DIR is the directory in which the build is done.
# FAKEIDENTD_SOURCE_DIR is the directory which holds all the
# patches and ipkg control files.
# FAKEIDENTD_IPK_DIR is the directory in which the ipk is built.
# FAKEIDENTD_IPK is the name of the resulting ipk files.
#
# You should not change any of these variables.
#
FAKEIDENTD_BUILD_DIR=$(BUILD_DIR)/fakeidentd
FAKEIDENTD_SOURCE_DIR=$(SOURCE_DIR)/fakeidentd
FAKEIDENTD_IPK_DIR=$(BUILD_DIR)/fakeidentd-$(FAKEIDENTD_VERSION)-ipk
FAKEIDENTD_IPK=$(BUILD_DIR)/fakeidentd_$(FAKEIDENTD_VERSION)-$(FAKEIDENTD_IPK_VERSION)_$(TARGET_ARCH).ipk

.PHONY: fakeidentd-source fakeidentd-unpack fakeidentd fakeidentd-stage fakeidentd-ipk fakeidentd-clean fakeidentd-dirclean fakeidentd-check

#
# This is the dependency on the source code.  If the source is missing,
# then it will be fetched from the site using wget.
#
$(DL_DIR)/$(FAKEIDENTD_SOURCE):
	$(WGET) -P $(@D) $(FAKEIDENTD_SITE)/$(@F)

#
# The source code depends on it existing within the download directory.
# This target will be called by the top level Makefile to download the
# source code's archive (.tar.gz, .bz2, etc.)
#
fakeidentd-source: $(DL_DIR)/$(FAKEIDENTD_SOURCE) $(FAKEIDENTD_PATCHES)

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
$(FAKEIDENTD_BUILD_DIR)/.configured: $(DL_DIR)/$(FAKEIDENTD_SOURCE) $(FAKEIDENTD_PATCHES) make/fakeidentd.mk
	rm -rf $(BUILD_DIR)/$(FAKEIDENTD_DIR) $(@D)
	mkdir -p $(BUILD_DIR)/$(FAKEIDENTD_DIR)
	cp $(DL_DIR)/$(FAKEIDENTD_SOURCE) $(BUILD_DIR)/$(FAKEIDENTD_DIR)/
	if test -n "$(FAKEIDENTD_PATCHES)" ; \
		then cat $(FAKEIDENTD_PATCHES) | \
		patch -d $(BUILD_DIR)/$(FAKEIDENTD_DIR) -p0 ; \
	fi
	if test "$(BUILD_DIR)/$(FAKEIDENTD_DIR)" != "$(@D)" ; \
		then mv $(BUILD_DIR)/$(FAKEIDENTD_DIR) $(@D) ; \
	fi
	#$(PATCH_LIBTOOL) $(@D)/libtool
	touch $@

fakeidentd-unpack: $(FAKEIDENTD_BUILD_DIR)/.configured

#
# This builds the actual binary.
#
$(FAKEIDENTD_BUILD_DIR)/.built: $(FAKEIDENTD_BUILD_DIR)/.configured
	rm -f $@
	TRG=$(@D)/fakeidentd DATE=`date -uR` CC=$(TARGET_CC) \
	    $(SHELL) $(@D)/$(FAKEIDENTD_SOURCE)
	touch $@

#
# This is the build convenience target.
#
fakeidentd: $(FAKEIDENTD_BUILD_DIR)/.built

#
# If you are building a library, then you need to stage it too.
#
$(FAKEIDENTD_BUILD_DIR)/.staged: $(FAKEIDENTD_BUILD_DIR)/.built
	rm -f $@
#	$(MAKE) -C $(@D) DESTDIR=$(STAGING_DIR) install
	touch $@

fakeidentd-stage: $(FAKEIDENTD_BUILD_DIR)/.staged

#
# This rule creates a control file for ipkg.  It is no longer
# necessary to create a seperate control file under sources/fakeidentd
#
$(FAKEIDENTD_IPK_DIR)/CONTROL/control:
	@install -d $(@D)
	@rm -f $@
	@echo "Package: fakeidentd" >>$@
	@echo "Architecture: $(TARGET_ARCH)" >>$@
	@echo "Priority: $(FAKEIDENTD_PRIORITY)" >>$@
	@echo "Section: $(FAKEIDENTD_SECTION)" >>$@
	@echo "Version: $(FAKEIDENTD_VERSION)-$(FAKEIDENTD_IPK_VERSION)" >>$@
	@echo "Maintainer: $(FAKEIDENTD_MAINTAINER)" >>$@
	@echo "Source: $(FAKEIDENTD_SITE)/$(FAKEIDENTD_SOURCE)" >>$@
	@echo "Description: $(FAKEIDENTD_DESCRIPTION)" >>$@
	@echo "Depends: $(FAKEIDENTD_DEPENDS)" >>$@
	@echo "Suggests: $(FAKEIDENTD_SUGGESTS)" >>$@
	@echo "Conflicts: $(FAKEIDENTD_CONFLICTS)" >>$@

#
# This builds the IPK file.
#
# Binaries should be installed into $(FAKEIDENTD_IPK_DIR)/opt/sbin or $(FAKEIDENTD_IPK_DIR)/opt/bin
# (use the location in a well-known Linux distro as a guide for choosing sbin or bin).
# Libraries and include files should be installed into $(FAKEIDENTD_IPK_DIR)/opt/{lib,include}
# Configuration files should be installed in $(FAKEIDENTD_IPK_DIR)/opt/etc/fakeidentd/...
# Documentation files should be installed in $(FAKEIDENTD_IPK_DIR)/opt/doc/fakeidentd/...
# Daemon startup scripts should be installed in $(FAKEIDENTD_IPK_DIR)/opt/etc/init.d/S??fakeidentd
#
# You may need to patch your application to make it use these locations.
#
$(FAKEIDENTD_IPK): $(FAKEIDENTD_BUILD_DIR)/.built
	rm -rf $(FAKEIDENTD_IPK_DIR) $(BUILD_DIR)/fakeidentd_*_$(TARGET_ARCH).ipk
	install -d $(FAKEIDENTD_IPK_DIR)/opt/bin
	install -m 755 $(FAKEIDENTD_BUILD_DIR)/fakeidentd $(FAKEIDENTD_IPK_DIR)/opt/bin/fakeidentd
	$(STRIP_COMMAND) $(FAKEIDENTD_IPK_DIR)/opt/bin/*
	install -d $(FAKEIDENTD_IPK_DIR)/opt/etc/default
	install -m 644 $(FAKEIDENTD_SOURCE_DIR)/default.fakeidentd $(FAKEIDENTD_IPK_DIR)/opt/etc/default/fakeidentd
	install -d $(FAKEIDENTD_IPK_DIR)/opt/etc/init.d
	install -m 755 $(FAKEIDENTD_SOURCE_DIR)/rc.fakeidentd $(FAKEIDENTD_IPK_DIR)/opt/etc/init.d/S55fakeidentd
	$(MAKE) $(FAKEIDENTD_IPK_DIR)/CONTROL/control
	echo $(FAKEIDENTD_CONFFILES) | sed -e 's/ /\n/g' > $(FAKEIDENTD_IPK_DIR)/CONTROL/conffiles
	cd $(BUILD_DIR); $(IPKG_BUILD) $(FAKEIDENTD_IPK_DIR)

#
# This is called from the top level makefile to create the IPK file.
#
fakeidentd-ipk: $(FAKEIDENTD_IPK)

#
# This is called from the top level makefile to clean all of the built files.
#
fakeidentd-clean:
	rm -f $(FAKEIDENTD_BUILD_DIR)/.built
	TRG=$(FAKEIDENTD_BUILD_DIR)/fakeidentd DATE=`date -uR` CC=$(TARGET_CC) \
	    $(SHELL) $(FAKEIDENTD_BUILD_DIR)/$(FAKEIDENTD_SOURCE) clean

#
# This is called from the top level makefile to clean all dynamically created
# directories.
#
fakeidentd-dirclean:
	rm -rf $(BUILD_DIR)/$(FAKEIDENTD_DIR) $(FAKEIDENTD_BUILD_DIR) $(FAKEIDENTD_IPK_DIR) $(FAKEIDENTD_IPK)
#
#
# Some sanity check for the package.
#
fakeidentd-check: $(FAKEIDENTD_IPK)
	perl scripts/optware-check-package.pl --target=$(OPTWARE_TARGET) $(FAKEIDENTD_IPK)
