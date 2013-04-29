#
# Broadcom web pages
#
MODEL := WL500gp

ifneq ($(MAKECMDGOALS),clean)
include $(TOP)/.config
export CONFIG_IPV6 CONFIG_MADWIMAX CONFIG_MODEM CONFIG_LLTD CONFIG_NTFS3G
endif


all: web_tmp

web_tmp:
	./mkweb tmp $(MODEL)

install:
	rm -rf $(INSTALLDIR)/www && install -d $(INSTALLDIR)/www
	install -m 644 web_tmp/*.asp $(INSTALLDIR)/www
	install -m 644 web_tmp/*.inc $(INSTALLDIR)/www
	install -m 644 web_tmp/*.js  $(INSTALLDIR)/www
	install -m 644 web_tmp/*.css $(INSTALLDIR)/www
	install -m 644 web_tmp/*.svg $(INSTALLDIR)/www
	install -m 644 web_tmp/*.ico $(INSTALLDIR)/www
	install -d $(INSTALLDIR)/www/graph
	install -m 644 web_tmp/graph/* $(INSTALLDIR)/www/graph

clean:
	rm -rf web_tmp
