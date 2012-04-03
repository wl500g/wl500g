CFLAGS=-Wall -Os -I$(LINUXDIR)/include $(EXTRACFLAGS)
ifeq ($(CONFIG_LINUX_26),y)
CFLAGS += -DHAVE_SG_IO
endif

DESTDIR = /usr/local/sbin


TARGETS = scsi-start

all: $(TARGETS)

clean:
	rm -f *.o $(TARGETS) *~

install:	all
	install -d $(DESTDIR)
	install -m 755 scsi-start $(DESTDIR)
	$(STRIP) $(DESTDIR)/scsi-start
	ln -sf scsi-start $(DESTDIR)/scsi-stop
#	install -m 755 scsi-idle $(DESTDIR)

uninstall:
#	$(RM) $(DESTDIR)/scsi-idle
	$(RM) $(DESTDIR)/scsi-stop $(DESTDIR)/scsi-start

dist: clean
	@(dir=`pwd`; name=`basename $$dir`; \
	rm -f $$name.tar.gz ; \
	cd .. ; tar -zcf $$name.tar.gz $$name ; mv $$name.tar.gz $$name/ )
