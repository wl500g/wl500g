INCLUDEDIR = ../../include/linux

Sqlzma = ../../fs/squashfs
LzmaDec = $(LZMAPATH)/../../../../C/Compress/Lzma

CFLAGS := -Wall -Wno-unused-variable -I. -idirafter$(INCLUDEDIR) -D_FILE_OFFSET_BITS=64 -D_LARGEFILE_SOURCE -D_GNU_SOURCE -O2
ifdef UseDebugFlags
DebugFlags = -g -O0 -UNDEBUG
endif
CFLAGS += -I$(Sqlzma) -I$(LzmaDec) -D_REENTRANT -DNDEBUG ${DebugFlags}

all: mksquashfs-lzma unsquashfs-lzma

mksquashfs-lzma: mksquashfs.o read_fs.o sort.o pseudo.o comp.o uncomp.o LzmaDecode.o $(LZMAPATH)/liblzma.a
	$(CXX) $(LDFLAGS) $^ -lpthread -lm -lz -o $@

mksquashfs.o: mksquashfs.c mksquashfs.h global.h sort.h pseudo.h

read_fs.o: read_fs.c read_fs.h global.h

sort.o: sort.c global.h sort.h

pseudo.o: pseudo.c global.h pseudo.h

comp.o: comp.c $(Sqlzma)/sqlzma.h

uncomp.o: uncomp.c $(Sqlzma)/sqlzma.h

vpath %.c $(Sqlzma) $(LzmaDec)

unsquashfs-lzma: unsquashfs.o uncomp.o LzmaDecode.o $(LZMAPATH)/liblzma.a
	$(CXX) $(LDFLAGS) $^ -lpthread -lm -lz -o $@

unsquashfs.o: unsquashfs.c read_fs.h global.h

clean:
	-rm -f *.o mksquashfs-lzma unsquashfs-lzma

clean-files += mksquashfs-lzma unsquashfs-lzma
