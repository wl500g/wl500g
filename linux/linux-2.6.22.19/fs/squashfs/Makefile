#
# Makefile for the linux squashfs routines.
#

obj-$(CONFIG_SQUASHFS) += squashfs.o

LZMADIR = ../../../../lzma4xx/C/Compress/Lzma

squashfs-objs := inode.o
squashfs-objs += uncomp.o
squashfs-objs += $(addprefix $(LZMADIR)/, LzmaDecode.o)

EXTRA_CFLAGS = -I$(src)/$(LZMADIR)
