#ifndef _ASM_SPINLOCK_TYPES_H
#define _ASM_SPINLOCK_TYPES_H

#ifndef __LINUX_SPINLOCK_TYPES_H
# error "please don't include this file directly"
#endif

#include <linux/types.h>

#include <asm/byteorder.h>

typedef union {
	/*
	 * bits  0..15 : serving_now
	 * bits 16..31 : ticket
	 */
	u32 lock;
	struct {
#ifdef __BIG_ENDIAN
		u16 ticket;
		u16 serving_now;
#else
		u16 serving_now;
		u16 ticket;
#endif
	} h;
} raw_spinlock_t;

#define __RAW_SPIN_LOCK_UNLOCKED	{ .lock = 0 }

typedef struct {
	volatile unsigned int lock;
} raw_rwlock_t;

#define __RAW_RW_LOCK_UNLOCKED		{ 0 }

#endif
