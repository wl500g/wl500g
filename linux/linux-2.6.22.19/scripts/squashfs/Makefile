INCLUDEDIR = $(SRCBASE)/linux/linux-2.6/include/linux

CFLAGS := -Wall -Wno-unused-variable -I. -idirafter$(INCLUDEDIR) -D_FILE_OFFSET_BITS=64 -D_LARGEFILE_SOURCE -D_GNU_SOURCE -O2

all: mksquashfs unsquashfs

mksquashfs: mksquashfs.o read_fs.o sort.o
	$(CC) mksquashfs.o read_fs.o sort.o -lz -lpthread -lm -o $@

mksquashfs.o: mksquashfs.c mksquashfs.h global.h sort.h

read_fs.o: read_fs.c read_fs.h global.h

sort.o: sort.c global.h sort.h

unsquashfs: unsquashfs.o
	$(CC) unsquashfs.o -lz -lpthread -lm -o $@

unsquashfs.o: unsquashfs.c read_fs.h global.h

clean:
	-rm -f *.o mksquashfs unsquashfs
