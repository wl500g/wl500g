###############################################################
#
# Purpose: Makefile for "M-JPEG Streamer"
# Author.: Tom Stoeveken (TST)
# Version: 0.4
# License: GPL
#
###############################################################

# specifies where to install the binaries after compilation
# to use another directory you can specify it with:
# $ sudo make DESTDIR=/some/path install
DESTDIR = /usr

PLUGIN_LIB_DIR = $(DESTDIR)/lib/mjpg-streamer

# set the compiler to use
#CC = gcc

# general compile flags, enable all warnings to make compile more verbose
CFLAGS += -O2 -DLINUX -D_GNU_SOURCE -Wall $(EXTRACFLAGS)
#CFLAGS +=  -DDEBUG

# we are using the libraries "libpthread" and "libdl"
# libpthread is used to run several tasks (virtually) in parallel
# libdl is used to load the plugins (shared objects) at runtime
LFLAGS += -lpthread -ldl $(EXTRALDFLAGS)

# define the name of the program
APP_BINARY = mjpg_streamer

# define the names and targets of the plugins
PLUGINS = input_uvc.so
PLUGINS += output_file.so
#PLUGINS += output_udp.so
PLUGINS += output_http.so
#PLUGINS += input_testpicture.so
PLUGINS += output_autofocus.so
#PLUGINS += input_gspcav1.so
#PLUGINS += input_file.so
PLUGINS += input_control.so
# PLUGINS += input_http.so
# The viewer plugin requires the SDL library for compilation
# This is very uncommmon on embedded devices, so it is commented out and will
# not be build automatically.
# PLUGINS += output_viewer.so

PLUGINS_DIRS :=$(patsubst %.so,plugins/%/,$(PLUGINS))
PLUGINS_SO := $(join $(PLUGINS_DIRS),$(PLUGINS))

# define the names of object files
OBJECTS=mjpg_streamer.o utils.o
SRCS=$(patsubst %.o,%.c,$(OBJECTS))
HEADERS=mjpg_streamer.h utils.h

export CC CFLAGS LFLAGS

# this is the first target, thus it will be used implictely if no other target
# was given. It defines that it is dependent on the application target and
# the plugins
all: application plugins

application: $(APP_BINARY)

plugins: $(PLUGINS)

$(PLUGINS): $(PLUGINS_SO)
	cp $? .

$(PLUGINS_SO): $(HEADERS)
	make -C $(dir $@) all USE_LIBV4L2=$(USE_LIBV4L2)

$(OBJECTS): $(SRCS) $(HEADERS)

$(APP_BINARY): $(OBJECTS)
	$(CC) -o $@ $(CFLAGS) $(LFLAGS) $(OBJECTS)

.c.o:
	$(CC) -c -o $@ $(CFLAGS) -DPLUGIN_LIB_DIR=\"$(PLUGIN_LIB_DIR)\" $<

# cleanup
clean:
	for plug in $(PLUGINS_DIRS); do \
	  make -C $$plug $@; \
	done;
	rm -f *.a *.o $(APP_BINARY) core *~ *.so *.lo

# useful to make a backup "make tgz"
tgz: clean
	mkdir -p backups
	tar czvf ./backups/mjpg_streamer_`date +"%Y_%m_%d_%H.%M.%S"`.tgz --exclude backups --exclude .svn *

# install MJPG-streamer and example webpages
install: all
	install --mode=755 -D $(APP_BINARY) $(DESTDIR)/bin/$(APP_BINARY)
	install --mode=755 -d $(DESTDIR)/lib/mjpg-streamer
	install --mode=644 $(PLUGINS) $(DESTDIR)/lib/mjpg-streamer
#	install --mode=755 -d $(DESTDIR)/share/mjpg-streamer/www
#	install --mode=644 -D www/* $(DESTDIR)/share/mjpg-streamer/www

# remove the files installed above
uninstall:
	rm -f $(DESTDIR)/bin/$(APP_BINARY)
	for plug in $(PLUGINS); do \
	  rm -f $(DESTDIR)/lib/$$plug; \
	done;
