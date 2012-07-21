#CROSS=mipsel-uclibc-
CC ?= $(CROSS)gcc
LD ?= $(CROSS)ld
AR ?= $(CROSS)ar
RANLIB ?= $(CROSS)ranlib
STRIP ?= $(CROSS)strip

CCFLAGS += -g -O2 -Wall -I. -Wno-pointer-sign -fomit-frame-pointer
CCFLAGS += -I$(TOP)/shared -I$(SRCBASE)/include $(EXTRACFLAGS)
LDFLAGS += -L$(TOP)/nvram -lnvram -L$(TOP)/shared -lshared
LIBS=

PACKAGE=infosrv
OBJS=$(PACKAGE).o model.o

all: $(PACKAGE)

$(PACKAGE): $(PACKAGE).o $(OBJS)
	$(CC) $(CCFLAGS) $(LDFLAGS) $(OBJS) -o $(PACKAGE) $(LIBS)

%.o: %.c
	$(CC) $(CCFLAGS) -c $<

clean:
	rm *.o $(PACKAGE)

install: all
	install -d $(INSTALLDIR)/usr/sbin
	install $(PACKAGE) $(INSTALLDIR)/usr/sbin
	$(STRIP) $(INSTALLDIR)/usr/sbin/$(PACKAGE)
