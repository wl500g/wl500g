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


#define MODEM_DEVICES_FILE	"/proc/bus/usb/devices"
#define MODEM_CONFIG_FILE	"/usr/share/modem/modems.conf"
#define MODEM_DIAL_SCRIPT	"/usr/ppp/dial"
#define MODEM_DIAL_PIDFILE	"/var/run/dial%s.pid"

#define TYPE_NO		'N'
#define TYPE_CDMA	'C'
#define TYPE_WCDMA	'W'
#define TYPE_HUB	'H'
#define TYPE_MEM	'M'
#define TYPE_ZEROCD	'Z'

// see http://www.usb.org/developers/defined_class
#define CLASS_ICLS	0x00
#define CLASS_ACM	0x02
#define CLASS_MEM	0x08
#define CLASS_HUB	0x09
#define CLASS_MISC	0xEF
#define CLASS_MOD	0xFF

#define BUF_LEN		512
#define MODEM_MAX_LOCATION_LEN	32

#define FORMAT_SHORT	1
#define FORMAT_JSON	2

#define nvram_prefix_get(name) nvram_safe_get(strcat_r(prefix, (name), tmp))


#ifdef RC_SEMAPHORE_ENABLED
int hotplug_check_prev_zerocd_processed(const char *product, const char *device)
{
	char tmp[100], prefix[sizeof("wanXXXXXXXXXX_")];
	char str_devusb[100];
	int found = 0, unit;
	char *dev_vidpid;

	snprintf(str_devusb, sizeof(str_devusb), "zerocd: %s : %s", product, device);

	hotplug_sem_open();
	hotplug_sem_lock();

	// prevent multiple processing of the same device
	for (unit = 0; !found && unit < MAX_NVPARSE; unit ++)
	{
		snprintf(prefix, sizeof(prefix), "wan%d_", unit);

		dev_vidpid = nvram_get( strcat_r(prefix, "usb_device", tmp) );
		if (dev_vidpid)
			found = (strcmp(dev_vidpid, str_devusb) == 0);
#ifdef _DEBUG
		if (found) dprintf("already processed\n");
#endif
	}

	if (!found ) for (unit = 0; unit < MAX_NVPARSE; unit ++) {
		snprintf(prefix, sizeof(prefix), "wan%d_", unit);

		dev_vidpid = nvram_get( strcat_r(prefix, "usb_device", tmp) );
		if (!dev_vidpid) {
			dprintf("set: %s, %s\n", prefix, str_devusb);
			nvram_set( strcat_r(prefix, "usb_device", tmp), str_devusb );
			break;
		}
	}

	hotplug_sem_unlock();
	hotplug_sem_close();

	if (found) return 1;
	else return 0;
}

void hotplug_release_zerocd_processed(const char *product, const char *device)
{
	char tmp[100], prefix[sizeof("wanXXXXXXXXXX_")];
	char str_devusb[100];
	int unit;
	char *dev_vidpid;

	snprintf(str_devusb, sizeof(str_devusb), "zerocd: %s : %s", product, device);

	hotplug_sem_open();
	hotplug_sem_lock();

	// prevent multiple processing of the same device
	for (unit = 0; unit < MAX_NVPARSE; unit ++)
	{
		snprintf(prefix, sizeof(prefix), "wan%d_", unit);

		dev_vidpid = nvram_get( strcat_r(prefix, "usb_device", tmp) );
		if (dev_vidpid && strcmp(dev_vidpid, str_devusb) == 0) {
			nvram_unset( strcat_r(prefix, "usb_device", tmp) );
			break;
		}
	}

	hotplug_sem_unlock();
	hotplug_sem_close();
	dprintf("done: %s\n", device );
}
#else
#define hotplug_check_prev_zerocd_processed(product,device) 1
#define hotplug_release_zerocd_processed(product,device)
#endif /* RC_SEMAPHORE_ENABLED */

/// Modems config file element
struct dev_conf {
	int vid;	///< vendor id
	int pid;	///< product id
	char type;	///< modem type - 'C' for CDMA or 'W' for WCDMA/HSPA/HSDPA/HSUPA/GSM
	int data_port;	///< main port for data communication number
	int ui_port;	///< user interface port for modem controlling, status sms, ussd and so on
	struct dev_conf *next;
};
typedef struct dev_conf dev_conf_t;

/// USB interface list element
struct ifs_usb_elem {
	int number;	///< interface number (used for data and ui ports)
	int endpoints;	///< number of endpoints (used for data port definition if no config exist)
	int cls;	///< interface class (used if device class == 0)
	int subclass;	///< interface subclass
	struct ifs_usb_elem *next;
};
typedef struct ifs_usb_elem ifs_usb_elem_t;

/// USB device list element
struct dev_usb {
	int bus;	///< bus number
	int parent;	///< parent device (hub) number
	int port;	///< connection port on the parent hub
	int number;	///< this device number
	int cls;	///< device class
	int sub;	///< device subclass
	int prot;	///< protocol type
	int vid;	///< vendor id
	int pid;	///< product id
	char *manuf;	///< manufacturer name
	char *prod;	///< product name
	int num_ifs;	///< number of interfaces in device
	ifs_usb_elem_t *ifs;	///< list of interfaces
	int data_port;	///< main data communication port number
	int ui_port;	///< user interface port for modem controlling, status sms, ussd and so on
	char type;	///< modem type - 'C' for CDMA or 'W' for WCDMA/HSPA/HSDPA/HSUPA/GSM
	char loc[MODEM_MAX_LOCATION_LEN];	///< device location string as 1.2.4
	struct dev_usb *next;
	struct dev_usb *prev;
};
typedef struct dev_usb dev_usb_t;

/// read decimal parameter with name 'pname' from vbuf
#define get_int_par(vbuf, pname)	get_numeric_par(vbuf, pname, 10, NULL)
/// read hex parameter with name 'pname' from vbuf
#define get_hex_par(vbuf, pname)	get_numeric_par(vbuf, pname, 16, NULL)

static void modem_get_script_name(char *filename)
{
	const char *script_fn = nvram_get("wan_modem_scriptfn");

	if (script_fn && *script_fn)
		strcpy(filename, script_fn);
	else
		strcpy(filename, MODEM_DIAL_SCRIPT);
};

/// read string parameter with name 'pname' from vbuf
static char *get_str_par(char *vbuf, const char *pname)
{
	char *ptr;

	if ((ptr = strstr(vbuf, pname)) == NULL)
		return NULL;
	ptr += strlen(pname);
	return strdup(ptr);
}

/// read numeric parameter with name 'pname' from vbuf
static int get_numeric_par(char *vbuf, const char *pname, int base, char **num_str)
{
	char *ptr;

	if ((ptr = strstr(vbuf, pname)) == NULL)
		return -1;
	ptr += strlen(pname);
	if (num_str)
		*num_str = ptr;
	return strtol(ptr, NULL, base);
}

/// get list of usb devices from driver
static dev_usb_t *get_usb_list(const char *fn_dev)
{
	FILE *fp = NULL;
	char *vbuf = NULL, *ptr;
	dev_usb_t *dev = NULL;
	dev_usb_t *last = NULL;
	dev_usb_t *list = NULL;
	ifs_usb_elem_t *i = NULL, *l = NULL;
	int skip = 0;

	if ((vbuf = malloc(BUF_LEN)) == NULL) return NULL;

	if ((fp = fopen(fn_dev, "rt")) == NULL) {
		free(vbuf);
		return NULL;
	}

	//  Citation of kernel/Documentation/usb/proc_usb_info.txt
	//
	//  Each line is tagged with a one-character ID for that line:
	//
	//  T = Topology (etc.)
	//  B = Bandwidth (applies only to USB host controllers, which are
	//      virtualized as root hubs)
	//  D = Device descriptor info.
	//  P = Product ID info. (from Device descriptor, but they won't fit
	//      together on one line)
	//  S = String descriptors.
	//  C = Configuration descriptor info. (* = active configuration)
	//  I = Interface descriptor info.
	//  E = Endpoint descriptor info.
	while (fgets(vbuf, BUF_LEN, fp) != 0) {
		if ((ptr = strchr(vbuf,'\n')))
			*ptr = 0;

		if (skip == 1 && (*vbuf != 'I' || *vbuf != 'E'))
			continue;
		else
			skip = 0;

		if (dev == NULL && *vbuf != 'T')
			continue;

//		dprintf("%s", vbuf);

		switch (*vbuf) {
			case 'T':
				if ( (dev = malloc(sizeof(dev_usb_t))) == NULL ||
					memset(dev,0,sizeof(dev_usb_t)) == NULL) {
					list = NULL;
					goto out;
				}

				dev->type = TYPE_NO;
				dev->bus = get_int_par(vbuf, "Bus=");
				dev->parent = get_int_par(vbuf, "Prnt=");
				dev->port = get_int_par(vbuf, "Port=");
				dev->number = get_int_par(vbuf, "Dev#=");

				if (list == NULL) {
					list = dev;
				} else {
					last->next = dev;
					dev->prev = last;
				}
				last = dev;
				break;
			case 'D':
				dev->cls = get_hex_par(vbuf, "Cls=");
				dev->sub = get_hex_par(vbuf, "Sub=");
				dev->prot = get_hex_par(vbuf, "Prot=");
				break;
			case 'P':
				dev->vid = get_hex_par(vbuf, "Vendor=");
				dev->pid = get_hex_par(vbuf, "ProdID=");
				break;
			case 'S':
				if (dev->manuf == NULL)
					dev->manuf = get_str_par(vbuf, "Manufacturer=");
				if (dev->prod == NULL)
					dev->prod = get_str_par(vbuf, "Product=");
				break;
			case 'C':
				if (vbuf[2] == '*') { //default configuration
					dev->num_ifs = get_int_par(vbuf, "#Ifs=");
				} else {
					skip = 1;
				}
				break;
			case 'I':
				if ((i = malloc(sizeof(ifs_usb_elem_t))) == NULL) {
					list = NULL;
					goto out;
				}
				i->number = get_int_par(vbuf, "If#=");
				i->endpoints = get_int_par(vbuf, "#EPs=");
				i->cls = get_hex_par(vbuf, "Cls=");
				i->subclass = get_hex_par(vbuf, "Sub=");
				i->next = NULL;
				if (dev->ifs == NULL) {
					dev->ifs = i;
				} else {
					if (l) {
						l->next = i;
					}
				}
				l = i;
				break;
			case 'B': 
				break;
			default:
				break;
		}
	}

out:
	free(vbuf);
	fclose(fp);
	return list;
}

/// get type and location for each device and count modems
/// with vid:pid (all if 0000:0000)
static int search_modems_in_list(dev_usb_t *list, int vid, int pid)
{
	dev_usb_t *dev = list, *tmp;
	ifs_usb_elem_t *i;
	int count = 0;

	while (dev) {
		// get location
		if (dev->parent != 0) {
			tmp = dev->prev;
			while (tmp && tmp->number != dev->parent) {
				tmp = tmp->prev;
			}
			if (tmp) {
				if (tmp->loc[0])
					sprintf(dev->loc,"%s.%d",tmp->loc,dev->port+1);
				else
					sprintf(dev->loc,"%d",dev->port+1);
			}
		}
		dprintf("dev:%04x:%04x\tprod:%s\tloc:%s\tcls:%x\tsub:%x\tprot:%x\n",
			dev->vid,dev->pid,dev->prod, dev->loc, dev->cls, dev->sub, dev->prot);
		// get type
		switch (dev->cls) {
			case CLASS_HUB:
				dev->type = TYPE_HUB;
				break;
			case CLASS_MEM:
				dev->type = TYPE_MEM;
				break;
			case CLASS_ACM:
				dev->type = TYPE_WCDMA;
				if (vid || pid) {
					if (dev->vid == vid && dev->pid == pid)
						count++;
				} else {
					count++;
				}
				break;
			case CLASS_MISC:
				// checking for Interface Association Descriptor ECN
				if (!(dev->sub == 0x02 && dev->prot == 0x01))
					break;
			case CLASS_ICLS:
				// class in IFACE
				for (i = dev->ifs; i; i = i->next) {
					if (i->cls == CLASS_MOD || i->cls == CLASS_ACM) {
						dev->type = TYPE_WCDMA;
						if (vid || pid) {
							if (dev->vid == vid && dev->pid == pid)
							count++;
						} else {
							count++;
						}

						// empirical prediction of the port type!
						// The data port typically contains 3 EndPoints
						// one of which has interface with the
						// Interrupt transfers mode.
						if (i->endpoints > 2)
							dev->data_port = i->number;
						else
							dev->ui_port = i->number;
					} else {
						if (i->cls == CLASS_MEM && 
							dev->type == TYPE_NO)
							dev->type = TYPE_MEM;
					}
				}
		}
		dev = dev->next;
	}
	return count;
}

/// read info from config file
static dev_conf_t *get_config_list(const char *fn_conf) 
{
	char *ptr;
	char *vbuf = NULL;
	dev_conf_t *conf, *lconf = NULL, *conf_list = NULL;
	FILE *fp;

	if ((vbuf = malloc(BUF_LEN)) == NULL)
		return NULL;

	fp = fopen(fn_conf, "rt");
	if (fp) {
		while (fgets(vbuf, BUF_LEN, fp) != 0) {
			ptr = vbuf;
			while (*ptr == ' ' || *ptr == '\t')
				ptr++;

			if (*ptr == '#' || *ptr == '\n' || *ptr == '\r' || *ptr == '\0')
				continue;

			if ((conf = malloc(sizeof(dev_conf_t))) == NULL)
				return NULL;
			sscanf(ptr, "%04x:%04x:%c:%d:%d",
				&conf->vid, &conf->pid, &conf->type, &conf->data_port, &conf->ui_port);
			conf->next = NULL;
			if (conf_list == NULL) {
				conf_list = conf;
			} else {
				if (lconf) {
					lconf->next = conf;
				}
			}
			lconf = conf;
		}
		fclose(fp);
	}
	free(vbuf);
	return conf_list;
}

/// check info in config file
static int check_config(dev_usb_t *dev, dev_conf_t *conf_list)
{
	dev_conf_t *conf = conf_list;

	while (conf) {
		if (conf->vid == dev->vid && conf->pid == dev->pid) {
			dev->type = conf->type;
			dev->data_port = conf->data_port;
			dev->ui_port = conf->ui_port;
			return 1;
		}
		conf = conf->next;
	}
	return 0;
}

/// print info about modems
static void print_modem_info(int format, dev_usb_t *dev_list)
{
	dev_usb_t *dev;

	if (format == FORMAT_JSON) puts("[");

	for (dev = dev_list; dev; dev = dev->next) {
		if (dev->type == TYPE_CDMA || dev->type == TYPE_WCDMA)
			switch (format) {
			case FORMAT_SHORT:
				printf("%s:%s:%s\n",
					dev->loc, dev->manuf, dev->prod);
				break;

			case FORMAT_JSON:
				printf(	"{'wan':'0','vid':'%04x','pid':'%04x','type':'%c',"
					"'data':'%d','ui':'%d','loc':'%s','manuf':'%s','prod':'%s'},\n",
					dev->vid, dev->pid, dev->type, dev->data_port, dev->ui_port,
					dev->loc, dev->manuf, dev->prod);
				break;

			default:
				printf("0:%04x:%04x:%c:%d:%d:%s:%s:%s\n",
					dev->vid, dev->pid, dev->type, dev->data_port, dev->ui_port,
					dev->loc, dev->manuf, dev->prod);
		}
	}
	if (format == FORMAT_JSON) puts("]");
}

/// free memory from conf_list
void free_conf_list(dev_conf_t *conf_list)
{
	dev_conf_t *tconf,
		 *conf = conf_list;

	while (conf) {
		tconf = conf->next;
		free(conf);
		conf = tconf;
	}
}

/// free memory from dev_usb_t list
void free_dev_list(dev_usb_t *list)
{
	dev_usb_t *dev = list, *tmp;
	ifs_usb_elem_t *t;

	while (dev) {
		free(dev->manuf);
		free(dev->prod);
		while (dev->ifs) {
			t = dev->ifs->next;
			free(dev->ifs);
			dev->ifs = t;
		}
		tmp = dev->next;
		free(dev);
		dev = tmp;
	}
}

/// get usb-device number and usb-bus from environment DEVICE
int get_bus_num_from_devstr(const char *device, int *bus, int *num)
{
	char *ptr;
	int result = 0;

	ptr = strrchr (device, '/');
	if (ptr > device) {
		*num = atoi(ptr+1);
		ptr--;
		while (ptr >= device && *ptr != '/')
			ptr--;

		if (ptr > device) {
			*bus = atoi(ptr+1);

			dprintf("device: %s\t%d\t%d\n", device, *bus, *num);

			result = 1;
		}
	}
	
	return result;
}

/// get usb-device by environment DEVICE
static dev_usb_t *get_usb_by_location_env(const char *device, dev_usb_t *list)
{
	int bus, dev_num;
	dev_usb_t *usb_dev, *result;

	result = NULL;

	dprintf("%s\n",device);
	if (get_bus_num_from_devstr(device, &bus, &dev_num)) {
		for (usb_dev = list; usb_dev; usb_dev=usb_dev->next) {
			dprintf("%x/%x	%s	%d/%d\n", 
				usb_dev->vid, usb_dev->pid, usb_dev->loc,
				usb_dev->bus, usb_dev->number);

			if (usb_dev->bus == bus && usb_dev->number == dev_num &&
				(usb_dev->type == 'W' || usb_dev->type == 'C')) {
				result = usb_dev;
				break;
			}
		}
	}
	dprintf("done\n");
	return result;
}

int start_modem_dial(const char *prefix)
{
	int ret = 0;
	pid_t pid;
	FILE * file;
	char sfn[200], tmp[200];
	char *dial_argv[] = {
		(modem_get_script_name(sfn), sfn),
		(char *)prefix,
		NULL
	};

	if (nvram_match(strcat_r(prefix, "dial_enabled", tmp), "1") &&
		nvram_match(strcat_r(prefix, "prepared", tmp), "1") &&
		nvram_get(strcat_r(prefix, "usb_device", tmp))) {
		dprintf("%s\n", sfn);

		if (nvram_match(strcat_r(prefix, "proto", tmp), "usbmodem")) {
			nvram_set(strcat_r(prefix, "ifname", tmp) , nvram_safe_get(strcat_r(prefix, "pppoe_ifname", tmp)));
			nvram_set(strcat_r(prefix, "dnsenable_x", tmp), "1");
		}
		nvram_set("wan_status_t", "Connecting...");
		ret = _eval(dial_argv, NULL, 0, &pid);
		if (pid) {
			sprintf(tmp, MODEM_DIAL_PIDFILE, prefix);
			file = fopen(tmp, "w");
			if (file) {
				fprintf(file, "%d", pid);
				fclose(file);
			}
		}
	}

	dprintf("done\n");
	return ret;
}

int stop_modem_dial(const char *prefix)
{
	int ret;
	char tmp[100];
	int unit;


	unit = nvram_get_int(strcat_r(prefix, "unit", tmp));
	ret = 0;

	sprintf(tmp, MODEM_DIAL_PIDFILE, prefix);
	dprintf("kill %s\n", tmp);
	kill_pidfile(tmp);
	unlink(tmp);

	sprintf(tmp, "/var/run/ppp%d.pid", unit);
	dprintf("kill %s\n", tmp);
	kill_pidfile(tmp);
	//		nvram_set("wan0_dial_enabled", "0");

	dprintf("%s done\n", prefix);
	return ret;
}

int parse_product_string(const char *product, int *vid, int *pid)
{
	int res = 0;
	char tmp[512], *str1, *str2;
	*vid = 0; *pid = 0;

	strncpy(tmp, product, sizeof(tmp)-1);
	tmp[sizeof(tmp)-1] = '\0';
	str1 = strchr(tmp, '/');
	if (str1) {
		*str1 = 0; str1++;
		str2 = strchr(str1, '/');
		if (str2)
			*str2 = '\0';
		sscanf(tmp, "%x", vid);
		sscanf(str1, "%x", pid);
		res = 1;
	}
	return res;
}


/// wait for device appearance in /proc/bus/usb/devices
int wait_for_dev_appearance(int vid, int pid, const char *device, const char *driver_list[])
{
	FILE *fp = NULL;
	char *vbuf = NULL, *ptr, **ptr2;
	dev_usb_t dev;
	int result, rep_counter, dev_bus, dev_num;

	const int max_time = 10;  //max wait in seconds
	const int rep_time = 100; //iteration time in milliseconds

	result = 0;
	rep_counter = max_time*1000/rep_time;

	if (!get_bus_num_from_devstr(device, &dev_bus, &dev_num) ||
		(vbuf = malloc(BUF_LEN)) == NULL) return result;

	for (;!result && rep_counter && !usleep(rep_time*1000) && 
		(fp = fopen(MODEM_DEVICES_FILE, "rt")); rep_counter--) {

		memset(&dev, 0, sizeof(dev));
		while (fgets (vbuf, BUF_LEN, fp) != 0 && result == 0) {
			switch (*vbuf) {
			case 'T':{
				memset(&dev, 0, sizeof(dev));
				dev.bus = get_numeric_par(vbuf, "Bus=", 10, &ptr);
				dev.number = get_int_par(ptr, "Dev#=");
				break;
			} 
			case 'P':{
				dev.vid = get_numeric_par(vbuf, "Vendor=", 16, &ptr);
				dev.pid = get_hex_par(ptr, "ProdID=");

				if (dev.vid == vid && dev.pid == pid && 
					dev.number == dev_num && dev.bus == dev_bus) {
					if (!driver_list) result = 1;
				}
				break;
			}
			case 'I':
				if (!driver_list || dev.vid != vid || dev.pid != pid ) break;

				if ((ptr = strstr(vbuf, "Driver="))) {
					ptr += sizeof("Driver=")-1;
					for (ptr2 = (char**)driver_list; *ptr2; ptr2++) {
						if (!strncmp(ptr, *ptr2, strlen(*ptr2))) {
							result = 1;
							break;
						}
					}
				}
				break;
			}
		}
		fclose (fp);
	}
#ifdef DEBUG
	dprintf("rep_counter %d\n", max_time*1000/rep_time-rep_counter);
#endif
	free (vbuf);

	return result;
}

int hotplug_check_modem(const char *interface, const char *product, const char *device, const char *prefix)
{
	int ret = 0;
	char *str1, *str2;
	char tmp[200];
	char stored_product[40];
	int vid, pid, hp_vid, hp_pid;
	dev_usb_t *list, *found_dev;
	dev_conf_t *conf_list;

	int autodetect = nvram_match(strcat_r(prefix, "modem_autodetect", tmp), "1");
	//int autodetect = nvram_match("wan_modem_autodetect", "1");

	// if device was already found for this wan, do nothing
	if (nvram_get(strcat_r(prefix, "usb_device", tmp))) return 0;

	str1 = nvram_prefix_get("modem_vid");
	//str1 = nvram_safe_get("wan_modem_vid");
	sscanf(str1, "%x", &vid);
	str2 = nvram_prefix_get("modem_pid");
	//str2 = nvram_safe_get("wan_modem_pid");
	sscanf(str2, "%x", &pid);

	dprintf("stored: %x/%x, found: %s, autodetect: %d\n", vid, pid, product, autodetect);

	sscanf(product, "%x/%x", &hp_vid, &hp_pid);

	if (!wait_for_dev_appearance(hp_vid, hp_pid, device, NULL)) {
		dprintf("device not found in /proc/bus/usb/devices\n");
		return 0;
	}

	found_dev = NULL;

	list = get_usb_list(MODEM_DEVICES_FILE);
	if (autodetect == 0) {
		if (vid == hp_vid && pid == hp_pid &&
			search_modems_in_list(list, 0, 0) > 0) {
				str1 = nvram_prefix_get("modem_usbloc");
				//str1 = nvram_safe_get("wan_modem_usbloc");
				found_dev = get_usb_by_location_env(device, list);
#ifdef DEBUG
				if (found_dev)
				    dprintf("found loc=%s, vid=%04x, pid=%04x\n", found_dev->loc, found_dev->vid, found_dev->pid);
#endif
				if (found_dev && strcmp(str1, found_dev->loc) == 0 &&
					found_dev->vid == vid && found_dev->pid == pid)
					ret = 1;
		}
	} else {
		if (search_modems_in_list(list, 0, 0) > 0) {
			dprintf("get_config_list\n");
			conf_list = get_config_list(MODEM_CONFIG_FILE);
			for (found_dev = list; found_dev; found_dev = found_dev->next) {
				check_config(found_dev, conf_list);
			}
			free_conf_list(conf_list);

			if ((found_dev = get_usb_by_location_env(device, list))) {
				sprintf(stored_product, "0x%04x", found_dev->vid);
				nvram_set(strcat_r(prefix, "modem_vid", tmp), stored_product);
				//nvram_set("wan_modem_vid", stored_product);
				sprintf(stored_product, "0x%04x", found_dev->pid);
				nvram_set(strcat_r(prefix, "modem_pid", tmp), stored_product);
				//nvram_set("wan_modem_pid", stored_product);

				strcpy(stored_product, found_dev->loc);
				nvram_set(strcat_r(prefix, "modem_usbloc", tmp), stored_product);
				//nvram_set("wan_modem_usbloc", stored_product);

				sprintf(stored_product, "%d", found_dev->data_port);
				nvram_set(strcat_r(prefix, "modem_pdata", tmp), stored_product);
				//nvram_set("wan_modem_pdata", stored_product);

				sprintf(stored_product, "%d", found_dev->ui_port);
				nvram_set(strcat_r(prefix, "modem_pui", tmp), stored_product);
				//nvram_set("wan_modem_pui", stored_product);
				
				sprintf(stored_product, "%c", found_dev->type);
				nvram_set(strcat_r(prefix, "modem_type", tmp), stored_product);
				//nvram_set("wan_modem_type", stored_product);

				dprintf("wrote: %04x:%04x USB:%s\n", found_dev->vid, found_dev->pid, found_dev->loc);
				ret = 1;
			}
		}
	}

	if (ret && found_dev) {
		nvram_set(strcat_r(prefix, "usb_device_name", tmp), found_dev->prod);
		if (!exists("/sys/module/usbserial")) insmod("usbserial", NULL);
#ifndef LINUX26
		insmod("acm", NULL);
#else
		if (!exists("/sys/module/cdc-acm")) insmod("cdc-acm", NULL);
#endif
		if (!exists("/sys/module/option")) insmod("option", NULL);

		const char *driver_list[] = {"option", "cdc-acm", NULL};

		if (!wait_for_dev_appearance(found_dev->vid, found_dev->pid, device, driver_list)) {
			dprintf("Found usbmodem %04X:%04X without driver. Loading 'option'\n",
				found_dev->vid, found_dev->pid);
			FILE * fnew_id = fopen("/sys/bus/usb-serial/drivers/option1/new_id", "a");

			if (fnew_id) {
				fprintf(fnew_id, "0x%x 0x%x\n", found_dev->vid, found_dev->pid);
				fclose(fnew_id);
			} else {
				dprintf( "Option file open error: %s\n", strerror(errno) );
			}
		}
	}

	free_dev_list(list);

	dprintf("done. ret %d\n", ret);

	return ret;
}

void
hotplug_exec_user_modem_init_script(const char *sVid, const char *sPid, const char *sDevice)
{
	char **argv_user;
	char *tok, *context;
	const char *delim = " ";
	int i;
	char tmp[256];

	tok = nvram_get("wan_modem_init_script");
	if (tok && *tok) {
		strncpy(tmp, tok, sizeof(tmp) - 1);
		dprintf("prepare to execute %s\n", tmp);

		argv_user = (char **)malloc((strlen(tmp)/2) * sizeof(char*));
		tok = strtok_r(tmp, delim, &context);
		for (i = 0; tok ; i++, (tok = strtok_r(NULL, delim, &context))) {
			if (!strcasecmp(tok, "$VID"))
				argv_user[i] = (char *)sVid;
			else if (!strcasecmp(tok, "$PID"))
				argv_user[i] = (char *)sPid;
			else if (!strcasecmp(tok, "$DEV"))
				argv_user[i] = (char *)sDevice;
			else
				argv_user[i] = tok;
		};
		argv_user[i] = NULL;
		_eval(argv_user, NULL, 0, NULL);
		free((void*)argv_user);
		dprintf("%s executed\n", tmp);
	}
}

void hotplug_usb_modeswitch(const char *interface, const char *action, const char *product, const char *device)
{
	int vid, pid;

	int dev_vid, dev_pid;
	int i = 0;
	char sFileName[128];
	char sPath[128] = "";
	char sVid[0x0F] = "";
	char sPid[0x0F] = "";

#define CNT_VE_STR 2
	char *sMaList[] = {
		// names for sVe=...
		"Option",
		"GT",

		// names for uMa=...
		"AnyDATA",
		"DGT",
		"SAMSUNG",
		"Vertex",
		"SSE",
		"Philips",
		"Wisue"
	};

	char *argv_usb_modeswitch[] = {
		"usb_modeswitch",
#ifdef DEBUG
		"-W",
#endif
		"-D",
		"-v",
		sVid,
		"-p",
		sPid,
		"-c",
		sPath,
		NULL
	};

	if (strcmp(action, "add") == 0 && !hotplug_check_prev_zerocd_processed(product,device)) {

		if (parse_product_string(product, &vid, &pid)) {
			sprintf(sVid, "0x%04x", vid);
			sprintf(sPid, "0x%04x", pid);
		}

		if (nvram_match("wan_modem_zerocd_mode", "UserDefined")) {
			strcpy(sPath, "/usr/local/etc/usb_modeswitch.conf");
		} else if (nvram_match("wan_modem_zerocd_mode", "Auto")) {
			if ( *sVid && *sPid ){
				sprintf(sFileName, "%04x:%04x", vid, pid);
				if ((vid == 0x05c6 && pid == 0x1000) ||
					(vid == 0x1a8d && pid == 0x1000) ||
					(vid == 0x0471 && pid == 0x1210))
				{
					FILE *file;
					char str[255];
					int i_size;
					char * str1;
					int added = 0;

					file = fopen("/proc/bus/usb/devices", "rt");
					if (file) {
						int ready = 0;
						char *templ[] = {
							"Vendor=",
							"ProdID=",
							"Manufacturer=",
							"Product="};
						char sManufacturer[0xFF];

						while (!feof(file)) {
							fgets(str, sizeof(str)-1, file);
							if ((str1 = strstr(str, templ[0]))) {
								sscanf(str1+strlen(templ[0]), "%x", &dev_vid);
								if ((str1 = strstr(str1, templ[1]))) {
									sscanf(str1+strlen(templ[1]), "%x", &dev_pid);
								}
								ready = 0;
							} else if ((str1 = strstr(str, templ[2]))) {
								strncpy(sManufacturer, str1+strlen(templ[2]), sizeof(sManufacturer)-1);
								ready = 1;
							} else if ((str1 = strstr(str, templ[3]))) {
								strncpy(sManufacturer, str1+strlen(templ[3]), sizeof(sManufacturer)-1);
								ready = 2;
							}

							if (ready) {
								dprintf("lsUSB: %04x:%04x\n", dev_vid, dev_pid);
								if (dev_vid == vid && dev_pid == pid) {
									i_size = ARRAY_SIZE(sMaList);
									if (ready == 1) {
										for (i = 0; i < i_size; i++) {
											if (strncmp(sManufacturer, sMaList[i], strlen(sMaList[i])) == 0) {
												if (i < CNT_VE_STR) strcat (sFileName, ":sVe=");
												else strcat (sFileName, ":uMa=");

												strcat(sFileName, sMaList[i]);
												added = 1;
												break;
											}
										}
									} else {
										if (strncmp(sManufacturer, "5G", 2) == 0) {
											strcat (sFileName, ":uPr=5G");
											added = 1;
											break;
										}
									}
								}
								ready = 0;
							}
						}
						fclose(file);
					}
					if (added == 0 && vid == 0x05c6)
						strcat (sFileName, ":uMa=AnyDATA");
				}
			};
			sprintf(sPath, "/usr/share/usb_modeswitch.d/%s", sFileName);
		}

		if (*sPath) {
			dprintf("Check usb_modeswitch data file %s\n",sPath);
			if (exists(sPath)) {
				dprintf("usb_modeswitch -c %s\n", sPath);
				_eval(argv_usb_modeswitch, ">>/tmp/chat.log", 0, NULL);
			}
		}

		//******* exec user script ******************
		hotplug_exec_user_modem_init_script(sVid, sPid, device);

	} else if(strcmp(action, "remove") == 0 ) {
		hotplug_release_zerocd_processed(product, device);
	}
	dprintf("done\n");
}

int usb_modem_check(const char *prefix)
{
	int ret = 0;
	char tmp[100];

	if (nvram_match(strcat_r(prefix, "dial_enabled", tmp), "1") &&
		nvram_match(strcat_r(prefix, "prepared", tmp), "1") &&
		nvram_get(strcat_r(prefix, "usb_device", tmp)))
	{
		sprintf(tmp, MODEM_DIAL_PIDFILE, prefix);
		if (!proc_check_pid(tmp))
			ret = start_modem_dial(prefix);
	}
	return ret;
}

/// list modems ( see format in usbmodem.h )
/// usage: lsmodem [-s] [-c config]
///   -s  short form listing ( : location : manufacturer : product : )
///   -c  custom configuration file ( see format in MODEM_CONFIG_FILE )
///
/// modem description string
///  0:xxxx:xxxx:C:0:0:0.1.2:manufacturer:product
///  | |    |    | | | |     |            |
///  | |    |    | | | |     |             `usb product string (char[])
///  | |    |    | | | |      `usb manufacturer string (char[])
///  | |    |    | | |  `usb device location string (char[])
///  | |    |    | |  `user interface port for modem controlling, status sms, ussd and so on (char[1])
///  | |    |    |  `main port for data communication number (char[1])
///  | |    |     `modem type - 'C' for CDMA or 'W' for WCDMA/HSPA/HSDPA/HSUPA/GSM (char[1])
///  | |     `product id in hex (char[4])
///  |  `vendor id in hex (char[4])
///   `connection enable flag ( 0 - disable, 1 - enable ) (char[1])
int lsmodem_main(int argc, char **argv)
{
	char *fcon = NULL;
	int format = 0;
	int i;
	dev_usb_t *dev, *list;
	dev_conf_t *conf_list;

	if (argc > 1) {
		for (i = 1; i < argc; i++) {
			if (strcmp(argv[i], "-s") == 0){ 
				format = FORMAT_SHORT;
			} else if (strcmp(argv[i], "-j") == 0) {
				format = FORMAT_JSON;
			} else if (strcmp(argv[i], "-c") == 0 && (argc - i > 1)) {
				i++;
				fcon = argv[i];
			} else {
				fprintf(stderr,"Usage: lsmodem [-s|j] [-c config]\n");
				return 1;
			}
		}
	}
	if (!fcon) fcon = MODEM_CONFIG_FILE;

	list = get_usb_list(MODEM_DEVICES_FILE);
	if (search_modems_in_list(list, 0, 0) > 0 && format != FORMAT_SHORT) {
		conf_list = get_config_list(fcon);
		for (dev = list; dev; dev = dev->next) {
			check_config(dev, conf_list);
		}
		free_conf_list(conf_list);
	}
	print_modem_info(format, list);
	free_dev_list(list);
	return 0;
}
