/* Portions Copyright 2001 Sun Microsystems (thockin@sun.com) */
/* Portions Copyright 2002 Intel (scott.feldman@intel.com) */
#ifndef ETHTOOL_UTIL_H__
#define ETHTOOL_UTIL_H__

#include <sys/types.h>
typedef unsigned long long __u64;         /* hack, so we may include kernel's ethtool.h */
typedef __uint32_t __u32;         /* ditto */
typedef __uint16_t __u16;         /* ditto */
typedef __uint8_t __u8;           /* ditto */
#include <linux/ethtool.h>
#endif
