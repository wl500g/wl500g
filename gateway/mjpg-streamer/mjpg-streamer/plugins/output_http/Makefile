###############################################################
#
# Purpose: Makefile for "M-JPEG Streamer"
# Author.: Tom Stoeveken (TST)
# Version: 0.3
# License: GPL
#
###############################################################


OTHER_HEADERS = ../../mjpg_streamer.h ../../utils.h ../output.h ../input.h

CFLAGS += -shared -fPIC -g
#CFLAGS += -DDEBUG
LFLAGS += -lpthread -ldl

ifeq ($(USE_LIBV4L2),true)
#LFLAGS += -lv4l2
CFLAGS += -DUSE_LIBV4L2
endif

all: output_http.so

clean:
	rm -f *.a *.o core *~ *.so *.lo

output_http.so: $(OTHER_HEADERS) output_http.c httpd.lo
	$(CC) $(CFLAGS) -o $@ output_http.c httpd.lo

httpd.lo: $(OTHER_HEADERS) httpd.h httpd.c
	$(CC) -c $(CFLAGS) -o $@ httpd.c
