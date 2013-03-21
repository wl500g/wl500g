#
# Miscellaneous utilities Makefile
#

UTILS := wl nas eapd mii-tool robocfg gpio hub-ctrl

CFLAGS += -s -Os -I$(SRCBASE)/include -I$(TOP)/shared -isystem$(LINUXDIR)/include -Wall $(EXTRACFLAGS)
LDFLAGS += -Wl,-rpath /lib $(EXTRALDFLAGS)

#vpath %.c $(SRCBASE)/tools/misc

ifneq ($(wildcard $(SRCBASE)/et/linux/et.c),)
UTILS += et
vpath %.c $(SRCBASE)/et/linux
endif


all: $(UTILS)


WL_BASE:=$(SRCBASE)/wl/mipsel-uclibc

ifneq ($(wildcard $(WL_BASE)/wl_exe.o),)
wl: $(WL_BASE)/wl_exe.o
	$(CC) -s $(LDFLAGS) -o $@ $<
else
wl: $(WL_BASE)/wl
	cp -p $< $@
endif

ifneq ($(wildcard $(WL_BASE)/nas_exe.o),)
nas: $(WL_BASE)/nas_exe.o
	$(CC) -s $(LDFLAGS) -o $@ -L$(TOP)/shared -lshared -L$(TOP)/nvram -lnvram -L$(TOP)/libbcmcrypto -lbcmcrypto $<
else
nas: $(WL_BASE)/nas
	cp -p $< $@
endif

ifneq ($(wildcard $(WL_BASE)/eapd_exe.o),)
eapd: $(WL_BASE)/eapd_exe.o
	$(CC) -s $(LDFLAGS) -o $@ -L$(TOP)/shared -lshared -L$(TOP)/nvram -lnvram $<
else
eapd: $(WL_BASE)/eapd
	cp -p $< $@
endif

mii-tool: mii-tool.o
mii-tool.o: mii-tool.c mii.h

robocfg: robocfg.o
robocfg.o: robocfg.c etc53xx.h

gpio: gpio.o
	$(CC) $(LDFLAGS) -o $@ -L$(TOP)/shared -lshared -L$(TOP)/nvram -lnvram $<

gpio.o: gpio.c
	$(CC) -c $(CFLAGS) -DHAVE_GETMODEL -o $@ $<

hub-ctrl: hub-ctrl.o
	$(CC) $(LDFLAGS) -o $@ -L$(TOP)/libusb10/libusb/.libs -lusb-1.0 $<

hub-ctrl.o: hub-ctrl.c
	$(CC) -c $(CFLAGS) -o $@ -I$(TOP)/libusb10/libusb -I$(TOP)/libusb10/ $<

clean:
	rm -f *.o *~ $(UTILS)

install: all
	install -d $(INSTALLDIR)/usr/sbin
	install $(UTILS) $(INSTALLDIR)/usr/sbin
	$(STRIP) $(foreach file,$(UTILS),$(INSTALLDIR)/usr/sbin/$(file))
	ln -sf nas $(INSTALLDIR)/usr/sbin/nas4not
	ln -sf gpio $(INSTALLDIR)/usr/sbin/pwrled
