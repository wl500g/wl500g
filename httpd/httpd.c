/* milli_httpd - pretty small HTTP server
** A combination of
** micro_httpd - really small HTTP server
** and
** mini_httpd - small HTTP server
**
** Copyright © 1999,2000 by Jef Poskanzer <jef@acme.com>.
** All rights reserved.
**
** Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions
** are met:
** 1. Redistributions of source code must retain the above copyright
**    notice, this list of conditions and the following disclaimer.
** 2. Redistributions in binary form must reproduce the above copyright
**    notice, this list of conditions and the following disclaimer in the
**    documentation and/or other materials provided with the distribution.
**
** THIS SOFTWARE IS PROVIDED BY THE AUTHOR AND CONTRIBUTORS ``AS IS'' AND
** ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
** IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
** ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR OR CONTRIBUTORS BE LIABLE
** FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
** DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
** OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
** HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
** LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
** OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
** SUCH DAMAGE.
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include <time.h>
#include <unistd.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>

#include <httpd.h>

#ifdef vxworks
static void fcntl(int a, int b, int c) {}
#include <signal.h>
#include <ioLib.h>
#include <sockLib.h>
extern int snprintf(char *str, size_t count, const char *fmt, ...);
extern int strcasecmp(const char *s1, const char *s2); 
extern int strncasecmp(const char *s1, const char *s2, size_t n); 
extern char *strsep(char **stringp, char *delim);
#define socklen_t 		int
#define main			milli
#else /* !vxvorks */
#include <error.h>
#include <sys/signal.h>
#include <sys/ioctl.h>
#endif

#include "bcmnvram_f.h"

// Added by Joey for ethtool
#include <net/if.h>
#include "ethtool-util.h"
#ifndef SIOCETHTOOL
#define SIOCETHTOOL 0x8946
#endif

#define SERVER_NAME "httpd"
#define SERVER_PORT 80
#define PROTOCOL "HTTP/1.0"
#define RFC1123FMT "%a, %d %b %Y %H:%M:%S GMT"

#include <bcmconfig.h>
#ifndef __CONFIG_IPV6__
#define uaddr struct in_addr
#define uaddr_init {.s_addr = 0}
#else
#define uaddr struct in6_addr
#define uaddr_init IN6ADDR_ANY_INIT
#endif

/* A multi-family sockaddr. */
typedef union {
    struct sockaddr sa;
#ifndef __CONFIG_IPV6__
    struct sockaddr_in sa_in;
#else
    struct sockaddr_in6 sa_in;
#endif
    } usockaddr;

/* Globals. */
static FILE *conn_fp;
static char auth_userid[AUTH_MAX];
static char auth_passwd[AUTH_MAX];
static char auth_realm[AUTH_MAX];

/* Forwards. */
static int initialize_listen_socket( usockaddr* usaP );
static int auth_check( char* dirname, char* authorization );
static void send_authenticate( char* realm );
static void send_error( int status, char* title, char* extra_header, char* text );
static void send_headers( int status, char* title, char* extra_header, char* mime_type );
static int b64_decode( const char* str, unsigned char* space, int size );
static int match( const char* pattern, const char* string );
static int match_one( const char* pattern, int patternlen, const char* string );
static void handle_request(void);

static int is_firsttime(void);
static int is_connected(void);
static void http_login(uaddr ip);
static int http_login_check(void);
static void http_logout(uaddr ip);
static void http_login_timeout(uaddr ip);


/* added by Joey */
int redirect = 1;
char wan_if[16];
int http_port=SERVER_PORT;
static int server_port = SERVER_PORT; /* Port for SERVER USER interface */

/* Added by Joey for handle one people at the same time */
uaddr login_ip = uaddr_init;
uaddr login_ip_tmp = uaddr_init;
uaddr login_try = uaddr_init;
time_t login_timestamp=0;

static int
initialize_listen_socket( usockaddr* usaP )
    {
    int listen_fd;
    int i;

    memset( usaP, 0, sizeof(usockaddr) );
#ifndef __CONFIG_IPV6__
    usaP->sa.sa_family = AF_INET;
    usaP->sa_in.sin_addr.s_addr = htonl( INADDR_ANY );
    usaP->sa_in.sin_port = htons( http_port );
#else
    usaP->sa.sa_family = AF_INET6;
    usaP->sa_in.sin6_addr = in6addr_any;
    usaP->sa_in.sin6_port = htons( http_port );
#endif

    listen_fd = socket( usaP->sa.sa_family, SOCK_STREAM, 0 );
    if ( listen_fd < 0 )
	{
	perror( "socket" );
	return -1;
	}
    (void) fcntl( listen_fd, F_SETFD, 1 );
    i = 1;
    if ( setsockopt( listen_fd, SOL_SOCKET, SO_REUSEADDR, (char*) &i, sizeof(i) ) < 0 )
	{
	perror( "setsockopt" );
	return -1;
	}
    if ( bind( listen_fd, &usaP->sa, sizeof(usaP->sa_in) ) < 0 )
	{
	perror( "bind" );
	return -1;
	}
    if ( listen( listen_fd, 1024 ) < 0 )
	{
	perror( "listen" );
	return -1;
	}
    return listen_fd;
    }


static int
auth_check( char* dirname, char* authorization )
    {
    char authinfo[500];
    char* authpass;
    int l;

    /* Is this directory unprotected? */
    if ( !strlen(auth_passwd) )
	/* Yes, let the request go through. */
	return 1;

    /* Basic authorization info? */
    if ( !authorization || strncmp( authorization, "Basic ", 6 ) != 0) 
    {
	send_authenticate( dirname );
	http_logout(login_ip_tmp);
	return 0;
    }

    /* Decode it. */
    l = b64_decode( &(authorization[6]), authinfo, sizeof(authinfo) );
    authinfo[l] = '\0';
    /* Split into user and password. */
    authpass = strchr( authinfo, ':' );
    if ( authpass == (char*) 0 ) {
	/* No colon?  Bogus auth info. */
	send_authenticate( dirname );
	http_logout(login_ip_tmp);
	return 0;
    }
    *authpass++ = '\0';

    /* Is this the right user and password? */
    if ( strcmp( auth_userid, authinfo ) == 0 && strcmp( auth_passwd, authpass ) == 0 )
    {
    	//fprintf(stderr, "login check : %x %x\n", login_ip, login_try);
    	/* Is this is the first login after logout */
#ifndef __CONFIG_IPV6__
    	if (login_ip.s_addr==0 && login_try.s_addr==login_ip_tmp.s_addr)
    	{
		send_authenticate(dirname);
		login_try.s_addr=0;
		return 0;
    	}
#else
    	if ((memcmp(&login_ip, &in6addr_any, sizeof(struct in6_addr)) == 0) &&
    	    (memcmp(&login_try, &login_ip_tmp, sizeof(struct in6_addr)) == 0))
    	{
		send_authenticate(dirname);
		memcpy(&login_try, &in6addr_any, sizeof(struct in6_addr));
		return 0;
    	}
#endif
	return 1;
    }

    send_authenticate( dirname );
    http_logout(login_ip_tmp);
    return 0;
    }


static void
send_authenticate( char* realm )
    {
    char header[10000];

    (void) snprintf(
	header, sizeof(header), "WWW-Authenticate: Basic realm=\"%s\"", realm );
    send_error( 401, "Unauthorized", header, "Authorization required." );
    }


static void
send_error( int status, char* title, char* extra_header, char* text )
    {
    send_headers( status, title, extra_header, "text/html" );
    (void) fprintf( conn_fp, "<HTML><HEAD><TITLE>%d %s</TITLE></HEAD>\n<BODY BGCOLOR=\"#cc9999\"><H4>%d %s</H4>\n", status, title, status, title );
    (void) fprintf( conn_fp, "%s\n", text );
    (void) fprintf( conn_fp, "</BODY></HTML>\n" );
    (void) fflush( conn_fp );
    }


static void
send_headers( int status, char* title, char* extra_header, char* mime_type )
    {
    time_t now;
    char timebuf[100];

    (void) fprintf( conn_fp, "%s %d %s\r\n", PROTOCOL, status, title );
    (void) fprintf( conn_fp, "Server: %s\r\n", SERVER_NAME );
    now = time( (time_t*) 0 );
    (void) strftime( timebuf, sizeof(timebuf), RFC1123FMT, gmtime( &now ) );
    (void) fprintf( conn_fp, "Date: %s\r\n", timebuf );
    if ( extra_header != (char*) 0 )
	(void) fprintf( conn_fp, "%s\r\n", extra_header );
    if ( mime_type != (char*) 0 )
	(void) fprintf( conn_fp, "Content-Type: %s\r\n", mime_type );
    (void) fprintf( conn_fp, "Connection: close\r\n" );
    (void) fprintf( conn_fp, "\r\n" );
    }


/* Base-64 decoding.  This represents binary data as printable ASCII
** characters.  Three 8-bit binary bytes are turned into four 6-bit
** values, like so:
**
**   [11111111]  [22222222]  [33333333]
**
**   [111111] [112222] [222233] [333333]
**
** Then the 6-bit values are represented using the characters "A-Za-z0-9+/".
*/

static int b64_decode_table[256] = {
    -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,  /* 00-0F */
    -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,  /* 10-1F */
    -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,62,-1,-1,-1,63,  /* 20-2F */
    52,53,54,55,56,57,58,59,60,61,-1,-1,-1,-1,-1,-1,  /* 30-3F */
    -1, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9,10,11,12,13,14,  /* 40-4F */
    15,16,17,18,19,20,21,22,23,24,25,-1,-1,-1,-1,-1,  /* 50-5F */
    -1,26,27,28,29,30,31,32,33,34,35,36,37,38,39,40,  /* 60-6F */
    41,42,43,44,45,46,47,48,49,50,51,-1,-1,-1,-1,-1,  /* 70-7F */
    -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,  /* 80-8F */
    -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,  /* 90-9F */
    -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,  /* A0-AF */
    -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,  /* B0-BF */
    -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,  /* C0-CF */
    -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,  /* D0-DF */
    -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,  /* E0-EF */
    -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1   /* F0-FF */
    };

/* Do base-64 decoding on a string.  Ignore any non-base64 bytes.
** Return the actual number of bytes generated.  The decoded size will
** be at most 3/4 the size of the encoded, and may be smaller if there
** are padding characters (blanks, newlines).
*/
static int
b64_decode( const char* str, unsigned char* space, int size )
    {
    const char* cp;
    int space_idx, phase;
    int d, prev_d=0;
    unsigned char c;

    space_idx = 0;
    phase = 0;
    for ( cp = str; *cp != '\0'; ++cp )
	{
	d = b64_decode_table[(int)*cp];
	if ( d != -1 )
	    {
	    switch ( phase )
		{
		case 0:
		++phase;
		break;
		case 1:
		c = ( ( prev_d << 2 ) | ( ( d & 0x30 ) >> 4 ) );
		if ( space_idx < size )
		    space[space_idx++] = c;
		++phase;
		break;
		case 2:
		c = ( ( ( prev_d & 0xf ) << 4 ) | ( ( d & 0x3c ) >> 2 ) );
		if ( space_idx < size )
		    space[space_idx++] = c;
		++phase;
		break;
		case 3:
		c = ( ( ( prev_d & 0x03 ) << 6 ) | d );
		if ( space_idx < size )
		    space[space_idx++] = c;
		phase = 0;
		break;
		}
	    prev_d = d;
	    }
	}
    return space_idx;
    }


/* Simple shell-style filename matcher.  Only does ? * and **, and multiple
** patterns separated by |.  Returns 1 or 0.
*/
int
match( const char* pattern, const char* string )
    {
    const char* or;

    for (;;)
	{
	or = strchr( pattern, '|' );
	if ( or == (char*) 0 )
	    return match_one( pattern, strlen( pattern ), string );
	if ( match_one( pattern, or - pattern, string ) )
	    return 1;
	pattern = or + 1;
	}
    }


static int
match_one( const char* pattern, int patternlen, const char* string )
    {
    const char* p;

    for ( p = pattern; p - pattern < patternlen; ++p, ++string )
	{
	if ( *p == '?' && *string != '\0' )
	    continue;
	if ( *p == '*' )
	    {
	    int i, pl;
	    ++p;
	    if ( *p == '*' )
		{
		/* Double-wildcard matches anything. */
		++p;
		i = strlen( string );
		}
	    else
		/* Single-wildcard matches anything but slash. */
		i = strcspn( string, "/" );
	    pl = patternlen - ( p - pattern );
	    for ( ; i >= 0; --i )
		if ( match_one( p, pl, &(string[i]) ) )
		    return 1;
	    return 0;
	    }
	if ( *p != *string )
	    return 0;
	}
    if ( *string == '\0' )
	return 1;
    return 0;
    }


void
do_file(char *path, FILE *stream)
{
	FILE *fp;
	int c;

	if (!(fp = fopen(path, "r")))
		return;
	while ((c = getc(fp)) != EOF)
		fputc(c, stream);
	fclose(fp);
}


static void
handle_request(void)
{
    char line[10000], *cur;
    char *method, *path, *protocol, *authorization, *boundary;
    char *cp;
    char *file;
    int len;
    struct mime_handler *handler;
    int cl = 0, flags;
#ifndef __CONFIG_IPV6__
    char *straddr;
#else
    char straddr[INET6_ADDRSTRLEN];
#endif

    /* Initialize the request variables. */
    authorization = boundary = NULL;
    bzero( line, sizeof line );

    /* Parse the first line of the request. */
    if ( fgets( line, sizeof(line), conn_fp ) == (char*) 0 ) {
	send_error( 400, "Bad Request", (char*) 0, "No request found." );
	return;
    }

    method = path = line;
    strsep(&path, " ");
    while (path && *path == ' ') path++;
    protocol = path;
    strsep(&protocol, " ");
    while (protocol && *protocol == ' ') protocol++;
    cp = protocol;
    strsep(&cp, " ");
    if ( !method || !path || !protocol ) {
	send_error( 400, "Bad Request", (char*) 0, "Can't parse request." );
	return;
    }
    cur = protocol + strlen(protocol) + 1;

    
    /* Parse the rest of the request headers. */
    while ( fgets( cur, line + sizeof(line) - cur, conn_fp ) != (char*) 0 )
	{
		

	if ( strcmp( cur, "\n" ) == 0 || strcmp( cur, "\r\n" ) == 0 ){
	    break;
	}
	else if ( strncasecmp( cur, "Authorization:", 14 ) == 0 )
	    {
	    cp = &cur[14];
	    cp += strspn( cp, " \t" );
	    authorization = cp;
	    cur = cp + strlen(cp) + 1;
	    }
	else if (strncasecmp( cur, "Content-Length:", 15 ) == 0) {
		cp = &cur[15];
		cp += strspn( cp, " \t" );
		cl = strtoul( cp, NULL, 0 );
	}
	else if ((cp = strstr( cur, "boundary=" ))) {
            boundary = &cp[9];
	    for( cp = cp + 9; *cp && *cp != '\r' && *cp != '\n'; cp++ );
	    *cp = '\0';
	    cur = ++cp;
	}

	}

    if ( strcasecmp( method, "get" ) != 0 && strcasecmp(method, "post") != 0 ) {
	send_error( 501, "Not Implemented", (char*) 0, "That method is not implemented." );
	return;
    }
    if ( path[0] != '/' ) {
	send_error( 400, "Bad Request", (char*) 0, "Bad filename." );
	return;
    }
    file = &(path[1]);
    len = strlen( file );
    if ( file[0] == '/' || strcmp( file, ".." ) == 0 || strncmp( file, "../", 3 ) == 0 || strstr( file, "/../" ) != (char*) 0 || strcmp( &(file[len-3]), "/.." ) == 0 ) {
	send_error( 400, "Bad Request", (char*) 0, "Illegal filename." );
	return;
    }

    redirect=0;

    //printf("File: %s\n", file);

    if (http_port==server_port && !http_login_check())
    {
#ifndef __CONFIG_IPV6__
	straddr = inet_ntoa(login_ip);
#else
	inet_ntop(AF_INET6, &login_ip, straddr, sizeof(straddr));
#endif
	sprintf(line, "Please log out user %s first or wait for session timeout(60 seconds).", straddr);
	printf("resposne: %s \n", line);
	send_error( 200, "Request is rejected", (char*) 0, line);
	return;
    }
	
    if ( (file[0] == '\0' || file[len-1] == '/'))
    {
	if (strlen(wan_if)>0 && !is_connected() && !is_firsttime() && http_port==server_port)
	{
		redirect=1;
		file="WizardDetect.asp";
	}
	else
	{
                file = "index.asp";
	}
    }	
    
    for (handler = &mime_handlers[0]; handler->pattern; handler++) 
    {
	    if (match(handler->pattern, file)) 
	    {
		    if (handler->auth) 
		    {
			    handler->auth(auth_userid, auth_passwd, auth_realm);
			    if (!auth_check(auth_realm, authorization))
				    return;
		    }

		    if (!redirect) http_login(login_ip_tmp);

		    if (strcasecmp(method, "post") == 0 && !handler->input) 
		    {
			    send_error(501, "Not Implemented", NULL, "That method is not implemented.");
			    return;
		    }			    
		    if (handler->input) {
			    handler->input(file, conn_fp, cl, boundary);
#if defined(linux)
			    if ((flags = fcntl(fileno(conn_fp), F_GETFL)) != -1 &&
				fcntl(fileno(conn_fp), F_SETFL, flags | O_NONBLOCK) != -1) {
				    /* Read up to two more characters */
				    if (fgetc(conn_fp) != EOF)
					    (void) fgetc(conn_fp);
				    fcntl(fileno(conn_fp), F_SETFL, flags);
			    }
#elif defined(vxworks)
			    flags = 1;
			    if (ioctl(fileno(conn_fp), FIONBIO, (int) &flags) != -1) {
				    /* Read up to two more characters */
				    if (fgetc(conn_fp) != EOF)
					    (void) fgetc(conn_fp);
				    flags = 0;
				    ioctl(fileno(conn_fp), FIONBIO, (int) &flags);
			    }
#endif				
		    }
		    send_headers( 200, "Ok", handler->extra_header, handler->mime_type );
		    if (handler->output)
			    handler->output(file, conn_fp);
		    break;
	    }
    }

    if (!handler->pattern) 
	    send_error( 404, "Not Found", (char*) 0, "File not found." );

    if (strcmp(file, "Logout.asp")==0)
    {
	http_logout(login_ip_tmp);
    }	
}

static void http_login_cache(usockaddr *u)
{
#ifndef __CONFIG_IPV6__
    	login_ip_tmp.s_addr = u->sa_in.sin_addr.s_addr;
#else
	memcpy(&login_ip_tmp, &(u->sa_in.sin6_addr), sizeof(struct in6_addr));
#endif
    	//printf("client :%x\n", login_ip_tmp);
}

static void http_login(uaddr ip)
{
	if ((http_port != server_port) ||
#ifndef __CONFIG_IPV6__
	    (ip.s_addr == 0x100007f)
#else
	    (memcmp(&ip, &in6addr_loopback, sizeof(struct in6_addr)) == 0)
#endif
	) return;

#ifndef __CONFIG_IPV6__
	login_ip = ip;
	login_try.s_addr = 0;
#else
	memcpy(&login_ip, &ip, sizeof(struct in6_addr));
	memcpy(&login_try, &in6addr_any, sizeof(struct in6_addr));
#endif

	time(&login_timestamp);
}

static int http_login_check(void)
{
	if ((http_port != server_port) ||
#ifndef __CONFIG_IPV6__
	    (login_ip_tmp.s_addr == 0x100007f)
#else
	    (memcmp(&login_ip_tmp, &in6addr_loopback, sizeof(struct in6_addr)) == 0)
#endif
	) return 1;

	http_login_timeout(login_ip_tmp);

	if (
#ifndef __CONFIG_IPV6__
	    (login_ip.s_addr != 0) &&
	    (login_ip.s_addr != login_ip_tmp.s_addr)
#else
	    (memcmp(&login_ip, &in6addr_any, sizeof(struct in6_addr)) != 0) &&
	    (memcmp(&login_ip, &login_ip_tmp, sizeof(struct in6_addr)) != 0)
#endif
	) return 0;

	return 1;
}

static void http_login_timeout(uaddr ip)
{
	time_t now;
	time(&now);

	//printf("login : %x %x\n", now, login_timestamp);
	if (
#ifndef __CONFIG_IPV6__
	    (ip.s_addr != login_ip.s_addr)
#else
	    (memcmp(&ip, &login_ip, sizeof(struct in6_addr)) != 0)
#endif
	&& (unsigned long)(now - login_timestamp)>60) //one minitues
	{
		http_logout(login_ip);
	}
}

static void http_logout(uaddr ip)
{
	//fprintf(stderr, "ip : %x %x %x\n", ip, login_ip, login_try);
#ifndef __CONFIG_IPV6__
	if (ip.s_addr == login_ip.s_addr)
	{
		login_try = login_ip;
		login_ip.s_addr = 0;
		login_timestamp = 0;
	}
#else
	if (memcmp(&ip, &login_ip, sizeof(struct in6_addr)) == 0)
	{
		memcpy(&login_try, &login_ip, sizeof(struct in6_addr));
		memcpy(&login_ip, &in6addr_any, sizeof(struct in6_addr));
		login_timestamp = 0;
	}
#endif
}

int is_auth(void)
{
	if (http_port==server_port ||
		strcmp(nvram_get_x("PrinterStatus", "usb_webhttpcheck_x"), "1")==0) return 1;
	else return 0;
}

#ifdef REMOVE
static int is_phyconnected(void)
{	
	int fd, err;
	struct ifreq ifr;
	struct ethtool_cmd ecmd;
	
	if (strstr(wan_if, "vlan"))
		return 1;

	memset(&ifr, 0, sizeof(ifr));

	strcpy(ifr.ifr_name, wan_if);
	fd=socket(AF_INET, SOCK_DGRAM, 0);
	if (fd<0)
	{
		//printf("fd error\n");
		return 0;
	}
	ecmd.cmd=ETHTOOL_GSET;
	ifr.ifr_data = (caddr_t)&ecmd;
	err=ioctl(fd, SIOCETHTOOL, &ifr);
	close(fd);
	if (err==0)
	{
		//printf("ecmd: %d\n", ecmd.speed);
		if (ecmd.speed==0)
		{
			nvram_set_x("", "wan_status_t", "Disconnected");
			nvram_set_x("", "wan_reason_t", "Cable is not attached");
		}			
		return(ecmd.speed);
	}
	else 
	{
		//printf("err error\n");
		return 0;
	}
}

int is_fileexist(char *filename)
{
	FILE *fp;

	fp=fopen(filename, "r");

	if (fp==NULL) return 0;
	fclose(fp);
	return 1;
}
#endif

static int is_connected(void)
{
	FILE *fp;
	char line[128], *reason;

	/* check if physical connection exist */
	/* if (!is_phyconnected()) return 0; */
	
	/* check if connection static is CONNECTED */
	if (strcmp(nvram_get_x("WANIPAddress", "wan_status_t"), "Disconnected")==0)
	{
		fp = fopen("/tmp/wanstatus.log", "r");
		if (fp!=NULL)
		{
			fgets(line, sizeof(line), fp);
			reason = strchr(line, ',');
			if (reason!=NULL) nvram_set_x("", "wan_reason_t", reason+1);
			fclose(fp);
		}
		
		return 0;
	}
        //printf("Connected\n");
	return 1;		
}

static int is_firsttime(void)
{
	if (strcmp(nvram_get_x("General", "x_Setting"), "1")==0) return 0;
	else return 1;
}

int main(int argc, char **argv)
{
	usockaddr usa;
	int listen_fd;
	int conn_fd;
	socklen_t sz = sizeof(usa);
	char pidfile[32];

	server_port = atoi(nvram_get_x("", "http_lanport"));
    if (server_port)
      http_port = server_port;
    else
      server_port = http_port;
      
    
    
	// Added by Joey for handling WAN Interface 
	// usage: httpd [wan interface] [port]
	if (argc>2) http_port=atoi(argv[2]);
	if (argc>1) strcpy(wan_if, argv[1]);
	else strcpy(wan_if, "");

	//websSetVer();


	/* Ignore broken pipes */
	signal(SIGPIPE, SIG_IGN);

	/* Initialize listen socket */
	if ((listen_fd = initialize_listen_socket(&usa)) < 0) {
		fprintf(stderr, "can't bind to any address\n" );
		exit(errno);
	}

#if !defined(DEBUG) && !defined(vxworks)
	{
	FILE *pid_fp;
	/* Daemonize and log PID */
	//if (http_port==server_port)
	//{
		if (daemon(1, 1) == -1) 
		{
			perror("daemon");
			exit(errno);
		}
	//}
	if (http_port==server_port)
		strcpy(pidfile, "/var/run/httpd.pid");
	else sprintf(pidfile, "/var/run/httpd-%d.pid", http_port);

	if (!(pid_fp = fopen(pidfile, "w"))) {
		perror(pidfile);
		return errno;
	}
	fprintf(pid_fp, "%d", getpid());
	fclose(pid_fp);
	}
#endif

	/* Loop forever handling requests */
	for (;;) {
		if ((conn_fd = accept(listen_fd, &usa.sa, &sz)) < 0) {
			perror("accept");
			return errno;
		}
		if (!(conn_fp = fdopen(conn_fd, "r+"))) {
			perror("fdopen");
			return errno;
		}		
		http_login_cache(&usa);
		handle_request();
		fflush(conn_fp);
		fclose(conn_fp);
		close(conn_fd);
	}

	shutdown(listen_fd, 2);
	close(listen_fd);

	return 0;
}
