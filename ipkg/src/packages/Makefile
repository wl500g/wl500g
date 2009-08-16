#
#
# Copyright (C) 2004 by Oleg I. Vdovikin <oleg@cs.msu.su>
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
#

IPKG_UTILS=ipkg-utils-1.7
IPKG_UTILS_PATCHES=ipkg-utils-1.7-ipkg_build_clean.patch \
	ipkg-utils-1.7-ipkg_buildpackage.patch ipkg-utils-1.7-ipkg_tar_argument_order.patch

IPKG=ipkg-0.99.163
#IPKG_PATCHES=ipkg-configure.patch ipkg-opt.patch
IPKG_PATCHES=ipkg-opt.patch
IPKG_IPK_VERSION=0.99.163-2

TARGET_CROSS=mipsel-uclibc-
TARGET_ARCH=mipsel

TARGET_CXX=$(TARGET_CROSS)g++
TARGET_CC=$(TARGET_CROSS)gcc
TARGET_LD=$(TARGET_CROSS)ld
TARGET_AR=$(TARGET_CROSS)ar
TARGET_AS=$(TARGET_CROSS)as
TARGET_NM=$(TARGET_CROSS)nm
TARGET_RANLIB=$(TARGET_CROSS)ranlib
TARGET_STRIP=$(TARGET_CROSS)strip
TARGET_CONFIGURE_OPTS= \
        AR=$(TARGET_AR) \
        AS=$(TARGET_AS) \
        LD=$(TARGET_LD) \
        NM=$(TARGET_NM) \
        CC=$(TARGET_CC) \
        GCC=$(TARGET_CC) \
        CXX=$(TARGET_CXX) \
        RANLIB=$(TARGET_RANLIB) \
        STRIP=$(TARGET_STRIP) \
        CPPFLAGS="-mips32" 

BUILD=$(shell (pwd -P))/$(TARGET_ARCH)
PACKAGES=$(BUILD)/packages
IPKG_UTILS_DIR=$(BUILD)/ipkg-utils
IPKG_BUILD=$(IPKG_UTILS_DIR)/ipkg-build -c -o root -g root

all: prep ipkg-utils packages
	@true

packages: ipkg-ipk
	(cd $(PACKAGES) && $(IPKG_UTILS_DIR)/ipkg-make-index . > $(PACKAGES)/Packages)
	gzip -c9 $(PACKAGES)/Packages > $(PACKAGES)/Packages.gz

prep:
	@mkdir -p $(PACKAGES)

$(BUILD)/ipkg-utils: $(IPKG_UTILS).tar.gz $(IPKG_UTILS_PATCHES)
	@rm -rf $(BUILD)/$(IPKG_UTILS) $(BUILD)/ipkg-utils
	tar xzf $(IPKG_UTILS).tar.gz -C $(BUILD)
	cat $(IPKG_UTILS_PATCHES) | patch -d $(BUILD)/$(IPKG_UTILS) -p1
	mv $(BUILD)/$(IPKG_UTILS) $(BUILD)/ipkg-utils
	
ipkg-utils: $(BUILD)/ipkg-utils
	@true

$(BUILD)/ipkg/Makefile: $(IPKG).tar.gz $(IPKG_PATCHES)
	@rm -rf $(BUILD)/$(IPKG) $(BUILD)/ipkg
	tar xzf $(IPKG).tar.gz -C $(BUILD)
	cat $(IPKG_PATCHES) | patch -d $(BUILD)/$(IPKG) -p1
	mv $(BUILD)/$(IPKG) $(BUILD)/ipkg
	(cd $(BUILD)/ipkg && $(TARGET_CONFIGURE_OPTS) ./configure \
		--host=mipsel-linux --prefix=/opt --disable-nls \
		--libdir=/opt/lib --with-ipkglibdir=/opt/lib --disable-shared \
	)

ipkg: $(BUILD)/ipkg/Makefile
	$(MAKE) -C $(BUILD)/ipkg

ipkg-ipk: ipkg
	rm -rf $(BUILD)/$@ $(PACKAGES)/ipkg_*_$(TARGET_ARCH).ipk
	install -d $(BUILD)/$@/opt/bin
	$(TARGET_STRIP) $(BUILD)/$^/ipkg-cl -o $(BUILD)/$@/opt/bin/ipkg
	install -m 755 $(BUILD)/$^/update-alternatives $(BUILD)/$@/opt/bin/update-alternatives
	install -d $(BUILD)/$@/opt/etc/init.d
	install -m 644 ipkg.conf $(BUILD)/$@/opt/etc/ipkg.conf
	install -m 755 ipkg-rc.unslung $(BUILD)/$@/opt/etc/init.d/rc.unslung
	install -d $(BUILD)/$@/CONTROL
	grep -v ^Depends $(BUILD)/ipkg/familiar/control.in | sed -e "s/@host_cpu@/$(TARGET_ARCH)/" \
		-e "s/@VERSION@/$(IPKG_IPK_VERSION)/" > $(BUILD)/$@/CONTROL/control
	echo "/opt/etc/ipkg.conf" > $(BUILD)/$@/CONTROL/conffiles
	$(IPKG_BUILD) $(BUILD)/$@ $(PACKAGES)

tarball:
	tar -C .. -cjf packages.tar.bz2 --exclude=$(TARGET_ARCH) --exclude=packages.tar.bz2 packages
	
clean distclean:
	rm -rf $(BUILD)
