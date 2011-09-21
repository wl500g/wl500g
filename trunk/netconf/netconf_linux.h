/*
 * Network configuration layer (Linux)
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

#ifndef _netconf_linux_h_
#define _netconf_linux_h_

/* Debug malloc() */
#ifdef DMALLOC
#include <dmalloc.h>
#endif /* DMALLOC */

/* iptables definitions */
#include <libiptc/libiptc.h>
#include <iptables.h>
#include <linux/netfilter_ipv4/ip_tables.h>
#include <linux/netfilter_ipv4/ip_nat_rule.h>
#define ETH_ALEN ETHER_ADDR_LEN
#include <linux/netfilter_ipv4/ipt_mac.h>
#include <linux/netfilter_ipv4/ipt_state.h>
#ifdef LINUX26
# include <linux/netfilter/xt_time.h>
# define ipt_time_info xt_time_info
# include <linux/netfilter/xt_conntrack.h>
# define ipt_conntrack_info xt_conntrack_mtinfo2
#else
# include <linux/netfilter_ipv4/ipt_time.h>
# include <linux/netfilter_ipv4/ipt_conntrack.h>
#endif
#include <linux/netfilter_ipv4/ipt_TCPMSS.h>
#include <linux/netfilter_ipv4/ipt_LOG.h>
#include <linux/netfilter_ipv4/ip_autofw.h>
#ifdef WEBSTRFILTER
# ifdef LINUX26
#  include <linux/netfilter/xt_webstr.h>
#  define ipt_webstr_info xt_webstr_info
# else
#  include <linux/netfilter_ipv4/ipt_webstr.h>
# endif
#endif

/* ipt_entry alignment attribute */
#define IPT_ALIGNED ((aligned (__alignof__ (struct ipt_entry))))

/* TCP flags */
#define	TH_FIN	0x01
#define	TH_SYN	0x02
#define	TH_RST	0x04
#define	TH_PUSH	0x08
#define	TH_ACK	0x10
#define	TH_URG	0x20

#endif /* _netconf_linux_h_ */
