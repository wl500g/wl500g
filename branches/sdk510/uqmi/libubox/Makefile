#CROSS=mipsel-uclibc-
CC ?= $(CROSS)gcc
LD ?= $(CROSS)ld
AR ?= $(CROSS)ar
RANLIB ?= $(CROSS)ranlib

CFLAGS  := --std=gnu99 -g -Os $(EXTRACFLAGS)
CFLAGS  += -Wall -Werror -Wmissing-declarations
CFLAGS  += -ffunction-sections -fdata-sections
LDFLAGS += -Wl,--gc-sections

TARGET = libubox.a
SOURCES = avl.c avl-cmp.c blob.c blobmsg.c uloop.c usock.c ustream.c ustream-fd.c vlist.c utils.c

OBJS=$(SOURCES:.c=.o)

# Uncomment next line in case of clock_gettime() in librt
#LIBS=-lrt

PREFIX=/usr

default_target: all
.PHONY : default_target

install: all
	$(INSTALL) -D $(TARGET) $(INSTALLDIR)/$(PREFIX)/lib/$(TARGET)
.PHONY : install

clean:
	rm $(OBJS) $(TARGET)
.PHONY : clean

# The main all target
all: $(TARGET)
.PHONY : all

# Build rule for targets
$(TARGET): $(OBJS)
	$(AR) cr $@ $(OBJS)
	$(RANLIB) $@

.c.o::
	$(CC) $(CFLAGS) -c $<

# Dependencies
avl-cmp.o: avl-cmp.c avl-cmp.h
avl.o: avl.c avl.h
blob.o: blob.c blob.h
blobmsg.o: blobmsg.c blobmsg.h
uloop.o: uloop.c uloop.h
usock.o: usock.c usock.h
ustream-fd.o: ustream-fd.c 
ustream.o: ustream.c ustream.h
utils.o: utils.c utils.h
vlist.o: vlist.c vlist.h
