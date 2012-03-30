// Web interface page localisation script
// WAN page, en language
// written by  dlukanidin
var lc_wan_lan_en_ver = 'ver. 1.100 from 30.03.2012'

// Global variables
var locale_mess ={
	'yes':'Yes',
	'no':'No',
	'page_header':'IP Config - WAN',
	'page_subheader':
		'ZVMODELVZ supports several connection types to WAN. These types are selected from<br>'+
		'the drop-down menu beside WAN Connection Type. The setting fields will differ depending<br>'+
		'on what kind of connection type you select.',
	'wan_type':'WAN Connection Type:',
	'wan_dhcp':'Automatic IP',
	'wan_static':'Static IP',
	'wan_pppoe':'PPPoE',
	'wan_pptp':'PPTP',
	'wan_l2tp':'L2TP',
	'wan_bigpond':'BigPond',
	'wan_wimax':'WiMAX',
	'wan_usbmodem':'USB Modem',
	'speed_wan':'WAN Connection Speed:',
	'speed_auto':'Auto negotiation',
	'speed_10half':'10Mbps half-duplex',
	'speed_10full':'10Mbps full-duplex',
	'speed_100half':'100Mpbs half-duplex',
	'speed_100full':'100Mpbs full-duplex',
	'iptv_port':'IPTV STB Port:',
	'iptv_help':'Choose the LAN port to bridge to WAN port. If you have another device to connect to WAN but your ISP only provide one WAN link, you can specify some LAN port to receive packets from WAN port. For example, you can connect your IPTV Set-top box to the specified port and get the signal and IP address from your ISP directly. Do not enable this function if you use a manual configuration of vlan ports',
	'iptv_0':'None',
	'iptv_1':'LAN1',
	'iptv_2':'LAN2',
	'iptv_3':'LAN3',
	'iptv_4':'LAN4',
	'iptv_5':'LAN3 & LAN4',
	'wanip_secheader':'WAN IP Setting',
	'get_ip_auto':'Get IP automatically?',
	'wanip_label':'IP Address:',
	'wanip_help':'This is IP address of ZVMODELVZ as seen on the remote network. If you set it to 0.0.0.0, ZVMODELVZ will get IP address from DHCP Server automatically.',
	'netmask_label':'Subnet Mask:',
	'netmask_help':'This is Subnet Mask of ZVMODELVZ as seen on the remote network.',
	'gateway_label':'Default Gateway:',
	'gateway_help':'This is the IP address of default gateway that allows for contact between ZVMODELVZ and the remote network or host.',
	'wandns_secheader':'WAN DNS Setting',
	'dnsenable_auto':'Get DNS Server automatically?',
	'dns1_label':'DNS Server1:',
	'dns1_help':'This field indicates the IP address of DNS that ZVMODELVZ contact to.',
	'dns2_label':'DNS Server2:',
	'dns2_help':'This field indicates the IP address of DNS that ZVMODELVZ contact to.',
	'pppoe_secheader':'PPPoE, PPTP or L2TP Account',
	'uname_label':'User Name:',
	'uname_help':'This field is only available, when you set WAN Connection Type as PPPoE.',
	'upass_label':'Password:',
	'upass_help':'This field is only available, when you set WAN Connection Type as PPPoE.',
	'idle_label':'Idle Disconnect Time in seconds(option):',
	'idle_help':'This field allows you to configure to terminate your ISP connection after a specified period of time. A value of zero allows infinite idle time. If Tx Only is checked, the data from Internet will be skipped for counting idle time.',
	'idle_check':'Tx Only',
	'mtu_label':'MTU:',
	'mtu_help':'That is Maximum Transmission Unit(MTU) of PPPoE packet.',
	'mru_label':'MRU:',
	'mru_help':'That is Maximum Receive Unit(MRU) of PPPoE packet.',
	'serv_label':'Service Name(option):',
	'serv_help':'This item may be specified by some ISPs. Check with your ISP and fill them in if required.',
	'conc_label':'Access Concentrator Name(option):',
	'conc_help':'This item may be specified by some ISPs. Check with your ISP and fill them in if required.',
	'pptp_label':'PPTP Options:',
	'pptp_help':'This item may be specified by some ISPs. Check with your ISP and fill them in if required',
	'pptp_none':'None',
	'pptp_mppc':'No Encryption',
	'pptp_mppe40':'MPPE 40',
	'pptp_mppe56':'MPPE 56',
	'pptp_mppe128':'MPPE 128',
	'pppd_label':'Additional pppd options:',
	'pppd_help':'This item may be specified by some ISPs. Check with your ISP and fill them in if required.',
	'relay_label':'Enable PPPoE Relay?',
	'relay_help':'Enable PPPoE relay allows stations in LAN to setup individual PPPoE connections that are passthrough NAT.',
	'spec_secheader':'Special Requirement from ISP',
	'hname_label':'Host Name:',
	'hname_help':'This field allows you to provide a host name for ZVMODELVZ. It is usually requested by your ISP.',
	'mac_label':'MAC Address:',
	'mac_help':'This field allows you to provide a unique MAC address for ZVMODELVZ to connect Internet. It is usually requested by your ISP.',
	'heart_label':'Heart-Beat or PPTP/L2TP (VPN) Server:',
//	'lan_secheader':'LAN IP Setting',
//	'lname_label':'Host Name:',
//	'lname_help':'This field allows you to provide a LAN host name for ZVMODELVZ.',
//	'lip_label':'IP Address:',
//	'lip_help':'This is IP Address of ZVMODELVZ as seen in your local network. The default value is 192.168.1.1.',
//	'lmask_label':'Subnet Mask:',
//	'lmask_help':'This is Subnet Mask of ZVMODELVZ as seen in your local network. The default value is 255.255.255.0.',
	'bad_static':'As you leave IP or subnet mask fields blank, connection type will be set as Automatic IP!',
	'':''
};

// Localization of string
function tr(str)
{
	var result;
	try{		
		result=locale_mess[str];
	} catch(err) {};
	if(!result) result=str;
	return result;
}

// Init module
function initLcPage()
{
	loadFlags.lc_page = true;
}

initLcPage();
