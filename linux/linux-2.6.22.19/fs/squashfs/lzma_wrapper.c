/*
 * Squashfs - a compressed read only filesystem for Linux
 *
 * Copyright (c) 2012 Vladislav Grishenko <themiron@mail.ru>
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
 * Foundation, 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 *
 * lzma_wrapper.c
 */


#include <linux/mutex.h>
#include <linux/buffer_head.h>
#include <linux/slab.h>
#include <linux/vmalloc.h>

#include "squashfs_fs.h"
#include "squashfs_fs_sb.h"
#include "squashfs.h"
#include "decompressor.h"

#include "Types.h"
#include "LzmaDec.h"

#if 0
#define DEBUG(s, args...) pr_debug("SQUASHFS: "s, ## args)
#else
#define DEBUG(s, args...)
#endif

struct lzma_stream {
	CLzmaDec *state;
	uint8_t *dict;
	uint8_t *in, *out;
	int in_avail, out_avail;
	int header_size;
	uint8_t header[LZMA_PROPS_SIZE + 8];
};

static void *lzma_malloc(void *p, size_t size)
{
	void *address = kmalloc(size, GFP_ATOMIC);
	DEBUG("%s(%p, %d) = %p\n", __FUNCTION__, p, size, address);
	return address;
}

static void lzma_mfree(void *p, void *address)
{
	DEBUG("%s(%p, %p)\n", __FUNCTION__, p, address);
	return kfree(address);
}

static ISzAlloc lzma_alloc_ops = {
	.Alloc = lzma_malloc,
	.Free = lzma_mfree
};

static void lzma_free(void *strm);

static void *lzma_init(struct squashfs_sb_info *msblk, void *buff, int len)
{
	struct lzma_stream *stream = kmalloc(sizeof(*stream), GFP_KERNEL);
	int dict_size = max_t(int, msblk->block_size, SQUASHFS_METADATA_SIZE);

	if (stream == NULL)
		goto failed;

	stream->state = kmalloc(sizeof(*stream->state), GFP_KERNEL);
	if (stream->state == NULL)
		goto failed;

	stream->dict = vmalloc(dict_size);
	if (stream->dict == NULL)
		goto failed;

	LzmaDec_Construct(stream->state);
	stream->state->dic = stream->dict;
	stream->state->dicBufSize = dict_size;

	DEBUG("%s dictionary size %d\n", __FUNCTION__, dict_size);
	return stream;

failed:
	ERROR("Failed to initialise lzma decompressor\n");
	lzma_free(stream);
	return ERR_PTR(-ENOMEM);
}

static void lzma_free(void *strm)
{
	struct lzma_stream *stream = strm;

	if (stream) {
		if (stream->state)
			LzmaDec_FreeProbs(stream->state, &lzma_alloc_ops);
		if (stream->dict)
			vfree(stream->dict);
		kfree(stream->state);
	}
	kfree(stream);
}

static int lzma_uncompress(struct squashfs_sb_info *msblk, void **buffer,
	struct buffer_head **bh, int b, int offset, int length, int srclength,
	int pages)
{
	struct lzma_stream *stream = msblk->stream;
	CLzmaDec *state = stream->state;
	ELzmaStatus status;
	int in_avail, out_avail, total = 0;
	int k = 0, page = 0;
	int lzma_err = SZ_OK;

	mutex_lock(&msblk->read_data_mutex);

	stream->in = NULL;
	stream->in_avail = 0;
	stream->out = NULL;
	stream->out_avail = 0;
	stream->header_size = 0;

	do {
		if (stream->in_avail == 0 && k < b) {
			in_avail = min(length, msblk->devblksize - offset);
			length -= in_avail;

			wait_on_buffer(bh[k]);
			if (!buffer_uptodate(bh[k]))
				goto release_mutex;

			stream->in = bh[k]->b_data + offset;
			stream->in_avail = in_avail;
			offset = 0;
		}

                if (stream->out_avail == 0 && page < pages) {
			stream->out = buffer[page++];
			stream->out_avail = PAGE_CACHE_SIZE;
		}

		if (stream->header_size < sizeof(stream->header)) {
			in_avail = min_t(int, stream->in_avail, sizeof(stream->header) - stream->header_size);
			memcpy(&stream->header[stream->header_size], stream->in, in_avail);
			stream->header_size += in_avail;
			stream->in += in_avail;
			stream->in_avail -= in_avail;

			if (stream->header_size < sizeof(stream->header))
				goto next_block;

			lzma_err = LzmaDec_AllocateProbs(state,
				stream->header, LZMA_PROPS_SIZE, &lzma_alloc_ops);

			if (lzma_err != SZ_OK) {
				ERROR("lzma_uncompress error, allocation failure\n");
				goto release_mutex;
			}

			if (state->prop.dicSize > state->dicBufSize) {
				ERROR("lzma_uncompress error, unexpected dictionaty size %d\n", state->prop.dicSize);
				goto release_mutex;
			}

			LzmaDec_Init(state);
		}

		in_avail = stream->in_avail;
		out_avail = stream->out_avail;

		lzma_err = LzmaDec_DecodeToBuf(state,
			stream->out, &out_avail,
			stream->in, &in_avail, LZMA_FINISH_ANY, &status);

		stream->in += in_avail;
		stream->in_avail -= in_avail;
		stream->out += out_avail;
		stream->out_avail -= out_avail;
		total += out_avail;

	next_block:
		if (stream->in_avail == 0 && k < b)
			put_bh(bh[k++]);
	} while (lzma_err == SZ_OK && k < b);

	if (lzma_err != SZ_OK) {
		ERROR("lzma_uncompress error, data probably corrupt\n");
		goto release_mutex;
	}

	if (k < b) {
		ERROR("lzma_uncompress error, input remaining\n");
		goto release_mutex;
	}

	mutex_unlock(&msblk->read_data_mutex);
	return total;

release_mutex:
	mutex_unlock(&msblk->read_data_mutex);

	for (; k < b; k++)
		put_bh(bh[k]);

	return -EIO;
}

const struct squashfs_decompressor squashfs_lzma_comp_ops = {
	.init = lzma_init,
	.free = lzma_free,
	.decompress = lzma_uncompress,
	.id = LZMA_COMPRESSION,
	.name = "lzma",
	.supported = 1
};
