<html>
<head>
<title>ZVMODELVZ Web Manager</title>
<meta http-equiv="Content-Type" content="text/html; charset=UTF-8">
<link rel="stylesheet" type="text/css" href="style.css" media="screen">
<script type="text/javascript" src="overlib.js"></script>
<script type="text/javascript" src="general.js"></script>
<script type="text/javascript" src="quick.js"></script>
</head>
<body bgcolor="#FFFFFF" onLoad="loadQuick()">
<div id="overDiv" style="position:absolute; visibility:hidden; z-index:1000;"></div>
<form method="GET" name="form" action="apply.cgi">
<input type="hidden" name="x_Mode" value="0">
<input type="hidden" name="current_page" value="Basic_Operation_Content.asp">
<input type="hidden" name="next_page" value="SaveRestart.asp">
<input type="hidden" name="next_host" value="">
<input type="hidden" name="sid_list" value="Layer3Forwarding;IPConnection;PPPConnection;WLANConfig11a;WLANConfig11b;LANHostConfig;">
<input type="hidden" name="group_id" value="">
<input type="hidden" name="action_mode" value="">
<input type="hidden" name="modified" value="0">
<input type="hidden" name="Layer3Forwarding_x_ConnectionType" value="<% nvram_get("x_ConnectionType"); %>">
<input type="hidden" name="IPConnection_ConnectionType" value="<% nvram_get("ConnectionType"); %>">
<input type="hidden" name="IPConnection_NATEnabled" value="<% nvram_get("NATEnabled"); %>">
<input type="hidden" name="WLANConfig11a_SSID" value="<% nvram_get("SSID"); %>">
<input type="hidden" name="WLANConfig11a_WEPType" value="<% nvram_get("WEPType"); %>">
<input type="hidden" name="WLANConfig11a_WEPKey1" value="<% nvram_get("WEPKey1"); %>">
<input type="hidden" name="WLANConfig11a_WEPKey2" value="<% nvram_get("WEPKey2"); %>">
<input type="hidden" name="WLANConfig11a_WEPKey3" value="<% nvram_get("WEPKey3"); %>">
<input type="hidden" name="WLANConfig11a_WEPKey4" value="<% nvram_get("WEPKey4"); %>">
<input type="hidden" name="WLANConfig11a_WEPDefaultKey" value="<% nvram_get("WEPDefaultKey"); %>">
<!-- Table for the conntent page -->
<table width="666" border="0" cellpadding="0" cellspacing="0">
<tr>
<td>
<table width="666" border="1" cellpadding="0" cellspacing="0" bordercolor="#E0E0E0">
<tr id="Broadband" class="content_header_tr">
<td class="content_header_td_title" colspan="2">Quick Setup</td>
</tr>
<tr class="content_section_header_tr">
<td class="content_section_header_td" colspan="2">Select the connection type to Internet</td>
</tr>
<tr>
<td class="content_desc_td" colspan="2" height="50">
ZVMODELVZ supports two kinds of connection to Internet through its WAN port. Please select connection type you need. In addition, before getting on Internet, please make sure you have connected WL600's WAN port to your DSL or Cable Modem.</td>
</tr>
<tr>
<td class="content_header_td_less" onMouseOut="return nd();">Connection Type:</td><td class="content_input_td"><select name="x_Connection"  onChange="changeConnection()"><option value="ADSL">ADSL</option><option value="Cable">Cable</option></select></td>
</tr>
<tr>
<td class="content_input_td" colspan="2">
<table>
<tr>
<td width="494" height="100"></td><td>
<div align="center">
<font face="Arial"><input onMouseOut="buttonOut(this)" onMouseOver="buttonOver(this)" onClick="toPrevTag('Broadband')" type="button" value=" Prev " name="action"></font>&nbsp;&nbsp;
<font face="Arial"> <input onMouseOut="buttonOut(this)" onMouseOver="buttonOver(this)" onClick="toTag('Broadband')" type="button" value="Next" name="action"></font>
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
<td  class="content_section_header_td" colspan="2">Set your account to ISP</td>
</tr>
<tr>
<td class="content_desc_td" colspan="2" height="50">If you apply an ADSL account with dynamic IP, you must get user account and password from your ISP. Please fill this data into the following fields carefully. Or, if you apply an ADSL account with static IP, just ignore user name and password information.</td>
</tr>
<tr>
<td class="content_header_td_less">Connect with static IP?</td><td class="content_input_td"><input type="radio" value="1" name="x_ADSLType" onClick="changeADSLType()">Yes</input><input type="radio" value="0" name="x_ADSLType" onClick="changeADSLType()">No</input></td>
</tr>
<tr>
<td class="content_header_td_less">User Name:</td><td class="content_input_td"><input type="text" maxlength="64" size="32" name="PPPConnection_UserName" value="<% nvram_get("UserName"); %>"></td>
</tr>
<tr>
<td class="content_header_td_less">Password:</td><td class="content_input_td"><input type="password" maxlength="64" size="32" name="PPPConnection_Password" value="<% nvram_get("Password"); %>"></td>
</tr>
<tr>
<td class="content_input_td" colspan="2">
<table>
<tr>
<td width="494" height="100"></td>
<td>
<div align="center">
<font face="Arial"><input onMouseOut="buttonOut(this)" onMouseOver="buttonOver(this)" onClick="toPrevTag('PPPoE')" type="button" value=" Prev " name="action"></font>&nbsp;&nbsp;
<font face="Arial"><input onMouseOut="buttonOut(this)" onMouseOver="buttonOver(this)" onClick="toTag('PPPoE')" type="button" value="Next" name="action"></font>
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
<td  class="content_section_header_td" colspan="2">Set information required by ISP</td>
</tr>
<tr>
<td class="content_desc_td" colspan="2" height="50">Your ISP may require the following information to identify your account. If not, just press Next to ignore it</td>
</tr>
<tr>
<td class="content_header_td_less" onMouseOver="return overlib('This field allows you to provide a host name for ZVMODELVZ. It is usually requested by your ISP.');" onMouseOut="return nd();">Host Name:</td><td class="content_input_td"><input type="text" maxlength="32" size="32" name="PPPConnection_x_HostNameForISP" value=""></td>
</tr>
<tr>
<td class="content_header_td_less" onMouseOver="return overlib('This field allows you to provide a unique MAC address for ZVMODELVZ to connect to Internet. It is usually requested by your ISP.');" onMouseOut="return nd();">MAC Address:</td><td class="content_input_td"><input type="text" maxlength="12" size="12" name="PPPConnection_x_MacAddressForISP" value="" onBlur="return validate_hwaddr(this)" onKeyPress="return is_hwaddr(event, this)"></td>
</tr>
<tr>
<td class="content_input_td_less" colspan="2">
<table>
<tr>
<td width="494" height="100"></td>
<td>
<div align="center">
<font face="Arial"><input onMouseOut="buttonOut(this)" onMouseOver="buttonOver(this)" onClick="toPrevTag('MacHost')" type="button" value=" Prev " name="action"></font>&nbsp;&nbsp;
<font face="Arial"><input onMouseOut="buttonOut(this)" onMouseOver="buttonOver(this)" onClick="toTag('MacHost')" type="button" value="Next" name="action"></font>
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
<td class="content_header_td_less">Get IP automatically?</td><td class="content_input_td"><input type="radio" value="1" name="x_DHCPClient" onClick="changeDHCPClient()">Yes</input><input type="radio" value="0" name="x_DHCPClient" onClick="changeDHCPClient()">No</input></td>
</tr>
<tr>
<td class="content_header_td_less" onMouseOver="return overlib('IP address of WAN Interface. If you leave it blank, ZVMODELVZ will get IP address from DHCP Server automatically');" onMouseOut="return nd();">IP Address:</td><td class="content_input_td"><input type="text" maxlength="15" size="15" name="IPConnection_ExternalIPAddress" value="<% nvram_get("ExternalIPAddress"); %>" onBlur="return validate_ipaddr(this, 'ExternalIPAddress')" onKeyPress="return is_ipaddr(event, this)"></td>
</tr>
<tr>
<td class="content_header_td_less">Subnet Mask:</td><td class="content_input_td"><input type="text" maxlength="15" size="15" name="IPConnection_x_ExternalSubnetMask" value="<% nvram_get("x_ExternalSubnetMask"); %>" onBlur="return validate_ipaddr(this)" onKeyPress="return is_ipaddr(event, this)"></td>
</tr>
<tr>
<td class="content_header_td_less">Gateway:</td><td class="content_input_td"><input type="text" maxlength="15" size="15" name="IPConnection_x_ExternalGateway" value="<% nvram_get("x_ExternalGateway"); %>" onBlur="return validate_ipaddr(this)" onKeyPress="return is_ipaddr(event, this)"></td>
</tr>
<tr>
<td class="content_header_td_less">Get DNS Server automatically?</td><td class="content_input_td"><input type="radio" value="1" name="IPConnection_x_DNSServerEnable" onClick="changeDNSServer()" <% nvram_match_x("IPConnection","x_DNSServerEnable", "1", "checked"); %>>Yes</input><input type="radio" value="0" name="IPConnection_x_DNSServerEnable" onClick="changeDNSServer()" <% nvram_match_x("IPConnection","x_DNSServerEnable", "0", "checked"); %>>No</input></td>
</tr>
<tr>
<td class="content_header_td_less">DNS Server 1:</td><td class="content_input_td"><input type="text" maxlength="15" size="15" name="IPConnection_x_DNSServer1" value="<% nvram_get("x_DNSServer1"); %>" onBlur="return validate_ipaddr(this)" onKeyPress="return is_ipaddr(event, this)"></td>
</tr>
<tr>
<td class="content_header_td_less">DNS Server 2:</td><td class="content_input_td"><input type="text" maxlength="15" size="15" name="IPConnection_x_DNSServer2" value="<% nvram_get("x_DNSServer2"); %>" onBlur="return validate_ipaddr(this)" onKeyPress="return is_ipaddr(event, this)"></td>
</tr>
<tr>
<td class="content_input_td" colspan="2">
<table>
<tr>
<td width="494" height="100"></td><td>
<div align="center">
<font face="Arial"><input onMouseOut="buttonOut(this)" onMouseOver="buttonOver(this)" onClick="toPrevTag('WANSetting')" type="button" value=" Prev " name="action"></font>&nbsp;&nbsp;
<font face="Arial"><input onMouseOut="buttonOut(this)" onMouseOver="buttonOver(this)" onClick="toTag('WANSetting')" type="button" value="Next" name="action"></font>
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
<td class="content_desc_td" colspan="2" height="50">First step for setting your wireless interface is to give it a name, called SSID. In addition, if you like to protect transmitted data, please select WEP protection and assign WEP keys for data transmission. Your wireless setting will be applied into both 802.11a and 802.11b interfaces.</td>
</tr>
<tr>
<td class="content_header_td_less" onMouseOver="return overlib('Assign an identification string, consisting of up to 32 characters, for your WLAN');" onMouseOut="return nd();">SSID:</td><td class="content_input_td"><input type="text" maxlength="32" size="32" name="WLANConfig11b_SSID" value="<% nvram_get("SSID"); %>"></td>
</tr>
<tr>
<td class="content_header_td_less" onMouseOver="return overlib('Enable WEP to encrypt data which can avoid disclosure to eavesdroppers');" onMouseOut="return nd();">WEP:</td><td class="content_input_td"><select name="WLANConfig11b_WEPType" onChange="return change_wlweptype(this, 'WLANConfig11b')"><option value="None" <% nvram_match_x("WLANConfig11b","WEPType", "None","selected"); %>>None</option><option value="64bits" <% nvram_match_x("WLANConfig11b","WEPType", "64bits","selected"); %>>64bits</option><option value="128bits" <% nvram_match_x("WLANConfig11b","WEPType", "128bits","selected"); %>>128bits</option></select></td>
</tr>
<tr>
<td class="content_header_td_less" onMouseOver="return overlib('Give a magic word to generate the WEP keys automatically or leave this field blank and type in the keys manually.');" onMouseOut="return nd();">Phrase:</td><td class="content_input_td"><script type="text/javascript" src="md5.js"></script><input type="password" maxlength="64" size="32" name="WLANConfig11b_x_Phrase" value="<% nvram_get("x_Phrase"); %>" onKeyUp="return is_wlphrase('WLANConfig11b',this)"></td>
</tr>
<tr>
<td class="content_header_td_less">WEP Key 1 (10 or 26 hex digits):</td><td class="content_input_td"><input type="text" maxlength="32" size="32" name="WLANConfig11b_WEPKey1" value="<% nvram_get("WEPKey1"); %>" onBlur="return validate_wlkey(this, 'WLANConfig11b')" onKeyPress="return is_wlkey(event, this, 'WLANConfig11b')"></td>
</tr>
<tr>
<td class="content_header_td_less">WEP Key 2 (10 or 26 hex digits):</td><td class="content_input_td"><input type="text" maxlength="32" size="32" name="WLANConfig11b_WEPKey2" value="<% nvram_get("WEPKey2"); %>" onBlur="return validate_wlkey(this, 'WLANConfig11b')" onKeyPress="return is_wlkey(event, this, 'WLANConfig11b')"></td>
</tr>
<tr>
<td class="content_header_td_less">WEP Key 3 (10 or 26 hex digits):</td><td class="content_input_td"><input type="text" maxlength="32" size="32" name="WLANConfig11b_WEPKey3" value="<% nvram_get("WEPKey3"); %>" onBlur="return validate_wlkey(this, 'WLANConfig11b')" onKeyPress="return is_wlkey(event, this, 'WLANConfig11b')"></td>
</tr>
<tr>
<td class="content_header_td_less">WEP Key 4 (10 or 26 hex digits):</td><td class="content_input_td"><input type="text" maxlength="32" size="32" name="WLANConfig11b_WEPKey4" value="<% nvram_get("WEPKey4"); %>" onBlur="return validate_wlkey(this, 'WLANConfig11b')" onKeyPress="return is_wlkey(event, this, 'WLANConfig11b')"></td>
</tr>
<tr>
<td class="content_header_td_less">Default Key:</td><td class="content_input_td"><select name="WLANConfig11b_WEPDefaultKey" onChange="return change_common(this, 'WLANConfig11b', 'WEPDefaultKey')"><option value="Key1" <% nvram_match_x("WLANConfig11b","WEPDefaultKey", "Key1","selected"); %>>Key1</option><option value="Key2" <% nvram_match_x("WLANConfig11b","WEPDefaultKey", "Key2","selected"); %>>Key2</option><option value="Key3" <% nvram_match_x("WLANConfig11b","WEPDefaultKey", "Key3","selected"); %>>Key3</option><option value="Key4" <% nvram_match_x("WLANConfig11b","WEPDefaultKey", "Key4","selected"); %>>Key4</option><option value="Key Rotation" <% nvram_match_x("WLANConfig11b","WEPDefaultKey", "Key Rotation","selected"); %>>Key Rotation</option></select></td>
</tr>
</table>
</td>
</tr>

<tr>
<td>		
<table width="666" border="1" cellpadding="0" cellspacing="0" bordercolor="#B0B0B0">
<tr bgcolor="#CCCCCC"><td colspan="2"><font face="arial" size="2"><b>&nbsp;</b></font></td></tr>
<tr bgcolor="#FFFFFF">  
   <td height="25" width="65%">  
   </td>
   <td height="25">  
   <div align="center">
   <font face="Arial"><input onMouseOut="buttonOut(this)" onMouseOver="buttonOver(this)" onClick="toPrevTag('Wireless')" type="button" value="  Prev  " name="action"></font>&nbsp;&nbsp;&nbsp;&nbsp;
   <font face="Arial"><input class=inputSubmit onMouseOut="buttonOut(this)" onMouseOver="buttonOver(this)" type="submit" value=" Finish " name="action"  onClick="saveQuick(this)"></font></div>
   </td>
</tr>
</table>
</td>
</tr>

<tr>
<td>
<table width="666" border="1" cellpadding="0" cellspacing="0" bordercolor="#B0B0B0">
<tr>
    <td colspan="2" width="666" height="25" bgcolor="#FFBB00"></td> 
</tr>                   
</table>
</td>
</tr>
<tr><td colspan="2" height="240"></td></tr>
</table>
</form>
</body>
</html>