#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <bcmnvram.h>


#define XSTR(s) STR(s)
#define STR(s) #s

typedef struct nvmap
{
	char *sid;
	char *fid;
	char *name;
} nvmap;

struct nvmap maps[] = {
	{ "BRIPAddress", "IPRouters", "currentip"},
	{ "general.log", "ProductID", "productid" },
	{ "General", "x_Username", "username" },
	{ "General", "x_Password", "password" },
	{ "LANHostConfig", "x_LANDHCPClient", "lan_proto"},
	{ "LANHostConfig", "IPRouters", "lan_ipaddr"},
	{ "LANHostConfig", "SubnetMask", "lan_netmask"},
	{ "LANHostConfig", "x_Gateway", "lan_gateway"},
	{ "LANHostConfig", "DHCPServerConfigurable", "dhcp_proto"},
	{ "LANHostConfig", "DomainName", "dhcp_domain"},
	{ "LANHostConfig", "MinAddress", "dhcp_start"},
	{ "LANHostConfig", "MaxAddress", "dhcp_end"},
	{ "LANHostConfig", "LeaseTime", "dhcp_leases"},
	{ "WLANConfig11b", "SSID", "wl_ssid" },
	{ "WLANConfig11b", "x_APMode", "wl_wdsmode_x"},
	{ "WLANConfig11b", "Channel", "wl_channel"},
	{ "WLANConfig11b", "x_BRApply", "wl_wdsapply_x"},
	{ "WLANConfig11b", "x_BRestrict", "wl_lazywds"},
	{ "WLANConfig11b", "RBRListCount", "wl_wds_num_x"},
	{ "WLANConfig11b", "RBRList", "wl_wdslist_x"},
	{ "WLANConfig11b", "DataRateAll", "wl_rate"},
	{ "WLANConfig11b", "x_Mode11g", "wl_gmode_x"},
	{ "WLANConfig11b", "x_GProtection", "wl_gmode_protection_x"},
	{ "WLANConfig11b", "AuthenticationMethod", "wl_authmode_x"},
	{ "WLANConfig11b", "WEPType", "wl_weptype_x"},
	{ "WLANConfig11b", "x_Phrase", "wl_wpapsk"},
	{ "WLANConfig11b", "WEPKey1", "wl_wepkey1"},
	{ "WLANConfig11b", "WEPKey2", "wl_wepkey2"},
	{ "WLANConfig11b", "WEPKey3", "wl_wepkey3"},
	{ "WLANConfig11b", "WEPKey4", "wl_wepkey4"},
	{ "WLANConfig11b", "WEPDefaultKey", "wl_wepkey"},
	{ "WLANConfig11b", "x_BlockBCSSID", "wl_closed"},
	{ "WLANConfig11b", "DataRate", "wl_rateset_x"},
	{ "WLANConfig11b", "x_Rekey", "wl_rekey"},
	{ "WLANConfig11b", "x_Frag", "wl_frag"},
	{ "WLANConfig11b", "x_RTS", "wl_rts"},
	{ "WLANConfig11b", "x_DTIM", "wl_dtim"},
	{ "WLANConfig11b", "x_Beacon", "wl_beacon"},
	{ "WLANConfig11b", "x_FrameBurst", "wl_framburst_x"},
	{ "WLANConfig11b", "x_BRhwaddr", "wl_wds"},

	{ "DeviceSecurity11a", "ACLMethod", "wl_macmode_x"},
	{ "DeviceSecurity11a", "ACLListCount", "wl_maclist_num_x"},
	{ "DeviceSecurity11a", "MacAddr", "wl_maclist_x"},
	{ "WLANAuthentication11a", "ExAuthDBIPAddr", "wl_radius_ipaddr"},
	{ "WLANAuthentication11a", "ExAuthDBPortNumber", "wl_radius_port"},
	{ "WLANAuthentication11a", "ExAuthDBPassword", "wl_radius_key"},


	{ "Storage", "StorageApps", "st_apps"},
	{ "Storage", "SharedCount", "sh_num"},
	{ "Storage", "SharedPath", "sh_path"},
	{ "Storage", "SharedName", "sh_name"},
	{ "Storage", "SharedEnable", "sh_rright_x"},
	{ "Storage", "WriteEnable", "sh_wright_x"},
	{ "Storage", "UserCount", "acc_num"},
	{ "Storage", "UserName", "acc_username"},
	{ "Storage", "UserPassword", "acc_password"},
	{ "Storage", "x_SharedPath", "sh_path_x"},
	{ "Storage", "x_SharedName", "sh_name_x"},
	{ "Storage", "x_SharedComment", "sh_comment_x"},
	{ "Storage", "x_SharedMaxUser", "sh_max_user_x"},
	{ "Storage", "x_AccAny", "sh_accuser_x0"},
	{ "Storage", "x_AccAny_Share", "sh_accuser_share_x0"},
	{ "Storage", "x_AccAny_Write", "sh_accuser_write_x0"},
	{ "Storage", "x_AccUser1", "sh_accuser_x1"},
	{ "Storage", "x_AccUser1_Share", "sh_accuser_share_x1"},
	{ "Storage", "x_AccUser1_Write", "sh_accuser_write_x1"},
	{ "Storage", "x_AccUser2", "sh_accuser_x2"},
	{ "Storage", "x_AccUser2_Share", "sh_accuser_share_x2"},
	{ "Storage", "x_AccUser2_Write", "sh_accuser_write_x2"},
	{ "Storage", "x_AccUser3", "sh_accuser_x3"},
	{ "Storage", "x_AccUser3_Share", "sh_accuser_share_x3"},
	{ "Storage", "x_AccUser3_Write", "sh_accuser_write_x3"},
	{ "Storage", "x_AccUser4", "sh_accuser_x4"},
	{ "Storage", "x_AccUser4_Share", "sh_accuser_share_x4"},
	{ "Storage", "x_AccUser4_Write", "sh_accuser_write_x4"},
	{ "Storage", "x_AccUser5", "sh_accuser_x5"},
	{ "Storage", "x_AccUser5_Share", "sh_accuser_share_x5"},
	{ "Storage", "x_AccUser5_Write", "sh_accuser_write_x5"},
	{ "Storage", "x_AccUser6", "sh_accuser_x6"},
	{ "Storage", "x_AccUser6_Share", "sh_accuser_share_x6"},
	{ "Storage", "x_AccUser6_Write", "sh_accuser_write_x6"},
	{ NULL, NULL, NULL }
};

char *mac_conv(char *mac_name, int idx, char *buf);

#ifdef REMOVE
/* This function is used to map nvram value from asus to Broadcom */
static void readFromNvram()
{	
	char tmpstr[32], tmpstr1[32], macbuf[18];
	char list[2048];
	int i, num;

	printf("read from nvram\n");

	/* Wireless Section */
	/* GMODE */
	strcpy(tmpstr, nvram_get("wl_gmode_x"));

	if (strcmp(tmpstr, "1")==0) {
		nvram_set("wl0_gmode", XSTR(GMODE_PERFORMANCE));
	} else if (strcmp(tmpstr, "2")==0) {
		nvram_set("wl0_gmode", XSTR(GMODE_LRS));
	} else if (strcmp(tmpstr, "3")==0) {
		nvram_set("wl0_gmode", XSTR(GMODE_B_DEFERRED));
	} else {
		nvram_set("wl0_gmode", XSTR(GMODE_AUTO));
	}

	if (nvram_match("wl_gmode_protection_x", "0")==0)
		nvram_set("wl0_gmode_protection", "off");
	else nvram_set("wl0_gmode_protection", "auto");

	/* Authentication and Encryption */
	strcpy(tmpstr, nvram_get("wl_authmode_x"));
	strcpy(tmpstr1, nvram_get("wl_weptype_x"));

	if (strcmp(tmpstr, "1")==0) { /* Shared */
		nvram_set("wl0_auth", "1");
		nvram_set("wl0_auth_mode", "disabled");
		nvram_set("wl0_wep", "wep");
	} else if (strcmp(tmpstr, "2")==0) { /* WPA-PSK */
		nvram_set("wl0_auth", "0");
		nvram_set("wl0_auth_mode", "psk");

		if (strcmp(tmpstr1, "1")==0) nvram_set("wl0_wep", "aes");
		else if (strcmp(tmpstr1, "2")==0) nvram_set("wl0_wep", "tkip+aes");
		else nvram_set("wl0_wep", "tkip");
	} else if (strcmp(tmpstr, "3")==0) { /* WPA */
		nvram_set("wl0_auth", "0");
		nvram_set("wl0_auth_mode", "wpa");

		if (strcmp(tmpstr1, "1")==0) nvram_set("wl0_wep", "aes");
		else if (strcmp(tmpstr1, "2")==0) nvram_set("wl0_wep", "tkip+aes");
		else nvram_set("wl0_wep", "tkip");
	} else if (strcmp(tmpstr, "4")==0) { /* Radius */
		nvram_set("wl0_auth", "0");
		nvram_set("wl0_auth_mode", "radius");

		if (strcmp(tmpstr1, "1")==0) nvram_set("wl0_wep", "wep");
		else if (strcmp(tmpstr1, "2")==0) nvram_set("wl0_wep", "wep");
		else nvram_set("wl0_wep", "");
	} else { /* Open or Shared */
		nvram_set("wl0_auth", "0");
		nvram_set("wl0_auth_mode", "disabled");

		if (strcmp(tmpstr1, "1")==0) nvram_set("wl0_wep", "wep");
		else if (strcmp(tmpstr1, "2")==0) nvram_set("wl0_wep", "wep");
		else nvram_set("wl0_wep", "");
	}


	/* WDS control */
	if (nvram_match("wl_wds_x", "1")) nvram_set("wl0_mode", "wds");
	else nvram_set("wl0_mode", "ap");

	if (nvram_match("wl_lazywds", "1")) nvram_set("wl0_lazywds", "1");
	else nvram_set("wl0_lazywds", "0");

	if (nvram_match("wl_wdsapply_x", "1")) {
		num = atoi(nvram_get("wl_wdslist_num_x"));
		list[0]=0;

		for (i=0;i<num;i++) {
			sprintf(list, "%s %s", list, mac_conv("wl_wdslist_x", i, macbuf));
		}
		nvram_set("wl0_maclist", list);
	}

	/* Mac filter */
	strcpy(tmpstr, nvram_get("wl_macmode_x"));

	if (strcmp(tmpstr, "Accept")==0) { /* allow */
		nvram_set("wl0_macmode", "allow");
	} else if (strcmp(tmpstr, "Deny")==0) { /* deny */
		nvram_set("wl0_macmode", "deny");
	}

	if (!strcmp(tmpstr, "Disable")) {
		num = atoi(nvram_get("wl_maclist_num_x"));
		list[0]=0;

		for (i=0;i<num;i++) {
			sprintf(list, "%s;%s", list, mac_conv("wl_maclist_x", i, macbuf));
		}

		nvram_set("wl0_maclist", list);
	}

	/* Direct copy value */

	nvram_set("wl0_ssid", nvram_get("wl_ssid"));
	nvram_set("wl0_channel", nvram_get("wl_channel"));
	nvram_set("wl0_country", nvram_get("wl_country"));
	nvram_set("wl0_rate", nvram_get("wl_rate"));
	nvram_set("wl0_frag", nvram_get("wl_frag"));
	nvram_set("wl0_rts", nvram_get("wl_rts"));
	nvram_set("wl0_dtim", nvram_get("wl_dtim"));
	nvram_set("wl0_bcn", nvram_get("wl_bcn"));
	nvram_set("wl0_plcphdr", nvram_get("wl_plcphdr"));
	nvram_set("wl0_key", nvram_get("wl_key"));
	nvram_set("wl0_key1", nvram_get("wl_key1"));
	nvram_set("wl0_key2", nvram_get("wl_key2"));
	nvram_set("wl0_key3", nvram_get("wl_key3"));
	nvram_set("wl0_key4", nvram_get("wl_key4"));

	/* LAN Section */

	/* Storage Section */
}
#endif // REMOVE

void findNVRAMName(const char *serviceId, const char *field, char *name)
{
	int idx;

	idx = 0;

	//printf("find : %s %s\n", serviceId, field);

	while (maps[idx].sid!=NULL) {
		if( strcmp(serviceId, maps[idx].sid) == 0) {
			if (strcmp(field, maps[idx].fid) == 0) break;
			//else if (strstr(field, maps[idx].fid)) break;
		}
		idx ++;
	} 

	if (maps[idx].sid==NULL) {
		strcpy(name, field);
	} else {	 
		sprintf(name, maps[idx].name);

		//printf("nvram name: %s\n", name);
	}
}

char *findpattern(char *target, char *pattern)
{
	char *find;
	int len;

	printf("find : %s %s\n", target, pattern);
	if ((find=strstr(target, pattern))) {
		len = strlen(pattern);
		if (find[len]==';' || find[len]==0) {
			return find;
		}
	}
	return NULL;
}

char *strcat_r(char *dst, char *src, char *tar)
{
	sprintf(tar, "%s%s", dst, src);
	return(tar);
}


char *nvram_get_i(char *name, int idx)
{
	char tmpstr1[64];

	if (idx!=-1) {
		sprintf(tmpstr1, "%s%d", name, idx);
	} else {
		sprintf(tmpstr1, "%s", name); 
	}
	//printf("get : %s %s %s\n", tmpstr2, tmpstr1, name1);

	return(nvram_get(tmpstr1));
}

char *mac_conv(char *mac_name, int idx, char *buf)
{
	char *mac;
	int i, j;

	mac = nvram_get_i(mac_name, idx);

	j=0;
	for (i=0; i<12; i++) {
		if (i!=0&&i%2==0) buf[j++] = ':';
		buf[j++] = mac[i];
	}
	buf[j] = 0;

	return(buf);
}


int nvram_match_i(char *name, int idx, char *match) {
	const char *value = nvram_get_i(name, idx);

	printf("match %s %s\n", value, match);
	return (value && !strcmp(value, match));
}


void nvram_set_i(char *name, int idx, char *value)
{
	char tmpstr1[64];

	if (idx!=-1) {
		sprintf(tmpstr1, "%s%d", name, idx);
	} else {
		sprintf(tmpstr1, "%s", name); 
	}
	//printf("get : %s %s %s\n", tmpstr2, tmpstr1, name1);

	nvram_set(tmpstr1, value);
}

int nvram_cpy(char *name1, char *name2, int idx)
{

	char tmpstr1[128];

	strcpy(tmpstr1, nvram_get_i(name2, idx));
	return nvram_set(name1, tmpstr1);
}

void nvram_cpy2(char *name1, char *name2, int idx)
{

	char tmpstr1[128];

	strcpy(tmpstr1, nvram_get(name2));
	nvram_set_i(name1, idx, tmpstr1);
}

void getSharedEntry(int index)
{
	char idxstr[10], tmpstr[32], tmpstr1[128];
	char rright[128], wright[128], username[32];
	char *ruser, *wuser;
	int i, j, acc_num;

	sprintf(idxstr, "%d", index);
	strcpy(tmpstr1, nvram_get(strcat_r("sh_path", idxstr, tmpstr)));
	printf("path: %s\n", tmpstr1);

	nvram_set("sh_path_x", idxstr);
	nvram_cpy("sh_name_x", "sh_name", index);
	nvram_cpy("sh_comment_x", "sh_comment", index);
	nvram_cpy("sh_max_user_x", "sh_max_user", index);

	nvram_set("sh_accuser_x0", "Guest");
	nvram_cpy("sh_accuser_share_x0", "sh_rright_x", index);
	nvram_cpy("sh_accuser_write_x0", "sh_wright_x", index);

	strcpy(rright, nvram_get_i("sh_rright", index));
	strcpy(wright, nvram_get_i("sh_wright", index));

	j=1;

	acc_num = atoi(nvram_get("acc_num"));

	for (i=0; i<acc_num; i++) {
		strcpy(username, nvram_get_i("acc_username", i));
		ruser = findpattern(rright, username);
		wuser = findpattern(wright, username);

		if (ruser!=NULL || wuser!=NULL) {
			if (j<=6) {
				sprintf(idxstr, "%d", i);
				nvram_set_i("sh_accuser_x", j, idxstr);
				if (ruser) nvram_set_i("sh_accuser_share_x", j, "on");
				else nvram_set_i("sh_accuser_share_x", j, "");
				if (wuser) nvram_set_i("sh_accuser_write_x", j, "on");
				else nvram_set_i("sh_accuser_write_x", j, "");
			}
			j++;
		}
	}

	sprintf(idxstr, "%d", j);
	nvram_set("sh_accuser_x_num", idxstr);

	for (;j<=6;j++)
	{
		nvram_set_i("sh_accuser_x", j, "99");
		nvram_set_i("sh_accuser_share_x", j, "");
		nvram_set_i("sh_accuser_write_x", j, "");
	}
}

void setSharedEntry(int index)
{
	char idxstr[10], tmpstr[32], tmpstr1[128], user[32];
	char rright[128], wright[128];
	int i;


	sprintf(idxstr, "%d", index);
	strcpy(tmpstr1, nvram_get(strcat_r("sh_path", idxstr, tmpstr)));
	printf("path: %s\n", tmpstr1);

	//nvram_set(strcat_r("sh_path", idxstr, tmpstr), nvram_get("sh_path_x"));
	nvram_cpy2("sh_name", "sh_name_x", index);
	nvram_cpy2("sh_comment", "sh_comment_x", index);
	nvram_cpy2("sh_max_user", "sh_max_user_x", index);

	memset(rright, 0, 128);
	memset(wright, 0, 128);

	for (i=0;i<=6;i++) {
		sprintf(idxstr, "%d", i);

		if (i==0) {
			strcpy(user, "Guest");
		} else {
			strcpy(idxstr, nvram_get_i("sh_accuser_x", i));
			if (strcmp(idxstr, "99") == 0) continue; 
			strcpy(user, nvram_get(strcat_r("acc_username", idxstr, tmpstr)));
		}

		printf("user : %s\n", user);

		if (nvram_match_i("sh_accuser_share_x", i, "on"))
			sprintf(rright, "%s%s;", rright, user);
		if (nvram_match_i("sh_accuser_write_x", i, "on"))
			sprintf(wright, "%s%s;", wright, user);


		nvram_set_i("sh_accuser_share_x", i, "");
		nvram_set_i("sh_accuser_write_x", i, "");
	}

	printf("list: %s - %s\n", rright, wright);
	nvram_set_i("sh_rright", index, rright);
	nvram_set_i("sh_wright", index, wright);
}
