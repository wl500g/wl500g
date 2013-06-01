#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>
#include <errno.h>
#include <ctype.h>
#include <time.h>

#include <netconf.h>
#include <nvparse.h>

#include <dirent.h>

#include "rc.h"

int wait_for_dev_appearance(int vid, int pid, const char *device, const char *device_list[]);
int wait_for_dev_with_drv(int vid, int pid, const char *device, const char *driver_list[], char **found_driver);

int usb_modem_send_to( const char *prefix, int unit, const char *ifname, const char *data, int port_type);

void usbnet_load_drivers(const char *prefix)
{
	char tmp[100], *subtype;
	insmod_cond("usbnet", NULL);
	insmod_cond("cdc_ether", NULL);
	insmod_cond("rndis_host", NULL);

#ifdef __CONFIG_MODEM__
	if ((subtype = nvram_get(strcat_r(prefix, "usbnet_subtype", tmp)))) {
		modem_load_drivers();
		if (nvram_match(strcat_r(prefix, "modem_autodetect", tmp), "1")) {
				insmod_cond("cdc-wdm", NULL);
				insmod_cond("qmi_wwan", NULL);
				insmod_cond("cdc_ncm", NULL);
				insmod_cond("cdc_mbim", NULL);
		} else {
			if (!strcmp(subtype, "qmi")) {
				insmod_cond("cdc-wdm", NULL);
				insmod_cond("qmi_wwan", NULL);

			} else if (!strcmp(subtype, "ncm")) {
				insmod_cond("cdc_ncm", NULL);

			} else if (!strcmp(subtype, "mbim")) {
				insmod_cond("cdc_mbim", NULL);
			}
		}
	}
#endif
};

int hotplug_usbnet_check(const char *interface, const char *product, const char *device, const char *prefix)
{
	int ret = 0;
	char *str1, *str2;
	char tmp[200];
	int vid, pid, hp_vid, hp_pid;

	dprintf("%s: %s - %s\n", prefix, product, device );

	// if device was already found for this wan, do nothing
	str1 = nvram_get(strcat_r(prefix, "usb_device", tmp));
	if (str1 && strncmp(str1, "zerocd", 6) != 0)
		return 0;

	str1 = nvram_safe_get(strcat_r(prefix, "modem_vid", tmp));
	sscanf(str1, "%x", &vid);
	str2 = nvram_safe_get(strcat_r(prefix, "modem_pid", tmp));
	sscanf(str2, "%x", &pid);

	sscanf(product, "%x/%x", &hp_vid, &hp_pid);

	if (!wait_for_dev_appearance(hp_vid, hp_pid, device, NULL)) {
		dprintf("device not found in /proc/bus/usb/devices\n");
		return 0;
	}

	dprintf( "stored: %x:%x, found: %x:%x\n", vid, pid, hp_vid, hp_pid );
	if (vid == hp_vid && pid == hp_pid) {
		ret = 1;
	}

#ifdef __CONFIG_MODEM__
	if (ret && nvram_match(strcat_r(prefix, "modem_autodetect", tmp), "1")) {
		hotplug_check_modem("usbnet", product, device, prefix);
	}
#endif

	dprintf("done. ret %d\n", ret);

	return ret;
}

void usbnet_if_up(char *prefix, int unit, char *ifname) {
	char tmp[100];
	char *wan_ifname;

	nvram_set( strcat_r(prefix, "usb_ifname", tmp), ifname );
	//nvram_set( strcat_r(prefix, "primary", tmp), "1" );

	wan_ifname = nvram_get(strcat_r(prefix, "ifname", tmp));
	if (!wan_ifname)
		return;

	ifconfig(ifname, IFUP, NULL, NULL);
	eval("brctl", "addif", wan_ifname, ifname );

	eval("ifconfig", nvram_safe_get( strcat_r(prefix, "usb_ifname", tmp)),
		"mtu", nvram_safe_get( strcat_r(prefix, "usbnet_mtu", tmp)));

	//connect qmi, ncm, mbim
	usbnet_connect(prefix, unit, ifname);

	//start_wan_unit(unit);

	dprintf("done\n");
}

void usbnet_if_down(char *prefix, int unit, char *ifname) {
	char tmp[100];
	char *wan_ifname;

	wan_ifname = nvram_get(strcat_r(prefix, "ifname", tmp));
	if (!wan_ifname)
		return;

	//stop_wan_unit(unit);

	// usbnet_if_down() is called only on hotplug net remove event
	// interface is already removed at this moment
	//eval("brctl", "delif", wan_ifname, ifname );
	//ifconfig(ifname, 0, NULL, NULL);

	dprintf("done\n");
}

void usbnet_check_and_act(char *ifname, char *action)
{
	FILE *fusb_descr;
	int unit, found = 0, i;
	char prefix[WAN_PREFIX_SZ];
	char	tmp[100], tmp2[200],
		product[50], device[50], product_descr[50], serial[50], 
		*devpath;

	char	csz_prod[] = "PRODUCT=",
		csz_dev[] = "DEVICE=";

	char *tmp_str;

	// search wan by interface
	*tmp = 0;
	*product = *device = *product_descr = *serial = 0;

	if (!strcmp(action, "remove")) {
		for (unit = 0; !found && unit < MAX_NVPARSE; unit++) {
			snprintf(prefix, sizeof(prefix), "wan%d_", unit);
			// ???
			//tmp_str = nvram_safe_get( strcat_r(prefix, "proto", tmp) );
			//if (!strcmp(tmp_str, "usbdev")) continue;

			tmp_str = nvram_safe_get( strcat_r(prefix, "usb_ifname", tmp) );
			if (strcmp(tmp_str, ifname)) continue;

			usbnet_if_down(prefix, unit, ifname);

			dprintf("remove done\n");
			return;
		}
	}

	if (strcmp(action, "add")) return;

	// read device info using ifname from hotplug_net event
	snprintf( tmp, sizeof(tmp), "/sys/class/net/%s/device", ifname );
	dprintf("%s\n", tmp);
	if ((devpath = realpath(tmp, NULL))) {
		// shrink usb-endpoint
		if ((tmp_str = strrchr(devpath, '/'))) *tmp_str = 0;

		// read device information by physical device name
		snprintf( tmp2, sizeof(tmp2), "%s/uevent", devpath );
		dprintf("%s\n",tmp2);
		fusb_descr = fopen( tmp2, "rt" );
		if (fusb_descr) {
			while (!feof(fusb_descr) && !(*product && *device)) {
				fgets(tmp, sizeof(tmp), fusb_descr);
				if((tmp_str = strchr(tmp, '\n'))) *tmp_str = 0;
				if (!strncmp(tmp, csz_prod, sizeof(csz_prod)-1)) {
					strcpy(product, tmp + sizeof(csz_prod) - 1);
				} else if (!strncmp(tmp, csz_dev, sizeof(csz_dev)-1)) {
					strcpy(device, tmp + sizeof(csz_dev) - 1);
				}
			}
		}

		// get product description
		snprintf( tmp2, sizeof(tmp2), "%s/product", devpath );
		dprintf("%s\n", tmp2);
		fgets_ex(tmp2, product_descr, sizeof(product_descr));
		// get serial number
		snprintf( tmp2, sizeof(tmp2), "%s/serial", devpath );
		dprintf("%s\n", tmp2);
		fgets_ex(tmp2, serial, sizeof(serial));
		dprintf("found: %s, %s, %s\n", product_descr, product, serial);

		free(devpath);
	}

	dprintf( "action for %s : %s\n", product, device );

	// checking device type
	if (!strstr(device, "/bus/usb/"))
		return;

	found = 0; i = 0;
	for (unit = 0; !found && unit < MAX_NVPARSE; unit ++) {
		snprintf(prefix, sizeof(prefix), "wan%d_", unit);
		// ???
		//tmp_str = nvram_safe_get( strcat_r(prefix, "proto", tmp) );
		//if (strcmp(tmp_str, "usbdev")) {
			found++;
			i = unit;
			dprintf("wan usbnet %d\n", unit);
		//}
	}
	unit = -1;

	if (found == 1) {
		int vid, pid;
		//found exactly one wan. autodetect device parameters
		unit = i;
		snprintf(prefix, sizeof(prefix), "wan%d_", unit);
		sscanf(product, "%x/%x", &vid, &pid);

		sprintf(tmp2, "0x%04x", vid);
		nvram_set(strcat_r(prefix, "modem_vid", tmp), tmp2);
		sprintf(tmp2, "0x%04x", pid);
		nvram_set(strcat_r(prefix, "modem_pid", tmp), tmp2);
	} else {
		// search device with the same vid/pid/serial
		found = 0;
		for (unit = 0; !found && unit < MAX_NVPARSE; unit++) {
			snprintf(prefix, sizeof(prefix), "wan%d_", unit);
#ifdef _DEBUG
			dprintf("Not realized yet. multiwan todo: change device checking to vid/pid/serial !!!\n");
#endif
			/*tmp_str = nvram_get( strcat_r(prefix, "usb_device", tmp) );

			if (devpath)
				found = (strcmp(tmp_str, tmp2) == 0);
			else
				continue;

			dprintf( "%d compare %s --- %s\n", unit, devpath, tmp2 );*/
			found = 1;
		}
	}

	if (found) {
		snprintf(prefix, sizeof(prefix), "wan%d_", unit);
		dprintf("devfound at %s\n", prefix);

		snprintf( tmp2, sizeof(tmp2), "%s : %s", product, device );
		nvram_set(strcat_r(prefix, "usb_device_name", tmp), product_descr);
		nvram_set(strcat_r(prefix, "usb_device", tmp), tmp2 );
		usbnet_if_up(prefix, unit, ifname);
	}

	dprintf("done\n");
}

#ifdef __CONFIG_MODEM__
void usbnet_get_wdm_by_ifusb(const char *if_usb, char *wdm_name, size_t wdm_size)
{
	DIR           *d;
	struct dirent *dir;
	char tmp[100];

	*wdm_name = 0;

	snprintf( tmp, sizeof(tmp), "/sys/class/net/%s/device/", if_usb );

	if ((d = opendir( tmp ))) {
		while ((dir = readdir(d)) != NULL){
			if (!strncmp(dir->d_name, "usb:cdc-wdm", 11)) {
				// create device name like /dev/cdc-wdm0
				snprintf(wdm_name, wdm_size, "/dev/%s", dir->d_name + 4);
				dprintf("%s\n", dir->d_name);
			}
		}
		closedir(d);
	}
	dprintf("usb: %s    wdm: %s", if_usb, wdm_name);
	dprintf("done\n");
}

void usbnet_qmi_connect(const char *prefix, int unit, const char *ifname)
{
	// uqmi --device=/dev/cdc-wdm0 --keep-client-id wds  --start-network internet
	char tmp[100], s_wdm[100], *ptr, s_APN[100];

	char *argv_uqmi[] = {
		"uqmi",
		s_wdm,
		"--keep-client-id", "wds",
		"--start-network", s_APN,
		NULL
	};

	ptr = nvram_get( strcat_r(prefix, "usb_ifname", tmp));

	if (!ptr) return; // it means that connect started but drivers not initialized yet

	usbnet_get_wdm_by_ifusb(ptr, tmp, sizeof(tmp));
	snprintf(s_wdm, sizeof(s_wdm), "--device=%s", tmp);

	strncpy(s_APN, nvram_safe_get(strcat_r(prefix, "modem_apn", tmp)), sizeof(s_APN)-1);
	s_APN[sizeof(s_APN)-1] = 0;

	_eval(argv_uqmi, ">>/tmp/chat.log", 0, NULL);
	dprintf("done\n");

}

void usbnet_qmi_disconnect(const char *prefix, int unit, const char *ifname)
{
}

void usbnet_at_connect(const char *prefix, int unit, const char *ifname)
{
	char tmp[100], data_str[0xff];

	if (nvram_match(strcat_r(prefix, "usbnet_subtype", tmp), "user")) {
		strncpy(data_str,
			nvram_safe_get(strcat_r(prefix, "modem_at_connect", tmp)),
			sizeof(data_str)-1);
		data_str[sizeof(data_str)-1] = 0;
	} else { // ncm, mbim

		// AT^NDISDUP=1,1,"apn"[,<"username">,<"password">,<auth_type>]
		// echo -e "AT^NDISDUP=1,1,\"internet\"" > /dev/ttyUSB2

		snprintf(data_str, sizeof(data_str),
			"AT^NDISDUP=1,1,\"%s\"", nvram_safe_get(strcat_r(prefix, "modem_apn", tmp)));
		// ignoring user, password, auth
	}

	strcat(data_str, "\n");
	usb_modem_send_to(prefix, unit, ifname, data_str, 1);

	dprintf("done\n");
}

void usbnet_at_disconnect(const char *prefix, int unit, const char *ifname)
{
	char tmp[100], data_str[0xff];

	if (nvram_match(strcat_r(prefix, "usbnet_subtype", tmp), "user")) {
		strncpy(data_str,
			nvram_safe_get(strcat_r(prefix, "modem_at_connect", tmp)),
			sizeof(data_str)-1);
		data_str[sizeof(data_str)-1] = 0;
	} else { // ncm, mbim
		strcpy( data_str, "AT^NDISDUP=1,0");
	}

	strcat(data_str, "\n");
	usb_modem_send_to(prefix, unit, ifname, data_str, 1);

	dprintf("done\n");
}
#endif

void usbnet_connect(const char *prefix, int unit, const char *ifname)
{
#ifdef __CONFIG_MODEM__
	char tmp[100], *subtype, *ptr;

	// don`t connect from boot. Allow connection from rc_service.
	if (getpid() == 1 && nvram_get("rc_service") == NULL) return;

	if (nvram_match(strcat_r(prefix, "modem_autodetect", tmp), "1")) {
		int vid, pid;
		// Internal drivers names. Not *.ko!
		const char *driver_list[] = {"qmi_wwan", "cdc_ncm", "cdc_mbim", NULL};
		char *found_driver, *device, drv_type[20];

		device = nvram_safe_get(strcat_r(prefix, "usb_device", tmp));

		ptr = nvram_safe_get(strcat_r(prefix, "modem_vid", tmp));
		sscanf(ptr, "%x", &vid);
		ptr = nvram_safe_get(strcat_r(prefix, "modem_pid", tmp));
		sscanf(ptr, "%x", &pid);

		dprintf("detecting modem subtype vid: %x, pid: %x, device: %s", vid, pid, device);

		*drv_type = 0;
		if (wait_for_dev_with_drv(vid, pid, device, driver_list, &found_driver)) {
			dprintf("found driver: %s", found_driver);

			if (strstr(found_driver, "qmi")) strcpy(drv_type, "qmi");
			else if (strstr(found_driver, "ncm")) strcpy(drv_type, "ncm");
			else if (strstr(found_driver, "mbim")) strcpy(drv_type, "mbim");
		}
		nvram_set(strcat_r(prefix, "usbnet_subtype", tmp), drv_type);
	}

	if ((subtype = nvram_safe_get(strcat_r(prefix, "usbnet_subtype", tmp)))) {

		if ( getpid() != 1 ) sleep(5); // awaiting device from hotplug

		dprintf("modem subtype: %s\n", subtype);
		if (!strcmp(subtype, "ncm") || !strcmp(subtype, "mbim") ||
		    !strcmp(subtype, "user"))
			usbnet_at_connect(prefix, unit, ifname);
		else if (!strcmp(subtype, "qmi"))
			usbnet_qmi_connect(prefix, unit, ifname);
	}
#endif

	dprintf("done\n");
}

void usbnet_disconnect(const char *prefix, int unit, const char *ifname)
{
#ifdef __CONFIG_MODEM__
	char tmp[100], *subtype;
	if ((subtype = nvram_safe_get(strcat_r(prefix, "usbnet_subtype", tmp)))) {
		dprintf("modem subtype %s:\n", subtype);
		if (!strcmp(subtype, "ncm") || !strcmp(subtype, "mbim") ||
		    !strcmp(subtype, "user")) {
			usbnet_at_disconnect(prefix, unit, ifname);
			sleep(1);
		}
	}
#endif
	dprintf("done\n");
}
