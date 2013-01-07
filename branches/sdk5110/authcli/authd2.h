#define DAEMON_NAME	"authcli"
#define VERSION		258
#define AUTH_VERSION	255

// auth result flags
#define AUTH_NONE	0
#define AUTH_ACCEPT	1
#define AUTH_REJECT	2

#define SERV_TCP_PORT	6326

#define A_ILLEGL	0x0
#define A_KEYREQ	0x1D01
#define A_KEYSND	0x1D02
#define A_PASSND	0x1D04
#define A_PASSND2	0x1D14
#define A_PASSND3	0x1D24
// auth status
#define A_PASASQ	0x1D08
// wrong password
#define A_PASNEG	0xDEAD
// unknown user
#define A_USRUNK	0xF00F
// wrong client
#define A_WRNVER	0x0555


#define AHEX(a) ((a)<='9'?((a)-'0'):(a)-'A'+10)

#define SHA1KEYL	20

#define MAXAREANAME_L 	20
#define MAXUSERNAME_L	20

struct message {
    unsigned attr;
    union {
        // challenge 
	unsigned char challenge[SHA1KEYL];
	unsigned char response[SHA1KEYL];
	// auth structure
	struct {
		unsigned result;
		int balance;
		int limit;
	} auth;
	// byte access
	unsigned char byte[1];
	// dword access
	unsigned dword[1];
    };
};

/* Delay in seconds before reconnect if net is unreachable */
#define RECONNECT_DELAY 60

/* Default unprivileged user to run deamon as */
/* #define UNPRIV_USERNAME "nobody" */
#define UNPRIV_USERNAME NULL

/* Default path for config file */
/* #define DEFAULT_CONF_PATH "/usr/local/etc/authcliw.conf" */
#define DEFAULT_CONF_PATH "/etc/authcliw.conf"

/* Default path for pid file. */
/* #define DEFAULT_PID_PATH "/var/run/authcli.pid" */
#define DEFAULT_PID_PATH NULL

/* Default filename for accounting info */
/* #define DEFAULT_INFO_PATH "/var/lib/authcli/info" */
#define DEFAULT_INFO_PATH NULL;

/* Log invalid auth messages */
#define LOG_INVALID_MESSAGES

/*  If you want debug, please, uncoment this */
/* #define DEBUG */
