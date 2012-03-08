#
# Makefile for the linux squashfs routines.
#

obj-$(CONFIG_SQUASHFS) += squashfs.o
squashfs-y += block.o cache.o dir.o export.o file.o fragment.o id.o inode.o
squashfs-y += namei.o super.o symlink.o decompressor.o
squashfs-$(CONFIG_SQUASHFS_XATTR) += xattr.o xattr_id.o
squashfs-$(CONFIG_SQUASHFS_LZO) += lzo_wrapper.o
squashfs-$(CONFIG_SQUASHFS_XZ) += xz_wrapper.o
squashfs-$(CONFIG_SQUASHFS_ZLIB) += zlib_wrapper.o
squashfs-$(CONFIG_SQUASHFS_LZMA) += lzma_wrapper.o LzmaDec.o

LzmaDec := $(src)/../../../../lzma/C
CFLAGS_lzma_wrapper.o += -I$(LzmaDec)
CFLAGS_LzmaDec.o += -I$(LzmaDec) -idirafterinclude/linux 
$(obj)/LzmaDec.o: $(LzmaDec)/LzmaDec.c FORCE
	$(call cmd,force_checksrc)
	$(call if_changed_rule,cc_o_c)
