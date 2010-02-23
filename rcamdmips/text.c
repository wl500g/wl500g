#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <errno.h>
#include <sys/wait.h>
#ifdef WL600
#include <time.h>
#else
#include <libxml/parser.h>
#include "config.h"
#endif

#define MODULE_FILTER
#ifndef WL600
#include "module.h"
#endif
#include "text.h"
#ifndef WL600
#include "xmlhelp.h"
#include "log.h"
#include "mod_handle.h"
#include "configfile.h"
#endif

#include "font_6x11.h"

//static int text_conf(struct text_ctx *, xmlNodePtr);
static int text_color(unsigned char *, unsigned char *);
static int text_hexdig(unsigned char);
//static void text_text(struct text_ctx *, xmlNodePtr, int);
static int text_format(struct text_ctx *);

static struct text_ctx text_gctx;

#define MODNAME "text"

char *name = MODNAME;
#ifndef WL600
char *version = VERSION;
#endif

#ifndef WL600
int
init(struct module_ctx *mctx)
{
	int ret;
	
	memset(text_gctx.color, 0xff, sizeof(text_gctx.color));
	ret = text_conf(&text_gctx, mctx->node);
	if (ret)
		return -1;
	return 0;
}
#endif

#ifdef WL600
add_time(unsigned char *img, int width, int height)
#else
int
filter(struct image *img, xmlNodePtr node)
#endif
{
	struct text_ctx ctx;
	int x, y;
	int subx, suby;
	int idx;
	unsigned char *imgptr;
	unsigned char *text;

	
#ifdef WL600
	time_t now;
	struct tm tm;
	char fbuf[256];

	time(&now);
	localtime_r(&now, &tm);
	strftime(fbuf, sizeof(fbuf)-1, "%b %d %Y %T", &tm);
	ctx.text = fbuf;
	ctx.type = 0;
	ctx.nosubst = 0;
	ctx.top = 0;
	ctx.right = 1;
	ctx.solid = 0;
	ctx.color[0] = ctx.color[1] = ctx.color[2] = 0xff;
	ctx.bgcolor[0] = ctx.bgcolor[1] = ctx.bgcolor[2] = 0x00;
#else
	memcpy(&ctx, &text_gctx, sizeof(ctx));
	if (text_gctx.text)
		ctx.text = strdup(text_gctx.text);
	idx = text_conf(&ctx, node);

	if (idx || !ctx.text)
	{
		free(ctx.text);
		return -1;
	}
	
	idx = text_format(&ctx);
	if (idx)
	{
		free(ctx.text);
		return -1;
	}
#endif
	
	text = ctx.text;

	if (height < 11)
	{
		//free(ctx.text);
		return 0;
	}

	if (ctx.right)
	{
		x = width - strlen(text) * 6 - 1;
		if (x < 0)
			x = 0;
	}
	else
		x = 0;
	
	if (ctx.top)
		y = 0;
	else
		y = height - 11;
	
	while (*text)
	{
		if (x + 6 > width)
			break;
		
		idx = *text * 11;
		for (suby = 0; suby < 11; suby++)
		{
			imgptr = img + ((y + suby) * width + x);
			for (subx = 0; subx < 6; subx++)
			{
				if (fontdata[idx] & (0x80 >> subx))
					memcpy(imgptr, ctx.color, 1);
				else
				{
					if (ctx.solid)
						memcpy(imgptr, ctx.bgcolor, 1);
				}
				imgptr += 1;
			}
			idx++;
		}
		text++;
		x += 6;
	}
	
	//free(ctx.text);
	return 0;
}

#ifndef WL600
static
int
text_conf(struct text_ctx *ctx, xmlNodePtr node)
{
	int ret;
	unsigned char *text;
	
	if (!node)
		return 0;
	
	for (node = node->xml_children; node; node = node->next)
	{
		if (xml_isnode(node, "text"))
			text_text(ctx, node, 0);
		else if (xml_isnode(node, "cmd"))
			text_text(ctx, node, 1);
		else if (xml_isnode(node, "file"))
			text_text(ctx, node, 2);
		else if (xml_isnode(node, "color"))
		{
			ret = text_color(ctx->color, xml_getcontent(node));
			if (ret)
			{
				log_log(MODNAME, "Invalid contents of <color> tag\n");
				return -1;
			}
		}
		else if (xml_isnode(node, "bgcolor") || xml_isnode(node, "background"))
		{
			text = xml_getcontent(node);
			if (!text)
			{
invbg:
				log_log(MODNAME, "Invalid contents of <bgcolor> tag\n");
				return -1;
			}
			if (!strcmp(text, "trans") || !strcmp(text, "transparent") || !strcmp(text, "none"))
				ctx->solid = 0;
			else
			{
				ctx->solid = 1;
				ret = text_color(ctx->bgcolor, text);
				if (ret)
					goto invbg;
			}
		}
		else if (xml_isnode(node, "pos") || xml_isnode(node, "position"))
		{
			text = xml_getcontent_def(node, "bl");
			while (*text)
			{
				if (*text == 'b')
					ctx->top = 0;
				else if (*text == 't')
					ctx->top = 1;
				else if (*text == 'r')
					ctx->right = 1;
				else if (*text == 'l')
					ctx->right = 0;
				text++;
			}
		}
	}
	return 0;
}
#endif

static
int
text_color(unsigned char *buf, unsigned char *text)
{
	int idx, reth, retl;
	
	if (!text)
		return -1;
	if (*text == '#')
		text++;
	if (strlen(text) < 6)
		return -1;
	for (idx = 0; idx < 3; idx++)
	{
		reth = text_hexdig(*text++);
		retl = text_hexdig(*text++);
		if (reth < 0 || retl < 0)
			return -1;
		buf[idx] = (reth << 4) | retl;
	}
	return 0;
}

static
int
text_hexdig(unsigned char c)
{
	if (c >= '0' && c <= '9')
		return c - '0';
	else if (c >= 'a' && c <= 'f')
		return c - 'a' + 10;
	else if (c >= 'A' && c <= 'F')
		return c - 'A' + 10;
	else
		return -1;
}

#ifndef WL600
static
void
text_text(struct text_ctx *ctx, xmlNodePtr node, int type)
{
	char *text;
	char *attr;
	
	text = xml_getcontent(node);
	if (!text)
		return;

	free(ctx->text);

	ctx->type = type;
	if (ctx->type == 0)
		ctx->text = strdup(text);
	else
		ctx->text = config_homedir(text);
	
	attr = xml_attrval(node, "nosubst");
	if (!attr || strcmp(attr, "yes"))
		ctx->nosubst = 0;
	else
		ctx->nosubst = 1;
}
#endif

static
int
text_format(struct text_ctx *ctx)
{
	unsigned char *text;
	int fds[2], ret, pid;
	unsigned char buf[1024], fbuf[1024];
	unsigned char *p;
	struct tm tm;
	time_t now;
	
	switch (ctx->type)
	{
	case 0:
		text = ctx->text;
		break;
#ifndef WL600		
	case 1:
		ret = pipe(fds);
		if (ret)
		{
			log_log(MODNAME, "pipe() failed: %s\n", strerror(errno));
			return -1;
		}
		pid = fork();
		if (pid < 0)
		{
			log_log(MODNAME, "fork() failed: %s\n", strerror(errno));
			close(fds[0]);
			close(fds[1]);
			return -1;
		}
		if (!pid)
		{
			/* child */
			close(STDIN_FILENO);
			close(fds[0]);
			dup2(fds[1], STDOUT_FILENO);
			close(fds[1]);
			/* stderr goes to log */
			for (ret = 3; ret < 1024; ret++)
				close(ret);

			execlp(ctx->text, ctx->text, NULL);
			
			/* notreached unless error */
			log_log(MODNAME, "exec(\"%s\") failed: %s\n", ctx->text, strerror(errno));
			_exit(0);
		}
		
		close(fds[1]);
		
		for (p = buf; p < buf + sizeof(buf) - 1; p++)
		{
			ret = read(fds[0], p, 1);
			if (ret < 0)
			{
				log_log(MODNAME, "read error from pipe: %s\n", strerror(errno));
				close(fds[0]);
				return -1;
			}
			if (ret == 0)
				break;
			if (*p == '\n')
				break;
		}
		close(fds[0]);

		*p = '\0';
		if (p > buf && *--p == '\r')
			*p = '\0';
		text = buf;
		
		waitpid(pid, NULL, 0);

		break;
		
	default: /* 2 */
		fds[0] = open(ctx->text, O_RDONLY);
		if (fds[0] < 0)
		{
			log_log(MODNAME, "open of %s failed: %s\n", ctx->text, strerror(errno));
			return -1;
		}
		ret = read(fds[0], buf, sizeof(buf) - 1);
		if (ret < 0)
		{
			log_log(MODNAME, "read from %s failed: %s\n", ctx->text, strerror(errno));
			close(fds[0]);
			return -1;
		}
		close(fds[0]);
		buf[ret] = '\0';
		p = strchr(buf, '\n');
		if (p)
			*p = '\0';
		text = buf;
		break;
#endif
	}

	if (!ctx->nosubst)
	{
		time(&now);
		localtime_r(&now, &tm);
		strftime(fbuf, sizeof(fbuf) - 1, text, &tm);
		text = fbuf;
	}
	if (text != ctx->text)
	{
		free(ctx->text);
		ctx->text = strdup(text);
	}
	
	return 0;
}

