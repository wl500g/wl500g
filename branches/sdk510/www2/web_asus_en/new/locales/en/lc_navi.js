// Web interface navigation localisation script
// en language
// written by  dlukanidin
var lc_navi_en_ver = 'ver. 1.100 from 30.03.2012';

// Global variables
var locale_navi_mess ={
	// Errors from global
	'no_answer_from_server':'No answer from server',
	'invalid_json':'Invalid JSON',
	'error_load_module':'Module not loaded',
	'error_in_mac':'Please enter the complete MAC address which contains 12 hexdecimal letters. Error string',
	'error_str_start':'This string can not start with "',
	'error_in_string':'This string can not contain',
	'error_in_int':'Please enter a value between',
	'error_in_ip':'It is not a valid IP address',
	'error_in_ipv6':'It is not a valid IPv6 address',
	'error_blank_ip':'IP fields can not be blank !!!',
	// Navi messages
	'restore':'Restore',
	'restore_help':'Clear the above settings and restore the settings in effect.',
	'restore_label':'Restore:',
	'finish':'Finish',
	'finish_help':'Confirm all settings and restart ZVMODELVZ now.',
	'finish_label':'Finish:',
	'apply':'Apply',
	'apply_help':'Confirm above settings and continue.',
	'apply_label':'Apply:',
	'logout':'Logout',
	'reboot':'Reboot',
	'status_mode':'Operation Mode:',
	'status_router':'Router',
	'status_fware':'Firmware Version:',
	'status_version':'1.0.0.0.0',
	'status_ssid':'SSID:',
	'status_ssid24':'ssid24',
	'status_ssid50':'ssid50',
	'logout_conf':'If you want to logout this session, please press "OK" and make sure this configuration window is closed.',
	'reboot_conf':'If you want to reboot Router, please press "OK" and wait 20 sec.',
	// Menu
	'sec_main':'Settings',
	'menu_hm':'Home',
	'menu_wz':'Quick Setup',
	'menu_wl':'Wireless',
	'menu_ip':'IP Config',
	'menu_nt':'NAT Setting',
	'menu_fw':'Internet Firewall',
	'menu_qs':'Bandwidth Management',
	'menu_ua':'USB Application',
	'menu_un':'USB Network Devices',
	'menu_an':'System Setup',
	'menu_lg':'Status &amp; Log',
	// Wireless submenu
	'sub_wl_iface':'Interface',
	'sub_wl_brdg':'Bridge',
	'sub_wl_acc':'Access Control',
	'sub_wl_rad':'RADIUS Setting',
	'sub_wl_pro':'Advanced',
	// WAN submenu
	'sub_ip_wan':'WAN',
	'sub_ip_lan':'LAN',
	'sub_ip_ipv6':'IPv6',
	'sub_ip_snmp':'SNMP',
	'sub_ip_dhcp':'DHCP Server',
	'sub_ip_rout':'Route',
	'sub_ip_misc':'Miscellaneous',
	// NAT Submenu
	'sub_nt_port':'Port Trigger',
	'sub_nt_virt':'Virtual Server',
	'sub_nt_dmz':'Virtual DMZ',
	// Firewall submenu
	'sub_fw_base':'Basic Config',
	'sub_fw_filt':'WAN and LAN Filter',
	'sub_fw_url':'URL Filter',
	'sub_fw_mac':'MAC Filter',
	// QOS submenu
	'sub_qs_qos':'Basic Config',
	// USB App submenu
	'sub_ua_ftp':'FTP Server',
	'sub_ua_smb':'Samba',
	'sub_ua_nfs':'NFS Server',
	'sub_ua_cam':'Web Camera',
	// USB Net submenu
	'sub_un_3g':'3G/CDMA Modem',
	'sub_un_max':'WiMAX',
	// System submenu
	'sub_an_mode':'Operation Mode',
	'sub_an_serv':'Services',
	'sub_an_name':'Change Name',
	'sub_an_pass':'Change Password',
	'sub_an_frmw':'Firmware Upgrade',
	'sub_an_set':'Setting Management',
	'sub_an_flsh':'Flashfs Management',
	'sub_an_def':'Factory Default',
	'sub_an_com':'System Command',
	// Status submenu
	'sub_lg_stat':'Status',
	'sub_lg_wifi':'WiFi',
	'sub_lg_wmax':'WiMAX',
	'sub_lg_uppp':'USB-PPP Log',
	'sub_lg_dhcp':'DHCP Leases',
	'sub_lg_port':'Port Forwarding',
	'sub_lg_rout':'Routing Table',
	'sub_lg_util':'System Utilization',
	'sub_lg_diag':'Diagnostic Info',
	'sub_lg_log':'System Log',
	'':''
};

// Localization of navigation
function trNavi(str)
{
	var result;
	try{		
		result=locale_navi_mess[str];
	} catch(err) {};
	if(!result) result=str;
	return result;
}

// Init module
function initLcNavi()
{
	loadFlags.lc_navi = true;
}

initLcNavi();
