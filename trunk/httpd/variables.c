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
	{"", 0 },
	{0, 0 }
}; 

static const struct variable variables_IPConnection_VSList[] = {
	{"vts_sipaddr_x", 16 },
	{"vts_port_x",    12 },
	{"vts_ipaddr_x",  16 },
	{"vts_lport_x",   7 },
	{"vts_proto_x",   8 },
	{"vts_protono_x", 12 },
	{"vts_desc_x",    20 },
	{0, 0 }
}; 

static const struct variable variables_IPConnection_TriggerList[] = {  
	{"autofw_outport_x",  12 },
	{"autofw_outproto_x", 7 },
	{"autofw_inport_x",   12 },
	{"autofw_inproto_x",  7 },
	{"autofw_desc_x",     18 },
	{0, 0 }
}; 

static const struct variable variables_PPPConnection_PPPoERouteList[] = {
	{"", 0 },
	{0, 0 }
}; 

static const struct variable variables_FirewallConfig_WLFilterList[] = {
	{"filter_wl_srcip_x",   16 },
	{"filter_wl_srcport_x", 12 },
	{"filter_wl_dstip_x",   16 },
	{"filter_wl_dstport_x", 12 },
	{"filter_wl_proto_x",   8 },
	{0, 0 }
}; 

static const struct variable variables_FirewallConfig_LWFilterList[] = {
	{"filter_lw_srcip_x",   16 },
	{"filter_lw_srcport_x", 12 },
	{"filter_lw_dstip_x",   16 },
	{"filter_lw_dstport_x", 12 },
	{"filter_lw_proto_x",   8 },
	{0, 0 }
}; 

static const struct variable variables_FirewallConfig_UrlList[] = {
	{"url_keyword_x", 36 },
	{0, 0 }
}; 

static const struct variable variables_FirewallConfig_WLocalFilterList[] = {
	{"", 0 },
	{0, 0 }
}; 

static const struct variable variables_FirewallConfig_DWFilterList[] = {
	{"", 0 },
	{0, 0 }
};

static const struct variable variables_FirewallConfig_WDFilterList[] = {
	{"", 0 },
	{0, 0 }
}; 

static const struct variable variables_FirewallConfig_DLFilterList[] = {
	{"", 0 },
	{0, 0 }
};

static const struct variable variables_FirewallConfig_LDFilterList[] = {
	{"", 0 },
	{0, 0 }
}; 

static const struct variable variables_FirewallConfig_MFList[] = {
	{"macfilter_list_x", 32 },
	{0, 0 }
}; 

static const struct variable variables_RouterConfig_GWStatic[] = {
	{"sr_ipaddr_x",  17 },
	{"sr_netmask_x", 17 },
	{"sr_gateway_x", 17 },
	{"sr_matric_x",  5 },
	{"sr_if_x",      3 },
	{0, 0 }
}; 

static const struct variable variables_RouterConfig_StaticRoute[] = {
	{"dr_staticipaddr_x",  20 },
	{"dr_staticnetmask_x", 11 },
	{"dr_staticgateway_x", 18 },
	{0, 0 }
};

static const struct variable variables_RouterConfig_RipSRoute[] = {
	{"", 0 },
	{0, 0 }
}; 

static const struct variable variables_RouterConfig_RouteFilter[] = {
	{"", 0 },
	{0, 0 }
}; 

static const struct variable variables_RouterConfig_RipOffset[] = {
	{"", 0 },
	{0, 0 }
}; 

static const struct variable variables_LANHostConfig_ManualDHCPList[] = {
	{"dhcp_staticmac_x",  14 },
	{"dhcp_staticip_x",   15 },
	{"dhcp_staticname_x", 32 },
	{0, 0 }
}; 

static const struct variable variables_LANHostConfig_DmzManualDHCPList[] = {
	{"", 0 },
	{0, 0 }
}; 

static const struct variable variables_LANHostConfig_ReservedAddressList[] = {
	{"", 0 },
	{0, 0 }
}; 

static const struct variable variables_DeviceSecurity11a_ACLList[] = {
	{"wl_maclist_x", 20 },
	{"wl_macdesc_x", 40 },
	{0, 0 }
}; 

static const struct variable variables_DeviceSecurity11a_AESList[] = {
	{"", 0 },
	{0, 0 }
}; 

static const struct variable variables_WLANAuthentication11a_LocalAuthDB[] = {
	{"", 0 },
	{0, 0 }
}; 

static const struct variable variables_WLANAuthentication11a_LocalCertDB[] = {
	{"", 0 },
	{0, 0 }
}; 

static const struct variable variables_WLANConfig11b_RBRList[] = {
	{"wl_wdslist_x", 32 },
	{0, 0 }
}; 

static const struct variable variables_PrinterStatus_x_FUserList[] = {
	{"usb_ftpusername_x", 18 },
	{"usb_ftppasswd_x",   18 },
	{"usb_ftprights_x",   16 },
	{0, 0 }
}; 

static const struct variable variables_PrinterStatus_x_FBanIPList[] = {
	{"usb_ftpbanip_x", 24 },
	{0, 0 }
}; 

static const struct variable variables_PrinterStatus_x_ExportsList[] = {
	{"usb_nfslist_x", 80 },
	{0, 0 }
}; 

static const struct variable variables_PrinterStatus_x_SharesList[] = {
	{"usb_smbdir_x",   20 },
	{"usb_smbshare_x", 20 },
	{"usb_smblevel_x", 14 },
	{"usb_smbdesc_x",  20 },
	{0, 0 }
}; 

static const struct variable variables_PrinterStatus_x_QRuleList[] = {
	{"qos_ipaddr_x", 16 },
	{"qos_port_x",   12 },
	{"qos_maxbw_x",  9 },
	{"qos_minbw_x",  7 },
	{0, 0 }
}; 

static const struct variable variables_PrinterStatus_x_UQRuleList[] = {
	{"qos_uport_x",  12 },
	{"qos_umaxbw_x", 9 },
	{"qos_uminbw_x", 7 },
	{0, 0 }
}; 

static const struct variable variables_General[] = {
	{"http_username", 0 },
	{"http_passwd", 0 },
	{0, 0 }
};

static const struct variable variables_Layer3Forwarding[] = {
	{"wan_proto", 0 },
	{"wan_mode_x", 0 },
	{"wan_etherspeed_x", 0 },
	{"wan_stb_x", 0 },
	{"wan_auth_x", 0 },
	{0, 0 }
};

static const struct variable variables_IPConnection[] = {
	{"wan_route_x", 0 },
	{"wan_nat_x", 0 },
	{"wan_ipaddr", 0 },
	{"wan_netmask", 0 },
	{"wan_gateway", 0 },
	{"wan_priority", 0 },
	{"wan_dnsenable_x", 0 },
	{"wan_dns1_x", 0 },
	{"wan_dns2_x", 0 },
	{"wan_nat_x", 0 },
	{"dmz_ip", 0 },
	{"sp_battle_ips", 0 },
	{"vts_enable_x", 0 },
	{"vts_num_x", 0 },
	{"autofw_enable_x", 0 },
	{"autofw_num_x", 0 },
	{0, 0 }
};

static const struct variable variables_PPPConnection[] = {
	{"wan_pppoe_username", 0 },
	{"wan_pppoe_passwd", 0 },
	{"wan_pppoe_idletime", 0 },
	{"wan_pppoe_txonly_x", 0 },
	{"wan_pppoe_options_x", 0 },
	{"wan_pptp_options_x", 0 },
	{"wan_pppoe_mtu", 0 },
	{"wan_pppoe_mru", 0 },
	{"wan_pppoe_service", 0 },
	{"wan_pppoe_ac", 0 },
	{"wan_pppoe_relay_x", 0 },
	{"wan_hostname", 0 },
	{"wan_hwaddr_x", 0 },
	{"wan_heartbeat_x", 0 },
	{0, 0 }
};

static const struct variable variables_FirewallConfig[] = {
	{"fw_enable_x", 0 },
	{"fw_dos_x", 0 },
	{"fw_log_x", 0 },
	{"misc_natlog_x", 0 },
	{"misc_http_x", 0 },
	{"misc_httpport_x", 0 },
	{"misc_lpr_x", 0 },
	{"misc_ping_x", 0 },
	{"misc_conntrack_x", 0 },
	{"recent_ssh_enable", 0 },
	{"recent_ftp_enable", 0 },
	{"recent_seconds", 0 },
	{"recent_hitcount", 0 },
	{"fw_wl_enable_x", 0 },
	{"filter_wl_date_x", 0 },
	{"filter_wl_time_x", 0 },
	{"filter_wl_default_x", 0 },
	{"filter_vs_default_x", 0 },
	{"filter_wl_icmp_x", 0 },
	{"fw_lw_enable_x", 0 },
	{"filter_lw_date_x", 0 },
	{"filter_lw_time_x", 0 },
	{"filter_lw_default_x", 0 },
	{"filter_lw_icmp_x", 0 },
	{"url_enable_x", 0 },
	{"url_date_x", 0 },
	{"url_time_x", 0 },
	{"url_num_x", 0 },
	{"filter_wl_num_x", 0 },
	{"filter_lw_num_x", 0 },
	{"macfilter_enable_x", 0 },
	{"macfilter_num_x", 0 },
	{0, 0 }
};

static const struct variable variables_RouterConfig[] = {
	{"dr_enable_x", 0 },
	{"mr_enable_x", 0 },
	{"sr_enable_x", 0 },
	{"sr_rip_x", 0 },
	{"sr_num_x", 0 },
	{"dr_static_rip_x", 0 },
	{"dr_static_matric_x", 0 },
	{"dr_default_x", 0 },
	{"dr_static_rip_x", 0 },
	{"dr_staticnum_x", 0 },
	{0, 0 }
};

static const struct variable variables_LANHostConfig[] = {
	{"lan_proto_x", 0 },
	{"lan_ipaddr", 0 },
	{"lan_netmask", 0 },
	{"lan_hostname", 0 },
	{"lan_gateway", 0 },
	{"lan_dns", 0 },
	{"dhcp_enable_x", 0 },
	{"lan_domain", 0 },
	{"dhcp_start", 0 },
	{"dhcp_end", 0 },
	{"dhcp_lease", 0 },
	{"dhcp_gateway_x", 0 },
	{"dhcp_dns1_x", 0 },
	{"dhcp_wins_x", 0 },
	{"dhcp_static_x", 0 },
	{"dhcp_staticnum_x", 0 },
	{"upnp_enable", 0 },
	{"upnp_proto", 0 },
	{"udpxy_enable_x", 0 },
	{"udpxy_wan_x", 0 },
	{"emf_enable", 0 },
	{"log_ipaddr", 0 },
	{"time_zone", 0 },
	{"time_interval", 0 },
	{"ntp_server0", 0 },
	{"ntp_server1", 0 },
	{"ntp_interval_x", 0 },
	{"ddns_enable_x", 0 },
	{"ddns_realip_x", 0 },
	{"ddns_server_x", 0 },
	{"ddns_username_x", 0 },
	{"ddns_passwd_x", 0 },
	{"ddns_hostname_x", 0 },
	{"ddns_wildcard_x", 0 },
	{"snmp_enable", 0 },
	{"snmp_community", 0 },
	{"snmp_contact", 0 },
	{"snmp_location", 0 },
	{0, 0 }
};

static const struct variable variables_WLANConfig11a[] = {
	{"", 0 },
	{0, 0 }
};

static const struct variable variables_DeviceSecurity11a[] = {
	{"wl_macmode", 0 },
	{"wl_macapply_x", 0 },
	{"wl_macnum_x", 0 },
	{0, 0 }
};

static const struct variable variables_WLANAuthentication11a[] = {
	{"wl_radius_ipaddr", 0 },
	{"wl_radius_port", 0 },
	{"wl_radius_key", 0 },
	{0, 0 }
};

static const struct variable variables_WLANConfig11b[] = {
	{"wl_country_code", 0 },
	{"wl_ssid", 0 },
	{"wl_mode_x", 0 },
	{"wl_channel", 0 },
	{"wl_wdsapply_x", 0 },
	{"wl_lazywds", 0 },
	{"wl_wdsnum_x", 0 },
	{"wl_gmode", 0 },
	{"wl_gmode_protection_x", 0 },
	{"wl_auth_mode", 0 },
	{"wl_wpa_mode", 0 },
	{"wl_crypto", 0 },
	{"wl_wpa_psk", 0 },
	{"wl_wep_x", 0 },
	{"wl_phrase_x", 0 },
	{"wl_key1", 0 },
	{"wl_key2", 0 },
	{"wl_key3", 0 },
	{"wl_key4", 0 },
	{"wl_key", 0 },
	{"wl_wpa_gtk_rekey", 0 },
	{"wl_afterburner", 0 },
	{"wl_closed", 0 },
	{"wl_ap_isolate", 0 },
	{"wl_rate", 0 },
	{"wl_mrate", 0 },
	{"wl_rateset", 0 },
	{"wl_reg_mode", 0 },
	{"wl_frag", 0 },
	{"wl_rts", 0 },
	{"wl_dtim", 0 },
	{"wl_bcn", 0 },
	{"wl_plcphdr", 0 },
	{"wl_frameburst", 0 },
	{"wl_mode_ex", 0 },
	{"wl_radio_x", 0 },
	{"wl_radio_date_x", 0 },
	{"wl_radio_time_x", 0 },
	{"wl_radio_power_x", 0 },
	{"wl_wme", 0 },
	{"wl_wme_no_ack", 0 },
	{"wl_wme_ap_bk", 0 },
	{"wl_wme_ap_be", 0 },
	{"wl_wme_ap_vi", 0 },
	{"wl_wme_ap_vo", 0 },
	{"wl_wme_sta_bk", 0 },
	{"wl_wme_sta_be", 0 },
	{"wl_wme_sta_vi", 0 },
	{"wl_wme_sta_vo", 0 },
	{"wl_preauth", 0 },
	{"wl_net_reauth", 0 },
	{"wl_nbw", 0 },
	{"wl_nctrlsb", 0 },
	{"wl_nband", 2 },
	{"wl_nmcsidx", 0 },
	{"wl_nmode", 0 },
	{"wl_leddc", 0 },
	{"wl_wme_apsd", 0 },
	{"wl_sta_retry_time", 5 },
	{"wl_wmf_bss_enable", 0 },
	{0, 0 }
};


static const struct variable variables_DeviceSecurity11b[] = {
	{"wl_macmode", 0 },
	{"wl_macapply_x", 0 },
	{"wl_macnum_x", 0 },
	{0, 0 }
};

static const struct variable variables_WLANAuthentication11b[] = {
	{"wl_radius_ipaddr", 0 },
	{"wl_radius_port", 0 },
	{"wl_radius_key", 0 },
	{0, 0 }
};

static const struct variable variables_PrinterStatus[] = {
	{"usb_webenable_x", 0 },
	{"usb_webdriver_x", 0 },
	{"usb_webformat_x", 0 },
	{"usb_webimage_x", 0 },
	{"usb_websense_x", 0 },
	{"usb_webrectime_x", 0 },
	{"usb_webfresh_x", 0 },
	{"usb_webquality_x", 0 },
	{"usb_webhttpport_x", 0 },
	{"usb_webhttpcheck_x", 0 },
	{"usb_webhttp_username", 0 },
	{"usb_webhttp_passwd", 0 },
	{"usb_websecurity_x", 0 },
	{"usb_websecurity_date_x", 0 },
	{"usb_websecurity_time_x", 0 },
	{"usb_websendto_x", 0 },
	{"usb_webmserver_x", 0 },
	{"usb_websubject_x", 0 },
	{"usb_webattach_x", 0 },
	{"usb_webremote_x", 0 },
	{"usb_webremote1_x", 0 },
	{"usb_webremote2_x", 0 },
	{"usb_webremote3_x", 0 },
	{"usb_webremote4_x", 0 },
	{"usb_webremote5_x", 0 },
	{"usb_webremote6_x", 0 },
	{"usb_storage_x", 0 },
	{"usb_smbenable_x", 0 },
	{"usb_smbhidden_x", 0 },
	{"usb_smbwrkgrp_x", 0 },
	{"usb_smbcpage_x", 0 },
	{"usb_ftpenable_x", 0 },
	{"usb_ftp_pasvenable_x", 0 },
	{"usb_ftp_pasvminport_x", 0 },
	{"usb_ftp_pasvmaxport_x", 0 },
	{"usb_ftpanonymous_x", 0 },
	{"usb_ftpsuper_x", 0 },
	{"usb_ftpport_x", 0 },
	{"usb_ftppubroot_x", 0 },
	{"usb_ftppvtroot_x", 0 },
	{"usb_ftpanonroot_x", 0 },
	{"usb_ftpdirlist_x", 0 },
	{"usb_ftpmax_x", 0 },
	{"usb_ftpipmax_x", 0 },
	{"usb_ftptimeout_x", 0 },
	{"usb_ftpstaytimeout_x", 0 },
	{"usb_ftpscript_x", 0 },
	{"usb_ftpscript_x", 0 },
	{"usb_ftprate_x", 0 },
	{"usb_ftpanonrate_x", 0 },
	{"usb_ftpnum_x", 0 },
	{"usb_bannum_x", 0 },
	{"qos_enable_x", 0 },
	{"qos_rulenum_x", 0 },
	{"qos_urulenum_x", 0 },
	{"usb_nfsenable_x", 0 },
	{"telnet_enable", 0 },
	{"ssh_enable", 0 },
	{"ssh_port", 0 },
	{"ssh_password_logins", 0 },
	{"lpr_enable", 0 },
	{"raw_enable", 0 },
	{"audio_enable", 0 },
	{"usb20_disable_x", 0 },
	{"lltd_enable", 0 },
	{"usb_ntfs3g_enable", 0 },
	{0, 0 }
};

static const struct variable variables_IPv6Config[] = {
	{"ipv6_proto", 0 },
	{"ipv6_if_x", 0 },
	{"ipv6_lanauto_x", 0 },
	{"ipv6_lan_addr", 0 },
	{"ipv6_lan_netsize", 0 },
	{"ipv6_wanauto_x", 0 },
	{"ipv6_wan_addr", 0 },
	{"ipv6_wan_netsize", 0 },
	{"ipv6_wan_router", 0 },
	{"ipv6_sit_remote", 0 },
	{"ipv6_sit_relay", 0 },
 	{"ipv6_6rd_router", 0 },
 	{"ipv6_6rd_ip4size", 0 },
	{"ipv6_sit_mtu", 0 },
	{"ipv6_sit_ttl", 0 },
	{"ipv6_dnsenable_x", 0 },
	{"ipv6_dns1_x", 0 },
	{"ipv6_dns2_x", 0 },
	{"ipv6_dns3_x", 0 },
	{"ipv6_radvd_enable", 0 },
//	{"ipv6_radvd_lifetime", 0 },
	{"ipv6_radvd_dns1_x", 0 },
	{ 0, 0 }
};

#ifdef __CONFIG_MADWIMAX__
static const struct variable variables_WiMaxConfig[] = {
	{"wan_wimax_ssid", 0 },
	{"wan_wimax_check", 0 },
	{"wan_wimax_interval", 0 },
	{"wan_wimax_restart", 0 },
	{"wan_wimax_syslog", 0 },
	{"wan_proto", 0 },
	{ 0, 0 }
};
#endif

#ifdef __CONFIG_MODEM__
static const struct variable variables_3GConfigvariables_3GConfig[] = {
	{"wan_modem_type", 0 },
	{"wan_modem_username", 0 },
	{"wan_modem_passwd", 0 },
	{"wan_modem_usbloc", 0 },
	{"wan_modem_apn", 0 },
	{"wan_modem_dialno", 0 },
	{"wan_modem_autodetect", 0 },
	{"wan_modem_vid", 0 },
	{"wan_modem_pid", 0 },
	{"wan_modem_portspeed", 0 },
	{"wan_modem_zerocd_mode", 0 },
	{"wan_modem_dialup_init", 0 },
	{"wan_modem_options", 0 },
	{"wan_modem_pdata", 0 },
	{"wan_modem_pui", 0 },
	{"wan_modem_standard", 0 },
	{"wan_modem_demand", 0 },
	{"wan_modem_idle", 0 },
	{"wan_modem_mtu", 0 },
	{"wan_modem_mru", 0 },
	{"wan_modem_onfailure", 0 },
	{"wan_modem_maxfail", 0 },
	{"wan_modem_init_script", 0 },
	{"wan_proto", 0 },
	{"wan_usbnet_subtype", 0 },
	{"wan_modem_at_connect", 0 },
	{"wan_modem_at_disconnect", 0 },
	{0, 0 }
};
#endif

static const struct group_variable grpList[] = {
	{"PortMappingList", variables_IPConnection_PortMappingList, 58, "PortMappingNumberOfEntries" },
	{"VSList", variables_IPConnection_VSList, 91, "vts_num_x" },
	{"TriggerList", variables_IPConnection_TriggerList, 56, "autofw_num_x" },
	{"PPPoERouteList", variables_PPPConnection_PPPoERouteList, 50, "PPPoERouteCount" },
	{"WLFilterList", variables_FirewallConfig_WLFilterList, 63, "filter_wl_num_x" },
	{"LWFilterList", variables_FirewallConfig_LWFilterList, 63, "filter_lw_num_x" },
	{"UrlList", variables_FirewallConfig_UrlList, 36, "url_num_x" },
	{"WLocalFilterList", variables_FirewallConfig_WLocalFilterList, 36, "WanLocalRuleCount" },
	{"DWFilterList", variables_FirewallConfig_DWFilterList, 63, "DmzWanRuleCount" },
	{"WDFilterList", variables_FirewallConfig_WDFilterList, 63, "WanDmzRuleCount" },
	{"DLFilterList", variables_FirewallConfig_DLFilterList, 63, "DmzLanRuleCount" },
	{"LDFilterList", variables_FirewallConfig_LDFilterList, 63, "LanDmzRuleCount" },
	{"MFList", variables_FirewallConfig_MFList, 32, "macfilter_num_x" },
	{"GWStatic", variables_RouterConfig_GWStatic, 59, "sr_num_x" },
	{"StaticRoute", variables_RouterConfig_StaticRoute, 46, "dr_staticnum_x" },
	{"RipSRoute", variables_RouterConfig_RipSRoute, 24, "RipSRouteCount" },
	{"RouteFilter", variables_RouterConfig_RouteFilter, 33, "RipDRouteCount" },
	{"RipOffset", variables_RouterConfig_RipOffset, 30, "RipOffsetCount" },
	{"ManualDHCPList", variables_LANHostConfig_ManualDHCPList, 61, "dhcp_staticnum_x" },
	{"DmzManualDHCPList", variables_LANHostConfig_DmzManualDHCPList, 29, "DmzManualDHCPCount" },
	{"ReservedAddressList", variables_LANHostConfig_ReservedAddressList, 24, "ReservedAddressListCount" },
	{"RBRList", variables_WLANConfig11b_RBRList, 32, "wl_wdsnum_x" },
	{"ACLList", variables_DeviceSecurity11a_ACLList, 60, "wl_macnum_x" },
	{"AESList", variables_DeviceSecurity11a_AESList, 58, "AESListCount" },
	{"LocalAuthDB", variables_WLANAuthentication11a_LocalAuthDB, 36, "AuthDBNumberOfEntries" },
	{"LocalCertDB", variables_WLANAuthentication11a_LocalCertDB, 40, "CertDBNumberOfEntries" },
	{"x_SharesList", variables_PrinterStatus_x_SharesList, 80, "usb_smbnum_x" },
	{"x_FUserList", variables_PrinterStatus_x_FUserList, 52, "usb_ftpnum_x" },
	{"x_FBanIPList", variables_PrinterStatus_x_FBanIPList, 24, "usb_bannum_x" },
	{"x_ExportsList", variables_PrinterStatus_x_ExportsList, 80, "usb_nfsnum_x" },
	{"x_QRuleList", variables_PrinterStatus_x_QRuleList, 44, "qos_rulenum_x" },
	{"x_UQRuleList", variables_PrinterStatus_x_UQRuleList, 28, "qos_urulenum_x" },
	{ 0, 0, 0, 0 }
};

static const struct svcLink svcLinks[] = {
	{"General",		 variables_General },
	{"LANHostConfig",	 variables_LANHostConfig },
	{"Layer3Forwarding",	 variables_Layer3Forwarding },
	{"IPConnection",	 variables_IPConnection },
	{"PPPConnection",	 variables_PPPConnection },
	{"FirewallConfig",	 variables_FirewallConfig },
	{"RouterConfig",	 variables_RouterConfig },
	{"WLANConfig11a",	 variables_WLANConfig11a },
	{"DeviceSecurity11a",	 variables_DeviceSecurity11a },
	{"WLANAuthentication11a", variables_WLANAuthentication11a },
	{"WLANConfig11b",	 variables_WLANConfig11b },
	{"DeviceSecurity11b",	 variables_DeviceSecurity11b },
	{"WLANAuthentication11b", variables_WLANAuthentication11b },
	{"PrinterStatus", 	 variables_PrinterStatus },
	{"IPv6Config",		 variables_IPv6Config },
#ifdef __CONFIG_MADWIMAX__
	{"WiMaxConfig",		 variables_WiMaxConfig },
#endif
#ifdef __CONFIG_MODEM__
	{"3GConfig",		 variables_3GConfigvariables_3GConfig },
#endif
	{0, 0 }
};
