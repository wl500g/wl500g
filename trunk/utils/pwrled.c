/*
 * pwrled v.0.2.1
 * Copyright (С) 2007 Dmitriy I. Balakin <http://mam0n.ru>
 * 2007-11-28
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

#define PWRLED_VERSION_STRING "0.2.1"

typedef
struct 		{
				char *name; int cycles; int ondur; int offdur; int sleep; int negative; int count; int def_off;
																												} preset;
preset
presets[] = {
			{		"on",		0,			0,			0,			0,			0,			-1,			0		},
			{		"off",		0,			0,			0,			0,			0,			-1,			1		},
			{		"alarm0",	24,			2,			2,			2,			0,			0,			0		},
			{		"alarm1",	1,			30,			30,			2,			0,			0,			0		},
			{		"alarm2",	2,			23,			23,			2,			0,			0,			0		},
			{		"alarm3",	3,			19,			19,			2,			0,			0,			0		},
			{		"alarm4",	4,			14,			14,			2,			0,			0,			0		},
			{		"alarm5",	1,			1,			30,			2,			1,			0,			0		},
			{		"alarm6",	2,			1,			30,			2,			1,			0,			0		},
			{		"alarm7",	3,			1,			30,			2,			1,			0,			0		},
			{		"alarm8",	4,			1,			30,			2,			1,			0,			0		},
			{		"alarm9",	24,			2,			2,			2,			1,			0,			0		},
			{		NULL,		0,			0,			0,			0,			0,			-1,			0		}
};

static int pwr_mask = 0;
static int pwr_value = 0;
static int running = 1;

static void gpio_out(char *dev, int mask, int value)
{
	unsigned int val = 0;
	int fd = open(dev, O_RDWR);

	if (fd != -1) {
		if (read(fd, &val, sizeof(val)) == sizeof(val)) {
			val = (val & ~mask) | (value & mask);
			write(fd, &val, sizeof(val));
		}
		close(fd);
	}
}

static int init()
{
	if (nvram_match("boardnum", "mn700"))
		pwr_mask = 0x0040, pwr_value = 0x0040;
	else
	if (nvram_match("boardtype", "bcm95365r")	&& nvram_match("boardnum", "45"))	/* wl500gx */
		pwr_mask = 0x0001, pwr_value = 0;
	else
	if (nvram_match("boardtype", "0x467") 		&& nvram_match("boardnum", "45"))	/* wl550gE */
		pwr_mask = 0x0004, pwr_value = 0;
	else
	if (nvram_match("boardtype", "0x042f") 		&& nvram_match("boardnum", "45"))	/* wl500gP */
		pwr_mask = 0x0002, pwr_value = 0;
	else
	if (nvram_match("boardtype", "0x0472") 		&& nvram_match("boardnum", "45"))	/* wl500w  */
		pwr_mask = 0x0020, pwr_value = 0;
	else
	if (nvram_match("boardtype", "0x042f") 		&& nvram_match("boardnum", "44")) 	/* wl700g  */
		pwr_mask = 0x0002, pwr_value = 0;
	else
	if (nvram_match("boardtype", "0x48E") 		&& nvram_match("boardnum", "45")) 	/* wl520gC/wl520gU/wl500gPV2 */
		pwr_mask = 0x0001, pwr_value = 0;
	else
	if (nvram_match("boardtype", "0x048e") 		&& !nvram_match("boardnum", "45"))	/* DLINK DIR-320 */
		pwr_mask = 0x0002, pwr_value = 0x0002;
	else return 0;

	return 1;
}


static void pwr_on()
{
	gpio_out ("/dev/gpio/out", pwr_mask, pwr_value);
}

static void pwr_off()
{
	gpio_out ("/dev/gpio/out", pwr_mask, ~pwr_value);
}

static void p_alarm(preset *p)
{
	int i;
	int ondur_x = p->ondur;

	if (p->def_off && !p->negative && p->count >= 0) {
		pwr_on();
		sleep(p->sleep);
	}
	else
		ondur_x = 0;

	while (running && p->count >= 0) {
		if (p->count > 0) {
			if (p->count == 1) p->count = -1;
			p->count--;
		}
		if ((p->negative && running)) {pwr_off(); sleep(p->sleep);};
		for (i = 0; i < p->cycles && running; i++) {
			pwr_on();
			if (running)
				usleep(ondur_x * 10000);
				ondur_x = p->ondur;
			pwr_off();
			if (running)
				usleep(p->offdur * 10000);
		}
		if (!p->negative && running) {pwr_on(); sleep(p->sleep);};
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


static preset *get_preset_by_name(char *name)
{
	int i;
	for (i = 0;; i++) {
		if (presets[i].name == NULL)
			break;
		if (strcmp(presets[i].name, name ? name : "") == 0)
			break;
	}
	return &presets[i];
}

static void print_preset(preset *pprst)
{
	printf ("%s%s-c%d -n%d -s%d -d%d -e%d %s\n",
			pprst->negative ? "-n " : "", pprst->def_off ? "-f " : "",
			pprst->count, pprst->cycles, pprst->sleep,
			pprst->ondur, pprst->offdur, pprst->name ? pprst->name : "");
}

static int usage(char *name)
{
	fprintf (stderr,	"Usage: %s [-fghl] [-b [-p pidile]] [-c count] [-e on_dur] "
						"[-d off_dur] [-n cycles] [-s sleep] [preset]\n\n", name);
	return 1;
}

static int i_usage(char *name)
{
	fprintf (stderr,
			"pwrled v" PWRLED_VERSION_STRING ", an utility for manipulate state of the pwr led\n"
			"on ASUS WL-500g DELUXE/WL-550gE/WL-500g PREMIUM/WL-500w routers\n"
			"Written by Mam(O)n; http://wl500g.info\n\n");
	usage (name);
	fprintf (stderr,
			"   -n cycle   Count of flash's before pause.\n"
			"   -e on_dur  Sleep time in the flash cycle when led is on.\n"
			"   -d off_dur Sleep time in the flash cycle when led is off.\n"
			"   -s sleep   Pause after full flash cycle is reached.\n"
			"   -g         \"Negative flash\" mode. E.g. Pause(-s) used when\n"
			"              led in turned off.\n"
			"   -c count   Count of full flash cycles with pauses(-s).\n"
			"              Use 0 for infinity, -1 for none.\n"
			"   -f         Turn off the led after done or kill.\n"
			"   -b         Fork to background (demonize).\n"
			"   -p file    Write PID to file. Use '-' to stdout output.\n"
			"              This option must be used with conjunction of -b.\n"
			"   -l         Print conjunction of preset & user options and exit.\n"
			"   -h         Print this help and exit.\n"
			"\n"
			"Avaliable presets: ");

	int i;
	for (i = 0;; i++) {
		if (presets[i].name == NULL)
			break;
		fprintf (stderr, i == 0 ? "%s": " %s", presets[i].name);
	}

	if (i == 0) {
		fprintf(stderr, "ERROR: No presets avaliable");
	}
	fprintf (stderr,"\n\n");

	return 1;
}

static int h_usage(char *name) {
	usage (name);
	fprintf (stderr, "Type %s -h for more information\n\n", name);
	return 1;
}

int main(int argc, char *argv[])
{
	int opt_prprint = 0;
	int opt_bfork = 0;

	char *opt_pidfile = NULL;
	int pid_stdout = 0;
	FILE *pidfile;

	const preset defopt = {NULL, -1, -1, -1, -1, -1, -2, -1};
	preset opt = defopt;
	preset *pprst;

	int c;
	while ((c = getopt (argc, argv, "fghlbc:n:e:d:s:p:")) != -1) {
		switch (c) {
			case 'c':	opt.count = atoi(optarg); break;
			case 'n':	opt.cycles = atoi(optarg); break;
			case 'd':	opt.offdur = atoi(optarg); break;
			case 'e':	opt.ondur = atoi(optarg); break;
			case 's':	opt.sleep = atoi(optarg); break;
			case 'l':	opt_prprint = 1; break;
			case 'b':	opt_bfork = 1; break;
			case 'p':
						if (strcmp(optarg, "-") == 0)
							pid_stdout = 1;
						else
							opt_pidfile = optarg;
						break;
			case 'g': opt.negative = 1; break;
			case 'f': opt.def_off = 1; break;
			case 'h':
				return (i_usage(argv[0]));
			default :
				return (h_usage(argv[0]));
		}
	}

	if (optind < argc) {
		opt.name = argv[optind];
	}

	for (c = optind + 1; c < argc; c++) {
		if (c == optind + 1)
			fprintf (stderr, "ERROR: Garbage after preset name '%s': %s", opt.name, argv[c]);
		else
			fprintf (stderr, " %s", argv[c]);
	}

	if (optind + 1 < argc) {
		fprintf (stderr, "\n");
		return (1);
	}


	pprst = get_preset_by_name(opt.name);
	if (opt.name && !pprst->name) {
		fprintf(stderr, "ERROR: Preset '%s' not found\n", opt.name);
		return 1;
	}
	if (opt.count	> -2) pprst->count		= opt.count;
	if (opt.cycles	> -1) pprst->cycles		= opt.cycles;
	if (opt.offdur	> -1) pprst->offdur		= opt.offdur;
	if (opt.ondur	> -1) pprst->ondur		= opt.ondur;
	if (opt.sleep	> -1) pprst->sleep		= opt.sleep;
	if (opt.negative> -1) pprst->negative	= opt.negative;
	if (opt.def_off	> -1) pprst->def_off	= opt.def_off;


	if (memcmp(&opt, &defopt, sizeof(preset)) == 0) {
		fprintf(stderr, "ERROR: No one of alarm option or preset has been specified\n");
		return (h_usage(argv[0]));
	}

	if (opt_prprint) {
		print_preset(pprst);
		return 0;
	}

	if (!init()) {
		fprintf(stderr, "ERROR: No suitable hardware found\n");
		return (1);
	}

	if (opt_bfork) {
		pid_t pid;

		pid = fork();

		if (pid > 0) {
			if (opt_pidfile || pid_stdout) {
				if (pid_stdout)
					pidfile = stdout;
				else if (!(pidfile = fopen(opt_pidfile, "w"))) {
					fprintf (stderr, "WARNING: Can't open pid file '%s' for writing", opt_pidfile);
					return 2;
				}
				if (fprintf(pidfile, "%d", (int)pid) <= 0) {
					fprintf (stderr, "WARNING: Can't write to pid file '%s'", opt_pidfile);
					return 2;
				}
				if (!pid_stdout && fclose(pidfile) != 0) {
					fprintf (stderr, "WARNING: Can't close pid file '%s'", opt_pidfile);
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

	p_alarm(pprst);

	if (opt_bfork && opt_pidfile && !pid_stdout) {
		unlink(opt_pidfile);
	}
	return (0);
}
