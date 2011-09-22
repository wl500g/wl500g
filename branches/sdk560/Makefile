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

export KERNEL_DIR := $(ROOT)/linux/linux-2.6
KERNEL=kernel-2.6
BRCM-SRC=brcm-src-2.6
IPROUTE2=iproute2-2.6
IPTABLES=iptables-2.6

BUSYBOX=busybox-1.18.5
DROPBEAR=dropbear-0.53.1
DNSMASQ=dnsmasq-2.58
LPRNG=LPRng-3.8.22
P910ND=p910nd-0.95
SAMBA=samba-2.0.10
E2FSPROGS=e2fsprogs-1.41.14
UCDSNMP=ucd-snmp-3.6.2
MINIUPNPD=miniupnpd-1.6
PPP=ppp-2.4.5
RP-PPPOE=rp-pppoe-3.10
ACCEL-PPTP=accel-pptp-git-20100829
PPTP=pptp-1.7.1
LZMA=lzma457
NFSUTILS=nfs-utils-1.1.6
PORTMAP=portmap_6
RADVD=radvd-1.8
QUAGGA=quagga-0.99.17
L2TP=rp-l2tp
XL2TPD=xl2tpd-1.3.0
BRIDGE=bridge-utils-1.0.6
IGMPPROXY=igmpproxy-0.1
VSFTPD=vsftpd-2.3.4
UDPXY=udpxy-1.0-Chipmunk-20
INADYN=inadyn-1.96.3
SCSIIDLE=scsi-idle-2.4.23
LIBUSB10=libusb-1.0.8
USBMODESWITCH=usb-modeswitch-1.1.9
MADWIMAX=madwimax-0.1.1
LLTD=LLTD-PortingKit
TCPDUMP=tcpdump-4.1.1
LIBPCAP=libpcap-1.1.1
HOTPLUG2=hotplug2-0.9
UDEV=udev-113
NTFS3G=ntfs-3g_ntfsprogs-2011.4.12AR.7
SYSFSUTILS=sysfsutils-2.1.0
WPA_SUPPLICANT=wpa_supplicant-0.6.10

UCLIBC=uClibc-0.9.30.1

# tar has --exclude parameter ?
export TAR_EXCL_SVN := $(shell tar --exclude .svn -cf - Makefile >/dev/null 2>&1 && echo "--exclude .svn")

export PATCHER := $(shell pwd)/patch.sh

define patches_list
    $(shell ls -1 $(1)/[0-9][0-9][0-9]-*.patch 2>/dev/null)
endef

DIFF := LC_ALL=C TZ=UTC0 diff

define make_diff
    (cd $(ROOT) && $(DIFF) $(1) -x'*.o' -x'*.orig' $(2)/$(4) $(3)/$(4) | grep -v "^Files .* differ$$" | grep -v ^Binary.*differ$$) > $(4).diff
    diffstat $(4).diff
endef

all: prep custom
	@true

custom:	$(TOP)/.config loader busybox dropbear dnsmasq p910nd samba iproute2 iptables \
	ppp pptp rp-l2tp rp-pppoe accel-pptp xl2tpd \
	nfs-utils portmap radvd quagga ucd-snmp igmpproxy vsftpd udpxy \
	bpalogin bridge ez-ipupdate inadyn httpd libjpeg lib LPRng \
	misc netconf nvram others rc mjpg-streamer udev hotplug2 \
	scsi-idle libusb usb_modeswitch wimax lltd tcpdump ntfs-3g \
	shared upnp miniupnpd utils wlconf www libbcmcrypto asustrx cdma \
	sysfsutils e2fsprogs wpa_supplicant lanauth authcli
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
	$(MAKE) -C $(KERNEL) version
	$(MAKE) -C $(TOP) .config

$(ROOT)/lzma: $(LZMA).tbz2
	@rm -rf $@ && mkdir -p $@
	tar -C $@ -xjf $(LZMA).tbz2
	$(PATCHER) -Z $@ $(LZMA).patch

lzma: $(ROOT)/lzma
	@true

wl:
	$(MAKE) -C $(BRCM-SRC) $@

brcm-shared:
	$(MAKE) -C $(BRCM-SRC) $@

kernel-mrproper:
	$(MAKE) -C $(KERNEL) mrproper

kernel-patch:
	$(MAKE) -C $(KERNEL) patch

kernel-extra-drivers:
	$(MAKE) -C $(KERNEL) extra-drivers

kernel: lzma wl brcm-shared kernel-extra-drivers kernel-patch
	$(MAKE) -C $(KERNEL) config

$(ROOT)/asustrx:
	@rm -rf $@
	tar -C . $(TAR_EXCL_SVN) -cf - asustrx | tar -C $(ROOT) -xf -

asustrx: $(ROOT)/asustrx
	@true

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
	tar -xzf $^ -C $(TOP)
	$(PATCHER) -Z $(TOP)/$(VSFTPD) $(vsftpd_Patches)
	mv $(TOP)/$(VSFTPD) $@

vsftpd: $(TOP)/vsftpd
	@true

ntfs-3g_Patches := $(call patches_list,ntfs-3g)

$(TOP)/ntfs-3g: ntfs-3g/$(NTFS3G).tgz
	@rm -rf $(TOP)/$(NTFS3G) $@
	tar -xzf $^ -C $(TOP)
	$(PATCHER) -Z $(TOP)/$(NTFS3G) $(ntfs-3g_Patches)
	mv $(TOP)/$(NTFS3G) $@

ntfs-3g: $(TOP)/ntfs-3g
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

iproute2:
	$(MAKE) -C $(IPROUTE2) $@

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

iptables:
	$(MAKE) -C $(IPTABLES) $@

nfs-utils_Patches := $(call patches_list,nfs-utils)

$(TOP)/nfs-utils: nfs-utils/$(NFSUTILS).tar.bz2
	@rm -rf $(TOP)/$(NFSUTILS) $@
	tar -xjf $^ -C $(TOP)
	$(PATCHER) -Z $(TOP)/$(NFSUTILS) $(nfs-utils_Patches)
	mv $(TOP)/$(NFSUTILS) $@

nfs-utils: $(TOP)/nfs-utils
	@true

portmap_Patches := $(call patches_list,portmap)

$(TOP)/portmap: portmap/$(PORTMAP).tar.gz
	@rm -rf $(TOP)/$(PORTMAP) $@
	tar -xzf $^ -C $(TOP)
	$(PATCHER) -Z $(TOP)/$(PORTMAP) $(portmap_Patches)
	mv $(TOP)/$(PORTMAP) $@

portmap: $(TOP)/portmap
	@true

bridge_Patches := $(call patches_list,bridge-utils)

$(TOP)/bridge: bridge-utils/$(BRIDGE).tar.gz
	@rm -rf $(TOP)/$(BRIDGE) $@
	tar -xzf $^ -C $(TOP)
	$(PATCHER) -Z $(TOP)/$(BRIDGE) $(bridge_Patches)
	mv $(TOP)/$(BRIDGE) $@

bridge: $(TOP)/bridge
	@true

radvd_Patches := $(call patches_list,radvd)

$(TOP)/radvd: radvd/$(RADVD).tar.gz
	@rm -rf $(TOP)/$(RADVD) $@
	tar -xzf $^ -C $(TOP)
	$(PATCHER) -Z $(TOP)/$(RADVD) $(radvd_Patches)
	mv $(TOP)/$(RADVD) $@

radvd: $(TOP)/radvd
	@true

quagga_Patches := $(call patches_list,quagga)

$(TOP)/quagga: quagga/$(QUAGGA).tar.gz
	@rm -rf $(TOP)/$(QUAGGA) $@
	tar -xzf $^ -C $(TOP)
	$(PATCHER) -Z $(TOP)/$(QUAGGA) $(quagga_Patches)
	mv $(TOP)/$(QUAGGA) $@

quagga: $(TOP)/quagga
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

$(TOP)/rp-l2tp:
	tar -C . $(TAR_EXCL_SVN) -cf - $(L2TP) | tar -C $(TOP) -xf -

rp-l2tp: $(TOP)/rp-l2tp
	@true

xl2tpd_Patches := $(call patches_list,xl2tpd)

$(TOP)/xl2tpd: xl2tpd/$(XL2TPD).tar.gz
	@rm -rf $(TOP)/$(XL2TPD) $@
	tar -xzf $^ -C $(TOP)
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
	tar -xzf $^ -C $(TOP)
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

udpxy_Patches := $(call patches_list,udpxy)

$(TOP)/udpxy: udpxy/$(UDPXY).tgz
	@rm -rf $(TOP)/$(UDPXY) $@
	tar -xzf $^ -C $(TOP)
	$(PATCHER) -Z $(TOP)/$(UDPXY) $(udpxy_Patches)
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
	tar -xjf $^ -C $(TOP)
	[ ! -f bpalogin.patch ] || $(PATCHER) -Z $@ bpalogin.patch

bpalogin: $(TOP)/bpalogin
	@true

rcamd_Patches := $(call patches_list,mjpg-streamer)

$(TOP)/mjpg-streamer: mjpg-streamer/mjpg-streamer-r103.tar.bz2
	tar -xjf $^ -C $(TOP)
	$(PATCHER) -Z $(TOP)/mjpg-streamer $(rcamd_Patches)

mjpg-streamer: $(TOP)/mjpg-streamer
	@true

libjpeg_Patches := $(call patches_list,libjpeg)

$(TOP)/jpeg-8b: libjpeg/jpegsrc.v8b.tar.bz2
	tar -xjf $^ -C $(TOP)
	$(PATCHER) -Z $@ $(libjpeg_Patches)

libjpeg: $(TOP)/jpeg-8b
	@true

scsi_idle_Patches := $(call patches_list,scsi-idle)

$(TOP)/scsi-idle: scsi-idle/$(SCSIIDLE).tar.gz
	@rm -rf $(TOP)/$(SCSIIDLE) $@
	tar -xzf $^ -C $(TOP)
	$(PATCHER) -Z $(TOP)/$(SCSIIDLE) $(scsi_idle_Patches)
	mv $(TOP)/$(SCSIIDLE) $@ && touch $@

scsi-idle: $(TOP)/scsi-idle
	@true

libusb: $(TOP)/libusb10
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
	tar -jxf $^ -C $(TOP)
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

libpcap_Patches := $(call patches_list,tcpdump/libpcap)

$(TOP)/libpcap: tcpdump/libpcap/$(LIBPCAP).tar.gz
	rm -rf $(TOP)/$(LIBPCAP) $@
	tar -zxf $^ -C $(TOP)
	$(PATCHER) -Z $(TOP)/$(LIBPCAP) $(libpcap_Patches)
	mv $(TOP)/$(LIBPCAP) $@ && touch $@

libpcap: $(TOP)/libpcap
	@true

tcpdump_Patches := $(call patches_list,tcpdump)

$(TOP)/tcpdump: tcpdump/$(TCPDUMP).tar.gz
	rm -rf $(TOP)/$(TCPDUMP) $@
	tar -zxf $^ -C $(TOP)
	$(PATCHER) -Z $(TOP)/$(TCPDUMP) $(tcpdump_Patches)
	mv $(TOP)/$(TCPDUMP) $@ && touch $@

tcpdump: libpcap $(TOP)/tcpdump
	@true

hotplug2_Patches := $(call patches_list,hotplug2)

$(TOP)/hotplug2: hotplug2/$(HOTPLUG2).tar.gz
	@rm -rf $(TOP)/$(HOTPLUG2) $@
	tar -xzf $^ -C $(TOP)
	cp -pf hotplug2/hotplug2.rules $(TOP)/$(HOTPLUG2)/examples/hotplug2.rules-2.6kernel
	$(PATCHER) -Z $(TOP)/$(HOTPLUG2) $(hotplug2_Patches)
	mv $(TOP)/$(HOTPLUG2) $@ && touch $@

hotplug2: $(TOP)/hotplug2
	@true

udev_Patches := $(call patches_list,udev)

$(TOP)/udev: udev/$(UDEV).tar.bz2
	@rm -rf $(TOP)/$(UDEV) $@
	tar -xjf $^ -C $(TOP)
	$(PATCHER) -Z $(TOP)/$(UDEV) $(udev_Patches)
	mv $(TOP)/$(UDEV) $@ && touch $@

udev: $(TOP)/udev
	@true

sysfsutils_Patches := $(call patches_list,sysfsutils)

$(TOP)/sysfsutils: sysfsutils/$(SYSFSUTILS).tar.bz2
	@rm -rf $(TOP)/$(SYSFSUTILS) $@
	tar -xjf $^ -C $(TOP)
	$(PATCHER) -Z $(TOP)/$(SYSFSUTILS) $(sysfsutils_Patches)
	mv $(TOP)/$(SYSFSUTILS) $@ && touch $@

sysfsutils: $(TOP)/sysfsutils
	@true

e2fsprogs_Patches := $(call patches_list,e2fsprogs)

$(TOP)/e2fsprogs: e2fsprogs/$(E2FSPROGS).tar.bz2
	@rm -rf $(TOP)/$(E2FSPROGS) $@
	tar -xjf $^ -C $(TOP)
	$(PATCHER) -Z $(TOP)/$(E2FSPROGS) $(e2fsprogs_Patches)
	cp e2fsprogs/e2fsck.conf $(TOP)/$(E2FSPROGS)/e2fsck/e2fsck.conf
	mv $(TOP)/$(E2FSPROGS) $@ && touch $@

e2fsprogs: $(TOP)/e2fsprogs
	@true

$(TOP)/others:
	tar -C . $(TAR_EXCL_SVN) -cf - others | tar -C $(TOP) -xf -

others: $(TOP)/others

$(TOP)/lib:
	tar -C . $(TAR_EXCL_SVN) -cf - lib | tar -C $(TOP) -xf -

lib: $(TOP)/lib

libbcmcrypto:
	$(MAKE) -C $(BRCM-SRC) $@

wlconf:
	$(MAKE) -C $(BRCM-SRC) $@

$(TOP)/upnp:
	[ -d $@ ] || \
		tar -C . $(TAR_EXCL_SVN) -cf - upnp | tar -C $(TOP) -xf -

upnp: $(TOP)/upnp
	@true

miniupnpd_Patches := $(call patches_list,miniupnpd)

$(TOP)/miniupnpd: miniupnpd/$(MINIUPNPD).tar.gz
	rm -rf $(TOP)/$(MINIUPNPD) $@
	tar -zxf $^ -C $(TOP)
	$(PATCHER) -Z $(TOP)/$(MINIUPNPD) $(miniupnpd_Patches)
	mv $(TOP)/$(MINIUPNPD) $@ && touch $@

miniupnpd: $(TOP)/miniupnpd
	@true

wpa_supplicant_Patches := $(call patches_list,wpa_supplicant)

$(TOP)/wpa_supplicant: wpa_supplicant/$(WPA_SUPPLICANT).tar.gz
	rm -rf $(TOP)/$(WPA_SUPPLICANT) $@
	tar -zxf $^ -C $(TOP)
	$(PATCHER) -Z $(TOP)/$(WPA_SUPPLICANT) $(wpa_supplicant_Patches)
	cp -p wpa_supplicant/wpa_supplicant.config $(TOP)/$(WPA_SUPPLICANT)/wpa_supplicant/.config
	mv $(TOP)/$(WPA_SUPPLICANT) $@ && touch $@

wpa_supplicant: $(TOP)/wpa_supplicant
	@true

$(TOP)/lanauth:
	[ -d $@ ] || \
		tar -C . $(TAR_EXCL_SVN) -cf - lanauth | tar -C $(TOP) -xf -

lanauth: $(TOP)/lanauth
	@true

$(TOP)/authcli:
	[ -d $@ ] || \
		tar -C . $(TAR_EXCL_SVN) -cf - authcli | tar -C $(TOP) -xf -

authcli: $(TOP)/authcli
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

.PHONY: custom kernel kernel-patch kernel-extra-drivers brcm-src www \
	accel-pptp busybox dropbear ez-ipupdate inadyn httpd iptables others \
	rc mjpg-streamer libjpeg config igmpproxy iproute2 lib shared utils
