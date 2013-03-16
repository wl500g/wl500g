/*
 * System V init functionality
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
#include <errno.h>
#include <paths.h>
#include <signal.h>
#include <stdarg.h>
#define __USE_GNU
#include <string.h>
#include <termios.h>
#include <unistd.h>
#include <limits.h>
#include <sys/klog.h>
#include <sys/fcntl.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <sys/mount.h>
#include <sys/reboot.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/time.h>
#include <sys/utsname.h>

#include "rc.h"

#define loop_forever() do { sleep(1); } while (1)
#define SHELL "/bin/sh"

int noconsole = 0;

static const int fatal_signals[] = {
	SIGQUIT,	/* halt */
	SIGILL,
	SIGABRT,
	SIGFPE,
	SIGPIPE,
	SIGBUS,
	SIGSEGV,
	SIGSYS,
	SIGTRAP,
	SIGPWR,		/* reboot w/o preshutdown */
	SIGTERM,	/* reboot */
};

/* Set terminal settings to reasonable defaults */
static void set_term(int fd)
{
	struct termios tty;

	tcgetattr(fd, &tty);

	/* set control chars */
	tty.c_cc[VINTR]  = 3;	/* C-c */
	tty.c_cc[VQUIT]  = 28;	/* C-\ */
	tty.c_cc[VERASE] = 127; /* C-? */
	tty.c_cc[VKILL]  = 21;	/* C-u */
	tty.c_cc[VEOF]   = 4;	/* C-d */
	tty.c_cc[VSTART] = 17;	/* C-q */
	tty.c_cc[VSTOP]  = 19;	/* C-s */
	tty.c_cc[VSUSP]  = 26;	/* C-z */

	/* use line dicipline 0 */
	tty.c_line = 0;

	/* Make it be sane */
	tty.c_cflag &= CBAUD|CBAUDEX|CSIZE|CSTOPB|PARENB|PARODD;
	tty.c_cflag |= CREAD|HUPCL|CLOCAL;


	/* input modes */
	tty.c_iflag = ICRNL | IXON | IXOFF;

	/* output modes */
	tty.c_oflag = OPOST | ONLCR;

	/* local modes */
	tty.c_lflag =
		ISIG | ICANON | ECHO | ECHOE | ECHOK | ECHOCTL | ECHOKE | IEXTEN;

	tcsetattr(fd, TCSANOW, &tty);
}

static int console_init()
{
	int fd;
	int ret = 0;

	/* Clean up */
	ioctl(0, TIOCNOTTY, 0);
	close(0);
	close(1);
	close(2);
	setsid();

	/* Reopen console */
	if ((fd = open(_PATH_CONSOLE, O_RDWR)) < 0) {
		perror(_PATH_CONSOLE);
		if ((fd = open("/dev/null", O_RDWR)) < 0) {
			perror("/dev/null");
			return errno;
		}
		ret = -1;
	}
	dup2(fd, 1);
	dup2(fd, 2);

	ioctl(0, TIOCSCTTY, 1);
	tcsetpgrp(0, getpgrp());
	set_term(0);

	return ret;
}

pid_t
run_shell(int timeout, int nowait)
{
	pid_t pid;
	char tz[128];
	char * const envp[] = {
		"TERM=vt100",
		"HOME=/",
		"PATH=/usr/bin:/bin:/usr/sbin:/sbin:/usr/local/sbin",
		"SHELL=" SHELL,
		"USER=root",
		tz,
		NULL
	};
	int sig;

	/* Wait for user input */
	cprintf("Hit enter to continue...\n");
	if (waitfor(STDIN_FILENO, timeout) <= 0)
		return 0;

	switch ((pid = fork())) {
	case -1:
		perror("fork");
		return 0;
	case 0:
		/* Reset signal handlers set for parent process */
		for (sig = 0; sig < (_NSIG-1); sig++)
			signal(sig, SIG_DFL);

		/* Reopen console */
		console_init();

		/* Pass on TZ */
		snprintf(tz, sizeof(tz), "TZ=%s", getenv("TZ"));

		/* Now run it.  The new program will take over this PID, 
		 * so nothing further in init.c should be run. */
		execve(SHELL, (char *[]) { "/bin/sh", NULL }, envp);

		/* We're still here?  Some error happened. */
		perror(SHELL);
		exit(errno);
	default:
		if (nowait)
			return pid;
		else {
			waitpid(pid, NULL, 0);
			return 0;
		}
	}
}

void
preshutdown_system(void)
{
	eval("/usr/local/sbin/pre-shutdown");
	sleep(2);
}

static void
shutdown_system(void)
{
	int sig;
	sigset_t set;

	/* Block fatal signals */
	sigemptyset(&set);
	for (sig = 0; sig < ARRAY_SIZE(fatal_signals); sig++)
		sigaddset(&set, fatal_signals[sig]);
	sigprocmask(SIG_BLOCK, &set, NULL);

	/* Disable signal handlers */
	for (sig = 0; sig < (_NSIG-1); sig++)
		signal(sig, SIG_DFL);

	/* Disconnect pppd - need this for PPTP/L2TP to finish gracefully */
#ifdef __CONFIG_XL2TPD__
	killall("xl2tpd");
#else
	killall("l2tpd");
#endif
	killall("pppd");

	if (exists(DEV_RTC))
		eval("/sbin/hwclock", "-w");

	cprintf("Sending SIGTERM to all processes\n");
	kill(-1, SIGTERM);
	sleep(3);

	cprintf("Sending SIGKILL to all processes\n");
	kill(-1, SIGKILL);
	sleep(1);

	sync(), sync();

	/* bring wifi interfaces down */
	eval("wl", "radio", "off");
}

/*
 * create /etc/passwd and /etc/group files
 */
static void make_etc(void)
{
	FILE *f;
	const char *name, *pass;
	
	/* crypt using md5, no salt */
	name = nvram_safe_default_get("http_username");
	pass = crypt(nvram_safe_default_get("http_passwd"), "$1$");
	
	if ((f = fopen("/etc/passwd", "w"))) {
		fprintf(f, "%s:%s:0:0:root:/usr/local/root:/bin/sh\n"
			"nobody:x:99:99:nobody:/:/sbin/nologin\n", name, pass);
		fclose(f);
	}
	
	if ((f = fopen("/etc/group", "w"))) {
		fprintf(f, "root:x:0:%s\nnobody:x:99:\n", name);
		fclose(f);
	}
	
	/* uClibc TZ */
	if ((f = fopen("/etc/TZ", "w"))) {
		fprintf(f, "%s\n", nvram_safe_get("time_zone"));
		fclose(f);
	}
	
	/* /etc/resolv.conf compatibility */
	symlink("/tmp/resolv.conf", "/etc/resolv.conf");
	
	/* hostname */
	fputs_ex("/proc/sys/kernel/hostname", nvram_safe_get("lan_hostname"));

	/* crond */
	symlink("/etc/crontabs", "/var/spool/cron/crontabs");

	/* mtab compability */
	symlink("/proc/mounts", "/etc/mtab");
}

void sysinit(void)
{
	char buf[PATH_MAX];
	struct utsname name;
	struct rlimit lim;
	int totalram;
	const char *tmpfsopt = NULL;

	/* set default hardlimit */
	getrlimit(RLIMIT_NOFILE, &lim);
	lim.rlim_max = 16384;
	setrlimit(RLIMIT_NOFILE, &lim);

	/* /proc */
	mount("proc", "/proc", "proc", MS_MGC_VAL, NULL);

	mount("sysfs", "/sys", "sysfs", MS_MGC_VAL, NULL);
	mount("devfs", "/dev", "tmpfs", MS_MGC_VAL | MS_NOATIME, "size=100K");

	/* populate /dev */
	mknod("/dev/console", S_IFCHR | 0600, makedev(5, 1));
	mknod("/dev/null", S_IFCHR | 0666, makedev(1, 3));
	eval("/sbin/mdev", "-s");

	/* /dev/pts */
	mkdir("/dev/pts", 0755);
	mount("devpts", "/dev/pts", "devpts", MS_MGC_VAL, NULL);

	/* /dev/shm */
	mkdir("/dev/shm", S_ISVTX | 0755);
	
	/* extra links */
	symlink("/proc/self/fd", "/dev/fd");
	symlink("/proc/self/fd/0", "/dev/stdin");
	symlink("/proc/self/fd/1", "/dev/stdout");
	symlink("/proc/self/fd/2", "/dev/stderr");

	totalram = router_totalram();

	/* /tmp */
	if (totalram <= 16*1024*1024 /* 16MB */) {
		snprintf(buf, sizeof(buf), "size=%dK", (totalram >> 2) >> 10);
		tmpfsopt = buf;
	}
	mount("tmpfs", "/tmp", "tmpfs", MS_MGC_VAL | MS_NOATIME, tmpfsopt);

	/* /var */
	mkdir("/tmp/var", 0777);
	mkdir("/var/lock", 0777);
	mkdir("/var/log", 0777);
	mkdir("/var/run", 0777);
	mkdir("/var/spool", 0777);
	mkdir("/var/spool/cron", 0777);
	mkdir("/var/state", 0777);
	mkdir("/var/tmp", 0777);
	
	/* /usr/local */
	mkdir("/tmp/local", 0755);
	mkdir("/tmp/local/root", 0700);
	
	/* /etc contents */
	eval("cp", "-dpR", "/usr/etc", "/tmp");

	/* create /etc/{passwd,group,TZ} */
	make_etc();
	
	/* Setup console */
	if (console_init())
		noconsole = 1;
	klogctl(8, NULL, nvram_get_int("console_loglevel"));

	/* load flashfs */
	if (eval("flashfs", "start") == 0)
		eval("/usr/local/sbin/pre-boot");

	/* Modules */
	uname(&name);
	snprintf(buf, sizeof(buf), "/lib/modules/%s", name.release);
	if (exists("/proc/modules") && exists(buf)) {
		char module[80], *modules, *next;

		modules = nvram_get("kernel_mods") ? : 
#if defined(MODEL_WL700G)
		"ide-core aec62xx "
		"ide-disk "
#endif
#if defined(__CONFIG_EMF__)
		"emf igs "
#endif
		"et wl";
		foreach(module, modules, next)
			insmod(module, NULL);
	}

	update_tztime(1);

#if defined(MODEL_WL700G)
	insmod("gpiortc", "sda_mask=0x04", "scl_mask=0x20", NULL);
	usleep(100000);
#endif

	if (exists(DEV_RTC))
		eval("/sbin/hwclock", "-s");

	dprintf("done\n");
}

static void fatal_signal(int sig)
{
	const char *message = strsignal(sig);

	if (message)
		cprintf("%s\n", message);
	else
		cprintf("Caught signal %d\n", sig);

	/* Perform pre-shutdown script only with common signals */
	switch (sig) {
	case SIGQUIT:
	case SIGTERM:
		preshutdown_system();
		break;
	}

	shutdown_system();
	sleep(2);

	/* Halt on SIGQUIT */
	reboot((sig == SIGQUIT) ? RB_HALT_SYSTEM : RB_AUTOBOOT);
	loop_forever();
}

void
child_reap(int sig)
{
	pid_t pid;

	while ((pid = waitpid(-1, NULL, WNOHANG)) > 0)
		dprintf("Reaped %d\n", pid);
}

void
signal_init(void)
{
	int i;

	for (i = 0; i < ARRAY_SIZE(fatal_signals); i++)
		signal(fatal_signals[i], fatal_signal);

	signal(SIGCHLD, child_reap);
}
