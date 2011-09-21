#CROSS ?= mipsel-uclibc-
CC ?= $(CROSS)gcc
STRIP ?= $(CROSS)strip

CCFLAGS+=-Os -Wall -I. $(EXTRACFLAGS)
LDFLAGS+=-s
LIBS=md5.o rmd160.o

lanauth: lanauth.o $(LIBS)
	$(CC) $(CCFLAGS) $(LDFLAGS) lanauth.o -o lanauth $(LIBS)

%.o: %.c
	$(CC) $(CCFLAGS) -c $<

clean:
	rm *.o lanauth

install: lanauth
	install -D lanauth $(INSTALLDIR)/usr/sbin/lanauth
	$(STRIP) $(INSTALLDIR)/usr/sbin/lanauth
