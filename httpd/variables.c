/*
* variables_$ServiceId[] = { $Name, $Argu }
*
*$Argu : file and field to get value
*
* Variables are set in order (put dependent variables later).
*
* This data structure is generated automatically by script, maybe not 
* easy to read. But it really work!!!
*/

static const struct variable variables_IPConnection_PortMappingList[] = {
	{"", "7", NULL },
	{"", "0", NULL },
	{"", "0", NULL },
	{"", "16", NULL },
	{"", "6", NULL },
	{"", "25", NULL },
	{"", "6", NULL },
	{0, 0, 0 }
}; 

static const struct variable variables_IPConnection_ExposedIPList[] = {
	{"", "16", NULL },
	{"", "16", NULL },
	{"", "20", NULL },
	{0, 0, 0 }
}; 

static const struct variable variables_IPConnection_VSList[] = {
	{"vts_sipaddr_x", "16", NULL },
	{"vts_port_x", "12", NULL },
	{"vts_ipaddr_x", "16", NULL },
	{"vts_lport_x", "7", NULL },
	{"vts_proto_x", "8", NULL },
	{"vts_protono_x", "12", NULL },
	{"vts_desc_x", "20", NULL },
	{0, 0, 0 }
}; 

static const struct variable variables_IPConnection_TriggerList[] = {  
	{"autofw_outport_x", "12", NULL },
	{"autofw_outproto_x", "7", NULL },
	{"autofw_inport_x", "12", NULL },
	{"autofw_inproto_x", "7", NULL },
	{"autofw_desc_x", "18", NULL },
	{0, 0, 0 }
}; 

static const struct variable variables_PPPConnection_PPPoERouteList[] = {
	{"", "7", NULL },
	{"", "16", NULL },
	{"", "18", NULL },
	{"", "11", NULL },
	{0, 0, 0 }
}; 

static const struct variable variables_FirewallConfig_WLFilterList[] = {
	{"filter_wl_srcip_x", "16", NULL },
	{"filter_wl_srcport_x", "12", NULL },
	{"filter_wl_dstip_x", "16", NULL },
	{"filter_wl_dstport_x", "12", NULL },
	{"filter_wl_proto_x", "8", NULL },
	{0, 0, 0 }
}; 

static const struct variable variables_FirewallConfig_LWFilterList[] = {
	{"filter_lw_srcip_x", "16", NULL },
	{"filter_lw_srcport_x", "12", NULL },
	{"filter_lw_dstip_x", "16", NULL },
	{"filter_lw_dstport_x", "12", NULL },
	{"filter_lw_proto_x", "8", NULL },
	{0, 0, 0 }
}; 

static const struct variable variables_FirewallConfig_UrlList[] = {
	{"url_keyword_x", "36", NULL },
	{0, 0, 0 }
}; 

static const struct variable variables_FirewallConfig_WLocalFilterList[] = {
	{"", "16", NULL },
	{"", "12", NULL },
	{"", "8", NULL },
	{0, 0, 0 }
}; 

static const struct variable variables_FirewallConfig_DWFilterList[] = {
	{"", "16", NULL },
	{"", "12", NULL },
	{"", "16", NULL },
	{"", "12", NULL },
	{"", "8", NULL },
	{0, 0, 0 }
};

static const struct variable variables_FirewallConfig_WDFilterList[] = {
	{"", "16", NULL },
	{"", "12", NULL },
	{"", "16", NULL },
	{"", "12", NULL },
	{"", "8", NULL },
	{0, 0, 0 }
}; 

static const struct variable variables_FirewallConfig_DLFilterList[] = {
	{"", "16", NULL },
	{"", "12", NULL },
	{"", "16", NULL },
	{"", "12", NULL },
	{"", "8", NULL },
	{0, 0, 0 }
};

static const struct variable variables_FirewallConfig_LDFilterList[] = {
	{"", "16", NULL },
	{"", "12", NULL },
	{"", "16", NULL },
	{"", "12", NULL },
	{"", "8", NULL },
	{0, 0, 0 }
}; 

static const struct variable variables_FirewallConfig_MFList[] = {
	{"macfilter_list_x", "32", NULL },
	{0, 0, 0 }
}; 

static const struct variable variables_RouterConfig_GWStatic[] = {
	{"sr_ipaddr_x", "17", NULL },
	{"sr_netmask_x", "17", NULL },
	{"sr_gateway_x", "17", NULL },
	{"sr_matric_x", "5", NULL },
	{"sr_if_x", "3", NULL },
	{0, 0, 0 }
}; 

static const struct variable variables_RouterConfig_StaticRoute[] = {
	{"dr_staticipaddr_x", "20", NULL },
	{"dr_staticnetmask_x", "11", NULL },
	{"dr_staticgateway_x", "18", NULL },
	{0, 0, 0 }
};

static const struct variable variables_RouterConfig_RipSRoute[] = {
	{"", "17", NULL },
	{"", "7", NULL },
	{0, 0, 0 }
}; 

static const struct variable variables_RouterConfig_RouteFilter[] = {
	{"", "9", NULL },
	{"", "17", NULL },
	{"", "8", NULL },
	{0, 0, 0 }
}; 

static const struct variable variables_RouterConfig_RipOffset[] = {
	{"", "7", NULL },
	{"", "17", NULL },
	{"", "6", NULL },
	{0, 0, 0 }
}; 

static const struct variable variables_LANHostConfig_ManualDHCPList[] = {
	{"dhcp_staticmac_x", "14", NULL },
	{"dhcp_staticip_x", "15", NULL },
	{"dhcp_staticname_x", "32", NULL },
	{0, 0, 0 }
}; 

static const struct variable variables_LANHostConfig_DmzManualDHCPList[] = {
	{"", "14", NULL },
	{"", "15", NULL },
	{0, 0, 0 }
}; 

static const struct variable variables_LANHostConfig_ReservedAddressList[] = {
	{"", "24", NULL },
	{0, 0, 0 }
}; 

static const struct variable variables_DeviceSecurity11a_ACLList[] = {
	{"wl_maclist_x", "20", NULL },
	{"wl_macdesc_x", "40", NULL },
	{0, 0, 0 }
}; 

static const struct variable variables_DeviceSecurity11a_AESList[] = {
	{"", "16", NULL },
	{"", "10", NULL },
	{"", "32", NULL },
	{0, 0, 0 }
}; 

static const struct variable variables_WLANAuthentication11a_LocalAuthDB[] = {
	{"", "18", NULL },
	{"", "18", NULL },
	{0, 0, 0 }
}; 

static const struct variable variables_WLANAuthentication11a_LocalCertDB[] = {
	{"", "8", NULL },
	{"", "", NULL },
	{0, 0, 0 }
}; 

static const struct variable variables_WLANConfig11b_RBRList[] = {
	{"wl_wdslist_x", "32", NULL },
	{0, 0, 0 }
}; 

static const struct variable variables_DeviceSecurity11b_ACLList[] = {
	{"wl_maclist_x", "20", NULL },
	{"wl_macdesc_x", "40", NULL },
	{0, 0, 0 }
}; 

static const struct variable variables_DeviceSecurity11b_AESList[] = {
	{"", "16", NULL },
	{"", "10", NULL },
	{"", "32", NULL },
	{0, 0, 0 }
}; 

static const struct variable variables_WLANAuthentication11b_LocalAuthDB[] = {
	{"", "18", NULL },
	{"", "18", NULL },
	{0, 0, 0 }
}; 

static const struct variable variables_WLANAuthentication11b_LocalCertDB[] = {
	{"", "8", NULL },
	{"", "", NULL },
	{0, 0, 0 }
}; 

static const struct variable variables_PrinterStatus_x_FUserList[] = {
	{"usb_ftpusername_x", "18", NULL },
	{"usb_ftppasswd_x", "18", NULL },
	{"usb_ftprights_x", "16", NULL },
	{0, 0, 0 }
}; 

static const struct variable variables_PrinterStatus_x_FBanIPList[] = {
	{"usb_ftpbanip_x", "24", NULL },
	{0, 0, 0 }
}; 

static const struct variable variables_PrinterStatus_x_ExportsList[] = {
	{"usb_nfslist_x", "80", NULL },
	{0, 0, 0 }
}; 

static const struct variable variables_PrinterStatus_x_SharesList[] = {
	{"usb_smbdir_x", "20", NULL },
	{"usb_smbshare_x", "20", NULL },
	{"usb_smblevel_x", "14", NULL },
	{"usb_smbdesc_x", "20", NULL },
	{0, 0, 0 }
}; 

static const struct variable variables_PrinterStatus_x_QRuleList[] = {
	{"qos_ipaddr_x", "16", NULL },
	{"qos_port_x", "12", NULL },
	{"qos_maxbw_x", "9", NULL },
	{"qos_minbw_x", "7", NULL },
	{0, 0, 0 }
}; 

static const struct variable variables_PrinterStatus_x_UQRuleList[] = {
	{"qos_uport_x", "12", NULL },
	{"qos_umaxbw_x", "9", NULL },
	{"qos_uminbw_x", "7", NULL },
	{0, 0, 0 }
}; 

static const struct variable variables_General[] = {
	{"http_username", "", NULL },
	{"http_passwd", "", NULL },
	{"x_SystemUpTime", "Status", ARGV("general.log","UpTime") },
	{"x_ProductID", "Status", ARGV("general.log","ProductID") },
	{"x_FirmwareVersion", "Status", ARGV("general.log","FirmwareVer") },
	{"x_HardwareVersion", "Status", ARGV("general.log","HardwareVer") },
	{"x_BootloaderVersion", "Status", ARGV("general.log","BootloaderVer") },
	{0, 0, 0 }
};

static const struct variable variables_Layer3Forwarding[] = {
	{"", "", NULL },
	{"wan_proto", "", NULL },
	{"wan_mode_x", "", NULL },
	{"wan_etherspeed_x", "", NULL },
	{"wan_stb_x", "", NULL },
	{"wan_auth_x", "", NULL },
	{0, 0, 0 }
};

static const struct variable variables_WANCommonInterface[] = {
	{"", "", NULL },
	{"", "", NULL },
	{"", "", NULL },
	{"", "", NULL },
	{"", "", NULL },
	{"", "", NULL },
	{"", "", NULL },
	{"", "", NULL },
	{"", "", NULL },
	{"", "", NULL },
	{"", "", NULL },
	{"", "", NULL },
	{"", "", NULL },
	{"", "", NULL },
	{0, 0, 0 }
};

static const struct variable variables_IPConnection[] = {
	{"wan_route_x", "", NULL },
	{"", "", NULL },
	{"ConnectionStatus", "Status", ARGV("wan.log","IPLink") },
	{"Uptime", "Status", ARGV("wan.log","Uptime") },
	{"", "", NULL },
	{"wan_nat_x", "", NULL },
	{"", "", NULL },
	{"wan_ipaddr", "", NULL },
	{"wan_netmask", "", NULL },
	{"wan_gateway", "", NULL },
	{"wan_priority", "", NULL },
	{"wan_dnsenable_x", "", NULL },
	{"wan_dns1_x", "", NULL },
	{"wan_dns2_x", "", NULL },
	{"IPTablesInfo", "Status", ARGV("iptable.log","") },
	{"", "", NULL },
	{"", "", NULL },
	{"wan_nat_x", "", NULL },
	{"dmz_ip", "", NULL },
	{"sp_battle_ips", "", NULL },
	{"", "", NULL },
	{"vts_enable_x", "", NULL },
	{"vts_num_x", "", NULL },
	{"autofw_enable_x", "", NULL },
	{"autofw_num_x", "", NULL },
	{"", "", NULL },
	{"", "", NULL },
	{"", "", NULL },
	{"", "", NULL },
	{"", "", NULL },
	{"", "", NULL },
	{"", "", NULL },
	{"", "", NULL },
	{"", "", NULL },
	{"", "", NULL },
	{"", "", NULL },
	{"", "", NULL },
	{"", "", NULL },
	{"", "", NULL },
	{"", "", NULL },
	{"", "", NULL },
	{"", "", NULL },
	{"", "", NULL },
	{"", "", NULL },
	{"", "", NULL },
	{"", "", NULL },
	{"", "", NULL },
	{"", "", NULL },
	{"", "", NULL },
	{"PortMappingList", "Group", ARGV(variables_IPConnection_PortMappingList, "16", "58", "PortMappingNumberOfEntries") },
	{"ExposedIPList", "Group", ARGV(variables_IPConnection_ExposedIPList, "32", "52", "ExposedIPCount") },
	{"VSList", "Group", ARGV(variables_IPConnection_VSList, "24", "91", "vts_num_x") },
	{"TriggerList", "Group", ARGV(variables_IPConnection_TriggerList, "10", "56", "autofw_num_x") },
	{0, 0, 0 }
};

static const struct variable variables_PPPConnection[] = {
	{"", "", NULL },
	{"", "", NULL },
	{"wan_pppoe_username", "", NULL },
	{"wan_pppoe_passwd", "", NULL },
	{"ConnectionStatus", "Status", ARGV("wan.log","WANLink") },
	{"", "", NULL },
	{"", "", NULL },
	{"", "", NULL },
	{"", "", NULL },
	{"", "", NULL },
	{"wan_pppoe_idletime", "", NULL },
	{"wan_pppoe_txonly_x", "", NULL },
	{"wan_pppoe_options_x", "", NULL },
	{"wan_pptp_options_x", "", NULL },
	{"wan_pppoe_mtu", "", NULL },
	{"wan_pppoe_mru", "", NULL },
	{"", "", NULL },
	{"wan_pppoe_service", "", NULL },
	{"wan_pppoe_ac", "", NULL },
	{"wan_pppoe_relay_x", "", NULL },
	{"wan_hostname", "", NULL },
	{"wan_hwaddr_x", "", NULL },
	{"wan_heartbeat_x", "", NULL },
	{"", "", NULL },
	{"", "", NULL },
	{"", "", NULL },
	{"x_WANType", "Status", ARGV("wan.log","wan_proto_t") },
	{"x_WANIPAddress", "Status", ARGV("wan.log","wan_ipaddr_t") },
	{"x_WANSubnetMask", "Status", ARGV("wan.log","wan_netmask_t") },
	{"x_WANGateway", "Status", ARGV("wan.log","wan_gateway_t") },
	{"x_WANDNSServer", "Status", ARGV("wan.log","wan_dns_t") },
	{"x_WANLink", "Status", ARGV("wan.log","wan_status_t") },
	{"", "", NULL },
	{"x_DDNSStatus", "Status", ARGV("ddns.log","DDNSStatus") },
	{"", "", NULL },
	{"", "", NULL },
	{"", "", NULL },
	{"", "", NULL },
	{"", "", NULL },
	{"", "", NULL },
	{"", "", NULL },
	{"", "", NULL },
	{"", "", NULL },
	{"", "", NULL },
	{"", "", NULL },
	{"", "", NULL },
	{"", "", NULL },
	{"", "", NULL },
	{"", "", NULL },
	{"", "", NULL },
	{"", "", NULL },
	{"", "", NULL },
	{"", "", NULL },
	{"", "", NULL },
	{"", "", NULL },
	{"PPPoERouteList", "Group", ARGV(variables_PPPConnection_PPPoERouteList, "4", "50", "PPPoERouteCount") },
	{0, 0, 0 }
};

static const struct variable variables_EthernetLink[] = {
	{"", "", NULL },
	{0, 0, 0 }
};

static const struct variable variables_FirewallConfig[] = {
	{"", "", NULL },
	{"", "", NULL },
	{"", "", NULL },
	{"fw_enable_x", "", NULL },
	{"fw_dos_x", "", NULL },
	{"fw_log_x", "", NULL },
	{"misc_natlog_x", "", NULL },
	{"misc_http_x", "", NULL },
	{"misc_httpport_x", "", NULL },
	{"misc_lpr_x", "", NULL },
	{"misc_ping_x", "", NULL },
	{"misc_conntrack_x", "", NULL },
	{"recent_ssh_enable", "", NULL },
	{"recent_ftp_enable", "", NULL },
	{"recent_seconds", "", NULL },
	{"recent_hitcount", "", NULL },
	{"fw_wl_enable_x", "", NULL },
	{"filter_wl_date_x", "", NULL },
	{"filter_wl_time_x", "", NULL },
	{"filter_wl_default_x", "", NULL },
	{"filter_vs_default_x", "", NULL },
	{"filter_wl_icmp_x", "", NULL },
	{"", "", NULL },
	{"fw_lw_enable_x", "", NULL },
	{"filter_lw_date_x", "", NULL },
	{"filter_lw_time_x", "", NULL },
	{"filter_lw_default_x", "", NULL },
	{"filter_lw_icmp_x", "", NULL },
	{"FirewallLog", "Status", ARGV("firewall.log","") },
	{"SystemLog", "Status", ARGV("syslog.log","") },
	{"SystemCmd", "Status", ARGV("syscmd.log","") },
	{"url_enable_x", "", NULL },
	{"url_date_x", "", NULL },
	{"url_time_x", "", NULL },
	{"", "", NULL },
	{"url_num_x", "", NULL },
	{"filter_wl_num_x", "", NULL },
	{"filter_lw_num_x", "", NULL },
	{"", "", NULL },
	{"", "", NULL },
	{"", "", NULL },
	{"", "", NULL },
	{"", "", NULL },
	{"", "", NULL },
	{"", "", NULL },
	{"", "", NULL },
	{"", "", NULL },
	{"", "", NULL },
	{"", "", NULL },
	{"", "", NULL },
	{"", "", NULL },
	{"", "", NULL },
	{"", "", NULL },
	{"", "", NULL },
	{"", "", NULL },
	{"", "", NULL },
	{"", "", NULL },
	{"", "", NULL },
	{"", "", NULL },
	{"", "", NULL },
	{"", "", NULL },
	{"", "", NULL },
	{"", "", NULL },
	{"", "", NULL },
	{"", "", NULL },
	{"", "", NULL },
	{"", "", NULL },
	{"macfilter_enable_x", "", NULL },
	{"macfilter_num_x", "", NULL },
	{"WLFilterList", "Group", ARGV(variables_FirewallConfig_WLFilterList, "32", "63", "filter_wl_num_x") },
	{"LWFilterList", "Group", ARGV(variables_FirewallConfig_LWFilterList, "32", "63", "filter_lw_num_x") },
	{"UrlList", "Group", ARGV(variables_FirewallConfig_UrlList, "128", "36", "url_num_x") },
	{"WLocalFilterList", "Group", ARGV(variables_FirewallConfig_WLocalFilterList, "32", "36", "WanLocalRuleCount") },
	{"DWFilterList", "Group", ARGV(variables_FirewallConfig_DWFilterList, "32", "63", "DmzWanRuleCount") },
	{"WDFilterList", "Group", ARGV(variables_FirewallConfig_WDFilterList, "32", "63", "WanDmzRuleCount") },
	{"DLFilterList", "Group", ARGV(variables_FirewallConfig_DLFilterList, "32", "63", "DmzLanRuleCount") },
	{"LDFilterList", "Group", ARGV(variables_FirewallConfig_LDFilterList, "32", "63", "LanDmzRuleCount") },
	{"MFList", "Group", ARGV(variables_FirewallConfig_MFList, "16", "32", "macfilter_num_x") },
	{0, 0, 0 }
};

static const struct variable variables_RouterConfig[] = {
	{"dr_enable_x", "", NULL },
	{"mr_enable_x", "", NULL },
	{"sr_enable_x", "", NULL },
	{"sr_rip_x", "", NULL },
	{"sr_num_x", "", NULL },
	{"dr_static_rip_x", "", NULL },
	{"dr_static_matric_x", "", NULL },
	{"dr_default_x", "", NULL },
	{"RouteInfo", "Status", ARGV("route.log","") },
	{"dr_static_rip_x", "", NULL },
	{"dr_staticnum_x", "", NULL },
	{"", "", NULL },
	{"", "", NULL },
	{"", "", NULL },
	{"", "", NULL },
	{"", "", NULL },
	{"", "", NULL },
	{"", "", NULL },
	{"", "", NULL },
	{"", "", NULL },
	{"", "", NULL },
	{"", "", NULL },
	{"", "", NULL },
	{"", "", NULL },
	{"", "", NULL },
	{"", "", NULL },
	{"", "", NULL },
	{"", "", NULL },
	{"", "", NULL },
	{"", "", NULL },
	{"", "", NULL },
	{"", "", NULL },
	{"", "", NULL },
	{"", "", NULL },
	{"GWStatic", "Group", ARGV(variables_RouterConfig_GWStatic, "16", "59", "sr_num_x") },
	{"StaticRoute", "Group", ARGV(variables_RouterConfig_StaticRoute, "16", "46", "dr_staticnum_x") },
	{"RipSRoute", "Group", ARGV(variables_RouterConfig_RipSRoute, "16", "24", "RipSRouteCount") },
	{"RouteFilter", "Group", ARGV(variables_RouterConfig_RouteFilter, "16", "33", "RipDRouteCount") },
	{"RipOffset", "Group", ARGV(variables_RouterConfig_RipOffset, "16", "30", "RipOffsetCount") },
	{0, 0, 0 }
};

static const struct variable variables_LANHostConfig[] = {
	{"", "", NULL },
	{"lan_proto_x", "", NULL },
	{"lan_ipaddr", "", NULL },
	{"lan_netmask", "", NULL },
	{"x_LANIPAddress", "Status", ARGV("lan.log","lan_ipaddr_t") },
	{"x_LANSubnetMask", "Status", ARGV("lan.log","lan_netmask_t") },
	{"x_LANGateway", "Status", ARGV("lan.log","lan_gateway_t") },
	{"lan_hostname", "", NULL },
	{"lan_gateway", "", NULL },
	{"lan_dns", "", NULL },
	{"dhcp_enable_x", "", NULL },
	{"", "", NULL },
	{"", "", NULL },
	{"lan_domain", "", NULL },
	{"dhcp_start", "", NULL },
	{"dhcp_end", "", NULL },
	{"dhcp_lease", "", NULL },
	{"dhcp_gateway_x", "", NULL },
	{"dhcp_dns1_x", "", NULL },
	{"x_LDNSServer2", "Status", ARGV("LANHostConfig","lan_ipaddr") },
	{"dhcp_wins_x", "", NULL },
	{"dhcp_static_x", "", NULL },
	{"dhcp_staticnum_x", "", NULL },
	{"", "", NULL },
	{"DHCPLog", "Status", ARGV("leases.log","") },
	{"", "", NULL },
	{"", "", NULL },
	{"", "", NULL },
	{"", "", NULL },
	{"", "", NULL },
	{"", "", NULL },
	{"x_DmzLDNSServer2", "Status", ARGV("FirewallConfig","DmzIP") },
	{"", "", NULL },
	{"", "", NULL },
	{"", "", NULL },
	{"DmzDHCPLog", "Status", ARGV("dleases.log","") },
	{"upnp_enable", "", NULL },
	{"upnp_proto", "", NULL },
	{"udpxy_enable_x", "", NULL },
	{"udpxy_wan_x", "", NULL },
#ifdef __CONFIG_EMF__
	{"emf_enable", "", NULL },
#endif
	{"log_ipaddr", "", NULL },
	{"time_zone", "", NULL },
	{"time_interval", "", NULL },
	{"ntp_server0", "", NULL },
	{"ntp_server1", "", NULL },
	{"ntp_interval_x", "", NULL },
	{"", "", NULL },
	{"", "", NULL },
	{"ddns_enable_x", "", NULL },
	{"ddns_realip_x", "", NULL },
	{"ddns_server_x", "", NULL },
	{"ddns_username_x", "", NULL },
	{"ddns_passwd_x", "", NULL },
	{"ddns_hostname_x", "", NULL },
	{"ddns_wildcard_x", "", NULL },
	{"x_DDNSStatus", "Status", ARGV("ddns.log","DDNSStatus") },
	{"snmp_enable", "", NULL },
	{"snmp_community", "", NULL },
	{"snmp_contact", "", NULL },
	{"snmp_location", "", NULL },
	{"ManualDHCPList", "Group", ARGV(variables_LANHostConfig_ManualDHCPList, "32", "61", "dhcp_staticnum_x") },
	{"DmzManualDHCPList", "Group", ARGV(variables_LANHostConfig_DmzManualDHCPList, "8", "29", "DmzManualDHCPCount") },
	{"ReservedAddressList", "Group", ARGV(variables_LANHostConfig_ReservedAddressList, "6", "24", "ReservedAddressListCount") },
	{0, 0, 0 }
};

static const struct variable variables_WLANConfig11a[] = {
	{"", "", NULL },
	{"", "", NULL },
	{"", "", NULL },
	{"", "", NULL },
	{"", "", NULL },
	{"", "", NULL },
	{"", "", NULL },
	{"", "", NULL },
	{"", "", NULL },
	{"", "", NULL },
	{"", "", NULL },
	{"", "", NULL },
	{"", "", NULL },
	{"", "", NULL },
	{"", "", NULL },
	{"", "", NULL },
	{"", "", NULL },
	{"WirelessLog", "Status", ARGV("wlan11a.log","") },
	{0, 0, 0 }
};

static const struct variable variables_DeviceSecurity11a[] = {
	{"", "", NULL },
	{"wl_macmode", "", NULL },
	{"wl_macapply_x", "", NULL },
	{"wl_macnum_x", "", NULL },
	{"", "", NULL },
	{"", "", NULL },
	{"ACLList", "Group", ARGV(variables_DeviceSecurity11a_ACLList, "64", "60", "wl_macnum_x") },
	{"AESList", "Group", ARGV(variables_DeviceSecurity11a_AESList, "64", "58", "AESListCount") },
	{0, 0, 0 }
};

static const struct variable variables_WLANAuthentication11a[] = {
	{"", "", NULL },
	{"wl_radius_ipaddr", "", NULL },
	{"wl_radius_port", "", NULL },
	{"wl_radius_key", "", NULL },
	{"", "", NULL },
	{"", "", NULL },
	{"", "", NULL },
	{"LocalAuthDB", "Group", ARGV(variables_WLANAuthentication11a_LocalAuthDB, "32", "36", "AuthDBNumberOfEntries") },
	{"LocalCertDB", "Group", ARGV(variables_WLANAuthentication11a_LocalCertDB, "32", "40", "CertDBNumberOfEntries") },
	{0, 0, 0 }
};

static const struct variable variables_WLANConfig11b[] = {
	{"", "", NULL },
	{"", "", NULL },
	{"wl_country_code", "", NULL },
	{"wl_ssid", "", NULL },
	{"", "", NULL },
	{"wl_mode_x", "", NULL },
	{"wl_channel", "", NULL },
	{"wl_wdsapply_x", "", NULL },
	{"wl_lazywds", "", NULL },
	{"wl_wdsnum_x", "", NULL },
	{"wl_gmode", "", NULL },
	{"wl_gmode_protection_x", "", NULL },
	{"wl_auth_mode", "", NULL },
	{"wl_wpa_mode", "", NULL },
	{"wl_crypto", "", NULL },
	{"wl_wpa_psk", "", NULL },
	{"wl_wep_x", "", NULL },
	{"wl_phrase_x", "", NULL },
	{"wl_key1", "", NULL },
	{"wl_key2", "", NULL },
	{"wl_key3", "", NULL },
	{"wl_key4", "", NULL },
	{"wl_key", "", NULL },
	{"wl_wpa_gtk_rekey", "", NULL },
	{"", "", NULL },
	{"wl_afterburner", "", NULL },
	{"wl_closed", "", NULL },
	{"wl_ap_isolate", "", NULL },
	{"wl_rate", "", NULL },
	{"wl_mrate", "", NULL },
	{"wl_rateset", "", NULL },
	{"wl_reg_mode", "", NULL },
	{"wl_frag", "", NULL },
	{"wl_rts", "", NULL },
	{"wl_dtim", "", NULL },
	{"wl_bcn", "", NULL },
	{"wl_frameburst", "", NULL },
	{"wl_mode_ex", "", NULL },
	{"wl_radio_x", "", NULL },
	{"wl_radio_date_x", "", NULL },
	{"wl_radio_time_x", "", NULL },
	{"wl_radio_power_x", "", NULL },
	{"WirelessLog", "Status", ARGV("wlan11b.log","") },
	{"", "", NULL },
	{"wl_wme", "", NULL },
	{"wl_wme_no_ack", "", NULL },
	{"wl_wme_ap_bk", "", NULL },
	{"wl_wme_ap_be", "", NULL },
	{"wl_wme_ap_vi", "", NULL },
	{"wl_wme_ap_vo", "", NULL },
	{"wl_wme_sta_bk", "", NULL },
	{"wl_wme_sta_be", "", NULL },
	{"wl_wme_sta_vi", "", NULL },
	{"wl_wme_sta_vo", "", NULL },
	{"wl_preauth", "", NULL },
	{"wl_net_reauth", "", NULL },
	{"wl_nbw", "", NULL },
	{"wl_nctrlsb", "", NULL },
	{"wl_nband", "2", NULL },
	{"wl_nmcsidx", "-1", NULL },
	{"wl_nmode", "-1", NULL },
	{"wl_leddc", "0x640000", NULL },
	{"wl_wme_apsd", "on", NULL },
	{"wl_sta_retry_time", "5", NULL },
	{"wl_wmf_bss_enable", "", NULL },
	{"RBRList", "Group", ARGV(variables_WLANConfig11b_RBRList, "16", "32", "wl_wdsnum_x") },
	{0, 0, 0 }
};


static const struct variable variables_DeviceSecurity11b[] = {
	{"", "", NULL },
	{"wl_macmode", "", NULL },
	{"wl_macapply_x", "", NULL },
	{"wl_macnum_x", "", NULL },
	{"", "", NULL },
	{"", "", NULL }, 
	{"ACLList", "Group", ARGV(variables_DeviceSecurity11b_ACLList, "64", "60", "wl_macnum_x") },
	{"AESList", "Group", ARGV(variables_DeviceSecurity11b_AESList, "64", "58", "AESListCount") },
	{0, 0, 0 }
};

static const struct variable variables_WLANAuthentication11b[] = {
	{"", "", NULL },
	{"wl_radius_ipaddr", "", NULL },
	{"wl_radius_port", "", NULL },
	{"wl_radius_key", "", NULL },
	{"", "", NULL },
	{"", "", NULL },
	{"", "", NULL },
	{"LocalAuthDB", "Group", ARGV(variables_WLANAuthentication11b_LocalAuthDB, "32", "36", "AuthDBNumberOfEntries") },
	{"LocalCertDB", "Group", ARGV(variables_WLANAuthentication11b_LocalCertDB, "32", "40", "CertDBNumberOfEntries") },
	{0, 0, 0 }
};

static const struct variable variables_PrinterStatus[] = {
	{"x_PrinterModel", "Status", ARGV("printer_status.log","printer_model_t") },
	{"x_PrinterStatus", "Status", ARGV("printer_status.log","printer_status_t") },
	{"x_PrinterUser", "Status", ARGV("printer_status.log","printer_user_t") },
	{"", "", NULL },
	{"usb_webenable_x", "", NULL },
	{"usb_webdriver_x", "", NULL },
	{"usb_webformat_x", "", NULL },
	{"usb_webimage_x", "", NULL },
	{"usb_websense_x", "", NULL },
	{"usb_webrectime_x", "", NULL },
	{"usb_webfresh_x", "", NULL },
	{"usb_webquality_x", "", NULL },
	{"usb_webhttpport_x", "", NULL },
	{"usb_webhttpcheck_x", "", NULL },
	{"usb_webhttp_username", "", NULL },
	{"usb_webhttp_passwd", "", NULL },
	{"usb_websecurity_x", "", NULL },
	{"usb_websecurity_date_x", "", NULL },
	{"usb_websecurity_time_x", "", NULL },
	{"usb_websendto_x", "", NULL },
	{"usb_webmserver_x", "", NULL },
	{"usb_websubject_x", "", NULL },
	{"usb_webattach_x", "", NULL },
	{"", "", NULL },
	{"usb_webremote_x", "", NULL },
	{"usb_webremote1_x", "", NULL },
	{"usb_webremote2_x", "", NULL },
	{"usb_webremote3_x", "", NULL },
	{"usb_webremote4_x", "", NULL },
	{"usb_webremote5_x", "", NULL },
	{"usb_webremote6_x", "", NULL },
	{"x_FEject", "Status", ARGV("ddns.log","DDNSStatus") },
	{"usb_storage_x", "", NULL },
	{"usb_smbenable_x", "", NULL },
	{"usb_smbhidden_x", "", NULL },
	{"usb_smbwrkgrp_x", "", NULL },
	{"usb_smbcpage_x", "", NULL },
	{"x_SharesList", "Group", ARGV(variables_PrinterStatus_x_SharesList, "32", "80", "usb_smbnum_x") },
	{"usb_ftpenable_x", "", NULL },
	{"usb_ftp_pasvenable_x", "", NULL },
	{"usb_ftp_pasvminport_x", "", NULL },
	{"usb_ftp_pasvmaxport_x", "", NULL },
	{"usb_ftpanonymous_x", "", NULL },
	{"usb_ftpsuper_x", "", NULL },
	{"usb_ftpport_x", "", NULL },
	{"usb_ftppubroot_x", "", NULL },
	{"usb_ftppvtroot_x", "", NULL },
	{"usb_ftpanonroot_x", "", NULL },
	{"usb_ftpdirlist_x", "", NULL },
	{"usb_ftpmax_x", "", NULL },
	{"usb_ftpipmax_x", "", NULL },
	{"usb_ftptimeout_x", "", NULL },
	{"usb_ftpstaytimeout_x", "", NULL },
	{"usb_ftpscript_x", "", NULL },
	{"usb_ftpscript_x", "", NULL },
	{"usb_ftprate_x", "", NULL },
	{"usb_ftpanonrate_x", "", NULL },
	{"", "", NULL },
	{"usb_ftpnum_x", "", NULL },
	{"usb_bannum_x", "", NULL },
	{"", "", NULL },
	{"qos_enable_x", "", NULL },
	{"qos_rulenum_x", "", NULL },
	{"qos_urulenum_x", "", NULL },
	{"x_FUserList", "Group", ARGV(variables_PrinterStatus_x_FUserList, "32", "52", "usb_ftpnum_x") },
	{"x_FBanIPList", "Group", ARGV(variables_PrinterStatus_x_FBanIPList, "16", "24", "usb_bannum_x") },
	{"usb_nfsenable_x", "", NULL },
	{"x_ExportsList", "Group", ARGV(variables_PrinterStatus_x_ExportsList, "32", "80", "usb_nfsnum_x") },
	{"x_QRuleList", "Group", ARGV(variables_PrinterStatus_x_QRuleList, "32", "44", "qos_rulenum_x") },
	{"x_UQRuleList", "Group", ARGV(variables_PrinterStatus_x_UQRuleList, "32", "28", "qos_urulenum_x") },
	{"telnet_enable", "", NULL },
	{"ssh_enable", "", NULL },
	{"ssh_port", "", NULL },
	{"ssh_password_logins", "", NULL },
	{"lpr_enable", "", NULL },
	{"raw_enable", "", NULL },
	{"audio_enable", "", NULL },
	{"usb20_disable_x", "", NULL },
	{"lltd_enable", "", NULL },
	{"usb_ntfs3g_enable", "", NULL },
	{0, 0, 0 }
};

static const struct variable variables_IPv6Config[] = {
	{"ipv6_proto", "", NULL },
	{"ipv6_if_x", "", NULL },
	{"ipv6_lanauto_x", "", NULL },
	{"ipv6_lan_addr", "", NULL },
	{"ipv6_lan_netsize", "", NULL },
	{"ipv6_wanauto_x", "", NULL },
	{"ipv6_wan_addr", "", NULL },
	{"ipv6_wan_netsize", "", NULL },
	{"ipv6_wan_router", "", NULL },
	{"ipv6_sit_remote", "", NULL },
	{"ipv6_sit_relay", "", NULL },
 	{"ipv6_6rd_router", "", NULL },
 	{"ipv6_6rd_ip4size", "", NULL },
	{"ipv6_sit_mtu", "", NULL },
	{"ipv6_sit_ttl", "", NULL },
	{"ipv6_dnsenable_x", "", NULL },
	{"ipv6_dns1_x", "", NULL },
	{"ipv6_dns2_x", "", NULL },
	{"ipv6_dns3_x", "", NULL },
	{"ipv6_radvd_enable", "", NULL },
//	{"ipv6_radvd_lifetime", "", NULL },
	{"ipv6_radvd_dns1_x", "", NULL },
	{ 0, 0, 0 }
};

#ifdef __CONFIG_MADWIMAX__
static const struct variable variables_WiMaxConfig[] = {
	{"wan_wimax_ssid", "", NULL },
	{"wan_wimax_check", "", NULL },
	{"wan_wimax_interval", "", NULL },
	{"wan_wimax_restart", "", NULL },
	{"wan_wimax_syslog", "", NULL },
	{"wan_proto", "", NULL },
	{ 0, 0, 0 }
};
#endif

#ifdef __CONFIG_MODEM__
static const struct variable variables_3GConfigvariables_3GConfig[] = {
	{"wan_modem_type","", NULL },
	{"wan_modem_username", "", NULL },
	{"wan_modem_passwd", "", NULL },
	{"wan_modem_usbloc", "", NULL },
	{"wan_modem_apn", "", NULL },
	{"wan_modem_dialno", "", NULL },
	{"wan_modem_autodetect", "", NULL },
	{"wan_modem_vid", "", NULL },
	{"wan_modem_pid", "", NULL },
	{"wan_modem_portspeed", "", NULL },
	{"wan_modem_zerocd_mode", "", NULL },
	{"wan_modem_dialup_init", "", NULL },
	{"wan_modem_options", "", NULL },
	{"wan_modem_pdata", "", NULL },
	{"wan_modem_pui", "", NULL },
	{"wan_modem_standard", "", NULL },
	{"wan_modem_demand", "", NULL },
	{"wan_modem_idle", "", NULL },
	{"wan_modem_mtu", "", NULL },
	{"wan_modem_mru", "", NULL },
	{"wan_modem_onfailure", "", NULL },
	{"wan_modem_maxfail", "", NULL },
	{"wan_modem_init_script", "", NULL },
	{"wan_proto", "", NULL },
	{0, 0, 0 }
};
#endif


static const struct svcLink svcLinks[] = {  
	{"General",		 variables_General },
	{"LANHostConfig", variables_LANHostConfig },
	{"Layer3Forwarding", variables_Layer3Forwarding },
	{"WANCommonInterface", variables_WANCommonInterface }, 
	{"IPConnection",	 variables_IPConnection },
	{"PPPConnection",	 variables_PPPConnection },
	{"EthernetLink",	 variables_EthernetLink },
	{"FirewallConfig",	 variables_FirewallConfig },
	{"RouterConfig",	 variables_RouterConfig },
	{"WLANConfig11a",	 variables_WLANConfig11a },
	{"DeviceSecurity11a", variables_DeviceSecurity11a },
	{"WLANAuthentication11a", variables_WLANAuthentication11a },
	{"WLANConfig11b",	 variables_WLANConfig11b },
	{"DeviceSecurity11b", variables_DeviceSecurity11b },
	{"WLANAuthentication11b", variables_WLANAuthentication11b },
	{"PrinterStatus", 	 variables_PrinterStatus },
	{"IPv6Config",	 variables_IPv6Config },
#ifdef __CONFIG_MADWIMAX__
	{"WiMaxConfig",	 variables_WiMaxConfig },
#endif
#ifdef __CONFIG_MODEM__
	{"3GConfig",	 variables_3GConfigvariables_3GConfig },
#endif
	{0, 0 }
};
