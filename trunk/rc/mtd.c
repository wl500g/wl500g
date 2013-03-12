/*
 * MTD utility functions
 *
 * Copyright 2004, Broadcom Corporation
 * All Rights Reserved.
 * 
 * THIS SOFTWARE IS OFFERED "AS IS", AND BROADCOM GRANTS NO WARRANTIES OF ANY
 * KIND, EXPRESS OR IMPLIED, BY STATUTE, COMMUNICATION OR OTHERWISE. BROADCOM
 * SPECIFICALLY DISCLAIMS ANY IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS
 * FOR A SPECIFIC PURPOSE OR NONINFRINGEMENT CONCERNING THIS SOFTWARE.
 *
 * $Id$
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <limits.h>
#include <sys/sysmacros.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <error.h>
#include <sys/ioctl.h>
#include <sys/sysinfo.h>

#include <linux/compiler.h>
#include <mtd/mtd-user.h>

#include <trxhdr.h>
#include <rts/crc.h>
#include <bcmutils.h>
#include "rc.h"
#include "mtd.h"

/*
 * Open an MTD device
 * @param	mtd	path to or partition name of MTD device
 * @param	flags	open() flags
 * @return	return value of open()
 */
static int mtd_open(const char *mtd, int flags)
{
	FILE *fp;
	char dev[PATH_MAX];
	char *tok, *context;
	int i, m;

	/* Check for mtd partition */
	if ((fp = fopen("/proc/mtd", "r"))) {
		while (fgets(dev, sizeof(dev), fp)) {
			/* Match by device name & alias only (fields 1, 4) */
			tok = strtok_r(dev, " \t", &context);
			if (sscanf(tok, "mtd%d:", &m) < 1)
				continue;
			for (i=1; tok && i<=4; i++, tok = strtok_r(NULL, " \t", &context)) {
				if (i >1 && i < 4)
					continue;
				if (strstr(tok, mtd))
				{
					snprintf(dev, sizeof(dev), MTD_DEV(%d), m);
					fclose(fp);
					return open(dev, flags);
				}
			}
		}
		fclose(fp);
	}

	return open(mtd, flags);
}

/*
 * Erase an MTD device
 * @param	mtd	path to or partition name of MTD device
 * @return	0 on success and errno on failure
 */
int mtd_erase(const char *mtd)
{
	int mtd_fd;
	mtd_info_t mtd_info;
	erase_info_t erase_info;

	/* Open MTD device */
	if ((mtd_fd = mtd_open(mtd, O_RDWR)) < 0) {
		perror(mtd);
		return errno;
	}

	/* Get sector size */
	if (ioctl(mtd_fd, MEMGETINFO, &mtd_info) != 0) {
		perror(mtd);
		close(mtd_fd);
		return errno;
	}

	erase_info.length = mtd_info.erasesize;

	for (erase_info.start = 0;
	     erase_info.start < mtd_info.size;
	     erase_info.start += mtd_info.erasesize) {
		(void) ioctl(mtd_fd, MEMUNLOCK, &erase_info);
		if (ioctl(mtd_fd, MEMERASE, &erase_info) != 0) {
			perror(mtd);
			close(mtd_fd);
			return errno;
		}
	}

	close(mtd_fd);
	return 0;
}


/*
 * Flash a file to an MTD device
 * @param	path	file to write or a URL
 * @param	mtd	path to or partition name of MTD device 
 * @return	0 on success and errno on failure
 */
int mtd_flash(const char *path, const char *mtd)
{
	int mtd_fd = -1;
	mtd_info_t mtd_info;
	erase_info_t erase_info;

	FILE *fp = NULL;
	struct stat stats;
	char *buf = NULL;
	long count, len;
	int ret = -1;

	if (stat(path, &stats) || !(fp = fopen(path, "r"))) 
	{
		perror(path);
		goto fail;
	}

	/* Open MTD device and get sector size */
	if ((mtd_fd = mtd_open(mtd, O_RDWR)) < 0 ||
	    ioctl(mtd_fd, MEMGETINFO, &mtd_info) != 0) 
	{
		perror(mtd);
		goto fail;
	}
	
	if (stats.st_size > mtd_info.size) {
		fprintf(stderr, "%s: File is too big (%ld, max %d)\n", path,
				stats.st_size, mtd_info.size);
		goto fail;
	}

	/* Allocate temporary buffer */
	erase_info.length = mtd_info.erasesize;
	
	if (!(buf = malloc(erase_info.length))) {
		perror("malloc");
		goto fail;
	}

	/* Write file to MTD device */
	for (erase_info.start = 0; erase_info.start < stats.st_size; erase_info.start += count) {
		len = MIN(erase_info.length, stats.st_size - erase_info.start);
		count = safe_fread(buf, 1, len, fp);

		if (count < len) {
			fprintf(stderr, "%s: Truncated file (actual %ld expect %ld)\n", path,
				count, len);
			goto fail;
		}

		/* Do it */
		(void) ioctl(mtd_fd, MEMUNLOCK, &erase_info);
		if (write(1, "-", 1) != 1 ||
		    ioctl(mtd_fd, MEMERASE, &erase_info) != 0 ||
		    write(1, "\b+", 2) != 2 ||
		    write(mtd_fd, buf, count) != count) {
			perror(mtd);
			goto fail;
		}
		
		write(1, "\b.", 2);
	}

	ret = 0;

 fail:
	printf("\n");

	if (buf) {
		/* Dummy read to ensure chip(s) are out of lock/suspend state */
		(void) read(mtd_fd, buf, 2);
		free(buf);
	}

	if (mtd_fd >= 0)
		close(mtd_fd);
	if (fp)
		fclose(fp);
	return ret;
}

/*
 * Write a file to an MTD device
 * @param	path	file to write or a URL
 * @param	mtd	path to or partition name of MTD device 
 * @return	0 on success and errno on failure
 */
int mtd_write(const char *path, const char *mtd)
{
	int mtd_fd = -1;
	mtd_info_t mtd_info;
	erase_info_t erase_info;

	struct sysinfo info;
	struct trx_header trx;
	unsigned long crc;

	FILE *fp;
	char *buf = NULL;
	long count, len, off;
	int ret = -1;

	/* Examine TRX header */
	if ((fp = fopen(path, "r")))
		count = safe_fread(&trx, 1, sizeof(struct trx_header), fp);
	else
		count = http_get(path, (char *) &trx, sizeof(struct trx_header), 0);
	if (count < sizeof(struct trx_header)) {
		fprintf(stderr, "%s: File is too small (%ld bytes)\n", path, count);
		goto fail;
	}

	/* Open MTD device and get sector size */
	if ((mtd_fd = mtd_open(mtd, O_RDWR)) < 0 ||
	    ioctl(mtd_fd, MEMGETINFO, &mtd_info) != 0 ||
	    mtd_info.erasesize < sizeof(struct trx_header)) {
		perror(mtd);
		goto fail;
	}

	if (trx.magic != TRX_MAGIC ||
	    trx.len > mtd_info.size ||
	    trx.len < sizeof(struct trx_header)) {
		fprintf(stderr, "%s: Bad trx header\n", path);
		goto fail;
	}

	/* See if we have enough memory to store the whole file */
	sysinfo(&info);
	if ((info.freeram + info.bufferram) >= trx.len)
		erase_info.length = ROUNDUP(trx.len, mtd_info.erasesize);
	else
		erase_info.length = mtd_info.erasesize;

	/* Allocate temporary buffer */
	if (!(buf = malloc(erase_info.length))) {
		perror("malloc");
		goto fail;
	}

	/* Calculate CRC over header */
	crc = crc32((uint8 *) &trx.flag_version,
		    sizeof(struct trx_header) - OFFSETOF(struct trx_header, flag_version),
		    CRC32_INIT_VALUE);

	if (trx.flag_version & TRX_NO_HEADER)
		trx.len -= sizeof(struct trx_header);

	/* Write file or URL to MTD device */
	for (erase_info.start = 0; erase_info.start < trx.len; erase_info.start += count) {
		len = MIN(erase_info.length, trx.len - erase_info.start);
		if ((trx.flag_version & TRX_NO_HEADER) || erase_info.start)
			count = off = 0;
		else {
			count = off = sizeof(struct trx_header);
			memcpy(buf, &trx, sizeof(struct trx_header));
		}
		if (fp)
			count += safe_fread(&buf[off], 1, len - off, fp);
		else
			count += http_get(path, &buf[off], len - off, erase_info.start + off);
		if (count < len) {
			fprintf(stderr, "%s: Truncated file (actual %ld expect %ld)\n", path,
				count - off, len - off);
			goto fail;
		}
		/* Update CRC */
		crc = crc32((uint8 *)&buf[off], count - off, crc);
		/* Check CRC before writing if possible */
		if (count == trx.len) {
			if (crc != trx.crc32) {
				fprintf(stderr, "%s: Bad CRC\n", path);
				goto fail;
			}
		}
		/* Do it */
		(void) ioctl(mtd_fd, MEMUNLOCK, &erase_info);

		if (
		    ioctl(mtd_fd, MEMERASE, &erase_info) != 0 ||
		    write(mtd_fd, buf, count) != count) {
			perror(mtd);
			goto fail;
		}
	}

	printf("%s: CRC OK\n", mtd);
	ret = 0;

 fail:
	if (buf) {
		/* Dummy read to ensure chip(s) are out of lock/suspend state */
		(void) read(mtd_fd, buf, 2);
		free(buf);
	}

	if (mtd_fd >= 0)
		close(mtd_fd);
	if (fp)
		fclose(fp);
	return ret;
}
