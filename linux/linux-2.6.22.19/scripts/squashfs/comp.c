/*
 * Copyright (C) 2006-2007 Junjiro Okajima
 * Copyright (C) 2006-2007 Tomas Matejicek, slax.org
 *
 * LICENSE follows the described one in lzma.txt.
 */

/* $Id: comp.cc,v 1.3 2007-11-13 13:27:23 jro Exp $ */

//////////////////////////////////////////////////////////////////////
#include <zlib.h>
#include "sqlzma.h"

int sqlzma_cm(struct sqlzma_opts *opts, z_stream *stream)
{
	int err;

	if (opts->try_lzma) {
		uLong processed = stream->avail_out;

		err = compress_lzma(stream->next_out, &processed,
				stream->next_in, stream->avail_in, opts->dicsize);
		if (!err) {
			stream->total_out = processed;
			err = Z_STREAM_END;
			return err;
		}
	}

	err = deflate(stream, Z_FINISH);
	return err;
}
