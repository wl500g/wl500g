#
# Makefile for the linux squashfs routines.
#

obj-$(CONFIG_SQUASHFS3) += squashfs3.o

LZMADIR := ../../../../lzma4xx/C/Compress/Lzma
LZMA_OBJS := $(addprefix $(LZMADIR)/, LzmaDecode.o)

squashfs3-objs := squashfs.o
squashfs-objs := inode.o
squashfs-objs += uncomp.o
squashfs-objs += $(LZMA_OBJS)

EXTRA_CFLAGS = -I$(src)/$(LZMADIR)

clean_cmds := $(foreach f,$(LZMA_OBJS),$(dir $(f)).$(notdir $(f)).cmd)
clean-files := $(LZMA_OBJS) $(clean_cmds)
