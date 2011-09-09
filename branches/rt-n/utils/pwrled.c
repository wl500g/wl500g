/*
 * pwrled v.0.5.20101007
 * Copyright (c) 2007 Dmitriy I. Balakin <http://mam0n.ru>
 * Copyright (c) 2009 theMIROn <http://wl500g.googlecode.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <signal.h>
#include <time.h>
#include <unistd.h>
#include <sys/types.h>
#include <fcntl.h>

#include <bcmnvram.h>

#define PWRLED_VERSION_STRING "0.5.20101007"

#ifdef HAVE_GETMODEL
#include <shutils.h>
#endif

#define GPIO0 0x0001
#define GPIO1 0x0002
#define GPIO2 0x0004
#define GPIO3 0x0008
#define GPIO4 0x0010
#define GPIO5 0x0020
#define GPIO6 0x0040
#define GPIO7 0x0080
#define GPIO15 0x8000

#ifdef LINUX26
#define GPIOCTL
#else
#undef GPIOCTL
#endif

#ifdef GPIOCTL
#include <sys/ioctl.h>
#include <linux_gpio.h>
#define GPIO_DEV_OUT	GPIO_IOC_OUT
#else
#define GPIO_DEV_OUT	"/dev/gpio/out"
#endif

static inline int startswith(char *source, char *cmp) { return !strncmp(source,cmp,strlen(cmp)); }

struct preset_t {
	char *name;	// preset name
	int cycles;	// cycles
	int ondur;	// pwr_on duration
	int offdur;	// pwr_off duration
	int sleep;	// sleep between durations
	int negative;	// inverse on/off
	int count;	// cycles count
	int def_off;	// default pwr value is off
};

struct platform_t {
	char *name;
	int pwr_mask;
	int pwr_value;
};

static struct preset_t presets[] = {
	{"on",		 0,  0,  0, 0, 0, -1, 0},
	{"off",		 0,  0,  0, 0, 0, -1, 1},
	{"alarm0",	24,  2,  2, 2, 0,  0, 0},
	{"alarm1",	 1, 30, 30, 2, 0,  0, 0},
	{"alarm2",	 2, 23, 23, 2, 0,  0, 0},
	{"alarm3",	 3, 19, 19, 2, 0,  0, 0},
	{"alarm4",	 4, 14, 14, 2, 0,  0, 0},
	{"alarm5",	 1,  1, 30, 2, 1,  0, 0},
	{"alarm6",	 2,  1, 30, 2, 1,  0, 0},
	{"alarm7",	 3,  1, 30, 2, 1,  0, 0},
	{"alarm8",	 4,  1, 30, 2, 1,  0, 0},
	{"alarm9",	24,  2,  2, 2, 1,  0, 0},
	{NULL, 0, 0, 0, 0, 0, -1, 0},
};
static struct preset_t defopt =
	{NULL, -1, -1, -1, -1, -1, -2, -1};

#ifndef HAVE_GETMODEL
enum {
	MDL_UNKNOWN,
	MDL_MN700,
	MDL_WL700G,
	MDL_WL500,
	MDL_WL500GX,
	MDL_WL550GE,
	MDL_WL320GE,
	MDL_WL330GE,
	MDL_WL500GP,
	MDL_WL500GPV2,
	MDL_WL500W,
	MDL_WL520GU,
	MDL_WL520GC,
	MDL_DIR320,
	MDL_RTN16,
	MDL_RTN12,
	MDL_RTN10,
	MDL_RTN10U,
	MDL_WNR3500L,
};
#endif

struct platform_t platforms[] = {
	/* Asus */
	[MDL_WL320GE]	= {"ASUS WL-320gE/WL-320gP",	GPIO2, 0},
	[MDL_WL330GE]	= {"ASUS WL-330gE",		GPIO0, 0},
#ifndef HAVE_GETMODEL
	[MDL_WL500]	= {"ASUS WL-500g",		GPIO0, 0},
#endif
	[MDL_WL500GX]	= {"ASUS WL-500g Deluxe",	GPIO0, 0},
	[MDL_WL500GP]	= {"ASUS WL-500g Premium",  	GPIO1, 0},
	[MDL_WL500GPV2]	= {"ASUS WL-500g PremiumV2",	GPIO0, 0},
	[MDL_WL500W]	= {"ASUS WL-500W",		GPIO5, 0},
	[MDL_WL520GU]	= {"ASUS WL-520gU",		GPIO0, 0},
#ifndef HAVE_GETMODEL
	[MDL_WL520GC]	= {"ASUS WL-520GC",		GPIO0, 0},
#endif
	[MDL_WL550GE]	= {"ASUS WL-550gE",		GPIO2, 0},
	[MDL_WL700G]	= {"ASUS WL-700g",		GPIO1, GPIO1},
	[MDL_RTN16]	= {"ASUS RT-N16",		GPIO1, 0},
	[MDL_RTN12]	= {"ASUS RT-N12",		GPIO2, 0},
	[MDL_RTN10]	= {"ASUS RT-N10",		GPIO1, 0},
	[MDL_RTN10U]	= {"ASUS RT-N10U",		GPIO1, 0},
	/* D-Link */
	[MDL_DIR320]	= {"D-Link DIR-320",		GPIO0, GPIO0},
	/* Microsoft */
	[MDL_MN700]	= {"Microsoft MN-700",		GPIO6, GPIO6},
	/* Netgear */
	[MDL_WNR3500L]	= {"Netgear WNR3500L",		GPIO3, GPIO3},
	{NULL, 0, 0},
};

static char *rname;
static int pwr_mask  = -1;
static int pwr_value = -1;
static int running   = 1;

#ifdef GPIOCTL
static int tgpio_ioctl(int fd, int gpioreg, unsigned int mask , unsigned int value)
{
	struct gpio_ioctl gpio;

	gpio.val = value;
	gpio.mask = mask;

	if (ioctl(fd, gpioreg, &gpio) < 0) {
		printf ("Invalid gpioreg %d\n", gpioreg);
		return -1;
	}
	return gpio.val;
}

static void gpio_out(unsigned int mask, unsigned int value)
{
	int fd = open("/dev/gpio", O_RDWR);
	if (fd != -1)
	{
		tgpio_ioctl(fd, GPIO_IOC_RESERVE, mask, mask);
		tgpio_ioctl(fd, GPIO_IOC_OUT, mask, value & mask);
		close(fd);
	}
}
#else
static void gpio_out(unsigned int mask, unsigned int value)
{
	unsigned int val = 0;
	int fd = open("/dev/gpio/out", , O_RDWR);
	if (fd != -1) {
		if (read(fd, &val, sizeof(val)) == sizeof(val)) {
			val = (val & ~mask) | (value & mask);
			write(fd, &val, sizeof(val));
		}
		close(fd);
	}
}
#endif

#ifndef HAVE_GETMODEL
static int get_model(void)
{
	char *boardnum, *boardtype, *buf;

	if (nvram_match("boardnum", "mn700"))
		return MDL_MN700;		/* MN700 */

	if ((buf = nvram_get("model_no"))) {
	    if (startswith(buf, "WL700"))		/* WL700G* */
		return MDL_WL700G;
	}

	if ((buf = nvram_get("hardware_version"))) {
            if (startswith(buf,"WL500GPV2-"))		/* WL500GPV2-* */
                return MDL_WL500GPV2;
            if (startswith(buf,"WL520GC-"))		/* WL520GU-* */
                return MDL_WL520GC;
            if (startswith(buf,"WL520GU-"))		/* WL520GU-* */
                return MDL_WL520GU;
            if (startswith(buf,"WL330GE-"))		/* WL330GE-* */
                return MDL_WL330GE;
        }

	boardnum = nvram_get("boardnum");
        boardtype = nvram_get("boardtype");

	if (startswith(nvram_get("pmon_ver"), "CFE")) {
		if (!strcmp(boardnum, "45")) {
			if (!strcmp(boardtype,"0x04cf"))
				return MDL_RTN16;
			if (!strcmp(boardtype,"0x04CD"))
				return MDL_RTN12;
			if (!strcmp(boardtype,"0x04EC"))
				return MDL_RTN10;
			if (!strcmp(boardtype,"0x0550"))
				return MDL_RTN10U;
			if (!strcmp(boardtype,"0x042f"))
				return MDL_WL500GP;
			if (!strcmp(boardtype,"0x0472"))
				return MDL_WL500W;
			if (!strcmp(boardtype,"0x048e"))
				return MDL_WL500GPV2;
			if (!strcmp(boardtype,"0x467"))
				return MDL_WL550GE;
			if (!strcmp(boardtype,"0x467"))
				return MDL_WL320GE;
			if (!strcmp(boardtype,"0x48E"))
				return MDL_WL520GU;
			if (!strcmp(boardtype,"bcm95365r"))
        	    		return MDL_WL500GX;
		} else
		if (!strcmp(boardnum, "44")) {
			if (!strcmp(boardtype,"0x042f"))
				return MDL_WL700G;
		} else
		if (!strcmp(boardnum, "3500")) {
			if (!strcmp(boardtype,"0x04CF"))
				return MDL_WNR3500L;
		}
		if (!strcmp(boardtype,"0x048e"))
			return MDL_DIR320;
	} else {
		if (startswith(nvram_get("hardware_version"), "WL500-"))
			return MDL_WL500;
	}
	return MDL_UNKNOWN;
}
#endif

static struct platform_t *platform_detect()
{
	int model = get_model();

	if (model != MDL_UNKNOWN && platforms[model].name)
		return &platforms[model];
	return NULL;
}

static int init()
{
	struct platform_t *platform;
	if ((platform = platform_detect())) {
		rname = platform->name;
		pwr_mask  = platform->pwr_mask;
		pwr_value = platform->pwr_value;
		return 1;
	}
	return 0;
}

static void pwr_on()
{
	gpio_out(pwr_mask, pwr_value);
}

static void pwr_off()
{
	gpio_out(pwr_mask, ~pwr_value);
}

static void play_alarm(struct preset_t *p)
{
	int i;
	int ondur_x = p->ondur;

	if (p->def_off && !p->negative && p->count >= 0) {
		pwr_on();
		sleep(p->sleep);
	} else
		ondur_x = 0;

	while (running && p->count >= 0) {
		if (p->count > 0) {
			if (p->count == 1) p->count = -1;
			p->count--;
		}
		if ((p->negative && running)) {
		    pwr_off();
		    sleep(p->sleep);
		};
		for (i = 0; i < p->cycles && running; i++) {
			pwr_on();
			if (running) usleep(ondur_x * 10000);
			ondur_x = p->ondur;
			pwr_off();
			if (running) usleep(p->offdur * 10000);
		}
		if (!p->negative && running) {
		    pwr_on();
		    sleep(p->sleep);
		};
	}
	if (p->def_off)
		pwr_off();
	else
		pwr_on();
}

static void shutdown(int sig)
{
	running = 0;
}


static struct preset_t *get_preset_by_name(char *name)
{
	int i;
	for (i = 0; ; i++) {
		if (!presets[i].name || !strcmp(presets[i].name, name ? name : ""))
			break;
	}
	return &presets[i];
}

static void print_preset(struct preset_t *p)
{
	printf("%s%s-c%d -n%d -s%d -d%d -e%d %s\n",
		p->negative ? "-n " : "",
		p->def_off ? "-f " : "",
		p->count,
		p->cycles,
		p->sleep,
		p->ondur,
		p->offdur,
		p->name ? p->name : "");
}

static int usage(char *name)
{
	fprintf(stderr,
	    "Usage: %s [-fghlv] [-b [-p pidile]] [-c count] [-e on_dur] "
	    "[-d off_dur] [-n cycles] [-s sleep] [preset]\n\n", name);
	return 1;
}

static int i_usage(char *name)
{
	int i;
	fprintf(stderr,
		"pwrled v" PWRLED_VERSION_STRING ", an utility for manipulate state of the power led\n"
		"Written by Mam(O)n; http://wl500g.info; theMIROn; http://wl500g.googlecode.com\n\n");
	usage(name);
	fprintf(stderr,
		"   -n cycle   Count of flash's before pause.\n"
		"   -e on_dur  Sleep time in the flash cycle when led is on.\n"
		"   -d off_dur Sleep time in the flash cycle when led is off.\n"
		"   -s sleep   Pause after full flash cycle is reached.\n"
		"   -g         \"Negative flash\" mode. E.g. pause(-s) used when led is off.\n"
		"   -c count   Count of full flash cycles with pauses(-s).\n"
		"              Use 0 for infinity, -1 for none.\n"
		"   -f         Turn off the led after done or kill.\n"
		"   -b         Fork to background (demonize).\n"
		"   -p file    Write PID to file. Use '-' to stdout output.\n"
		"              This option must be used with conjunction of -b.\n"
		"   -l         Print conjunction of preset & user options and exit.\n"
		"   -v         Print detected router model.\n"
		"   -h         Print this help and exit.\n\n");
	fprintf(stderr, "Supported routers: ");
	for (i = 0; platforms[i].name; i++) {
		fprintf(stderr, i == 0 ? "%s": ", %s", platforms[i].name);
	}
	fprintf(stderr,"\n\n");
	fprintf(stderr, "Avaliable presets: ");
	for (i = 0; presets[i].name; i++) {
		fprintf(stderr, i == 0 ? "%s": ", %s", presets[i].name);
	}
	fprintf(stderr,"\n\n");
	return 1;
}

static int h_usage(char *name) {
	usage(name);
	fprintf(stderr, "Type %s -h for more information\n\n", name);
	return 1;
}

int main(int argc, char *argv[])
{
	int opt_prprint = 0;
	int opt_bfork = 0;
	int opt_verbose = 0;

	char *opt_pidfile = NULL;
	int pid_stdout = 0;
	FILE *pidfile;

	struct preset_t opt = defopt;
	struct preset_t *pprst;

	int c;
	while ((c = getopt (argc, argv, "fghlbvc:n:e:d:s:p:")) != -1) {
		switch (c) {
			case 'c':	opt.count = atoi(optarg); break;
			case 'n':	opt.cycles = atoi(optarg); break;
			case 'd':	opt.offdur = atoi(optarg); break;
			case 'e':	opt.ondur = atoi(optarg); break;
			case 's':	opt.sleep = atoi(optarg); break;
			case 'l':	opt_prprint = 1; break;
			case 'b':	opt_bfork = 1; break;
			case 'v':	opt_verbose = 1; break;
			case 'p':	if (!strcmp(optarg, "-"))
						opt_pidfile = optarg;
					else
						pid_stdout = 1;
					break;
			case 'g':	opt.negative = 1; break;
			case 'f':	opt.def_off = 1; break;
			case 'h':	return i_usage(argv[0]);
			default :	return h_usage(argv[0]);
		}
	}

	if (optind < argc) {
		opt.name = argv[optind];
	}

	for (c = optind + 1; c < argc; c++) {
		if (c == optind + 1)
			fprintf(stderr, "ERROR: Garbage after preset name '%s': %s", opt.name, argv[c]);
		else
			fprintf(stderr, " %s", argv[c]);
	}

	if (optind + 1 < argc) {
		fprintf(stderr, "\n");
		return (1);
	}

	pprst = get_preset_by_name(opt.name);
	if (opt.name && !pprst->name) {
		fprintf(stderr, "ERROR: Preset '%s' not found\n", opt.name);
		return 1;
	}

	if (opt.count	> -2) pprst->count	= opt.count;
	if (opt.cycles	> -1) pprst->cycles	= opt.cycles;
	if (opt.offdur	> -1) pprst->offdur	= opt.offdur;
	if (opt.ondur	> -1) pprst->ondur	= opt.ondur;
	if (opt.sleep	> -1) pprst->sleep	= opt.sleep;
	if (opt.negative> -1) pprst->negative	= opt.negative;
	if (opt.def_off	> -1) pprst->def_off	= opt.def_off;

	if (!memcmp(&opt, &defopt, sizeof(struct preset_t))) {
		return h_usage(argv[0]);
	}

	if (opt_prprint) {
		print_preset(pprst);
		return 0;
	}

	if (!init()) {
		fprintf(stderr, "ERROR: No known hardware detected.\n");
		return 1;
	}
	if (opt_verbose)
		printf("Router detected: %s.\n", rname);

	if (opt_bfork) {
		pid_t pid;

		pid = fork();

		if (pid > 0) {
			if (opt_pidfile || pid_stdout) {
				if (pid_stdout)
					pidfile = stdout;
				else if (!(pidfile = fopen(opt_pidfile, "w"))) {
					fprintf(stderr, "WARNING: Can't open pid file '%s' for writing", opt_pidfile);
					return 2;
				}
				if (fprintf(pidfile, "%d", (int)pid) <= 0) {
					fprintf(stderr, "WARNING: Can't write to pid file '%s'", opt_pidfile);
					return 2;
				}
				if (!pid_stdout && fclose(pidfile) != 0) {
					fprintf(stderr, "WARNING: Can't close pid file '%s'", opt_pidfile);
					return 2;
				}
			}
			return 0;
		}
		else if (pid < 0) {
			fprintf(stderr, "ERROR: Unable fork to background\n");
			return 1;
		}
	}

	signal(SIGTERM, shutdown);
	signal(SIGINT, shutdown);
	signal(SIGHUP, shutdown);

	play_alarm(pprst);

	if (opt_bfork && opt_pidfile && !pid_stdout) {
		unlink(opt_pidfile);
	}
	return 0;
}
