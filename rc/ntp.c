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
 
#include <stdio.h>
#include <signal.h>
#include <time.h>
#include <sys/time.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>

#include <bcmnvram.h>
#include <shutils.h>
#include <rc.h>
#include <stdarg.h>

static void catch_sig(int sig)
{
	if (sig == SIGUSR1)
	{
		//printf("SIGUSR1 catched\n");
	}
}

int ntp_main(void)
{
	char servers[32]; 
	//char *ntpclient_argv[] = {"ntpclient", "-h", servers, "-i", "3", "-l", "-s", NULL};
	pid_t pid;
	FILE *fp;
	int ret;
	
	strcpy(servers, nvram_safe_get("ntp_servers"));

	if (!strlen(servers)) return 0;
	
	fp=fopen("/var/run/ntp.pid","w");
	if (fp==NULL) exit(0);
	fprintf(fp, "%d", getpid());
	fclose(fp);

	signal(SIGUSR1, catch_sig);

	while(1)
	{
		ret=eval("ntpclient", "-h", servers, "-i", "3", "-l", "-s");
		pause();
	}	
}
