/*
 * Copyright 2004, ASUSTek Inc.
 * All Rights Reserved.
 * 
 * THIS SOFTWARE IS OFFERED "AS IS", AND ASUS GRANTS NO WARRANTIES OF ANY
 * KIND, EXPRESS OR IMPLIED, BY STATUTE, COMMUNICATION OR OTHERWISE. BROADCOM
 * SPECIFICALLY DISCLAIMS ANY IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS
 * FOR A SPECIFIC PURPOSE OR NONINFRINGEMENT CONCERNING THIS SOFTWARE.
 *
 * $Id$
 */

#ifdef ASUS_EXT

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>
#include <errno.h>
#include <sys/fcntl.h>
#include <dirent.h>
#include <sys/mount.h>
#include <netdb.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <arpa/nameser.h>
#include <resolv.h>
#include <bcmnvram.h>
#include <netconf.h>
#include <shutils.h>
#include <rc.h>
#include <syslog.h>

#define logs(s) syslog(LOG_NOTICE, s)

static void filecat(FILE *fp, char *catted)
{
	FILE *cfp;
	char line[1024];
	size_t i;

	if ((cfp=fopen(catted, "r"))==NULL) return;

	while((i=fread(line, 1, sizeof(line), cfp)))
	{
		//printf("write: %s\n", line);
		fwrite(line, 1, i, fp);
	}
	fclose(cfp);
	return;
}

static char *nslookup(char *name, int qtype, char *ret, size_t retsize)
{
	unsigned char	reply[1024];	/* Reply buffer */
	char		host[MAXDNAME];
	int		replylen = 0;
	HEADER		*rheader;	/* Reply header */
	unsigned char	*rrptr;		/* Current Resource record ptr */
	int		exprc;          /* dn_expand return code */
	int		rrtype,	rrdlen;

	/* Initialize lookup system if needed (check global _res structure) */
	if (((_res.options & RES_INIT) == 0) && (res_init() == -1))
    		return NULL;

	if (retsize <= 0)
    		return NULL;
	*ret = '\0';

	replylen = res_query(name, C_IN, qtype, (unsigned char*)&reply, sizeof(reply));
	rheader = (HEADER *)reply;

	if ( (replylen <= 0) || (ntohs(rheader->rcode) != NOERROR)
	     || (ntohs(rheader->ancount) <= 0) )
		return NULL;

	/* Parse out the Question section, and get to the following.
	 RRs (see RFC 1035-4.1.2) */
	exprc = dn_expand(reply,                /* Msg ptr */
			reply + replylen,       /* End of msg ptr */
			reply + sizeof(HEADER), /* Offset into msg */
			host, sizeof(host));
	if (exprc < 0)
		return NULL;

	/* Determine offset of the first RR */
	rrptr = reply + sizeof(HEADER) + exprc + NS_QFIXEDSZ;

	/* Walk the RRs, building a list of targets */
	while (rrptr < (reply + replylen))
	{
		/* Expand the domain name */
		exprc = dn_expand(reply, reply + replylen, rrptr,
				host, sizeof(host));

		if (exprc < 0)
			return NULL;

		/* Jump to RR info */
		/*  NAME     (? bytes)
		    TYPE     (2 bytes)
		    CLASS    (2 bytes)
		    TTL      (4 bytes)
		    RDLENGTH (2 bytes)
		    RDATA    (? bytes)  */
		rrptr += exprc;
		rrtype = (rrptr[0] << 8 | rrptr[1]);  /* Extract RR type */
		rrdlen = (rrptr[8] << 8 | rrptr[9]);  /* Extract RR payload */
		rrptr += NS_RRFIXEDSZ;

		if (rrtype == qtype) // Our answer
		{
			/* Process the RR */
			switch (rrtype)
			{
#ifdef T_AAAA
			case T_AAAA:
				if (rrdlen < NS_IN6ADDRSZ)
					break;
				inet_ntop(AF_INET6, rrptr, ret, retsize);
				break;
#endif
			case T_A:
				if (rrdlen < NS_INADDRSZ)
					break;
				inet_ntop(AF_INET, rrptr, ret, retsize);
				break;
			case T_MX:
				if (rrdlen < NS_INT16SZ+1)
					break;
				exprc = dn_expand(reply, reply + replylen,
						 rrptr + NS_INT16SZ,
						 host, sizeof(host));
				if (exprc > 0)
					strncpy(ret, host, retsize);
				break;
			}
			if (*ret)	/* Answer found */
				return ret;
		}

		/* Increment to next RR */
		rrptr += rrdlen;
	}

	return NULL;
}

int
sendalarm_main(int argc, char *argv[])
{
	FILE *fp;
	char *serverip, servername[64];
	char command[128];
	char image[64], *imagebase;
	char *boundry="alarmmailalarmmail";
	int i;
	struct hostent *serverhost = NULL;

	if (nvram_invmatch("usb_websecurity_x", "1")) return -1;

	*servername = '\0';

	if (nvram_invmatch("usb_websendto_x", ""))
	{
		char *mname;

		mname = strstr(nvram_safe_get("usb_websendto_x"), "@");

		if (mname!=NULL)
			nslookup(mname+1, T_MX, servername, sizeof(servername));

		if (*servername)
			serverhost = gethostbyname(servername);
	}

	if (!serverhost && nvram_invmatch("usb_webmserver_x", ""))
	{
		serverhost = gethostbyname(nvram_safe_get("web_webmserver_x"));
	}

	if (!serverhost)
	{
		logs("send email alarm, but can not resolve ip of email server!");
		return -2;
	}
	else
	{
		serverip = inet_ntoa( *( struct in_addr* )(serverhost->h_addr) );
		dprintf("send alarm to : %s\n", serverip);
	}
	// Build mail source file

	if ((fp = fopen("/var/tmp/alarmmail", "w"))==NULL) return -1;

	fprintf(fp, "To: %s\n", nvram_safe_get("usb_websendto_x"));
	fprintf(fp, "Subject: %s\n", nvram_safe_get("usb_websubject_x"));
	fprintf(fp, "X-Mailer: Mailer\n");
	fprintf(fp, "Mime-Version: 1.0\n");
	fprintf(fp, "Content-Type: multipart/mixed; boundary=\"%s\"\n", boundry);
	fprintf(fp, "Content-Disposition: inline\n\n\n");
	fprintf(fp, "--%s\n", boundry);
	fprintf(fp, "Content-Type: text/plain; charset=us-ascii\n");
	fprintf(fp, "Content-Disposition: inline\n\n");
	fprintf(fp, "Image motion detected!!!\n\n");

	if (nvram_match("usb_webattach_x", "1"))
	{	
		for(i=1;i<argc;i++)
		{
			strcpy(image, argv[i]);
			
			if (!(imagebase=rindex(image, '/'))) imagebase = image;
			else imagebase++;
			
			//eval("uuencode", "-m", image, imagebase, "-f", "/var/tmp/uuencode");
			sprintf(command, "uuencode -m %s %s > /var/tmp/uuencode", image, imagebase);
			system(command);
			
			fprintf(fp, "--%s\n", boundry);
			fprintf(fp, "Content-Type: application/unknown\n");
			fprintf(fp, "Content-Disposition: attachment; filename=\"%s\"\n", imagebase);

			fprintf(fp, "Content-Transfer-Encoding: base64\n\n");
			filecat(fp, "/var/tmp/uuencode");
			fprintf(fp, "\n\n\n");
		}
		fprintf(fp, "--%s--\n\n", boundry);
	}
	fclose(fp);

	sprintf(command, "cat /var/tmp/alarmmail | sendmail -S%s %s", serverip, nvram_safe_get("usb_websendto_x"));
	system(command);
	
	// log
	logs("send mail alert");
	kill_pidfile_s("/var/run/watchdog.pid", SIGUSR2);

	// clean temp file
	// unlink("/var/tmp/uuencode");
	unlink("/var/tmp/alarmmail");
	
	dprintf("done\n");

	return 0;
}
	
#endif		
