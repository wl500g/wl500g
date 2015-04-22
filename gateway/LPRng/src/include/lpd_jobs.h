/***************************************************************************
 * LPRng - An Extended Print Spooler System
 *
 * Copyright 1988-2003, Patrick Powell, San Diego, CA
 *     papowell@lprng.com
 * See LICENSE for conditions of use.
 * $Id: lpd_jobs.h,v 1.57 2003/09/05 20:07:21 papowell Exp $
 ***************************************************************************/



#ifndef _LPD_JOBS_H_
#define _LPD_JOBS_H_ 1

/* PROTOTYPES */
void Update_spool_info( struct line_list *sp );
int cmp_server( const void *left, const void *right, const void *p );
void Get_subserver_pc( char *printer, struct line_list *subserver_info, int done_time );
void Dump_subserver_info( char *title, struct line_list *l);
void Get_subserver_info( struct line_list *order,
	char *list, char *old_order);
char *Make_temp_copy( char *srcfile, char *destdir );
int Do_queue_jobs( char *name, int subserver );
int Remote_job( struct job *job, int lpd_bounce, char *move_dest, char *id );
int Local_job( struct job *job, char *id );
int Fork_subserver( struct line_list *server_info, int use_subserver,
	struct line_list *parms );
void Wait_for_subserver( int timeout, struct line_list *servers
	/*, struct line_list *order */ );
int Decode_transfer_failure( int attempt, struct job *job );
void Update_status( struct job *job, int status );
int Check_print_perms( struct job *job );
void Setup_user_reporting( struct job *job );
void Service_worker( struct line_list *args );
int Printer_open( char *lp_device, int *status_fd, struct job *job,
	int max_attempts, int interval, int max_interval, int grace,
	int connect_tmout, int *filterpid, int *poll_for_status );
void Add_banner_to_job( struct job *job );
void Fix_bq_format( int format, struct line_list *datafile );
void Filter_files_in_job( struct job *job, int outfd, char *user_filter );
void Service_queue( struct line_list *args );
int Remove_done_jobs( void );

#endif
