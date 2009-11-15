###########################################################
#
# cpulimit
#
###########################################################

# You must replace "cpulimit" and "CPULIMIT" with the lower case name and
# upper case name of your new package.  Some places below will say
# "Do not change this" - that does not include this global change,
# which must always be done to ensure we have unique names.

#
# CPULIMIT_VERSION, CPULIMIT_SITE and CPULIMIT_SOURCE define
# the upstream location of the source code for the package.
# CPULIMIT_DIR is the directory which is created when the source
# archive is unpacked.
# CPULIMIT_UNZIP is the command used to unzip the source.
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
CPULIMIT_SITE=http://$(SOURCEFORGE_MIRROR)/sourceforge/cpulimit
CPULIMIT_VERSION=1.1
CPULIMIT_SOURCE=cpulimit-$(CPULIMIT_VERSION).tar.gz
CPULIMIT_DIR=cpulimit-$(CPULIMIT_VERSION)
CPULIMIT_UNZIP=zcat
CPULIMIT_MAINTAINER=NSLU2 Linux <nslu2-linux@yahoogroups.com>
CPULIMIT_DESCRIPTION=Describe cpulimit here.
CPULIMIT_SECTION=system
CPULIMIT_PRIORITY=optional
CPULIMIT_DEPENDS=
CPULIMIT_SUGGESTS=
CPULIMIT_CONFLICTS=

#
# CPULIMIT_IPK_VERSION should be incremented when the ipk changes.
#
CPULIMIT_IPK_VERSION=1

#
# CPULIMIT_CONFFILES should be a list of user-editable files
#CPULIMIT_CONFFILES=/opt/etc/cpulimit.conf /opt/etc/init.d/SXXcpulimit

#
# CPULIMIT_PATCHES should list any patches, in the the order in
# which they should be applied to the source code.
#
CPULIMIT_PATCHES=$(CPULIMIT_SOURCE_DIR)/Makefile.patch

#
# If the compilation of the package requires additional
# compilation or linking flags, then list them here.
#
CPULIMIT_CPPFLAGS=-Wall -O2
CPULIMIT_LDFLAGS=
ifneq ($(LIBC_STYLE), uclibc)
SIMH_LDFLAGS+=-lrt
endif

#
# CPULIMIT_BUILD_DIR is the directory in which the build is done.
# CPULIMIT_SOURCE_DIR is the directory which holds all the
# patches and ipkg control files.
# CPULIMIT_IPK_DIR is the directory in which the ipk is built.
# CPULIMIT_IPK is the name of the resulting ipk files.
#
# You should not change any of these variables.
#
CPULIMIT_BUILD_DIR=$(BUILD_DIR)/cpulimit
CPULIMIT_SOURCE_DIR=$(SOURCE_DIR)/cpulimit
CPULIMIT_IPK_DIR=$(BUILD_DIR)/cpulimit-$(CPULIMIT_VERSION)-ipk
CPULIMIT_IPK=$(BUILD_DIR)/cpulimit_$(CPULIMIT_VERSION)-$(CPULIMIT_IPK_VERSION)_$(TARGET_ARCH).ipk

.PHONY: cpulimit-source cpulimit-unpack cpulimit cpulimit-stage cpulimit-ipk cpulimit-clean cpulimit-dirclean cpulimit-check

#
# This is the dependency on the source code.  If the source is missing,
# then it will be fetched from the site using wget.
#
$(DL_DIR)/$(CPULIMIT_SOURCE):
	$(WGET) -P $(@D) $(CPULIMIT_SITE)/$(@F) || \
	$(WGET) -P $(@D) $(SOURCES_NLO_SITE)/$(@F)

#
# The source code depends on it existing within the download directory.
# This target will be called by the top level Makefile to download the
# source code's archive (.tar.gz, .bz2, etc.)
#
cpulimit-source: $(DL_DIR)/$(CPULIMIT_SOURCE) $(CPULIMIT_PATCHES)

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
$(CPULIMIT_BUILD_DIR)/.configured: $(DL_DIR)/$(CPULIMIT_SOURCE) $(CPULIMIT_PATCHES) make/cpulimit.mk
	rm -rf $(BUILD_DIR)/$(CPULIMIT_DIR) $(@D)
	$(CPULIMIT_UNZIP) $(DL_DIR)/$(CPULIMIT_SOURCE) | tar -C $(BUILD_DIR) -xvf -
	if test -n "$(CPULIMIT_PATCHES)" ; \
		then cat $(CPULIMIT_PATCHES) | \
		patch -d $(BUILD_DIR)/$(CPULIMIT_DIR) -p1 ; \
	fi
	if test "$(BUILD_DIR)/$(CPULIMIT_DIR)" != "$(@D)" ; \
		then mv $(BUILD_DIR)/$(CPULIMIT_DIR) $(@D) ; \
	fi
	touch $@

cpulimit-unpack: $(CPULIMIT_BUILD_DIR)/.configured

#
# This builds the actual binary.
#
$(CPULIMIT_BUILD_DIR)/.built: $(CPULIMIT_BUILD_DIR)/.configured
	rm -f $@
	CPPFLAGS="$(STAGING_CPPFLAGS) $(CPULIMIT_CPPFLAGS)" \
        LDFLAGS="$(STAGING_LDFLAGS) $(CPULIMIT_LDFLAGS)" \
        $(MAKE) -C $(CPULIMIT_BUILD_DIR) $(TARGET_CONFIGURE_OPTS)
	touch $@

#
# This is the build convenience target.
#
cpulimit: $(CPULIMIT_BUILD_DIR)/.built

#
# If you are building a library, then you need to stage it too.
#
$(CPULIMIT_BUILD_DIR)/.staged: $(CPULIMIT_BUILD_DIR)/.built
#	rm -f $@
#	$(MAKE) -C $(@D) DESTDIR=$(STAGING_DIR) install
	touch $@

cpulimit-stage: $(CPULIMIT_BUILD_DIR)/.staged

#
# This rule creates a control file for ipkg.  It is no longer
# necessary to create a seperate control file under sources/cpulimit
#
$(CPULIMIT_IPK_DIR)/CONTROL/control:
	@install -d $(@D)
	@rm -f $@
	@echo "Package: cpulimit" >>$@
	@echo "Architecture: $(TARGET_ARCH)" >>$@
	@echo "Priority: $(CPULIMIT_PRIORITY)" >>$@
	@echo "Section: $(CPULIMIT_SECTION)" >>$@
	@echo "Version: $(CPULIMIT_VERSION)-$(CPULIMIT_IPK_VERSION)" >>$@
	@echo "Maintainer: $(CPULIMIT_MAINTAINER)" >>$@
	@echo "Source: $(CPULIMIT_SITE)/$(CPULIMIT_SOURCE)" >>$@
	@echo "Description: $(CPULIMIT_DESCRIPTION)" >>$@
	@echo "Depends: $(CPULIMIT_DEPENDS)" >>$@
	@echo "Suggests: $(CPULIMIT_SUGGESTS)" >>$@
	@echo "Conflicts: $(CPULIMIT_CONFLICTS)" >>$@

#
# This builds the IPK file.
#
# Binaries should be installed into $(CPULIMIT_IPK_DIR)/opt/sbin or $(CPULIMIT_IPK_DIR)/opt/bin
# (use the location in a well-known Linux distro as a guide for choosing sbin or bin).
# Libraries and include files should be installed into $(CPULIMIT_IPK_DIR)/opt/{lib,include}
# Configuration files should be installed in $(CPULIMIT_IPK_DIR)/opt/etc/cpulimit/...
# Documentation files should be installed in $(CPULIMIT_IPK_DIR)/opt/doc/cpulimit/...
# Daemon startup scripts should be installed in $(CPULIMIT_IPK_DIR)/opt/etc/init.d/S??cpulimit
#
# You may need to patch your application to make it use these locations.
#
$(CPULIMIT_IPK): $(CPULIMIT_BUILD_DIR)/.built
	rm -rf $(CPULIMIT_IPK_DIR) $(BUILD_DIR)/cpulimit_*_$(TARGET_ARCH).ipk
	install -d $(CPULIMIT_IPK_DIR)/opt/sbin/
	install -m 755 $(CPULIMIT_BUILD_DIR)/cpulimit $(CPULIMIT_IPK_DIR)/opt/sbin/cpulimit
	$(STRIP_COMMAND) $(CPULIMIT_IPK_DIR)/opt/sbin/cpulimit
	$(MAKE) $(CPULIMIT_IPK_DIR)/CONTROL/control
	echo $(CPULIMIT_CONFFILES) | sed -e 's/ /\n/g' > $(CPULIMIT_IPK_DIR)/CONTROL/conffiles
	cd $(BUILD_DIR); $(IPKG_BUILD) $(CPULIMIT_IPK_DIR)

#
# This is called from the top level makefile to create the IPK file.
#
cpulimit-ipk: $(CPULIMIT_IPK)

#
# This is called from the top level makefile to clean all of the built files.
#
cpulimit-clean:
	rm -f $(CPULIMIT_BUILD_DIR)/.built
	-$(MAKE) -C $(CPULIMIT_BUILD_DIR) clean

#
# This is called from the top level makefile to clean all dynamically created
# directories.
#
cpulimit-dirclean:
	rm -rf $(BUILD_DIR)/$(CPULIMIT_DIR) $(CPULIMIT_BUILD_DIR) $(CPULIMIT_IPK_DIR) $(CPULIMIT_IPK)
#
#
# Some sanity check for the package.
#
cpulimit-check: $(CPULIMIT_IPK)
	perl scripts/optware-check-package.pl --target=$(OPTWARE_TARGET) $^
