/***************************************************************************
 * LPRng - An Extended Print Spooler System
 *
 * Copyright 1988-2003, Patrick Powell, San Diego, CA
 *     papowell@lprng.com
 * See LICENSE for conditions of use.
 * $Id: lpd_dispatch.h,v 1.57 2003/09/05 20:07:21 papowell Exp $
 ***************************************************************************/



#ifndef _LPD_DISPATCH_H_
#define _LPD_DISPATCH_H_ 1

/* PROTOTYPES */
void Dispatch_input(int *talk, char *input );
void Service_all( struct line_list *args );
void Service_connection( struct line_list *args );

#endif
