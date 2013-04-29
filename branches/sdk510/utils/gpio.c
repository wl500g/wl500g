/*
 * gpio v.0.6.20111008
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

#define GPIO_VERSION_STRING "0.6.20111009"

#ifdef HAVE_GETMODEL
#include <shutils.h>
#endif

#define GPIO0  0x00000001
#define GPIO1  0x00000002
#define GPIO2  0x00000004
#define GPIO3  0x00000008
#define GPIO4  0x00000010
#define GPIO5  0x00000020
#define GPIO6  0x00000040
#define GPIO7  0x00000080
#define GPIO8  0x00000100
#define GPIO9  0x00000200
#define GPIO10 0x00000400
#define GPIO11 0x00000800
#define GPIO12 0x00001000
#define GPIO13 0x00002000
#define GPIO14 0x00004000
#define GPIO15 0x00008000
#define GPIO16 0x00010000
#define GPIO17 0x00020000
#define GPIO18 0x00040000
#define GPIO19 0x00080000
#define GPIO20 0x00100000
#define GPIO21 0x00200000
#define GPIO22 0x00400000
#define GPIO23 0x00800000

#define GPIOCTL


#ifdef GPIOCTL
#include <sys/ioctl.h>
#include <linux_gpio.h>
#define GPIO_DEV_OUT	GPIO_IOC_OUT
#else
#define GPIO_DEV_OUT	"/dev/gpio/out"
#endif

static inline int startswith(char *source, char *cmp)
{
	return !strncmp(source, cmp, strlen(cmp));
}

enum {
	LED_PWR = 0,
	LED_WAN,
	LED_LAN,
	LED_WLN,
	LED_WPS,
	LED_USB,
	LED_MAX,
};

#ifndef HAVE_GETMODEL
enum {
	MDL_UNKNOWN,
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
	MDL_RTN66U,
	MDL_RTN53,
	MDL_RTN16,
	MDL_RTN15U,
	MDL_RTN12,
	MDL_RTN12B1,
	MDL_RTN12C1,
	MDL_RTN10,
	MDL_RTN10U,
	MDL_WNR3500L,
};
#endif

typedef struct {
	char *name;	// preset name
	int cycles;	// cycles
	int ondur;	// on duration
	int offdur;	// off duration
	int sleep;	// sleep between durations
	int negative;	// inverse on/off
	int count;	// cycles count
	int def_off;	// default value is off
} preset_t;

typedef struct {
	char *name;
	unsigned int mask;
	unsigned int value;
} led_t;

typedef struct {
	int model;
	char *name;
	led_t leds[LED_MAX];
} platform_t;

static preset_t presets[] = {
	{"off",		 0,  0,  0, 0, 0, -1, 1},
	{"on",		 0,  0,  0, 0, 0, -1, 0},
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
	{NULL},
};
static preset_t defopt =
	{NULL, -1, -1, -1, -1, -1, -2, -1};

static led_t leds[] = {
	[LED_PWR] = {"pwr"},
	[LED_WAN] = {"wan"},
	[LED_LAN] = {"lan"},
	[LED_WLN] = {"wlan"},
	[LED_WPS] = {"wps"},
	[LED_USB] = {"usb"},
	{NULL},
};

#define _LED(led, m, h) [led] = {.mask = m, .value = h ? m : 0}
#define _PWR(mask, high) _LED(LED_PWR, mask, high)
#define _WAN(mask, high) _LED(LED_WAN, mask, high)
#define _LAN(mask, high) _LED(LED_LAN, mask, high)
#define _WLN(mask, high) _LED(LED_WLN, mask, high)
#define _WPS(mask, high) _LED(LED_WPS, mask, high)
#define _USB(mask, high) _LED(LED_USB, mask, high)

static platform_t platforms[] = {
	/* Asus */
	{MDL_WL320GE,	"ASUS WL-320gE/WL-320gP",	{_PWR(GPIO2, 0)}},
	{MDL_WL330GE,	"ASUS WL-330gE",		{_PWR(GPIO0, 0)}},
#ifndef HAVE_GETMODEL
	{MDL_WL500,	"ASUS WL-500g",			{_PWR(GPIO0, 0)}},
	{MDL_WL500GX,	"ASUS WL-500g Deluxe",		{_PWR(GPIO0, 0)}},
#endif
	{MDL_WL500GP,	"ASUS WL-500g Premium",  	{_PWR(GPIO1, 0)}},
	{MDL_WL500GPV2,	"ASUS WL-500g PremiumV2",	{_PWR(GPIO0, 0)}},
	{MDL_WL500W,	"ASUS WL-500W",			{_PWR(GPIO5, 0)}},
	{MDL_WL520GU,	"ASUS WL-520gU",		{_PWR(GPIO0, 0)}},
#ifndef HAVE_GETMODEL
	{MDL_WL520GC,	"ASUS WL-520GC",		{_PWR(GPIO0, 0)}},
#endif
	{MDL_WL550GE,	"ASUS WL-550gE",		{_PWR(GPIO2, 0)}},
	{MDL_WL700G,	"ASUS WL-700g",			{_PWR(GPIO1, 1)}},
	{MDL_RTN16,	"ASUS RT-N16",			{_PWR(GPIO1, 0), _WLN(GPIO7, 1)}},
	{MDL_RTN15U,	"ASUS RT-N15U",			{_PWR(GPIO6, 0), _WLN(GPIO0, 1), _USB(GPIO9, 1)}},
	{MDL_RTN12,	"ASUS RT-N12",			{_PWR(GPIO2, 0)}},
	{MDL_RTN12B1,	"ASUS RT-N12B1",		{_PWR(GPIO18,0), _WLN(GPIO5, 1)}},
	{MDL_RTN12C1,	"ASUS RT-N12C1",		{_PWR(GPIO18,0), _WLN(GPIO5, 1)}},
	{MDL_RTN10,	"ASUS RT-N10",			{_PWR(GPIO1, 0)}},
	{MDL_RTN10U,	"ASUS RT-N10U",			{_PWR(GPIO6, 0), _WLN(GPIO5, 1), _WPS(GPIO7, 1), _USB(GPIO8, 1)}},
	/* D-Link */
	{MDL_DIR320,	"D-Link DIR-320",		{_PWR(GPIO0, GPIO0)}},
	/* Netgear */
	{MDL_WNR3500L,	"Netgear WNR3500L",		{_PWR(GPIO3, GPIO3)}},
	{-1,		NULL},
};

static platform_t *platform;
static unsigned int mask;
static unsigned int value;
static int running = 1;

#ifdef GPIOCTL
static int tgpio_ioctl(int fd, int gpioreg, unsigned int mask, unsigned int value)
{
	struct gpio_ioctl gpio;

	gpio.mask = mask;
	gpio.val = value;

	if (ioctl(fd, gpioreg, &gpio) < 0) {
		fprintf(stderr, "ERROR: Invalid gpioreg %d\n", gpioreg);
		return -1;
	}

	return gpio.val;
}

static unsigned int tgpio_multi(int fd, int gpioreg, unsigned int mask, unsigned int value)
{
	unsigned int bit, val = 0;
	int i;

	for (i = 0; mask >> i; i++) {
		if ((bit = mask & (1 << i)) && /* enabled */
		    tgpio_ioctl(fd, gpioreg, bit, value & bit) >= 0)
			val |= bit;
	}
	return val;
}

static void gpio_write(unsigned int mask, unsigned int value)
{
	unsigned int val;
	int fd = open("/dev/gpio", O_RDWR);

	if (fd < 0)
		return;
	val = tgpio_multi(fd, GPIO_IOC_RESERVE, mask, mask);
	tgpio_multi(fd, GPIO_IOC_OUTEN, mask, mask);
	tgpio_ioctl(fd, GPIO_IOC_OUT, mask, value & mask);
	tgpio_multi(fd, GPIO_IOC_RELEASE, val, val);
	close(fd);
}

static unsigned int gpio_read(unsigned int mask)
{
	unsigned int val;
	int fd = open("/dev/gpio", O_RDWR);

	if (fd < 0)
		return 0;
	val = tgpio_ioctl(fd, GPIO_IOC_IN, mask, 0);
	close(fd);
	return val & mask;
}

#else
static void gpio_write(unsigned int mask, unsigned int value)
{
	unsigned int val = 0;
	int fd = open("/dev/gpio/out", O_RDWR);

	if (fd < 0)
		return;
	if (read(fd, &val, sizeof(val)) == sizeof(val)) {
		val = (val & ~mask) | (value & mask);
		write(fd, &val, sizeof(val));
	}
	close(fd);
}

static unsigned int gpio_read(unsigned int mask)
{
	unsigned int val = 0;
	int fd = open("/dev/gpio/in", O_RDWR);

	if (fd < 0)
		return 0;
	read(fd, &val, sizeof(val));
	close(fd);

	return val & mask;
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
			if (!strcmp(boardtype,"0x052b"))
				return MDL_RTN15U;
			if (!strcmp(boardtype,"0x04CD"))
				return MDL_RTN12;
			if (!strcmp(boardtype,"0x054D") &&
			    !strncmp(nvram_safe_get("hardware_version", "RTN12C1", 7))
				return MDL_RTN12C1;
			if (!strcmp(boardtype,"0x054D"))
				return MDL_RTN12B1;
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

static platform_t *platform_detect()
{
	int i, model = get_model();

	for (i = 0; platforms[i].name; i++) {
		if (platforms[i].model == model)
			return &platforms[i];
	}
	return NULL;
}

static void led(int on)
{
	gpio_write(mask, on ? value : ~value);
}

static void play_alarm(preset_t *p)
{
	int i;
	int ondur_x = p->ondur;

	if (p->def_off && !p->negative && p->count >= 0) {
		led(1);
		sleep(p->sleep);
	} else
		ondur_x = 0;

	while (running && p->count >= 0) {
		if (p->count > 0) {
			if (p->count == 1) p->count = -1;
			p->count--;
		}
		if ((p->negative && running)) {
		    led(0);
		    sleep(p->sleep);
		};
		for (i = 0; i < p->cycles && running; i++) {
			led(1);
			if (running) usleep(ondur_x * 10000);
			ondur_x = p->ondur;
			led(0);
			if (running) usleep(p->offdur * 10000);
		}
		if (!p->negative && running) {
		    led(1);
		    sleep(p->sleep);
		};
	}

	led(p->def_off ? 0 : 1);
}

static void shutdown(int sig)
{
	running = 0;
}

static preset_t *search_preset(char *name)
{
	int i;

	for (i = 0; presets[i].name; i++) {
		if (!strcmp(presets[i].name, name))
			return &presets[i];
	}
	return NULL;
}

static led_t *search_led(char *name, platform_t *platform)
{
	int i;

	for (i = 0; platform && leds[i].name; i++) {
		if (!strcmp(leds[i].name, name))
			return platform->leds[i].mask ? &platform->leds[i] : NULL;
	}
	return NULL;
}

static void print_preset(preset_t *p)
{
	printf("%s%s-c%d -n%d -s%d -d%d -e%d %s\n",
		p->negative ? "-g " : "",
		p->def_off > 0 ? "-f " : "",
		p->count,
		p->cycles,
		p->sleep,
		p->ondur,
		p->offdur,
		p->name ? p->name : "");
}

static int strtoi(char *str, int min, int max)
{
	char *err = NULL;
	int ret;

	ret = strtol(str, &err, 10);
	if (*err || ret < min || ret > max)
		return min - 1;

	return ret;
}

static int usage(char *name)
{
	fprintf(stderr,
	    "Usage: %s [-fghlvr] [-b [-p pidile]] [-c count] [-e on_dur] "
	    "[-d off_dur] [-n cycles] [-s sleep] [led|0~31] [preset|1|0]\n", name);

	return 1;
}

static int i_usage(char *name)
{
	int i;

	fprintf(stderr,
		"gpio/pwrled v" GPIO_VERSION_STRING ", an utility for manipulate state of gpio leds\n"
		"Written by Mam(O)n; http://wl500g.info; theMIROn; http://wl500g.googlecode.com\n");
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
		"   -r         Read led/gpio value.\n"
		"   -h         Print this help and exit.\n");
	fprintf(stderr, "Supported routers: ");
	for (i = 0; platforms[i].name; i++) {
		fprintf(stderr, i == 0 ? "%s": ", %s", platforms[i].name);
	}
	fprintf(stderr, "\n");
	fprintf(stderr, "Supported leds: ");
	for (i = 0; leds[i].name; i++) {
		fprintf(stderr, i == 0 ? "%s": ", %s", leds[i].name);
	}
	fprintf(stderr, "\n");
	fprintf(stderr, "Avaliable presets: ");
	for (i = 0; presets[i].name; i++) {
		fprintf(stderr, i == 0 ? "%s": ", %s", presets[i].name);
	}
	fprintf(stderr, "\n");

	return 1;
}

static int h_usage(char *name)
{
	usage(name);
	fprintf(stderr, "Type %s -h for more information\n\n", name);

	return 1;
}

int main(int argc, char *argv[])
{
	FILE *pidfile;
	char *led_name, *set_name;
	char *opt_pidfile = NULL;
	int opt_prprint = 0;
	int opt_bfork = 0;
	int opt_verbose = 0;
	int opt_read = 0;
	int pid_stdout = 0;
	int i;
	led_t *led = NULL;
	preset_t *preset = NULL;
	preset_t opt = defopt;

	platform = platform_detect();

	while ((i = getopt (argc, argv, "fghlbvrc:n:e:d:s:p:")) != -1) {
		switch (i) {
			case 'c':	opt.count = atoi(optarg); break;
			case 'n':	opt.cycles = atoi(optarg); break;
			case 'd':	opt.offdur = atoi(optarg); break;
			case 'e':	opt.ondur = atoi(optarg); break;
			case 's':	opt.sleep = atoi(optarg); break;
			case 'l':	opt_prprint = 1; break;
			case 'b':	opt_bfork = 1; break;
			case 'v':	opt_verbose = 1; break;
			case 'r':	opt_read = 1; break;
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

	if (opt_verbose) {
		if (platform) {
			fprintf(stderr, "Router detected: %s\n", platform->name);
			fprintf(stderr, "Available leds:  ");
			for (i = 0; leds[i].name; i++) {
				if (platform->leds[i].mask)
					fprintf(stderr, i == 0 ? "%s": ", %s", leds[i].name);
			}
			fprintf(stderr, "\n");
		} else
			fprintf(stderr, "No known hardware detected.\n");
	}

	/* pwr by default, if not specified */
	if (argv[optind]) {
		led_name = argv[optind++];
		set_name = argv[optind++];
	} else {
		led_name = leds[LED_PWR].name;
		set_name = NULL;
	}

	/* gpio [led_name] ... */
	if ((led = search_led(led_name, platform)) != NULL) {
		mask = led->mask;
		value = led->value;
	} else
	/* gpio [gpio] ... */
	if ((i = strtoi(led_name, 0, 31)) >= 0) {
		mask = 1 << i;
		value = 1 << i;
		opt_read = 1;
	} else
	/* gpio [preset] ... */
	if ((preset = search_preset(led_name)) != NULL &&
	    (set_name = led_name) &&
	    (led_name = leds[LED_PWR].name) &&
	    (led = search_led(led_name, platform)) != NULL) {
		mask = led->mask;
		value = led->value;
	} else {
		fprintf(stderr, "ERROR: led/gpio/preset '%s' not valid\n", led_name);
		return 1;
	}

	/* gpio [opts] */
	if (!set_name) {
		if (opt_read) {
			fprintf(stdout, "%d\n", gpio_read(mask) == mask ? 1 : 0);
			return 0;
		}
		preset = search_preset("on");
	} else
	/* gpio ... [preset] */
	if ((preset = search_preset(set_name)) != NULL) {
		opt.name = preset->name;
	} else
	/* gpio ... <gpio_val> */
	if ((i = strtoi(set_name, 0, 1)) >= 0) {
		gpio_write(mask, i ? mask : ~mask);
		return 0;
	} else {
		fprintf(stderr, "ERROR: preset/value '%s' not valid\n", set_name);
		return 1;
	}

	if (!memcmp(&opt, &defopt, sizeof(preset_t)))
		return h_usage(argv[0]);

	if (opt.count <= defopt.count)
		opt.count = preset->count;
	if (opt.cycles <= defopt.cycles) 
		opt.cycles = preset->cycles;
	if (opt.offdur <= defopt.offdur)
		opt.offdur = preset->offdur;
	if (opt.ondur <= defopt.ondur)
		opt.ondur = preset->ondur;
	if (opt.sleep <= defopt.sleep)
		opt.sleep = preset->sleep;
	if (opt.negative <= defopt.negative)
		opt.negative = preset->negative;
	if (opt.def_off	<= defopt.def_off)
		opt.def_off = preset->def_off;

	preset = &opt;
	if (opt_prprint) {
		print_preset(preset);
		return 0;
	}

	if (opt_bfork) {
		pid_t pid;

		switch(pid = fork()) {
		case -1:
			fprintf(stderr, "ERROR: Unable fork to background\n");
			return 1;
		case 0:
			break;
		default:
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
	}

	signal(SIGTERM, shutdown);
	signal(SIGINT, shutdown);
	signal(SIGHUP, shutdown);

	play_alarm(preset);

	if (opt_bfork && opt_pidfile && !pid_stdout) {
		unlink(opt_pidfile);
	}

	return 0;
}
