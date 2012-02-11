#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <errno.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <sys/ioctl.h>
#include <fcntl.h>

#include <netconf.h>
#include <bcmnvram.h>
#include <nvparse.h>
#include <shutils.h>

int get_fw_ver(char *productid, char *fwver)
{
	int fd;
	uint32_t imagelen;
	char verPtr[64];

	strcpy(productid, "WL500");
	strcpy(fwver, "0.1.0.1");

	if ((fd = open(MTD_DEV(1), O_RDONLY)) < 0)
		return -1;

	if (lseek(fd, 4, SEEK_SET) < 0)
		return -2;
	if (read(fd, &imagelen, 4) < 4)
		return -3;

	dprintf("image len %x\n", imagelen);
	if (lseek(fd, imagelen - 64, SEEK_SET) < 0)
		return -2;
	if (read(fd, verPtr, 64) < 4)
		return -3;
	strncpy(productid, verPtr + 4, 12);
	productid[12] = '\0';
	sprintf(fwver, "%d.%d.%d.%d", verPtr[0], verPtr[1], verPtr[2], verPtr[3]);

	dprintf("get_fw_ver: %s - %s\n", productid, fwver);
	close(fd);
	return 0;
}

int get_model(void)
{
	char *hwver = nvram_safe_get("hardware_version");

	if (nvram_match("boardnum", "mn700"))
		return MDL_MN700;
	else if (nvram_match("boardtype", "bcm95365r"))
		return MDL_WL500GX;
	else if (nvram_match("boardtype", "0x467") &&
		 nvram_match("boardnum", "45"))
		return MDL_WL550GE;
	else if (nvram_match("boardtype", "0x042f") &&
		 nvram_match("boardnum", "44"))
		return MDL_WL700G;
	else if (nvram_match("boardtype", "0x042f") &&
		 nvram_match("boardnum", "45"))
		return MDL_WL500GP;
	else if (nvram_match("boardtype", "0x0472") &&
		 nvram_match("boardnum", "45"))
		return MDL_WL500W;
	else if (nvram_match("boardtype", "0x48E") &&
		 nvram_match("boardnum", "45")) {
		if (strncmp(hwver, "WL520GU", 7) == 0)
			return MDL_WL520GU;
		if (strncmp(hwver, "WL330GE", 7) == 0)
			return MDL_WL330GE;
		return MDL_WL500GPV2;
	}
	else if (nvram_match("boardtype", "0x048e") &&
		 !nvram_match("boardnum", "45"))
		return MDL_DIR320;
	else if (nvram_match("boardtype", "0x04cf") &&
		 nvram_match("boardnum", "45"))
		return MDL_RTN16;
	else if (nvram_match("boardtype", "0x04CD") &&
		 nvram_match("boardnum", "45") && nvram_match("boardrev", "0x1201"))
		return MDL_RTN12;
	else if (nvram_match("boardtype", "0x054D") &&
		 nvram_match("boardnum", "45") && nvram_match("boardrev", "0x1101")) {
		if (strncmp(hwver, "RTN12C1", 7) == 0)
			return MDL_RTN12C1;
		return MDL_RTN12B1;
	}
	else if (nvram_match("boardtype", "0x04EC") &&
		 nvram_match("boardnum", "45") && nvram_match("boardrev", "0x1402"))
		return MDL_RTN10;
	else if (nvram_match("boardtype", "0x0550") &&
		 nvram_match("boardnum", "45") && nvram_match("boardrev", "0x1102"))
		return MDL_RTN10U;
	else if (nvram_match("boardtype", "0x04CF") &&
		 nvram_match("boardnum", "3500") &&
		 (nvram_match("boardrev", "02") || nvram_match("boardrev", "0x1213")))
		return MDL_WNR3500L;

	return MDL_UNKNOWN;
}

/*
 * Get the ip configuration index if it exists given the 
 * eth name.
 * 
 * @param	wl_ifname 	pointer to eth interface name
 * @return	index or -1 if not found
 */ 
int 
get_ipconfig_index(const char *eth_ifname)
{
	char varname[64];
	char varval[64];
	char *ptr;
	char wl_ifname[64];
	int index;
	
	/* Bail if we get a NULL or empty string */
	
	if (!eth_ifname) return -1;
	if (!*eth_ifname) return -1;
	
	/* Look up wl name from the eth name */
	if( osifname_to_nvifname( eth_ifname, wl_ifname, sizeof(wl_ifname)) != 0 )
		return -1;
		
	snprintf(varname,sizeof(varname),"%s_ipconfig_index",wl_ifname);

	ptr = nvram_get(varname);
	
	if (ptr){
	/* Check ipconfig_index pointer to see if it is still pointing 
	   the correct lan config block */
		if (*ptr) {
			int index;
			char *ifname;
			char buf[64];
			index = atoi(ptr);
			
			snprintf(buf,sizeof(buf),"lan%d_ifname",index);
			
			ifname = nvram_get(buf);
			
			if (ifname) {
				if  (!(strcmp(ifname,wl_ifname)))
					return index;
			}
			nvram_unset(varname);
		}
	}
	
	/* The index pointer may not have been configured if the
	 * user enters the variables manually. Do a brute force search 
	 *  of the lanXX_ifname variables
	 */
	for (index=0 ; index < MAX_NVPARSE; index++){
		snprintf(varname,sizeof(varname),"lan%d_ifname",index);
		if ( nvram_match(varname,wl_ifname)){
			/* if a match is found set up a corresponding index pointer for wlXX */
			snprintf(varname,sizeof(varname),"%s_ipconfig_index",wl_ifname);
			snprintf(varval,sizeof(varval),"%d",index);
			nvram_set(varname,varval);
			nvram_commit();
			return index;
		};
	}
	return -1;
}
	
/*
 * Set the ip configuration index given the eth name
 * Updates both wlXX_ipconfig_index and lanYY_ifname.
 * 
 * @param	eth_ifname 	pointer to eth interface name
 * @return	0 if successful -1 if not.
 */
int 
set_ipconfig_index(const char *eth_ifname, int index)
{
	char varname[255];
	char varval[16];
	char wl_ifname[64];
	
	/* Bail if we get a NULL or empty string */
	
	if (!eth_ifname) return -1;
	if (!*eth_ifname) return -1;
	
	if (index >= MAX_NVPARSE) return -1;
	
	/* Look up wl name from the eth name only if the name contains
	   eth
	*/

	if( osifname_to_nvifname( eth_ifname, wl_ifname, sizeof(wl_ifname)) != 0 )
		return -1;
	
	snprintf(varname,sizeof(varname),"%s_ipconfig_index",wl_ifname);
	snprintf(varval,sizeof(varval),"%d",index);
	nvram_set(varname,varval);
	
	snprintf(varname,sizeof(varname),"lan%d_ifname",index);
	nvram_set(varname,wl_ifname);
	
	nvram_commit();
	
	return 0;
}
	
/*
 * Get interfaces belonging to a specific bridge.
 * 
 * @param	bridge_name 	pointer to bridge interface name
 * @return	list of interfaces belonging to the bridge or NULL
 *              if not found/empty
 */	
char *
get_bridged_interfaces(const char *bridge_name)
{
	static char interfaces[255] ;	
	char *ifnames=NULL;
	char bridge[64];
	
	if (!bridge_name) return NULL;
	
	memset(interfaces,0,sizeof(interfaces));
	snprintf(bridge,sizeof(bridge),"%s_ifnames",bridge_name);
	
	ifnames=nvram_get(bridge);
	
	if (ifnames) 
		strncpy(interfaces,ifnames,sizeof(interfaces));
	else
		return NULL;
		
	return  interfaces;
		
}

/*
 * Parse the unit and subunit from an interface string such as wlXX or wlXX.YY
 *
 * @param	ifname	interface string to parse
 * @param	unit	pointer to return the unit number, may pass NULL
 * @param	subunit	pointer to return the subunit number, may pass NULL
 * @return	Returns 0 if the string ends with digits or digits.digits, -1 otherwise.
 *		If ifname ends in digits.digits, then unit and subuint are set
 *		to the first and second values respectively. If ifname ends 
 *		in just digits, unit is set to the value, and subunit is set
 *		to -1. On error both unit and subunit are -1. NULL may be passed
 *		for unit and/or subuint to ignore the value.
 */
int
get_ifname_unit(const char *ifname, int *unit, int *subunit)
{
	const char digits[] = "0123456789";
	char str[64];
	char *p;
	size_t ifname_len = strlen(ifname);
	size_t len;
	long val;

	if (unit)
		*unit = -1;
	if (subunit)
		*subunit = -1;
	
	if (ifname_len + 1 > sizeof(str)) 
		return -1;

	strcpy(str, ifname);

	/* find the trailing digit chars */
	len = sh_strrspn(str, digits);
	
	/* fail if there were no trailing digits */
	if (len == 0)
		return -1;

	/* point to the beginning of the last integer and convert */
	p = str + (ifname_len - len);
	val = strtol(p, NULL, 10);
	
	/* if we are at the beginning of the string, or the previous
	 * character is not a '.', then we have the unit number and
	 * we are done parsing
	 */
	if (p == str || p[-1] != '.') {
		if (unit)
			*unit = val;
		return 0;
	} else {
		if (subunit)
			*subunit = val;
	}

	/* chop off the '.NNN' and get the unit number */
	p--;
	p[0] = '\0';
	
	/* find the trailing digit chars */
	len = sh_strrspn(str, digits);

	/* fail if there were no trailing digits */
	if (len == 0)
		return -1;

	/* point to the beginning of the last integer and convert */
	p = p - len;
	val = strtol(p, NULL, 10);

	/* save the unit number */
	if (unit)
		*unit = val;

	return 0;
}


