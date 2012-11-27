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
#include "rc.h"

int wait_for_dev_appearance(int vid, int pid, const char *device, const char *device_list[]);

void usbnet_load_drivers()
{
	if (!exists("/sys/module/usbnet")) insmod("usbnet", NULL);
	if (!exists("/sys/module/cdc_ether")) insmod("cdc_ether", NULL);
	if (!exists("/sys/module/rndis_host")) insmod("rndis_host", NULL);
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
	char prefix[sizeof("wanXXXXXXXXXX_")];
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
