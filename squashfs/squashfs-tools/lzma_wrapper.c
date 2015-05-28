/*
 * Copyright (c) 2009, 2010
 * Phillip Lougher <phillip@lougher.demon.co.uk>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2,
 * or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 *
 * lzma_wrapper.c
 *
 * Support for LZMA1 compression using LZMA SDK (4.65 used in
 * development, other versions may work) http://www.7-zip.org/sdk.html
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include <LzmaEnc.h>
#include <LzmaLib.h>

#include "squashfs_fs.h"
#include "compressor.h"

#define LZMA_HEADER_SIZE	(LZMA_PROPS_SIZE + 8)

static int fb = 32;
static int lc = 3;
static int lp = 0;
static int pb = 2;
static int mf = 4;


static void *lzma_alloc(void *p, size_t size)
{
	return size ? malloc(size) : NULL;
}

static void lzma_free(void *p, void *address)
{
	free(address);
}

static ISzAlloc alloc_ops = {
	lzma_alloc,
	lzma_free
};


static int lzma_compress(void *strm, void *dest, void *src, int size, int block_size,
		int *error)
{
	CLzmaEncProps props;
	unsigned char *d = dest;
	size_t props_size = LZMA_PROPS_SIZE,
		outlen = block_size - LZMA_HEADER_SIZE;
	int res;

	LzmaEncProps_Init(&props);
	props.level = 5;
	props.dictSize = block_size;
	props.lc = lc;
	props.lp = lp;
	props.pb = pb;
	props.fb = fb;
	props.algo = 1;
	props.btMode = (mf > 0) ? 1 : 0;
	props.numHashBytes = mf;
	props.numThreads = 1;

	res = LzmaEncode(dest + LZMA_HEADER_SIZE, &outlen, src, size, &props, dest,
		&props_size, 0, NULL, &alloc_ops, &alloc_ops);
	
	if(res == SZ_ERROR_OUTPUT_EOF) {
		/*
		 * Output buffer overflow.  Return out of buffer space error
		 */
		return 0;
	}

	if(res != SZ_OK) {
		/*
		 * All other errors return failure, with the compressor
		 * specific error code in *error
		 */
		*error = res;
		return -1;
	}

	/*
	 * Fill in the 8 byte little endian uncompressed size field in the
	 * LZMA header.  8 bytes is excessively large for squashfs but
	 * this is the standard LZMA header and which is expected by the kernel
	 * code
	 */
	d[LZMA_PROPS_SIZE] = size & 255;
	d[LZMA_PROPS_SIZE + 1] = (size >> 8) & 255;
	d[LZMA_PROPS_SIZE + 2] = (size >> 16) & 255;
	d[LZMA_PROPS_SIZE + 3] = (size >> 24) & 255;
	d[LZMA_PROPS_SIZE + 4] = 0;
	d[LZMA_PROPS_SIZE + 5] = 0;
	d[LZMA_PROPS_SIZE + 6] = 0;
	d[LZMA_PROPS_SIZE + 7] = 0;

	/*
	 * Success, return the compressed size.  Outlen returned by the LZMA
	 * compressor does not include the LZMA header space
	 */
	return outlen + LZMA_HEADER_SIZE;
}


static int lzma_uncompress(void *dest, void *src, int size, int block_size,
	int *error)
{
	unsigned char *s = src;
	size_t outlen, inlen = size - LZMA_HEADER_SIZE;
	int res;

	outlen = s[LZMA_PROPS_SIZE] |
		(s[LZMA_PROPS_SIZE + 1] << 8) |
		(s[LZMA_PROPS_SIZE + 2] << 16) |
		(s[LZMA_PROPS_SIZE + 3] << 24);

	res = LzmaUncompress(dest, &outlen, src + LZMA_HEADER_SIZE, &inlen, src,
		LZMA_PROPS_SIZE);
	
	*error = res;
	return res == SZ_OK ? outlen : -1;
}


static int lzma_options(char *argv[], int argc)
{
	int arg;
	char *end = NULL;

	if (argc < 2) {
		fprintf(stderr, "lzma: %s missing value\n", argv[0]);
		return -2;
	}

	arg = strtoul(argv[1], &end, 10);

	if (strcmp(argv[0], "-Xfb") == 0) {
		if (!end || *end || arg < 5 || arg > 273)
			goto error;
		fb = arg;
	} else if (strcmp(argv[0], "-Xlc") == 0) {
		if (!end || *end || arg < 0 || arg > 8)
			goto error;
		lc = arg;
	} else if (strcmp(argv[0], "-Xlp") == 0) {
		if (!end || *end || arg < 0 || arg > 4)
			goto error;
		lp = arg;
	} else if (strcmp(argv[0], "-Xpb") == 0) {
		if (!end || *end || arg < 0 || arg > 4)
			goto error;
		pb = arg;
	} else if (strcmp(argv[0], "-Xmf") == 0) {
		if (strcmp(argv[1], "hc4") == 0)
			mf = 0;
		else if (strcmp(argv[1], "bt1") == 0)
			mf = 1;
		else if (strcmp(argv[1], "bt2") == 0)
			mf = 2;
		else if (strcmp(argv[1], "bt4") == 0)
			mf = 4;
		else
			goto error;
	} else
		return -1;

	return 1;

error:
	fprintf(stderr, "lzma: %s invalid value\n", argv[0]);
	return -2;
}


void lzma_usage()
{
	fprintf(stderr,
		"\t  -Xfb <N>:  set number of fast bytes - [5, 273], default: %d\n"
		"\t  -Xlc <N>:  set number of literal context bits - [0, 8], default: %d\n"
		"\t  -Xlp <N>:  set number of literal pos bits - [0, 4], default: %d\n"
		"\t  -Xpb <N>:  set number of pos bits - [0, 4], default: %d\n"
		"\t  -Xmf <id>: set match finder - [bt1, bt2, bt4, hc4], default: %s\n",
		fb, lc, lp, pb,
		(mf == 1) ? "bt1" : (mf == 2) ? "bt2" : (mf == 4) ? "bt4" :
		(mf == 0) ? "hc4" : "");
}


struct compressor lzma_comp_ops = {
	.init = NULL,
	.compress = lzma_compress,
	.uncompress = lzma_uncompress,
	.options = lzma_options,
	.usage = lzma_usage,
	.id = LZMA_COMPRESSION,
	.name = "lzma",
	.supported = 1
};

