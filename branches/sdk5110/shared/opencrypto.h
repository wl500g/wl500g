/*
 * Broadcom Home Gateway Reference Design
 *
 * Copyright (C) 2009, Broadcom Corporation
 * All Rights Reserved.
 * 
 * THIS SOFTWARE IS OFFERED "AS IS", AND BROADCOM GRANTS NO WARRANTIES OF ANY
 * KIND, EXPRESS OR IMPLIED, BY STATUTE, COMMUNICATION OR OTHERWISE. BROADCOM
 * SPECIFICALLY DISCLAIMS ANY IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS
 * FOR A SPECIFIC PURPOSE OR NONINFRINGEMENT CONCERNING THIS SOFTWARE.
 * $Id: opencrypto.h,v 1.2 2010/12/03 07:59:57 Exp $
 */

#ifndef _opencrypto_h
#define _opencrypto_h


#define AES_BLOCK_LEN   8	/* bytes for AES block */


/*  AES-based keywrap function defined in RFC3394 */
int aes_wrap(size_t kl, uint8 *key, size_t il, uint8 *input, uint8 *output);

/* AES-based key unwrap function defined in RFC3394 */
int aes_unwrap(size_t kl, uint8 *key, size_t il, uint8 *input, uint8 *output);

/* Pseudo random function */
int fPRF(unsigned char *key, int key_len, unsigned char *prefix,
        int prefix_len, unsigned char *data, int data_len,
        unsigned char *output, int len);

/* hmac-sha1 keyed secure hash algorithm */
void hmac_sha1(unsigned char *text, int text_len, unsigned char *key,
               int key_len, unsigned char *digest);



#endif /* _opencrypto_h */
