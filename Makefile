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

ROOT := $(shell (cd .. && pwd -P))
export TOP := $(ROOT)/gateway
export KERNEL_DIR := $(ROOT)/linux/linux

BUSYBOX=busybox-1.18.3
DROPBEAR=dropbear-0.52
DNSMASQ=dnsmasq-2.57
LPRNG=LPRng-3.8.22
P910ND=p910nd-0.95
SAMBA=samba-2.0.10
IPROUTE2=iproute2-2.4.7-now-ss020116-try
#E2FSPROGS=e2fsprogs-1.35
UCDSNMP=ucd-snmp-3.6.2
IPTABLES=iptables-1.3.8
MINIUPNPD=miniupnpd-1.4.20100921
PPP=ppp-2.4.5
RP-PPPOE=rp-pppoe-3.10
ACCEL-PPTP=accel-pptp-git-20100829
PPTP=pptp-1.7.1
LZMA=lzma457
NFSUTILS=nfs-utils-1.0.9
PORTMAP=portmap_4
RADVD=radvd-1.7
L2TP=rp-l2tp-0.4
XL2TPD=xl2tpd-1.2.7
BRIDGE=bridge-utils-1.0.6
IGMPPROXY=igmpproxy-0.1
VSFTPD=vsftpd-2.3.4
UDPXY=udpxy-1.0-Chipmunk-19
INADYN=inadyn-1.96.3
SCSIIDLE=scsi-idle-2.4.23
LIBUSB=libusb-compat-0.1.3
LIBUSB10=libusb-1.0.8
USBMODESWITCH=usb-modeswitch-1.1.7
MADWIMAX=madwimax-0.1.1
LLTD=LLTD-PortingKit

UCLIBC=uClibc-0.9.29

ET=et-4.108.9
WL=wl-4.150.10.29
LIBBCMCRYPTO=libbcmcrypto-3.130.20

# tar has --exclude parameter ?
TAR_EXCL_SVN := $(shell tar --exclude .svn -cf - Makefile >/dev/null 2>&1 && echo "--exclude .svn")

PATCHER := $(shell pwd)/patch.sh

define patches_list
    $(shell ls -1 $(1)/[0-9][0-9][0-9]-*.patch 2>/dev/null)
endef

DIFF := LC_ALL=C TZ=UTC0 diff

define make_diff
    (cd $(ROOT) && $(DIFF) $(1) -x'*.o' -x'*.orig' $(2)/$(4) $(3)/$(4) | grep -v "^Files .* differ$$" | grep -v ^Binary.*differ$$) > $(4).diff
    diffstat $(4).diff
endef

MIPS_Kernel_Patches:=$(call patches_list,kernel/linux-mips)
OPENWRT_Kernel_Patches:=$(call patches_list,kernel/openwrt)
OPENWRT_Brcm_Patches:=$(call patches_list,kernel/openwrt/brcm)
OUR_Kernel_Patches:=$(call patches_list, kernel)

all: prep custom
	@true

custom:	$(TOP)/.config loader busybox dropbear dnsmasq p910nd samba iproute2 iptables \
	ppp pptp rp-l2tp rp-pppoe accel-pptp xl2tpd \
	nfs-utils portmap radvd ucd-snmp igmpproxy vsftpd udpxy \
	bpalogin bridge ez-ipupdate inadyn httpd jpeg-6b lib LPRng \
	misc netconf nvram others rc rcamdmips \
	scsi-idle libusb usb_modeswitch wimax lltd \
	shared upnp miniupnpd utils wlconf www libbcmcrypto asustrx cdma
	@echo
	@echo Sources prepared for compilation
	@echo

$(TOP):
	@mkdir -p $(TOP)

$(TOP)/Makefile: Makefile.top
	cp -p $^ $@

prep: $(TOP) $(TOP)/Makefile
	-svnversion -c 2>/dev/null | sed "s/.*://" > $(TOP)/.svnrev

$(TOP)/config:
	[ -d $@ ] || \
		tar -C . $(TAR_EXCL_SVN) -cf - config | tar -C $(TOP) -xf -

config: $(TOP)/config
	@true

$(TOP)/.config: config shared
	$(MAKE) -C $(KERNEL_DIR) include/linux/version.h
	$(MAKE) -C $(TOP) .config

$(ROOT)/lzma: $(LZMA).tbz2
	@rm -rf $@ && mkdir -p $@
	tar -C $@ -xjf $(LZMA).tbz2
	$(PATCHER) -Z $@ $(LZMA).patch

lzma: $(ROOT)/lzma
	@true

wl:
	tar -C $(ROOT) --recursive-unlink -xjf brcm-src/$(WL).tar.bz2

brcm_Patches := $(call patches_list,brcm-src)

brcm-shared:
	tar -C brcm-src $(TAR_EXCL_SVN) -cf - include rts shared emf et | tar -C $(ROOT) --recursive-unlink -xf -
	$(PATCHER) -Z $(ROOT) $(brcm_Patches)

kernel-mrproper:
	$(MAKE) -C $(KERNEL_DIR) mrproper

kernel-patch:
	@echo Preparing kernel ...
	[ -d $(KERNEL_DIR)/arch/mips/bcm947xx ] || tar -C $(KERNEL_DIR) -xvjf kernel/brcm-boards.tar.bz2
	$(MAKE) -C $(KERNEL_DIR)/arch/mips/bcm947xx/compressed/ clean
	@$(PATCHER) -Z $(KERNEL_DIR) kernel/buildhost.patch
	$(MAKE) -C $(KERNEL_DIR) mrproper
	@echo Patching kernel...
	@$(PATCHER) -Z $(KERNEL_DIR) $(MIPS_Kernel_Patches)
	@$(PATCHER) -Z $(KERNEL_DIR) $(OPENWRT_Kernel_Patches)
	@$(PATCHER) -Z $(KERNEL_DIR) $(OPENWRT_Brcm_Patches)
	@$(PATCHER) -Z $(KERNEL_DIR) $(OUR_Kernel_Patches)

kernel-extra-drivers:
	tar -C kernel/drivers/ov51x-1.65 $(TAR_EXCL_SVN) -cf - . | tar -C $(KERNEL_DIR)/drivers/usb -xf -
	tar -C kernel/drivers/pwc-9.0.2 $(TAR_EXCL_SVN) -cf - . | tar -C $(KERNEL_DIR)/drivers/usb -xf -
	if [ ! -d $(KERNEL_DIR)/fs/fuse ]; then \
	  tar -C $(KERNEL_DIR)/fs -xvjf kernel/drivers/fuse-2.5.3.tar.bz2 fuse-2.5.3/kernel/ \
	   && mv $(KERNEL_DIR)/fs/fuse-2.5.3/kernel $(KERNEL_DIR)/fs/fuse && rmdir $(KERNEL_DIR)/fs/fuse-2.5.3; \
	  $(PATCHER) -Z $(KERNEL_DIR)/fs/fuse kernel/drivers/fuse-2.5.3.patch; \
	fi

kernel: lzma wl brcm-shared kernel-patch kernel-extra-drivers
	cp -p kernel/kernel.config $(KERNEL_DIR)/arch/mips/defconfig-bcm947xx

asustrx:
	tar -C $(ROOT) -xjf asustrx.tar.bz2 

$(TOP)/loader:
	@rm -rf $(TOP)/loader
	tar -C . $(TAR_EXCL_SVN) -cf - loader | tar -C $(TOP) -xf -

loader: $(TOP)/loader
	@true

busybox_Patches := $(call patches_list,busybox)

$(TOP)/busybox: busybox/$(BUSYBOX).tar.bz2
	@rm -rf $(TOP)/$(BUSYBOX) $@
	tar -xjf busybox/$(BUSYBOX).tar.bz2 -C $(TOP)
	rm -rf $(TOP)/$(BUSYBOX)/e2fsprogs/old_e2fsprogs/fsck.c \
	    $(TOP)/$(BUSYBOX)/e2fsprogs/old_e2fsprogs/chattr.c \
	    $(TOP)/$(BUSYBOX)/e2fsprogs/old_e2fsprogs/lsattr.c \
	    $(TOP)/$(BUSYBOX)/e2fsprogs/old_e2fsprogs/README \
	    $(TOP)/$(BUSYBOX)/e2fsprogs/old_e2fsprogs/uuid \
	    $(TOP)/$(BUSYBOX)/e2fsprogs/old_e2fsprogs/blkid
	mv $(TOP)/$(BUSYBOX)/e2fsprogs/old_e2fsprogs/* $(TOP)/$(BUSYBOX)/e2fsprogs/
	rmdir $(TOP)/$(BUSYBOX)/e2fsprogs/old_e2fsprogs
	$(PATCHER) -Z $(TOP)/$(BUSYBOX) $(busybox_Patches)
	mkdir -p $(TOP)/$(BUSYBOX)/sysdeps/linux/
	cp -p busybox/busybox.config $(TOP)/$(BUSYBOX)/sysdeps/linux/defconfig
	chmod a+x $(TOP)/$(BUSYBOX)/testsuite/*.tests
	find $(TOP)/$(BUSYBOX)/shell/ash_test/ -name *.tests -perm 0644 -exec chmod a+x '{}' \;
	mv $(TOP)/$(BUSYBOX) $@

busybox: $(TOP)/busybox
	@true

vsftpd_Patches := $(call patches_list,vsftpd)

$(TOP)/vsftpd: vsftpd/$(VSFTPD).tar.gz
	@rm -rf $(TOP)/$(VSFTPD) $@
	tar -xzf vsftpd/$(VSFTPD).tar.gz -C $(TOP)
	$(PATCHER) -Z $(TOP)/$(VSFTPD) $(vsftpd_Patches)
	mv $(TOP)/$(VSFTPD) $@

vsftpd: $(TOP)/vsftpd
	@true

dropbear_Patches := $(call patches_list,dropbear)

$(TOP)/dropbear: dropbear/$(DROPBEAR).tar.bz2
	@rm -rf $(TOP)/$(DROPBEAR) $@
	tar -xjf $^ -C $(TOP)
	$(PATCHER) -Z $(TOP)/$(DROPBEAR) $(dropbear_Patches)
	mv $(TOP)/$(DROPBEAR) $@

dropbear: $(TOP)/dropbear
	@true

ucd-snmp_Patches := $(call patches_list,ucd-snmp)

$(TOP)/ucd-snmp: ucd-snmp/$(UCDSNMP).tar.gz
	@rm -rf $(TOP)/$(UCDSNMP) $@
	tar -xzf $^ -C $(TOP)
	$(PATCHER) -Z $(TOP)/$(UCDSNMP) $(ucd-snmp_Patches)
	mv $(TOP)/$(UCDSNMP) $@

ucd-snmp: $(TOP)/ucd-snmp
	@true

iproute2_Patches := $(call patches_list,iproute2)

$(TOP)/iproute2: iproute2/$(IPROUTE2).tar.bz2
	@rm -rf $(TOP)/$@
	tar -xjf $^ -C $(TOP)
	$(PATCHER) -Z $(TOP)/iproute2 $(iproute2_Patches) && touch $@

iproute2: $(TOP)/iproute2
	@true

dnsmasq_Patches := $(call patches_list,dnsmasq)

$(TOP)/dnsmasq: dnsmasq/$(DNSMASQ).tar.gz
	@rm -rf $(TOP)/$(DNSMASQ) $@
	tar -xzf $^ -C $(TOP)
	$(PATCHER) -Z $(TOP)/$(DNSMASQ) $(dnsmasq_Patches)
	mv $(TOP)/$(DNSMASQ) $@ && touch $@

dnsmasq: $(TOP)/dnsmasq
	@true

LPRng_Patches := $(call patches_list,LPRng)

$(TOP)/LPRng: LPRng/$(LPRNG).tgz
	@rm -rf $(TOP)/$(LPRNG) $@
	tar -xzf $^ -C $(TOP)
	$(PATCHER) -Z $(TOP)/$(LPRNG) $(LPRng_Patches)
	mv $(TOP)/$(LPRNG) $@

LPRng: $(TOP)/LPRng
	@true

p910nd_Patches := $(call patches_list,p910nd)

$(TOP)/p910nd: p910nd/$(P910ND).tar.bz2
	@rm -rf $(TOP)/$(P910ND) $@
	tar -xjf $^ -C $(TOP)
	$(PATCHER) -Z $(TOP)/$(P910ND) $(p910nd_Patches)
	mv $(TOP)/$(P910ND) $@ && touch $@

p910nd-diff:
	@rm -rf $(TOP)/$(P910ND)
	tar -xjf $(P910ND).tar.bz2 -C $(TOP)
	-rm -f $(TOP)/p910nd/p910nd
	-cd $(TOP) && $(DIFF) -BurpN $(P910ND) p910nd > $(P910ND).patch

p910nd: $(TOP)/p910nd
	@true

samba_Patches := $(call patches_list,samba)

$(TOP)/samba: samba/$(SAMBA).tar.bz2
	@rm -rf $(TOP)/$(SAMBA) $@
	tar -xjf $^ -C $(TOP)
	$(PATCHER) -Z $(TOP)/$(SAMBA) $(samba_Patches)
	tar -xzvf samba/$(SAMBA)-codepages.tar.gz -C $(TOP)/$(SAMBA)
	mv $(TOP)/$(SAMBA) $@

samba: $(TOP)/samba
	@true

iptables_Patches := $(call patches_list,iptables)

$(TOP)/iptables: iptables/$(IPTABLES).tar.bz2
	@rm -rf $(TOP)/$(IPTABLES) $@
	tar -xjf $^ -C $(TOP)
	$(PATCHER) -Z $(TOP)/$(IPTABLES) $(iptables_Patches)
	chmod a+x $(TOP)/$(IPTABLES)/extensions/.*-test $(TOP)/$(IPTABLES)/extensions/.*-test6
	mv $(TOP)/$(IPTABLES) $@ && touch $@

iptables: $(TOP)/iptables
	@true

nfs-utils_Patches := $(call patches_list,nfs-utils)

$(TOP)/nfs-utils: nfs-utils/$(NFSUTILS).tar.bz2
	@rm -rf $(TOP)/$(NFSUTILS) $@
	tar -xjf nfs-utils/$(NFSUTILS).tar.bz2 -C $(TOP)
	$(PATCHER) -Z $(TOP)/$(NFSUTILS) $(nfs-utils_Patches)
	mv $(TOP)/$(NFSUTILS) $@

nfs-utils: $(TOP)/nfs-utils
	@true

$(TOP)/portmap: $(PORTMAP).tar.gz
	@rm -rf $(TOP)/$(PORTMAP) $@
	tar -xzf $^ -C $(TOP)
	$(PATCHER) -Z $(TOP)/$(PORTMAP) $(PORTMAP).patch
	mv $(TOP)/$(PORTMAP) $@

portmap: $(TOP)/portmap
	@true

$(TOP)/bridge:
	@rm -rf $(TOP)/$(BRIDGE) $@
	tar -xzf $(BRIDGE).tar.gz -C $(TOP)
	[ ! -f $(BRIDGE).patch ] || $(PATCHER) -Z $(TOP)/$(BRIDGE) $(BRIDGE).patch
	mv $(TOP)/$(BRIDGE) $@

bridge: $(TOP)/bridge
	@true

radvd_Patches := $(call patches_list,radvd)

$(TOP)/radvd: radvd/$(RADVD).tar.gz
	@rm -rf $(TOP)/$(RADVD) $@
	tar -xzf radvd/$(RADVD).tar.gz -C $(TOP)
	$(PATCHER) -Z $(TOP)/$(RADVD) $(radvd_Patches)
	mv $(TOP)/$(RADVD) $@

radvd: $(TOP)/radvd
	@true

$(TOP)/netconf:
	[ -d $@ ] || \
		tar -C . $(TAR_EXCL_SVN) -cf - netconf | tar -C $(TOP) -xf -

netconf: $(TOP)/netconf
	@true

$(TOP)/rc/Makefile:
	[ -d $@ ] || \
		tar -C . $(TAR_EXCL_SVN) -cf - rc | tar -C $(TOP) -xf -

rc: $(TOP)/rc/Makefile
	@true

ppp_Patches := $(call patches_list,ppp)

$(TOP)/ppp: ppp/$(PPP).tar.bz2
	@rm -rf $(TOP)/$(PPP) $@
	tar -xjf $^ -C $(TOP)
	$(PATCHER) -Z $(TOP)/$(PPP) $(ppp_Patches)
	mv $(TOP)/$(PPP) $@ && touch $@

ppp: $(TOP)/ppp
	@true

rp-l2tp_Patches := $(call patches_list,rp-l2tp)

$(TOP)/rp-l2tp:
	@rm -rf $(TOP)/$(L2TP) $@
	tar -xzf rp-l2tp/$(L2TP).tar.gz -C $(TOP)
	$(PATCHER) -Z $(TOP)/$(L2TP) $(rp-l2tp_Patches)
	mv $(TOP)/$(L2TP) $@

rp-l2tp: $(TOP)/rp-l2tp
	@true

xl2tpd_Patches := $(call patches_list,xl2tpd)

$(TOP)/xl2tpd:
	@rm -rf $(TOP)/$(XL2TPD) $@
	tar -xzf xl2tpd/$(XL2TPD).tar.gz -C $(TOP)
	$(PATCHER) -Z $(TOP)/$(XL2TPD) $(xl2tpd_Patches)
	mv $(TOP)/$(XL2TPD) $@

xl2tpd: $(TOP)/xl2tpd
	@true

rp-pppoe_Patches := $(call patches_list,rp-pppoe)

$(TOP)/rp-pppoe: rp-pppoe/$(RP-PPPOE).tar.gz
	@rm -rf $(TOP)/$(RP-PPPOE) $@
	tar -xzf $^ -C $(TOP)
	$(PATCHER) -Z $(TOP)/$(RP-PPPOE) $(rp-pppoe_Patches)
	mv $(TOP)/$(RP-PPPOE) $@ && touch $@

rp-pppoe: $(TOP)/rp-pppoe
	@true

accel-pptp_Patches := $(call patches_list,accel-pptp)

$(TOP)/accel-pptp: accel-pptp/$(ACCEL-PPTP).tar.bz2
	@rm -rf $(TOP)/$(ACCEL-PPTP) $@
	tar -xjf $^ -C $(TOP)
	rm -rf $(TOP)/$(ACCEL-PPTP)/pppd_plugin/src/pppd
	ln -s $(TOP)/ppp/pppd $(TOP)/$(ACCEL-PPTP)/pppd_plugin/src/pppd
	$(PATCHER) -Z $(TOP)/$(ACCEL-PPTP) $(accel-pptp_Patches)
	mv $(TOP)/$(ACCEL-PPTP) $@ && touch $@
	touch $@

accel-pptp: $(TOP)/accel-pptp
	@true

igmpproxy_Patches := $(call patches_list,igmpproxy)

$(TOP)/igmpproxy: igmpproxy/$(IGMPPROXY).tar.gz
	@rm -rf $(TOP)/igmpproxy
	tar -xzf igmpproxy/$(IGMPPROXY).tar.gz -C $(TOP)
	$(PATCHER) -Z $(TOP)/$(IGMPPROXY) $(igmpproxy_Patches)
	mv $(TOP)/$(IGMPPROXY) $@ && touch $@

igmpproxy: $(TOP)/igmpproxy
	@true

pptp_Patches := $(call patches_list,pptp)

$(TOP)/pptp: pptp/$(PPTP).tar.gz
	@rm -rf $(TOP)/$(PPTP) $@
	tar -xzf $^ -C $(TOP)
	$(PATCHER) -Z $(TOP)/$(PPTP) $(pptp_Patches)
	mv $(TOP)/$(PPTP) $@ && touch $@

pptp: $(TOP)/pptp
	@true

$(TOP)/udpxy: $(UDPXY).tgz
	@rm -rf $(TOP)/$(UDPXY) $@
	tar -xzf $^ -C $(TOP)
	[ ! -f $(UDPXY).patch ] || $(PATCHER) -Z $(TOP)/$(UDPXY) $(UDPXY).patch
	mv $(TOP)/$(UDPXY) $@ && touch $@

udpxy: $(TOP)/udpxy
	@true

$(TOP)/ez-ipupdate:
	tar -C . $(TAR_EXCL_SVN) -cf - ez-ipupdate | tar -C $(TOP) -xf -

ez-ipupdate: $(TOP)/ez-ipupdate
	@true

inadyn_Patches := $(call patches_list,inadyn)

$(TOP)/inadyn: inadyn/$(INADYN).tar.bz2
	@rm -rf $(TOP)/$(INADYN) $@
	tar -xjf $^ -C $(TOP)
	$(PATCHER) -Z $(TOP)/$(INADYN) $(inadyn_Patches)
	chmod a+x $(TOP)/$(INADYN)/configure
	mv $(TOP)/$(INADYN) $@ && touch $@

inadyn: $(TOP)/inadyn
	@true

$(TOP)/bpalogin: bpalogin.tar.bz2
	tar -xjf bpalogin.tar.bz2 -C $(TOP)
	[ ! -f bpalogin.patch ] || $(PATCHER) -Z $@ bpalogin.patch

bpalogin: $(TOP)/bpalogin
	@true

$(TOP)/rcamdmips:
	tar -C . $(TAR_EXCL_SVN) -cf - rcamdmips | tar -C $(TOP) -xf -

rcamdmips: $(TOP)/rcamdmips
	@true

$(TOP)/jpeg-6b: jpeg-6b.tar.bz2
	tar -xjf jpeg-6b.tar.bz2 -C $(TOP)
	[ ! -f jpeg-6b.patch ] || $(PATCHER) -Z $@ jpeg-6b.patch

jpeg-6b: $(TOP)/jpeg-6b
	@true

$(TOP)/scsi-idle: $(SCSIIDLE).tar.gz
	@rm -rf $(TOP)/$(SCSIIDLE) $@
	tar -xzf $^ -C $(TOP)
	[ ! -f $(SCSIIDLE).patch ] || $(PATCHER) -Z $(TOP)/$(SCSIIDLE) $(SCSIIDLE).patch
	mv $(TOP)/$(SCSIIDLE) $@ && touch $@

scsi-idle: $(TOP)/scsi-idle
	@true

$(TOP)/libusb: libusb/$(LIBUSB).tar.bz2
	@rm -rf $(TOP)/$(LIBUSB) $@
	tar -jxf $^ -C $(TOP)
	[ ! -f libusb/$(LIBUSB).patch ] || $(PATCHER) -Z $(TOP)/$(LIBUSB) libusb/$(LIBUSB).patch
	mv $(TOP)/$(LIBUSB) $@ && touch $@

libusb: $(TOP)/libusb10 $(TOP)/libusb
	@true

libusb10_Patches := $(call patches_list,libusb)

$(TOP)/libusb10: libusb/$(LIBUSB10).tar.bz2
	@rm -rf $(TOP)/$(LIBUSB10) $@
	tar -jxf $^ -C $(TOP)
	$(PATCHER) -Z $(TOP)/$(LIBUSB10) $(libusb10_Patches)
	mv $(TOP)/$(LIBUSB10) $@ && touch $@

modeswitch_Patches := $(call patches_list,usb_modeswitch)

$(TOP)/usb_modeswitch: usb_modeswitch/$(USBMODESWITCH).tar.bz2
	rm -rf $(TOP)/$(USBMODESWITCH) $@
	tar -jxf usb_modeswitch/$(USBMODESWITCH).tar.bz2  -C $(TOP)
	$(PATCHER) -Z $(TOP)/$(USBMODESWITCH) $(modeswitch_Patches)
	$(MAKE) -C $(TOP)/$(USBMODESWITCH) clean
	mv $(TOP)/$(USBMODESWITCH) $@ && touch $@
	# usb_modeswitch data
	tar -C usb_modeswitch $(TAR_EXCL_SVN) -cf - data | tar -C $(TOP)/usb_modeswitch -xf -

usb_modeswitch: $(TOP)/usb_modeswitch
	@true

wimax_Patches := $(call patches_list,wimax)

$(TOP)/madwimax: wimax/$(MADWIMAX).tar.gz
	rm -rf $(TOP)/$(MADWIMAX) $@
	tar -zxf $^ -C $(TOP)
	$(PATCHER) -Z $(TOP)/$(MADWIMAX) $(wimax_Patches)
	mv $(TOP)/$(MADWIMAX) $@ && touch $@

wimax: $(TOP)/madwimax
	@true

lltd_Patches := $(call patches_list,lltd)

$(TOP)/lltd: lltd/$(LLTD).tar.bz2
	rm -rf $(TOP)/$(LLTD) $@
	tar -jxf $^ -C $(TOP)
	$(PATCHER) -Z $(TOP)/$(LLTD) $(lltd_Patches)
	tar -jxf lltd/icons.tar.bz2 -C $(TOP)/$(LLTD)
	mv $(TOP)/$(LLTD) $@ && touch $@

lltd: $(TOP)/lltd
	@true

$(TOP)/others:
	tar -C . $(TAR_EXCL_SVN) -cf - others | tar -C $(TOP) -xf -

others: $(TOP)/others

$(TOP)/lib:
	tar -C . $(TAR_EXCL_SVN) -cf - lib | tar -C $(TOP) -xf -

lib: $(TOP)/lib

libbcmcrypto: brcm-src/$(LIBBCMCRYPTO).tar.gz
	tar -zxf $^ -C $(TOP)
	$(PATCHER) $(TOP)/libbcmcrypto brcm-src/$(LIBBCMCRYPTO).patch

$(TOP)/wlconf:
	tar -C brcm-src/ $(TAR_EXCL_SVN) -cf - wlconf | tar -C $(TOP) -xf -

wlconf: $(TOP)/wlconf
	@true

upnp:
	[ -d $(TOP)/$@ ] || \
		tar -xjf $@.tar.bz2 -C $(TOP)
	[ ! -f $@.diff ] || $(PATCHER) -Z $(TOP) $@.diff

upnp-diff:
	$(call make_diff,-BurpN,tools,gateway,upnp)

miniupnpd_Patches := $(call patches_list,miniupnpd)

$(TOP)/miniupnpd: miniupnpd/$(MINIUPNPD).tar.gz
	rm -rf $(TOP)/$(MINIUPNPD) $@
	tar -zxf $^ -C $(TOP)
	$(PATCHER) -Z $(TOP)/$(MINIUPNPD) $(miniupnpd_Patches)
	mv $(TOP)/$(MINIUPNPD) $@ && touch $@

miniupnpd: $(TOP)/miniupnpd
	@true

$(TOP)/httpd:
	[ -d $@ ] || \
		tar -C . $(TAR_EXCL_SVN) -cf - httpd | tar -C $(TOP) -xf -

httpd: $(TOP)/httpd
	@true

$(TOP)/nvram:
	[ -d $@ ] || \
		tar -C . $(TAR_EXCL_SVN) -cf - nvram | tar -C $(TOP) -xf -

nvram: $(TOP)/nvram
	@true

$(TOP)/shared:
	[ -d $@ ] || \
		tar -C . $(TAR_EXCL_SVN) -cf - shared | tar -C $(TOP) -xf -

shared: $(TOP)/shared
	@true

$(TOP)/utils:
	[ -d $@ ] || \
		tar -C . $(TAR_EXCL_SVN) -cf - utils | tar -C $(TOP) -xf -

utils: $(TOP)/utils
	@true

$(TOP)/cdma:
	[ -d $@ ] || \
		tar -C . $(TAR_EXCL_SVN) -cf - cdma | tar -C $(TOP) -xf -

cdma: $(TOP)/cdma
	@true


$(TOP)/misc:
	[ -d $@ ] || \
		tar -C . $(TAR_EXCL_SVN) -cf - misc | tar -C $(TOP) -xf -

misc: $(TOP)/misc
	@true

$(TOP)/www:
	[ -d $@ ] || \
		tar -C . $(TAR_EXCL_SVN) -cf - www | tar -C $(TOP) -xf -

www: $(TOP)/www
	@true


#%-diff:
#	[ -d $(SRC)/$* ] || [ -d $(TOP)/$* ] && \
#	    $(call make_diff,-BurpN,router,gateway,$*)

.PHONY: custom kernel kernel-patch kernel-extra-drivers brcm-shared www \
	accel-pptp busybox dropbear ez-ipupdate inadyn httpd iptables others \
	rc rcamdmips jpeg-6b config igmpproxy iproute2 lib shared utils
