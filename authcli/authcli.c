/*
*
*  Copyright (c) ISP "Convex"
*  Copyright (c) 2007 spiritus (sirspiritus@yandex.ru)
*  Copyright (c) 2009 theMIROn (themiron@mail.ru)
*
*/

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>
#include <syslog.h>
#include <sys/param.h>
#include <sys/resource.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdarg.h>
#include <pwd.h>
#include <time.h>
#include <errno.h>
#include "sha1.h"
#include "authd2.h"

static char AREANAME[MAXAREANAME_L+1];
static char USERNAME[MAXUSERNAME_L+1];
static char HOSTNAME[] = "172.20.255.1";
static char MKEY[SHA1KEYL*2+1];
static unsigned char NKEY[SHA1KEYL];

static unsigned a_passnd = A_PASSND3 | (AUTH_VERSION<<16);

static volatile int loop_flag  = 1;

// child process PID
static int child_pid = 0;
// current client IP
static char* pstrclientip = NULL;

static char *conf_name = DEFAULT_CONF_PATH;
static char *pid_name  = DEFAULT_PID_PATH;
static char *info_name = DEFAULT_INFO_PATH;
static char *user_name = UNPRIV_USERNAME;

#define AHEX(a) ((a)<='9'?((a)-'0'):(a)-'A'+10)

/*******************************************************
 *
 *           Read configuration function
 *
 *******************************************************/

static int readconfig(void)
{
    FILE *f;
    char buf[256], *p;
    int i;

    AREANAME[0] = '\0';
    USERNAME[0] = '\0';
    MKEY[0] = '\0';
    memset(NKEY, 0, sizeof(NKEY));

    f = fopen(conf_name, "r");
    if (!f) return 0;

    while (!feof(f)) {
	fscanf(f, "%255s\n", buf);

	if (!strncmp(buf, "key",3)) {
	    if ((p = strchr(buf, '='))) {
		strncpy(MKEY, p+1, sizeof(MKEY)-1);
		MKEY[sizeof(MKEY)-1] = '\0';
		for (i=0; i<(sizeof(MKEY)-1) && MKEY[i]; i++) {
		      NKEY[i] = AHEX(MKEY[i*2])*16 + AHEX(MKEY[i*2+1]);
		}
	    }
	} else
	if (!strncmp(buf, "area", 4)) {
	    if ((p = strchr(buf, '='))) {
		strncpy(AREANAME, p+1, MAXAREANAME_L);
		AREANAME[MAXAREANAME_L] = '\0';
	    }
	} else
	if (!strncmp(buf, "user", 4)) {
	    if ((p = strchr(buf, '='))) {
		strncpy(USERNAME, p+1, MAXUSERNAME_L);
		USERNAME[MAXUSERNAME_L] = '\0';
	    }
	}
    }
    fclose(f);
    return (MKEY[0] != 0);
}

/*******************************************************
*
*            Logging functions
*
*******************************************************/

static void log_common(int prio, const char *fmt, va_list ap)
{
    char buf[256];

    vsnprintf(buf, sizeof(buf), fmt, ap);
    openlog(DAEMON_NAME, LOG_PID | LOG_CONS, LOG_DAEMON);
    syslog(prio, buf);
    closelog();
}

static void log_msg(char *fmt, ...)
{
    va_list ap;

    va_start(ap, fmt);
    log_common(LOG_NOTICE, fmt, ap);
    va_end(ap);
}

static void log_err(char *fmt, ...)
{
    va_list ap;

    va_start(ap, fmt);
    log_common(LOG_ERR, fmt, ap);
    va_end(ap);
}

#ifdef DEBUG
static void log_dbg(char *fmt, ...)
{
    va_list ap;

    va_start(ap, fmt);
    log_common(LOG_DEBUG,fmt,ap);
    va_end(ap);
}
#else
#define log_dbg(fmt, ...)
#endif

static void authcli_term(int sig)
{
    log_msg("signal %d received. %s stoped.", sig, DAEMON_NAME);
    loop_flag = 0;
    if (child_pid) kill(child_pid, SIGKILL);  // kill faststart child
}

static void authcli_hup(int sig)
{
    log_msg("SIGHUP received. Please, use stop and then start to reconfigure.");
}

static void authcli_child(int sig) 
{
    pid_t pid;
    while (((pid = waitpid(-1, NULL, WNOHANG)) > 0) ||
	    (pid < 0 && errno == EINTR));
}

/*******************************************************
*
*  Function which creates socket for accept
*
*******************************************************/

static int make_socket(unsigned short int port)
{
    int sock;
    struct sockaddr_in name;

    /* Create the socket */
    sock = socket (PF_INET, SOCK_STREAM, 0);

    /* If socet is't created then exit with EXIT_FAILURE */
    if (sock < 0) {
	log_err("can't create main socket");
	exit(EXIT_FAILURE);
    }

    /* Bind socket to port */
    name.sin_family = AF_INET;
    name.sin_port = htons(port);
    name.sin_addr.s_addr = htonl(INADDR_ANY);
    if (bind(sock, (struct sockaddr*)&name, sizeof(name)) < 0) {
	log_msg("can't bind to main socket");
	exit (EXIT_FAILURE);
    }

    return sock;
}

static int recv_message(int fd, struct message * msg)
/*
 *  Receive message from remote host
 *
 *  Result	Status
 *  -1   	Network error or inalid message received
 *   0		Remote host closed connection
 *   1		Success
 */
{
    int  nb;

    nb = recv(fd, msg, sizeof(*msg), 0);

    if (nb > 0)
	log_dbg("message received: attr %04X msg %X %X %X len %d",
	    msg->attr, msg->dword[0], msg->dword[1], msg->dword[2], nb);

    if (nb == 0) {		/* Remote host closed connection. exit with err */
	log_dbg("host %s closed connection", pstrclientip);
	return 0;
    } else
    if (nb < 0) {		/* Read error */
	#ifdef LOG_INVALID_MESSAGES
	log_err("error reading message from %s: %s", pstrclientip, strerror(errno));
	#endif
	return -1;
    } else
    if (nb != sizeof(*msg)) {	/* Message size mismatch */
	#ifdef LOG_INVALID_MESSAGES
        log_err("invalid message received from %s: attr %04X msg %X %X %X len %d",
	    pstrclientip, msg->attr, msg->dword[0], msg->dword[1], msg->dword[2], nb);
	#endif
	return -1;
    }

    return 1;
}

static int send_message(int fd, struct message * msg)
/*
 *  Send message to remote host
 *
 *  Result	Status
 *  -1   	Network error or invalid message received
 *   1		Success
 */
{
    int  nb;

    nb = send(fd, msg, sizeof(*msg), 0);

    if (nb > 0)
	log_dbg("sending message: attr %04X msg %X %X %X len %d",
	    msg->attr, msg->dword[0], msg->dword[1], msg->dword[2], sizeof(*msg));

    if (nb <= 0) {
	#ifdef LOG_INVALID_MESSAGES
	log_err("error sending message to %s: %s", pstrclientip, strerror(errno));
	#endif
	return -1;
    }

    return 1;
}

inline int init_sockaddr(
    struct sockaddr_in *name,
    const char *host,
    unsigned short int port)
{
    bzero(name, sizeof(struct sockaddr_in));
    name->sin_family = AF_INET;
    name->sin_port = htons(port);
    name->sin_addr.s_addr = inet_addr(host);

    return 1;
}

static void request_auth_child(void)
{
    int sock;
    int ret;
    struct sockaddr_in serveraddr;

    sock = socket (PF_INET, SOCK_STREAM, 0);
    if ((sock > 0) && init_sockaddr(&serveraddr, HOSTNAME, SERV_TCP_PORT)) {
	log_dbg("connecting to auth server at %s", inet_ntoa(serveraddr.sin_addr));

	while(1) {
	    ret = connect(sock, (struct sockaddr*) &serveraddr, sizeof(serveraddr));
	    if (ret == ENETUNREACH) {
		log_dbg("network unreachable - reconnecting");
		sleep(RECONNECT_DELAY);
	    } else
	       break;
	}

	if (!ret) {
	    log_dbg("sending init request");
	    send(sock, "IMAL", 4, 0);
	} else
	    log_dbg("connect error: %s", strerror(errno));
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
    int auth_state = AUTH_NONE;
    char cryptobuf[SHA1KEYL*2];
    char buf[256];
    int i, res, nullfd;
    int sock, newsock;
    fd_set active_fd_set, read_fd_set;
    struct sockaddr_in clientaddr;
    size_t size;
    FILE *fd;
    /*  previous balance and limit status  */
    int balance = 0, limit = 0;
    int opt;

    while ((opt = getopt(argc, argv, ":c:p:i:u:h")) != -1) {
	switch (opt) {
	    case 'c':
		conf_name = optarg;
		break;
	    case 'p':
		pid_name = optarg;
		break;
	    case 'i':
		info_name = optarg;
		break;
	    case 'u':
		user_name = optarg;
		break;
	    case ':':
		log_err("wrong parameters");
	    case 'h':
	    default:
		fprintf(stderr,
		    "usage: %s [options]\n"
		    "options:\n"
		    "  -c <conf_file>\n"
		    "  -p <pid_file>\n"
		    "  -c <info_file>\n"
		    "  -u <user_name>\n", DAEMON_NAME);
		exit(EXIT_FAILURE);
	}
    }

    if (!readconfig()) {
        sprintf(buf, "can't read configuration file %s: %s\n", conf_name, strerror(errno));
	fprintf(stderr, buf);
	log_err(buf);
	exit (EXIT_FAILURE);
    }

    /* close standard I/O handles */
    nullfd = open("/dev/null", O_RDWR);
    if (nullfd < 0) {
	perror("/dev/null");
    } else {
	dup2(nullfd,0);
	dup2(nullfd,1);
	dup2(nullfd,2);
	close(nullfd);
    }

    if(fork()) {
	exit(EXIT_SUCCESS);
    }

    /* Daemon is started */

    /* Create the socket and set it up to accept connections */
    sock = make_socket(SERV_TCP_PORT);
    if (listen(sock, 3) < 0) {
	log_err("can't listen to main socket");
	exit(EXIT_FAILURE);
    }

    /* Set stop signal */
    signal(SIGINT, authcli_term);
    signal(SIGTERM, authcli_term);
    signal(SIGQUIT, authcli_term);

    /* Set SIGCHLD */
    signal(SIGCHLD, authcli_child);

    /* Set SIGHUP */
    signal(SIGHUP, authcli_hup);

    log_msg("%s v.%.2f started.", DAEMON_NAME, VERSION/100.);
    log_msg("listening on TCP port %d", SERV_TCP_PORT);

    /*  Write PID file */
    if (pid_name) {
	pid_t pid = getpid();
	if (pid < 0) {
	    log_err("can't get pid");
	    exit(EXIT_FAILURE);
	}
	fd = fopen(pid_name, "w");
	if (fd != NULL) {
	    fprintf(fd, "%d", pid);
	    fclose(fd);
	} else
	    log_err("can't create pid file %s: %s", pid_name, strerror(errno));
    }

    /* Switch to unpriviledged account */
    if (user_name) {
	struct passwd* ppwd;

	ppwd = getpwnam(user_name); 
	if (ppwd) {
	    log_dbg("switching to unprivileged user \'%s\' (UID %d, GID %d)",
		user_name, ppwd->pw_uid, ppwd->pw_gid);
	    setuid(ppwd->pw_uid);
	    setgid(ppwd->pw_gid);
	} else
	    log_dbg("can't switch to unprivileged user \'%s\'", user_name);
    }

    child_pid = fork();
    if (!child_pid) {
	sleep(1);  // wait for parent network init
	request_auth_child();
	exit(EXIT_SUCCESS);
    }

    /* Initialize the set of active sockets */
    FD_ZERO(&active_fd_set);
    FD_SET(sock, &active_fd_set);

    while (loop_flag) {
	/* Block until input arrives on one or more active sockets */
	read_fd_set = active_fd_set;
	if (select(FD_SETSIZE, &read_fd_set, NULL, NULL, NULL) < 0) {
	    log_dbg("select error: %s", strerror(errno));
	    continue;
	}

	/* Service all the sockets with input pending */
	for (i = 0; i < FD_SETSIZE; i++) {
	    if (FD_ISSET(i, &read_fd_set)) {
		if (i == sock) {  /* New connection request on original socket.  */
		    size = sizeof(clientaddr);
		    if ((newsock = accept(sock, (struct sockaddr*)&clientaddr, (void*)&size)) < 0) {
        		log_err("error accepting connection");
			exit(EXIT_FAILURE);
		    }

		    pstrclientip = inet_ntoa(clientaddr.sin_addr);
		    log_dbg("new connection from %s", pstrclientip);

		    FD_SET(newsock, &active_fd_set);

		} else {  /* Data arriving on an already-connected socket */
		    struct message mr, ms;

		    log_dbg("receiving data");

		    memset((void*)&mr, 0, sizeof(mr)); // clear message structure before recv
		    res = recv_message(i, &mr);
		    if (res < 1) {
			if (res != 0) {
			    #ifndef LOG_INVALID_MESSAGES
			    log_err("error receiving auth message");
			    #endif
			}
        		close(i);
        		FD_CLR(i, &active_fd_set);
			continue;
		    }

		    if(mr.attr == A_KEYSND) {
                	memset((void*)&ms, 0, sizeof(ms)); // clear send buffer
			ms.attr = a_passnd;
			a_passnd = A_PASSND2 | (AUTH_VERSION<<16);

			memcpy(cryptobuf, mr.challenge,  SHA1KEYL);
			memcpy(cryptobuf+SHA1KEYL, NKEY, SHA1KEYL);
			sha1encode_strn((unsigned char*)cryptobuf, SHA1KEYL*2, ms.response);

			if (!send_message(i, &ms)) {
			    #ifndef LOG_INVALID_MESSAGES
			    log_msg("error sending auth message");
			    #endif
			    close(i);
			    FD_CLR(i, &active_fd_set);
			}
			continue;
		    } else

		    if (mr.attr == A_PASASQ) {
			int balance_cur, limit_cur;

			if (mr.auth.result == AUTH_ACCEPT) {
			    balance_cur = mr.auth.balance;
			    limit_cur = mr.auth.limit;

			    /* log balance only if it has been changed or if this is the
			     * first time we've got auth accept message	*/
			    if ((balance_cur != balance) || (limit_cur != limit) || 
				(auth_state != AUTH_ACCEPT)) {
				/* dump info */
				if (info_name) {
				    fd = fopen(info_name, "w");
				    if (fd != NULL) {
					fprintf(fd,
					    "BALANCE=%.2f\n"
					    "LIMIT=%.2f\n",
					    balance_cur/100., limit_cur/100.);
					fclose(fd);
				    } else
					log_err("can't create info file %s: %s", info_name, strerror(errno));
				}
				if (balance_cur < limit_cur) {
				    log_msg("low balance %.2f",balance_cur/100.);
				} else {
				    log_msg("balance %.2f",balance_cur/100.);
				}
				balance = balance_cur;
				limit = limit_cur;
			    }

			    if (auth_state != AUTH_ACCEPT) {
				log_msg("auth key accepted");
				auth_state = AUTH_ACCEPT;
			    }
			}
		    } else

		    if (mr.attr == A_PASNEG) {
			log_msg("invalid password");
                	auth_state = AUTH_REJECT;
                    } else

		    if (mr.attr == A_WRNVER) {
			log_msg("wrong client version");
		    } else

		    if (auth_state != AUTH_REJECT) {
		        log_msg("auth key rejected");
		        auth_state = AUTH_REJECT;
		    }

		    close(i);
		    FD_CLR(i, &active_fd_set);
		}
	    } /* end if */
	} /* end for */
    } /* end while */

    /* Clean up pid and info files
     * Note: Take care of unprivileged user permissions */
    if (info_name)
	unlink(info_name);
    if (pid_name)
	unlink(pid_name);
    return 0;
}
