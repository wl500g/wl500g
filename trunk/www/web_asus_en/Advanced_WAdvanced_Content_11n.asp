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
<input type="hidden" name="current_page" value="Advanced_WAdvanced_Content.asp">
<input type="hidden" name="next_page" value="SaveRestart.asp">
<input type="hidden" name="next_host" value="">
<input type="hidden" name="sid_list" value="WLANAuthentication11a;WLANConfig11b;">
<input type="hidden" name="group_id" value="">
<input type="hidden" name="modified" value="0">
<input type="hidden" name="action_mode" value="">
<input type="hidden" name="first_time" value="">
<input type="hidden" name="action_script" value="">
<input type="hidden" name="wl_gmode" value="<% nvram_get("wl_gmode"); %>">
<input type="hidden" name="wl_gmode_protection_x" value="<% nvram_get("wl_gmode_protection_x"); %>">
<!-- Table for the conntent page -->	    
<table width="666" border="1" cellpadding="0" cellspacing="0" bordercolor="#E0E0E0">
<tr class="content_header_tr">
<td class="content_header_td_title" colspan="2">Wireless - Advanced</td>
</tr>
<tr>
<td class="content_desc_td" colspan="2">This section allows you to set up additional parameters for wireless. But default values are recommended.
         </td>
</tr>
<tr>
<td class="content_header_td" onMouseOver="return overlib('Enabling AfterBurner will improve througput with the peer with Afterburner Capability. AfterBurner mode requires: Authentication Method set to Open System or Shared Key, AP Mode set to AP only and Allow Anonymous set to No.', LEFT);" onMouseOut="return nd();">Enable AfterBurner?
           </td><td class="content_input_td"><select name="wl_afterburner" class="content_input_fd" onChange="return change_common(this, 'WLANConfig11b', 'wl_afterburner')"><option class="content_input_fd" value="off" <% nvram_match_x("WLANConfig11b","wl_afterburner", "off","selected"); %>>Disabled</option><option class="content_input_fd" value="auto" <% nvram_match_x("WLANConfig11b","wl_afterburner", "auto","selected"); %>>Enabled</option></select></td>
</tr>
<tr>
<td class="content_header_td" onMouseOver="return overlib('Selecting YES will hide your AP from those clients that query AP by broadcast packets. So, only clients that are aware of the SSID of AP can connect to it.', LEFT);" onMouseOut="return nd();">Hide SSID:
           </td><td class="content_input_td"><input type="radio" value="1" name="wl_closed" class="content_input_fd" onClick="return change_common_radio(this, 'WLANConfig11b', 'wl_closed', '1')" <% nvram_match_x("WLANConfig11b","wl_closed", "1", "checked"); %>>Yes</input><input type="radio" value="0" name="wl_closed" class="content_input_fd" onClick="return change_common_radio(this, 'WLANConfig11b', 'wl_closed', '0')" <% nvram_match_x("WLANConfig11b","wl_closed", "0", "checked"); %>>No</input></td>
</tr>
<tr>
<td class="content_header_td" onMouseOver="return overlib('Selecting Yes to prevent wireless clients from communicating with each other.', LEFT);" onMouseOut="return nd();">Set AP Isolated?
           </td><td class="content_input_td"><input type="radio" value="1" name="wl_ap_isolate" class="content_input_fd" onClick="return change_common_radio(this, 'WLANConfig11b', 'wl_ap_isolate', '1')" <% nvram_match_x("WLANConfig11b","wl_ap_isolate", "1", "checked"); %>>Yes</input><input type="radio" value="0" name="wl_ap_isolate" class="content_input_fd" onClick="return change_common_radio(this, 'WLANConfig11b', 'wl_ap_isolate', '0')" <% nvram_match_x("WLANConfig11b","wl_ap_isolate", "0", "checked"); %>>No</input></td>
</tr>
<tr>
<td class="content_header_td" onMouseOver="return overlib('Select the transmission rate. You are recommended to select Auto to maximize performace.', LEFT);" onMouseOut="return nd();">802.11b/g Data Rate (Mbps):
           </td><td class="content_input_td"><select name="wl_rate" class="content_input_fd" onChange="return change_common(this, 'WLANConfig11b', 'wl_rate')"><option class="content_input_fd" value="0" <% nvram_match_x("WLANConfig11b","wl_rate", "0","selected"); %>>Auto</option><option class="content_input_fd" value="1000000" <% nvram_match_x("WLANConfig11b","wl_rate", "1000000","selected"); %>>1</option><option class="content_input_fd" value="2000000" <% nvram_match_x("WLANConfig11b","wl_rate", "2000000","selected"); %>>2</option><option class="content_input_fd" value="5500000" <% nvram_match_x("WLANConfig11b","wl_rate", "5500000","selected"); %>>5.5</option><option class="content_input_fd" value="6000000" <% nvram_match_x("WLANConfig11b","wl_rate", "6000000","selected"); %>>6</option><option class="content_input_fd" value="9000000" <% nvram_match_x("WLANConfig11b","wl_rate", "9000000","selected"); %>>9</option><option class="content_input_fd" value="11000000" <% nvram_match_x("WLANConfig11b","wl_rate", "11000000","selected"); %>>11</option><option class="content_input_fd" value="12000000" <% nvram_match_x("WLANConfig11b","wl_rate", "12000000","selected"); %>>12</option><option class="content_input_fd" value="18000000" <% nvram_match_x("WLANConfig11b","wl_rate", "18000000","selected"); %>>18</option><option class="content_input_fd" value="24000000" <% nvram_match_x("WLANConfig11b","wl_rate", "24000000","selected"); %>>24</option><option class="content_input_fd" value="36000000" <% nvram_match_x("WLANConfig11b","wl_rate", "36000000","selected"); %>>36</option><option class="content_input_fd" value="48000000" <% nvram_match_x("WLANConfig11b","wl_rate", "48000000","selected"); %>>48</option><option class="content_input_fd" value="54000000" <% nvram_match_x("WLANConfig11b","wl_rate", "54000000","selected"); %>>54</option></select></td>
</tr>
<tr>
<td class="content_header_td" onMouseOver="return overlib('Select the multicast transmission rate. You are recommended to select Auto for maximum compatibility.', LEFT);" onMouseOut="return nd();">Multicast Rate (Mbps):
           </td><td class="content_input_td"><select name="wl_mrate" class="content_input_fd" onChange="return change_common(this, 'WLANConfig11b', 'wl_mrate')"><option class="content_input_fd" value="0" <% nvram_match_x("WLANConfig11b","wl_mrate", "0","selected"); %>>Auto</option><option class="content_input_fd" value="1000000" <% nvram_match_x("WLANConfig11b","wl_mrate", "1000000","selected"); %>>1</option><option class="content_input_fd" value="2000000" <% nvram_match_x("WLANConfig11b","wl_mrate", "2000000","selected"); %>>2</option><option class="content_input_fd" value="5500000" <% nvram_match_x("WLANConfig11b","wl_mrate", "5500000","selected"); %>>5.5</option><option class="content_input_fd" value="6000000" <% nvram_match_x("WLANConfig11b","wl_mrate", "6000000","selected"); %>>6</option><option class="content_input_fd" value="9000000" <% nvram_match_x("WLANConfig11b","wl_mrate", "9000000","selected"); %>>9</option><option class="content_input_fd" value="11000000" <% nvram_match_x("WLANConfig11b","wl_mrate", "11000000","selected"); %>>11</option><option class="content_input_fd" value="12000000" <% nvram_match_x("WLANConfig11b","wl_mrate", "12000000","selected"); %>>12</option><option class="content_input_fd" value="18000000" <% nvram_match_x("WLANConfig11b","wl_mrate", "18000000","selected"); %>>18</option><option class="content_input_fd" value="24000000" <% nvram_match_x("WLANConfig11b","wl_mrate", "24000000","selected"); %>>24</option><option class="content_input_fd" value="36000000" <% nvram_match_x("WLANConfig11b","wl_mrate", "36000000","selected"); %>>36</option><option class="content_input_fd" value="48000000" <% nvram_match_x("WLANConfig11b","wl_mrate", "48000000","selected"); %>>48</option><option class="content_input_fd" value="54000000" <% nvram_match_x("WLANConfig11b","wl_mrate", "54000000","selected"); %>>54</option></select></td>
</tr>
<tr>
<td class="content_header_td" onMouseOver="return overlib('Select the transmission rate. You are recommended to select Auto to maximize performace.', LEFT);" onMouseOut="return nd();">802.11n MCS Index:
           </td><td class="content_input_td"><select name="wl_nmcsidx" class="content_input_fd" onChange="return change_common(this, 'WLANConfig11b', 'wl_nmcsidx')">
<option class="content_input_fd" value="-1" <% nvram_match_x("WLANConfig11b","wl_rate", "-1","selected"); %>>Auto</option>
<option class="content_input_fd" value="0" <% nvram_match_x("WLANConfig11b","wl_nmcsidx", "0","selected"); %>>0</option>
<option class="content_input_fd" value="1" <% nvram_match_x("WLANConfig11b","wl_nmcsidx", "1","selected"); %>>1</option>
<option class="content_input_fd" value="2" <% nvram_match_x("WLANConfig11b","wl_nmcsidx", "2","selected"); %>>2</option>
<option class="content_input_fd" value="3" <% nvram_match_x("WLANConfig11b","wl_nmcsidx", "3","selected"); %>>3</option>
<option class="content_input_fd" value="4" <% nvram_match_x("WLANConfig11b","wl_nmcsidx", "4","selected"); %>>4</option>
<option class="content_input_fd" value="5" <% nvram_match_x("WLANConfig11b","wl_nmcsidx", "5","selected"); %>>5</option>
<option class="content_input_fd" value="6" <% nvram_match_x("WLANConfig11b","wl_nmcsidx", "6","selected"); %>>6</option>
<option class="content_input_fd" value="7" <% nvram_match_x("WLANConfig11b","wl_nmcsidx", "7","selected"); %>>7</option>
<option class="content_input_fd" value="8" <% nvram_match_x("WLANConfig11b","wl_nmcsidx", "8","selected"); %>>8</option>
<option class="content_input_fd" value="9" <% nvram_match_x("WLANConfig11b","wl_nmcsidx", "9","selected"); %>>9</option>
<option class="content_input_fd" value="10" <% nvram_match_x("WLANConfig11b","wl_nmcsidx", "10","selected"); %>>10</option>
<option class="content_input_fd" value="11" <% nvram_match_x("WLANConfig11b","wl_nmcsidx", "11","selected"); %>>11</option>
<option class="content_input_fd" value="12" <% nvram_match_x("WLANConfig11b","wl_nmcsidx", "12","selected"); %>>12</option>
<option class="content_input_fd" value="13" <% nvram_match_x("WLANConfig11b","wl_nmcsidx", "13","selected"); %>>13</option>
<option class="content_input_fd" value="14" <% nvram_match_x("WLANConfig11b","wl_nmcsidx", "14","selected"); %>>14</option>
<option class="content_input_fd" value="15" <% nvram_match_x("WLANConfig11b","wl_nmcsidx", "15","selected"); %>>15</option>
<option class="content_input_fd" value="32" <% nvram_match_x("WLANConfig11b","wl_nmcsidx", "32","selected"); %>>32</option>
</select></td>
</tr>
<tr>
<td class="content_header_td" onMouseOver="return overlib('This field indicates the basic rates that wireless client must support.', LEFT);" onMouseOut="return nd();">Basic Rate Set:
           </td><td class="content_input_td"><select name="wl_rateset" class="content_input_fd" onChange="return change_common(this, 'WLANConfig11b', 'wl_rateset')"><option class="content_input_fd" value="default" <% nvram_match_x("WLANConfig11b","wl_rateset", "default","selected"); %>>Default</option><option class="content_input_fd" value="all" <% nvram_match_x("WLANConfig11b","wl_rateset", "all","selected"); %>>All</option><option class="content_input_fd" value="12" <% nvram_match_x("WLANConfig11b","wl_rateset", "12","selected"); %>>1, 2 Mbps</option></select></td>
</tr>
<tr>
<td class="content_header_td" onMouseOver="return overlib('Selects a regulatory mode to use.', LEFT);" onMouseOut="return nd();">Regulatory Mode:
           </td><td class="content_input_td"><select name="wl_reg_mode" class="content_input_fd" onChange="return change_common(this, 'WLANConfig11b', 'wl_reg_mode')"><option class="content_input_fd" value="off" <% nvram_match_x("WLANConfig11b","wl_reg_mode", "off","selected"); %>>Off</option><option class="content_input_fd" value="d" <% nvram_match_x("WLANConfig11b","wl_reg_mode", "d","selected"); %>>802.11d</option><option class="content_input_fd" value="h" <% nvram_match_x("WLANConfig11b","wl_reg_mode", "h","selected"); %>>802.11h</option></select></td>
</tr>
<tr>
<td class="content_header_td">Fragmentation Threshold:
           </td><td class="content_input_td"><input type="text" maxlength="5" size="5" name="wl_frag" class="content_input_fd" value="<% nvram_get("wl_frag"); %>" onBlur="validate_range(this, 256, 2346)" onKeyPress="return is_number(event, this)"></td>
</tr>
<tr>
<td class="content_header_td">RTS Threshold:
           </td><td class="content_input_td"><input type="text" maxlength="5" size="5" name="wl_rts" class="content_input_fd" value="<% nvram_get("wl_rts"); %>" onBlur="validate_range(this, 0, 2347)" onKeyPress="return is_number(event, this)"></td>
</tr>
<tr>
<td class="content_header_td">DTIM Interval:
           </td><td class="content_input_td"><input type="text" maxlength="5" size="5" name="wl_dtim" class="content_input_fd" value="<% nvram_get("wl_dtim"); %>" onBlur="validate_range(this, 1, 255)" onKeyPress="return is_number(event, this)"></td>
</tr>
<tr>
<td class="content_header_td">Beacon Interval:
           </td><td class="content_input_td"><input type="text" maxlength="5" size="5" name="wl_bcn" class="content_input_fd" value="<% nvram_get("wl_bcn"); %>" onBlur="validate_range(this, 1, 65535)" onKeyPress="return is_number(event, this)"></td>
</tr>
<tr>
<td class="content_header_td" onMouseOver="return overlib('Selecting Yes enables frame bursting to improve performance.', LEFT);" onMouseOut="return nd();">Enable Frame Bursting?
           </td><td class="content_input_td"><select name="wl_frameburst" class="content_input_fd" onChange="return change_common(this, 'WLANConfig11b', 'wl_frameburst')"><option class="content_input_fd" value="off" <% nvram_match_x("WLANConfig11b","wl_frameburst", "off","selected"); %>>Disabled</option><option class="content_input_fd" value="on" <% nvram_match_x("WLANConfig11b","wl_frameburst", "on","selected"); %>>Enabled</option></select></td>
</tr>
<tr>
<td class="content_header_td" onMouseOver="return overlib('Selecting Station, ZVMODELVZ will connect to AP specified in Wireless - Interface page. Selecting Ethernet Bridge, ZVMODELVZ will connect to AP specified in Wireless - Interface page and allow devices connected in LAN port of ZVMODELVZ link to this AP as well. Station and Ethernet Bridge modes only works for Authentication Method set to Open System, Shared Key or WPA-PSK.', LEFT);" onMouseOut="return nd();">Extended Mode:
           </td><td class="content_input_td"><select name="wl_mode_ex" class="content_input_fd" onChange="return change_common(this, 'WLANConfig11b', 'wl_mode_ex')"><option class="content_input_fd" value="ap" <% nvram_match_x("WLANConfig11b","wl_mode_ex", "ap","selected"); %>>AP or WDS</option><option class="content_input_fd" value="sta" <% nvram_match_x("WLANConfig11b","wl_mode_ex", "sta","selected"); %>>Station</option><option class="content_input_fd" value="wet" <% nvram_match_x("WLANConfig11b","wl_mode_ex", "wet","selected"); %>>Ethernet Bridge</option></select></td>
</tr>
<tr>
<td class="content_header_td" onMouseOver="return overlib('Selecting Yes enables wireless function by date and time.', LEFT);" onMouseOut="return nd();">Enable Radio?
           </td><td class="content_input_td"><input type="radio" value="1" name="wl_radio_x" class="content_input_fd" onClick="return change_common_radio(this, 'WLANConfig11b', 'wl_radio_x', '1')" <% nvram_match_x("WLANConfig11b","wl_radio_x", "1", "checked"); %>>Yes</input><input type="radio" value="0" name="wl_radio_x" class="content_input_fd" onClick="return change_common_radio(this, 'WLANConfig11b', 'wl_radio_x', '0')" <% nvram_match_x("WLANConfig11b","wl_radio_x", "0", "checked"); %>>No</input></td>
</tr>
<tr>
<td class="content_header_td" onMouseOver="return overlib('This field defines the dates that wireless function will be enabled.', LEFT);" onMouseOut="return nd();">Date to Enable Radio:
           </td><td class="content_input_td"><input type="hidden" maxlength="7" class="content_input_fd" size="7" name="wl_radio_date_x" value="<% nvram_get("wl_radio_date_x"); %>">
<p style="word-spacing: 0; margin-top: 0; margin-bottom: 0">
<input type="checkbox" class="content_input_fd" name="wl_radio_date_x_Sun">Sun</input><input type="checkbox" class="content_input_fd" name="wl_radio_date_x_Mon">Mon</input><input type="checkbox" class="content_input_fd" name="wl_radio_date_x_Tue">Tue</input><input type="checkbox" class="content_input_fd" name="wl_radio_date_x_Wed">Wed</input>
</p>
<input type="checkbox" class="content_input_fd" name="wl_radio_date_x_Thu">Thu</input><input type="checkbox" class="content_input_fd" name="wl_radio_date_x_Fri">Fri</input><input type="checkbox" class="content_input_fd" name="wl_radio_date_x_Sat">Sat</input></td>
</tr>
<tr>
<td class="content_header_td" onMouseOver="return overlib('This field defines the time interval that wireless function will be enabled.', LEFT);" onMouseOut="return nd();">Time of Day to Enable Radio:
           </td><td class="content_input_td"><input type="hidden" maxlength="11" class="content_input_fd" size="11" name="wl_radio_time_x" value="<% nvram_get("wl_radio_time_x"); %>"><input type="text" maxlength="2" class="content_input_fd" size="2" name="wl_radio_time_x_starthour" onKeyPress="return is_number(event, this)" onBlur="return validate_timerange(this, 0)">:
                <input type="text" maxlength="2" class="content_input_fd" size="2" name="wl_radio_time_x_startmin" onKeyPress="return is_number(event, this)" onBlur="return validate_timerange(this, 1)">-
                <input type="text" maxlength="2" class="content_input_fd" size="2" name="wl_radio_time_x_endhour" onKeyPress="return is_number(event, this)" onBlur="return validate_timerange(this, 2)">:
                <input type="text" maxlength="2" class="content_input_fd" size="2" name="wl_radio_time_x_endmin" onKeyPress="return is_number(event, this)" onBlur="return validate_timerange(this, 3)"></td>
</tr>
<tr>
<td class="content_header_td" onMouseOver="return overlib('The Radio Power should be between 0 to 400 mW, 0 is used for hardware defaults. But default value is recommended.', LEFT);" onMouseOut="return nd();">Radio Power:
           </td><td class="content_input_td"><input type="text" maxlength="5" size="5" name="wl_radio_power_x" class="content_input_fd" value="<% nvram_get("wl_radio_power_x"); %>" onBlur="validate_range(this, 0, 400)" onKeyPress="return is_number(event, this)"></td>
</tr>
<tr>
<td class="content_header_td" onMouseOver="return overlib('Enable or Disable WMM.', LEFT);" onMouseOut="return nd();">Enable WMM?
           </td><td class="content_input_td"><select name="wl_wme" class="content_input_fd" onChange="return change_common(this, 'WLANConfig11b', 'wl_wme')"><option class="content_input_fd" value="auto" <% nvram_match_x("WLANConfig11b","wl_wme", "auto","selected"); %>>Auto</option><option class="content_input_fd" value="off" <% nvram_match_x("WLANConfig11b","wl_wme", "off","selected"); %>>Disabled</option><option class="content_input_fd" value="on" <% nvram_match_x("WLANConfig11b","wl_wme", "on","selected"); %>>Enabled</option></select></td>
</tr>
<tr>
<td class="content_header_td" onMouseOver="return overlib('Enable or Disable WMM No-Acknowledgment.', LEFT);" onMouseOut="return nd();">Enable WMM No-Acknowledgement?
           </td><td class="content_input_td"><select name="wl_wme_no_ack" class="content_input_fd" onChange="return change_common(this, 'WLANConfig11b', 'wl_wme_no_ack')"><option class="content_input_fd" value="off" <% nvram_match_x("WLANConfig11b","wl_wme_no_ack", "off","selected"); %>>Disabled</option><option class="content_input_fd" value="on" <% nvram_match_x("WLANConfig11b","wl_wme_no_ack", "on","selected"); %>>Enabled</option></select></td>
</tr>
<tr>
<td class="content_header_td" onMouseOver="return overlib('Selecting Yes enables Wireless Multicast Forwarding.', LEFT);" onMouseOut="return nd();">Enable WMF?
           </td><td class="content_input_td">
           <input type="radio" value="1" name="wl_wmf_bss_enable" class="content_input_fd" onClick="return change_common_radio(this, 'WLANConfig11b', 'wl_wmf_bss_enable', '1')" <% nvram_match_x("WLANConfig11b","wl_wmf_bss_enable", "1", "checked"); %>>Yes</input>
           <input type="radio" value="0" name="wl_wmf_bss_enable" class="content_input_fd" onClick="return change_common_radio(this, 'WLANConfig11b', 'wl_wmf_bss_enable', '0')" <% nvram_match_x("WLANConfig11b","wl_wmf_bss_enable", "0", "checked"); %>>No</input></td>
</tr>
</table>

<% include("footer_buttons.inc"); %>

</form>
</body>
</html>
