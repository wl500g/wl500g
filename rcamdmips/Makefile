#
#CC = mipsel-uclibc-gcc
#STRIP = mipsel-uclibc-strip
INSTALL = install
LD = $(CC)

PROG = rcamd

LINUXKERNELSRC	= /root/broadcom/src/linux/linux/
LINUXKERNELINCLUDEDIR	= $(LINUXKERNELSRC)/include
#TARGETDIR = /root/broadcom/src/router/mipsel/target

# add -DANYCAM for general video4linux devices
# remove it for Backpaq cameras
#CFLAGS = `gtk-config --cflags` -O -g -g -I$(LINUXKERNELINCLUDEDIR) -Wall 
CFLAGS = -O -g -I$(LINUXKERNELINCLUDEDIR) -I. -I../jpeg-6b -Wall -DWL600 -DNONBLOCK 
LDFLAGS = -L/opt/brcm/hndtools-mipsel-uclibc/lib/ -lpthread 

SRCS = rcamd.c text.c glist.c gtimer.c gslist.c gmutex.c gmem.c garray.c gtree.c ccvt_c.c #djpeg.c
INCLUDES = ${SRCS:.c=.h}
OBJS = ${SRCS:.c=.o}

all:	$(PROG)

%.o:	%.c ${INCLUDES}
	$(CC) -c $(CFLAGS) $< -o $@

install: $(PROG)
	install -D rcamd $(INSTALLDIR)/usr/sbin/rcamd
	$(STRIP) $(INSTALLDIR)/usr/sbin/rcamd

${PROG}: ${OBJS}
	${LD} ${LDFLAGS} ${OBJS} -o $@ libjpeg.a

clean:
	/bin/rm -f *.o *~ core .depend log ${PROG}

VERSION=0.3

# doesn't copy landcam over
ipkg::
	rm -fr $(PROG)_$(VERSION)_arm.ipk
	mkdir -p ipkg/usr/bin
	mkdir -p ipkg/usr/lib/menu
	cp rcamd ipkg/usr/bin/
	cp rcamd-menu ipkg/usr/lib/menu/rcamd
	echo "Package: " ${VERSION} >> control
	echo "Priority: required" >> control
	echo "Version: " ${VERSION} >> control
	echo "Section: extras" >> control
	echo "Architecture: arm" >> control
	echo "Maintainer: Amay Champaneria <amayc@mit.edu>" >> control
	echo "Packager: Amay Champaneria <amayc@mit.edu>" >> control
	echo "Depends: libgtk1.2, libjpeg62, libpng2" >> control
	echo "Description: Remote camera daemon" >> control
	mkdir -p ipkg/CONTROL; mv control ipkg/CONTROL
	cp postinst ipkg/CONTROL
	cp postrm ipkg/CONTROL
	ipkg-build ipkg
	rm -fr ipkg

