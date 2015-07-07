/*
 * Copyright (C) 2003 Christoph Hellwig.
 *	Released under GPL v2.
 */

#include <linux/errno.h>
#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/sysctl.h>

#include "scsi_logging.h"


static ctl_table scsi_table[] = {
	{ .ctl_name	= DEV_SCSI_LOGGING_LEVEL,
	  .procname	= "logging_level",
	  .data		= &scsi_logging_level,
	  .maxlen	= sizeof(scsi_logging_level),
	  .mode		= 0644,
	  .proc_handler	= &proc_dointvec },
	{ }
};

static const struct ctl_path scsi_path[] = {
	{ .procname = "dev", .ctl_name = CTL_DEV, },
	{ .procname = "scsi", .ctl_name = DEV_SCSI, },
	{ }
};

static struct ctl_table_header *scsi_table_header;

int __init scsi_init_sysctl(void)
{
	scsi_table_header = register_sysctl_paths(scsi_path, scsi_table);
	if (!scsi_table_header)
		return -ENOMEM;
	return 0;
}

void scsi_exit_sysctl(void)
{
	unregister_sysctl_table(scsi_table_header);
}
