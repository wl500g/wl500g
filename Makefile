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

export ROOT := $(shell (cd .. && pwd -P))
export TOP := $(ROOT)/gateway
export CWD := $(shell pwd)

export KERNEL_DIR := $(ROOT)/linux/linux-2.6

ACCEL-PPP=accel-ppp-git-20140916
ODHCP6C=odhcp6c-git-20140814

# tar has --exclude parameter ?
export TAR_EXCL_VCS := $(shell tar --exclude-vcs -cf - Makefile >/dev/null 2>&1 && echo "--exclude-vcs")

export PATCHER := $(CWD)/patch.sh

define patches_list
    $(shell ls -1 $(1)/$(2)[0-9][0-9][0-9]-*.patch 2>/dev/null)
endef


all: prep custom
	@true

subdirs=loader busybox dropbear dnsmasq p910nd iproute2 iptables ipset \
	rp-l2tp nfs-utils portmap radvd quagga ucd-snmp igmpproxy vsftpd udpxy \
	bpalogin bridge inadyn httpd jpeg-8b lib LPRng ppp rp-pppoe accel-pptp \
	misc nvram others rc mjpg-streamer udev \
	scsi-idle libusb usb_modeswitch madwimax uqmi lltd tcpdump libpcap \
	miniupnpd utils www config shared cdma ntfs-3g samba \
	sysfsutils e2fsprogs wpa_supplicant lanauth authcli infosrv \
	wlconf libbcmcrypto 

rsubdirs=linux asustrx lzma4xx lzma bcm57xx emf et include rts shared wl

custom:	odhcp6c accel-ppp $(subdirs)
	for dir in $(rsubdirs); do \
		ln -sfT $(CWD)/$${dir} $(ROOT)/$${dir}; \
	done
	$(MAKE) -C $(KERNEL_DIR) include/linux/version.h
	$(MAKE) -C $(TOP) .config
	@echo
	@echo Sources prepared for compilation
	@echo

$(TOP):
	@mkdir -p $(TOP)

$(TOP)/Makefile: $(TOP)
	ln -sfT $(CWD)/gateway/Makefile $@

prep: $(TOP)/Makefile
	-@echo "$(shell git rev-list --all --count origin/HEAD)$(if $(shell git status -s -uno),M,)" > $(TOP)/.svnrev


odhcp6c_Patches := $(call patches_list,odhcp6c)

$(TOP)/odhcp6c: odhcp6c/$(ODHCP6C).tar.gz
	@rm -rf $(TOP)/$(ODHCP6C) $@
	tar -xzf $^ -C $(TOP)
	$(PATCHER) -Z $(TOP)/$(ODHCP6C) $(odhcp6c_Patches)
	mv $(TOP)/$(ODHCP6C) $@

odhcp6c: $(TOP)/odhcp6c
	@true

accel-ppp_Patches := $(call patches_list,accel-ppp)

$(TOP)/accel-ppp: accel-ppp/$(ACCEL-PPP).tar.gz
	@rm -rf $(TOP)/$(ACCEL-PPP) $@
	tar -xzf $^ -C $(TOP)
	$(PATCHER) -Z $(TOP)/$(ACCEL-PPP) $(accel-ppp_Patches)
	mv $(TOP)/$(ACCEL-PPP) $@ && touch $@
	touch $@

accel-ppp: $(TOP)/accel-ppp
	@true

$(subdirs):
	ln -sfT $(CWD)/gateway/$@ $(TOP)/$@

clean distclean: $(TOP)/Makefile
	$(MAKE) -C $(TOP) $@
	$(MAKE) -C asustrx $@

.PHONY: asustrx lzma $(subdirs)
