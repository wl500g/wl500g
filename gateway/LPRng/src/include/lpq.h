/***************************************************************************
 * LPRng - An Extended Print Spooler System
 *
 * Copyright 1988-2003, Patrick Powell, San Diego, CA
 *     papowell@lprng.com
 * See LICENSE for conditions of use.
 * $Id: lpq.h,v 1.57 2003/09/05 20:07:21 papowell Exp $
 ***************************************************************************/



#ifndef _LPQ_H_
#define _LPQ_H_ 1
EXTERN int Auth;			/* Authentication */
EXTERN int LP_mode;			/* LP mode */
EXTERN int Longformat;      /* Long format */
EXTERN int Rawformat;       /* Long format */
EXTERN int Displayformat;   /* Display format */
EXTERN int All_printers;    /* show all printers */
EXTERN int Status_line_count; /* number of status lines */
EXTERN int Clear_scr;       /* clear screen */
EXTERN int Interval;        /* display interval */
EXTERN int Show_all;        /* show all status */

/* PROTOTYPES */
int main(int argc, char *argv[], char *envp[]);
void Show_status(char **argv);
int Read_status_info( char *host, int sock,
	int output, int timeout, int displayformat,
	int status_line_count );
void Term_clear();
void Get_parms(int argc, char *argv[] );
void usage(void);

#endif
