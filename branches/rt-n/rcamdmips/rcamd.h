/* 
 * Routines exported to the interface file 
 */


#include <stdlib.h>
#include <pthread.h>
#include <linux/videodev.h>


/* Our preference for the next image to capture 
   (or the next series of images */

enum ECaptureSize {
	ECS_TINY = 0,  // 160 x 120
	ECS_QVGA = 1,  // 320 x 240
	ECS_VGA  = 2   // 640 x 480
};

struct PrefData {
	pthread_mutex_t    lock;     /* Prevent modification of params during a grab */
	pthread_cond_t     cond;
	unsigned short     brightness;
	unsigned short     fps;
	unsigned short     power;
	unsigned short     gain;
	unsigned short     power_mgmt;
	unsigned short     special_modes;
	unsigned char      flip;
	enum ECaptureSize  capture_size;
	
	gboolean           pause;  // Set to TRUE to pause capture
	gboolean           dirty;
};

/* A single captured image */
struct ImageData {
	pthread_mutex_t    lock;
	struct video_mmap  vmap;   // Copied over at capture time
#ifdef WL600
	int				   size;
	char               data[2][640*480*3/2]; 
#else
	char			   data[2][640*480*3];
#endif
  	int cur;
};

#define SMALL_IMAGE 0
#define LARGE_IMAGE 1

struct CaptureData {
	struct PrefData   desired;   /* Change these when you want to reset the image capture */

	// V4L stuff
	int                                video_fd;
	struct video_capability            vcap;
	struct video_picture               vpic;
	struct video_window                vwin;
	struct video_mbuf                  vmbuf;
	struct video_mmap                  vmap[2];
	unsigned char *                    videoBuffer;
	int                                grab_frame; // Which vmap[] we're grabbing

	// The most recently captured image
	struct ImageData    image;
	int                 current_display;
  //struct DisplayArea  displayarea[2];
	
	GTimer        *timer;
	guint          context_id;
};

typedef struct _clientdata {
  int sockfd;
  char *address;
#ifdef WL600
  int written;
  int errcount;	
#endif
} clientdata;

static char *unknown_address = "unknown";

int server(void);
int sender(unsigned char *yuvcap, int size);
unsigned int compare_images(unsigned char *last, unsigned char *current,
				   int width, int height);

void put_image_jpeg (char *filename, char *image, int width, int height, int quality);
