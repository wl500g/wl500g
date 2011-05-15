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

VPATH.h=.:..:../../include:$(SRCBASE)/include:$(TOP)/shared

vpath %.c .:..:$(TOP)/shared:$(SRCBASE)/shared/netconf
vpath %.h $(VPATH.h)

CFLAGS += -I$(subst :, -I,$(VPATH.h))
OBJDIR=obj

SOURCES = upnp.c ssdp.c http.c gena.c soap.c \
	  service.c device.c  \
	  linux_osl.c uio.c

SOURCES.OBJ := $(SOURCES)
SOURCES.OBJ := $(patsubst %.c,$(OBJDIR)/%.o,$(SOURCES.OBJ))

vpath %.c .:..:
vpath %.h .:..:../../include

TARGET = libupnp.a
LIBS = 

all : $(OBJDIR) $(TARGET)

$(OBJDIR):
	mkdir -p $(OBJDIR)

$(SOURCES.OBJ): upnp.h

$(OBJDIR)/%.o:	%.c
	$(CC) $(CFLAGS) $(INCLUDES) -o $@ -c $<


$(TARGET) : $(SOURCES.OBJ)
	$(AR) -rcs $(TARGET) $(SOURCES.OBJ) 
	$(RANLIB) $(TARGET)
	$(SIZE)  $(TARGET)

clean:
	rm -rf obj

distclean:
	rm -f $(TARGET)

timertest: $(OBJDIR)/linux_timer.o $(OBJDIR)/timertest.o
	$(CC) $(OBJDIR)/linux_timer.o $(OBJDIR)/timertest.o -o timertest
