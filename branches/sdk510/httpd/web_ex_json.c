#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <errno.h>
#include <unistd.h>
#include <limits.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <httpd.h>

#include <fcntl.h>
#include <signal.h>
#include <time.h>
#include <sys/klog.h>
#include <sys/wait.h>
#include <dirent.h>
#include <wlutils.h>
#include <typedefs.h>
#include <shutils.h>
#include <bcmconfig.h>
#include <bcmutils.h>
#include <bcmnvram.h>

#include "bcmnvram_f.h"
#include "common.h"

#define MAX_LINE_SIZE 1024

int js0n(unsigned char *js, unsigned int len, unsigned short *out);

// Answer list that created by json commands
struct json_answer_list {
	char *json;
	void *prev;
	void *next;
};

// Struct for story request session data
struct json_data {
	struct json_answer_list *json_answer;	// Answer list pointer
	void *reserved;							// reserved
};
typedef struct json_data JData;

// Available json commands. List at the end of file.
struct json_command {
	char *pattern;
	void (*command)(char *args, JData *data);
};
struct json_command json_commands[];

// Forward declarations
static void json_data_init(JData *data);
static void json_answer_add_pair(char *name, char *val, JData *data);
static void json_answer_write(FILE *stream, int flag, JData *data);

// Check if file "*.new" exist then do_ej(*.new), else do_ej(*.asp)
void
do_ej_ex(char *path, FILE *stream)
{
	FILE *fp;
	char *cp, *new_name;

	new_name = strdup(path);
	if (new_name) {
		if (( cp = strstr(new_name, ".asp") )) {
			strcpy(cp,".new");
			if (( fp = fopen(new_name, "r") )) {
				fclose(fp);
				do_ej(new_name, stream);
				free(new_name);
				return;
			}
		}
		free(new_name);
	}
	do_ej(path, stream);
}

/*
* Get up to 60 nvram variables
*
* Example: 
* lan_ipaddr=192.168.1.1
* wan_proto=ppoe
* <% nvram_get_json("sid","wan_proto","lan_ipaddr"); %>
*	produces "wan_proto":"pppoe","lan_ipaddr":"192.168.1.1"
* <% nvram_get_json("sid","undefined"); %> produces "undefined":""
*/
int
ej_nvram_get_json(int eid, webs_t wp, int argc, char_t **argv)
{
	JData res;
	int arg;

	if (argc < 2) {
		websError(wp, 400, "Insufficient args\n");
		return -1;
	}
	
	json_data_init(&res);
	for (arg = 1; arg < argc; arg++) {
		json_answer_add_pair(argv[arg], nvram_safe_get_x(argv[0], argv[arg]), &res);
	}
	json_answer_write(wp, 0, &res);
	return 1;
}

/*
* Get up to 60 nvram variables from numbered name spase
*
* Example:
* wan_unit=0
* wan0_ipaddr=192.168.1.1
* wan0_proto=ppoe
* <% nvram_get_n_json("sid","wan","_unit","_proto","_ipaddr"); %>
*	produces "wan0_proto":"pppoe","wan0_ipaddr":"192.168.1.1"
* <% nvram_get_n_json("sid","wan","_unit","_undefined"); %> produces "wan0_undefined":""
*/
int
ej_nvram_get_n_json(int eid, webs_t wp, int argc, char_t **argv)
{
	JData res;
	int arg;
	char tmp[100];
	char *unit, *cp;
	int len;

	if (argc < 4) {
		websError(wp, 400, "Insufficient args\n");
		return -1;
	}

	strncpy(tmp, argv[1], sizeof(tmp));
	if (strlen(tmp) + strlen(argv[2]) > (sizeof(tmp) - 1)) {
		websError(wp, 400, "Variable name is too big\n");
		return -1;
	}
	strcat(tmp, argv[2]);
	unit = nvram_safe_get_x(argv[0], tmp);

	strncpy(tmp, argv[1], sizeof(tmp));
	if (strlen(tmp) + strlen(unit) > (sizeof(tmp) - 1)) {
		websError(wp, 400, "Variable name is too big\n");
		return -1;
	}
	strcat(tmp, unit);
	len = strlen(tmp);
	cp = &tmp[len];

	json_data_init(&res);
	for (arg = 3; arg < argc; arg++) {
		strncpy(cp, argv[arg], sizeof(tmp) - len);
		json_answer_add_pair(tmp, nvram_safe_get_x(argv[0], tmp), &res);
	}
	json_answer_write(wp, 0, &res);
	return 1;
}

// Initialize data:
// 	- answer list
static void
json_data_init(JData *data)
{
	data->json_answer = NULL;
}

// Add answer string to list
static void
json_answer_add(char *str, JData *data)
{
	struct json_answer_list *ans, *prev_ans;

	ans = (struct json_answer_list *) malloc(sizeof(struct json_answer_list));
	prev_ans = data->json_answer;
	if (ans) {
		ans->prev = prev_ans;
		ans->next = NULL;
		if (prev_ans)
			prev_ans->next = ans;
		ans->json = strdup(str);
		data->json_answer = ans;
	}
}

// Add answer name:value to list
static void
json_answer_add_pair(char *name, char *val, JData *data)
{
	struct json_answer_list *ans, *prev_ans;

	ans = (struct json_answer_list *) malloc(sizeof(struct json_answer_list));
	prev_ans = data->json_answer;
	if (ans) {
		ans->prev = prev_ans;
		ans->next = NULL;
		if (prev_ans)
			prev_ans->next = ans;
		ans->json = (char*) malloc(strlen(name) + strlen(val) + 10);
		if (ans->json)
			sprintf(ans->json, "\"%s\":\"%s\"", name, val);
		data->json_answer = ans;
	}
}

// Send answer to user
static void
json_answer_write(FILE *stream, int flag, JData *data)
{
	struct json_answer_list *ans;

#ifdef DEBUG
	char part_ans[220];
	part_ans[0] = '\0';
#endif

	if (flag)
		fputc ('{', stream);
	ans = data->json_answer;
	if (ans)
		while (ans->prev)
			ans = ans->prev;
	data->json_answer = ans;
	while (data->json_answer) {
		ans = data->json_answer;

#ifdef DEBUG
		if (strlen(part_ans) + strlen(ans->json) < sizeof(part_ans) - 4) {
			strcat(part_ans, ans->json);
			if (ans->next)
				strcat(part_ans, ",");
		} else
			strcat(part_ans, "...");
#endif

		fputs(ans->json, stream);
		data->json_answer = ans->next;
		if (data->json_answer)
			fputc(',', stream);
		free(ans->json);
		free(ans);
	}
#ifdef DEBUG
	if (flag)
		dprintf("ans-> {%s}", part_ans);
	else
		dprintf("ans-> %s", part_ans);
#endif
	if (flag)
		fputc('}', stream);
}

// Parse json and call function for each parameter of top level
static void
json_parse(char *json, void (*func)(char *name, char *val, JData *data), JData *data)
{
	size_t l, i, j, tmp_len;
	unsigned short *res;
	char name[0x800],val[0x800];

	l = strlen(json);
	res = malloc(l);
	memset(res, 0, l);

	while (*json == ' ' || *json == '\t' || *json == '\r' || *json == '\n')
		json++;
	js0n(json, l, res);
	if (*json == '{') { // list of pairs
		for (i = 0; res[i]; i += 2) {
			strncpy(name, json+res[i], ( tmp_len = MIN(res[i+1], sizeof(name) - 1) ));
			name[tmp_len] = '\0';
			if (res[(i += 2)]) {
				strncpy(val, json+res[i], ( tmp_len = MIN(res[i+1], sizeof(val)-1) ));
				val[tmp_len] = '\0';
				(*func)(name, val, data);
			}
		}
	} else if (*json == '[') { // list of values
		for (i = 0, j = 0; res[i]; i += 2, j++) {
			strncpy(val, json+res[i], ( tmp_len = MIN(res[i+1], sizeof(val) - 1) ));
			val[tmp_len] = '\0';
			sprintf(name, "%d", j);
			(*func)(name, val, data);
		}
	} else if (*json) {  // single value
		*name = '\0';
		(*func)(name, json, data);
	}

	free(res);
}

// Replace symbols starting with '\' to it's origin
static void
decode_symbols(char *str)
{
	char *str1;

	for (str1 = str; *str; str++, str1++) {
		if (*str == '\\') {
			switch (*++str) {
			case '\"':
			case '\\':
			case '/': *str1 = *str; break;
			case 'b': *str1 = '\b'; break;
			case 'f': *str1 = '\f'; break;
			case 'n': *str1 = '\n'; break;
			case 'r': *str1 = '\r'; break;
			case 't': *str1 = '\t'; break;
			}
		} else {
			*str1 = *str;
		}
	}
	*str1 = '\0';
}

// Replace spec symbols with '\' paar
static void
encode_symbols(char *str, char *res, size_t res_len)
{
	char *res_limit = res + res_len - 2; // guarantee for symbol extending and zero

	for (; *str && res < res_limit; str++, res++) {
		switch (*str) {
		case '\"': *res++ = '\\';  *res = '\"'; break;
		case '\\': *res++ = '\\';  *res = '\\'; break;
		case '/' : *res++ = '\\';  *res = '/'; break;
		case '\b': *res++ = '\\';  *res = 'b'; break;
		case '\f': *res++ = '\\';  *res = 'f'; break;
		case '\n': *res++ = '\\';  *res = 'n'; break;
		case '\r': *res++ = '\\';  *res = 'r'; break;
		case '\t': *res++ = '\\';  *res = 't'; break;
		default: *res = *str;
		}
	}
	*res = 0;
}

// Execute command and add answer to list
static void
do_json_command(char *name, char *val, JData *data)
{
	struct json_command *handler;

	for (handler = &json_commands[0]; handler->pattern; handler++) {
		if (strncmp(handler->pattern, name, strlen(handler->pattern)) == 0)
			handler->command(val, data);
	}
}

//!!! temporary variable for data pointer story--------------------------------
void *json_data_ptr = NULL;

// Write json answer to stream
void
do_json_get(char *url, FILE *stream)
{
	json_answer_write(stream, 1, (JData *)(json_data_ptr));
	if (json_data_ptr)
		free(json_data_ptr);
}

// Read query from stream in json format treate it and create answer string
// The query is a list of commands and treated by 'do_json_command'

void
do_json_set(char *url, FILE *stream, int len, char *boundary)
{
	unsigned char *buf;
	size_t l;

	buf = malloc(len + 1);
	l = fread(buf, 1, len, stream);

	buf[l] = 0;

#ifdef DEBUG
	dprintf("cmd-> %s", buf);
#endif

	json_data_ptr = malloc(sizeof(JData));
	json_data_init((JData *)(json_data_ptr));
	json_parse(buf, & do_json_command, (JData *)(json_data_ptr));

	free (buf);
}

// Set value of nvram variable
static void
do_nvram_set(char *name, char *val, JData *data)
{
	decode_symbols(val);
	nvram_set(name, val);
}

// Json commands
static void
json_nvram_set(char *params, JData *data)
{
	json_parse(params, do_nvram_set , data);
}
static void
json_nvram_commit(char *params, JData *data)
{
	nvram_commit();
}
static void
json_sys_reboot(char *params, JData *data)
{
	sys_reboot();
}

#ifdef DEBUG
// Dump file as strings array 
static void
do_dump_file(char *filename, JData *data)
{
	FILE *fp;
	char buf[MAX_LINE_SIZE];
	char buf_en[2 * MAX_LINE_SIZE + 10];
	int len;

	sprintf(buf_en, "\"dump\":[\"%s\"", filename);
	json_answer_add(buf_en, data);

	fp = fopen(filename, "r");
	if (fp != NULL){
		while (fgets(buf, MAX_LINE_SIZE, fp) != NULL){
			buf_en[0] = '"';
			encode_symbols(buf, buf_en + 1, sizeof(buf_en) - 3);
			len = strlen(buf_en);
			buf_en[len] = '"';
			buf_en[len + 1] = '\0';
			json_answer_add(buf_en, data);
		}
	}
	fclose(fp);

	strcpy(buf_en, "\"\"]");
	json_answer_add(buf_en, data);
}
static void
json_dump_file(char *params, JData *data)
{
	char filename[128];

	sprintf(filename, "/tmp/%s", params);
	do_dump_file(filename, data);
	
}

// Get value of nvram variable
static void
do_nvram_get(char *index, char *name, JData *data)
{
	char *var;
	char buf[0x800];

	decode_symbols(name);
	if (! strstr(name, "http_passwd") && ! strstr(name, "http_username"))
		var = nvram_get(name);
	else
		var = "****";

	if (var) {
		encode_symbols(var, buf, sizeof(buf));
		json_answer_add_pair(name, buf, data);
	}

}
static void
json_nvram_get(char *params, JData *data)
{
	json_parse(params, do_nvram_get, data);
}
#endif

// Json commands list
struct json_command json_commands[] = {
#ifdef DEBUG
	{"nvram_get",json_nvram_get},	// sample { "nvram_get" : ["var_name_1","var_name_2"] }
	{"dump_file",json_dump_file},	// sample { "dump_file" : "../.version" }
#endif
	{"nvram_set",json_nvram_set},	// sample { "nvram_set" : { "nvram_var_name_1" : "val_1" , "nvram_var_name_2" : "val_2" } }
	{"nvram_commit",json_nvram_commit},	// sample { "nvram_commit" : "" } 
	{"sys_reboot",json_sys_reboot},	// sample { "sys_reboot" : "" } 
	{ NULL, NULL }
};

