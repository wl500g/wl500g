/***************************************************************************
 * LPRng - An Extended Print Spooler System
 *
 * Copyright 1988-2003, Patrick Powell, San Diego, CA
 *     papowell@lprng.com
 * See LICENSE for conditions of use.
 * $Id: lpd_rcvjob.h,v 1.57 2003/09/05 20:07:21 papowell Exp $
 ***************************************************************************/



#ifndef _LPD_RCVJOB_H_
#define _LPD_RCVJOB_H_ 1

/* PROTOTYPES */
int Receive_job( int *sock, char *input );
int Receive_block_job( int *sock, char *input );
int Scan_block_file( int fd, char *error, int errlen, struct line_list *header_info );
int Read_one_line( int fd, char *buffer, int maxlen );
int Check_space( double jobsize, int min_space, char *pathname );
int Do_perm_check( struct job *job, char *error, int errlen );
int Check_for_missing_files( struct job *job, struct line_list *files,
	char *error, int errlen, struct line_list *header_info, int *holdfile_fd );
int Set_up_temporary_hold_file( struct job *job,
	char *error, int errlen  );
int Find_non_colliding_job_number( struct job *job );
int Get_route( struct job *job, char *error, int errlen );

#endif
