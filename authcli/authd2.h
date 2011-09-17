#define SERV_TCP_PORT	6326

#define A_ILLEGL	0x0
#define A_KEYREQ	0x1D01
#define A_KEYSND	0x1D02
#define A_PASSND	0x1D04
#define A_PASSND2	0x1D14
#define A_PASSND3	0x1D24
#define A_PASASQ	0x1D08
#define A_PASNEG	0xDEAD
#define A_USRUNK	0xF00F
#define A_WRNVER	0x0555

#define SHA1KEYL	20

struct message {
    unsigned atr;
    unsigned char msg[20];
};

#define	ALARMSC		2
#define PERIOD		10

/* Default path for pid file. */
#define	DEFAUL_PID_PATH	"/var/run/authcli.pid"


/*  If you want debug, please, uncoment this */
//#define DEBUG

#define DEFAULT_CONF_PATH "/usr/local/etc/authcliw.conf"
