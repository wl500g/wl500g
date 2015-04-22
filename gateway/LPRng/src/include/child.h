/***************************************************************************
 * LPRng - An Extended Print Spooler System
 *
 * Copyright 1988-2003, Patrick Powell, San Diego, CA
 *     papowell@lprng.com
 * See LICENSE for conditions of use.
 * $Id: child.h,v 1.57 2003/09/05 20:07:20 papowell Exp $
 ***************************************************************************/



#ifndef _CHILD_H_
#define _CHILD_H_ 1

/* PROTOTYPES */
pid_t plp_waitpid (pid_t pid, plp_status_t *statusPtr, int options);
void Dump_pinfo( char *title, struct line_list *p ) ;
int Countpid(void);
void Killchildren( int sig );
pid_t dofork( int new_process_group );
plp_signal_t cleanup_USR1 (int passed_signal);
plp_signal_t cleanup_HUP (int passed_signal);
plp_signal_t cleanup_INT (int passed_signal);
plp_signal_t cleanup_QUIT (int passed_signal);
plp_signal_t cleanup_TERM (int passed_signal);
void Max_open( int fd );
plp_signal_t cleanup (int passed_signal);
void Dump_unfreed_mem(char *title);

#endif
