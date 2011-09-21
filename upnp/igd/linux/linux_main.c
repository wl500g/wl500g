/*
 * Copyright 2004, Broadcom Corporation
 * All Rights Reserved.
 * 
 * THIS SOFTWARE IS OFFERED "AS IS", AND BROADCOM GRANTS NO WARRANTIES OF ANY
 * KIND, EXPRESS OR IMPLIED, BY STATUTE, COMMUNICATION OR OTHERWISE. BROADCOM
 * SPECIFICALLY DISCLAIMS ANY IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS
 * FOR A SPECIFIC PURPOSE OR NONINFRINGEMENT CONCERNING THIS SOFTWARE.
 *
 * $Id$
 */

#include <errno.h>	    // for errno, of course.
#include <error.h>	    // for perror
#include <signal.h>	    // for signal, etc.
#include <assert.h>	    // for assert, of course.
#include <stdlib.h>	    // for malloc, free, etc.
#include <string.h>	    // for memset, strncasecmp, etc.
#include <stdarg.h>	    // for va_list, etc.
#include <stdio.h>	    // for printf, perror, fopen, fclose, etc.
#include <net/if.h>	    // for struct ifreq, etc.
#include <sys/ioctl.h>	    // for SIOCGIFCONF, etc.
#include <fcntl.h>	    // for fcntl, F_GETFL, etc.
#include <unistd.h>	    // for read, write, etc.
#include <arpa/inet.h>	    // for inet_aton, inet_addr, etc.
#include <time.h>	    // for time
#include <netinet/in.h>	    // for sockaddr_in
#include <wait.h>	    // for sockaddr_in


#include "ctype.h"
#include "upnp_dbg.h"
#include "upnp.h"


extern void define_variable(char *name, char *value);
extern void uuidstr_create(char *);
extern char *strip_chars(char *, char *);
extern void init_devices();
void init_event_queue(int);

extern struct net_connection *net_connections;
extern int global_exit_now;
extern struct iface *global_lans;

#include <syslog.h>
#include <bcmnvram.h>

static timer_t t_nvram1;
static volatile int nvcommit_state = 0;


#ifdef DEBUG
static void logmessage(char *fmt, ...)
{
    va_list args;
    char buf[512];

    va_start(args, fmt);
    vsnprintf(buf, sizeof(buf), fmt, args);
    va_end(args);

    openlog("UPNP", 0, 0);
    syslog(0, buf);
    closelog();
}
#else
 #define logmessage(...)
#endif

static inline void do_nvram_commit()
{
    logmessage("do nvram_commit");

    nvcommit_state = 2;
    nvram_commit();
    nvcommit_state = 0;
}

void req_nvram_commit(int force)
{
    if (nvcommit_state != 2) {
	if (force)
	    do_nvram_commit();
	else
	    nvcommit_state = 1;
    }
}

static void nvram_commit_helper(timer_t t, void *arg)
{
    if (nvcommit_state == 1)
	do_nvram_commit();
}

static void
reap(int sig)
{
	pid_t pid;

	while ((pid = waitpid(-1, NULL, WNOHANG)) > 0)
		UPNP_TRACE(("Reaped %d\n", pid));
}


int main(int argc, char *argv[])
{
    extern char g_wandevs[];
    extern DeviceTemplate IGDeviceTemplate;
    char **argp = &argv[1];
    char *wanif = NULL;
    char *lanif = NULL;
    int daemonize = 0;
    struct itimerspec timer;

    while (argp < &argv[argc]) {
	if (strcasecmp(*argp, "-L") == 0) {
	    lanif = *++argp;
	} 
	else if (strcasecmp(*argp, "-W") == 0) {
	    wanif = *++argp;
	    strcpy(g_wandevs, wanif);
	} 
	else if (strcasecmp(*argp, "-D") == 0) {
	    daemonize = 1;
	}
	argp++;
    }

    init_event_queue(40);

    if (lanif == NULL || wanif == NULL) {
	fprintf(stderr, "usage: %s -L lan_ifname -W wan_ifname [-D]\n", argv[0]);
    } else {
	if (daemonize && daemon(1, 1) == -1) {
	    perror("daemon");
	    exit(errno);
	}

	/* We need to have a reaper for child processes we may create.
	   That happens when we send signals to the dhcp process to
	   release an renew a lease on the external interface. */
	signal(SIGCHLD, reap);

	/* For some reason that I do not understand, this process gets
	   a SIGTERM after sending SIGUSR1 to the dhcp process (to
	   renew a lease).  Ignore SIGTERM to avoid being killed when
	   this happens.  */
	signal(SIGTERM, SIG_IGN);
	signal(SIGUSR1, SIG_IGN);

	memset(&timer, 0, sizeof(timer));
        timer.it_interval.tv_sec = 60;
        timer.it_value.tv_sec = 60;
        t_nvram1 = enqueue_event(&timer, (event_callback_t) nvram_commit_helper, NULL);

	fprintf(stderr, "calling upnp_main\n");
	upnp_main(&IGDeviceTemplate, lanif);

	timer_delete(t_nvram1);

    }
    
    return 0;
}

