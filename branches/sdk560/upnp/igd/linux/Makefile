#
# Copyright 2004, Broadcom Corporation
# All Rights Reserved.
# 
# THIS SOFTWARE IS OFFERED "AS IS", AND BROADCOM GRANTS NO WARRANTIES OF ANY
# KIND, EXPRESS OR IMPLIED, BY STATUTE, COMMUNICATION OR OTHERWISE. BROADCOM
# SPECIFICALLY DISCLAIMS ANY IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS
# FOR A SPECIFIC PURPOSE OR NONINFRINGEMENT CONCERNING THIS SOFTWARE.
#
# $Id$
#

SRCBASE=../../../..

VPATH.h = .:..:../../include:$(SRCBASE)/include:$(TOP)/shared

vpath %.c .:..:$(SRCBASE)/shared/netconf
vpath %.h $(VPATH.h)


CFLAGS += -I$(subst :, -I,$(VPATH.h))
OBJDIR=obj

# compile in the generic UPNP IGD files.
#
SOURCES = igd.c igd_desc.c wancommon.c wanipc.c ipt.c \
	  layer3.c  \
	  x_wanpppconnection.c wanppp.c \
	  mapmgr.c

# Add the OS-specific source files.
SOURCES += linux_main.c linux_net.c linux_igd.c


SOURCES.TEST = test.c shutils.c 

SOURCES.OBJ := $(SOURCES)
SOURCES.OBJ := $(patsubst %.c,$(OBJDIR)/%.o,$(SOURCES.OBJ))

TARGET = upnp
LIBS = -L../../upnp/linux -L$(TOP)/netconf \
	-L$(TOP)/nvram -L$(TOP)/shared -lupnp -lnetconf -lnvram -lshared

all : $(OBJDIR) $(TARGET)

$(OBJDIR):
	mkdir -p $(OBJDIR)

wanipc.c : wanipc.h

ipt.c : wanipc.h

$(SOURCES.OBJ): upnp.h

$(OBJDIR)/%.o:	%.c
	$(CC) $(CFLAGS) $(INCLUDES) -o $@ -c $<

# uncomment BUILD_LIBS if you want this makefile to build the netconf and itables libraries.
#BUILD_LIBS=1

ifneq ($(BUILD_LIBS),)

$(TARGET) :: libnetconf.so

$(TARGET) :: $(TOP)/iptables/libiptables.a 

$(TARGET) :: $(TOP)/nvram/libnvram.a 

$(TOP)/iptables/libiptables.a : FORCE
	$(MAKE) -C $(TOP)/iptables SRCBASE=../.. TOP=..
$(TOP)/nvram/libnvram.a : FORCE
	$(MAKE) -C $(TOP)/nvram SRCBASE=../.. TOP=..

$(TOP)/netconf/libnetconf.so : $(TOP)/iptables/libiptables.a FORCE
	$(MAKE) -C $(TOP)/netconf SRCBASE=../.. TOP=..

libnetconf.so : $(TOP)/netconf/libnetconf.so
	#	cp $(TOP)/netconf/libnetconf.so /router/usr/lib/

endif


../../upnp/linux/libupnp.a : FORCE
	$(MAKE) -C ../../upnp/linux

$(TARGET) :: ../../upnp/linux/libupnp.a  $(SOURCES.OBJ)
	$(CC) -o $(TARGET) $(SOURCES.OBJ) $(LIBS)
	$(SIZE)  $(TARGET)

clean:
	rm -rf obj
	rm -f upnp
	$(MAKE) -C ../../upnp/linux clean
ifneq ($(BUILD_LIBS),)
	$(MAKE) -C $(TOP)/netconf clean
	$(MAKE) -C $(TOP)/iptables clean
	$(MAKE) -C $(TOP)/nvram clean
endif

distclean:
	$(MAKE) -C ../../upnp/linux distclean

fptest: fptest.o
	$(CC) -o fptest fptest.o $(LIBS)

btest: btest.o
	$(CC) -o btest btest.o

FORCE:

.PHONY: FORCE clean libs
