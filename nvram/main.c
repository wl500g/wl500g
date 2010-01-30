/*
 * Frontend command-line utility for Linux NVRAM layer
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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <typedefs.h>
#include <bcmnvram.h>

static void
usage(void)
{
	fprintf(stderr, "usage: nvram [get name] [set name=value] [unset name] [show]\n");
	exit(0);
}

#ifdef ASUS
#define PROFILE_HEADER 	"HDR1"

// save nvram to file
int nvram_save(char *file, char *buf)
{
	FILE *fp;
	char *name;
	unsigned long count, filelen, i;

   	if ((fp=fopen(file, "w"))==NULL) return -1;

	count = 0;
	for (name=buf;*name;name+=strlen(name)+1)
	{	
		puts(name);
		count = count+strlen(name)+1;
	}
   	
   	filelen = count + (1024 - count%1024);	
   	fwrite(PROFILE_HEADER, 1, 4, fp);
   	fwrite(&filelen, 1, 4, fp);
   	fwrite(buf, 1, count, fp);
   	for(i=count;i<filelen;i++) fwrite(name, 1, 1, fp);   	
   	fclose(fp);
	return 0;
}
int issyspara(char *p)
{
	extern struct nvram_tuple router_defaults[];
	struct nvram_tuple *t, *u;

	for (t = router_defaults; t->name; t++)
	{
		if (strstr(p, t->name))
			break;
	}

	if (t->name) return 1;
	else return 0;
}

// restore nvram from file
int nvram_restore(char *file, char *buf)
{
   	FILE *fp;
   	char header[8], *p, *v;
  	unsigned long count, *filelen;

   	if ((fp=fopen(file, "r+"))==NULL) return -1;
    	   
   	count = fread(header, 1, 8, fp);
   	if (count>=8 && strncmp(header, PROFILE_HEADER, 4)==0)
   	{  
	    filelen = (unsigned long *)(header + 4);
   	    count = fread(buf, 1, *filelen, fp);
   	}   
   	fclose(fp);

   	p = buf;       	           
   
   	while(*p)
   	{       
		//printf("nv:%s\n", p);     	     	        	     	
       		v = strchr(p, '=');

		if(v!=NULL)
		{	
			*v++ = NULL;

			if (issyspara(p))
			{
				nvram_set(p, v);
			}

       			p = v + strlen(v) + 1;			
		}
		else 
		{
			nvram_unset(p);
			p = p + 1;
		}
   	}

	nvram_set("x_Setting", "1");
	return 0;
}
#endif

/* NVRAM utility */
int
main(int argc, char **argv)
{
	char *name, *value, buf[NVRAM_SPACE];
	int size;

	/* Skip program name */
	--argc;
	++argv;

	if (!*argv) 
		usage();

	/* Process the remaining arguments. */
	for (; *argv; argv++) {
		if (!strncmp(*argv, "get", 3)) {
			if (*++argv) {
				if ((value = nvram_get(*argv)))
					puts(value);
			}
		}
		else if (!strncmp(*argv, "set", 3)) {
			if (*++argv) {
				strncpy(value = buf, *argv, sizeof(buf));
				name = strsep(&value, "=");
				nvram_set(name, value);
			}
		}
		else if (!strncmp(*argv, "unset", 5)) {
			if (*++argv)
				nvram_unset(*argv);
		}
		else if (!strncmp(*argv, "commit", 5)) {
			nvram_commit();
		}
#ifdef ASUS
		else if (!strncmp(*argv, "save", 4)) 
		{
			if (*++argv) 
			{
				nvram_getall(buf, sizeof(buf));	
				nvram_save(*argv, buf);
			}
			
		}
		else if (!strncmp(*argv, "restore", 7)) 
		{
			if (*++argv) 
			{
				nvram_restore(*argv, buf);
			}
			
		}
#endif
		else if (!strncmp(*argv, "show", 4) ||
			   !strncmp(*argv, "getall", 6)) {
			nvram_getall(buf, sizeof(buf));
			for (name = buf; *name; name += strlen(name) + 1)
				puts(name);
			size = sizeof(struct nvram_header) + (int) name - (int) buf;
			fprintf(stderr, "size: %d bytes (%d left)\n", size, NVRAM_SPACE - size);
		}
		if (!*argv)
			break;
	}

	return 0;
}	
