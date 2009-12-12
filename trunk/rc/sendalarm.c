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
#include <bcmnvram.h>
#include <netconf.h>
#include <shutils.h>
#include <rc.h>
#include <syslog.h>

#define logs(s) syslog(LOG_NOTICE, s)

#define WEBCAM_SUPPORT 1
#define PRINTER_SUPPORT 1
#define MASSSTORAGE_SUPPORT 1

void filecat(FILE *fp, char *catted)
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

char *nslookup(char *name, char qtype, char *ret)
{
	#define MAX_LINE_SIZE 512
	FILE *fp;
	char buf[MAX_LINE_SIZE];

	fp=fopen("/etc/resolv.conf", "r+");
	
	if (fp!=NULL && 
		fgets(buf, MAX_LINE_SIZE, fp)!=NULL &&
		strncmp(buf,"nameserver", 10)==0)
	{
		dns_query(&buf[11], name, qtype, ret);
		fclose(fp);
	}
	return ret;
}

int
sendalarm_main(int argc, char *argv[])
{
	FILE *fp;
	char serverip[32], servername[32];
	char command[128];
	char image[64], *imagebase;
	char *boundry="alarmmailalarmmail";
	int i;

	if (nvram_invmatch("usb_websecurity_x", "1")) return -1;

	*servername = *serverip = 0x0;

	if (nvram_invmatch("usb_websendto_x", ""))
	{
		char *mname;

		mname = strstr(nvram_safe_get("usb_websendto_x"), "@");

		if (mname!=NULL)
			nslookup(mname+1, 'M', servername);

		if (*servername)
			nslookup(servername, 'A', serverip);
	}

	if (!*serverip && nvram_invmatch("usb_webmserver_x", ""))
	{
		nslookup(nvram_safe_get("web_webmserver_x"), 'A' , serverip);
	}

	if (!*serverip)
	{
		logs("send email alarm, but can not resolve ip of email server!");
		return -1;
	}
	else dprintf("send alarm to : %s\n", serverip);
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
			
			eval("uuencode", "-m", image, imagebase, "-f", "/var/tmp/uuencode");
			
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

	sprintf(command, "cat /var/tmp/alarmmail | mini_sendmail -s%s %s", serverip, nvram_safe_get("usb_websendto_x"));
	system(command);
	
	//eval("mini_sendmail", "-f", "/var/tmp/alarmmail", "-s", serverip, nvram_safe_get("usb_websendto"));
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
