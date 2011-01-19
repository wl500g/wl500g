/*	$KAME: config.c,v 1.53 2005/09/16 11:30:14 suz Exp $	*/

/*
 * Copyright (C) 2002 WIDE Project.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name of the project nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE PROJECT AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE PROJECT OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/queue.h>
#include <sys/ioctl.h>

#include <net/if.h>
#include <netinet/in.h>
#ifdef __KAME__
#include <net/if_dl.h>
#endif
#ifdef __linux__
#include <linux/if_packet.h>
#endif

#include <syslog.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ifaddrs.h>
#include <errno.h>
#ifdef __linux__
#define __USE_XOPEN
#include <time.h>
#endif

#include <dhcp6.h>
#include <config.h>
#include <common.h>
#include <auth.h>
#include <base64.h>

extern int errno;

struct prefix_ifconf *prefix_ifconflist;
struct dhcp6_list siplist, sipnamelist, dnslist, dnsnamelist, ntplist;
struct dhcp6_list nislist, nisnamelist;
struct dhcp6_list nisplist, nispnamelist;
struct dhcp6_list bcmcslist, bcmcsnamelist;
long long optrefreshtime;

static struct dhcp6_ifconf *dhcp6_ifconflist;
struct ia_conflist ia_conflist0;
static struct host_conf *host_conflist0, *host_conflist;
static struct keyinfo *key_list, *key_list0;
static struct authinfo *auth_list, *auth_list0;
static struct dhcp6_list siplist0, sipnamelist0, dnslist0, dnsnamelist0, ntplist0;
static struct dhcp6_list nislist0, nisnamelist0;
static struct dhcp6_list nisplist0, nispnamelist0;
static struct dhcp6_list bcmcslist0, bcmcsnamelist0;
static long long optrefreshtime0 = -1;
#ifndef DHCP6_DYNAMIC_HOSTCONF_MAX
#define DHCP6_DYNAMIC_HOSTCONF_MAX	1024
#endif
struct dynamic_hostconf {
	TAILQ_ENTRY(dynamic_hostconf) link;
	struct host_conf *host;
};
static TAILQ_HEAD(dynamic_hostconf_listhead, dynamic_hostconf)
	dynamic_hostconf_head;
static unsigned int dynamic_hostconf_count;
static struct pool_conf *pool_conflist, *pool_conflist0;

enum { DHCPOPTCODE_SEND, DHCPOPTCODE_REQUEST, DHCPOPTCODE_ALLOW };

/* temporary configuration structure for DHCP interface */
struct dhcp6_ifconf {
	struct dhcp6_ifconf *next;

	char *ifname;

	/* configuration flags */
	u_long send_flags;
	u_long allow_flags;

	int server_pref;	/* server preference (server only) */

	char *scriptpath;	/* path to config script (client only) */

	struct dhcp6_list reqopt_list;
	struct ia_conflist iaconf_list;

	struct authinfo *authinfo; /* authentication information
				    * (no need to clear) */
	struct dhcp6_poolspec pool;
};

extern struct cf_list *cf_dns_list, *cf_dns_name_list, *cf_ntp_list;
extern struct cf_list *cf_sip_list, *cf_sip_name_list;
extern struct cf_list *cf_nis_list, *cf_nis_name_list;
extern struct cf_list *cf_nisp_list, *cf_nisp_name_list;
extern struct cf_list *cf_bcmcs_list, *cf_bcmcs_name_list;
extern long long cf_refreshtime;
extern char *configfilename;

static struct keyinfo *find_keybyname __P((struct keyinfo *, char *));
static int add_pd_pif __P((struct iapd_conf *, struct cf_list *));
static int add_options __P((int, struct dhcp6_ifconf *, struct cf_list *));
static int add_prefix __P((struct dhcp6_list *, char *, int,
    struct dhcp6_prefix *));
static void clear_pd_pif __P((struct iapd_conf *));
static void clear_ifconf __P((struct dhcp6_ifconf *));
static void clear_iaconf __P((struct ia_conflist *));
static void clear_hostconf __P((struct host_conf *));
static void clear_keys __P((struct keyinfo *));
static void clear_authinfo __P((struct authinfo *));
static int configure_duid __P((char *, struct duid *));
static int configure_addr __P((struct cf_list *, struct dhcp6_list *, char *));
static int configure_domain __P((struct cf_list *, struct dhcp6_list *, char *));
static int get_default_ifid __P((struct prefix_ifconf *));
static void clear_poolconf __P((struct pool_conf *));
static struct pool_conf *create_pool __P((char *, struct dhcp6_range *));
struct host_conf *find_dynamic_hostconf __P((struct duid *));
static int in6_addr_cmp __P((struct in6_addr *, struct in6_addr *));

int
configure_interface(struct cf_namelist *iflist)
{
	struct cf_namelist *ifp;
	struct dhcp6_ifconf *ifc;
	char *cp;

	for (ifp = iflist; ifp; ifp = ifp->next) {
		struct cf_list *cfl;

		if (if_nametoindex(ifp->name) == 0) {
			log_error("invalid interface(%s): %s",
			    ifp->name, strerror(errno));
			goto bad;
		}

		if ((ifc = malloc(sizeof(*ifc))) == NULL) {
			log_error("memory allocation for %s failed", ifp->name);
			goto bad;
		}
		memset(ifc, 0, sizeof(*ifc));
		ifc->next = dhcp6_ifconflist;
		dhcp6_ifconflist = ifc;

		if ((ifc->ifname = strdup(ifp->name)) == NULL) {
			log_error("failed to copy ifname");
			goto bad;
		}

		ifc->server_pref = DH6OPT_PREF_UNDEF;
		TAILQ_INIT(&ifc->reqopt_list);
		TAILQ_INIT(&ifc->iaconf_list);

		for (cfl = ifp->params; cfl; cfl = cfl->next) {
			switch(cfl->type) {
			case DECL_REQUEST:
				if (dhcp6_mode != DHCP6_MODE_CLIENT) {
					log_info("%s:%d "
						"client-only configuration",
						configfilename,
						cfl->line);
					goto bad;
				}
				if (add_options(DHCPOPTCODE_REQUEST,
						ifc, cfl->list)) {
					goto bad;
				}
				break;
			case DECL_SEND:
				if (add_options(DHCPOPTCODE_SEND,
						ifc, cfl->list)) {
					goto bad;
				}
				break;
			case DECL_ALLOW:
				if (add_options(DHCPOPTCODE_ALLOW,
						ifc, cfl->list)) {
					goto bad;
				}
				break;
			case DECL_INFO_ONLY:
				if (dhcp6_mode != DHCP6_MODE_CLIENT) {
					log_info("%s:%d "
						"client-only configuration",
						configfilename, cfl->line);
					goto bad;
				}
				ifc->send_flags |= DHCIFF_INFO_ONLY;
				break;
			case DECL_PREFERENCE:
				if (dhcp6_mode != DHCP6_MODE_SERVER) {
					log_info("%s:%d "
						"server-only configuration",
						configfilename, cfl->line);
					goto bad;
				}
				ifc->server_pref = (int)cfl->num;
				if (ifc->server_pref < 0 ||
				    ifc->server_pref > 255) {
					log_info("%s:%d "
						"bad value: %d",
						configfilename, cfl->line,
						ifc->server_pref);
					goto bad;
				}
				break;
			case DECL_SCRIPT:
				if (dhcp6_mode != DHCP6_MODE_CLIENT) {
					log_info("%s:%d "
						"client-only configuration",
						configfilename, cfl->line);
					goto bad;
				}
				if (ifc->scriptpath) {
					log_info(
					    "%s:%d duplicated configuration",
					    configfilename, cfl->line);
					goto bad;
				}
				cp = cfl->ptr;
				ifc->scriptpath = strdup(cp + 1);
				if (ifc->scriptpath == NULL) {
					log_notice(
					    "failed to copy script path");
					goto bad;
				}
				cp = ifc->scriptpath;
				if (*cp != '/') {
					log_info(
					    "script must be an absolute path");
					goto bad;
				}
				cp += strlen(ifc->scriptpath) - 1;
				*cp = '\0'; /* clear the terminating quote */
				break;
			case DECL_ADDRESSPOOL:
				{
					struct dhcp6_poolspec* spec;
					struct pool_conf* pool;

					spec = (struct dhcp6_poolspec *)cfl->ptr;

					for (pool = pool_conflist0; pool; pool = pool->next)
						if (strcmp(spec->name, pool->name) == 0)
							break;
					if (pool == NULL) {
						log_error("%s:%d "
							"pool '%s' not found",
							configfilename, cfl->line,
					   		spec->name);
						goto bad;
					}
					if (spec->vltime != DHCP6_DURATION_INFINITE &&
						(spec->pltime == DHCP6_DURATION_INFINITE ||
						spec->pltime > spec->vltime)) {
						log_error("%s:%d ",
							configfilename, cfl->line,
							"specified a larger preferred lifetime "
							"than valid lifetime");
						goto bad;
					}
					ifc->pool = *spec;
					if ((ifc->pool.name = strdup(spec->name)) == NULL) {
						log_error("memory allocation failed");
						goto bad;
					}
					log_debug("pool '%s' is specified to the interface '%s'",
						ifc->pool.name, ifc->ifname);
				}
				break;
			default:
				log_error("%s:%d "
					"invalid interface configuration",
					configfilename, cfl->line);
				goto bad;
			}
		}
	}
	
	return (0);

  bad:
	clear_ifconf(dhcp6_ifconflist);
	dhcp6_ifconflist = NULL;
	return (-1);
}

int
configure_ia(struct cf_namelist *ialist, iatype_t iatype)
{
	struct cf_namelist *iap;
	struct ia_conf *iac = NULL;
	size_t confsize;
	static int init = 1;

	if (init) {
		TAILQ_INIT(&ia_conflist0);
		init = 0;
	}

	switch(iatype) {
	case IATYPE_PD:
		confsize = sizeof(struct iapd_conf);
		break;
	case IATYPE_NA:
		confsize = sizeof(struct iana_conf);
		break;
	default:
		log_error("internal error");
		goto bad;
	}

	for (iap = ialist; iap; iap = iap->next) {
		struct cf_list *cfl;

		if ((iac = malloc(confsize)) == NULL) {
			log_error("memory allocation for IA %s failed",
			    iap->name);
			goto bad;
		}
		memset(iac, 0, confsize);

		/* common initialization */
		iac->type = iatype;
		iac->iaid = (u_int32_t)atoi(iap->name);
		TAILQ_INIT(&iac->iadata);
		TAILQ_INSERT_TAIL(&ia_conflist0, iac, link);

		/* IA-type specific initialization */
		switch(iatype) {
		case IATYPE_PD:
			TAILQ_INIT(&((struct iapd_conf *)iac)->iapd_prefix_list);
			TAILQ_INIT(&((struct iapd_conf *)iac)->iapd_pif_list);
			break;
		case IATYPE_NA:
			TAILQ_INIT(&((struct iana_conf *)iac)->iana_address_list);
			break;
		}

		/* set up parameters for the IA */
		for (cfl = iap->params; cfl; cfl = cfl->next) {
			struct iapd_conf *pdp = (struct iapd_conf *) iac;
			struct iana_conf *nap = (struct iana_conf *) iac;

			switch (iatype) {
			case IATYPE_PD:
				switch(cfl->type) {
				case IACONF_PIF:
					if (add_pd_pif(pdp, cfl))
						goto bad;
					break;
				case IACONF_PREFIX:
					if (add_prefix(&pdp->iapd_prefix_list,
					    "IAPD", DHCP6_LISTVAL_PREFIX6,
					    cfl->ptr)) {
						log_notice("failed "
							"to configure prefix");
						goto bad;
					}
					break;
				default:
					log_error("%s:%d "
					    "invalid configuration",
					    configfilename, cfl->line);
					goto bad;
				}
				break;
			case IATYPE_NA:
				switch(cfl->type) {
				case IACONF_ADDR:
					if (add_prefix(&nap->iana_address_list,
					    "IANA", DHCP6_LISTVAL_STATEFULADDR6,
					    cfl->ptr)) {
						log_notice("failed "
							"to configure address");
						goto bad;
					}
					break;
				default:
					log_error("%s:%d "
					    "invalid configuration",
					    configfilename, cfl->line);
					goto bad;
				}
				break;
			default:
				log_error("%s:%d "
				    "invalid iatype %d",
				    configfilename, cfl->line, iatype);
				goto bad;
			}
		}
	}

	return (0);

  bad:
	return (-1);
}

static int
add_pd_pif(struct iapd_conf *iapdc, struct cf_list *cfl0)
{
	struct cf_list *cfl;
	struct prefix_ifconf *pif;

	/* duplication check */
	for (pif = TAILQ_FIRST(&iapdc->iapd_pif_list); pif;
	    pif = TAILQ_NEXT(pif, link)) {
		if (strcmp(pif->ifname, cfl0->ptr) == 0) {
			log_notice("%s:%d "
			    "duplicated prefix interface: %s",
			    configfilename, cfl0->line, cfl0->ptr);
			return (0); /* ignore it */
		}
	}

	if ((pif = malloc(sizeof(*pif))) == NULL) {
		log_error("memory allocation for %s failed", cfl0->ptr);
		goto bad;
	}
	memset(pif, 0, sizeof(*pif));

	/* validate and copy ifname */
	if (if_nametoindex(cfl0->ptr) == 0) {
		log_error("%s:%d invalid interface (%s): %s",
		    configfilename, cfl0->line,
		    cfl0->ptr, strerror(errno));
		goto bad;
	}
	if ((pif->ifname = strdup(cfl0->ptr)) == NULL) {
		log_error("failed to copy ifname");
		goto bad;
	}

	pif->ifid_len = IFID_LEN_DEFAULT;
	pif->sla_len = SLA_LEN_DEFAULT;
	if (get_default_ifid(pif)) {
		log_notice(
		    "failed to get default IF ID for %s", pif->ifname);
		goto bad;
	}

	for (cfl = cfl0->list; cfl; cfl = cfl->next) {
		switch(cfl->type) {
		case IFPARAM_SLA_ID:
			pif->sla_id = (u_int32_t)cfl->num;
			break;
		case IFPARAM_SLA_LEN:
			pif->sla_len = (int)cfl->num;
			if (pif->sla_len < 0 || pif->sla_len > 128) {
				log_error("%s:%d "
				    "invalid SLA length: %d",
				    configfilename, cfl->line, pif->sla_len); 
				goto bad;
			}
			break;
		default:
			log_error("%s:%d internal error: "
			    "invalid configuration",
			    configfilename, cfl->line);
			goto bad;
		}
	}

	TAILQ_INSERT_TAIL(&iapdc->iapd_pif_list, pif, link);
	return (0);

  bad:
	if (pif->ifname)
		free(pif->ifname);
	free(pif);
	return (-1);
}

int
configure_host(struct cf_namelist *hostlist)
{
	struct cf_namelist *host;
	struct host_conf *hconf;

	for (host = hostlist; host; host = host->next) {
		struct cf_list *cfl;

		if ((hconf = malloc(sizeof(*hconf))) == NULL) {
			log_error("memory allocation failed "
				"for host %s", host->name);
			goto bad;
		}
		memset(hconf, 0, sizeof(*hconf));
		TAILQ_INIT(&hconf->prefix_list);
		TAILQ_INIT(&hconf->addr_list);
		hconf->next = host_conflist0;
		host_conflist0 = hconf;

		if ((hconf->name = strdup(host->name)) == NULL) {
			log_error("failed to copy host name: %s",
			    host->name);
			goto bad;
		}

		for (cfl = host->params; cfl; cfl = cfl->next) {
			switch(cfl->type) {
			case DECL_DUID:
				if (hconf->duid.duid_id) {
					log_error("%s:%d "
					    "duplicated DUID for %s",
					    configfilename,
					    cfl->line, host->name);
					goto bad;
				}
				if ((configure_duid((char *)cfl->ptr,
						    &hconf->duid)) != 0) {
					log_error("%s:%d "
					    "failed to configure "
					    "DUID for %s",
					    configfilename, cfl->line,
					    host->name);
					goto bad;
				}
				log_debug("configure DUID for %s: %s",
				    host->name, duidstr(&hconf->duid));
				break;
			case DECL_PREFIX:
				if (add_prefix(&hconf->prefix_list,
				    hconf->name, DHCP6_LISTVAL_PREFIX6,
				    cfl->ptr)) {
					log_error("failed "
					    "to configure prefix for %s",
					    host->name);
					goto bad;
				}
				break;
			case DECL_ADDRESS:
				if (add_prefix(&hconf->addr_list,
				    hconf->name, DHCP6_LISTVAL_STATEFULADDR6,
				    cfl->ptr)) {
					log_error("failed "
					    "to configure address for %s",
					    host->name);
					goto bad;
				}
				break;
			case DECL_DELAYEDKEY:
				if (hconf->delayedkey != NULL) {
					log_warning(
					    "%s:%d: duplicate key %s for %s"
					    " (ignored)", configfilename,
					    cfl->line, cfl->ptr, host->name);
					continue;
				}
				if ((hconf->delayedkey =
				    find_keybyname(key_list0, cfl->ptr))
				    == NULL) {
					log_error("failed to "
					    "find key information for %s",
					    cfl->ptr);
					goto bad;
				}
				log_debug("configure key for "
				    "delayed auth with %s (keyid=%08x)",
				    host->name, hconf->delayedkey->keyid);
				break;
			case DECL_ADDRESSPOOL:
				{
					struct dhcp6_poolspec* spec;
					struct pool_conf *pool;

					spec = (struct dhcp6_poolspec *)cfl->ptr;

					for (pool = pool_conflist0; pool; pool = pool->next)
						if (strcmp(spec->name, pool->name) == 0)
							break;
					if (pool == NULL) {
						log_error("%s:%d "
							"pool '%s' not found",
							configfilename, cfl->line,
					   		spec->name);
						goto bad;
					}
					if (spec->vltime != DHCP6_DURATION_INFINITE &&
						(spec->pltime == DHCP6_DURATION_INFINITE ||
						spec->pltime > spec->vltime)) {
						log_error("%s:%d ",
							configfilename, cfl->line,
							"specified a larger preferred lifetime "
							"than valid lifetime");
						goto bad;
					}
					hconf->pool = *spec;
					if ((hconf->pool.name = strdup(spec->name)) == NULL) {
						log_error(
							"memory allocation failed");
						goto bad;
					}
					log_debug("pool '%s' is specified to the host '%s'",
						hconf->pool.name, hconf->name);
				}
				break;
			default:
				log_error("%s:%d "
				    "invalid host configuration for %s",
				    configfilename, cfl->line,
				    host->name);
				goto bad;
			}
		}
	}

	return (0);

  bad:
	/* there is currently nothing special to recover the error */
	return (-1);
}

int
configure_keys(struct cf_namelist *keylist)
{
	struct cf_namelist *key;
	char *secretstr;
	char secret[1024];
	int secretlen;
	struct keyinfo *kinfo;
	long long keyid;
	char *expire = NULL;

	for (key = keylist; key; key = key->next) {
		struct cf_list *cfl;

		if ((kinfo = malloc(sizeof(*kinfo))) == NULL) {
			log_error("memory allocation failed "
				"for key %s", key->name);
			goto bad;
		}
		memset(kinfo, 0, sizeof(*kinfo));
		kinfo->next = key_list0;
		key_list0 = kinfo;

		if ((kinfo->name = strdup(key->name)) == NULL) {
			log_error("failed to copy key name: %s",
			    key->name);
			goto bad;
		}

		keyid = -1;
		expire = NULL;
		for (cfl = key->params; cfl; cfl = cfl->next) {
			switch (cfl->type) {
			case KEYPARAM_REALM:
				if (kinfo->realm != NULL) {
					log_warning(
					    "%s:%d duplicate realm for key %s "
					    "(ignored)", configfilename,
					    cfl->line, key->name);
					continue;
				}
				kinfo->realm = qstrdup(cfl->ptr);
				if (kinfo->realm == NULL) {
					log_warning(
					    "failed to allocate memory for "
					    "realm");
					goto bad;
				}
				kinfo->realmlen = strlen(kinfo->realm);
				break;
			case KEYPARAM_KEYID:
				if (keyid != -1) {
					log_warning(
					    "%s:%d duplicate realm for key %s "
					    "(ignored)",
					    configfilename, cfl->line);
					continue;
				}
				keyid = cfl->num;
				if (keyid < 0 || keyid > 0xffffffff) {
					log_warning(
					    "%s:%d key ID overflow",
					     configfilename, cfl->line);
					goto bad;
				}
				break;
			case KEYPARAM_SECRET:
				/* duplicate check */
				if (kinfo->secret != NULL) {
					log_warning(
					    "%s:%d duplicate secret "
					    "for key %s (ignored)",
					    configfilename, cfl->line,
					    key->name);
					continue; /* ignored */
				}

				/* convert base64 string to binary secret */
				if ((secretstr = qstrdup(cfl->ptr)) == NULL) {
					log_warning(
					    "failed to make a copy of secret");
					goto bad;
				}
				memset(secret, 0, sizeof(secret));
				secretlen = base64_decodestring(secretstr,
				    secret, sizeof(secret));
				if (secretlen < 0) {
					log_error(
					    "%s:%d failed to parse base64 key",
					    configfilename, cfl->line);
					free(secretstr);
					goto bad;
				}
				free(secretstr);

				/* set the binary secret */
				kinfo->secret = malloc(secretlen);
				if (kinfo->secret == NULL) {
					log_warning(
					    "failed to allocate memory "
					    "for secret");
					goto bad;
				}
				memcpy(kinfo->secret, secret, secretlen); 
				kinfo->secretlen = secretlen;
				break;
			case KEYPARAM_EXPIRE:
				if (expire != NULL) {
					log_warning(
					    "%s:%d duplicate expire for key "
					    "%s (ignored)", configfilename,
					    cfl->line, key->name);
					continue;
				}
				expire = qstrdup(cfl->ptr);
				break;
			default:
				log_error(
				    "%s:%d invalid key parameter for %s",
				    configfilename, cfl->line, key->name);
				goto bad;
			}
		}

		/* check for mandatory parameters or use default */
		if (kinfo->realm == NULL) {
			log_error(
			    "realm not specified for key %s", key->name);
			goto bad;
		}
		if (keyid == -1) {
			log_error(
			    "key ID not specified for key %s", key->name);
			goto bad;
		}
		kinfo->keyid = keyid;
		if (kinfo->secret == NULL) {
			log_error(
			    "secret not specified for key %s", key->name);
			goto bad;
		}
		kinfo->expire = 0;
		if (expire != NULL) {
			if (strcmp(expire, "forever") != 0) {
				time_t now, expire_time;
				struct tm *lt;

				if (time(&now) == -1) {
					log_error("cannot get "
					    "current time: %s",
					    strerror(errno));
					goto bad;
				}
				lt = localtime(&now);
				lt->tm_sec = 0;
				
				if (strptime(expire, "%Y-%m-%d %H:%M", lt)
				    == NULL &&
				    strptime(expire, "%m-%d %H:%M", lt)
				    == NULL &&
				    strptime(expire, "%H:%M", lt) == NULL) {
					log_error("invalid "
					    "expiration time: %s");
					goto bad;
				}

				expire_time = mktime(lt);
				if (expire_time < now) {
					log_error("past "
					    "expiration time specified: %s",
					    expire);
					goto bad;
				}

				kinfo->expire = expire_time;
			}
		}
	}

	return (0);

  bad:
	if (expire != NULL)
		free(expire);
	return (-1);
}

static struct keyinfo *
find_keybyname(struct keyinfo *head, char *kname)
{
	struct keyinfo *kinfo;

	for (kinfo = head; kinfo != NULL; kinfo = kinfo->next) {
		if (strcmp(kname, kinfo->name) == 0)
			return (kinfo);
	}

	return (NULL);
}

int
configure_authinfo(struct cf_namelist *authlist)
{
	struct cf_namelist *auth;
	struct authinfo *ainfo;

	for (auth = authlist; auth; auth = auth->next) {
		struct cf_list *cfl;

		if ((ainfo = malloc(sizeof(*ainfo))) == NULL) {
			log_error("memory allocation failed "
				"for auth info %s", auth->name);
			goto bad;
		}
		memset(ainfo, 0, sizeof(*ainfo));
		ainfo->next = auth_list0;
		auth_list0 = ainfo;
		ainfo->protocol = DHCP6_AUTHPROTO_UNDEF;
		ainfo->algorithm = DHCP6_AUTHALG_UNDEF;
		ainfo->rdm = DHCP6_AUTHRDM_UNDEF;

		if ((ainfo->name = strdup(auth->name)) == NULL) {
			log_error("failed to copy auth info name: %s", auth->name);
			goto bad;
		}

		for (cfl = auth->params; cfl; cfl = cfl->next) {
			switch (cfl->type) {
			case AUTHPARAM_PROTO:
				if (ainfo->protocol != DHCP6_AUTHPROTO_UNDEF) {
					log_warning(
					    "%s:%d duplicate protocol "
					    "for auth info %s "
					    "(ignored)",
					    configfilename, cfl->line,
					    auth->name);
					continue; /* ignored */
				}
				ainfo->protocol = (int)cfl->num;
				break;
			case AUTHPARAM_ALG:
				if (ainfo->algorithm != DHCP6_AUTHALG_UNDEF) {
					log_warning(
					    "%s:%d duplicate algorithm "
					    "for auth info %s "
					    "(ignored)",
					    configfilename, cfl->line,
					    auth->name);
					continue; /* ignored */
				}
				ainfo->algorithm = (int)cfl->num;
				break;
			case AUTHPARAM_RDM:
				if (ainfo->rdm != DHCP6_AUTHRDM_UNDEF) {
					log_warning(
					    "%s:%d duplicate RDM "
					    "for auth info %s "
					    "(ignored)",
					    configfilename, cfl->line,
					    auth->name);
					continue; /* ignored */
				}
				ainfo->rdm = (int)cfl->num;
				break;
			case AUTHPARAM_KEY:
				log_warning(
				    "%s:%d auth info specific keys "
				    "are not supported",
				    configfilename, cfl->line);
				break;
			default:
				log_error("%s:%d invalid auth info parameter for %s",
				    configfilename, cfl->line, auth->name);
				goto bad;
			}
		}

		/* check for mandatory parameters and consistency */
		switch (ainfo->protocol) {
		case DHCP6_AUTHPROTO_UNDEF:
			log_error("auth protocol is not specified for %s",
			    auth->name);
			goto bad;
		case DHCP6_AUTHPROTO_DELAYED:
			if (dhcp6_mode != DHCP6_MODE_CLIENT) {
				log_error("client-only auth protocol is specified");
				goto bad;
			}
			break;
		case DHCP6_AUTHPROTO_RECONFIG:
			if (dhcp6_mode != DHCP6_MODE_SERVER) {
				log_error("server-only auth protocol is specified");
				goto bad;
			}
			break;
		}
		if (ainfo->algorithm == DHCP6_AUTHALG_UNDEF)
			ainfo->algorithm = DHCP6_AUTHALG_HMACMD5;
		if (ainfo->rdm == DHCP6_AUTHRDM_UNDEF)
			ainfo->rdm = DHCP6_AUTHRDM_MONOCOUNTER;
	}

	return (0);

  bad:
	/* there is currently nothing special to recover the error */
	return (-1);
}

int
configure_global_option()
{
	/* SIP Server address */
	if (configure_addr(cf_sip_list, &siplist0, "SIP") < 0)
		goto bad;

	/* SIP domain name */
	if (configure_domain(cf_sip_name_list, &sipnamelist0, "SIP") < 0)
		goto bad;

	/* DNS servers */
	if (configure_addr(cf_dns_list, &dnslist0, "DNS") < 0)
		goto bad;

	/* DNS name */
	if (configure_domain(cf_dns_name_list, &dnsnamelist0, "DNS") < 0)
		goto bad;

	/* NTP servers */
	if (configure_addr(cf_ntp_list, &ntplist0, "NTP") < 0)
		goto bad;

	/* NIS Server address */
	if (configure_addr(cf_nis_list, &nislist0, "NIS") < 0)
		goto bad;

	/* NIS domain name */
	if (configure_domain(cf_nis_name_list, &nisnamelist0, "NIS") < 0)
		goto bad;

	/* NIS+ Server address */
	if (configure_addr(cf_nisp_list, &nisplist0, "NISP") < 0)
		goto bad;

	/* NIS+ domain name */
	if (configure_domain(cf_nisp_name_list, &nispnamelist0, "NISP") < 0)
		goto bad;

	/* BCMCS Server address */
	if (configure_addr(cf_bcmcs_list, &bcmcslist0, "BCMCS") < 0)
		goto bad;

	/* BCMCS domain name */
	if (configure_domain(cf_bcmcs_name_list, &bcmcsnamelist0, "BCMCS") < 0)
		goto bad;

	/* Lifetime for stateless options */
	if (cf_refreshtime >= 0) {
		optrefreshtime0 = cf_refreshtime;
	}

	return (0);

  bad:
	return (-1);		/* no need to free intermediate list */
}

static int
configure_addr(struct cf_list *cf_addr_list, struct dhcp6_list *list0, char *optname)
{
	struct cf_list *cl;

	/* check against configuration restriction */
	if (cf_addr_list != NULL && dhcp6_mode != DHCP6_MODE_SERVER) {
		log_info("%s:%d server-only configuration",
		    configfilename, cf_addr_list->line);
		return -1;
	}

	TAILQ_INIT(list0);
	for (cl = cf_addr_list; cl; cl = cl->next) {
		/* duplication check */
		if (dhcp6_find_listval(list0, DHCP6_LISTVAL_ADDR6,
		    cl->ptr, 0)) {
			log_info(
			    "%s:%d duplicated %s server: %s",
			    configfilename, cl->line,
			    optname,
			    in6addr2str((struct in6_addr *)cl->ptr, 0));
			return -1;
		}
		if (dhcp6_add_listval(list0, DHCP6_LISTVAL_ADDR6,
		    cl->ptr, NULL) == NULL) {
			log_error("failed to add a %s server",
			    optname);
			return -1;
		}
	}

	return 0;
}

static int
configure_domain(struct cf_list *cf_name_list, struct dhcp6_list *list0, char *optname)
{
	struct cf_list *cl;

	/* check against configuration restriction */
	if (cf_name_list != NULL && dhcp6_mode != DHCP6_MODE_SERVER) {
		log_info("%s:%d server-only configuration",
		    configfilename, cf_name_list->line);
		return -1;
	}

	TAILQ_INIT(list0);
	for (cl = cf_name_list; cl; cl = cl->next) {
		char *name, *cp;
		struct dhcp6_vbuf name_vbuf;

		name = strdup(cl->ptr + 1);
		if (name == NULL) {
			log_error("failed to copy a %s domain name",
			    optname);
			return -1;
		}
		cp = name + strlen(name) - 1;
		*cp = '\0';	/* clear the terminating quote */

		name_vbuf.dv_buf = name;
		name_vbuf.dv_len = strlen(name) + 1;

		/* duplication check */
		if (dhcp6_find_listval(list0, DHCP6_LISTVAL_VBUF,
		    &name_vbuf, 0)) {
			log_info(
			    "%s:%d duplicated %s name: %s",
			    configfilename, cl->line, optname,
			    name_vbuf.dv_buf);
			dhcp6_vbuf_free(&name_vbuf);
			return -1;
		}

		/* add the name */
		if (dhcp6_add_listval(list0, DHCP6_LISTVAL_VBUF,
		    &name_vbuf, NULL) == NULL) {
			log_error("failed to add a %s name",
			    optname);
			dhcp6_vbuf_free(&name_vbuf);
			return -1;
		}
		dhcp6_vbuf_free(&name_vbuf);
	}

	return 0;
}

static int
configure_duid(char *str /* this is a valid DUID string */, struct duid *duid)
{
	char *cp, *bp;
	char *idbuf = NULL;
	int duidlen, slen;
	unsigned int x;

	/* calculate DUID len */
	slen = strlen(str);
	if (slen < 2)
		goto bad;
	duidlen = 1;
	slen -= 2;
	if ((slen % 3) != 0)
		goto bad;
	duidlen += (slen / 3);
	if (duidlen > 128) {
		log_error("too long DUID (%d)", duidlen);
		return (-1);
	}

	if ((idbuf = malloc(duidlen)) == NULL) {
		log_error("memory allocation failed");
		return (-1);
	}

	for (cp = str, bp = idbuf; *cp;) {
		if (*cp == ':') {
			cp++;
			continue;
		}

		if (sscanf(cp, "%02x", &x) != 1)
			goto bad;
		*bp = x;
		cp += 2;
		bp++;
	}

	duid->duid_len = duidlen;
	duid->duid_id = idbuf;

	return (0);

  bad:
	if (idbuf)
		free(idbuf);
	log_error("assumption failure (bad string)");
	return (-1);
}

/* we currently only construct EUI-64 based interface ID */
static int
get_default_ifid(struct prefix_ifconf *pif)
{
	struct ifaddrs *ifa, *ifap;
#ifdef __KAME__
	struct sockaddr_dl *sdl;
#endif
#ifdef __linux__
	struct sockaddr_ll *sll;
#endif

	if (pif->ifid_len < 64) {
		log_notice("ID length too short");
		return (-1);
	}

	if (getifaddrs(&ifap) < 0) {
		log_error("getifaddrs failed: %s",
		    strerror(errno));
		return (-1);
	}

	for (ifa = ifap; ifa; ifa = ifa->ifa_next) {
		char *cp;

		if (strcmp(ifa->ifa_name, pif->ifname) != 0)
			continue;

		if (ifa->ifa_addr == NULL)
			continue;

#ifdef __KAME__
		if (ifa->ifa_addr->sa_family != AF_LINK)
			continue;

		sdl = (struct sockaddr_dl *)ifa->ifa_addr;
		if (sdl->sdl_alen < 6) {
			log_notice(
			    "link layer address is too short (%s)",
			    pif->ifname);
			goto fail;
		}

		memset(pif->ifid, 0, sizeof(pif->ifid));
		cp = (char *)(sdl->sdl_data + sdl->sdl_nlen);
#endif
#ifdef __linux__
		if (ifa->ifa_addr->sa_family != AF_PACKET)
			continue;

		sll = (struct sockaddr_ll *)ifa->ifa_addr;
		if (sll->sll_halen < 6) {
			log_notice(
			    "link layer address is too short (%s)",
			    pif->ifname);
			goto fail;
		}

		memset(pif->ifid, 0, sizeof(pif->ifid));
		cp = (char *)(sll->sll_addr);
#endif
		pif->ifid[8] = cp[0];
		pif->ifid[8] ^= 0x02; /* reverse the u/l bit*/
		pif->ifid[9] = cp[1];
		pif->ifid[10] = cp[2];
		pif->ifid[11] = 0xff;
		pif->ifid[12] = 0xfe;
		pif->ifid[13] = cp[3];
		pif->ifid[14] = cp[4];
		pif->ifid[15] = cp[5];

		break;
	}

	if (ifa == NULL) {
		log_info(
		    "cannot find interface information for %s", pif->ifname);
		goto fail;
	}

	freeifaddrs(ifap);
	return (0);

  fail:
	freeifaddrs(ifap);
	return (-1);
}

void
configure_cleanup()
{
	clear_iaconf(&ia_conflist0);
	clear_ifconf(dhcp6_ifconflist);
	dhcp6_ifconflist = NULL;
	clear_hostconf(host_conflist0);
	host_conflist0 = NULL;
	clear_keys(key_list0);
	key_list0 = NULL;
	clear_authinfo(auth_list0);
	auth_list0 = NULL;

	dhcp6_clear_list(&siplist0);
	TAILQ_INIT(&siplist0);
	dhcp6_clear_list(&sipnamelist0);
	TAILQ_INIT(&sipnamelist0);
	dhcp6_clear_list(&dnslist0);
	TAILQ_INIT(&dnslist0);
	dhcp6_clear_list(&dnsnamelist0);
	TAILQ_INIT(&dnsnamelist0);
	dhcp6_clear_list(&ntplist0);
	TAILQ_INIT(&ntplist0);
	optrefreshtime0 = -1;
	clear_poolconf(pool_conflist0);
}

void
configure_commit()
{
	struct dhcp6_ifconf *ifc;
	struct dhcp6_if *ifp;
	struct ia_conf *iac;

	/* commit interface configuration */
	for (ifp = dhcp6_if; ifp; ifp = ifp->next) {
		/* re-initialization */
		ifp->send_flags = 0;
		ifp->allow_flags = 0;
		dhcp6_clear_list(&ifp->reqopt_list);
		clear_iaconf(&ifp->iaconf_list);
		ifp->server_pref = DH6OPT_PREF_UNDEF;
		if (ifp->scriptpath != NULL)
			free(ifp->scriptpath);
		ifp->scriptpath = NULL;
		ifp->authproto = DHCP6_AUTHPROTO_UNDEF;
		ifp->authalgorithm = DHCP6_AUTHALG_UNDEF; 
		ifp->authrdm = DHCP6_AUTHRDM_UNDEF;

		for (ifc = dhcp6_ifconflist; ifc; ifc = ifc->next) {
			if (strcmp(ifp->ifname, ifc->ifname) == 0)
				break;
		}
		if (ifc == NULL)
			continue;

		/* copy new configuration */
		ifp->send_flags = ifc->send_flags;
		ifp->allow_flags = ifc->allow_flags;
		dhcp6_copy_list(&ifp->reqopt_list, &ifc->reqopt_list);
		while ((iac = TAILQ_FIRST(&ifc->iaconf_list)) != NULL) {
			TAILQ_REMOVE(&ifc->iaconf_list, iac, link);
			TAILQ_INSERT_TAIL(&ifp->iaconf_list,
			    iac, link);
		}
		ifp->server_pref = ifc->server_pref;
		ifp->scriptpath = ifc->scriptpath;
		ifc->scriptpath = NULL;

		if (ifc->authinfo != NULL) {
			ifp->authproto = ifc->authinfo->protocol;
			ifp->authalgorithm = ifc->authinfo->algorithm;
			ifp->authrdm = ifc->authinfo->rdm;
		}
		ifp->pool = ifc->pool;
		ifc->pool.name = NULL;
	}

	clear_ifconf(dhcp6_ifconflist);
	dhcp6_ifconflist = NULL;

	/* clear unused IA configuration */
	if (!TAILQ_EMPTY(&ia_conflist0)) {
		log_info(
		    "some IA configuration defined but not used");
	}
	clear_iaconf(&ia_conflist0);

	/* commit per-host configuration */
	clear_hostconf(host_conflist);
	host_conflist = host_conflist0;
	host_conflist0 = NULL;

	/* commit secret key information */
	clear_keys(key_list);
	key_list = key_list0;
	key_list0 = NULL;

	/* commit authentication information */
	clear_authinfo(auth_list);
	auth_list = auth_list0;
	auth_list0 = NULL;

	/* commit SIP server addresses */
	dhcp6_clear_list(&siplist);
	dhcp6_move_list(&siplist, &siplist0);

	/* commit SIP domain names */
	dhcp6_clear_list(&sipnamelist);
	dhcp6_move_list(&sipnamelist, &sipnamelist0);

	/* commit DNS addresses */
	dhcp6_clear_list(&dnslist);
	dhcp6_move_list(&dnslist, &dnslist0);

	/* commit DNS names */
	dhcp6_clear_list(&dnsnamelist);
	dhcp6_move_list(&dnsnamelist, &dnsnamelist0);

	/* commit NTP addresses */
	dhcp6_clear_list(&ntplist);
	dhcp6_move_list(&ntplist, &ntplist0);

	/* commit NIS server addresses */
	dhcp6_clear_list(&nislist);
	dhcp6_move_list(&nislist, &nislist0);

	/* commit NIS domain names */
	dhcp6_clear_list(&nisnamelist);
	dhcp6_move_list(&nisnamelist, &nisnamelist0);

	/* commit NIS+ server addresses */
	dhcp6_clear_list(&nisplist);
	dhcp6_move_list(&nisplist, &nisplist0);

	/* commit NIS+ domain names */
	dhcp6_clear_list(&nispnamelist);
	dhcp6_move_list(&nispnamelist, &nispnamelist0);

	/* commit BCMCS server addresses */
	dhcp6_clear_list(&bcmcslist);
	dhcp6_move_list(&bcmcslist, &bcmcslist0);

	/* commit BCMCS domain names */
	dhcp6_clear_list(&bcmcsnamelist);
	dhcp6_move_list(&bcmcsnamelist, &bcmcsnamelist0);

	/* commit information refresh time */
	optrefreshtime = optrefreshtime0;
	/* commit pool configuration */
	clear_poolconf(pool_conflist);
	pool_conflist = pool_conflist0;
	pool_conflist0 = NULL;
}

static void
clear_ifconf(struct dhcp6_ifconf *iflist)
{
	struct dhcp6_ifconf *ifc, *ifc_next;

	for (ifc = iflist; ifc; ifc = ifc_next) {
		ifc_next = ifc->next;

		free(ifc->ifname);
		dhcp6_clear_list(&ifc->reqopt_list);

		clear_iaconf(&ifc->iaconf_list);

		if (ifc->scriptpath)
			free(ifc->scriptpath);

		if (ifc->pool.name)
			free(ifc->pool.name);
		free(ifc);
	}
}

static void
clear_pd_pif(struct iapd_conf *iapdc)
{
	struct prefix_ifconf *pif, *pif_next;

	for (pif = TAILQ_FIRST(&iapdc->iapd_pif_list); pif; pif = pif_next) {
		pif_next = TAILQ_NEXT(pif, link);

		free(pif->ifname);
		free(pif);
	}

	dhcp6_clear_list(&iapdc->iapd_prefix_list);
}

static void
clear_iaconf(struct ia_conflist *ialist)
{
	struct ia_conf *iac;

	while ((iac = TAILQ_FIRST(ialist)) != NULL) {
		TAILQ_REMOVE(ialist, iac, link);

		switch(iac->type) {
		case IATYPE_PD:
			if (!TAILQ_EMPTY(&iac->iadata)) {
				log_error("assumption failure");
				exit(1);
			}
			clear_pd_pif((struct iapd_conf *)iac);
			break;
		case IATYPE_NA:
			break;
		}
		free(iac);
	}
}

static void
clear_hostconf(struct host_conf *hlist)
{
	struct host_conf *host, *host_next;

	for (host = hlist; host; host = host_next) {
		host_next = host->next;

		free(host->name);
		dhcp6_clear_list(&host->prefix_list);
		dhcp6_clear_list(&host->addr_list);
		if (host->duid.duid_id)
			free(host->duid.duid_id);
		if (host->pool.name)
			free(host->pool.name);
		free(host);
	}
}

static void
clear_keys(struct keyinfo *klist)
{
	struct keyinfo *key, *key_next;

	for (key = klist; key; key = key_next) {
		key_next = key->next;

		free(key->name);
		free(key->realm);
		free(key->secret);
		free(key);
	}
}

static void
clear_authinfo(alist)
	struct authinfo *alist;
{
	struct authinfo *auth, *auth_next;

	for (auth = alist; auth; auth = auth_next) {
		auth_next = auth->next;
		free(auth);
	}
}

static int
add_options(int opcode, struct dhcp6_ifconf *ifc, struct cf_list *cfl0)
{
	struct cf_list *cfl;
	int opttype;
	struct authinfo *ainfo;
	struct ia_conf *iac;

	for (cfl = cfl0; cfl; cfl = cfl->next) {
		switch(cfl->type) {
		case DHCPOPT_RAPID_COMMIT:
			switch (opcode) {
			case DHCPOPTCODE_SEND:
				ifc->send_flags |= DHCIFF_RAPID_COMMIT;
				break;
			case DHCPOPTCODE_ALLOW:
				ifc->allow_flags |= DHCIFF_RAPID_COMMIT;
				break;
			default:
				log_error("invalid operation (%d) "
				    "for option type (%d)",
				    opcode, cfl->type);
				return (-1);
			}
			break;
		case DHCPOPT_AUTHINFO:
			if (opcode != DHCPOPTCODE_SEND) {
				log_error("invalid operation (%d) "
				    "for option type (%d)",
				    opcode, cfl->type);
				return (-1);
			}
			ainfo = find_authinfo(auth_list0, cfl->ptr);
			if (ainfo == NULL) {
				log_error("%s:%d "
				    "auth info (%s) is not defined",
				    configfilename, cfl->line,
				    (char *)cfl->ptr);
				return (-1);
			}
			if (ifc->authinfo != NULL) {
				log_error("%s:%d authinfo is doubly specified on %s",
				    configfilename, cfl->line, ifc->ifname);
				return (-1);
			}
			ifc->authinfo = ainfo; 
			break;
		case DHCPOPT_IA_PD:
			switch (opcode) {
			case DHCPOPTCODE_SEND:
				iac = find_iaconf(&ia_conflist0, IATYPE_PD,
				    (u_int32_t)cfl->num);
				if (iac == NULL) {
					log_error("%s:%d "
					    "IA_PD (%lu) is not defined",
					    configfilename, cfl->line,
					    (u_long)cfl->num);
					return (-1);
				}

				TAILQ_REMOVE(&ia_conflist0, iac, link);
				TAILQ_INSERT_TAIL(&ifc->iaconf_list,
				    iac, link);

				break;
			default:
				log_error("invalid operation (%d) "
				    "for option type (%d)", opcode, cfl->type);
				break;
			}
			break;
		case DHCPOPT_IA_NA:
			switch (opcode) {
			case DHCPOPTCODE_SEND:
				iac = find_iaconf(&ia_conflist0, IATYPE_NA,
				    (u_int32_t)cfl->num);
				if (iac == NULL) {
					log_error("%s:%d "
					    "IA_NA (%lu) is not defined",
					    configfilename, cfl->line,
					    (u_long)cfl->num);
					return (-1);
				}

				TAILQ_REMOVE(&ia_conflist0, iac, link);
				TAILQ_INSERT_TAIL(&ifc->iaconf_list,
				    iac, link);

				break;
			default:
				log_error("invalid operation (%d) "
				    "for option type (%d)", opcode, cfl->type);
				break;
			}
			break;
		case DHCPOPT_SIP:
		case DHCPOPT_SIPNAME:
		case DHCPOPT_DNS:
		case DHCPOPT_DNSNAME:
		case DHCPOPT_NTP:
		case DHCPOPT_NIS:
		case DHCPOPT_NISNAME:
		case DHCPOPT_NISP:
		case DHCPOPT_NISPNAME:
		case DHCPOPT_BCMCS:
		case DHCPOPT_BCMCSNAME:
		case DHCPOPT_REFRESHTIME:
			switch (cfl->type) {
			case DHCPOPT_SIP:
				opttype = DH6OPT_SIP_SERVER_A;
				break;
			case DHCPOPT_SIPNAME:
				opttype = DH6OPT_SIP_SERVER_D;
				break;
			case DHCPOPT_DNS:
				opttype = DH6OPT_DNS;
				break;
			case DHCPOPT_DNSNAME:
				opttype = DH6OPT_DNSNAME;
				break;
			case DHCPOPT_NTP:
				opttype = DH6OPT_NTP;
				break;
			case DHCPOPT_NIS:
				opttype = DH6OPT_NIS_SERVERS;
				break;
			case DHCPOPT_NISNAME:
				opttype = DH6OPT_NIS_DOMAIN_NAME;
				break;
			case DHCPOPT_NISP:
				opttype = DH6OPT_NISP_SERVERS;
				break;
			case DHCPOPT_NISPNAME:
				opttype = DH6OPT_NISP_DOMAIN_NAME;
				break;
			case DHCPOPT_BCMCS:
				opttype = DH6OPT_BCMCS_SERVER_A;
				break;
			case DHCPOPT_BCMCSNAME:
				opttype = DH6OPT_BCMCS_SERVER_D;
				break;
			case DHCPOPT_REFRESHTIME:
				opttype = DH6OPT_REFRESHTIME;
				break;
			}
			switch(opcode) {
			case DHCPOPTCODE_REQUEST:
				if (dhcp6_find_listval(&ifc->reqopt_list,
					DHCP6_LISTVAL_NUM, &opttype, 0)
				    != NULL) {
					log_info(
					    "duplicated requested option: %s",
					    dhcp6optstr(opttype));
					goto next; /* ignore it */
				}
				if (dhcp6_add_listval(&ifc->reqopt_list,
				    DHCP6_LISTVAL_NUM, &opttype, NULL)
				    == NULL) {
					log_error("failed to "
					    "configure an option");
					return (-1);
				}
				break;
			default:
				log_error("invalid operation (%d) "
				    "for option type (%d)", opcode, cfl->type);
				break;
			}
			break;
		default:
			log_error("%s:%d unsupported option type: %d",
			    configfilename, cfl->line, cfl->type);
			return (-1);
		}

	  next:
		;
	}

	return (0);
}

static int
add_prefix(struct dhcp6_list *head, char *name, int type,
	   struct dhcp6_prefix *prefix0)
{
	struct dhcp6_prefix oprefix;

	oprefix = *prefix0;

	/* additional validation of parameters */
	if (oprefix.plen < 0 || oprefix.plen > 128) {
		log_error("invalid prefix: %d", oprefix.plen);
		return (-1);
	}
	/* clear trailing bits */
	prefix6_mask(&oprefix.addr, oprefix.plen);
	if (!IN6_ARE_ADDR_EQUAL(&prefix0->addr, &oprefix.addr)) {
		log_warning("prefix %s/%d for %s "
		    "has a trailing garbage.  It should be %s/%d",
		    in6addr2str(&prefix0->addr, 0), prefix0->plen,
		    name, in6addr2str(&oprefix.addr, 0), oprefix.plen);
		/* ignore the error */
	}

	/* avoid invalid prefix addresses */
	if (IN6_IS_ADDR_MULTICAST(&oprefix.addr) ||
	    IN6_IS_ADDR_LINKLOCAL(&oprefix.addr) ||
	    IN6_IS_ADDR_SITELOCAL(&oprefix.addr)) {
		log_error("invalid prefix address: %s",
		    in6addr2str(&oprefix.addr, 0));
		return (-1);
	}

	/* prefix duplication check */
	if (dhcp6_find_listval(head, type, &oprefix, 0)) {
		if (type == DHCP6_LISTVAL_PREFIX6) {
			log_notice(
			    "duplicated prefix: %s/%d for %s",
			    in6addr2str(&oprefix.addr, 0), oprefix.plen, name);
		} else {
			log_notice(
			    "duplicated address: %s for %s",
			    in6addr2str(&oprefix.addr, 0), name);
		}
		return (-1);
	}

	/* validation about relationship of pltime and vltime */
	if (oprefix.vltime != DHCP6_DURATION_INFINITE &&
	    (oprefix.pltime == DHCP6_DURATION_INFINITE ||
	    oprefix.pltime > oprefix.vltime)) {
		if (type == DHCP6_LISTVAL_PREFIX6) {
			log_notice(
			    "%s/%d has larger preferred lifetime "
			    "than valid lifetime",
			    in6addr2str(&oprefix.addr, 0), oprefix.plen);
		} else {
			log_notice(
			    "%s has larger preferred lifetime "
			    "than valid lifetime",
			    in6addr2str(&oprefix.addr, 0));
		}
		return (-1);
	}

	/* insert the new prefix to the chain */
	if (dhcp6_add_listval(head, type, &oprefix, NULL) == NULL) {
		return (-1);
	}

	return (0);
}

struct ia_conf *
find_iaconf(struct ia_conflist *head, int type, u_int32_t iaid)
{
	struct ia_conf *iac;

	for (iac = TAILQ_FIRST(head); iac; iac = TAILQ_NEXT(iac, link)) {
		if (iac->type == type && iac->iaid == iaid)
			return (iac);
	}

	return (NULL);
}

struct host_conf *
find_hostconf(struct duid *duid)
{
	struct host_conf *host;

	if ((host = find_dynamic_hostconf(duid)) != NULL) {
		return (host);
	}

	for (host = host_conflist; host; host = host->next) {
		if (host->duid.duid_len == duid->duid_len &&
		    memcmp(host->duid.duid_id, duid->duid_id,
			   host->duid.duid_len) == 0) {
			return (host);
		}
	}

	return (NULL);
}

struct authinfo *
find_authinfo(struct authinfo *head, char *name)
{
	struct authinfo *ainfo;

	for (ainfo = head; ainfo; ainfo = ainfo->next) {
		if (strcmp(ainfo->name, name) == 0)
			return (ainfo);
	}

	return (NULL);
}

struct dhcp6_prefix *
find_prefix6(struct dhcp6_list *list, struct dhcp6_prefix *prefix)
{
	struct dhcp6_listval *v;

	for (v = TAILQ_FIRST(list); v; v = TAILQ_NEXT(v, link)) {
		if (v->val_prefix6.plen == prefix->plen &&
		    IN6_ARE_ADDR_EQUAL(&v->val_prefix6.addr, &prefix->addr)) {
			return (&v->val_prefix6);
		}
	}
	return (NULL);
}

struct keyinfo *
find_key(char *realm, size_t realmlen, u_int32_t id)
{
	struct keyinfo *key;

	for (key = key_list; key; key = key->next) {
		if (key->realmlen == realmlen &&
		    memcmp(key->realm, realm, realmlen) == 0 &&
		    key->keyid == id) {
			return (key);
		}
	}

	return (NULL);
}

char *
qstrdup(char *qstr)
{
	size_t len;
	char *dup;

	len = strlen(qstr);
	if (qstr[0] != '"' || len < 2 || qstr[len - 1] != '"')
		return (NULL);

	if ((dup = malloc(len)) == NULL)
		return (NULL);

	memcpy(dup, qstr + 1, len - 1);
	dup[len - 2] = '\0';

	return (dup);
}

int
configure_pool(struct cf_namelist *poollist)
{
	struct cf_namelist *plp;

	log_debug("called");

	if (poollist && dhcp6_mode != DHCP6_MODE_SERVER) {
		log_error("%s:%d "
			"pool statement is server-only",
			configfilename, poollist->line);
		goto bad;
	}

	for (plp = poollist; plp; plp = plp->next) {
		struct pool_conf *pool = NULL;
		struct dhcp6_range *range = NULL;
		struct cf_list *cfl;

		for (cfl = plp->params; cfl; cfl = cfl->next) {
			switch(cfl->type) {
			case DECL_RANGE:
				range = cfl->ptr;
				break;
			default:
				log_error("%s:%d "
					"invalid pool configuration",
					configfilename, cfl->line);
				goto bad;
			}
		}

		if (!range) {
			log_error("%s:%d "
				"pool '%s' has no range declaration",
				configfilename, plp->line,
				plp->name);
			goto bad;
		}
		if ((pool = create_pool(plp->name, range)) == NULL) {
			log_error("faled to create pool '%s'", plp->name);
			goto bad;
		}
		pool->next = pool_conflist0;
		pool_conflist0 = pool;
	}

	return (0);

  bad:
	/* there is currently nothing special to recover the error */
	return (-1);
}

static void
clear_poolconf(struct pool_conf *plist)
{
	struct pool_conf *pool, *pool_next;

	log_debug("called");

	for (pool = plist; pool; pool = pool_next) {
		pool_next = pool->next;
		free(pool->name);
		free(pool);
	}
}

struct host_conf *
create_dynamic_hostconf(struct duid *duid, struct dhcp6_poolspec *pool)
{
	struct dynamic_hostconf *dynconf = NULL;
	struct host_conf *host;
	char* strid = NULL;
	static int init = 1;

	if (init) {
		TAILQ_INIT(&dynamic_hostconf_head);
		dynamic_hostconf_count = 0;
		init = 0;
	}

	if (dynamic_hostconf_count >= DHCP6_DYNAMIC_HOSTCONF_MAX) {
		struct dynamic_hostconf_listhead *head = &dynamic_hostconf_head;

		log_debug("reached to the max count (count=%lu)",
			dynamic_hostconf_count);

		/* Find the last entry that doesn't need authentication */
		TAILQ_FOREACH_REVERSE(dynconf, head, dynamic_hostconf_listhead, link)
			if (dynconf->host->delayedkey == NULL)
				break;
		if (dynconf == NULL)
			dynconf = TAILQ_LAST(head, dynamic_hostconf_listhead);
		TAILQ_REMOVE(head, dynconf, link);
		dynamic_hostconf_count--;
		clear_hostconf(dynconf->host);
	} else {
		if ((dynconf = malloc(sizeof(*dynconf))) == NULL) {
			log_error("memory allocation failed");
			return (NULL);
		}
	}
	memset(dynconf, 0, sizeof(*dynconf));

	if ((host = malloc(sizeof(*host))) == NULL) {
		log_error("memory allocation failed");
		goto bad;
	}
	memset(host, 0, sizeof(*host));
	TAILQ_INIT(&host->prefix_list);
	TAILQ_INIT(&host->addr_list);

	if ((strid = duidstr(duid)) == NULL)
		strid = "???";
	if ((host->name = strdup(strid)) == NULL) {
		log_error("memory allocation failed");
		goto bad;
	}
	if (duidcpy(&host->duid, duid) != 0) {
		goto bad;
	}
	if (pool->name) {
		if ((host->pool.name = strdup(pool->name)) == NULL) {
			log_error("memory allocation failed");
			goto bad;
		}
	}
	host->pool.pltime = pool->pltime;
	host->pool.vltime = pool->vltime;

	dynconf->host = host;
	TAILQ_INSERT_HEAD(&dynamic_hostconf_head, dynconf, link);
	dynamic_hostconf_count++; 

	log_debug("created host_conf (name=%s)", host->name);

	return (host);

bad:
	if (host)
		clear_hostconf(host);	/* host->next must be NULL */
	if (dynconf)
		free(dynconf);

	return (NULL);
}

struct host_conf *
find_dynamic_hostconf(struct duid *duid)
{
	struct dynamic_hostconf *dynconf = NULL;

	TAILQ_FOREACH(dynconf, &dynamic_hostconf_head, link) {
		if (dynconf->host->duid.duid_len == duid->duid_len &&
			memcmp(dynconf->host->duid.duid_id, duid->duid_id,
			   duid->duid_len) == 0)
			break;
	}

	if (dynconf) {
		/* relocation */
		TAILQ_REMOVE(&dynamic_hostconf_head, dynconf, link);
		TAILQ_INSERT_HEAD(&dynamic_hostconf_head, dynconf, link);

		return (dynconf->host);
	}

	return (NULL);
}

struct pool_conf *
create_pool(char *name, struct dhcp6_range *range)
{
	struct pool_conf *pool = NULL;

	if (!name || !range) {
		return (NULL);
	}

	log_debug("name=%s, range=%s->%s", name,
		in6addr2str(&range->min, 0), in6addr2str(&range->max, 0));

	if (in6_addr_cmp(&range->min, &range->max) > 0) {
		log_error("invalid address range %s->%s",
			in6addr2str(&range->min, 0),
			in6addr2str(&range->max, 0));
		return (NULL);
	}

	if ((pool = malloc(sizeof(struct pool_conf))) == NULL) {
		log_error("memory allocation failed");
		return (NULL);
	}
	if ((pool->name = strdup(name)) == NULL) {
		log_error("memory allocation failed");
		free(pool);
		return (NULL);
	}
	pool->min = range->min;
	pool->max = range->max;

	return (pool);
}

struct pool_conf *
find_pool(const char *name)
{
	struct pool_conf *pool = NULL;
	
	if (!name)
		return (NULL);

	log_debug("name=%s", name);

	for (pool = pool_conflist; pool; pool = pool->next) {
		if (strcmp(name, pool->name) == 0) {
			log_debug("found (name=%s)", name);
			return (pool);
		}
	}

	log_debug("not found (name=%s)", name);

	return (NULL);
}

static int 
in6_addr_cmp(struct in6_addr *addr1, struct in6_addr *addr2)
{
	int i;

	for (i = 0; i < 16; i++) {
		if (addr1->s6_addr[i] != addr2->s6_addr[i]) {
        	if (addr1->s6_addr[i] > addr2->s6_addr[i])
				return (1);
			else
				return (-1);
		}
	}

	return (0);
}
