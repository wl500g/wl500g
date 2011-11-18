#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <arpa/inet.h>

#include "rc.h"

#ifdef __CONFIG_EAPOL__
extern int router_model;


static int stop_wpa_supplicant(int wait)
{
	killall_w("wpa_supplicant", 0, wait);
	killall_w("wpa_cli", 0, wait);

	return 0;
}

static int start_wpa_supplicant(const char *prefix, int restart)
{
	FILE *fp;
	char tmp[100];
	const char *options = "/etc/wpa_supplicant.conf";
	char *wpa_argv[] = {"/usr/sbin/wpa_supplicant",
		"-B", "-W",
		"-D", "roboswitch",
		"-i", nvram_safe_get(strcat_r(prefix, "ifname", tmp)),
		"-c", (char *)options,
		NULL
	};
	char *cli_argv[] = {"/usr/sbin/wpa_cli",
		"-B",
		"-a", "/tmp/wpa_cli.script",
		NULL
	};
	int ret;

	if (restart)
		stop_wpa_supplicant(1);

	/* Select supplicant drivers here */
	if (router_model == MDL_RTN16)
		wpa_argv[4] = "wired";

	/* Generate options file */
	if ((fp = fopen(options, "w")) == NULL) {
		perror(options);
		return -1;
	}
	fprintf(fp,
		"ctrl_interface=/var/run/wpa_supplicant\n"
		"ap_scan=0\n"
		"fast_reauth=1\n"
		"network={\n"
		"	key_mgmt=IEEE8021X\n"
		"	eap=MD5\n"
		"	identity=\"%s\"\n"
		"	password=\"%s\"\n"
		"	eapol_flags=0\n"
		"}\n",
		nvram_safe_get(strcat_r(prefix, "auth_username", tmp)),
		nvram_safe_get(strcat_r(prefix, "auth_passwd", tmp)));
	fclose(fp);

	/* Start wpa_supplicant */
	ret = _eval(wpa_argv, NULL, 0, NULL);
	if (ret == 0)
		_eval(cli_argv, NULL, 0, NULL);

	return ret;
}

int wpacli_main(int argc, char **argv)
{
	char tmp[100], prefix[sizeof("wanXXXXXXXXXX_")];
	int unit;

	if (!argv[1] || (unit = wan_prefix(argv[1], prefix)) < 0)
		return EINVAL;

	if (!nvram_match(strcat_r(prefix, "auth_x", tmp), "eap-md5"))
		return 0;

	if (nvram_match(strcat_r(prefix, "proto", tmp), "dhcp") &&
	    strncmp(argv[2], "EAP-SUCCESS", sizeof("EAP-SUCCESS")) == 0)
	{
		/* Renew DHCP lease */
		snprintf(tmp, sizeof(tmp), "/var/run/udhcpc%d.pid", unit);
		kill_pidfile_s(tmp, SIGUSR1);
	}

	logmessage("auth client", "%s", argv[2]);

	return 0;
}
#endif /* __CONFIG_EAPOL__ */

#ifdef __CONFIG_TELENET__
static int stop_lanauth(int wait)
{
	return killall_w("lanauth", 0, wait);
}

static int start_lanauth(const char *prefix, int restart)
{
	char tmp[100];
	char *lanauth_argv[] = {
		"/usr/sbin/lanauth",
	    /*	"-v", protocol,	   */
	    /*	"-l", acces level, */
		"-p", nvram_safe_get(strcat_r(prefix, "auth_passwd", tmp)),
		nvram_invmatch("wan_heartbeat_x", "") ? "-s" : NULL,
		nvram_safe_get("wan_heartbeat_x"),
   		NULL
	};

	if (restart)
		stop_lanauth(1);

	/* Start lanauth */
	return _eval(lanauth_argv, NULL, 0, NULL);
}
#endif /* __CONFIG_TELENET__ */

#ifdef __CONFIG_CONVEX__
static int stop_authcli(int wait)
{
	return killall_w("authcli", 0, wait);
}

static int start_authcli(const char *prefix, int restart)
{
	FILE *fp;
	char tmp[100];
	const char *options = "/etc/authcliw.conf";
	char *authcli_argv[] = {
		"/usr/sbin/authcli",
		"-c", (char *)options,
	    /*	"-p", pid_file,    */
	    /*	"-c", info_name,   */
	    	"-u", "nobody",
   		NULL
	};

	if (restart)
		stop_authcli(1);

	/* Generate options file */
	if ((fp = fopen(options, "w")) == NULL) {
		perror(options);
		return -1;
	}
	fprintf(fp,
		"area=%s\r\n"
		"user=%s\r\n"
		"key=%s\r\n",
		nvram_invmatch("wan_heartbeat_x", "") ?
		    nvram_safe_get("wan_heartbeat_x") : "convex",
		nvram_safe_get(strcat_r(prefix, "auth_username", tmp)),
		nvram_safe_get(strcat_r(prefix, "auth_passwd", tmp)));
	fclose(fp);

	/* Start authcli */
	return _eval(authcli_argv, NULL, 0, NULL);
}
#endif /* __CONFIG_CONVEX__ */


int start_auth(const char *prefix, int wan_up)
{
	char tmp[100];
	const char *wan_proto = nvram_safe_get(strcat_r(prefix, "proto", tmp));
	const char *wan_auth = nvram_safe_get(strcat_r(prefix, "auth_x", tmp));
	int ret = 0;

	if (strcmp(wan_proto, "static") == 0 ||
	    strcmp(wan_proto, "dhcp") == 0)
	{
#ifdef __CONFIG_EAPOL__
		if (strcmp(wan_auth, "eap-md5") == 0 && !wan_up)
			ret = start_wpa_supplicant(prefix, 1);
#endif
#ifdef __CONFIG_TELENET__
		if (strcmp(wan_auth, "telenet") == 0 && wan_up)
			ret = start_lanauth(prefix, 1);
#endif
#ifdef __CONFIG_CONVEX__
		if (strcmp(wan_auth, "convex") == 0 && wan_up)
			ret = start_authcli(prefix, 1);
#endif
	}
/* TODO: ugly, remake bigpond as auth, not wan proto */
	if (strcmp(wan_proto, "bigpond") == 0)
	{
		if (wan_up)
		{
			stop_bpalogin();
			ret = start_bpalogin();
		}
	}

	return ret;
}

int stop_auth(const char *prefix, int wan_down)
{
	char tmp[100];
	const char *wan_proto = prefix ? nvram_safe_get(strcat_r(prefix, "proto", tmp)) : NULL;
	const char *wan_auth = prefix ? nvram_safe_get(strcat_r(prefix, "auth_x", tmp)) : NULL;

	if (wan_proto == NULL ||
	    strcmp(wan_proto, "static") == 0 ||
	    strcmp(wan_proto, "dhcp") == 0)
	{
#ifdef __CONFIG_EAPOL__
		if ((!wan_auth || strcmp(wan_auth, "eap-md5") == 0) && !wan_down)
			stop_wpa_supplicant(0);
#endif
#ifdef __CONFIG_TELENET__
		if ((!wan_auth || strcmp(wan_auth, "telenet") == 0) && wan_down)
			stop_lanauth(0);
#endif
#ifdef __CONFIG_CONVEX__
		if ((!wan_auth || strcmp(wan_auth, "convex") == 0) && wan_down)
			stop_authcli(0);
#endif
	}
/* TODO: ugly, remake bigpond as auth, not wan proto */
	if (wan_proto == NULL ||
	    strcmp(wan_proto, "bigpond") == 0)
	{
		if (wan_down)
			stop_bpalogin();
	}

	return 0;
}
