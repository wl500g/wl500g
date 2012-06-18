/*
 * milli_httpd - pretty small HTTP server
 *
 * Copyright (C) 2001 Broadcom Corporation
 *
 * $Id: httpd.h,v 1.1.1.1 2003/11/19 11:14:40 Cheni_Shen Exp $
 */

#ifndef _httpd_h_
#define _httpd_h_

#if defined(DEBUG) && defined(DMALLOC)
#include <dmalloc.h>
#endif

/* Basic authorization userid and passwd limit */
#define AUTH_MAX 64

extern int is_auth(void);

/* Generic MIME type handler */
struct mime_handler {
	const char *pattern;
	const char *mime_type;
	const char *extra_header;
	void (*input)(const char *path, FILE *stream, int len, const char *boundary);
	void (*output)(char *path, FILE *stream);
	void (*auth)(char *userid, char *passwd, char *realm);
};
extern const struct mime_handler mime_handlers[];

/* CGI helper functions */
extern void init_cgi(char *query);
extern char * get_cgi(char *name);

/* Regular file handler */
extern void do_file(char *path, FILE *stream);

/*
** FLAGS of function save_text_to_file()
*/
#define STF_USE_MODE		0x0001L
#define STF_USE_UID 		0x0002L
#define STF_USE_GID 		0x0004L

#define STF_INHERIT_MODE 	0x0008L
#define STF_INHERIT_UID		0x0010L
#define STF_INHERIT_GID 	0x0020L
#define STF_INHERIT_PERM	(STF_INHERIT_MODE|STF_INHERIT_UID|STF_INHERIT_GID)

#define STF_CHOWN_PERM_ERROR	0x0040L	/* if it not set EPERM error on chown will be ignored */

#define STF_CHMOD		(STF_USE_MODE|STF_INHERIT_MODE)
#define STF_CHOWN		(STF_USE_UID|STF_INHERIT_UID|STF_USE_GID|STF_INHERIT_GID)
#define STF_CHGRP		(STF_USE_GID|STF_INHERIT_GID)

#define STF_LTRIM		0x0100L
#define STF_RTRIM		0x0200L
#define STF_TRIM		(STF_LTRIM|STF_RTRIM)
#define STF_SKIP_0LINE		0x1000L
#define STF_LINE_NUM		0x2000L
#define STF_USE_EOL		0x4000L

/*
** OPTIONS of function save_text_to_file()
*/
struct stf_opts {
    mode_t mode;
    uid_t  uid;
    gid_t  gid;
    char*  eol;
};
int save_text_to_file(char *text, char *file, int flags, struct stf_opts *opts);

/* GoAhead 2.1 compatibility */
typedef FILE * webs_t;
typedef char char_t;
#define T(s) (s)
#define websWrite(wp, fmt, args...) ({ int ret = fprintf(wp, fmt, ## args); fflush(wp); ret; })
#define websError(wp, code, msg, args...) fprintf(wp, msg, ## args)
#define websHeader(wp) fputs("<html lang=\"en\">", wp)
#define websFooter(wp) fputs("</html>", wp)
#define websDone(wp, code) fflush(wp)
#define websGetVar(wp, var, default) (get_cgi((var)) ? : (default))
#define websWriteData(wp, buf, nChars) ({ int ret = fwrite(buf, 1, nChars, wp); fflush(wp); ret; })

extern int ejArgs(int argc, char_t **argv, char_t *fmt, ...);

/* GoAhead 2.1 Embedded JavaScript compatibility */
extern void do_ej(char *path, FILE *stream);
struct ej_handler {
	const char *pattern;
	int (*output)(int eid, webs_t wp, int argc, char_t **argv);
};
extern const struct ej_handler ej_handlers[];

extern int ej_nat_table(int eid, webs_t wp, int argc, char_t **argv);
extern int ej_route_table(int eid, webs_t wp, int argc, char_t **argv);
extern int ej_wl_status(int eid, webs_t wp, int argc, char_t **argv);

extern int sys_renew(void);
extern int sys_release(void);

#define sys_restart()	kill(1, SIGHUP)
#define sys_reboot()	kill(1, SIGTERM)
#define sys_forcereboot()	kill(1, SIGABRT)

#ifdef USE_JSON
extern int js0n(unsigned char *js, unsigned int len, unsigned short *out);
extern void do_json_get(char *url, FILE *stream);
extern void do_json_set(const char *url, FILE *stream, int len, const char *boundary);
extern void do_ej_ex(char *path, FILE *stream);
extern int ej_nvram_get_json(int eid, webs_t wp, int argc, char_t **argv);
extern int ej_nvram_get_n_json(int eid, webs_t wp, int argc, char_t **argv);
#endif

#endif /* _httpd_h_ */
