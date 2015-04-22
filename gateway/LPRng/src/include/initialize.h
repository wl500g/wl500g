/***************************************************************************
 * LPRng - An Extended Print Spooler System
 *
 * Copyright 1988-2003, Patrick Powell, San Diego, CA
 *     papowell@lprng.com
 * See LICENSE for conditions of use.
 * $Id: initialize.h,v 1.57 2003/09/05 20:07:20 papowell Exp $
 ***************************************************************************/



#ifndef _INITIALIZE_H
#define _INITIALIZE_H

/* PROTOTYPES */
void Initialize(int argc,  char *argv[], char *envp[], int debugchar );
void Setup_configuration();
char *Get_user_information( void );

#endif
