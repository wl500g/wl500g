#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>
#include <syslog.h>
#include <sys/param.h>
#include <sys/resource.h>
#include <time.h>
#include <errno.h>
#include "sha1.h"
#include "authd2.h"

#define  MESSAGE_NO      0
#define  MESSAGE_ACCEPT  1
#define  MESSAGE_REJECT  2

#define VERSION 255

char AREANAME[80]="";
char USERNAME[80]="";
char HOSTNAME[]="172.20.255.1";
char MKEY[80]="";
unsigned char NKEY[20];

unsigned a_passnd=A_PASSND3|(VERSION<<16);

#define AHEX(a) ((a)<='9'?((a)-'0'):(a)-'A'+10)

int readconfig(void)
{
FILE *f;
char buf[256];
int i;

  MKEY[0]=0;
  f=fopen( DEFAULT_CONF_PATH,"r");
  if( !f ) return 0;

  while( !feof(f) ) {

    fscanf(f,"%s\n",buf);

    if( strstr(buf,"key") ) {
	  i=0;
      while(buf[i]!='=')i++;
	  i++;
	  strncpy(MKEY,(buf+i),40);
      for( i=0;i<20;i++ ) {
        NKEY[i] = AHEX(MKEY[i*2])*16 + AHEX(MKEY[i*2+1]);
      }
    }

    if( strstr(buf,"area") ) {
	  i=0;while(buf[i]!='=')i++;
	  i++;
	  strncpy(AREANAME,(buf+i),20);
    }

    if( strstr(buf,"user") ) {
	  i=0;
      while(buf[i]!='=')i++;
	  i++;
	  strncpy(USERNAME,(buf+i),20);
    }
/*
    if( strstr(buf,"password") ) {
	  i=0;while(buf[i]!='=')i++;
	  i++;
      if( buf[i]!=0 ) {
    	strncpy((char*)passwd,(buf+i),20);
      }
    }
*/
  }
  fclose(f);
  return ( MKEY[0]!=0 );
}


/*******************************************************
*
*  Function which provide log messages
*
*******************************************************/
void syslog_authd(int pr,  char *msg)
{
  
  
  #ifdef DEBUG
    openlog("authcli", LOG_PID | LOG_CONS | LOG_PERROR, LOG_DAEMON);
  #else
    openlog("authcli", LOG_PID | LOG_CONS, LOG_DAEMON);
  #endif

    syslog(pr,msg);
    closelog();

}


int loop_flag  = 1;

void authcli_stop(int sig)
{
  syslog_authd(LOG_NOTICE,"Recived SIGINT. Authcli stoped.");
  loop_flag=0;	
}

void authcli_term(int sig)
{
  syslog_authd(LOG_NOTICE,"Recived SIGTERM. Authcli stoped.");
  loop_flag=0;	
}

void authcli_hup(int sig)
{
  syslog_authd(LOG_NOTICE,"Recived SIGHUP. Please, use stop and start with new settings.");
}


void prmsg(unsigned char*m)
{
  int i;
  for( i=0;i<20;i++ ) printf( "%X",(int)m[i] );
}


/*******************************************************
*
*  Function which create socked for accept
*
*******************************************************/

int make_socket (unsigned short int port)
{
  int sock;
  struct sockaddr_in name;

  /* Create the socket */
  sock = socket (PF_INET, SOCK_STREAM, 0);

  /* If socet is't created then exit with EXIT_FAILURE */
  if( sock < 0 ) {
    syslog_authd( LOG_ERR, "can't create main socket" );
    exit (EXIT_FAILURE);
  }

  /* Give the socket a name */
  name.sin_family = AF_INET;
  name.sin_port = htons (port);
  name.sin_addr.s_addr = htonl (INADDR_ANY);
  if (bind(sock, (struct sockaddr*)&name, sizeof(name)) < 0) {
    syslog_authd( LOG_ERR, "can't bind to main socket" );
    exit (EXIT_FAILURE);
  }

  return sock;
}



int read_message (int fd, struct message * msg)
{
  int  nb;

  nb = read (fd, msg, sizeof(struct message) );

  #ifdef DEBUG
    char buf[128];
    sprintf( buf, "recv msg: %X%X %X len %d", *(unsigned*)msg->msg, *((unsigned*)msg->msg+1) ,msg->atr, nb);
    syslog_authd( LOG_DEBUG, buf );
  #endif

  if( nb<0 ) {  /* Read error */
    return 0;
  } else if( nb!=sizeof(struct message)) {
    return 0;
  }

  return 1;
}



int write_message (int fd, struct message * msg)
{
  int  nb;

  nb = write (fd, msg, sizeof(struct message) );
  if (nb < 0) {
    return 0;
  }

  #ifdef DEBUG
    char buf[128];
    sprintf( buf, "send msg: %X%X %X", *(unsigned*)msg->msg, *((unsigned*)msg->msg+1) ,msg->atr);
    syslog_authd( LOG_DEBUG, buf );
  #endif

  return 1;
}

int init_sockaddr (
  struct sockaddr_in *name,
  const char *host,
  unsigned short int port )
{
struct hostent *hostinfo;
unsigned long addr;

  bzero( name, sizeof(struct sockaddr_in) );
  name->sin_family = AF_INET;
  name->sin_port = htons (port);
  addr = inet_addr( HOSTNAME );
  name->sin_addr = *(struct in_addr *) (&addr);
  return 1;
}

void ThreadFastStartReq(void)
{
int sock;
struct sockaddr_in servername;

  sock = socket (PF_INET, SOCK_STREAM, 0);
  if( (sock>0) && init_sockaddr(&servername,HOSTNAME,SERV_TCP_PORT) ) {
    connect(sock,(struct sockaddr*) &servername, sizeof(servername));
    send(sock,"IMAL", 4, 0);
    close(sock);
  }
}



/*******************************************************
*
*  Main function
*
*******************************************************/
int main(int argc, char *argv[])
{
  int  message_flag = MESSAGE_NO; // This flag for syslog
  int  sockfd, i, res, cs;
  struct sockaddr_in serv_addr, cli_addr;
  char s[17],temptext[128];
  char buf[1280];
  int  fd;
  struct  rlimit  flim;
  int     sock, newsock;
  fd_set  active_fd_set, read_fd_set;
  struct  sockaddr_in  clientname;
  size_t  size;
  FILE *fpid;


  /* Read comand promt */
/*  if( argc<2 ) {
    fprintf(stderr, "usage: passwd\n");
    syslog_authd(LOG_ERR, "please, use: authcli password");
    exit(1);
  }
*/

  if( !readconfig() ) {
    fprintf(stderr, "Error reading config file %s\n", DEFAULT_CONF_PATH);
    syslog_authd(LOG_ERR, "Error reading config file");
    exit(1);
  }

  /* Create the socket and set it up to accept connections */
  sock = make_socket (SERV_TCP_PORT);
  if( listen(sock, 3) < 0 ) {
    syslog_authd( LOG_ERR, "can't listen to main socket" );
    exit (EXIT_FAILURE);
  }

  /*  Daemon is started. Write PID file */
  {
    int pid = getpid();
    if (pid<0)
    {
      syslog_authd(LOG_ERR, "Can't get pid: pid error.");
      exit(1);
    }

    fpid=fopen(DEFAUL_PID_PATH, "w");

    if (fpid==NULL)
    {
      syslog_authd(LOG_ERR, "Can't create pid file.");
      exit(1);
    }

    sprintf( buf, "%d", pid );
    fwrite( buf, strlen(buf), 1, fpid);
    fclose( fpid);

    /*  Set stop signal */
    signal(SIGINT,authcli_stop);
    signal(SIGTERM,authcli_term);
    
    /* Set SIGHUP */
    signal(SIGHUP,authcli_hup);

    sprintf( buf, "authcli v.%.2f started.",VERSION/100. );
    syslog_authd(LOG_NOTICE, buf);
  }
  
  if( fork() ){
    ThreadFastStartReq();
    exit(0);
  }
  

  /* Initialize the set of active sockets */
  FD_ZERO (&active_fd_set);
  FD_SET  (sock, &active_fd_set);


  while (loop_flag)
  {
    /* Block until input arrives on one or more active sockets */
    read_fd_set = active_fd_set;
    if (select(FD_SETSIZE, &read_fd_set, NULL, NULL, NULL) < 0) {
      #ifdef DEBUG
	sprintf( buf, "errno %d select",errno );
        syslog_authd( LOG_DEBUG, buf );
      #endif
      continue;
    }

    /* Service all the sockets with input pending */
    for (i = 0; i < FD_SETSIZE; i++)
    {
      if (FD_ISSET(i,&read_fd_set))
      {
        if (i == sock) {  /* New connection request on original socket.  */

          size = sizeof(clientname);
          if( (newsock = accept(sock,(struct sockaddr*)&clientname, &size)) < 0) { 
            syslog_authd( LOG_ERR, "error on accept" );
            exit (EXIT_FAILURE);
          }

          #ifdef DEBUG
            syslog_authd( LOG_DEBUG, "new connection" );
	  #endif

          FD_SET (newsock, &active_fd_set);

        } else {           /* Data arriving on an already-connected socket */
	  struct message ms,mr;
    	  struct client *c;
	  unsigned char rez[20];

          #ifdef DEBUG
            syslog_authd( LOG_DEBUG, "Reciving the data" );
	  #endif

          if( !read_message(i, &mr) ) {
            syslog_authd( LOG_NOTICE, "Error reading msg");
	    close(i);
            FD_CLR (i, &active_fd_set);
	    continue;
	  }

          if( mr.atr == A_KEYSND ) {

	    ms.atr = a_passnd;
	    a_passnd = A_PASSND2 | (VERSION<<16);

	    memcpy(temptext, mr.msg,  SHA1KEYL );
	    memcpy(temptext+SHA1KEYL, NKEY, SHA1KEYL );
            sha1encode_strn( temptext, SHA1KEYL*2, ms.msg);

            if( !write_message(i, &ms) ) {
              close(i);
              FD_CLR (i, &active_fd_set);
              syslog_authd( LOG_NOTICE, "Error writing msg");
	      continue;
	    }

	  } else if( mr.atr==A_PASASQ ) {
	    int bal,lim;    
	  
	    if( ((int*)mr.msg)[0] == 1 ) {
	      bal = ((int*)mr.msg)[1];
	      lim = ((int*)mr.msg)[2];
	      if( bal<lim ) {
		sprintf( buf, "Low balance %.2f",bal/100. );
	        syslog_authd( LOG_NOTICE, buf );
	      } else {
		if (message_flag!=MESSAGE_ACCEPT) {
		  sprintf( buf, "balance %.2f",bal/100. );
	          syslog_authd( LOG_NOTICE, buf );
 	        }
	      }
	    if (message_flag!=MESSAGE_ACCEPT)
	    {
	      syslog_authd( LOG_NOTICE, "key accepted");
	      message_flag=MESSAGE_ACCEPT;
	    }


	    }
	    
	    close(i);
            FD_CLR (i, &active_fd_set);
	  } else {
	    if (message_flag!=MESSAGE_REJECT)
	    {
              syslog_authd( LOG_NOTICE, "key rejected");
	      message_flag=MESSAGE_REJECT;
	    }
	    close(i);
            FD_CLR (i, &active_fd_set);
	  }


        }
      } /* end if */
    } /* end for */
  } /* end while */
}




