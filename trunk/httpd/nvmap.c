#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <bcmnvram.h>
#include <shutils.h>


typedef struct nvmap
{
	const char *fid;
	const char *name;
} nvmap;

static const struct nvmap maps[] = {
	{ "IPRouters",		"currentip"},
	{ "ProductID",		"productid" },
	{ "x_Username",		"username" },
	{ "x_Password",		"password" },
	{ "x_LANDHCPClient",	"lan_proto"},
	{ "IPRouters",		"lan_ipaddr"},
	{ "SubnetMask",		"lan_netmask"},
	{ "x_Gateway",		"lan_gateway"},
	{ "DHCPServerConfigurable", "dhcp_proto"},
	{ "DomainName",		"dhcp_domain"},
	{ "MinAddress",		"dhcp_start"},
	{ "MaxAddress",		"dhcp_end"},
	{ "LeaseTime",		"dhcp_leases"},
	{ "SSID",		"wl_ssid" },
	{ "x_APMode",		"wl_wdsmode_x"},
	{ "Channel",		"wl_channel"},
	{ "x_BRApply",		"wl_wdsapply_x"},
	{ "x_BRestrict",	"wl_lazywds"},
	{ "RBRListCount",	"wl_wds_num_x"},
	{ "RBRList",		"wl_wdslist_x"},
	{ "DataRateAll",	"wl_rate"},
	{ "x_Mode11g",		"wl_gmode_x"},
	{ "x_GProtection",	"wl_gmode_protection_x"},
	{ "AuthenticationMethod", "wl_authmode_x"},
	{ "WEPType",		"wl_weptype_x"},
	{ "x_Phrase",		"wl_wpapsk"},
	{ "WEPDefaultKey",	"wl_wepkey"},
	{ "x_BlockBCSSID",	"wl_closed"},
	{ "DataRate",		"wl_rateset_x"},
	{ "x_Rekey",		"wl_rekey"},
	{ "x_Frag",		"wl_frag"},
	{ "x_RTS",		"wl_rts"},
	{ "x_DTIM",		"wl_dtim"},
	{ "x_Beacon",		"wl_beacon"},
	{ "x_FrameBurst",	"wl_framburst_x"},
	{ "x_BRhwaddr",		"wl_wds"},

	{ "ACLMethod",		"wl_macmode_x"},
	{ "ACLListCount",	"wl_maclist_num_x"},
	{ "MacAddr",		"wl_maclist_x"},
	{ "ExAuthDBIPAddr",	"wl_radius_ipaddr"},
	{ "ExAuthDBPortNumber",	"wl_radius_port"},
	{ "ExAuthDBPassword",	"wl_radius_key"},


	{ "StorageApps",	"st_apps"},
	{ "SharedCount",	"sh_num"},
	{ "SharedPath",		"sh_path"},
	{ "SharedName",		"sh_name"},
	{ "SharedEnable",	"sh_rright_x"},
	{ "WriteEnable",	"sh_wright_x"},
	{ "UserCount",		"acc_num"},
	{ "UserName",		"acc_username"},
	{ "UserPassword",	"acc_password"},
	{ "x_SharedPath",	"sh_path_x"},
	{ "x_SharedName",	"sh_name_x"},
	{ "x_SharedComment",	"sh_comment_x"},
	{ "x_SharedMaxUser",	"sh_max_user_x"},
};


const char *findNVRAMName(const char *field)
{
	int idx;

	//dprintf("find : %s %s\n", serviceId, field);

	for (idx=0; idx < ARRAY_SIZE(maps); idx++) {
		if (strcmp(field, maps[idx].fid) == 0) {
			return maps[idx].name;
		}
	} 

	return field;
}
