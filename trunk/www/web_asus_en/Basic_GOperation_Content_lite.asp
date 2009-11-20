<head>
<title>ZVMODELVZ Web Manager</title>
<meta http-equiv="Content-Type" content="text/html; charset=ISO-8859-1">
<meta http-equiv="Content-Type" content="text/html; charset=UTF-8">
<meta http-equiv="Content-Type" content="text/html; charset=gb2312">
<link rel="stylesheet" type="text/css" href="style.css" media="screen"></link>
<script language="JavaScript" type="text/javascript" src="overlib.js"></script>
<script language="JavaScript" type="text/javascript" src="general.js"></script>
<script language="JavaScript" type="text/javascript" src="quick.js"></script>
</head>
<body bgcolor="#FFFFFF" onLoad="loadQuick()">
<div id="overDiv" style="position:absolute; visibility:hidden; z-index:1000;"></div>
<form method="GET" name="form" action="apply.cgi">
<!-- Table for the conntent page -->
<table width="666" border="0" cellpadding="0" cellspacing="0">
<input type="hidden" name="x_Mode" value="0">
<input type="hidden" name="current_page" value="Basic_GOperation_Content.asp">
<input type="hidden" name="next_page" value="Basic_HomeGateway_SaveRestart.asp">
<input type="hidden" name="next_host" value="">
<input type="hidden" name="sid_list" value="Layer3Forwarding;IPConnection;PPPConnection;WLANConfig11a;WLANConfig11b;LANHostConfig;FirewallConfig;">
<input type="hidden" name="action_mode" value="">
<input type="hidden" name="modified" value="0">
<input type="hidden" name="group_id" value="">
<input type="hidden" name="wan_proto" value="<% nvram_get_x("Layer3Forwarding","wan_proto"); %>">
<input type="hidden" name="wan_route_x" value="<% nvram_get_x("IPConnection","wan_route_x"); %>">
<input type="hidden" name="wan_nat_x" value="<% nvram_get_x("IPConnection","wan_nat_x"); %>">
<input type="hidden" name="wl_auth_mode" value="<% nvram_get_x("WLANConfig11b","wl_auth_mode"); %>">
<input type="hidden" name="wl_crypto" value="<% nvram_get_x("WLANConfig11b","wl_crypto"); %>">
<input type="hidden" name="wl_wep_x" value="<% nvram_get_x("WLANConfig11b","wl_wep_x"); %>">
<tr>
<td>
<table width="666" border="1" cellpadding="0" cellspacing="0" bordercolor="E0E0E0">
<tr id="Country" class="content_header_tr">
<td class="content_header_td_title" colspan="2">Quick Setup</td>
</tr>
<tr class="content_section_header_tr">
<td class="content_section_header_td" colspan="2">Select Time Zone</td>
</tr>
<tr>
<td class="content_desc_td" colspan="2" height="50">
Please choose the time zone where you are locating in.</td>
</tr>
<tr>
<td class="content_header_td" onMouseOver="return overlib('This field indicates time zone where you are locating in.', LEFT);" onMouseOut="return nd();">Time Zone:</td><td class="content_input_td"><select name="time_zone" class="content_input_fd" onChange="return change_common(this, 'LANHostConfig', 'time_zone')"><option class="content_input_fd" value="UCT11" <% nvram_match_x("LANHostConfig","time_zone", "UCT11","selected"); %>>(GMT-11:00) Midway Island, Samoa</option><option class="content_input_fd" value="UCT10" <% nvram_match_x("LANHostConfig","time_zone", "UCT10","selected"); %>>(GMT-10:00) Hawaii</option><option class="content_input_fd" value="NAST9NADT" <% nvram_match_x("LANHostConfig","time_zone", "NAST9NADT","selected"); %>>(GMT-09:00) Alaska</option><option class="content_input_fd" value="PST8PDT" <% nvram_match_x("LANHostConfig","time_zone", "PST8PDT","selected"); %>>(GMT-08:00) Pacific Time</option><option class="content_input_fd" value="MST7MDT" <% nvram_match_x("LANHostConfig","time_zone", "MST7MDT","selected"); %>>(GMT-07:00) Mountain Time</option><option class="content_input_fd" value="MST7" <% nvram_match_x("LANHostConfig","time_zone", "MST7","selected"); %>>(GMT-07:00) Arizona</option><option class="content_input_fd" value="CST6CDT" <% nvram_match_x("LANHostConfig","time_zone", "CST6CDT","selected"); %>>(GMT-06:00) Central Time</option><option class="content_input_fd" value="UCT6" <% nvram_match_x("LANHostConfig","time_zone", "UCT6","selected"); %>>(GMT-06:00) Middle America</option><option class="content_input_fd" value="UCT5" <% nvram_match_x("LANHostConfig","time_zone", "UCT5","selected"); %>>(GMT-05:00) Indiana East, Colombia</option><option class="content_input_fd" value="EST5EDT" <% nvram_match_x("LANHostConfig","time_zone", "EST5EDT","selected"); %>>(GMT-05:00) Eastern Time</option><option class="content_input_fd" value="AST4ADT" <% nvram_match_x("LANHostConfig","time_zone", "AST4ADT","selected"); %>>(GMT-04:00) Atlantic Time, Brazil West</option><option class="content_input_fd" value="UCT4" <% nvram_match_x("LANHostConfig","time_zone", "UCT4","selected"); %>>(GMT-04:00) Bolivia, Venezuela</option><option class="content_input_fd" value="UCT3" <% nvram_match_x("LANHostConfig","time_zone", "UCT3","selected"); %>>(GMT-03:00) Guyana</option><option class="content_input_fd" value="EBST3EBDT" <% nvram_match_x("LANHostConfig","time_zone", "EBST3EBDT","selected"); %>>(GMT-03:00) Brazil East, Greenland</option><option class="content_input_fd" value="NORO2" <% nvram_match_x("LANHostConfig","time_zone", "NORO2","selected"); %>>(GMT-02:00) Mid-Atlantic</option><option class="content_input_fd" value="UCT0" <% nvram_match_x("LANHostConfig","time_zone", "UCT0","selected"); %>>(GMT) Gambia, Liberia, Morocco</option><option class="content_input_fd" value="GMT0BST" <% nvram_match_x("LANHostConfig","time_zone", "GMT0BST","selected"); %>>(GMT) England</option><option class="content_input_fd" value="MET-1METDST" <% nvram_match_x("LANHostConfig","time_zone", "MET-1METDST","selected"); %>>(GMT+01:00) Netherland, France, Italy</option><option class="content_input_fd" value="MEZ-1MESZ" <% nvram_match_x("LANHostConfig","time_zone", "MEZ-1MESZ","selected"); %>>(GMT+01:00) Germany</option><option class="content_input_fd" value="UCT-1" <% nvram_match_x("LANHostConfig","time_zone", "UCT-1","selected"); %>>(GMT+01:00) Tunisia</option><option class="content_input_fd" value="EET-2EETDST" <% nvram_match_x("LANHostConfig","time_zone", "EET-2EETDST","selected"); %>>(GMT+02:00) Greece, Ukraine, Turkey</option><option class="content_input_fd" value="SAST-2" <% nvram_match_x("LANHostConfig","time_zone", "SAST-2","selected"); %>>(GMT+02:00) South Africa</option><option class="content_input_fd" value="IST-3IDT" <% nvram_match_x("LANHostConfig","time_zone", "IST-3IDT","selected"); %>>(GMT+03:00) Iraq, Jordan, Kuwait</option><option class="content_input_fd" value="MSK-3MSD" <% nvram_match_x("LANHostConfig","time_zone", "MSK-3MSD","selected"); %>>(GMT+03:00) Moscow Winter Time</option><option class="content_input_fd" value="UCT-4" <% nvram_match_x("LANHostConfig","time_zone", "UCT-4","selected"); %>>(GMT+04:00) Armenia</option><option class="content_input_fd" value="UCT-5" <% nvram_match_x("LANHostConfig","time_zone", "UCT-5","selected"); %>>(GMT+05:00) Pakistan, Russia</option><option class="content_input_fd" value="UCT-6" <% nvram_match_x("LANHostConfig","time_zone", "UCT-6","selected"); %>>(GMT+06:00) Bangladesh, Russia</option><option class="content_input_fd" value="UCT-7" <% nvram_match_x("LANHostConfig","time_zone", "UCT-7","selected"); %>>(GMT+07:00) Thailand, Russia</option><option class="content_input_fd" value="CST-8" <% nvram_match_x("LANHostConfig","time_zone", "CST-8","selected"); %>>(GMT+08:00) China Coast, Hong Kong</option><option class="content_input_fd" value="CCT-8" <% nvram_match_x("LANHostConfig","time_zone", "CCT-8","selected"); %>>(GMT+08:00) Taipei</option><option class="content_input_fd" value="SST-8" <% nvram_match_x("LANHostConfig","time_zone", "SST-8","selected"); %>>(GMT+08:00) Singapore</option><option class="content_input_fd" value="WAS-8WAD" <% nvram_match_x("LANHostConfig","time_zone", "WAS-8WAD","selected"); %>>(GMT+08:00) Australia West</option><option class="content_input_fd" value="JST-9" <% nvram_match_x("LANHostConfig","time_zone", "JST-9","selected"); %>>(GMT+09:00) Japan, Korea</option><option class="content_input_fd" value="KST-9KDT" <% nvram_match_x("LANHostConfig","time_zone", "KST-9KDT","selected"); %>>(GMT+09:00) Korean</option><option class="content_input_fd" value="UCT-10" <% nvram_match_x("LANHostConfig","time_zone", "UCT-10","selected"); %>>(GMT+10:00) Guam, Russia</option><option class="content_input_fd" value="EST-10EDT" <% nvram_match_x("LANHostConfig","time_zone", "EST-10EDT","selected"); %>>(GMT+10:00) Australia</option><option class="content_input_fd" value="UCT-11" <% nvram_match_x("LANHostConfig","time_zone", "UCT-11","selected"); %>>(GMT+11:00) Solomon Islands</option><option class="content_input_fd" value="UCT-12" <% nvram_match_x("LANHostConfig","time_zone", "UCT-12","selected"); %>>(GMT+12:00) Fiji</option><option class="content_input_fd" value="NZST-12NZDT" <% nvram_match_x("LANHostConfig","time_zone", "NZST-12NZDT","selected"); %>>(GMT+12:00) New Zealand</option></select></td>
</tr>
<tr>
<td class="content_input_td" colspan="2">
<table>
<tr>
<td width="500" height="100"></td><td>
<div align="center">
<font face="Arial"><input class=inputSubmit onMouseOut="buttonOut(this)" onMouseOver="buttonOver(this)" onClick="toPrevTag('Country')" type="hidden" value="Prev" name="action1"></font>&nbsp;&nbsp;
<font face="Arial"> <input class=inputSubmit onMouseOut="buttonOut(this)" onMouseOver="buttonOver(this)" onClick="toTag('Country')" type="button" value="Next" name="action"></font>
</div>
</td>
</tr>
<tr><td colspan="2" height="360"></td></tr>
</table>
</td>
</tr>
<tr id="Broadband" class="content_header_tr">
<td class="content_header_td_title" colspan="2">Quick Setup</td>
</tr>
<tr class="content_section_header_tr">
<td class="content_section_header_td" colspan="2">Select Internet Connection Type</td>
</tr>
<tr>
<td class="content_desc_td" colspan="2" height="50">
ZVMODELVZ supports several kinds of connection to Internet through its WAN port. Please select connection type you need. In addition, before getting on Internet, please make sure you have connected ZVMODELVZ's WAN port to your DSL or Cable Modem.</td>
</tr>
<tr> 
          <td class="content_header_td_less" onMouseOut="return nd();" colspan="2">          
          <p></p>
          <p><input type="radio" checked name="x_WANType" value="1" class="content_input_fd" onClick="changeWANType()">Cable Modem or other connection type that gets IP automatically.</p>          
          <p><input type="radio" checked name="x_WANType" value="2" class="content_input_fd" onClick="changeWANType()">ADSL connection that requires username and password. It is known as PPPoE.</p>          
          <p><input type="radio" checked name="x_WANType" value="3" class="content_input_fd" onClick="changeWANType()">ADSL connection that requires username, password and IP address. It is known as PPTP.</p>                    
          <p></p>
          <p></p>
          </td>
</tr>
<tr>
<td class="content_input_td" colspan="2">
<table>
<tr>
<td width="444" height="100"></td><td>
<div align="center">
<font face="Arial"><input class=inputSubmit onMouseOut="buttonOut(this)" onMouseOver="buttonOver(this)" onClick="toPrevTag('Broadband')" type="button" value="Prev" name="action"></font>&nbsp;&nbsp;
<font face="Arial"> <input class=inputSubmit onMouseOut="buttonOut(this)" onMouseOver="buttonOver(this)" onClick="toTag('Broadband')" type="button" value="Next" name="action"></font>
</div>
</td>
</tr>
<tr><td colspan="2" height="360"></td></tr>
</table>
</td>
</tr>
<tr id="PPPoE" class="content_header_tr">
<td class="content_header_td_title" colspan="2">Quick Setup</td>
</tr>
<tr class="content_section_header_tr">
<td  class="content_section_header_td" colspan="2">Set Your Account to ISP</td>
</tr>
<tr>
<td class="content_desc_td" colspan="2" height="50">If you apply an account with dynamic IP. You must get user account and password from your ISP. Please fill this data into the following fields carefully. Or, if you apply an ADSL account with static IP, just ignore user name and pasword information.</td>
</tr>
<tr>
<td class="content_header_td_less">User Name:</td><td class="content_input_td"><input type="text" maxlength="64" size="32" name="wan_pppoe_username" class="content_input_fd" value="<% nvram_get_x("PPPConnection","wan_pppoe_username"); %>"></td>
</tr>
<tr>
<td class="content_header_td_less">Password:</td><td class="content_input_td"><input type="password" maxlength="64" size="32" name="wan_pppoe_passwd" class="content_input_fd" value="<% nvram_get_x("PPPConnection","wan_pppoe_passwd"); %>"></td>
</tr>
<tr>
<td class="content_input_td" colspan="2">
<table>
<tr>
<td width="444" height="100"></td>
<td>
<div align="center">
<font face="Arial"><input class=inputSubmit onMouseOut="buttonOut(this)" onMouseOver="buttonOver(this)" onClick="toPrevTag('PPPoE')" type="button" value="Prev" name="action"></font>&nbsp;&nbsp;
<font face="Arial"><input class=inputSubmit onMouseOut="buttonOut(this)" onMouseOver="buttonOver(this)" onClick="toTag('PPPoE')" type="button" value="Next" name="action"></font>
</div>
</td>
</tr>
<tr><td colspan="2" height="360"></td></tr>
</table>
</td>
</tr>
<tr id="MacHost" class="content_header_tr">
<td class="content_header_td_title" colspan="2">Quick Setup</td>
</tr>
<tr class="content_section_header_tr">
<td  class="content_section_header_td" colspan="2">Fill Information Required by ISP</td>
</tr>
<tr>
<td class="content_desc_td" colspan="2" height="50">Your ISP may require the following information to identify your account. If not, just press Next to ignore it.</td>
</tr>
<tr>
<td class="content_header_td_less" onMouseOver="return overlib('This field allows you to provide a host name for ZVMODELVZ. It is usually requested by your ISP.', LEFT);" onMouseOut="return nd();">Host Name:</td><td class="content_input_td"><input type="text" maxlength="32" size="32" name="wan_hostname" class="content_input_fd" value="<% nvram_get_x("PPPConnection","wan_hostname"); %>"></td>
</tr>
<tr>
<td class="content_header_td_less" onMouseOver="return overlib('This field allows you to provide a unique MAC address for ZVMODELVZ to connect to Internet. It is usually requested by your ISP.', LEFT);" onMouseOut="return nd();">MAC Address:</td><td class="content_input_td"><input type="text" maxlength="12" size="12" name="wan_hwaddr_x" class="content_input_fd" value="<% nvram_get_x("PPPConnection","wan_hwaddr_x"); %>" onBlur="return validate_hwaddr(this)" onKeyPress="return is_hwaddr()"></td>
</tr>
<tr>
<td class="content_header_td_less" onMouseOver="return overlib('Please enter the server name or server ip of the authentication server of BigPond service.', LEFT);" onMouseOut="return nd();">Heart-Beat Server:</td><td class="content_input_td"><input type="text" maxlength="12" size="32" name="wan_heartbeat_x" class="content_input_fd" value="<% nvram_get_x("PPPConnection","wan_heartbeat_x"); %>"></td>
</tr>
<tr>
<td class="content_input_td_less" colspan="2">
<table>
<tr>
<td width="444" height="100"></td>
<td>
<div align="center">
<font face="Arial"><input class=inputSubmit onMouseOut="buttonOut(this)" onMouseOver="buttonOver(this)" onClick="toPrevTag('MacHost')" type="button" value="Prev" name="action"></font>&nbsp;&nbsp;
<font face="Arial"><input class=inputSubmit onMouseOut="buttonOut(this)" onMouseOver="buttonOver(this)" onClick="toTag('MacHost')" type="button" value="Next" name="action"></font>
</div>
</td>
</tr>
<tr><td colspan="2" height="360"></td></tr>
</table>
</td>
</tr>
<tr id="WANSetting" class="content_header_tr">
<td class="content_header_td_title" colspan="2">Quick Setup</td>
</tr>
<tr class="content_section_header_tr">
<td class="content_section_header_td" colspan="2">WAN IP Setting</td>
</tr>
<tr>
<td class="content_desc_td" colspan="2" height="50">Fill TCP/IP setting for ZVMODELVZ to connect to Internet through WAN port.</td>
</tr>
<tr>
<td class="content_header_td_less">Get IP automatically?</td><td class="content_input_td"><input type="radio" value="1" name="x_DHCPClient" class="content_input_fd" onClick="changeDHCPClient()">Yes</input><input type="radio" value="0" name="x_DHCPClient" class="content_input_fd" onClick="changeDHCPClient()">No</input></td>
</tr>
<tr>
<td class="content_header_td_less" onMouseOver="return overlib('IP address of WAN Interface. If you leave it blank, ZVMODELVZ will get IP address from DHCP Server automatically.', LEFT);" onMouseOut="return nd();">IP Address:</td><td class="content_input_td"><input type="text" maxlength="15" size="15" name="wan_ipaddr" class="content_input_fd" value="<% nvram_get_x("IPConnection","wan_ipaddr"); %>" onBlur="return validate_ipaddr(this, 'wan_ipaddr')" onKeyPress="return is_ipaddr(this)"></td>
</tr>
<tr>
<td class="content_header_td_less">Subnet Mask:</td><td class="content_input_td"><input type="text" maxlength="15" size="15" name="wan_netmask" class="content_input_fd" value="<% nvram_get_x("IPConnection","wan_netmask"); %>" onBlur="return validate_ipaddr(this)" onKeyPress="return is_ipaddr(this)"></td>
</tr>
<tr>
<td class="content_header_td_less">Default Gateway:</td><td class="content_input_td"><input type="text" maxlength="15" size="15" name="wan_gateway" class="content_input_fd" value="<% nvram_get_x("IPConnection","wan_gateway"); %>" onBlur="return validate_ipaddr(this)" onKeyPress="return is_ipaddr(this)"></td>
</tr>
<tr>
<td class="content_header_td_less">Get DNS Server automatically?</td><td class="content_input_td"><input type="radio" value="1" name="wan_dnsenable_x" class="content_input_fd" onClick="changeDNSServer()" <% nvram_match_x("IPConnection","wan_dnsenable_x", "1", "checked"); %>>Yes</input><input type="radio" value="0" name="wan_dnsenable_x" class="content_input_fd" onClick="changeDNSServer()" <% nvram_match_x("IPConnection","wan_dnsenable_x", "0", "checked"); %>>No</input></td>
</tr>
<tr>
<td class="content_header_td_less">DNS Server 1:</td><td class="content_input_td"><input type="text" maxlength="15" size="15" name="wan_dns1_x" class="content_input_fd" value="<% nvram_get_x("IPConnection","wan_dns1_x"); %>" onBlur="return validate_ipaddr(this)" onKeyPress="return is_ipaddr(this)"></td>
</tr>
<tr>
<td class="content_header_td_less">DNS Server 2:</td><td class="content_input_td"><input type="text" maxlength="15" size="15" name="wan_dns2_x" class="content_input_fd" value="<% nvram_get_x("IPConnection","wan_dns2_x"); %>" onBlur="return validate_ipaddr(this)" onKeyPress="return is_ipaddr(this)"></td>
</tr>
<tr>
<td class="content_input_td" colspan="2">
<table>
<tr>
<td width="444" height="100"></td><td>
<div align="center">
<font face="Arial"><input class=inputSubmit onMouseOut="buttonOut(this)" onMouseOver="buttonOver(this)" onClick="toPrevTag('WANSetting')" type="button" value="Prev" name="action"></font>&nbsp;&nbsp;
<font face="Arial"><input class=inputSubmit onMouseOut="buttonOut(this)" onMouseOver="buttonOver(this)" onClick="toTag('WANSetting')" type="button" value="Next" name="action"></font>
</div>
</td>
</tr>
<tr><td colspan="2" height="360"></td></tr>
</table>
</td>
</tr>

<tr  id="Wireless"  class="content_header_tr">
<td class="content_header_td_title" colspan="2">Quick Setup</td>
</tr>
<tr class="content_section_header_tr">
<td class="content_section_header_td" colspan="2">Configure Wireless Interface</td>
</tr>
<tr>
<td class="content_desc_td" colspan="2" height="50">First step to set your wireless interface is to give it a name, called SSID. In addition, if you would like to protect transmitted data, please select the Security Level and assign a password for authentication and data transmission if it is required.</td>
</tr>
<tr>
<td class="content_header_td_less" onMouseOver="return overlib('Assign an identification string, consisting of up to 32 characters, for your WLAN.', LEFT);" onMouseOut="return nd();">SSID:</td><td class="content_input_td"><input type="text" maxlength="32" size="32" name="wl_ssid" class="content_input_fd" value="<% nvram_get_x("WLANConfig11b","wl_ssid"); %>"  onBlur="validate_string(this)"></td>
</tr>
<tr>
<td class="content_header_td_less" onMouseOver="return overlib('Selecting Low allows any users to connect to this access point and to transmit data without encryption. Selecting Middle allows only those users use the same WEP key to connect to this access point and to transmit data with WEP encryption. Selecting High allows only those users use the same WPA pre-shared key to connect to this access point and to transmit data with TKIP encryption.', LEFT);" onMouseOut="return nd();">Security Level:</td>
<td class="content_input_td">
 <select name="SecurityLevel" class="content_input_fd" onChange="return change_security(this, 'WLANConfig11b', 0)">
     <option value="0">Low(None)</option>
     <option value="1">Medium(WEP-64bits)</option>
     <option value="2">Medium(WEP-128bits)</option>
     <option value="3">High(WPA-PSK)</option>
 </select>    
</td>
</tr>
<tr>
<td class="content_header_td_less" onMouseOver="return overlib('Selecting High Security Level, this filed will be used as a password to kicks off the TKIP encryption process. A 8~63 characters password is required. Selecting Middle Security Level, this field will be used to generate four WEP keys automatically.', LEFT);" onMouseOut="return nd();">Passphrase:</td><td class="content_input_td"><script language="JavaScript" type="text/javascript" src="phrase.js"></script><script language="JavaScript" type="text/javascript" src="md5.js"></script><input type="password" maxlength="64" size="32" name="wl_wpa_psk" class="content_input_fd"  value="<% nvram_get_x("WLANConfig11b","wl_wpa_psk"); %>" onKeyUp="return is_wlphrase_q('WLANConfig11b', this)" onBlur="return validate_wlphrase_q('WLANConfig11b', this)"></td>
</tr>
<tr>
<td class="content_header_td_less">WEP Key 1 (10 or 26 hex digits):</td><td class="content_input_td"><input type="pssword" maxlength="32" size="32" name="wl_key1" class="content_input_fd" value="<% nvram_get_x("WLANConfig11b","wl_key1"); %>" onBlur="return validate_wlkey(this, 'WLANConfig11b')" onKeyPress="return is_wlkey(this, 'WLANConfig11b')" onKeyUp="return change_wlkey(this, 'WLANConfig11b')"></td>
</tr>				   
<tr>
<td class="content_header_td_less">WEP Key 2 (10 or 26 hex digits):</td><td class="content_input_td"><input type="pssword" maxlength="32" size="32" name="wl_key2" class="content_input_fd" value="<% nvram_get_x("WLANConfig11b","wl_key2"); %>" onBlur="return validate_wlkey(this, 'WLANConfig11b')" onKeyPress="return is_wlkey(this, 'WLANConfig11b')" onKeyUp="return change_wlkey(this, 'WLANConfig11b')"></td>
</tr>
<tr>
<td class="content_header_td_less">WEP Key 3 (10 or 26 hex digits):</td><td class="content_input_td"><input type="pssword" maxlength="32" size="32" name="wl_key3" class="content_input_fd" value="<% nvram_get_x("WLANConfig11b","wl_key3"); %>" onBlur="return validate_wlkey(this, 'WLANConfig11b')" onKeyPress="return is_wlkey(this, 'WLANConfig11b')" onKeyUp="return change_wlkey(this, 'WLANConfig11b')"></td>
</tr>
<tr>
<td class="content_header_td_less">WEP Key 4 (10 or 26 hex digits):</td><td class="content_input_td"><input type="pssword" maxlength="32" size="32" name="wl_key4" class="content_input_fd" value="<% nvram_get_x("WLANConfig11b","wl_key4"); %>" onBlur="return validate_wlkey(this, 'WLANConfig11b')" onKeyPress="return is_wlkey(this, 'WLANConfig11b')" onKeyUp="return change_wlkey(this, 'WLANConfig11b')"></td>
</tr>
<tr>
<td class="content_header_td_less">Key Index:</td><td class="content_input_td"><select name="wl_key" class="content_input_fd" onChange="return change_common(this, 'WLANConfig11b', 'wl_key')"><option value="1" <% nvram_match_x("WLANConfig11b","wl_key", "1","selected"); %>>1</option><option value="2" <% nvram_match_x("WLANConfig11b","wl_key", "2","selected"); %>>2</option><option value="3" <% nvram_match_x("WLANConfig11b","wl_key", "3","selected"); %>>3</option><option value="4" <% nvram_match_x("WLANConfig11b","wl_key", "4","selected"); %>>4</option></select></td>
</tr>
</table>
</td>
</tr>

<tr>
<td>		
<table width="666" border="1" cellpadding="0" cellspacing="0" bordercolor="B0B0B0">
<tr bgcolor="#CCCCCC"><td colspan="3"><font face="arial" size="2"><b>&nbsp</b></font></td></tr>
<tr bgcolor="#FFFFFF">  
   <td height="25" width="34%">  
   </td>
   <td height="25" width="33%">  
   </td>
   <td height="25" width="33%">  
   <div align="center">
   <font face="Arial"><input class=inputSubmit onMouseOut="buttonOut(this)" onMouseOver="buttonOver(this)" onClick="toPrevTag('Wireless')" type="button" value="Prev" name="action"></font>&nbsp;&nbsp;&nbsp;&nbsp;
   <font face="Arial"><input class=inputSubmit onMouseOut=buttonOut(this) onMouseOver="buttonOver(this)" type="submit" value="Finish" name="action"  onClick="saveQuick(this)"></font></div>
   </td>
</tr>
</table>
</td>
</tr>

<tr>
<td>
<table width="666" border="1" cellpadding="0" cellspacing="0" bordercolor="B0B0B0">
<tr>
    <td colspan="2" width="666" height="25" bgcolor="#FFBB00"></td> 
</tr>                   
</table>
</td>
</tr>
<tr><td colspan="2" height="240"></td></tr>
</table>
</form>

