/***************************************************************************
 * LPRng - An Extended Print Spooler System
 *
 * Copyright 1988-2003, Patrick Powell, San Diego, CA
 *     papowell@lprng.com
 * See LICENSE for conditions of use.
 * $Id: gethostinfo.h,v 1.57 2003/09/05 20:07:20 papowell Exp $
 ***************************************************************************/



#ifndef _GETHOSTINFO_H_ 
#define _GETHOSTINFO_H_ 1

/*****************************************************************
 * Get Fully Qualified Domain Name (FQDN) host name
 * The 'domain' information is appended to the host name only in
 * desperation cases and only if it is non-null.
 * If the FQDN cannot be found,  then you fall back on the host name.
 *****************************************************************/


/*****************************************************************
 * Host name and address information
 *  The gethostbyname function returns a pointer to a structure
 *  which contains all the host information.  But this
 *  value disappears or is modified on the next call.  We save
 *  the information  using this structure.  Also,  we look
 *  forward to the IPV6 structure,  where we need a structure
 *  for an address.
 *****************************************************************/

struct host_information{
	char *shorthost;
	char *fqdn;
	struct line_list host_names;	/* official name of host is first */
	int h_addrtype;		/* address type */
	int h_length;		/* address length */
	struct line_list h_addr_list;	/* address list */
};

EXTERN struct host_information Localhost_IP;	/* IP from localhost lookup */
EXTERN struct host_information Host_IP;	/* current host ip */
EXTERN struct host_information RemoteHost_IP;	/* IP from localhost lookup */
EXTERN struct host_information LookupHost_IP;	/* for lookup */
EXTERN struct host_information PermHost_IP;		/* for permission lookup */

/* PROTOTYPES */
void Clear_host_information( struct host_information *info );
void Clear_all_host_information(void);
void Check_for_dns_hack( struct hostent *h_ent );
char *Find_fqdn( struct host_information *info, const char *shorthost );
char *Fixup_fqdn( const char *shorthost, struct host_information *info,
	struct hostent *host_ent );
void Get_local_host( void );
char *Get_hostinfo_byaddr( struct host_information *info,
	struct sockaddr *sinaddr, int addr_only );
char *Get_remote_hostbyaddr( struct host_information *info,
	struct sockaddr *sinaddr, int force_ip_addr_use );
int Same_host( struct host_information *host,
	struct host_information *remote );
void Dump_host_information( char *title,  struct host_information *info );
void form_addr_and_mask(char *v, char *addr,char *mask,
	int addrlen, int family );
int cmp_ip_addr( char *h, char *a, char *m, int len );
int Match_ipaddr_value( struct line_list *list, struct host_information *host );

#endif
