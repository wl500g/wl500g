/* scsi-start / scsi-stop
 * Copyright (C) 1999 Trent Piepho <xyzzy@speakeasy.org>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */

#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <sys/stat.h>
#include <linux/major.h>
#include <linux/kdev_t.h>
#ifdef HAVE_SG_IO
#include <scsi/scsi.h>
#include <scsi/sg.h>
#else
#include <scsi/scsi_ioctl.h>
#endif

#ifndef HZ // for START_STOP_TIMEOUT
#define HZ 1000
#endif

#ifdef SCSI_DISK0_MAJOR
#define IS_SCSI_DISK(rdev)	SCSI_DISK_MAJOR(MAJOR(rdev))
#else
#define IS_SCSI_DISK(rdev)	(MAJOR(rdev)==SCSI_DISK_MAJOR)
#endif

int main(int argc, char *argv[])
{
	int fd, mode, ret = 0;
	struct stat statbuf;
#ifdef HAVE_SG_IO
	sg_io_hdr_t sg_io_hdr;
#endif

	mode = argv[0][strlen(argv[0])-1];
	if(mode=='p' || mode=='P')  {
		mode = 0;	/* stoP */
	} else if(mode=='t' || mode=='T')  {
		mode = 1;	/* starT */
	} else {
		fprintf(stderr, "Try ending the executable name with 'stop' or 'start'\n");
		exit(1);
	}

	if (argc != 2) {
		fprintf(stderr, "Usage: %s device\n",argv[0]);
		fprintf(stderr, "%s the device's motor\n", mode?"Starts":"Stops");
		exit(1);
	}
	if ((fd = open(argv[1], O_RDWR)) < 0) {
		perror(argv[1]);
		exit(1);
	}
	if ((fstat(fd, &statbuf)) < 0) {
		perror(argv[1]);
		close(fd);
		exit(1);
	}
	if (!S_ISBLK(statbuf.st_mode)
		|| !IS_SCSI_DISK(statbuf.st_rdev) )  {
		fprintf(stderr, "%s is not a SCSI block device\n", argv[1]);
		close(fd);
		exit(1);
	}

#ifdef HAVE_SG_IO
	memset(&sg_io_hdr, 0, sizeof(sg_io_hdr));
	sg_io_hdr.interface_id = 'S';
	sg_io_hdr.dxfer_direction = SG_DXFER_NONE;
	sg_io_hdr.cmdp = (unsigned char []){START_STOP, 0, 0, 0, mode ? 1 : 0, 0};
	sg_io_hdr.cmd_len = 6;
	sg_io_hdr.timeout = START_STOP_TIMEOUT;

	if (ioctl(fd, SG_IO, &sg_io_hdr) == 0) {
		ret = sg_io_hdr.status;
	} else {
#else
	if (ioctl(fd, mode?SCSI_IOCTL_START_UNIT:SCSI_IOCTL_STOP_UNIT) < 0) {
#endif
		perror(argv[1]);
		ret = 1;
	}

	close(fd);
	exit(ret);
}
