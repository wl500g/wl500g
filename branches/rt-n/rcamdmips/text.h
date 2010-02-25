#ifndef _TEXT_H_
#define _TEXT_H_

/* $Id: text.h,v 1.1.1.1 2003/11/21 13:43:58 Cheni_Shen Exp $ */

struct text_ctx
{
	unsigned char *text;
	unsigned char color[3];
	unsigned char bgcolor[3];
	int solid:1,
	    top:1,
	    right:1,
	    type:2,	/* 0 = text, 1 = command, 2 = file */
	    nosubst:1;
};

#endif

