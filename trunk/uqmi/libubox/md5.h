/*
 * Copyright (C) 2013 Felix Fietkau <nbd@openwrt.org>
 * Copyright (C) 2013 John Crispin <blogic@openwrt.org>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License version 2.1
 * as published by the Free Software Foundation
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#ifndef __PROCD_MD5_H
#define __PROCD_MD5_H

#include <stdint.h>
#include <stddef.h>

typedef struct md5_ctx {
	uint32_t A;
	uint32_t B;
	uint32_t C;
	uint32_t D;
	uint64_t total;
	uint32_t buflen;
	char buffer[128];
} md5_ctx_t;

void md5_begin(md5_ctx_t *ctx);
void md5_hash(const void *data, size_t length, md5_ctx_t *ctx);
void md5_end(void *resbuf, md5_ctx_t *ctx);
int md5sum(char *file, uint32_t *md5);

#endif
