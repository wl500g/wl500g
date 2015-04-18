// Web interface navigation script
// written by  dlukanidin
var navi_ver = 'ver. 1.100 from 30.03.2012';

// Global variables
var ui_script_id;
var lc_navi_script_id;
var lc_script_id;
var page_script_id;
var ext_menu_id;

var default_skin = 'wl500g';
var default_locale = 'en';
var skin_name = default_skin;
var locale_name = default_locale;

var common_obj = {
	'actions':[
		{'type':'button','id':'logout','text':'logout','on':'onclick="onClickLogout();"'},
		{'type':'button','id':'reboot','text':'reboot','on':'onclick="onClickReboot();"'}
	],
	'settings':[
		{'type':'select','id':'language','disabled':'1','help':'language_help','options':[
			{'value':'en','text':'English','state':'on'},
			{'value':'ru','text':'Russian','state':'off'}
		]},
		{'type':'select','id':'skin','help':'skin_help','options':[
			{'value':'wlg500','text':'wlg500_skin','state':'on'},
			{'value':'rtn16','text':'rtn16_skin','state':'off'},
			{'value':'rtn66','text':'rtn66_skin','state':'off'}
		]}
	],
	'status':[
		{'type':'label','id':'stat_stat','content':[
			{'type':'str','text':'status_mode'},
			{'type':'ref','id':'stat_mode','text':'status_router','href':'Advanced_OperationMode_Content'},
			{'type':'str','text':'status_fware'},
			{'type':'ref','id':'stat_vers','text':'status_version','href':'Advanced_FirmwareUpgrade_Content'},
			{'type':'str','text':'status_ssid'},
			{'type':'ref','id':'stat_ssid','text':'status_ssid24','href':'Advanced_Wireless_Content'}
		]}
	],
	'buttons':[
		{'type':'button','id':'restore','text':'restore','on':'onclick="onClickRestore();"'},
		{'type':'button','id':'finish','text':'finish','on':'onclick="onClickFinish();"'},
		{'type':'button','id':'apply','text':'apply','on':'onclick="onClickApply();"'}
	],
	'help':[
		{'type':'text','id':'id_restore','text':'restore_help','label':'restore_label'},
		{'type':'text','id':'id_finish','text':'finish_help','label':'finish_label'},
		{'type':'text','id':'id_apply','text':'apply_help','label':'apply_label'}
	]
};

var menu_obj = [
	{'type':'section','text':'sec_main','content':[
		{'type':'item','text':'menu_hm','page':'index'},
		{'type':'item','text':'menu_wz','page':'Basic_Operation_Content'},
		{'type':'item','text':'menu_wl','pages':[
			{'type':'subitem','text':'sub_wl_iface','page':'Advanced_Wireless_Content'},
			{'type':'subitem','text':'sub_wl_brdg','page':'Advanced_WMode_Content'},
			{'type':'subitem','text':'sub_wl_acc','page':'Advanced_ACL_Content'},
			{'type':'subitem','text':'sub_wl_rad','page':'Advanced_WSecurity_Content'},
			{'type':'subitem','text':'sub_wl_pro','page':'Advanced_WAdvanced_Content'}
		]},
		{'type':'item','text':'menu_ip','pages':[
			{'type':'subitem','text':'sub_ip_wan','page':'Advanced_WAN_Content'},
			{'type':'subitem','text':'sub_ip_lan','page':'Advanced_LAN_Content'},
			{'type':'subitem','text':'sub_ip_ipv6','page':'Advanced_IPv6_Content'},
			{'type':'subitem','text':'sub_ip_snmp','page':'Advanced_SNMP_Content'},
			{'type':'subitem','text':'sub_ip_dhcp','page':'Advanced_DHCP_Content'},
			{'type':'subitem','text':'sub_ip_rout','page':'Advanced_GWStaticRoute_Content'},
			{'type':'subitem','text':'sub_ip_misc','page':'Advanced_DDNS_Content'}
		]},
		{'type':'item','text':'menu_nt','pages':[
			{'type':'subitem','text':'sub_nt_port','page':'Advanced_PortTrigger_Content'},
			{'type':'subitem','text':'sub_nt_virt','page':'Advanced_VirtualServer_Content'},
			{'type':'subitem','text':'sub_nt_dmz','page':'Advanced_Exposed_Content'}
		]},
		{'type':'item','text':'menu_fw','pages':[
			{'type':'subitem','text':'sub_fw_base','page':'Advanced_BasicFirewall_Content'},
			{'type':'subitem','text':'sub_fw_filt','page':'Advanced_Firewall_Content'},
			{'type':'subitem','text':'sub_fw_url','page':'Advanced_URLFilter_Content'},
			{'type':'subitem','text':'sub_fw_mac','page':'Advanced_MACFilter_Content'}
		]},
		{'type':'item','text':'menu_qs','pages':[
			{'type':'subitem','text':'sub_qs_qos','page':'Advanced_QOS_Content'}
		]},
		{'type':'item','text':'menu_ua','pages':[
			{'type':'subitem','text':'sub_ua_ftp','page':'Advanced_USBStorage_Content'},
			{'type':'subitem','text':'sub_ua_smb','page':'Advanced_Samba_Content'},
			{'type':'subitem','text':'sub_ua_nfs','page':'Advanced_NFS_Content'},
			{'type':'subitem','text':'sub_ua_cam','page':'Advanced_WebCam_Content'}
		]},
		{'type':'item','text':'menu_un','pages':[
			{'type':'subitem','text':'sub_un_3g','page':'Advanced_ConnectionMode_Content'},
			{'type':'subitem','text':'sub_un_max','page':'Advanced_WiMax_Content'}
		]},
		{'type':'item','text':'menu_an','pages':[
			{'type':'subitem','text':'sub_an_mode','page':'Advanced_OperationMode_Content'},
			{'type':'subitem','text':'sub_an_serv','page':'Advanced_Services_Content'},
			{'type':'subitem','text':'sub_an_name','page':'Advanced_Name_Content'},
			{'type':'subitem','text':'sub_an_pass','page':'Advanced_Password_Content'},
			{'type':'subitem','text':'sub_an_frmw','page':'Advanced_FirmwareUpgrade_Content'},
			{'type':'subitem','text':'sub_an_set','page':'Advanced_SettingBackup_Content'},
			{'type':'subitem','text':'sub_an_flsh','page':'Advanced_FlashfsBackup_Content'},
			{'type':'subitem','text':'sub_an_def','page':'Advanced_FactoryDefault_Content'},
			{'type':'subitem','text':'sub_an_com','page':'Main_AdmStatus_Content'}
		]},
		{'type':'item','text':'menu_lg','pages':[
			{'type':'subitem','text':'sub_lg_stat','page':'Main_GStatus_Content'},
			{'type':'subitem','text':'sub_lg_wifi','page':'Main_WStatus_Content'},
			{'type':'subitem','text':'sub_lg_wmax','page':'Main_WiMaxStatus_Content'},
			{'type':'subitem','text':'sub_lg_uppp','page':'Main_ChatStatus_Content'},
			{'type':'subitem','text':'sub_lg_dhcp','page':'Main_DHCPStatus_Content'},
			{'type':'subitem','text':'sub_lg_port','page':'Main_IPTStatus_Content'},
			{'type':'subitem','text':'sub_lg_rout','page':'Advanced_RouteStatus_Content'},
			{'type':'subitem','text':'sub_lg_util','page':'Main_Utilization_Content'},
			{'type':'subitem','text':'sub_lg_diag','page':'Main_SysInfo_Content'},
			{'type':'subitem','text':'sub_lg_log','page':'Main_LogStatus_Content'}
		]}
	]}
];

// ON functions
function onClickLogout()
{
	if ( confirm(trNavi('logout_conf')) ) {
		location = "Logout.asp";
	}
}
function onClickReboot()
{
	if ( confirm(trNavi('reboot_conf')) ) {
		sendJSON('{"sys_reboot":""}');
		location = "Restarting.asp";
	}
}
function onClickRestore()
{
	// initPage();
	location.href = location.href;
}
function onClickFinish()
{
	var data = {};
	var page = getPage();
	getPageValues(page,data);
	saveData(data);
	var host = window.location.host;
	window.location = 'http://' + host + '/SaveRestart.asp';
}
function onClickApply()
{
	var data = {};
	var page = getPage();
	getPageValues(page,data);
	saveData(data);
}

// Goto page
function setPage(page)
{
	var host = location.host;
	location = 'http://' + host + '/'+page+'.asp';
}

// Init page locale
function initLocale(page_name) {
	var locale_name = getCurrentLocale();
	if ( ! loadFlags.lc_page ) {
		lc_script_id = loadModule(pathSys+'locales/'+locale_name+'/lc_'+page_name+'.js');
	}
}

// Init of navigation
function initNavi()
{
	var skin_name = getCurrentSkin();
	var locale_name = getCurrentLocale();
	
	loadDynamic('ext_menu_id','ext/ext_menu',on_ext_ready);
	if ( ! loadFlags.lc_navi ) {
		lc_navi_script_id = loadModule(pathSys+'locales/'+locale_name+'/lc_navi.js');
	}
	if ( ! loadFlags.skin ) {
		ui_script_id = loadModule(pathSys+'ui/ui_'+skin_name+'.js');
	}
}

// Update router status
function updateStatus(call_back)
{
	router_state.firmware_ver;
	queryServer('{"dump_file":"../.version"',function(obj){
		router_state.firmware_ver = obj.dump[1];
		router_state.mode = operationMode(router_state);
		router_state.ssid = router_state.wl_ssid;
		if ( typeof call_back === 'function' ) call_back(router_state);
	});
}	

// Add user menu
function on_ext_ready()
{
	if ( typeof ext_menu_obj == 'object' && show_ext_menu) {
		var j = menu_obj.length;
		for ( var i=0; i<ext_menu_obj.length; i++ , j++) menu_obj[j] = ext_menu_obj[i];
	}
}

// Get description of common elements
function getCommon()
{
	return common_obj;
}

// Get description of menu elements
function getMenu()
{
	return menu_obj;
}

// Get active menu elements
function getActiveMenu()
{
	var active = {};
	var current_page = getCurrentPage();
	for ( var i=0; i<menu_obj.length; i++ ) {
		var elem=menu_obj[i];
		if ( elem.type == 'item' && elem.page == current_page ) {
			active.mainitem = i;
			return active;
		} else if ( elem.type == 'section' ) {
			for ( var j=0; j<elem.content.length; j++ ) {
				var menu_item=elem.content[j];
				if ( menu_item && menu_item.page && menu_item.page == current_page ) {
					active.item = j;
					active.section = i;
					return active;
				} else if ( menu_item && menu_item.pages ) {
					for ( var k=0; k<menu_item.pages.length; k++ ) {
						var submenu_item=menu_item.pages[k];
						if ( submenu_item.page && submenu_item.page == current_page ) {
							active.subitem = k;
							active.item = j;
							active.section = i;
							return active;
						}
					}
				}
			}
		}
	}
	return active;
}

// Get current page
function getCurrentPage()
{
	if ( typeof getPage === 'function' ) {
		var id = getPage().id;
		if ( id ) return id;
	}
	var str = window.location.href;
	var i = str.lastIndexOf("/") + 1;
	str = str.substring(i);
	var url = str.split('.');
	if ( url[0] && url[0] !== '' ) return url[0];
	return 'index';
}

// Get current skin name
function getCurrentSkin()
{
	if ( skin_name == '' ) {
		skin_name = default_skin;
		sendJSON('{"nvram_set":{"www_skin":"'+default_skin+'"}}');
	}
	return skin_name;
}

// Get current locale name
function getCurrentLocale()
{
	if ( locale_name == '' ) {
		locale_name = default_locale;
		sendJSON('{"nvram_set":{"www_locale":"'+default_locale+'"}}');
	}
	return locale_name;
}

