
#CFLAGS += 


OBJS += nvports.o nvcommons5.o



#Other config specific defines 

ifeq ($(CONFIG_PPTP_CLIENT),y)
OBJS += nvpptp.o
endif
ifeq ($(CONFIG_IPSEC_FREESWAN),y)
OBJS += nvipsec.o
endif
