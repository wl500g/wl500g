/*
 * Copyright 2004, ASUSTek Inc.
 * All Rights Reserved.
 * 
 * THIS SOFTWARE IS OFFERED "AS IS", AND ASUS GRANTS NO WARRANTIES OF ANY
 * KIND, EXPRESS OR IMPLIED, BY STATUTE, COMMUNICATION OR OTHERWISE. BROADCOM
 * SPECIFICALLY DISCLAIMS ANY IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS
 * FOR A SPECIFIC PURPOSE OR NONINFRINGEMENT CONCERNING THIS SOFTWARE.
 *
 * $Id$
 */

#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <ctype.h>
#include <string.h>
#include <signal.h>
#include <time.h>
#include <unistd.h>
#include <errno.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/stat.h>

#include <shutils.h>

#ifdef ASUS
#define PATH_MAX 256
#endif

/*#define dprintf printf*/

static char *
base64enc(const char *p, char *buf, int len)
{
        char al[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz"
		"0123456789+/";
	char *s = buf;

        while (*p) {
		if (s >= buf+len-4)
			break;
                *(s++) = al[(*p >> 2) & 0x3F];
                *(s++) = al[((*p << 4) & 0x30) | ((*(p+1) >> 4) & 0x0F)];
                *s = *(s+1) = '=';
                *(s+2) = 0;
                if (! *(++p)) break;
                *(s++) = al[((*p << 2) & 0x3C) | ((*(p+1) >> 6) & 0x03)];
                if (! *(++p)) break;
                *(s++) = al[*(p++) & 0x3F];
        }

	return buf;
}

enum {
	METHOD_GET,
	METHOD_POST,
	METHOD_CHECK,
};

static int 
waitsock(int fd, int sec, int usec)
{
	struct timeval tv;
	fd_set fdvar;
	int res;
	
	FD_ZERO(&fdvar);
	FD_SET(fd, &fdvar);
	tv.tv_sec = sec;
	tv.tv_usec = usec;
	res = select(fd+1, &fdvar, NULL, NULL, &tv);

	return res;
}

static int
wget(int method, const char *server, char *buf, size_t count, off_t offset)
{
	char url[PATH_MAX] = { 0 }, *s;
	char *host = url, *path = "", auth[128] = { 0 }, line[1024];
	unsigned short port = 80;
	int fd;
	FILE *fp;
	struct sockaddr_in sin;

	int chunked = 0, len = 0;

	if (server == NULL || !strcmp(server, "")) {
		dprintf("wget: null server input\n");
		return (0);
	}

	strncpy(url, server, sizeof(url));

	/* Parse URL */
	if (!strncmp(url, "http://", 7)) {
		port = 80;
		host = url + 7;
	}
	if ((s = strchr(host, '/'))) {
		*s++ = '\0';
		path = s;
	}
	if ((s = strchr(host, '@'))) {
		*s++ = '\0';
		base64enc(host, auth, sizeof(auth));
		host = s;
	}
	if ((s = strchr(host, ':'))) {
		*s++ = '\0';
		port = atoi(s);
	}

	/* Open socket */
	if (!inet_aton(host, &sin.sin_addr))
		return 0;
	sin.sin_family = AF_INET;
	sin.sin_port = htons(port);

//	dprintf("Connecting to %s:%u...\n", host, port);
	if ((fd = socket(AF_INET, SOCK_STREAM, 0)) < 0 ||
	    connect(fd, (struct sockaddr *) &sin, sizeof(sin)) < 0
		 ||
	    (!(method == METHOD_CHECK) && !(fp = fdopen(fd, "r+")))
		) {
		perror(host);
		if (fd >= 0)
			close(fd);
		return 0;
	}

//	dprintf("connected!\n");

	/* Send HTTP request */
	sprintf(line, "%s /%s HTTP/1.1\r\n", method == METHOD_POST ? "POST" : "GET", path);
	sprintf(line, "%sHost: %s\r\n", line, host);
	sprintf(line, "%sUser-Agent: wget\r\n", line);
	if (strlen(auth))
		sprintf(line, "%sAuthorization: Basic %s\r\n", line, auth);
	if (offset)
		sprintf(line, "%sRange: bytes=%ld-\r\n", line, offset);

	if (method == METHOD_POST) {
		sprintf(line, "%sContent-Type: application/x-www-form-urlencoded\r\n", line);
		sprintf(line, "%sContent-Length: %d\r\n\r\n", line, (int) strlen(buf));
		sprintf(line, "%s%s", line, buf);
	} else
		sprintf(line,"%sConnection: close\r\n\r\n", line);

	/* Check HTTP response */
//	dprintf("HTTP request sent, awaiting response..\n");
	write(fd, line, strlen(line));

	if (waitsock(fd, 2, 0) && read(fd, line, sizeof(line))) 
	{
		// get message anyway
		close(fd);
		return 1;
		if (!(method == METHOD_CHECK)) {
			for (s = line; *s && !isspace((int)*s); s++);
			for (; isspace((int)*s); s++);
			switch (atoi(s)) {
			case 200: if (offset) goto done; else break;
			case 206: if (offset) break; else goto done;
			default: goto done;
			}
		}
	}
	else
	{
		close(fd); 
		return 0;
	}

	if ((method == METHOD_CHECK)) return 0;

	/* Parse headers */
	while (fgets(line, sizeof(line), fp)) {
		dprintf("%s", line);
		for (s = line; *s == '\r'; s++);
		if (*s == '\n')
			break;
		if (!strncasecmp(s, "Content-Length:", 15)) {
			for (s += 15; isblank(*s); s++);
			chomp(s);
			len = atoi(s);
		}
		else if (!strncasecmp(s, "Transfer-Encoding:", 18)) {
			for (s += 18; isblank(*s); s++);
			chomp(s);
			if (!strncasecmp(s, "chunked", 7))
				chunked = 1;
		}
	}

	if (chunked && fgets(line, sizeof(line), fp))
		len = strtol(line, NULL, 16);
	
	len = (len > count) ? count : len;
	len = fread(buf, 1, len, fp);

 done:
	/* Close socket */
	fflush(fp);
	fclose(fp);
	return len;
}

/* Checks existence of process in memory by pidfile
 *  returns zero if process seems to be dead
 */
int
proc_check_pid(const char *pidfile)
{
	FILE *fp;
	pid_t pid;
	char tmp[32];
	struct stat f_st;

	fp = fopen(pidfile, "r");
	if (fp && fgets(tmp, sizeof(tmp), fp))
	{
		fclose(fp);
		pid = strtoul(tmp, NULL, 0);
		if (pid > 0)
		{
			memset(tmp, 0, sizeof(tmp));
			sprintf(tmp, "/proc/%d", pid);
			if (lstat(tmp, &f_st) == 0)
				return S_ISDIR(f_st.st_mode);
		}
	}

	return 0;
}

int
http_check(const char *server, char *buf, size_t count, off_t offset)
{
	return wget(METHOD_CHECK, server, buf, count, offset);
}

int
http_get(const char *server, char *buf, size_t count, off_t offset)
{
	return wget(METHOD_GET, server, buf, count, offset);
}

int
http_post(const char *server, char *buf, size_t count)
{
        /* No continuation generally possible with POST */
        return wget(METHOD_POST, server, buf, count, 0);
}
