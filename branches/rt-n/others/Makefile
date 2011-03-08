
ifneq ($(MAKECMDGOALS),clean)
 ifneq ($(wildcard $(TOP)/.config),)
  include $(TOP)/.config
 endif
endif

all :
	rm -rf $(INSTALLDIR)

install: all
	install -d $(INSTALLDIR)/usr/etc
	install -d $(INSTALLDIR)/sbin
	install -d $(INSTALLDIR)/usr/sbin
	install -d $(INSTALLDIR)/usr/bin
	install -d $(INSTALLDIR)/usr/share/empty
	install -d $(INSTALLDIR)/lib
	install -d $(INSTALLDIR)/www

	install -m 644 services $(INSTALLDIR)/usr/etc
	install -m 644 protocols $(INSTALLDIR)/usr/etc
	install -m 644 rpc $(INSTALLDIR)/usr/etc
	install -m 644 profile $(INSTALLDIR)/usr/etc
	install -m 644 ipkg.conf $(INSTALLDIR)/usr/etc
	install -m 644 mdev.conf $(INSTALLDIR)/usr/etc
	
	ln -sf /tmp/settings $(INSTALLDIR)/www/WL500g.Premium.cfg
	ln -sf /tmp/settings $(INSTALLDIR)/www/WL700g.cfg
	ln -sf /tmp/settings $(INSTALLDIR)/www/WL500W.cfg
	ln -sf /tmp/settings $(INSTALLDIR)/www/WL550gE.cfg
	ln -sf /tmp/flash.tar.gz $(INSTALLDIR)/www/WL500g.Premium.tar.gz
	ln -sf /tmp/flash.tar.gz $(INSTALLDIR)/www/WL500W.tar.gz
	ln -sf /tmp/flash.tar.gz $(INSTALLDIR)/www/WL550gE.tar.gz
	ln -sf /var/tmp/display.jpg $(INSTALLDIR)/www/display.jpg

	install -m 755 linuxrc.sh $(INSTALLDIR)/linuxrc
	install -m 755 flashfs.sh $(INSTALLDIR)/sbin/flashfs
	install -m 755 wshaper.sh $(INSTALLDIR)/sbin/wshaper
	install -m 755 ipkg.sh $(INSTALLDIR)/usr/bin/ipkg.sh
ifeq ($(CONFIG_SSHD),y)
	install -m 755 dropbearstart.sh $(INSTALLDIR)/usr/sbin/dropbearstart
endif
ifeq ($(CONFIG_MADWIMAX),y)
	install -m 755 wimax_check_connection.sh $(INSTALLDIR)/usr/sbin/wimax_check_connection.sh
endif

clean:
