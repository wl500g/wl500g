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
SRC := $(ROOT)/router
export TOP := $(ROOT)/gateway
export KERNEL_DIR := $(ROOT)/linux/linux

BUSYBOX=busybox-1.13.2
DROPBEAR=dropbear-0.52
DNSMASQ=dnsmasq-2.47
P910ND=p910nd-0.93
SAMBA=samba-2.0.10
IPROUTE2=iproute2-2.4.7-now-ss010824
#E2FSPROGS=e2fsprogs-1.35
UCDSNMP=ucd-snmp-3.6.2
IPTABLES=iptables-1.3.8
PPP=ppp-2.4.5-pre
PPTP=pptp-1.7.1
LZMA=lzma406
LOADER=loader-0.05
NFSUTILS=nfs-utils-1.0.7
PORTMAP=portmap_4
RADVD=radvd-0.7.2
L2TP=rp-l2tp-0.4
IGMPPROXY=igmpproxy-src-0.1-beta2
VSFTPD=vsftpd-2.0.7
UDPXY=udpxy-1.0_Chipmunk-8
NTPCLIENT=ntpclient-2007_365

UCLIBC=uClibc-0.9.29

ET=et-4.108.9
WL=wl-4.150.10.29
LIBBCMCRYPTO=libbcmcrypto-3.130.20
WLCONF=wlconf

CROSS=mipsel-uclibc-
CC=$(CROSS)gcc
LD=$(CROSS)ld
AR=$(CROSS)ar
RANLIB=$(CROSS)ranlib

EXTRACFLAGS=-mips32 -mtune=mips32 -Wno-pointer-sign

PATCHER := $(shell pwd)/patch.sh

define patches_list
    $(shell ls -1 $(1)/[0-9][0-9][0-9]-*.patch)
endef

OPENWRT_Kernel_Patches:=$(call patches_list,kernel/openwrt)
OPENWRT_Brcm_Patches:=$(call patches_list,kernel/openwrt/brcm)
OUR_Kernel_Patches:=$(call patches_list, kernel)

all: prep custom
	@true

$(ROOT)/uClibc: uClibc/$(UCLIBC).tar.bz2
	@rm -rf $(ROOT)/$(UCLIBC) $@
	tar -xjf $^ -C $(ROOT)
	@cd uClibc && $(PATCHER) $(ROOT)/$(UCLIBC) $(UCLIBC).diff \
		$(UCLIBC)-scalar_t.patch
	cp uClibc/$(UCLIBC).config $(ROOT)/$(UCLIBC)/.config
	mv $(ROOT)/$(UCLIBC) $@

uClibc: $(ROOT)/uClibc
	make -C $^ all install CROSS=$(CROSS) HOSTCC="gcc -m32"
	rm -f /opt/brcm/hndtools-mipsel-uclibc
ifeq ($(strip $(UCLIBC)),uClibc-0.9.19)
	ln -s hndtools-mipsel-uclibc-3.2.3-full /opt/brcm/hndtools-mipsel-uclibc
else
	ln -s hndtools-mipsel-uclibc-0.9.29 /opt/brcm/hndtools-mipsel-uclibc
endif

custom:	$(TOP)/.config loader busybox dropbear dnsmasq p910nd samba iproute2 iptables ppp pptp \
	nfs-utils portmap radvd ucdsnmp rp-l2tp igmpproxy vsftpd udpxy \
	ntpclient bpalogin bridge ez-ipupdate httpd infosvr jpeg-6b lib LPRng \
	misc netconf nvram others pppoe-relay rc rcamdmips sendmail \
	shared test upnp utils vlan wlconf www rt2460 libbcmcrypto asustrx
	cp iBox_title_all.jpg $(TOP)/www/asus/web_asus_en/graph/
	cp iBox_title_all_HDD.jpg $(TOP)/www/asus/web_asus_en/graph/
	cp iBox_title_all_550g.jpg $(TOP)/www/asus/web_asus_en/graph/
	tar -C $(TOP) -xjf others-parport.tar.bz2
	@echo
	@echo Sources prepared for compilation
	@echo

prep:
	@mkdir -p $(TOP)
	[ -f $(TOP)/Makefile ] || cp $(SRC)/Makefile $(TOP) && $(PATCHER) $(TOP) Makefile.diff

$(TOP)/.config: config
	$(MAKE) -C $(TOP) .config

$(ROOT)/lzma/SRC/7zip/Compress:
	@true

$(ROOT)/lzma: $(LZMA).zip $(ROOT)/lzma/SRC/7zip/Compress
	@rm -rf $@ && mkdir -p $@
	unzip -q $< -d $@
	$(PATCHER) $@ $(LZMA).patch

lzma: $(ROOT)/lzma
	@true

et:
	[ -d $(ROOT)/$(ET).orig ] || mv $(ROOT)/et $(ROOT)/$(ET).orig
	tar -C $(ROOT) -xzf $(ET).tar.gz
	$(PATCHER) $(ROOT)/et $(ET).patch

wl:
	[ -d $(ROOT)/$(WL).orig ] || mv $(ROOT)/wl $(ROOT)/$(WL).orig
	tar -C $(ROOT) -xjf $(WL).tar.bz2

brcm-shared:
	@cd brcm-src && $(PATCHER) -Z $(ROOT) brcm-src-shared.patch brcm-src-include.patch \
		brcm-src-5365.patch brcm-src-5365-robo.patch brcm-src-5354.patch \
		brcm-src-robo-tag.patch 

kernel-mrproper:
	$(MAKE) -C $(KERNEL_DIR) mrproper

kernel-patch:
	@echo Preparing kernel ...
	[ -d $(KERNEL_DIR)/arch/mips/bcm947xx ] || tar -C $(KERNEL_DIR) -xvjf brcm-boards.tar.bz2
	$(MAKE) -C $(KERNEL_DIR)/arch/mips/bcm947xx/compressed/ clean
	@$(PATCHER) -Z $(KERNEL_DIR) kernel/buildhost.patch
	$(MAKE) -C $(KERNEL_DIR) mrproper
	@echo Patching kernel...
	@$(PATCHER) -Z $(KERNEL_DIR) $(OPENWRT_Kernel_Patches)
	@$(PATCHER) -Z $(KERNEL_DIR) $(OPENWRT_Brcm_Patches)
	@$(PATCHER) -Z $(KERNEL_DIR) $(OUR_Kernel_Patches)
	tar -C $(KERNEL_DIR) -xvjf ov51x-1.65-1.12.tar.bz2
	tar -C $(KERNEL_DIR) -xvjf pwc-9.0.2.tar.bz2
	cp kernel/kernel.config $(KERNEL_DIR)/arch/mips/defconfig-bcm947xx

kernel: lzma et wl brcm-shared kernel-patch
	@true

asustrx:
	tar -C $(ROOT) -xjf asustrx.tar.bz2 

$(TOP)/loader: $(LOADER).tar.bz2
	@rm -rf $(TOP)/$(LOADER) $(TOP)/loader
	tar -xjf $^ -C $(TOP)
	mv $(TOP)/$(LOADER) $(TOP)/loader

loader: $(TOP)/loader
	@true

$(TOP)/busybox: busybox/$(BUSYBOX).tar.bz2
	@rm -rf $(TOP)/$(BUSYBOX) $(TOP)/busybox
	tar -xjf busybox/$(BUSYBOX).tar.bz2 -C $(TOP)
	mv $(TOP)/$(BUSYBOX)/e2fsprogs/old_e2fsprogs/* $(TOP)/$(BUSYBOX)/e2fsprogs/
	$(PATCHER) $(TOP)/$(BUSYBOX) busybox/$(BUSYBOX)-init.patch \
		busybox/$(BUSYBOX)-depmod.patch busybox/$(BUSYBOX)-tar.patch \
		busybox/$(BUSYBOX)-modprobe.patch \
		busybox/busybox.patch busybox/awk.patch \
		busybox/e2fsprogs.patch busybox/udhcp-options.patch \
		busybox/udhcp-vci.patch busybox/udhcp-secs.patch \
		busybox/cgi-env.patch busybox/http-host.patch \
		busybox/insmod.patch busybox/e2fsck-checkinterval.patch \
		busybox/ether-wake.patch 
	mkdir -p $(TOP)/$(BUSYBOX)/sysdeps/linux/
	cp busybox/busybox.config $(TOP)/$(BUSYBOX)/sysdeps/linux/defconfig
	mv $(TOP)/$(BUSYBOX) $(TOP)/busybox

busybox: $(TOP)/busybox
	@true

$(TOP)/vsftpd: vsftpd/$(VSFTPD).tar.gz
	@rm -rf $(TOP)/$(VSFTPD) $(TOP)/vsftpd
	tar -xzf vsftpd/$(VSFTPD).tar.gz -C $(TOP)
	$(PATCHER) $(TOP)/$(VSFTPD) vsftpd/$(VSFTPD).patch \
	vsftpd/$(VSFTPD)-passwd_file.patch vsftpd/$(VSFTPD)-utf8.patch \
	vsftpd/$(VSFTPD)-anon_root.patch vsftpd/$(VSFTPD)-log.patch \
	vsftpd/$(VSFTPD)-getpwnam.patch 
	mv $(TOP)/$(VSFTPD) $(TOP)/vsftpd

vsftpd: $(TOP)/vsftpd
	@true

$(TOP)/dropbear/config.h: dropbear/$(DROPBEAR).tar.bz2
	@rm -rf $(TOP)/$(DROPBEAR) $(TOP)/dropbear
	tar -xjf $^ -C $(TOP)
	$(PATCHER) $(TOP)/$(DROPBEAR) dropbear/$(DROPBEAR).patch \
	dropbear/$(DROPBEAR)-charo-io.patch \
	dropbear/$(DROPBEAR)-pipe.patch dropbear/$(DROPBEAR)-uninit-defect.patch 
	cd $(TOP)/$(DROPBEAR) && \
		CC=$(CC) CPPFLAGS="$(EXTRACFLAGS)" ./configure --host=mipsel-linux --disable-zlib --disable-shadow \
		--disable-lastlog --disable-utmp --disable-utmpx --disable-wtmp --disable-wtmpx \
		--disable-libutil --disable-loginfunc --disable-pututline --disable-pututxline
	mv $(TOP)/$(DROPBEAR) $(TOP)/dropbear

dropbear: $(TOP)/dropbear/config.h
	@true

$(TOP)/ucdsnmp: ucd-snmp/$(UCDSNMP).tar.gz
	@rm -rf $(TOP)/$(UCDSNMP) $(TOP)/ucdsnmp
	tar -xzf $^ -C $(TOP)
	$(PATCHER) $(TOP)/$(UCDSNMP) ucd-snmp/$(UCDSNMP).patch \
	ucd-snmp/ucd-snmp-3.6.2-interfaces.patch ucd-snmp/ucd-snmp-3.6.2-uptime.patch 
	mv $(TOP)/$(UCDSNMP) $(TOP)/ucdsnmp

$(TOP)/ucdsnmp/Makefile:
	cd $(TOP)/ucdsnmp && CC=$(CC) AR=$(AR) RANLIB=$(RANLIB) \
		ac_cv_CAN_USE_SYSCTL=no ac_cv_struct_ifnet_has_if_obytes=yes \
		ac_cv_struct_ifnet_has_if_ibytes=yes ac_cv_struct_ifnet_has_if_ibytes=yes \
		./configure --host=mipsel-linux --with-cflags="-g -O2 $(EXTRACFLAGS)" \
		--disable-debugging --without-opaque-special-types --with-out-mib-modules="v2party" \
		--with-sys-location=Unknown --with-sys-contact=Administrator --with-logfile=/var/log/snmpd.log

ucdsnmp: $(TOP)/ucdsnmp $(TOP)/ucdsnmp/Makefile
	@true

$(TOP)/iproute2: $(IPROUTE2).tar.gz
	@rm -rf $(TOP)/$@
	tar -xzf $^ -C $(TOP)
	patch -d $@ -p1 <$(IPROUTE2).patch && touch $@

iproute2: $(TOP)/iproute2
	@true

$(TOP)/dnsmasq: $(DNSMASQ).tar.gz
	@rm -rf $(TOP)/$(DNSMASQ) $@
	tar -xzf $^ -C $(TOP)
	$(PATCHER) $(TOP)/$(DNSMASQ) $(DNSMASQ).patch
	mv $(TOP)/$(DNSMASQ) $(TOP)/dnsmasq && touch $@

dnsmasq-diff: $(DNSMASQ).tar.gz
	@rm -rf $(TOP)/$(DNSMASQ)
	tar -xzf $^ -C $(TOP)
	-make -C $(TOP)/dnsmasq clean
	-(cd $(TOP) && diff -BurN $(DNSMASQ) dnsmasq) > $(DNSMASQ).patch

dnsmasq: $(TOP)/dnsmasq
	@true

$(TOP)/p910nd: $(P910ND).tar.bz2
	@rm -rf $(TOP)/$(P910ND) $@
	tar -xjf $^ -C $(TOP)
	$(PATCHER) $(TOP)/$(P910ND) $(P910ND).patch
	mv $(TOP)/$(P910ND) $@ && touch $@

p910nd-diff:
	@rm -rf $(TOP)/$(P910ND)
	tar -xjf $(P910ND).tar.bz2 -C $(TOP)
	-rm -f $(TOP)/p910nd/p910nd
	-cd $(TOP) && diff -BurN $(P910ND) p910nd > $(P910ND).patch

p910nd: $(TOP)/p910nd
	@true

$(TOP)/samba/source/Makefile: samba/$(SAMBA).tar.bz2
	@rm -rf $(TOP)/$(SAMBA) $@
	tar -xjf $^ -C $(TOP)
	$(PATCHER) $(TOP)/$(SAMBA) samba/$(SAMBA).patch \
	samba/$(SAMBA)-security.patch samba/$(SAMBA)-writex.patch \
	samba/$(SAMBA)-cp1251.patch samba/$(SAMBA)-utf8.patch \
	samba/$(SAMBA)-vista.patch samba/$(SAMBA)-2k3sp1-rpc.patch \
	samba/$(SAMBA)-nt-ioctl.patch samba/$(SAMBA)-mount.patch \
	samba/$(SAMBA)-gcc34.patch 
	tar xzvf samba/$(SAMBA)-codepages.tar.gz -C $(TOP)/$(SAMBA)
	mv $(TOP)/$(SAMBA) $(TOP)/samba
	cd $(TOP)/samba/source && \
		ac_cv_sizeof_int=4 ac_cv_sizeof_long=4 ac_cv_sizeof_short=2 \
		samba_cv_FTRUNCATE_NEEDS_ROOT=no samba_cv_HAVE_BROKEN_FCNTL64_LOCKS=no \
		samba_cv_HAVE_BROKEN_GETGROUPS=no samba_cv_HAVE_BROKEN_READDIR=no \
		samba_cv_HAVE_FCNTL_LOCK=yes samba_cv_HAVE_FNMATCH=yes \
		samba_cv_HAVE_FTRUNCATE_EXTEND=no samba_cv_HAVE_IFACE_AIX=no \
		samba_cv_HAVE_IFACE_IFCONF=yes samba_cv_HAVE_IFACE_IFREQ=yes \
		samba_cv_HAVE_INO64_T=yes samba_cv_HAVE_IRIX_SPECIFIC_CAPABILITIES=no \
		samba_cv_HAVE_OFF64_T=yes samba_cv_HAVE_ROOT=yes \
		samba_cv_HAVE_SECURE_MKSTEMP=yes samba_cv_HAVE_SHARED_MMAP=yes \
		samba_cv_HAVE_STRUCT_FLOCK64=yes samba_cv_HAVE_SYSV_IPC=no \
		samba_cv_HAVE_TRUNCATED_SALT=no samba_cv_HAVE_UNION_SEMUN=no \
		samba_cv_HAVE_UNSIGNED_CHAR=yes samba_cv_NEED_SGI_SEMUN_HACK=no \
		samba_cv_REPLACE_INET_NTOA=no samba_cv_SIZEOF_INO_T=4 \
		samba_cv_SIZEOF_OFF_T=4 samba_cv_SYSCONF_SC_NGROUPS_MAX=yes \
		samba_cv_USE_SETRESUID=no samba_cv_USE_SETREUID=yes \
		samba_cv_USE_SETEUID=yes samba_cv_USE_SETUIDX=no \
		samba_cv_have_longlong=yes samba_cv_have_setresgid=no \
		samba_cv_have_setresuid=no samba_cv_HAVE_GETTIMEOFDAY_TZ=yes \
		CPPFLAGS="-D_GNU_SOURCE -DNDEBUG -DSHMEM_SIZE=524288 -Dfcntl=fcntl64" CFLAGS="-Os $(EXTRACFLAGS)" \
		CC=$(CC) LD=$(LD) AR=$(AR) RANLIB=$(RANLIB) ./configure --host=mipsel-linux \
		--prefix=/usr --localstatedir=/var/log --libdir=/etc \
		--with-privatedir=/etc --with-lockdir=/var/lock --with-syslog

samba: $(TOP)/samba/source/Makefile
	@true

$(TOP)/iptables: $(IPTABLES).tar.bz2
	@rm -rf $(TOP)/$(IPTABLES) $@
	tar -xjf $^ -C $(TOP)
	$(PATCHER) $(TOP)/$(IPTABLES) $(IPTABLES).patch
	mv $(TOP)/$(IPTABLES) $@ && touch $@

iptables: $(TOP)/iptables
	@true

$(TOP)/nfs-utils/Makefile:
	@rm -rf $(TOP)/$(NFSUTILS) $(TOP)/nfs-utils
	tar -xzf $(NFSUTILS).tar.gz -C $(TOP)
	$(PATCHER) $(TOP)/$(NFSUTILS) $(NFSUTILS).patch $(NFSUTILS)-libnfs.patch 
	mv $(TOP)/$(NFSUTILS) $(TOP)/nfs-utils
	cd $(TOP)/nfs-utils && \
		CC=$(CC) LD=$(LD) AR=$(AR) RANLIB=$(RANLIB) ./configure --host=mipsel-linux \
		--enable-nfsv3 --disable-nfsv4 --disable-gss --disable-rquotad

nfs-utils: $(TOP)/nfs-utils/Makefile
	@true

$(TOP)/portmap: $(PORTMAP).tar.gz
	@rm -rf $(TOP)/$(PORTMAP) $(TOP)/portmap
	tar -xzf $^ -C $(TOP)
	$(PATCHER) $(TOP)/$(PORTMAP) $(PORTMAP).patch
	mv $(TOP)/$(PORTMAP) $(TOP)/portmap

portmap: $(TOP)/portmap
	@true

$(TOP)/radvd/Makefile:
	@rm -rf $(TOP)/$(RADVD) $(TOP)/radvd
	tar -xzf $(RADVD).tar.gz -C $(TOP)
	$(PATCHER) $(TOP)/$(RADVD) $(RADVD).patch
	mv $(TOP)/$(RADVD) $(TOP)/radvd
	cd $(TOP)/radvd && \
		CC=$(CC) LD=$(LD) AR=$(AR) RANLIB=$(RANLIB) ./configure --host=mipsel-linux \
		--prefix=""

radvd: $(TOP)/radvd/Makefile
	@true

$(TOP)/rc/Makefile:
	tar -C $(SRC) -cf - rc | tar -C $(TOP) -xf -
	$(PATCHER) -Z $(TOP) rc/rc-common.patch \
	    rc/rc-ddns-daemon.patch rc/rc-masq2snat.patch \
	    rc/rc-ppp-connected.patch
	$(MAKE) -C $(TOP)/rc clean

rc: $(TOP)/rc/Makefile
	@true

$(TOP)/rp-l2tp/Makefile:
	@rm -rf $(TOP)/$(L2TP) $(TOP)/rp-l2tp
	tar -xzf rp-l2tp/$(L2TP).tar.gz -C $(TOP)
	$(PATCHER) $(TOP)/$(L2TP) rp-l2tp/$(L2TP).patch rp-l2tp/$(L2TP)-persist.patch \
		    rp-l2tp/$(L2TP)-route.patch rp-l2tp/$(L2TP)-log.patch \
		    rp-l2tp/$(L2TP)-peername.patch rp-l2tp/$(L2TP)-debug.patch 
	mv $(TOP)/$(L2TP) $(TOP)/rp-l2tp
	cd $(TOP)/rp-l2tp && \
		CC=$(CC) LD=$(LD) AR=$(AR) RANLIB=$(RANLIB) CFLAGS="-g -O2 $(EXTRACFLAGS)" \
		./configure --host=mipsel-linux --prefix=/usr

rp-l2tp: $(TOP)/rp-l2tp/Makefile
	@true

ppp_Patches := $(call patches_list,ppp)

$(TOP)/ppp: ppp/$(PPP).tar.bz2
	@rm -rf $(TOP)/$(PPP) $@
	tar -xjf $^ -C $(TOP)
	$(PATCHER) $(TOP)/$(PPP) $(ppp_Patches)
	mv $(TOP)/$(PPP) $@ && touch $@

$(TOP)/ppp/Makefile: $(TOP)/ppp
	cd $^ && ./configure --prefix=/usr --sysconfdir=/tmp

ppp: $(TOP)/ppp/Makefile
	@true

$(TOP)/igmpproxy/src/Makefile:
	@rm -rf $(TOP)/igmpproxy
	tar -xzf igmpproxy/$(IGMPPROXY).tar.gz -C $(TOP)
	$(PATCHER) $(TOP)/igmpproxy igmpproxy/$(IGMPPROXY)-zeroaltnet.patch \
	igmpproxy/$(IGMPPROXY)-timer.patch igmpproxy/$(IGMPPROXY)-syslog.patch \
	igmpproxy/$(IGMPPROXY)-local.patch igmpproxy/$(IGMPPROXY)-disabled.patch 

igmpproxy: $(TOP)/igmpproxy/src/Makefile
	@true

$(TOP)/pptp: pptp/$(PPTP).tar.gz
	@rm -rf $(TOP)/$(PPTP) $@
	tar -xzf $^ -C $(TOP)
	$(PATCHER) $(TOP)/$(PPTP) pptp/$(PPTP).patch pptp/$(PPTP)-route.patch \
	pptp/$(PPTP)-n_hdlc.patch pptp/$(PPTP)-writev.patch 
	mv $(TOP)/$(PPTP) $@ && touch $@

pptp: $(TOP)/pptp
	@true

$(TOP)/udpxy: $(UDPXY).tgz
	@rm -rf $(TOP)/udpxy-wl $@
	tar -xzf $^ -C $(TOP)
	[ ! -f $(UDPXY).patch ] || $(PATCHER) $(TOP)/udpxy-wl $(UDPXY).patch
	mv $(TOP)/$(UDPXY) $@ && touch $@

udpxy: $(TOP)/udpxy
	@true

$(TOP)/ntpclient: $(NTPCLIENT).tar.bz2
	@rm -rf $(TOP)/$(NTPCLIENT) $@
	tar -xjf $^ -C $(TOP)
	[ ! -f $(NTPCLIENT).patch ] || $(PATCHER) $(TOP)/$(NTPCLIENT) $(NTPCLIENT).patch
	mv $(TOP)/$(NTPCLIENT) $@ && touch $@

ntpclient: $(TOP)/ntpclient
	@true

libbcmcrypto: $(LIBBCMCRYPTO).tar.gz
	tar -zxf $^ -C $(TOP)
	$(PATCHER) $(TOP)/libbcmcrypto $(LIBBCMCRYPTO).patch

wlconf: $(WLCONF).tar.gz
	tar -zxf $^ -C $(TOP)

upnp:
	[ ! -d $(SRC)/../tools/$@ ] || [ -d $(TOP)/$@ ] || \
		tar -C $(SRC)/../tools -cf - $@ | tar -C $(TOP) -xf -
	[ ! -f $@.diff ] || $(PATCHER) $(TOP) $@.diff

upnp-diff:
	(cd .. && diff -BurN tools/upnp gateway/upnp | grep -v ^Binary.*differ$$) > upnp.diff
	diffstat upnp.diff

%:
	[ ! -d $(SRC)/$* ] || [ -d $(TOP)/$* ] || \
		tar -C $(SRC) -cf - $* | tar -C $(TOP) -xf -
	[ ! -f $*.diff ] || $(PATCHER) -Z $(TOP) $*.diff
	[ ! -f $*.patch ] || patch -d $(TOP) -d $* -p1 --no-backup-if-mismatch -Z < $*.patch
	[ ! -f $(TOP)/$*/Makefile ] || $(MAKE) -C $(TOP)/$* clean

www-diff:
	(cd .. && diff -BurN router/www/asus/web_asus_en gateway/www/asus/web_asus_en | grep -v ^Binary.*differ$$) > www.diff
	(cd .. && diff -BuN router/www/asus/ gateway/www/asus/ | grep -v ^Binary.*differ$$) >> www.diff
	diffstat www.diff

%-diff:
	(cd .. && diff -BurpN -x*.o router/$* gateway/$* | grep -v "^Files .* differ$$") > $*.diff
	diffstat $*.diff


