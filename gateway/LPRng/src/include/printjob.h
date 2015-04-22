/***************************************************************************
 * LPRng - An Extended Print Spooler System
 *
 * Copyright 1988-2003, Patrick Powell, San Diego, CA
 *     papowell@lprng.com
 * See LICENSE for conditions of use.
 * $Id: printjob.h,v 1.57 2003/09/05 20:07:21 papowell Exp $
 ***************************************************************************/



#ifndef _PRINTJOB_H_
#define _PRINTJOB_H_ 1

/* PROTOTYPES */
int Print_job( int output, int status_device, struct job *job,
	int send_job_rw_timeout, int poll_for_status, char *user_filter );
int Run_OF_filter( int send_job_rw_timeout, int *of_pid, int *of_stdin, int *of_stderr,
	int output, char **outbuf, int *outmax, int *outlen,
	struct job *job, char *id, int terminate_of,
	char *msgbuffer, int msglen );
void Print_banner( char *name, char *pgm, struct job *job );
int Write_outbuf_to_OF( struct job *job, char *title,
	int of_fd, char *buffer, int outlen,
	int of_error, char *msg, int msgmax,
	int timeout, int poll_for_status, char *status_file );
int Get_status_from_OF( struct job *job, char *title, int of_pid,
	int of_error, char *msg, int msgmax,
	int timeout, int suspend, int max_wait, char *status_file );
int Wait_for_pid( int of_pid, char *name, int suspend, int timeout );

#endif
