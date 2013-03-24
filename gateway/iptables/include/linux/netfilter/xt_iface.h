#ifndef _LINUX_NETFILTER_XT_IFACE_H
#define _LINUX_NETFILTER_XT_IFACE_H 1

enum {
	XT_IFACE_UP          = 1 << 0,
	XT_IFACE_BROADCAST   = 1 << 1,
	XT_IFACE_LOOPBACK    = 1 << 2,
	XT_IFACE_POINTOPOINT = 1 << 3,
	XT_IFACE_RUNNING     = 1 << 4,
	XT_IFACE_NOARP       = 1 << 5,
	XT_IFACE_PROMISC     = 1 << 6,
	XT_IFACE_MULTICAST   = 1 << 7,
	XT_IFACE_DYNAMIC     = 1 << 8,
	XT_IFACE_LOWER_UP    = 1 << 9,
	XT_IFACE_DORMANT     = 1 << 10,
	XT_IFACE_DEV_IN      = 1 << 11,
	XT_IFACE_DEV_OUT     = 1 << 12,
};

struct xt_iface_mtinfo {
	char ifname[IFNAMSIZ];
	__u16 flags;
	__u16 invflags;
};

#endif
