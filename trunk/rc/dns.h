typedef u_int16_t uint16;
typedef u_int32_t uint32;
#define NAME_SIZE 255
#define MAX_PACKET_SIZE 512
#define NUM_RRS 5
/*****************************************************************************/
struct dns_rr{
  char name[NAME_SIZE];
  uint16 type;
  uint16 class;
  uint32 ttl;
  uint16 rdatalen;
  char data[NAME_SIZE];
};
/*****************************************************************************/
union header_flags {
  uint16 flags;
  
  struct {
    unsigned short int rcode:4;
    unsigned short int unused:3;
    unsigned short int recursion_avail:1;
    unsigned short int want_recursion:1;
    unsigned short int truncated:1;
    unsigned short int authorative:1;
    unsigned short int opcode:4;
    unsigned short int question:1;
  } f;
};
/*****************************************************************************/
struct dns_header_s{
  uint16 id;
  union header_flags flags;
  uint16 qdcount;
  uint16 ancount;
  uint16 nscount;
  uint16 arcount;
};
/*****************************************************************************/
struct dns_message{
  struct dns_header_s header;
  struct dns_rr question[NUM_RRS];
  struct dns_rr answer[NUM_RRS];
};
/*****************************************************************************/
typedef struct dns_request_s{
  char cname[NAME_SIZE];
  char ip[20];
  int ttl;
  int time_pending; /* request age in seconds */

  /* the actual dns request that was recieved */
  struct dns_message message;

  /* where the request came from */
  struct in_addr src_addr;
  int src_port;

  /* the orginal packet */
  char original_buf[MAX_PACKET_SIZE];
  int numread;
  char *here;

  /* next node in list */
  struct dns_request_s *next;
}dns_request_t;
/*****************************************************************************/
/* TYPE values */
enum{ A = 1,      /* a host address */
	NS,       /* an authoritative name server */
	MD,       /* a mail destination (Obsolete - use MX) */
	MF,       /* */
	CNAME,    /* the canonical name for an alias */
	SOA,      /* marks the start of a zone of authority  */
	MB,       /* a mailbox domain name (EXPERIMENTAL) */
	MG,       /* */
	MR,       /* */
	NUL,      /* */
	WKS,      /* a well known service description */
	PTR,      /* a domain name pointer */
	HINFO,    /* host information */
	MINFO,    /* mailbox or mail list information */
	MX,       /* mail exchange */
	TXT,      /* text strings */

	AAA = 0x1c /* IPv6 A */
	};

/* CLASS values */
enum{
  IN = 1,         /* the Internet */
    CS,
    CH,
    HS
};

/* OPCODE values */
enum{
  QUERY,
    IQUERY,
    STATUS
};
