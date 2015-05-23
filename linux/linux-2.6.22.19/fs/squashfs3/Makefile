#
# Makefile for the linux squashfs routines.
#

obj-$(CONFIG_SQUASHFS) += squashfs3.o
squashfs3-objs := inode.o
squashfs3-objs += uncomp.o
squashfs3-objs += LzmaDecode.o

LZMADIR := $(src)/../../../../lzma4xx/C/Compress/Lzma
EXTRA_CFLAGS = -I$(LZMADIR)
$(obj)/LzmaDecode.o: $(LZMADIR)/LzmaDecode.c FORCE
	$(call cmd,force_checksrc)
	$(call if_changed_rule,cc_o_c)
