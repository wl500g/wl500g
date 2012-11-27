#
# Generic portion of the Broadcom wl driver makefile
#
# input: O_TARGET, CONFIG_WL_CONF and wl_suffix
# output: obj-m, obj-y
#
# $Id: wl_generic.mk,v 1.3.2 2011/09/14 21:46:00 Exp $
#

REBUILD_WL_MODULE=$(shell if [ -d "$(src)/$(SRCBASE)/wl/sys" -a "$(REUSE_PREBUILT_WL)" != "1" ]; then echo 1; else echo 0; fi)

# If source directory (src/wl/sys) exists and REUSE_PREBUILT_WL is undefined, 
# then build inside $(SRCBASE)/wl/sys, otherwise use pre-builts
ifeq ($(REBUILD_WL_MODULE),1)

    # Get the source files and flags from the specified config file
    # (Remove config's string quotes before trying to use the file)
    ifeq ($(CONFIG_WL_CONF),)
         $(error var_vlist($(VLIST)) var_config_wl_use($(shell env|grep CONFIG_WL_USE)))
         $(error CONFIG_WL_CONF is undefined)
    endif
    
    WLCONFFILE := $(strip $(subst ",,$(CONFIG_WL_CONF))) 
    WLCFGDIR   := $(src)/$(SRCBASE)/wl/config
    
    # define OS flag to pick up wl osl file from wl.mk
    WLLX=1
    include $(WLCFGDIR)/$(WLCONFFILE)
    include $(WLCFGDIR)/wl.mk
    
    ifeq ($(WLFILES),)
         $(error WLFILES is undefined in $(WLCFGDIR)/$(WLCONFFILE))
    endif
    
    ifeq ("$(CONFIG_WL_EMULATOR)","y") 
         WLFILES += wl_bcm57emu.c
    endif
    
ifeq ($(WL_PARTIAL_REBUILD),1)
    WL_SOURCE   := wl_linux.c
    ifeq ($(WLLXIW),1)
    	WL_SOURCE += wl_iw.c
    endif
else
    WL_SOURCE := $(WLFILES)
endif

    WL_DFLAGS := $(WLFLAGS)
    # If source directory (src/wl/sys) exists, use sources to build objects
    WL_OBJS   := $(foreach file, $(WL_SOURCE), \
		 $(if $(wildcard $(src)/$(SRCBASE)/wl/sys/$(file)), \
		 $(addprefix $(SRCBASE)/wl/sys/, $(patsubst %.c,%.o,$(file)))))
    WL_OBJS   += $(foreach file, $(WL_SOURCE), \
		 $(if $(wildcard $(src)/$(SRCBASE)/bcmcrypto/$(file)), \
		 $(addprefix $(SRCBASE)/bcmcrypto/, $(patsubst %.c,%.o,$(file)))))
    WL_OBJS   += $(foreach file, $(WL_SOURCE), \
		 $(if $(wildcard $(src)/$(SRCBASE)/shared/$(file)), \
		 $(addprefix $(SRCBASE)/shared/, $(patsubst %.c,%.o,$(file)))))
    WL_OBJS   += $(foreach file, $(WL_SOURCE), \
		 $(if $(wildcard $(src)/$(SRCBASE)/bcmsdio/sys/$(file)), \
		 $(addprefix $(SRCBASE)/bcmsdio/sys/, $(patsubst %.c,%.o,$(file)))))
    
    # need -I. to pick up wlconf.h in build directory
    
    EXTRA_CFLAGS += -DDMA $(WL_DFLAGS) -I$(src) -I$(src)/.. -I$(src)/$(SRCBASE)/wl/linux \
		    -I$(src)/$(SRCBASE)/wl/sys -finline-limit=2048
    
    # If the PHY_HAL flag is defined we look in directory wl/phy for the
    # phy source files.
    ifneq ($(findstring PHY_HAL,$(WL_DFLAGS)),)
        WL_OBJS   += $(foreach file, $(WL_SOURCE), \
		     $(if $(wildcard $(src)/$(SRCBASE)/wl/phy/$(file)), \
		     $(addprefix $(SRCBASE)/wl/phy/, $(patsubst %.c,%.o,$(file)))))
        EXTRA_CFLAGS += -I$(src)/$(SRCBASE)/wl/phy
    endif

	UPDATESH   := $(WLCFGDIR)/diffupdate.sh

	WLTUNEFILE ?= wltunable_lx_router.h

    # wl-objs is for linking to wl.o
ifeq ($(WL_PARTIAL_REBUILD),1)
    $(TARGET)-objs := $(SRCBASE)/wl/linux/wl_$(wl_suffix)_prebuilt.o
else
    $(TARGET)-objs := $(WLCONF_O)

$(obj)/$(WLCONF_O): $(obj)/$(WLCONF_H)

$(obj)/$(WLCONF_H): $(WLCFGDIR)/$(WLTUNEFILE) FORCE
	[ ! -f $@ ] || chmod +w $@
	@echo "check and update config file"
	@echo $(if $(VLIST),"VLIST          = $(VLIST)")
	@echo "CONFIG_WL_CONF = $(CONFIG_WL_CONF)"
	@echo "WLTUNEFILE     = $(WLTUNEFILE)"
	cp $< wltemp
	$(UPDATESH) wltemp $@

endif # partial rebuild

    $(TARGET)-objs += $(WL_OBJS)

else # SRCBASE/wl/sys doesn't exist

    # Otherwise, assume prebuilt object module(s) in src/wl/linux directory
    prebuilt := wl_$(wl_suffix).o
    $(TARGET)-objs := $(SRCBASE)/wl/linux/$(prebuilt)

endif

obj-$(CONFIG_WL) := $(TARGET).o


FORCE:

clean-files += $(SRCBASE)/wl/sys/*.o $(SRCBASE)/wl/phy/*.o $(SRCBASE)/wl/sys/.*.*.cmd $(SRCBASE)/wl/phy/.*.*.cmd $(WLCONF_H) $(WLCONF_O)
