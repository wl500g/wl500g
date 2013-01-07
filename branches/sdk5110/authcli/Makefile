#CROSS=mipsel-uclibc-
CC ?= $(CROSS)gcc
STRIP ?= $(CROSS)strip

CCFLAGS=-Os -Wall -I. $(EXTRACFLAGS)
LDFLAGS=-s
LIBS=globals.o sha1.o


authcli: authcli.o $(LIBS)
	$(CC) $(CCFLAGS) $(LDFLAGS) authcli.o -o authcli $(LIBS)

%.o: %.c
	$(CC) $(CCFLAGS) -c $<

clean:
	rm -f *.o authcli

install: authcli
	install -D authcli $(INSTALLDIR)/usr/sbin/authcli
	$(STRIP) $(INSTALLDIR)/usr/sbin/authcli
