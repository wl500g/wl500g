CFLAGS=-Wall -O2 -I$(LINUXDIR)/include $(EXTRACFLAGS)
ifeq ($(CONFIG_LINUX_26),y)
CFLAGS += -DHAVE_SG_IO
endif

TARGETS = scsi-start scsi-stop

all: $(TARGETS)

scsi-stop:	scsi-start
	ln -s scsi-start scsi-stop

clean:
	rm -f *.o $(TARGETS) *~

install:	all
	install -m 755 scsi-start /usr/local/sbin
	install -m 755 scsi-stop /usr/local/sbin
	install -m 755 scsi-idle /usr/local/sbin

uninstall:
	$(RM) /usr/local/sbin/scsi-idle
	$(RM) /usr/local/sbin/scsi-stop
	$(RM) /usr/local/sbin/scsi-start

dist: clean
	@(dir=`pwd`; name=`basename $$dir`; \
	rm -f $$name.tar.gz ; \
	cd .. ; tar -zcf $$name.tar.gz $$name ; mv $$name.tar.gz $$name/ )
