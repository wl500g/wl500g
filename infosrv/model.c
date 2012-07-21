/*
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 * MA 02111-1307 USA
 */

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <errno.h>
#include <fcntl.h>
#include <net/if.h>
#include <sys/ioctl.h>
#include <net/if_arp.h>
#include <proto/ethernet.h>

#include "infosrv.h"
#include <bcmnvram.h>
#include <shutils.h>

//#define USE_IFNAME

char *get_printer(void *buf, int size)
{
	snprintf(buf, size, "%s", nvram_safe_get("printer_model_t"));
	return buf;
}

char *get_productid(void *buf, int size)
{
	snprintf(buf, size, "%s", nvram_safe_get("productid"));
	return buf;
}

char *get_firmware(void *buf, int size)
{
	FILE *fp;

	fp = fopen("/.version", "r");
	if (fp == NULL)
		return NULL;

	fgets(buf, size, fp);
	fclose(fp);
	return buf;
}

char *get_ssid(char *ifname, void *buf, int size)
{
	snprintf(buf, size, "%s", nvram_safe_get("wl_ssid"));
	return buf;
}

char *get_netmask(char *ifname, void *buf, int size)
{
#ifdef USE_IFNAME
	struct ifreq ifr;
	int fd, res;
	char *netmask;

	if ((fd = socket(AF_INET, SOCK_RAW, IPPROTO_RAW)) < 0)
		return NULL;

	memset(&ifr, 0, sizeof(ifr));
	snprintf(ifr.ifr_name, sizeof(ifr.ifr_name), "%s", ifname);
	res = ioctl(fd, SIOCGIFNETMASK, &ifr);
	close(fd);

	if (res < 0)
		return NULL;

	netmask = inet_ntoa(((struct sockaddr_in *)(&ifr.ifr_netmask))->sin_addr);
	snprintf(buf, size, "%s", netmask);
	return buf;
#else
	snprintf(buf, size, "%s", nvram_safe_get("lan_netmask"));
	return buf;
#endif
}

unsigned char *get_hwaddr(char *ifname, void *buf, int size)
{
#ifdef USE_IFNAME
	struct ifreq ifr;
	int fd, res;
	int addrlen = size > ETHER_ADDR_LEN ? ETHER_ADDR_LEN : size;

	if ((fd = socket(AF_INET, SOCK_RAW, IPPROTO_RAW)) < 0)
		return NULL;

	memset(&ifr, 0, sizeof(ifr));
	snprintf(ifr.ifr_name, sizeof(ifr.ifr_name), "%s", ifname);
	res = ioctl(fd, SIOCGIFHWADDR, &ifr);
	close(fd);

	if (res < 0 || ifr.ifr_hwaddr.sa_family != ARPHRD_ETHER)
		return NULL;

	memmove(buf, ifr.ifr_hwaddr.sa_data, addrlen);
	return buf;
#else
	if (!nvram_invmatch("et0macaddr", ""))
		return NULL;

	ether_atoe(nvram_safe_get("et0macaddr"), buf);
	return buf;
#endif
}

int get_opmode(char *ifname)
{
	return OPERATION_MODE_WB;
}

int get_regulation(char *ifname)
{
	return 0xff;
}
