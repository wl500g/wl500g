/*
 * LZMA compressed kernel decompressor for bcm947xx boards
 *
 * Copyright (C) 2005 by Oleg I. Vdovikin <oleg@cs.msu.su>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
 *
 *
 * Please note, this was code based on the bunzip2 decompressor code
 * by Manuel Novoa III  (mjn3@codepoet.org), although the only thing left
 * is an idea and part of original vendor code
 *
 *
 * 12-Mar-2005  Mineharu Takahara <mtakahar@yahoo.com>
 *   pass actual output size to decoder (stream mode
 *   compressed input is not a requirement anymore)
 *
 * 24-Apr-2005 Oleg I. Vdovikin
 *   reordered functions using lds script, removed forward decl
 *
 * 06-Oct-2009 Leonid Lisovskiy
 *   LZMA SDK 4.57
 *
 * 05-Mar-2012 Vladiaslav Grishenko
 *   LZMA SDK 4.65/9.xx
 */

#include "Types.h"
#include "LzmaDec.h"
#include "trxhdr.h"
#include "string.h"

#define BCM4710_FLASH		0x1fc00000	/* Flash */

#define KSEG0			0x80000000
#define KSEG1			0xa0000000

#define KSEG1ADDR(a)		((((unsigned)(a)) & 0x1fffffffU) | KSEG1)

#define Index_Invalidate_I	0x00
#define Index_Writeback_Inv_D   0x01

#define cache_unroll(base,op)	\
	__asm__ __volatile__(		\
		".set noreorder;\n"		\
		".set mips3;\n"			\
		"cache %1, (%0);\n"		\
		".set mips0;\n"			\
		".set reorder\n"		\
		:						\
		: "r" (base),			\
		  "i" (op));

static __inline__ void blast_icache(unsigned long size, unsigned long lsize)
{
	unsigned long start = KSEG0;
	unsigned long end = (start + size);

	while (start < end) {
		cache_unroll(start,Index_Invalidate_I);
		start += lsize;
	}
}

static __inline__ void blast_dcache(unsigned long size, unsigned long lsize)
{
	unsigned long start = KSEG0;
	unsigned long end = (start + size);

	while (start < end) {
		cache_unroll(start,Index_Writeback_Inv_D);
		start += lsize;
	}
}

/* beyound the image end, size not known in advance */
extern unsigned char workspace[];

static unsigned int offset;
static unsigned char *data;

/* flash access should be aligned, so wrapper is used */
/* read bytes from the flash, all accesses are 32-bit aligned */
static int read_bytes(void *object, void *buffer, size_t *size)
{
	static unsigned int val;
	unsigned int byteoffset = offset & 3;
	unsigned int bytehave = 4 - byteoffset;

	if (byteoffset == 0) {
		val = *(unsigned int *)data;
		data += 4;
		offset = 0;
	}
	/* Up to four bytes */
	if (bytehave < *size)
		*size = bytehave;
	memcpy(buffer, (unsigned char *)&val + byteoffset, *size);
	offset += *size;

	return SZ_OK;
}

void *dummy_alloc(void *p, size_t size)
{
	/* leave space for stream buffer */
	return &workspace[LZMA_REQUIRED_INPUT_MAX];
}

void dummy_free(void *p, void *address)
{
	/* really dummy */
}

/* should be the first function */
void entry(unsigned long icache_size, unsigned long icache_lsize, 
	unsigned long dcache_size, unsigned long dcache_lsize,
	unsigned long fw_arg0, unsigned long fw_arg1,
	unsigned long fw_arg2, unsigned long fw_arg3)
{
	CLzmaDec state;
	ISzAlloc dummy;
	ISeqInStream stream;
	ELzmaStatus status;
	unsigned int i;  /* temp value */
	unsigned int osize; /* uncompressed size */

	dummy.Alloc = dummy_alloc;
	dummy.Free = dummy_free;
	stream.Read = read_bytes;

	/* look for trx header, 32-bit data access */
	for (data = ((unsigned char *)KSEG1ADDR(BCM4710_FLASH));
		((struct trx_header *)data)->magic != TRX_MAGIC; data += 65536);

	/* compressed kernel is in the partition 1 */
	data += ((struct trx_header *)data)->offsets[1];
	offset = 0;

	/* read lzma stream header */
	SeqInStream_Read(&stream, workspace, LZMA_PROPS_SIZE + 8);

	/* read the lower half of uncompressed size in the header */
	osize = (workspace[LZMA_PROPS_SIZE + 0]) +
		(workspace[LZMA_PROPS_SIZE + 1] << 8) +
		(workspace[LZMA_PROPS_SIZE + 2] << 16) +
		(workspace[LZMA_PROPS_SIZE + 3] << 24);

	LzmaDec_Construct(&state);
	LzmaDec_AllocateProbs(&state, workspace, LZMA_PROPS_SIZE, &dummy);

	state.dic = (unsigned char *)LOADADDR;
	state.dicBufSize = osize;

	/* decompress kernel */
	LzmaDec_Init(&state);
	do {
		i = LZMA_REQUIRED_INPUT_MAX;
		SeqInStream_Read(&stream, workspace, i);
		if (LzmaDec_DecodeToDic(&state, osize, workspace, &i,
		    LZMA_FINISH_ANY, &status) != SZ_OK) {
			/* something went wrong */
			return;
		}
	} while (status == LZMA_STATUS_NEEDS_MORE_INPUT);

	blast_dcache(dcache_size, dcache_lsize);
	blast_icache(icache_size, icache_lsize);

	/* jump to load address */
	((void (*)(unsigned long, unsigned long, unsigned long,
		   unsigned long))LOADADDR)(fw_arg0, fw_arg1, fw_arg2, fw_arg3);
}
