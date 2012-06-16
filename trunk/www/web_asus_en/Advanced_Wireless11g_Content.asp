<html>
<head>
<title>ZVMODELVZ Web Manager</title>
<meta http-equiv="Content-Type" content="text/html; charset=UTF-8">
<link rel="stylesheet" type="text/css" href="style.css" media="screen">
<script type="text/javascript" src="overlib.js"></script>
<script type="text/javascript" src="general.js"></script>
</head>  
<body onLoad="load_body()" onunLoad="return unload_body();">
<div id="overDiv" style="position:absolute; visibility:hidden; z-index:1000;"></div>    
<form method="GET" name="form" action="apply.cgi">
<input type="hidden" name="current_page" value="Advanced_Wireless_Content.asp">
<input type="hidden" name="next_page" value="Advanced_WMode_Content.asp">
<input type="hidden" name="next_host" value="">
<input type="hidden" name="sid_list" value="WLANConfig11a;WLANConfig11b;">
<input type="hidden" name="group_id" value="">
<input type="hidden" name="modified" value="0">
<input type="hidden" name="action_mode" value="">
<input type="hidden" name="first_time" value="">
<input type="hidden" name="action_script" value="">
<input type="hidden" name="wl_wpa_mode" value="<% nvram_get("wl_wpa_mode"); %>">
<input type="hidden" name="wl_gmode_protection_x" value="<% nvram_get("wl_gmode_protection_x"); %>">
<!-- Table for the conntent page -->	    
<table width="666" border="1" cellpadding="0" cellspacing="0" bordercolor="#E0E0E0">
<tr class="content_header_tr">
<td class="content_header_td_title" colspan="2">Wireless - Interface</td>
</tr>
<tr>
<td class="content_header_td" onMouseOver="return overlib('Assign an identification string, consisting of up to 32 characters, for your WLAN.', LEFT);" onMouseOut="return nd();">SSID:
           </td><td class="content_input_td"><input type="text" maxlength="32" class="content_input_fd" size="32" name="wl_ssid" value="<% nvram_get("wl_ssid"); %>" onKeyPress="return is_string(event, this)" onBlur="validate_string(this)"></td>
</tr>
<tr>
<td class="content_header_td" onMouseOver="return overlib('Select the operating radio channel', LEFT);" onMouseOut="return nd();">Channel:
           </td><td class="content_input_td"><select name="wl_channel" class="content_input_fd" onChange="return change_common(this, 'WLANConfig11b', 'wl_channel')">   
<% select_channel(); %>
                </select><input type="hidden" maxlength="15" size="15" name="x_RegulatoryDomain" value="<% nvram_get("x_RegulatoryDomain"); %>" readonly></td>
</tr>
<tr>
<td class="content_header_td" onMouseOver="return overlib('This field indicates the mode of 802.11g interface. Selecting Auto allows 802.11g and 802.11b to connect to your device. Selecting 54g Only maximizes performace, but 802.11b client is not allowed to connect to your device. If 54g Protection is checked, G-Mode protection of 11g traffic is enabled automatically in the presence of 11b traffic.', LEFT);" onMouseOut="return nd();">Wireless Mode:
	</td><td class="content_input_td">
	<select name="wl_gmode" class="content_input_fd" onChange="return change_common(this, 'WLANConfig11b', 'wl_gmode')">
	<option class="content_input_fd" value="1" <% nvram_match_x("WLANConfig11b","wl_gmode", "1","selected"); %>>Auto</option>
	<option class="content_input_fd" value="0" <% nvram_match_x("WLANConfig11b","wl_gmode", "0","selected"); %>>802.11b Only</option>
	<option class="content_input_fd" value="2" <% nvram_match_x("WLANConfig11b","wl_gmode", "2","selected"); %>>802.11g Only</option>
	<option class="content_input_fd" value="4" <% nvram_match_x("WLANConfig11b","wl_gmode", "4","selected"); %>>Performance</option>
	<option class="content_input_fd" value="5" <% nvram_match_x("WLANConfig11b","wl_gmode", "5","selected"); %>>54G LRS</option>
	</select>
	<input type="checkbox" style="margin-left:30" name="wl_gmode_check" value="" onClick="return change_common_radio(this, 'WLANConfig11b', 'wl_gmode', '1')">54g Protection</input></td>
</tr>
<tr>
<td class="content_header_td" onMouseOver="return overlib('This field indicates the method used to authenticate wireless users. Selecting different Authentication Method, different encryption scheme will be applied.', LEFT);" onMouseOut="return nd();">Authentication Method:</td>
	<td class="content_input_td">
		<select name="wl_auth_mode" class="content_input_fd" onChange="return change_common(this, 'WLANConfig11b', 'wl_auth_mode')">
			<option class="content_input_fd" value="open" <% nvram_match_x("WLANConfig11b","wl_auth_mode", "open","selected"); %>>Open System</option>
			<option class="content_input_fd" value="shared" <% nvram_match_x("WLANConfig11b","wl_auth_mode", "shared","selected"); %>>Shared Key</option>
			<option class="content_input_fd" value="psk" <% nvram_double_match_x("WLANConfig11b", "wl_auth_mode", "psk", "WLANConfig11b", "wl_wpa_mode", "1", "selected"); %>>WPA-Personal</option>
			<option class="content_input_fd" value="psk" <% nvram_double_match_x("WLANConfig11b", "wl_auth_mode", "psk", "WLANConfig11b", "wl_wpa_mode", "2", "selected"); %>>WPA2-Personal</option>
			<option class="content_input_fd" value="psk" <% nvram_double_match_x("WLANConfig11b", "wl_auth_mode", "psk", "WLANConfig11b", "wl_wpa_mode", "0", "selected"); %>>WPA-Auto-Personal</option>
			<option class="content_input_fd" value="wpa" <% nvram_match_x("WLANConfig11b","wl_auth_mode", "wpa","selected"); %>>WPA-Enterprise</option>
			<option class="content_input_fd" value="wpa2" <% nvram_match_x("WLANConfig11b","wl_auth_mode", "wpa2","selected"); %>>WPA2-Enterprise</option>
			<option class="content_input_fd" value="radius" <% nvram_match_x("WLANConfig11b","wl_auth_mode", "radius","selected"); %>>Radius with 802.1x</option>
		</select>
	</td>
</tr>
<tr>
<td class="content_header_td" onMouseOver="return overlib('This field indicates the encryption method used to encrypt data while WPA encryption is enabled.', LEFT);" onMouseOut="return nd();">WPA Encryption:
           </td><td class="content_input_td"><select name="wl_crypto" class="content_input_fd" onChange="return change_common(this, 'WLANConfig11b', 'wl_crypto')"><option class="content_input_fd" value="tkip" <% nvram_match_x("WLANConfig11b","wl_crypto", "tkip","selected"); %>>TKIP</option><option class="content_input_fd" value="aes" <% nvram_match_x("WLANConfig11b","wl_crypto", "aes","selected"); %>>AES</option><option class="content_input_fd" value="tkip+aes" <% nvram_match_x("WLANConfig11b","wl_crypto", "tkip+aes","selected"); %>>TKIP+AES</option></select></td>
</tr>
<tr>
<td class="content_header_td" onMouseOver="return overlib('Pre-shared key should be more than 7 characters and less than 64 characters!!! If you leave this field blank, system will assign 00000000 as your passphrase.', LEFT);" onMouseOut="return nd();">WPA Pre-Shared Key:
           </td><td class="content_input_td"><script type="text/javascript" src="md5.js"></script><input type="password" maxlength="64" class="content_input_fd" size="32" name="wl_wpa_psk" value="<% nvram_get("wl_wpa_psk"); %>" onKeyUp="return is_wlphrase('WLANConfig11b', 'wl_wpa_psk', this)" onBlur="return validate_wlphrase('WLANConfig11b', 'wl_wpa_psk', this)"></td>
</tr>
<tr>
<td class="content_header_td" onMouseOver="return overlib('This field indicates the encryption method used to encrypt data while WEP encryption is enabled.', LEFT);" onMouseOut="return nd();">WEP Encryption:
           </td><td class="content_input_td"><select name="wl_wep_x" class="content_input_fd" onChange="return change_common(this, 'WLANConfig11b', 'wl_wep_x')"><option class="content_input_fd" value="0" <% nvram_match_x("WLANConfig11b","wl_wep_x", "0","selected"); %>>None</option><option class="content_input_fd" value="1" <% nvram_match_x("WLANConfig11b","wl_wep_x", "1","selected"); %>>WEP-64bits</option><option class="content_input_fd" value="2" <% nvram_match_x("WLANConfig11b","wl_wep_x", "2","selected"); %>>WEP-128bits</option></select></td>
</tr>
<tr>
<td class="content_header_td" onMouseOver="return overlib('Selecting WEP-64bits or WEP-128bits encryption method, this field will be used to generate four WEP keys automatically.', LEFT);" onMouseOut="return nd();">Passphrase:
           </td><td class="content_input_td"><script type="text/javascript" src="md5.js"></script><input type="password" maxlength="64" class="content_input_fd" size="32" name="wl_phrase_x" value="<% nvram_get("wl_phrase_x"); %>" onKeyUp="return is_wlphrase('WLANConfig11b', 'wl_phrase_x', this)" onBlur="return validate_wlphrase('WLANConfig11b', 'wl_phrase_x', this)"></td>
</tr>
<tr>
<td class="content_header_td">WEP Key 1 (10 or 26 hex digits):
           </td><td class="content_input_td"><input type="text" maxlength="32" class="content_input_fd" size="32" name="wl_key1" value="<% nvram_get("wl_key1"); %>" onBlur="return validate_wlkey(this, 'WLANConfig11b')" onKeyPress="return is_wlkey(event, this, 'WLANConfig11b')" onKeyUp="return change_wlkey(this, 'WLANConfig11b')"></td>
</tr>
<tr>
<td class="content_header_td">WEP Key 2 (10 or 26 hex digits):
           </td><td class="content_input_td"><input type="text" maxlength="32" class="content_input_fd" size="32" name="wl_key2" value="<% nvram_get("wl_key2"); %>" onBlur="return validate_wlkey(this, 'WLANConfig11b')" onKeyPress="return is_wlkey(event, this, 'WLANConfig11b')" onKeyUp="return change_wlkey(this, 'WLANConfig11b')"></td>
</tr>
<tr>
<td class="content_header_td">WEP Key 3 (10 or 26 hex digits):
           </td><td class="content_input_td"><input type="text" maxlength="32" class="content_input_fd" size="32" name="wl_key3" value="<% nvram_get("wl_key3"); %>" onBlur="return validate_wlkey(this, 'WLANConfig11b')" onKeyPress="return is_wlkey(event, this, 'WLANConfig11b')" onKeyUp="return change_wlkey(this, 'WLANConfig11b')"></td>
</tr>
<tr>
<td class="content_header_td">WEP Key 4 (10 or 26 hex digits):
           </td><td class="content_input_td"><input type="text" maxlength="32" class="content_input_fd" size="32" name="wl_key4" value="<% nvram_get("wl_key4"); %>" onBlur="return validate_wlkey(this, 'WLANConfig11b')" onKeyPress="return is_wlkey(event, this, 'WLANConfig11b')" onKeyUp="return change_wlkey(this, 'WLANConfig11b')"></td>
</tr>
<tr>
<td class="content_header_td" onMouseOver="return overlib('This field indicates the default transmit WEP key.', LEFT);" onMouseOut="return nd();">Key Index:
           </td><td class="content_input_td"><select name="wl_key" class="content_input_fd" onChange="return change_common(this, 'WLANConfig11b', 'wl_key')"><option class="content_input_fd" value="1" <% nvram_match_x("WLANConfig11b","wl_key", "1","selected"); %>>Key1</option><option class="content_input_fd" value="2" <% nvram_match_x("WLANConfig11b","wl_key", "2","selected"); %>>Key2</option><option class="content_input_fd" value="3" <% nvram_match_x("WLANConfig11b","wl_key", "3","selected"); %>>Key3</option><option class="content_input_fd" value="4" <% nvram_match_x("WLANConfig11b","wl_key", "4","selected"); %>>Key4</option></select></td>
</tr>
<tr>
<td class="content_header_td" onMouseOver="return overlib('This field specifies the time interval, in seconds, that WPA group key is changed. 0 means no periodic key-change is required.', LEFT);" onMouseOut="return nd();">Network Key Rotation Interval:
           </td><td class="content_input_td"><input type="text" maxlength="5" size="5" name="wl_wpa_gtk_rekey" class="content_input_fd" value="<% nvram_get("wl_wpa_gtk_rekey"); %>" onBlur="validate_range(this, 0, 86400)" onKeyPress="return is_number(event, this)"></td>
</tr>
</table>

<% include("footer_buttons.inc"); %>

</form>
</body>
</html>
