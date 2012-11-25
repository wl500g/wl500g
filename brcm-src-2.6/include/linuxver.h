/*
 * Linux-specific abstractions to gain some independence from linux kernel versions.
 * Pave over some 2.4 versus 2.6 kernel differences.
 *
 * Copyright (C) 2008, Broadcom Corporation
 * All Rights Reserved.
 * 
 * THIS SOFTWARE IS OFFERED "AS IS", AND BROADCOM GRANTS NO WARRANTIES OF ANY
 * KIND, EXPRESS OR IMPLIED, BY STATUTE, COMMUNICATION OR OTHERWISE. BROADCOM
 * SPECIFICALLY DISCLAIMS ANY IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS
 * FOR A SPECIFIC PURPOSE OR NONINFRINGEMENT CONCERNING THIS SOFTWARE.
 *
 * $Id: linuxver.h,v 13.40.2.4 2008/05/02 20:40:31 Exp $
 */

#ifndef _linuxver_h_
#define _linuxver_h_

#include <linux/version.h>
#if (LINUX_VERSION_CODE < KERNEL_VERSION(2, 6, 0))
#include <linux/config.h>
#elif (LINUX_VERSION_CODE < KERNEL_VERSION(2, 6, 19))
#include <linux/autoconf.h>
#endif
#include <linux/module.h>

#if LINUX_VERSION_CODE < KERNEL_VERSION(2, 5, 0)
#define module_param(_name_, _type_, _perm_)	MODULE_PARM(_name_, "i")
#define module_param_string(_name_, _string_, _size_, _perm_) \
		MODULE_PARM(_string_, "c" __MODULE_STRING(_size_))
#endif

/* linux/malloc.h is deprecated, use linux/slab.h instead. */
#include <linux/slab.h>

#include <linux/types.h>
#include <linux/init.h>
#include <linux/mm.h>
#include <linux/string.h>
#include <linux/pci.h>
#include <linux/interrupt.h>
#include <linux/netdevice.h>
#include <asm/io.h>

#include <linux/workqueue.h>
#ifndef INIT_WORK
#define INIT_WORK(_work, _func, _data) INIT_TQUEUE((_work), (_func), (_data))
#endif
#ifndef schedule_work
#define schedule_work(_work) schedule_task((_work))
#endif
#ifndef flush_scheduled_work
#define flush_scheduled_work() flush_scheduled_tasks()
#endif

#if LINUX_VERSION_CODE >= KERNEL_VERSION(2, 6, 19)
#define	MY_INIT_WORK(_work, _func)	INIT_WORK(_work, _func)
#else
#define	MY_INIT_WORK(_work, _func)	INIT_WORK(_work, _func, _work)
typedef void (*work_func_t)(void *work);
#endif	/* >= 2.6.20 */

#if LINUX_VERSION_CODE < KERNEL_VERSION(2, 6, 18)
#define IRQF_SHARED	SA_SHIRQ
#endif /* < 2.6.18 */

#if LINUX_VERSION_CODE < KERNEL_VERSION(2, 6, 17)
#ifdef	CONFIG_NET_RADIO
#define	CONFIG_WIRELESS_EXT
#endif
#endif	/* < 2.6.17 */

#ifndef SANDGATE2G
#define MOD_INC_USE_COUNT
#define MOD_DEC_USE_COUNT
#endif /* not SANDGATE2G */


#if defined(CONFIG_PCMCIA) || defined(CONFIG_PCMCIA_MODULE)

#include <pcmcia/version.h>
#include <pcmcia/cs_types.h>
#include <pcmcia/cs.h>
#include <pcmcia/cistpl.h>
#include <pcmcia/cisreg.h>
#include <pcmcia/ds.h>

#if (LINUX_VERSION_CODE > KERNEL_VERSION(2, 6, 16))

typedef	struct pcmcia_device dev_link_t;

#endif

#endif /* CONFIG_PCMCIA */


#ifndef __exit
#define __exit
#endif
#ifndef __devexit
#define __devexit
#endif
#ifndef __devinit
#define __devinit	__init
#endif
#ifndef __devinitdata
#define __devinitdata
#endif
#ifndef __devexit_p
#define __devexit_p(x)	x
#endif

#if (LINUX_VERSION_CODE >= KERNEL_VERSION(2, 6, 18))
#define pci_module_init pci_register_driver
#endif

#define netif_device_attach(dev)	netif_start_queue(dev)
#define netif_device_detach(dev)	netif_stop_queue(dev)

/* SoftNet */
#define netif_down(dev)

/* Power management related macro & routines */
#if LINUX_VERSION_CODE > KERNEL_VERSION(2, 6, 9)
#define	PCI_SAVE_STATE(a, b)	pci_save_state(a)
#define	PCI_RESTORE_STATE(a, b)	pci_restore_state(a)
#else
#define	PCI_SAVE_STATE(a, b)	pci_save_state(a, b)
#define	PCI_RESTORE_STATE(a, b)	pci_restore_state(a, b)
#endif

/* Module refcount handled internally in 2.6.x */
#if (LINUX_VERSION_CODE < KERNEL_VERSION(2, 6, 24))
#ifndef SET_MODULE_OWNER
#define SET_MODULE_OWNER(dev)		do {} while (0)
#define OLD_MOD_INC_USE_COUNT		MOD_INC_USE_COUNT
#define OLD_MOD_DEC_USE_COUNT		MOD_DEC_USE_COUNT
#else
#define OLD_MOD_INC_USE_COUNT		do {} while (0)
#define OLD_MOD_DEC_USE_COUNT		do {} while (0)
#endif
#else /* LINUX_VERSION_CODE < KERNEL_VERSION(2, 6, 24) */
#ifndef SET_MODULE_OWNER
#define SET_MODULE_OWNER(dev)		do {} while (0)
#endif
#ifndef MOD_INC_USE_COUNT
#define MOD_INC_USE_COUNT			do {} while (0)
#endif
#ifndef MOD_DEC_USE_COUNT
#define MOD_DEC_USE_COUNT			do {} while (0)
#endif
#define OLD_MOD_INC_USE_COUNT		MOD_INC_USE_COUNT
#define OLD_MOD_DEC_USE_COUNT		MOD_DEC_USE_COUNT
#endif /* LINUX_VERSION_CODE >= KERNEL_VERSION(2, 6, 24) */

#ifndef SET_NETDEV_DEV
#define SET_NETDEV_DEV(net, pdev)	do {} while (0)
#endif

#ifndef HAVE_FREE_NETDEV
#define free_netdev(dev)		kfree(dev)
#endif

/* suspend args */
#if LINUX_VERSION_CODE >= KERNEL_VERSION(2, 6, 11)
#define DRV_SUSPEND_STATE_TYPE pm_message_t
#else
#define DRV_SUSPEND_STATE_TYPE uint32
#endif

#if LINUX_VERSION_CODE >= KERNEL_VERSION(2, 6, 19)
#define CHECKSUM_HW	CHECKSUM_PARTIAL
#endif

#include <linux/time.h>
#include <linux/wait.h>

#endif /* _linuxver_h_ */
