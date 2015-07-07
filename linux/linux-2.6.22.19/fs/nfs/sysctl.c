/*
 * linux/fs/nfs/sysctl.c
 *
 * Sysctl interface to NFS parameters
 */
#include <linux/types.h>
#include <linux/linkage.h>
#include <linux/ctype.h>
#include <linux/fs.h>
#include <linux/sysctl.h>
#include <linux/module.h>
#include <linux/nfs4.h>
#include <linux/nfs_idmap.h>
#include <linux/nfs_fs.h>

#include "callback.h"

static const int nfs_set_port_min = 0;
static const int nfs_set_port_max = 65535;
static struct ctl_table_header *nfs_callback_sysctl_table;

static ctl_table nfs_cb_sysctls[] = {
#ifdef CONFIG_NFS_V4
	{
		.ctl_name = CTL_UNNUMBERED,
		.procname = "nfs_callback_tcpport",
		.data = &nfs_callback_set_tcpport,
		.maxlen = sizeof(int),
		.mode = 0644,
		.proc_handler = &proc_dointvec_minmax,
		.extra1 = (int *)&nfs_set_port_min,
		.extra2 = (int *)&nfs_set_port_max,
	},
	{
		.ctl_name = CTL_UNNUMBERED,
		.procname = "idmap_cache_timeout",
		.data = &nfs_idmap_cache_timeout,
		.maxlen = sizeof(int),
		.mode = 0644,
		.proc_handler = &proc_dointvec_jiffies,
		.strategy = &sysctl_jiffies,
	},
#endif
	{
		.ctl_name	= CTL_UNNUMBERED,
		.procname	= "nfs_mountpoint_timeout",
		.data		= &nfs_mountpoint_expiry_timeout,
		.maxlen		= sizeof(nfs_mountpoint_expiry_timeout),
		.mode		= 0644,
		.proc_handler	= &proc_dointvec_jiffies,
		.strategy	= &sysctl_jiffies,
	},
	{
		.ctl_name	= CTL_UNNUMBERED,
		.procname	= "nfs_congestion_kb",
		.data		= &nfs_congestion_kb,
		.maxlen		= sizeof(nfs_congestion_kb),
		.mode		= 0644,
		.proc_handler	= &proc_dointvec,
	},
	{ .ctl_name = 0 }
};

static const struct ctl_path nfs_cb_path[] = {
	{ .procname = "fs", .ctl_name = CTL_FS, },
	{ .procname = "nfs", .ctl_name = 0, },
	{ }
};

int nfs_register_sysctl(void)
{
	nfs_callback_sysctl_table = register_sysctl_paths(nfs_cb_path, nfs_cb_sysctls);
	if (nfs_callback_sysctl_table == NULL)
		return -ENOMEM;
	return 0;
}

void nfs_unregister_sysctl(void)
{
	unregister_sysctl_table(nfs_callback_sysctl_table);
	nfs_callback_sysctl_table = NULL;
}
