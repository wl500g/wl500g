/***************************************************************************
 * LPRng - An Extended Print Spooler System
 *
 * Copyright 1988-2003, Patrick Powell, San Diego, CA
 *     papowell@lprng.com
 * See LICENSE for conditions of use.
 * $Id: ssl_auth.h,v 1.26 2003/09/05 20:07:21 papowell Exp $
 ***************************************************************************/



#ifndef _SSL_AUTH_H_
#define _SSL_AUTH_H_ 1

#include <openssl/ssl.h>
#include <openssl/err.h>


/* PROTOTYPES */
char *Set_ERR_str( char *header, char *errmsg, int errlen );
int SSL_Initialize_ctx(
	SSL_CTX **ctx_ret,
	char *errmsg, int errlen );
void Destroy_ctx(SSL_CTX *ctx);
int Open_SSL_connection( int sock, SSL_CTX *ctx, SSL **ssl_ret,
	struct line_list *info, char *errmsg, int errlen );
int Accept_SSL_connection( int sock, int timeout, SSL_CTX *ctx, SSL **ssl_ret,
	struct line_list *info, char *errmsg, int errlen );
int Write_SSL_connection( int timeout, SSL *ssl, char *buffer, int len,
	char *errmsg, int errlen );
int Gets_SSL_connection( int timeout, SSL *ssl, char *inbuffer, int len,
	char *errmsg, int errlen );
int Read_SSL_connection( int timeout, SSL *ssl, char *inbuffer, int *len,
	char *errmsg, int errlen );
int Close_SSL_connection( int sock, SSL *ssl );
const char * Error_SSL_name( int i );
int Ssl_send( int *sock,
	int transfer_timeout,
	char *tempfile,
	char *errmsg, int errlen,
	struct security *security, struct line_list *info );
int Ssl_receive( int *sock,
	char *user, char *jobsize, int from_server, char *authtype,
	struct line_list *info,
	char *errmsg, int errlen,
	struct line_list *header_info,
	struct security *security, char *tempfile );

#endif
