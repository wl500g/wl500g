/* assoclist.c */


#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <net/if.h>
#include <error.h>
#include <httpd.h>
#include <bcmnvram.h>
#include <shutils.h>


typedef unsigned char	UINT8;
typedef unsigned short	UINT16;
typedef unsigned long	UINT32;
typedef unsigned int	UINT;
typedef int		INT;

#define WLC_UP	       		0x02
#define WLC_DOWN		0x03
#define	WLC_DUMP       		0x06
#define WLC_GET_RATE   		0x0C
#define WLC_SET_RATE   		0x0D
#define WLC_GET_FRAG   		0x0F
#define WLC_SET_FRAG   		0x10
#define WLC_GET_RTS		0x11
#define WLC_SET_RTS		0x12
#define WLC_GET_INFRA		0x13
#define WLC_SET_INFRA		0x14
#define WLC_GET_BSSID  		0x17
#define WLC_SET_BSSID		0x18
#define WLC_GET_SSID  		0x19
#define WLC_SET_SSID		0x1A
#define WLC_GET_CHANNEL		0x1D
#define WLC_SET_CHANNEL		0x1E
#define WLC_GET_SRL    		0x1F
#define WLC_SET_SRL		0x20
#define WLC_GET_LRL    		0x21
#define WLC_SET_LRL    		0x22
#define WLC_GET_PLCPHDR		0x23
#define WLC_SET_PLCPHDR		0x24
#define WLC_GET_RADIO  		0x25
#define WLC_SET_RADIO		0x26

#define WLC_SCAN  		0x32
#define WLC_SCANRESULTS		0x33

#define WLC_SET_EVM  		0x3C
#define WLC_GET_TXPWR		0x41
#define WLC_SET_TXPWR  		0x42

#define WLC_GET_MACLIST		0x45
#define WLC_SET_MACLIST		0x46
#define WLC_GET_RATESET		0x47
#define WLC_SET_RATESET		0x48

#define WLC_GET_LOCALE		0x49
#define WLC_SET_LOCALE 		0x4A
#define WLC_GET_SROM		0x4F
#define WLC_SET_SROM   		0x50

#define WLC_FREQ_ACCURACY	0x5C
#define WLC_CARRIER_SUPPRESS	0x5D
#define WLC_GET_MAC_DENY	0x69
#define WLC_SET_MAC_DENY	0x6A
#define WLC_GET_WDSLIST		0x7B
#define WLC_SET_WDSLIST		0x7C
#define WLC_GET_LAZYWDS		0x8A
#define WLC_SET_LAZYWDS		0x8B
#define WLC_GET_SHORTSLOT	0x90
#define WLC_GET_SHORTSLOT_OVERRID	0x91
#define WLC_SET_SHORTSLOT_OVERRID	0x92
#define WLC_GET_SHORTSLOT_RESTRICT	0x93
#define WLC_SET_SHORTSLOT_RESTRICT	0x94
#define WLC_GET_ASSOCLIST	0x9F

#define RTPRIV_IOCTL 		(0x8be0 + 0x01)
#define RTPRIV_IOCTL_SET 	(0x8be0 + 0x02)
#define RTLINUX_GET_OID_802_11_QUERY_MAC_TABLE 	8048
#define RTLINUX_SET_OID_802_11_ACL		8049
#define RTLINUX_SET_OID_802_11_TEST_MODE        8051


#define MAX_LEN_OF_MAC_TABLE 64
#define ETH_LENGTH_OF_ADDRESS 6

typedef struct
{
	UINT8 Addr[ETH_LENGTH_OF_ADDRESS];
	UINT8 Aid;
	UINT8 Psm;
} MACENTRY;

typedef struct
{
	UINT32 num;
	MACENTRY mac[MAX_LEN_OF_MAC_TABLE];
} RT_802_11_MAC_TABLE;

/*================================================================*/
/* Local Types */

typedef struct wl_ioctl 
{
	UINT8	*buf;
	UINT16	len;
	UINT16  flag;
} wl_ioctl_t;

static int	wl_ioctl_nobr(UINT8 *name, int cmd, void *buf, int len );

int ej_wl_status_nobr(int eid, webs_t wp, int argc, char_t **argv)
{
	char		*devname;
	int		result;
	int		cmd;
	UINT8		buf[8192];
	int unit;
	char tmp[100], prefix[sizeof("wlXXXXXXXXXX_")];
	char *name;
	int i, ret = 0;
	int channel;

	channel = atoi(nvram_safe_get("wl_channel"));
	
	if ((unit = atoi(nvram_safe_get("wl_unit"))) < 0)
		return -1;

	snprintf(prefix, sizeof(prefix), "wl%d_", unit);
	name = nvram_safe_get(strcat_r(prefix, "ifname", tmp));		
	
	if (nvram_match(strcat_r(prefix, "mode", tmp), "ap"))
	{
		if (nvram_match("wl_lazywds", "1") ||
			nvram_match("wl_wdsapply_x", "1"))
			ret+=websWrite(wp, "Mode	: Hybrid\n");
		else    ret+=websWrite(wp, "Mode	: AP Only\n");

		ret+=websWrite(wp, "Channel	: %d\n", channel);

	}
	else if (nvram_match(strcat_r(prefix, "mode", tmp), "wds"))
	{
		ret+=websWrite(wp, "Mode	: WDS Only\n");
		ret+=websWrite(wp, "Channel	: %d\n", channel);
	}
	
	devname = "ra0";

	cmd = RTLINUX_GET_OID_802_11_QUERY_MAC_TABLE;


	websWrite(wp, "\n");
	websWrite(wp, "Stations List                           \n");
	websWrite(wp, "----------------------------------------\n");

	if ( (result = wl_ioctl_nobr(devname, cmd, buf, sizeof(RT_802_11_MAC_TABLE))) != 0 ){
		//fprintf(stderr, "Fail: cmd=0x%02x (%d)\n", cmd, result);
		//websWrite(wp, "Fail: cmd=0x%02x (%d)\n", cmd, result);
	}
	else if ( cmd == RTLINUX_GET_OID_802_11_QUERY_MAC_TABLE )
	{
		RT_802_11_MAC_TABLE  *mactable;
		int j;
		
		mactable = (RT_802_11_MAC_TABLE *)buf;

		//websWrite(wp, "mac number: %x\n", mactable->num);

		for ( i=0; i< mactable->num; i++ ) 
		{
			for(j=0;j<6;j++)
			{	
				websWrite(wp, "%02x", mactable->mac[i].Addr[j]);
				if ( (j % 6)==5 )
					websWrite(wp, "\n");
				else
					websWrite(wp, ":");
			}
		}
	}
	return result;
}

/*----------------------------------------------------------------
* wl_ioctl
*
* TODO: describe
*
* Arguments:
*
* Returns:
*       0       - success
*       ~0      - failure
----------------------------------------------------------------*/
int wl_ioctl_nobr( UINT8 *name, int cmd, void *buf, int len )
{
	struct ifreq	ifr;
	wl_ioctl_t	*ioc;
	int		ret = 0;
	int 		s;

	if ( (s = socket(AF_INET, SOCK_DGRAM, 0)) < 0 ) {
		perror("socket");
		return 1;
	}

	ioc = (wl_ioctl_t *)&(ifr.ifr_data);
	ioc->buf = buf;
	ioc->len = len;
	ioc->flag = cmd;

	strncpy(ifr.ifr_name, name, IFNAMSIZ);
	//ifr.ifr_data = (caddr_t) &ioc;
	if ( (ret = ioctl(s, RTPRIV_IOCTL, &ifr)) < 0 ) {
		perror(ifr.ifr_name);
	}
	close(s);

	//printf("ioctl %d : %x\n", ioc->len, ioc->buf);
//%x %x %x %x\n", ioc->len, (unsigned char)ioc->buf[0],(unsigned char)ioc->buf[1], (unsigned char)ioc->buf[2], (unsigned char)ioc->buf[3]);
	return ret;
}



