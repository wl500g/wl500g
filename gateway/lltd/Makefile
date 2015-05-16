VPATH = src

-include $(TOP)/.config

CFLAGS += -I. -I$(TOP)/shared -I$(SRCBASE)/include
#CFLAGS += -g -D__DEBUG__
CFLAGS += -Os -pipe -Wall -Wno-unused
CFLAGS += $(EXTRACFLAGS)
CFLAGS += -DBCM_WIRELESS $(if $(CONFIG_IPV6),-DUSE_IPV6,)
LDFLAGS += -L$(TOP)/nvram -lnvram -L$(TOP)/shared -lshared
LDFLAGS += $(EXTRALDFLAGS)

OS_LAYER = osl-linux.c

all: lld2d

include src/common.mk

depend:
	$(CC) $(CFLAGS) -M $(DCFILES) >.depend

-include .depend
