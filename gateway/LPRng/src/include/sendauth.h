/***************************************************************************
 * LPRng - An Extended Print Spooler System
 *
 * Copyright 1988-2003, Patrick Powell, San Diego, CA
 *     papowell@lprng.com
 * See LICENSE for conditions of use.
 * $Id: sendauth.h,v 1.57 2003/09/05 20:07:21 papowell Exp $
 ***************************************************************************/



#ifndef _SENDAUTH_H_
#define _SENDAUTH_H_ 1

/* PROTOTYPES */
int Send_auth_transfer( int *sock, int transfer_timeout,
	struct job *job, struct job *logjob, char *error, int errlen, char *cmd,
	struct security *security, struct line_list *info );
struct security *Fix_send_auth( char *name, struct line_list *info,
	struct job *job, char *error, int errlen );
void Put_in_auth( int tempfd, const char *key, char *value );

#endif
