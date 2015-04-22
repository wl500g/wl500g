/***************************************************************************
 * LPRng - An Extended Print Spooler System
 *
 * Copyright 1988-2003, Patrick Powell, San Diego, CA
 *     papowell@lprng.com
 * See LICENSE for conditions of use.
 *
 ***************************************************************************/

 static char *const _id =
"$Id: lpd_dispatch.c,v 1.57 2003/09/05 20:07:19 papowell Exp $";


#include "lp.h"
#include "errorcodes.h"
#include "getqueue.h"
#include "getprinter.h"
#include "gethostinfo.h"
#include "linksupport.h"
#include "child.h"
#include "fileopen.h"
#include "permission.h"
#include "proctitle.h"
#include "lpd_rcvjob.h"
#include "lpd_remove.h"
#include "lpd_status.h"
#include "lpd_control.h"
#include "lpd_secure.h"
#include "krb5_auth.h"
#include "lpd_dispatch.h"

void Dispatch_input(int *talk, char *input )
{
	switch( input[0] ){
		default:
			FATAL(LOG_INFO)
				_("Dispatch_input: bad request line '%s'"), input );
			break;
		case REQ_START:
			/* simply send a 0 ACK and close connection - NOOP */
			Write_fd_len( *talk, "", 1 );
			break;
		case REQ_RECV:
			Receive_job( talk, input );
			break;
		case REQ_DSHORT:
		case REQ_DLONG:
		case REQ_VERBOSE:
			Job_status( talk, input );
			break;
		case REQ_REMOVE:
			Job_remove( talk, input );
			break;
		case REQ_CONTROL:
			Job_control( talk, input );
			break;
		case REQ_BLOCK:
			Receive_block_job( talk, input );
			break;
		case REQ_SECURE:
			Receive_secure( talk, input );
			break;
#if defined(KERBEROS) && defined(MIT_KERBEROS4)
		case REQ_K4AUTH:
			Receive_k4auth( talk, input );
			break;
#endif
	}
}

void Service_all( struct line_list *args )
{
	int i, reportfd, fd, printable, held, move, printing_enabled,
		server_pid, change, error, done, do_service;
	char buffer[SMALLBUFFER], *pr, *forwarding;
	struct stat statb;
	int first_scan;
	char *remove_prefix = 0;
	
	/* we start up servers while we can */
	Name = "SERVICEALL";
	setproctitle( "lpd %s", Name );

	first_scan = Find_flag_value(args,FIRST_SCAN,Value_sep);
	reportfd = Find_flag_value(args,INPUT,Value_sep);
	Free_line_list(args);

	if(All_line_list.count == 0 ){
		Get_all_printcap_entries();
	}
	for( i = 0; i < All_line_list.count; ++i ){
		Set_DYN(&Printer_DYN,0);
		Set_DYN(&Spool_dir_DYN,0);
		pr = All_line_list.list[i];
		DEBUG1("Service_all: checking '%s'", pr );
		if( Setup_printer( pr, buffer, sizeof(buffer), 0) ) continue;
		/* now check to see if there is a server and unspooler process active */
		server_pid = 0;
		remove_prefix = 0;
		if( first_scan ){
			remove_prefix = Fifo_lock_file_DYN;
		}
		if( (fd = Checkread( Printer_DYN, &statb ) ) > 0 ){
			server_pid = Read_pid( fd, (char *)0, 0 );
			close( fd );
		}
		DEBUG3("Service_all: printer '%s' checking server pid %d", Printer_DYN, server_pid );
		if( server_pid > 0 && kill( server_pid, 0 ) == 0 ){
			DEBUG3("Get_queue_status: server %d active", server_pid );
			continue;
		}
		change = Find_flag_value(&Spool_control,CHANGE,Value_sep);
		printing_enabled = !(Pr_disabled(&Spool_control) || Pr_aborted(&Spool_control));

		Free_line_list( &Sort_order );
		if( Scan_queue( &Spool_control, &Sort_order,
				&printable,&held,&move, 1, &error, &done, 0, 0  ) ){
			continue;
		}
		forwarding = Find_str_value(&Spool_control,FORWARDING,Value_sep);
		do_service = 0;
		if( !(Save_when_done_DYN || Save_on_error_DYN )
			&& (Done_jobs_DYN || Done_jobs_max_age_DYN)
			&& (error || done ) ){
			do_service = 1;
		}
		if( do_service || change || move || (printable && (printing_enabled||forwarding)) ){
			if( Server_queue_name_DYN ){
				pr = Server_queue_name_DYN;
			} else {
				pr = Printer_DYN;;
			}
			DEBUG1("Service_all: starting '%s'", pr );
			SNPRINTF(buffer,sizeof(buffer))".%s\n",pr );
			if( Write_fd_str(reportfd,buffer) < 0 ) cleanup(0);
		}
	}
	Free_line_list( &Sort_order );
	Errorcode = 0;
	cleanup(0);
}

/***************************************************************************
 * Service_connection( struct line_list *args )
 *  Service the connection on the talk socket
 * 1. fork a connection
 * 2. Mother:  close talk and return
 * 2  Child:  close listen
 * 2  Child:  read input line and decide what to do
 *
 ***************************************************************************/

void Service_connection( struct line_list *args )
{
	char input[SMALLBUFFER];
	char buffer[LINEBUFFER];	/* for messages */
	int len, talk;
	int status;		/* status of operation */
	int permission;
	int port = 0;
	struct sockaddr sinaddr;

	memset( &sinaddr, 0, sizeof(sinaddr) );
	Name = "SERVER";
	setproctitle( "lpd %s", Name );
	(void) plp_signal (SIGHUP, cleanup );

	if( !(talk = Find_flag_value(args,INPUT,Value_sep)) ){
		Errorcode = JABORT;
		FATAL(LOG_ERR)"Service_connection: no talk fd"); 
	}

	DEBUG1("Service_connection: listening fd %d", talk );

	Free_line_list(args);

	/* make sure you use blocking IO */
	Set_block_io(talk);

	{
#if defined(HAVE_SOCKLEN_T)
		socklen_t len;
#else
		int len;
#endif
		len = sizeof( sinaddr );
		if( getpeername( talk, &sinaddr, &len ) ){
			LOGERR_DIE(LOG_DEBUG) _("Service_connection: getpeername failed") );
		}
	}

	DEBUG1("Service_connection: family %d, "
#ifdef AF_LOCAL
		"AF_LOCAL %d,"
#endif
#ifdef AF_UNIX
		"AF_UNIX %d"
#endif
	"%s" , sinaddr.sa_family,
#ifdef AF_LOCAL
	AF_LOCAL,
#endif
#ifdef AF_UNIX
	AF_UNIX,
#endif
	"");
	if( sinaddr.sa_family == AF_INET ){
		port = ((struct sockaddr_in *)&sinaddr)->sin_port;
#if defined(IPV6)
	} else if( sinaddr.sa_family == AF_INET6 ){
		port = ((struct sockaddr_in6 * )&sinaddr)->sin6_port;
#endif
	} else if( sinaddr.sa_family == 0
#if defined(AF_LOCAL)
	 	|| sinaddr.sa_family == AF_LOCAL
#endif
#if defined(AF_UNIX)
	 	|| sinaddr.sa_family == AF_UNIX
#endif
		){
		/* force the localhost address */
		int len;
		void *s, *addr;
		memset( &sinaddr, 0, sizeof(sinaddr) );
		Perm_check.unix_socket = 1;
	 	sinaddr.sa_family = Localhost_IP.h_addrtype;
		len = Localhost_IP.h_length;
		if( sinaddr.sa_family == AF_INET ){
			addr = &(((struct sockaddr_in *)&sinaddr)->sin_addr);
#if defined(IPV6)
		} else if( sinaddr->sa_family == AF_INET6 ){
			addr = &(((struct sockaddr_in6 *)&sinaddr)->sin6_addr);
#endif
		} else {
			FATAL(LOG_INFO) _("Service_connection: BAD LocalHost_IP value"));
			addr = 0;
		}
		s = Localhost_IP.h_addr_list.list[0];
		memmove(addr,s,len);
	} else {
		FATAL(LOG_INFO) _("Service_connection: bad protocol family '%d'"), sinaddr.sa_family );
	}

	DEBUG2("Service_connection: socket %d, ip '%s' port %d", talk,
		inet_ntop_sockaddr( &sinaddr, buffer, sizeof(buffer) ), ntohs( port ) );

	/* get the remote name and set up the various checks */

	Get_remote_hostbyaddr( &RemoteHost_IP, &sinaddr, 0 );
	Perm_check.remotehost  =  &RemoteHost_IP;
	Perm_check.host = &RemoteHost_IP;
	Perm_check.port =  ntohs(port);

	len = sizeof( input ) - 1;
	memset(input,0,sizeof(input));
	DEBUG1( "Service_connection: starting read on fd %d", talk );

	status = Link_line_read(ShortRemote_FQDN,&talk,
		Send_job_rw_timeout_DYN,input,&len);
	if( len >= 0 ) input[len] = 0;
	DEBUG1( "Service_connection: read status %d, len %d, '%s'",
		status, len, input );
	if( len == 0 ){
		DEBUG3( "Service_connection: zero length read" );
		cleanup(0);
	}
	if( status ){
		LOGERR_DIE(LOG_DEBUG) _("Service_connection: cannot read request") );
	}
	if( len < 2 ){
		FATAL(LOG_INFO) _("Service_connection: bad request line '%s', from '%s'"),
			input, inet_ntop_sockaddr( &sinaddr, buffer, sizeof(buffer) ) );
	}

	/* read the permissions information */

	if( Perm_filters_line_list.count ){
		Free_line_list(&Perm_line_list);
		Merge_line_list(&Perm_line_list,&RawPerm_line_list,0,0,0);
		Filterprintcap( &Perm_line_list, &Perm_filters_line_list, "");
	}
   
	Perm_check.service = 'X';

	permission = Perms_check( &Perm_line_list, &Perm_check, 0, 0 );
	if( permission == P_REJECT ){
		DEBUG1("Service_connection: talk socket '%d' no connect perms", talk );
		Write_fd_str( talk, _("\001no connect permissions\n") );
		cleanup(0);
	}
	Dispatch_input(&talk,input);
	cleanup(0);
}
