
SWAPIDIR = ../../switch/sys
SWLIBDIR = ../../switch/swlib
SWMODDIR = ../../switch/swmod
#CFLAGS += -g -D_GNU_SOURCE -I$(SWAPIDIR) -I$(SWMODDIR) -I../../linux/linux/include -DBCM5365
#CFLAGS += -g -D_GNU_SOURCE -I$(SWAPIDIR) -I$(SWMODDIR) -DBCM5365 -DCONFIG_SENTRY5
CFLAGS +=  -I$(SWAPIDIR) -I$(SWMODDIR) -DBCM5365 -DCONFIG_SENTRY5

ifneq ($(wildcard $(TOP)/../switch/swmod/swmod.c),)
LDFLAGS += -L$(SWLIBDIR)   -lsw
else
LDFLAGS += -L$(SWLIBDIR)/$(PLATFORM)   -lsw
endif
LDFLAGS +=  -ldl


OBJS += rcs5.o vlan.o switch.o  


#Other config specific defines 

ifeq ($(CONFIG_PPTP_CLIENT),y)
CFLAGS += -DCONFIG_PPTP_CLIENT
OBJS += pptp.o
endif
ifeq ($(CONFIG_IPSEC_FREESWAN),y)
CFLAGS += -DCONFIG_IPSEC_FREESWAN
OBJS += ipsec.o
endif
