/*
 * Landscape camera software for iPAQ H3600 Mercury Backpaq
 *  MODIFIED by Amay Champaneria for use with general video4linux devices
 *  Compile with ANYCAM #defined for general cameras
 *
 * Copyright 2001 Compaq Computer Corporation.
 *
 * Use consistent with the GNU GPL is permitted,
 * provided that this copyright notice is
 * preserved in its entirety in all copies and derived works.
 *
 * COMPAQ COMPUTER CORPORATION MAKES NO WARRANTIES, EXPRESSED OR IMPLIED,
 * AS TO THE USEFULNESS OR CORRECTNESS OF THIS CODE OR ITS
 * FITNESS FOR ANY PARTICULAR PURPOSE.
 *
 * Author: Andrew Christian 
 *         <andrew.christian@compaq.com>
 *         4 June 2001
 */

#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif

#include <unistd.h>
#include <stdio.h>

#include <string.h>
#include <getopt.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/poll.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <pthread.h>
#include <linux/types.h> 
#include <signal.h>
#include <sys/wait.h>
#include <errno.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <syslog.h>
#include <glib.h> // for GList

#ifdef WL600
#include <jpeglib.h>
#endif

#include "rcamd.h"

#define MYCLAMP(x,y,z) ((x<y) ? (x=y) : ((x>z) ? (x=z) : 0))

gboolean gQuitFlag = FALSE;
gboolean gResetCounters = FALSE;
struct CaptureData gCamera;

#define JPEGBUFFERSIZE 640*480*3

// cmdline args
int max_diff = 30;
int debug = 0;
int seconds = 1;
int port = 12000;
int quality = 75;
int framesize = 2;
int width = 640;
int height = 480;
int gain = 1;
int brightness = 400;
int cameratype = 0;

#ifdef WL600
long recordtime=3600; /* 60 minutes */
int recordindex;
char recordfile[64];
int alertflag;
int tmp_alertflag;
pid_t mypid;
pid_t watchdogpid=-1;
int alertstart=-3;
int alertcount=-1;
char alertfiles[256];
int clientinfo;
long max_count;
long imagecount=0;
int retryflag;
int image_cycle=2;
#define ALERT_DISABLED 0 // Alert is disabled
#define ALERT_ONTIME   1 // Alert is enabled and on time
#define ALERT_OFFTIME  2 // Alert is enabled but off time
#define ALERT_MAIL     3 // Alert is enabled and sendding mail
#define CLIENT_STATE_SLEEP  0
#define CLIENT_STATE_ACTIVE 1
#define CLIENT_STATE_QUIT   2
int grab_retry=0;
int gcount=0;
#endif

int sockfd = -1;
GList *clients;
int newclient = 1;
int idletimeout = 0;
int is_grabed=0;
int first_grabed=0;
pthread_mutex_t clients_lock;
pthread_t server_thread;

/*
 * logmessage
 *
 */
void logmessage(char *logheader, char *fmt, ...)
{
  va_list args;
  char buf[256];

  va_start(args, fmt);
  vsnprintf(buf, sizeof(buf), fmt, args);
  openlog(logheader, 0, 0);
  syslog(0, buf);
  closelog();
  va_end(args);
}



void
handle_sig_pipe(int i)
{
  if (debug) printf("SIGPIPE");
  return;
}

int
server(void)
{
  int clilen, newsockfd, ret=0;
  struct sockaddr_in serv_addr, cli_addr;
  struct sigaction act;
  clientdata *curclient = NULL;
  char *curaddress = NULL;

  // initialize clients
  clients = NULL;

  // set up sigpipe handler
  act.sa_handler = handle_sig_pipe;
  sigemptyset(&act.sa_mask);
  act.sa_flags = SA_SIGINFO;
  sigaction(SIGPIPE, &act, NULL);

  sockfd = socket(AF_INET, SOCK_STREAM, 0);
  if (sockfd < 0) {
    printf("Error opening socket\n");
    return -1;
  }
    
  bzero((char *)&serv_addr, sizeof(serv_addr));
  serv_addr.sin_family = AF_INET;
  serv_addr.sin_addr.s_addr = INADDR_ANY;
  serv_addr.sin_port = htons(port);
  if (bind(sockfd, (struct sockaddr *) &serv_addr,
	   sizeof(serv_addr)) < 0) {
    printf("Error on binding\n");
    return -1;
  }
  
  listen(sockfd, 5);
 
  while (1) {
    clilen = sizeof(cli_addr);
    newsockfd = accept(sockfd, (struct sockaddr *)&cli_addr, &clilen); 
    if (newsockfd < 0) {
      if (debug) printf("Error on accept\n");
    } else {

      // create a new clientdata item
      curclient = malloc(sizeof(clientdata));
      if (curclient==NULL) 
      {
		close(newsockfd);	
		continue;	
      }	
      curclient->sockfd = newsockfd;
      curaddress = inet_ntoa(cli_addr.sin_addr);
      if (curaddress==NULL) 
      {
		curaddress = unknown_address;
      }
      curclient->address = malloc((strlen(curaddress)+1)*sizeof(char));
      if (curclient->address==NULL)
      {
		free(curclient);
		close(newsockfd);
		continue;
      }
      strcpy(curclient->address, curaddress);
#ifdef NONBLOCK
      //ret = fcntl(curclient->sockfd, F_SETFL, O_NONBLOCK);
	  ret = 1;
	  if (ioctl(curclient->sockfd, FIONBIO, &ret))
	  {
		 if (debug) printf("Can't mark socket nonblocking\n");
		 //logmessage("USB Webcam", "can't mark socket nonblocking\n");
	  }		
#endif
      if (debug>1)
		printf("Accepted connection from %s (fcntl ret=%d).\n", curclient->address, ret);
      logmessage("USB webcam", "Accepted connection from %s\n", curclient->address);

      	pthread_mutex_lock(&clients_lock);

#ifdef WL600
		curclient->written = 0;
		curclient->errcount = 0;

		/* handle record and mail alert while the first client connect */
		if (clients==NULL)
		{
			if (debug>1) 
				printf("The first client is connected %d\n", mypid);
			//kill(mypid, 30);
		}
#endif
      	clients = g_list_append(clients, curclient);
      	newclient = 1;
     	pthread_mutex_unlock(&clients_lock);
    }
  }
}


void
closesocklist()
{
  	GList *cur;
  	clientdata *curclient;

  	pthread_mutex_lock(&clients_lock);    
	cur = clients;
 
  	while (cur) 
  	{
	    curclient = (clientdata *)cur->data;
	    if (curclient!=NULL && curclient->sockfd>0) close(curclient->sockfd);
	    cur = cur->next;
  	}
  	pthread_mutex_unlock(&clients_lock);    
}

void
printlist()
{
  GList *cur = clients;
  clientdata *curclient;
  while (cur) 
  {
    curclient = (clientdata *)cur->data;
    //printf("Cur %p data=%p (%d,%s) {%p, %p}\n", cur, curclient, curclient->sockfd, curclient->address, cur->prev, cur->next);
    cur = cur->next;
  }
}

int writeImage(clientdata *client, char *buf, int size)
{
	int ret, csize, written;
	struct pollfd pfd;
	int fd;
	int header;
	int rsize;
	unsigned short *len;

	if (cameratype==2)
	{
		header = 0xffff;
		len = (unsigned short *)(&buf[0]);
		rsize = *len;
	}
	else
	{
		header = size;
		rsize = size;
	}
	
	fd = client->sockfd;
	written = client->written;
	if (written>=sizeof(int))
	{	
		buf += (written - sizeof(int));
		csize = rsize - written + 4;
	}
	else csize = rsize - written;
	
	//logmessage("USB webcam", "dump image %s\n", client->address);
	//sleep(2);

	while(csize>0)
	{
		//logmessage("USB webcam", "de1\n");
		//printf("Write: %d\n", written);	
		if (written==0) // Write size first
			ret = send(fd, &header, sizeof(int), MSG_DONTWAIT);
		else	// 
			ret = send(fd, buf, csize, MSG_DONTWAIT);

		//printf("Set: %d %d\n", ret, written);

		//logmessage("USB webcam", "de2\n");

		if (ret<=0)
		{
			if (debug) printf("ERR: %d %d %d %d\n", ret, errno, size, csize);
			if (ret==0 || (ret<0 && (errno==EAGAIN || errno==EWOULDBLOCK)))
			{
				memset(&pfd, 0, sizeof(pfd));
				pfd.fd = fd;
				pfd.events = POLLOUT | POLLERR | POLLHUP;
				
				ret = poll(&pfd, 1, 500);

				if (ret>0 && (pfd.events&POLLOUT)) continue;
				else return 0;	
			}
			return -1;
		}
		else 
		{
			if (debug>1) printf("Written: %d %d %d\n", ret, csize, written);
			written+=ret;
			client->written = written;
			if (csize<=ret) 
			{
				return written;
			}
			if (written>sizeof(int))
			{
				csize -= ret;
				buf+=ret;
			}
		}
	}
    	return csize;	
}

int
sender(unsigned char *yuvcap, int size)
{
  int numwritten, i=0;
  GList *cur,*old,*del=NULL;
  clientdata *curclient;

  pthread_mutex_lock(&clients_lock);    
 
  cur = clients;
  if (debug) printlist(clients);

  while (cur) {
    curclient = (clientdata *)cur->data;
    if (curclient==NULL && debug>1)
      printf("BAD!!!!!!!!!!!!!! curclient is null!!!!\n");
 
    	if (curclient->sockfd > 0) 
	{
      		if (debug>1) 
			printf("Writing %d bytes to %s %d\n", size, curclient->address, curclient->written);

      		numwritten = writeImage(curclient, yuvcap, size);

      		if (numwritten > 0) 
		{
			if (debug>1) 
				printf("sender: Wrote %d to %s %d\n", numwritten, curclient->address, curclient->written);
			curclient->errcount = 0;
			if (numwritten == size + sizeof(int))
			{
				curclient->written = 0;
			}
      		}
      		else 
		{
			// release this client
			//printf("Error written: %d %d\n", curclient->written, curclient->errcount);
			if (numwritten==0) 
			{	
				curclient->errcount++;
	
				if (curclient->errcount<15) 
				{
					goto finish;
				}
			}
			
			if (debug)
			{ 
				printf("errcount %d %d\n", curclient->errcount, errno);
				printf("sender: Could not write to client %s\n", curclient->address);
			}

			close(curclient->sockfd);

			// mark this node for deletion
			del = g_list_append(del, cur);
finish:
		}
	}
    	i++;
    	cur = cur->next;
  }

  // delete all marked nodes
  cur = del;
  while (cur) 
  {
    old = (GList *)cur->data;
    clients = g_list_remove_link(clients, old);
    curclient = (clientdata *)old->data;
    logmessage("USB webcam", "Remove connection from %s\n", curclient->address);
    free(curclient->address);
    free(curclient);
    g_list_free_1(old);    
    cur = cur->next;
  }
  g_list_free(del);
  pthread_mutex_unlock(&clients_lock);
#ifdef WL600
  /* handle record time and mail alert if the last client is removed */
  if (clients==NULL)
  {
	   if (debug)
	   {	
		   printf("The last client is removed\n");
		   //kill(mypid, SIGUSR1);
	   }	
  }	
#endif
  return 0;
}

/***************************************/

#define CAMERA_IOCTL(TYPE,DATA) \
        do { int result; do {                                          \
		result = ioctl(camera->video_fd, TYPE, &camera->DATA); \
		if ( result && errno != ENODEV ) {                     \
			perror("ioctl: Unable to " #TYPE);             \
			exit(1);                                       \
		}                                                      \
	} while ( result ); } while (0)

void set_camera_info( struct CaptureData *camera )
{
	CAMERA_IOCTL( VIDIOCGWIN, vwin );
	CAMERA_IOCTL( VIDIOCGPICT, vpic );
	
	camera->desired.capture_size = framesize;
	camera->vwin.width = width;
	camera->vwin.height = height;

	if (cameratype==2)	
	{
		camera->vpic.palette = VIDEO_PALETTE_RGB24;
	}
	else
	{
		camera->vpic.palette = VIDEO_PALETTE_YUV420P;
	}
	//camera->vpic.palette = VIDEO_PALETTE_RGB24;
	//camera->vpic.palette = VIDEO_PALETTE_GREY;
	//camera->vpic.brightness = camera->desired.brightness;

	if (cameratype ==0) 
	{
#define PWC_FPS_SHIFT           16
#define PWC_FPS_MASK            0x00FF0000
#define PWC_FPS_FRMASK          0x003F0000
#define PWC_FPS_SNAPSHOT        0x00400000
	        /* Set new framerate */
		camera->vwin.flags &= ~PWC_FPS_FRMASK;
		camera->vwin.flags |= (5 << PWC_FPS_SHIFT);
		if (debug) printf("PWC type Camera has framerate setting; limit to %d fps\n", (camera->vwin.flags & PWC_FPS_FRMASK) >> PWC_FPS_SHIFT);
	}
	else {
		if (debug) printf("OV511 type Camera has framerate setting; max fps\n");
		CAMERA_IOCTL( VIDIOCSPICT, vpic );
	}
	
	CAMERA_IOCTL( VIDIOCSWIN, vwin );
}

void get_camera_info( struct CaptureData *camera )
{
	CAMERA_IOCTL( VIDIOCGWIN, vwin );
	CAMERA_IOCTL( VIDIOCGPICT, vpic );

}

/***************************************/
/*      Open and close the camera      */
/***************************************/

void open_camera( struct CaptureData *camera )
{
	camera->video_fd = open( "/dev/video", O_RDWR);
	if ( camera->video_fd < 0 ) {
		perror("Unable to open /dev/video");
		exit(1);
	}
	if ( ioctl(camera->video_fd, VIDIOCGCAP, &camera->vcap) != 0 ) {
		perror("ioctl VIDIOCGCAP");
		exit(1);
	}
	get_camera_info( camera );
}

void close_camera( struct CaptureData *camera )
{
	//printf("Close camera\n");
	if ( camera->video_fd > 0 ) {
		printf("Close Camera: %d\n", close(camera->video_fd));
		//camera->video_fd = 0;
	}
	//camera->drv->stopvideo(camera->h_drv);
}

/***************************************/
/*      Display thread                 */
/***************************************/

int grab_image( struct CaptureData *camera )
{
	/* Move preferences over */
	pthread_mutex_lock( &camera->desired.lock );
	// Wait until the pause is over
	while (camera->desired.pause) 
		pthread_cond_wait( &camera->desired.cond, &camera->desired.lock );

	if (camera->desired.dirty){
		set_camera_info(camera);
		get_camera_info(camera);
		camera->desired.dirty = FALSE;
	}

	camera->vmap[camera->grab_frame].width = camera->vwin.width;
	camera->vmap[camera->grab_frame].height = camera->vwin.height;
	camera->vmap[camera->grab_frame].format = camera->vpic.palette;
	pthread_mutex_unlock( &camera->desired.lock );

	while ( ioctl(camera->video_fd, 
		      VIDIOCMCAPTURE, 
		      &camera->vmap[camera->grab_frame]) != 0 ) {
		//perror("Unable to CAPTURE frame!\n");
		if (debug) printf("Can't get image\n");		
		usleep( 250000 );
		grab_retry++;

		if (grab_retry>10) 
		{
			logmessage("USB webcam", "webcam capture no frame\n");
			return 0;
		}

	}
	grab_retry=0;
	return 1;
}

void process_image( struct CaptureData *camera )
{
  	int last, cur, diff;
	unsigned char *data;
	struct ImageData *image = &(camera->image);
	FILE *fp;

	/* Copy the parameters and data used in the capture */
	pthread_mutex_lock(&image->lock);
	memcpy( &image->vmap, &camera->vmap[camera->grab_frame], sizeof(image->vmap));
	
	data = camera->videoBuffer + camera->vmbuf.offsets[camera->grab_frame];	

	if (cameratype==2)
	{
		unsigned short *len = (unsigned short *)(&(image->data[image->cur][0]));
		image->size = *len;
		memcpy( image->data[image->cur], data, image->size);
	}
	else
	{
		image->size = image->vmap.width * image->vmap.height * 3 / 2;
		memcpy( image->data[image->cur], data, image->size);
	}
	
    	cur = image->cur;	
	last = (image->cur+1)%2;

	/* Now we have the color-corrected image in image->data 
	   Compress it and signal sender to send it off...
	 */
#ifdef WL600	
	if ((diff = compare_images(image->data[cur], image->data[last], image->vmap.width, image->vmap.height))>max_count || newclient || idletimeout>5)
#else
	if ((diff = compare_images(image->data[cur], image->data[last], image->vmap.width, image->vmap.height))>max_diff || newclient || idletimeout>5) 
#endif
	{
          idletimeout = 0;

	  if (newclient)
	     newclient = 0;
	   	 
	  image->cur = last;

	  //printf("Send image %d %d %d %d\n", imagecount++, last, image->cur, cur);
	  add_time(&image->data[cur], image->vmap.width, image->vmap.height);	

	  if (clients) 
	  {
			/* Image : size + data */	
	    	sender(image->data[cur], image->size);
	  }
#ifdef WL600
	  if (clients == NULL || recordtime == 0)
	  {
			// When to record image:
			// 1. Mail alert and there is difference
			// 2. No mail alert, but record time reach

			if (!(tmp_alertflag==ALERT_ONTIME&&diff<=max_count))
			{
				recordindex = (recordindex+1)%image_cycle;
				sprintf(recordfile, "/var/tmp/record%d.jpg", recordindex);
				add_time(image->data[cur], image->vmap.width, image->vmap.height);
				put_image_jpeg(recordfile, image->data[cur], image->vmap.width, image->vmap.height, 50);
				unlink("/var/tmp/display.jpg");
				symlink(recordfile, "/var/tmp/display.jpg");
				fp = fopen("/var/tmp/webcam.log","w");
				if (fp!=NULL) 
				{
					fprintf(fp, "WebPic=\"%d\"\n", recordindex);
					fclose(fp);
				}
			}

			if (clients==NULL && tmp_alertflag==ALERT_ONTIME) 
			{
				alertstart++;

				if (diff>max_count)
				{
					alertcount++;
					if (alertcount==1)
					{
						sprintf(alertfiles, "sendalarm %s", recordfile);
						if (debug) printf("Start to collect alert message\n");
					}
					else
					{
						sprintf(alertfiles, "%s %s", alertfiles, recordfile);
						// 20030812 by Joey, if image is too large, send less images
						if (alertcount>=image_cycle) 
						{
							alertstart = 12;
						}
						if (debug) printf("Alert :%d\n", alertstart);
					}
				}
			}
			if (debug) printf("Save to file : %s\n", recordfile);
	  	}		
#endif
	} else {
	    if (debug>1)
	       printf("not different enough %d %d\n", cur, last);
	    idletimeout++;
	}
#ifdef WL600
	//printf("Alert? : %x %x %x\n", tmp_alertflag, alertstart, watchdogpid);
	
	if (tmp_alertflag==ALERT_ONTIME && alertstart>=12)
	{
		FILE *fp;
		int pid;

		fp = fopen("/var/tmp/runshell", "w");
		fprintf(fp, "%s &", alertfiles);
		fclose(fp);
		
		//fp = fopen("/etc/linuxigd/runshell1", "w");
		//fprintf(fp, "%s &", alertfiles);
		//fclose(fp);

		//printf("Alertfile: %s\n", alertfiles);
		if (watchdogpid==-1)
		{
			fp = fopen("/var/run/watchdog.pid", "r");
			if (fp!=NULL)
			{
				fscanf(fp, "%d", &watchdogpid);
			}
			fclose(fp);
		}
		if (watchdogpid!=-1)
		{
			kill(watchdogpid, SIGUSR2);
			tmp_alertflag = ALERT_OFFTIME;
		}
		alertstart = 0;
		alertcount = 0;
	}
#endif

	pthread_mutex_unlock(&image->lock);

	/* Set up alarm for VIDIOCSYNC */
	alarm(2);
	ioctl(camera->video_fd, VIDIOCSYNC, &camera->vmap[camera->grab_frame]);	      
	/* Release alarm for VIDIOCSYNC */
	alarm(0);
	camera->grab_frame = 1 - camera->grab_frame;
}

unsigned int
compare_images(unsigned char *last, unsigned char *current,
	       int width, int height)
{
    unsigned char *p1 = last;
    unsigned char *p2 = current;
    int avg, diff, max, i = width*height*3/2; /* divide  by 2 is for YUV420P CAPTURE */
	int count;

	count = 0;

    for (max = 0, avg = 0; --i; p1++,p2++) 
    {
	   diff = (*p1 < *p2) ? (*p2 - *p1) : (*p1 - *p2);
	   avg += diff;
	   if (diff > max)
	       max = diff;

	   if (diff>max_diff) count++;	
    }

    avg = avg / width / height;
    if (debug>1)
       printf("compare: max=%d,avg=%d,count=%d\n",max,avg,count);
    /* return avg */
#ifdef WL600
	/* Modify by Joey to return count that more than threshold */
	return count;
#else
	return max;
#endif
}


/************************************************************************/

int setup_memory_mapping( struct CaptureData *camera )
{
	int retval = 0;

	if ( (retval = ioctl(camera->video_fd,VIDIOCGMBUF,&camera->vmbuf)) < 0 )	{
		perror("failed VIDIOCGMBUF\n");
		return -1;
	}  

	camera->videoBuffer = (unsigned char *) mmap(0, camera->vmbuf.size, 
						     PROT_READ|PROT_WRITE, 
						     MAP_SHARED, camera->video_fd, 0);
	if ( camera->videoBuffer == MAP_FAILED ) {
		perror("unable to map memory\n");
		return -1;
	}
	camera->vmap[0].frame = 0;
	camera->vmap[0].format = camera->vpic.palette;
	camera->vmap[0].width  = camera->vwin.width;
	camera->vmap[0].height = camera->vwin.height;

	camera->vmap[1].frame = 1;
	camera->vmap[1].format = camera->vpic.palette;
	camera->vmap[1].width  = camera->vwin.width;
	camera->vmap[1].height = camera->vwin.height;
	return 0;
}

#ifdef WL600
static char rgbimage[160*120*3];

/*
 * Write image buffer to timestamp named jpeg file.
 */

void
put_image_jpeg (char *filename, char *image, int width, int height, int quality)
{
  int y, x, line_width;
  JSAMPROW row_ptr[1];
  struct jpeg_compress_struct cjpeg;
  struct jpeg_error_mgr jerr;
  char *line;
  char *imageptr;
  int index, gray;
  FILE *fd;

  if ((fd = fopen (filename, "w+")) == NULL) {
    	fprintf (stderr, "Error: Can't Create File %s\n", filename);
    	//exit (-2);
	return;
  }

  /* Add by Joey to handle OV519 frame */
#ifndef REMOVE
  if (cameratype==2)
  {
	unsigned short *len;

	len  = (unsigned short *)(&image[0]);
	*len = (*len)<<3;
	//printf("OV519 : len: %x\n", *len);
	fwrite(image + 2, sizeof(unsigned char), *len, fd);
	fclose(fd);
	return;
  }
#endif
  gray = 0;

  if (cameratype!=2)
  {
	if (width<=160&&height<=120)
	{
		ccvt_420p_bgr24(width, height, image, image + width*height, image + (width*height)+(width*height/4), rgbimage);
		imageptr=rgbimage;
	}
	else 
	{
		gray = 1;	
		imageptr=image;
	}
  }
  else
  {
	imageptr = image;
  }	
	 
  line = malloc (width * 3);
  if (!line) return;
  cjpeg.err = jpeg_std_error (&jerr);
  jpeg_create_compress (&cjpeg);
  cjpeg.image_width = width;
  cjpeg.image_height = height;
  cjpeg.input_components = 3;
  cjpeg.in_color_space = JCS_RGB;
  jpeg_set_defaults (&cjpeg);

  jpeg_set_quality (&cjpeg, quality, TRUE);
  //cjpeg.dct_method = JDCT_FASTEST;
  cjpeg.dct_method = JDCT_DEFAULT;
  jpeg_stdio_dest (&cjpeg, fd);

  jpeg_start_compress (&cjpeg, TRUE);

  row_ptr[0] = line;
  line_width = width * 3;
  index = 0;

  for (y = 0; y < height; y++) 
  {
    //imageptr = image + width;
    for (x = 0; x < line_width; x+=3) 
    {
		if (gray)
		{
			line[x]=imageptr[0];
			line[x+1]=imageptr[0];
			line[x+2]=imageptr[0];
			imageptr+=1;
		}
		else
		{
			line[x]=imageptr[2];
			line[x+1]=imageptr[1];
			line[x+2]=imageptr[0];
			imageptr+=3;
		}
    }
    jpeg_write_scanlines (&cjpeg, row_ptr, 1);
    //image += line_width;
    //image += width;
  }

  jpeg_finish_compress (&cjpeg);
  jpeg_destroy_compress (&cjpeg);
  free (line);
  fclose (fd);
}

void sig_usr(int sig)
{
  //printf("Signal %d\n", sig);
  FILE *fp;

  if (sig==SIGUSR1)
  {
	if ((fp=fopen("/var/run/rcamd.pid","r"))==NULL)
	{
		closesocklist();
  		munmap(gCamera.videoBuffer, gCamera.vmbuf.size);	
		close_camera( &gCamera );
		g_timer_stop( gCamera.timer );
		g_timer_destroy(gCamera.timer);
		pthread_mutex_destroy( &gCamera.desired.lock );
		pthread_cond_destroy( &gCamera.desired.cond );
		pthread_mutex_destroy( &gCamera.image.lock );
		pthread_mutex_destroy(&clients_lock);
		exit(0);
	}
	else
	{
		fclose(fp);
	  	/* Used to enable alert */	
		if (alertflag)
		{		
	  	  	tmp_alertflag = ALERT_ONTIME;
	  	}	
	}	
  }	
  else if(sig==SIGUSR2)
  {	
	 /* Used to disable alert */
	 if (clients==NULL)
	 {	
	  	if (tmp_alertflag == ALERT_ONTIME)
	  	{
		 	alertstart = alertcount = 0;
			tmp_alertflag = ALERT_OFFTIME;
	  	}
	 }	
  }
  if (debug) printf("Signal: %d %d\n", sig, tmp_alertflag);	
}

void sig_alrm(int sig)
{
  closesocklist();
  munmap(gCamera.videoBuffer, gCamera.vmbuf.size);	
  close_camera( &gCamera );
  g_timer_stop( gCamera.timer );
  g_timer_destroy(gCamera.timer);
  pthread_mutex_destroy( &gCamera.desired.lock );
  pthread_cond_destroy( &gCamera.desired.cond );
  pthread_mutex_destroy( &gCamera.image.lock );
  pthread_mutex_destroy(&clients_lock);
  if (debug) printf("Signal SIGALRM\n");	
}
#endif

int main (int argc, char *argv[])
{
	int i, nFrames=0, waittime;
	gdouble prevTime, curTime;
	sigset_t sig;
	FILE *fp;

	// parse cmdline args
	while((i = getopt(argc, argv, "t:d:m:s:f:p:hg:b:
#ifdef WL600
    r:a:c:z:
#endif
    ")) != EOF) {

	  switch(i) {
	  case 't':
	    cameratype = atoi(optarg);
	    break;
	  case 'd':
	    debug = atoi(optarg);
	    break;
	  case 'm':
	    max_diff = atoi(optarg);
	    break;
	  case 's':
	    seconds = atoi(optarg);	    
	    break;
	  case 'f':	    	    
	    sscanf (optarg, "%dx%d", &width, &height);
	    if (width>320) image_cycle = 2;
	    else image_cycle = 6;
            break;
	  case 'p':
	    port = atoi(optarg);
	    break;
	  case 'b':
	    brightness = atoi(optarg);
	    MYCLAMP(brightness, 1, 500);
	    break;
	  case 'g':
	    gain = atoi(optarg);
	    MYCLAMP(gain, 0, 4);
	    gain = (0x1<<gain)>>1;
	    break;
#ifdef WL600
	  case 'r':
		recordtime = (long )atoi(optarg);
		recordtime = recordtime*60;
		recordindex=0;
		//printf("Record Time: %d\n", recordtime);
		break;
	  case 'a':
		alertflag = atoi(optarg);
		if (alertflag)
			tmp_alertflag = ALERT_OFFTIME;
		else 
			tmp_alertflag = ALERT_DISABLED;
		break;
	  case 'c':
		max_count = atoi(optarg);
		break;
	  case 'z':
		setenv("TZ", optarg, 1);
		break;
#endif
	  case 'h':
	  default:
	    printf("Usage: rcamd [options]\n"\
		   "       -d <debug level>\n"\
		   "       -m <max difference threshold=0-255> (lower->more updates, default=30)\n"\
		   "       -s <seconds between frames> (default==1)\n"\
		   "       -f  <WidthxHeight obey supported size> define size of the output image (default:640x480)\n"\
		   "       -p <port to listen for connections>\n"\
		   "       -b <brightness=1-500> (default=400 available only cameratype=1)\n"\
		   "       -g <gain=0-4> corresponding to a gain of 2^<gain> (default=0)\n"\
		   "       -t <cameratype=0,1> corresponding to a camera driver (0:pwc or 1:ov511 default=0)\n"\	
		   "       -r <record interval=0-65535> (default=60 minutes)\n"\
		   "       -a <alert=0,1> define if alert is on or off)\n"\				
		   "       -s <max count for difference threshold=0-999999> (default=100)\n"\
		   "       -z <time zone string>\n");
	    exit(0);
	  }
	}

	grab_retry = 0;

	if (debug) printf("RCAMD 0.06 using debug=%d, maxdiff=%d, secs=%d, size=%dx%d, port=%d, bright=%d, gain=%d, cameratype=%d\n", debug, max_diff, seconds, width, height, port, brightness, gain, cameratype);

#ifdef WL600
	//if (daemon(NULL, 1) == -1) 
	//{
	//	perror("daemon");
	//	exit(errno);
	//}

	/* Add signal handle for timer, mode change */
	mypid = getpid();
	sigemptyset(&sig);
	sigaddset(&sig, SIGPIPE);
	sigprocmask(SIG_BLOCK, &sig, NULL);
	signal(SIGUSR1, sig_usr);
	signal(SIGUSR2, sig_usr);
	signal(SIGALRM, sig_alrm);

	if ((fp=fopen("/var/run/rcamd.pid","w"))!=NULL)
	{
		fprintf(fp, "%d", mypid);
		fclose(fp);
	}
#endif

Retry:
	// Initialize the storage structures
	memset(&gCamera,0,sizeof(struct CaptureData));
	gCamera.current_display = SMALL_IMAGE;

	pthread_mutex_init(&gCamera.desired.lock, NULL );
	pthread_cond_init(&gCamera.desired.cond, NULL );
	pthread_mutex_init(&gCamera.image.lock, NULL );
	
	if (!retryflag) 
	{
		pthread_mutex_init(&clients_lock, NULL );
	}

	open_camera( &gCamera);
	set_camera_info(&gCamera);
	get_camera_info(&gCamera);
	
	if (!retryflag)
	{
		pthread_create(&server_thread, NULL, (void *)&server, NULL);
	}
	
	if ( setup_memory_mapping( &gCamera ))
		exit(1);

	gCamera.timer = g_timer_new();
	g_timer_start( gCamera.timer );

	prevTime = g_timer_elapsed(gCamera.timer, NULL);
	gCamera.image.cur=0;

	retryflag = 0;

	while (1) 
	{
		if (retryflag) 
		{
			//logmessage("USB webcam", "Reset Webcam\n");
			break;
		}	
#ifdef WL600
		if (clients==NULL) 
		{
			if (recordtime>0&&tmp_alertflag!=ALERT_ONTIME)
			{  
				int rt;

				rt = recordtime;
				//printf("Fall in sleep %d\n", recordtime);
				while(rt)
				{	
					sleep(1);
					rt--;					
					if (clients || tmp_alertflag == ALERT_ONTIME) break;		
				}
				//printf("Wake up\n");
			}
		}
		if (debug) printf("Status: %d %d %d\n", tmp_alertflag, alertstart, alertcount);
#endif
	  	if (!grab_image( &gCamera ))
		{ 
			is_grabed=0;
			retryflag=1;
			continue;
		}
		else 	
		{
			first_grabed=1;
			is_grabed=1;
		}

	  	process_image( &gCamera );
		//printf("End of process image outside\n");
	  	curTime = g_timer_elapsed(gCamera.timer, NULL);
	  	//sleep until next capture
	  	if (seconds!=0) {
	    	waittime = (int)(1000000.0*(double)seconds-1000000.0*(curTime-prevTime));
	    	if (debug)
	      		printf("Waiting %f %f %f %d - %d = %d seconds\n", curTime, prevTime, curTime-prevTime, 1000000*seconds, (int)(1000000.0*(curTime-prevTime)), waittime);
	    	if (waittime > 0)
	      		usleep(waittime);
	  	}

	  	curTime = g_timer_elapsed(gCamera.timer, NULL);
	  	if (curTime != 0.0 && curTime > prevTime) 
		{
	    	if (debug)
	      	printf("Speed: avg(%6.2f fps) cur(%6.2f fps)\n", 
		     ++nFrames/curTime, 1.0/(curTime - prevTime));
	  	}
	  	prevTime = curTime;
	}

	gQuitFlag = TRUE;
	closesocklist();
	close_camera( &gCamera );
	g_timer_stop( gCamera.timer );
	g_timer_destroy(gCamera.timer);
	pthread_mutex_destroy( &gCamera.desired.lock );
	pthread_cond_destroy( &gCamera.desired.cond );
	pthread_mutex_destroy( &gCamera.image.lock );
	pthread_mutex_destroy(&clients_lock);
	return 0;
}

decompress_jpeg(char *buf, int size, char *data)
{
	
}
