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

#ifndef __user
#define __user	
#endif
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
	char buf[PATH_MAX];
	char part[256];
	int i;

	/* Check for mtd partition */
	if ((fp = fopen("/proc/mtd", "r"))) {
		while (fgets(buf, sizeof(buf), fp)) {
			/* Match partition name */
			if (sscanf(buf, "mtd%d: %*x %*x \"%255[^\"]\"", &i, part) < 2)
				continue;
			if (!strcmp(part, mtd)) {
				snprintf(buf, sizeof(buf), MTD_DEV(%d), i);
				fclose(fp);
				return open(buf, flags);
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
	long len;
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
	for (erase_info.start = 0; erase_info.start < stats.st_size; erase_info.start += len) {
		len = MIN(erase_info.length, stats.st_size - erase_info.start);

		if (safe_fread(buf, 1, len, fp) != len) {
			fprintf(stderr, "%s: Truncated file\n", path);
			goto fail;
		}

		/* Do it */
		(void) ioctl(mtd_fd, MEMUNLOCK, &erase_info);

		if (write(1, "-", 1) != 1 ||
		    ioctl(mtd_fd, MEMERASE, &erase_info) != 0 ||
		    write(1, "\b+", 2) != 2 ||
		    write(mtd_fd, buf, len) != len) {
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
 * @param	path	file to write
 * @param	mtd	path to or partition name of MTD device 
 * @return	0 on success and errno on failure
 */
int mtd_write(const char *path, const char *mtd)
{
	int mtd_fd = -1;
	mtd_info_t mtd_info;
	erase_info_t erase_info;

	struct trx_header trx;
	unsigned long crc;

	FILE *fp;
	struct stat stats;
	char *buf = NULL;
	long len, pos;
	int ret = -1;

	if (stat(path, &stats) || !(fp = fopen(path, "r"))) 
	{
		perror(path);
		goto fail;
	}

	/* Examine TRX header */
	len = safe_fread(&trx, 1, sizeof(struct trx_header), fp);
	if (len < sizeof(struct trx_header)) {
		fprintf(stderr, "%s: File is too small (%ld bytes)\n", path, len);
		goto fail;
	}

	/* Open MTD device and get sector size */
	if ((mtd_fd = mtd_open(mtd, O_RDWR)) < 0 ||
	    ioctl(mtd_fd, MEMGETINFO, &mtd_info) != 0 ||
	    mtd_info.erasesize < sizeof(struct trx_header)) {
		perror(mtd);
		goto fail;
	}

	if (stats.st_size > mtd_info.size) {
		fprintf(stderr, "%s: File is too big (%ld, max %d)\n", path,
				stats.st_size, mtd_info.size);
		goto fail;
	}

	if (trx.magic != TRX_MAGIC ||
	    trx.len != stats.st_size ||
	    trx.len < sizeof(struct trx_header)) {
		fprintf(stderr, "%s: Bad trx header\n", path);
		goto fail;
	}

	/* Allocate temporary buffer */
	erase_info.length = mtd_info.erasesize;

	if (!(buf = malloc(erase_info.length))) {
		perror("malloc");
		goto fail;
	}

	/* Calculate CRC over header */
	crc = crc32((uint8 *)&trx.flag_version,
		    sizeof(struct trx_header) - OFFSETOF(struct trx_header, flag_version),
		    CRC32_INIT_VALUE);

	/* Calculate CRC before writing */
	for (pos = sizeof(struct trx_header); pos < trx.len; pos += len) {
		len = MIN(erase_info.length, trx.len - pos);
		if (safe_fread(buf, 1, len, fp) != len)
			goto fail;
		/* Update CRC */
		crc = crc32((uint8 *)buf, len, crc);
	}

	/* Check CRC before writing */
	if (crc != trx.crc32) {
		fprintf(stderr, "%s: Bad CRC\n", path);
		goto fail;
	}
	
	printf("%s: CRC OK\n", path);
	
	if (trx.flag_version & TRX_NO_HEADER) {
		(void) fseek(fp, sizeof(struct trx_header), SEEK_SET);
		trx.len -= sizeof(struct trx_header);
	}
	else
		rewind(fp);

	/* Write file to MTD device */
	for (erase_info.start = 0; erase_info.start < trx.len; erase_info.start += len) {
		len = MIN(erase_info.length, trx.len - erase_info.start);

		if (safe_fread(buf, 1, len, fp) != len) {
			fprintf(stderr, "%s: Truncated file\n", path);
			goto fail;
		}

		/* Do it */
		(void) ioctl(mtd_fd, MEMUNLOCK, &erase_info);

		if (ioctl(mtd_fd, MEMERASE, &erase_info) != 0 ||
		    write(mtd_fd, buf, len) != len) {
			perror(mtd);
			goto fail;
		}
	}

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
