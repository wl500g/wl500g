###########################################################
#
# cutter
#
###########################################################

# You must replace "cutter" and "CUTTER" with the lower case name and
# upper case name of your new package.  Some places below will say
# "Do not change this" - that does not include this global change,
# which must always be done to ensure we have unique names.

#
# CUTTER_VERSION, CUTTER_SITE and CUTTER_SOURCE define
# the upstream location of the source code for the package.
# CUTTER_DIR is the directory which is created when the source
# archive is unpacked.
# CUTTER_UNZIP is the command used to unzip the source.
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
CUTTER_SITE=http://www.lowth.com/cutter/software
CUTTER_VERSION=1.03
CUTTER_SOURCE=cutter-$(CUTTER_VERSION).tgz
CUTTER_DIR=cutter-$(CUTTER_VERSION)
CUTTER_UNZIP=zcat
CUTTER_MAINTAINER=NSLU2 Linux <nslu2-linux@yahoogroups.com>
CUTTER_DESCRIPTION=Tool that allows to abort TCP/IP connections routed over the firewall or router on which it is run
CUTTER_SECTION=net
CUTTER_PRIORITY=optional
CUTTER_DEPENDS=
CUTTER_SUGGESTS=
CUTTER_CONFLICTS=

#
# CUTTER_IPK_VERSION should be incremented when the ipk changes.
#
CUTTER_IPK_VERSION=1

#
# CUTTER_CONFFILES should be a list of user-editable files
#CUTTER_CONFFILES=/opt/etc/cutter.conf /opt/etc/init.d/SXXcutter

#
# CUTTER_PATCHES should list any patches, in the the order in
# which they should be applied to the source code.
#
CUTTER_PATCHES=$(CUTTER_SOURCE_DIR)/Makefile.patch

#
# If the compilation of the package requires additional
# compilation or linking flags, then list them here.
#
CUTTER_CPPFLAGS=
CUTTER_LDFLAGS=

#
# CUTTER_BUILD_DIR is the directory in which the build is done.
# CUTTER_SOURCE_DIR is the directory which holds all the
# patches and ipkg control files.
# CUTTER_IPK_DIR is the directory in which the ipk is built.
# CUTTER_IPK is the name of the resulting ipk files.
#
# You should not change any of these variables.
#
CUTTER_BUILD_DIR=$(BUILD_DIR)/cutter
CUTTER_SOURCE_DIR=$(SOURCE_DIR)/cutter
CUTTER_IPK_DIR=$(BUILD_DIR)/cutter-$(CUTTER_VERSION)-ipk
CUTTER_IPK=$(BUILD_DIR)/cutter_$(CUTTER_VERSION)-$(CUTTER_IPK_VERSION)_$(TARGET_ARCH).ipk

.PHONY: cutter-source cutter-unpack cutter cutter-stage cutter-ipk cutter-clean cutter-dirclean cutter-check

#
# This is the dependency on the source code.  If the source is missing,
# then it will be fetched from the site using wget.
#
$(DL_DIR)/$(CUTTER_SOURCE):
	$(WGET) -P $(@D) $(CUTTER_SITE)/$(@F) || \
	$(WGET) -P $(@D) $(SOURCES_NLO_SITE)/$(@F)

#
# The source code depends on it existing within the download directory.
# This target will be called by the top level Makefile to download the
# source code's archive (.tar.gz, .bz2, etc.)
#
cutter-source: $(DL_DIR)/$(CUTTER_SOURCE) $(CUTTER_PATCHES)

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
$(CUTTER_BUILD_DIR)/.configured: $(DL_DIR)/$(CUTTER_SOURCE) $(CUTTER_PATCHES) make/cutter.mk
	rm -rf $(BUILD_DIR)/$(CUTTER_DIR) $(@D)
	$(CUTTER_UNZIP) $(DL_DIR)/$(CUTTER_SOURCE) | tar -C $(BUILD_DIR) -xvf -
	if test -n "$(CUTTER_PATCHES)" ; \
		then cat $(CUTTER_PATCHES) | \
		patch -d $(BUILD_DIR)/$(CUTTER_DIR) -p0 ; \
	fi
	if test "$(BUILD_DIR)/$(CUTTER_DIR)" != "$(@D)" ; \
		then mv $(BUILD_DIR)/$(CUTTER_DIR) $(@D) ; \
	fi
	touch $@

cutter-unpack: $(CUTTER_BUILD_DIR)/.configured

#
# This builds the actual binary.
#
$(CUTTER_BUILD_DIR)/.built: $(CUTTER_BUILD_DIR)/.configured
	rm -f $@
	CPPFLAGS="$(STAGING_CPPFLAGS) $(CPULIMIT_CPPFLAGS)" \
        LDFLAGS="$(STAGING_LDFLAGS) $(CPULIMIT_LDFLAGS)" \
        $(MAKE) -C $(@D) $(TARGET_CONFIGURE_OPTS)
	touch $@

#
# This is the build convenience target.
#
cutter: $(CUTTER_BUILD_DIR)/.built

#
# If you are building a library, then you need to stage it too.
#
$(CUTTER_BUILD_DIR)/.staged: $(CUTTER_BUILD_DIR)/.built
#	rm -f $@
#	$(MAKE) -C $(@D) DESTDIR=$(STAGING_DIR) install
	touch $@

cutter-stage: $(CUTTER_BUILD_DIR)/.staged

#
# This rule creates a control file for ipkg.  It is no longer
# necessary to create a seperate control file under sources/cutter
#
$(CUTTER_IPK_DIR)/CONTROL/control:
	@install -d $(@D)
	@rm -f $@
	@echo "Package: cutter" >>$@
	@echo "Architecture: $(TARGET_ARCH)" >>$@
	@echo "Priority: $(CUTTER_PRIORITY)" >>$@
	@echo "Section: $(CUTTER_SECTION)" >>$@
	@echo "Version: $(CUTTER_VERSION)-$(CUTTER_IPK_VERSION)" >>$@
	@echo "Maintainer: $(CUTTER_MAINTAINER)" >>$@
	@echo "Source: $(CUTTER_SITE)/$(CUTTER_SOURCE)" >>$@
	@echo "Description: $(CUTTER_DESCRIPTION)" >>$@
	@echo "Depends: $(CUTTER_DEPENDS)" >>$@
	@echo "Suggests: $(CUTTER_SUGGESTS)" >>$@
	@echo "Conflicts: $(CUTTER_CONFLICTS)" >>$@

#
# This builds the IPK file.
#
# Binaries should be installed into $(CUTTER_IPK_DIR)/opt/sbin or $(CUTTER_IPK_DIR)/opt/bin
# (use the location in a well-known Linux distro as a guide for choosing sbin or bin).
# Libraries and include files should be installed into $(CUTTER_IPK_DIR)/opt/{lib,include}
# Configuration files should be installed in $(CUTTER_IPK_DIR)/opt/etc/cutter/...
# Documentation files should be installed in $(CUTTER_IPK_DIR)/opt/doc/cutter/...
# Daemon startup scripts should be installed in $(CUTTER_IPK_DIR)/opt/etc/init.d/S??cutter
#
# You may need to patch your application to make it use these locations.
#
$(CUTTER_IPK): $(CUTTER_BUILD_DIR)/.built

	rm -rf $(CPULIMIT_IPK_DIR) $(BUILD_DIR)/cpulimit_*_$(TARGET_ARCH).ipk
	install -d $(CPULIMIT_IPK_DIR)/opt/sbin/
	install -m 755 $(CPULIMIT_BUILD_DIR)/cpulimit $(CPULIMIT_IPK_DIR)/opt/sbin/cpulimit
	$(STRIP_COMMAND) $(CPULIMIT_IPK_DIR)/opt/sbin/cpulimit
	$(MAKE) $(CPULIMIT_IPK_DIR)/CONTROL/control
	echo $(CPULIMIT_CONFFILES) | sed -e 's/ /\n/g' > $(CPULIMIT_IPK_DIR)/CONTROL/conffiles
	cd $(BUILD_DIR); $(IPKG_BUILD) $(CPULIMIT_IPK_DIR)


	rm -rf $(CUTTER_IPK_DIR) $(BUILD_DIR)/cutter_*_$(TARGET_ARCH).ipk
	install -d $(CUTTER_IPK_DIR)/opt/sbin/
	install -m 755 $(CUTTER_BUILD_DIR)/cutter $(CUTTER_IPK_DIR)/opt/sbin/cutter
	$(STRIP_COMMAND) $(CUTTER_IPK_DIR)/opt/sbin/cutter
	$(MAKE) $(CUTTER_IPK_DIR)/CONTROL/control
	echo $(CUTTER_CONFFILES) | sed -e 's/ /\n/g' > $(CUTTER_IPK_DIR)/CONTROL/conffiles
	cd $(BUILD_DIR); $(IPKG_BUILD) $(CUTTER_IPK_DIR)

#
# This is called from the top level makefile to create the IPK file.
#
cutter-ipk: $(CUTTER_IPK)

#
# This is called from the top level makefile to clean all of the built files.
#
cutter-clean:
	rm -f $(CUTTER_BUILD_DIR)/.built
	-$(MAKE) -C $(CUTTER_BUILD_DIR) clean

#
# This is called from the top level makefile to clean all dynamically created
# directories.
#
cutter-dirclean:
	rm -rf $(BUILD_DIR)/$(CUTTER_DIR) $(CUTTER_BUILD_DIR) $(CUTTER_IPK_DIR) $(CUTTER_IPK)
#
#
# Some sanity check for the package.
#
cutter-check: $(CUTTER_IPK)
	perl scripts/optware-check-package.pl --target=$(OPTWARE_TARGET) $^
