/***************************************************************************
 * LPRng - An Extended Print Spooler System
 *
 * Copyright 1988-2003, Patrick Powell, San Diego, CA
 *     papowell@lprng.com
 * See LICENSE for conditions of use.
 * $Id: lprm.h,v 1.57 2003/09/05 20:07:21 papowell Exp $
 ***************************************************************************/



#ifndef _LPRM_1_
#define _LPRM_1_


EXTERN int Auth;       /* use authentication */
EXTERN int All_printers;    /* show all printers */
EXTERN int LP_mode;    /* show all printers */

/* PROTOTYPES */
int main(int argc, char *argv[], char *envp[]);
void Do_removal(char **argv);
void Get_parms(int argc, char *argv[] );
void pr_msg( char **msg );
void usage(void);

#endif
