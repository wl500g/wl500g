/*
Copyright 2001, Broadcom Corporation
All Rights Reserved.

This is UNPUBLISHED PROPRIETARY SOURCE CODE of Broadcom Corporation;
the contents of this file may not be disclosed to third parties, copied or
duplicated in any form, in whole or in part, without the prior written
permission of Broadcom Corporation.
*/
/*
* This module creates an array of name, value pairs
* and supports updating the nvram space. 
*
* This module requires the following support routines
*
*      malloc, free, strcmp, strncmp, strcpy, strtol, strchr, printf and sprintf
*/

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <syslog.h>
#include <bcmnvram.h>
#include <sys/mman.h>


#define MAX_LINE_SIZE 512
#define MAX_FILE_NAME 64

#ifdef REMOVE_NVRAM
static const char *workingDir="/etc/linuxigd";
#endif


/*
* NOTE : The mutex must be initialized in the OS previous to the point at
*           which multiple entry points to the nvram code are enabled
*
*/
#define MAX_NVRAM_SIZE 4096
#define EPI_VERSION_STR "2.4.5"


/* Remove CR/LF/Space in the end of string 
* 
*/

char *strtrim(char *str)
{
	int i;

	i=strlen(str)-1;

	while (i>=0) {
		if (*(str+i)==13 || *(str+i)==10 || *(str+i)==' ') {
			*(str+i)=0x0; 	 
		} else if (*(str+i)=='"') {
			*(str+i)=0x0;
			break;
		} else break;
		i--;
	}

	return (str);
}

#ifdef REMOVE
char *strtrim_c(char *str)
{
	int i;

	return(str);

	i=strlen(str)-1;

	while (i>0) {   
		if (*(str+i)==';') {
			*(str+i)=0x0;
			break;
		}
		i--;
	}
	return (str);
}

int nvram_set_item(EnvRec *env, char *name, char *value)
{
	if (env->name) free(env->name);
	if (env->value) free(env->value);

	env->name = (char*)malloc(strlen(name)+1);
	if (!env->name) goto err;
	strcpy(env->name, name);
	env->value = (char*)malloc(strlen(value)+1);
	if (!env->value) goto err;
	strcpy(env->value, value);
	return (1);
err:
	if (env->name) free(env->name);
	if (env->value) free(env->value);
	return (0);
}

int nvram_add_item(EnvRec *env, char *name, char *value)
{
	EnvRec *orig;

	if (env->next!=NULL) orig = env->next;
	else orig = NULL;	
	env->next = (EnvRec*)malloc(sizeof(EnvRec));
	if (!env->next) return(0);
	env->next->name = NULL;
	env->next->value = NULL;
	if (nvram_set_item(env->next, name, value)==0) goto err;

	env->next->next = orig;

	return(1);

err:
	if (env->next) free(env->next);
	if (orig) env->next = orig;
	return(0);
}
#endif	// REMOVE

extern void findNVRAMName(const char *serviceId, const char *field, char *name);

/*
* Set the value of an NVRAM variable
* @param	name	name of variable to set
* @param	value	value of variable
* @return	0 on success and errno on failure
*/
int
nvram_set_x(const char *sid, const char *name, const char *value)
{
	return(nvram_set(name, value));
}


/*
* Get the value of an NVRAM variable
* @param	name	name of variable to get
* @return	value of variable or NULL if undefined
*/
char*
nvram_get_x(const char *sid, const char *name)
{
	return(nvram_safe_get(name));
}


/*
* Get the value of an NVRAM variable
* @param	name	name of variable to get
* @return	value of variable or NULL if undefined
*/
char*
nvram_get_f(const char *file, const char *field)
{
	return(nvram_safe_get(field));
}

/*
* Set the value of an NVRAM variable from file
* @param	name	name of variable to get
* @return	value of variable or NULL if undefined
*/
int
nvram_set_f(const char *file, const char *field, const char *value)
{
	return(nvram_set(field, value));
}

/*
* Get the value of an NVRAM variable list 
* @param	name	name of variable to get
*              index   index of the list, start from 1
* @return	value of variable or NULL if undefined
*/

char *nvram_get_list_x(const char *sid, const char *name, int index)
{
	char new_name[MAX_LINE_SIZE];

	sprintf(new_name, "%s%d", name, index);

	return(nvram_get_f(sid, new_name));
}

#ifdef REMOVE_NVRAM
/*
* Add the value into the end an NVRAM variable list 
* @param	name	name of variable to get
* @return	0 on success and errno on failure
*/
int nvram_add_list_x(const char *sid, const char *name, const char *value)
{
	FILE *fl, *flTemp;
	char buf[MAX_LINE_SIZE];
	char new_buf[MAX_LINE_SIZE];
	char filename[MAX_FILE_NAME];
	unsigned char *v, *sp;
	int found;

	sprintf(filename, "%s/%s", workingDir, sid);
	if ((fl=fopen(filename,"r+"))==NULL) return 1;
	if ((flTemp=fopen("/tmp/temp.cfg","w+"))==NULL) return 1;

	found = 0; 
	while (fgets(buf, MAX_LINE_SIZE, fl)) {
		v = strchr(buf, '=');
		if (v != NULL && ((sp = strchr(buf, ' ')) == NULL || (sp > v))) {
			/* change the "name=val" string to "set name val" */
			if (!strncmp(buf, name, strlen(name)) && buf[strlen(name)]=='=') {
				found++;
			} else if (found) {	
				if (value!=NULL) {
					sprintf(new_buf, "%s%d=\"%s\"\n", name, found-1, value);
					fputs(new_buf, flTemp);
				}
				found = 0;
			}
			fputs(buf,flTemp);
		}
	}

	fclose(fl);
	fclose(flTemp);

	if ((fl=fopen(filename,"w+"))==NULL) return 1;
	if ((flTemp=fopen("/tmp/temp.cfg","r+"))==NULL) return 1;

	while (fgets(buf, MAX_LINE_SIZE, flTemp)) {
		fputs(buf, fl);
	}
	fclose(fl);
	fclose(flTemp);

	return(0);
}
#endif

/*
* Add the value into the end an NVRAM variable list 
* @param	name	name of variable to get
* @return	0 on success and errno on failure
*/
int nvram_add_lists_x(const char *sid, const char *name, const char *value, int count)
{
#ifndef REMOVE_NVRAM
	char name1[32], name2[32];

	findNVRAMName(sid, name, name1);

	if (name[0]!='\0') {
		sprintf(name2, "%s%d", name1, count);
		nvram_set(name2, value);
	}
#else
	FILE *fl, *flTemp;
	char buf[MAX_LINE_SIZE];
	char new_buf[MAX_LINE_SIZE];
	char new_buf1[MAX_LINE_SIZE];
	char filename[MAX_FILE_NAME];
	unsigned char *v, *sp;
	int found;

	sprintf(filename, "%s/%s", workingDir, sid);
	if ((fl=fopen(filename,"r+"))==NULL) return 1;
	if ((flTemp=fopen("/tmp/temp.cfg","w+"))==NULL) return 1;

	if (count==0)
		sprintf(new_buf, "%s=", name);
	else 
		sprintf(new_buf, "%s%d=", name, count-1);

	sprintf(new_buf1, "%s%d=", name, count);

	found = 0;

	while (fgets(buf, MAX_LINE_SIZE, fl)) {
		v = strchr(buf, '=');
		if (v != NULL && ((sp = strchr(buf, ' ')) == NULL || (sp > v))) {
			/* change the "name=val" string to "set name val" */
			if (!strncmp(buf, new_buf, strlen(new_buf))) /* Find the previous record */
			{   
				if (!found) /* If not added yet */
				{	 
					fputs(buf, flTemp); /* Write this record */

					if (value!=NULL)    /* Add new record */
					{
						sprintf(new_buf, "%s%d=\"%s\"\n", name, count, value);
						fputs(new_buf, flTemp);
						found = 1;
						sprintf(new_buf, "%s%d=", name, count); /* Maintain the buf of new reocrd */
					}
				}
			} else if (strncmp(buf, new_buf1, strlen(new_buf1))!=0)
				fputs(buf,flTemp);
		}
	}

	fclose(fl);
	fclose(flTemp);

	if ((fl=fopen(filename,"w+"))==NULL) return 1;
	if ((flTemp=fopen("/tmp/temp.cfg","r+"))==NULL) return 1;

	while (fgets(buf, MAX_LINE_SIZE, flTemp)) {
		fputs(buf, fl);
	}
	fclose(fl);
	fclose(flTemp);
#endif
	return(0);
}


/*
* Delete the value from an NVRAM variable list 
* @param	name	name of variable list
*              index   index of variable list
* @return	0 on success and errno on failure
*/
int nvram_del_lists_x(const char *sid, const char *name, int *delMap)
{
#ifndef REMOVE_NVRAM

	char names[32], oname[32], nname[32], *oval, *nval;
	int oi, ni, di;

	findNVRAMName(sid, name, names);

	if (names[0]!='\0') {	
		oi=0;
		ni=0;
		di=0;
		while (1) {
			sprintf(oname, "%s%d", names, oi);
			sprintf(nname, "%s%d", names, ni);

			oval = nvram_get(oname);
			nval = nvram_get(nname);

			if (oval==NULL) break;

			printf("d: %d %d %d %d\n", oi, ni, di, delMap[di]);
			if (delMap[di]!=-1&&delMap[di]==oi) {
				oi++;
				di++;
			} else {
				nvram_set(nname, oval);
				ni++;
				oi++;
			}
		}
	}
#else
	FILE *fl, *flTemp;
	char buf[MAX_LINE_SIZE];
	char new_buf[MAX_LINE_SIZE];
	char filename[MAX_FILE_NAME];
	unsigned char *v, *sp;
	int found, count, i, del;

	sprintf(filename, "%s/%s", workingDir, sid);
	if ((fl=fopen(filename,"r+"))==NULL) return 1;
	if ((flTemp=fopen("/tmp/temp.cfg","w+"))==NULL) return 1;

	count = 0;

	del = 0;

	while (fgets(buf, MAX_LINE_SIZE, fl)) {
		v = strchr(buf, '=');
		if (v != NULL && ((sp = strchr(buf, ' ')) == NULL || (sp > v))) {
			/* change the "name=val" string to "set name val" */
			if (!strncmp(buf, name, strlen(name)) && buf[strlen(name)]!='=') {
				i = strlen(name);

				if (buf[i]>='0' && buf[i]<='9')
					found = buf[i] - '0';
				else
					continue;

				if (buf[i+1]>='0' && buf[i+1]<='9') {
					found = found*10 + buf[i+1] - '0';

					if (buf[i+2]>='0' && buf[i+2]<='9')
						found = found*10 + buf[i+2] - '0';
					else if (buf[i+2]!='=')
						continue;
				} else if (buf[i+1]!='=')
					continue;

				/*printf("Del : %d %d %d\n", del, delMap[del], found);	*/

				if ( delMap[del]!=-1 && delMap[del]==found) {
					del++;
					continue;
				}

				v++;

				if (*v=='"')
					v++;

				sprintf(new_buf, "%s%d=\"%s\"\n", name, count, v);
				fputs(new_buf, flTemp);

				count++;
			} else {	
				fputs(buf,flTemp);
			}
		}
	}

	fclose(fl);
	fclose(flTemp);

	if ((fl=fopen(filename,"w+"))==NULL) return 1;
	if ((flTemp=fopen("/tmp/temp.cfg","r+"))==NULL) return 1;

	while (fgets(buf, MAX_LINE_SIZE, flTemp)) {
		fputs(buf, fl);
	}
	fclose(fl);
	fclose(flTemp);
#endif

	return(0);
}
