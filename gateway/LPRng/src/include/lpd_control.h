/***************************************************************************
 * LPRng - An Extended Print Spooler System
 *
 * Copyright 1988-2003, Patrick Powell, San Diego, CA
 *     papowell@lprng.com
 * See LICENSE for conditions of use.
 * $Id: lpd_control.h,v 1.57 2003/09/05 20:07:21 papowell Exp $
 ***************************************************************************/



#ifndef _LPD_CONTROL_H_
#define _LPD_CONTROL_H_ 1

/* PROTOTYPES */
int Job_control( int *sock, char *input );
void Do_printer_work( char *user, int action, int *sock,
	struct line_list *tokens, char *error, int errorlen );
void Do_queue_control( char *user, int action, int *sock,
	struct line_list *tokens, char *error, int errorlen );
int Do_control_file( int action, int *sock,
	struct line_list *tokens, char *error, int errorlen, char *option );
int Do_control_lpq( char *user, int action,
	struct line_list *tokens );
int Do_control_status( int *sock,
	char *error, int errorlen );
int Do_control_redirect( int *sock,
	struct line_list *tokens, char *error, int errorlen );
int Do_control_class( int *sock,
	struct line_list *tokens, char *error, int errorlen );
int Do_control_debug( int *sock,
	struct line_list *tokens, char *error, int errorlen );
int Do_control_printcap( int *sock );
int Do_control_defaultq( int *sock );

#endif
