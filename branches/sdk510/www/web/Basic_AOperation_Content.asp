<html>
<head>
<title><% model(2); %></title>
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
<input type="hidden" name="x_Connection" value="">
<input type="hidden" name="x_ADSLType" value="">
<input type="hidden" name="x_DHCPClient" value="">
<input type="hidden" name="current_page" value="Basic_AOperation_Content.asp">
<input type="hidden" name="next_page" value="SaveRestart.asp">
<input type="hidden" name="next_host" value="">
<input type="hidden" name="sid_list" value="Layer3Forwarding;IPConnection;PPPConnection;WLANConfig11a;WLANConfig11b;LANHostConfig;">
<input type="hidden" name="group_id" value="">
<input type="hidden" name="action_mode" value="">
<input type="hidden" name="modified" value="0">
<input type="hidden" name="wan_proto" value="<% nvram_get("wan_proto"); %>">
<input type="hidden" name="wan_route_x" value="<% nvram_get("wan_route_x"); %>">
<input type="hidden" name="wan_nat_x" value="<% nvram_get("wan_nat_x"); %>">
<input type="hidden" name="wl_auth_mode" value="<% nvram_get("wl_auth_mode"); %>">
<input type="hidden" name="wl_crypto" value="<% nvram_get("wl_crypto"); %>">
<input type="hidden" name="wl_wep_x" value="<% nvram_get("wl_wep_x"); %>">
<!-- Table for the conntent page -->
<table width="666" border="1" cellpadding="0" cellspacing="0" bordercolor="#E0E0E0">
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
<td class="content_header_td_less" onMouseOver="return overlib('Assign an identification string, consisting of up to 32 characters, for your WLAN.', LEFT);" onMouseOut="return nd();">SSID:</td><td class="content_input_td"><input type="text" maxlength="32" size="32" name="wl_ssid" class="content_input_fd" value="<% nvram_get("wl_ssid"); %>"  onBlur="validate_string(this)"></td>
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
<td class="content_header_td_less" onMouseOver="return overlib('Selecting High Security Level, this filed will be used as a password to kicks off the TKIP encryption process. A 8~63 characters password is required. Selecting Middle Security Level, this field will be used to generate four WEP keys automatically.', LEFT);" onMouseOut="return nd();">Passphrase:</td><td class="content_input_td"><script type="text/javascript" src="md5.js"></script><input type="password" maxlength="64" size="32" name="wl_wpa_psk" class="content_input_fd"  value="<% nvram_get("wl_wpa_psk"); %>" onKeyUp="return is_wlphrase_q('WLANConfig11b',this)" onBlur="return validate_wlphrase_q('WLANConfig11b', this)"></td>
</tr>
<tr>
<td class="content_header_td_less">WEP Key 1 (10 or 26 hex digits):</td><td class="content_input_td"><input type="text" maxlength="32" size="32" name="wl_key1" class="content_input_fd" value="<% nvram_get("wl_key1"); %>" onBlur="return validate_wlkey(this, 'WLANConfig11b')" onKeyPress="return is_wlkey(event, this, 'WLANConfig11b')" onKeyUp="return change_wlkey(this, 'WLANConfig11b')"></td>
</tr>				   
<tr>
<td class="content_header_td_less">WEP Key 2 (10 or 26 hex digits):</td><td class="content_input_td"><input type="text" maxlength="32" size="32" name="wl_key2" class="content_input_fd" value="<% nvram_get("wl_key2"); %>" onBlur="return validate_wlkey(this, 'WLANConfig11b')" onKeyPress="return is_wlkey(event, this, 'WLANConfig11b')" onKeyUp="return change_wlkey(this, 'WLANConfig11b')"></td>
</tr>
<tr>
<td class="content_header_td_less">WEP Key 3 (10 or 26 hex digits):</td><td class="content_input_td"><input type="text" maxlength="32" size="32" name="wl_key3" class="content_input_fd" value="<% nvram_get("wl_key3"); %>" onBlur="return validate_wlkey(this, 'WLANConfig11b')" onKeyPress="return is_wlkey(event, this, 'WLANConfig11b')" onKeyUp="return change_wlkey(this, 'WLANConfig11b')"></td>
</tr>
<tr>
<td class="content_header_td_less">WEP Key 4 (10 or 26 hex digits):</td><td class="content_input_td"><input type="text" maxlength="32" size="32" name="wl_key4" class="content_input_fd" value="<% nvram_get("wl_key4"); %>" onBlur="return validate_wlkey(this, 'WLANConfig11b')" onKeyPress="return is_wlkey(event, this, 'WLANConfig11b')" onKeyUp="return change_wlkey(this, 'WLANConfig11b')"></td>
</tr>
<tr>
<td class="content_header_td_less">Key Index:</td><td class="content_input_td"><select name="wl_key" class="content_input_fd" onChange="return change_common(this, 'WLANConfig11b', 'wl_key')"><option value="1" <% nvram_match("wl_key", "1","selected"); %>>1</option><option value="2" <% nvram_match("wl_key", "2","selected"); %>>2</option><option value="3" <% nvram_match("wl_key", "3","selected"); %>>3</option><option value="4" <% nvram_match("wl_key", "Key4","selected"); %>>4</option></select></td>
</tr>
</table>

<table width="666" border="1" cellpadding="0" cellspacing="0" bordercolor="#B0B0B0">
<tr bgcolor="#CCCCCC"><td colspan="3"><font face="arial" size="2">&nbsp;</font></td></tr>
<tr bgcolor="#FFFFFF">  
   <td height="25" width="67%">  
   </td>
   <td height="25" >  
   <div align="center">

   <font face="Arial"><input class=inputSubmit onMouseOut="buttonOut(this)" onMouseOver="buttonOver(this)" type="submit" value="Finish" name="action"  onClick="saveQuick(this)"></font></div>
   </td>
</tr>
<tr bgcolor="#FFBB00">
    <td colspan="2" height="25"></td> 
</tr>
</table>

</form>
</html>