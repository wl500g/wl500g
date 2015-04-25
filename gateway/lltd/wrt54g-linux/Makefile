VPATH = ../src

CROSS = ~/openwrt/staging_dir_mipsel

CC = $(CROSS)/bin/mipsel-linux-uclibc-gcc
LD = $(CC)
CFLAGS = -I$(CROSS)/include -I$(CROSS)/usr/include -O3 -pipe -mips32 -mtune=mips32 -Wall -Wno-unused -s -g -fpack-struct -Wpadded
LDFLAGS = -L~/openwrt/staging_dir_mipsel/usr/lib

OS_LAYER = osl-linux.c

all: lld2d

include ../src/common.mk

depend:
	$(CC) $(CFLAGS) -M $(DCFILES) >.depend

-include .depend
