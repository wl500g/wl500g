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
	char *pattern;
	char *mime_type;
	char *extra_header;
	void (*input)(char *path, FILE *stream, int len, char *boundary);
	void (*output)(char *path, FILE *stream);
	void (*auth)(char *userid, char *passwd, char *realm);
};
extern struct mime_handler mime_handlers[];

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
#define __TMPVAR(x) tmpvar ## x
#define _TMPVAR(x) __TMPVAR(x)
#define TMPVAR _TMPVAR(__LINE__)
#define websWrite(wp, fmt, args...) ({ int TMPVAR = fprintf(wp, fmt, ## args); fflush(wp); TMPVAR; })
#define websError(wp, code, msg, args...) fprintf(wp, msg, ## args)
#define websHeader(wp) fputs("<html lang=\"en\">", wp)
#define websFooter(wp) fputs("</html>", wp)
#define websDone(wp, code) fflush(wp)
#define websGetVar(wp, var, default) (get_cgi(var) ? : default)
#define websDefaultHandler(wp, urlPrefix, webDir, arg, url, path, query) ({ do_ej(path, wp); fflush(wp); 1; })
#define websWriteData(wp, buf, nChars) ({ int TMPVAR = fwrite(buf, 1, nChars, wp); fflush(wp); TMPVAR; })
#define websWriteDataNonBlock websWriteData

extern int ejArgs(int argc, char_t **argv, char_t *fmt, ...);

/* GoAhead 2.1 Embedded JavaScript compatibility */
extern void do_ej(char *path, FILE *stream);
struct ej_handler {
	char *pattern;
	int (*output)(int eid, webs_t wp, int argc, char_t **argv);
};
extern struct ej_handler ej_handlers[];

extern int ej_lan_leases(int eid, webs_t wp, int argc, char_t **argv);
extern int ej_nat_table(int eid, webs_t wp, int argc, char_t **argv);
extern int ej_route_table(int eid, webs_t wp, int argc, char_t **argv);
extern int ej_wl_status(int eid, webs_t wp, int argc, char_t **argv);
extern int ej_wl_status_nobr(int eid, webs_t wp, int argc, char_t **argv);

extern int sys_renew(void);
extern int sys_release(void);

#ifdef vxworks
#define fopen(path, mode)	tar_fopen((path), (mode))
#define fclose(fp)		tar_fclose((fp))
#undef getc
#define getc(fp)		tar_fgetc((fp))
extern FILE * tar_fopen(const char *path, const char *mode);
extern void tar_fclose(FILE *fp);
extern int tar_fgetc(FILE *fp);
#endif

#define sys_restart() kill(1, SIGHUP)
#define sys_reboot() kill(1, SIGTERM)

#endif /* _httpd_h_ */
