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
#include <signal.h>
#include <sys/types.h>
#include <sys/time.h>
#include <sys/wait.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <errno.h>
#include <fcntl.h>
#include <net/if.h>
#include <sys/ioctl.h>
#include <malloc.h>

#include "infosrv.h"
#include "queue.h"

#define SRV_ADDR INADDR_ANY
#define SRV_PORT 9999
#define CLN_ADDR INADDR_BROADCAST
#define CLN_PORT 9999

typedef struct fd_item {
	TAILQ_ENTRY(fd_item) entry;
	int fd;
	char *ifname;
} fd_item_t;

typedef TAILQ_HEAD(, fd_item) fd_items_t;

static const int int_1 = 1;

int open_socket(char *ifname)
{
	struct sockaddr_in addr;
	struct ifreq ifr;
	int fd;

	if ((fd = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
		goto error;

	memset(&ifr, 0, sizeof(ifr));
	snprintf(ifr.ifr_name, sizeof(ifr.ifr_name), "%s", ifname);
	if (setsockopt(fd, SOL_SOCKET, SO_BINDTODEVICE, &ifr, sizeof(ifr)) < 0)
		goto error;

	if (setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &int_1, sizeof(int_1)) < 0)
		goto error;

	if (setsockopt(fd, SOL_SOCKET, SO_BROADCAST, &int_1, sizeof(int_1)) < 0)
		goto error;

	memset(&addr, 0, sizeof(addr));
	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = htonl(SRV_ADDR);
	addr.sin_port = htons(SRV_PORT);
	if (bind(fd, (struct sockaddr *)&addr, sizeof(addr)) < 0 )
		goto error;

	return fd;

error:
	fprintf(stderr, "%s[%s]: %s\n", __FUNCTION__, ifname, strerror(errno));
	if (fd >= 0)
		close(fd);
	return -1;
}

int read_packet(int fd, void *packet, int size)
{
	struct sockaddr_in addr;
	int addrlen = sizeof(addr);
	int res = 0;

	memset(packet, 0, sizeof(*packet));
	while ((res = recvfrom(fd, packet, size, 0, (struct sockaddr *)&addr , &addrlen)) < 0 && errno == EINTR)
		continue;
	if (res < 0)
		fprintf(stderr, "%s: %s\n", __FUNCTION__, strerror(errno));

	return (res == size);
}

int send_packet(int fd, void *packet, int size)
{
	struct sockaddr_in addr;
	int res = 0;

	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = htonl(CLN_ADDR);
	addr.sin_port = htons(CLN_PORT);

	while ((res = sendto(fd , packet, size, 0 ,(struct sockaddr *) &addr, sizeof(addr))) < 0 && errno == EINTR)
		continue;
	if (res < 0)
		fprintf(stderr, "%s: %s\n", __FUNCTION__, strerror(errno));

	return res;
}

void *process_packet(int fd, void *packet, char *ifname)
{
	static char packet_res[512];
	IBOX_COMM_PKT_HDR_EX *hdr = packet;
	IBOX_COMM_PKT_RES_EX *hdr_res = (IBOX_COMM_PKT_RES_EX *) &packet_res;
	PKT_GET_INFO *info;

	if (hdr->ServiceID != NET_SERVICE_ID_IBOX_INFO ||
	    hdr->PacketType != NET_PACKET_TYPE_CMD)
		return NULL;

	if (hdr->OpCode != NET_CMD_ID_GETINFO && hdr->OpCode != NET_CMD_ID_GETINFO_MANU &&
	    hdr->OpCode == hdr_res->OpCode &&
	    hdr->Info == hdr_res->Info)
		return hdr_res;

	hdr_res->ServiceID = NET_SERVICE_ID_IBOX_INFO;
	hdr_res->PacketType = NET_PACKET_TYPE_RES;
	hdr_res->OpCode = hdr->OpCode;

	if (hdr->OpCode != NET_CMD_ID_GETINFO && hdr->OpCode != NET_CMD_ID_GETINFO_MANU) {
		unsigned char hwaddr[6];
		get_hwaddr(ifname, hwaddr, sizeof(hwaddr));
		if (memcmp(hdr->MacAddress, hwaddr, sizeof(hdr_res->MacAddress)) == 0)
			return NULL;
		hdr_res->Info = hdr->Info;
		memcpy(hdr_res->MacAddress, hdr->MacAddress, sizeof(hdr_res->MacAddress));
	}

	switch (hdr->OpCode) {
	case NET_CMD_ID_GETINFO:
	case NET_CMD_ID_GETINFO_MANU:
		info = (PKT_GET_INFO *)(packet_res + sizeof(IBOX_COMM_PKT_RES));
		memset(info, 0, sizeof(*info));
		get_printer(info->PrinterInfo, sizeof(info->PrinterInfo));
		get_productid(info->ProductID, sizeof(info->ProductID));
		get_firmware(info->FirmwareVersion, sizeof(info->FirmwareVersion));
		get_ssid(ifname, info->SSID, sizeof(info->SSID));
		get_netmask(ifname, info->NetMask, sizeof(info->NetMask));
		get_hwaddr(ifname, info->MacAddress, sizeof(info->MacAddress));
		info->OperationMode = get_opmode(ifname);
		info->Regulation = get_regulation(ifname);
		break;
	default:
		return NULL;
	}

	return hdr_res;
}

int main(int argc, char *argv[])
{
	char packet[512], *reply;
	fd_items_t fds;
	fd_item_t *item, *next;
	fd_set active_rfds;
	int max_fd, i;
	pid_t pid;

	if (argv[1] == NULL) {
		fprintf(stderr, "usage: %s ifname [ifname]...\n", argv[0]);
		return 1;
	}

	FD_ZERO(&active_rfds);
	TAILQ_INIT(&fds);
	max_fd = -1;

	for (i = 1; argv[i] && *argv[i]; i++) {
		int fd = open_socket(argv[i]);
		if (fd < 0)
			continue;
		item = malloc(sizeof(*item));
		if (item == NULL)
			continue;
		item->fd = fd;
		item->ifname = argv[i];
		FD_SET(item->fd, &active_rfds);
		TAILQ_INSERT_TAIL(&fds, item, entry);
		if (max_fd < fd) max_fd = fd;
	}

	if (max_fd < 0)
		return 1;

	pid = fork();
	while (pid == 0) {
		struct timeval tv;
		fd_set rfds;
		int res;

		rfds = active_rfds;
		tv.tv_sec = 2;
		tv.tv_usec = 0;
		while ((res = select(max_fd + 1 , &rfds, NULL, NULL, &tv)) < 0 && errno == EINTR)
			continue;
		if (res <= 0)
			continue;

		TAILQ_FOREACH(item, &fds, entry) {
			if (!FD_ISSET(item->fd, &rfds))
				continue;

			if (read_packet(item->fd, packet, sizeof(packet)) < 0)
				continue;
			reply = process_packet(item->fd, packet, item->ifname);
			if (reply)
				send_packet(item->fd, reply, sizeof(packet));
		}
	}

	TAILQ_FOREACH_SAFE(item, &fds, entry, next) {
		TAILQ_REMOVE(&fds, item, entry);
		close(item->fd);
		free(item);
	}

	return 0;
}
