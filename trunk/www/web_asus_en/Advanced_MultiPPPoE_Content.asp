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
<input type="hidden" name="current_page" value="Advanced_MultiPPPoE_Content.asp">
<input type="hidden" name="next_page" value="Advanced_DHCP_Content.asp">
<input type="hidden" name="next_host" value="">
<input type="hidden" name="sid_list" value="Layer3Forwarding;LANHostConfig;IPConnection;PPPConnection;">
<input type="hidden" name="group_id" value="">
<input type="hidden" name="modified" value="0">
<input type="hidden" name="action_mode" value="">
<input type="hidden" name="first_time" value="">
<input type="hidden" name="action_script" value="">
<!-- Table for the conntent page -->	    
<table width="666" border="1" cellpadding="0" cellspacing="0" bordercolor="#E0E0E0">
<tr class="content_header_tr">
<td class="content_header_td_title" colspan="2">IP Config - PPPoE Sessions</td>
</tr>
<tr>
<td class="content_desc_td" colspan="2">This function allows you to apply more than one PPPoE session according where you want to connect to.
         </td>
</tr>
<tr>
<td class="content_header_td" onMouseOver="return overlib('Please select one Session for your default connection. Session 0 means PPPoE session set in WAN &amp; LAN setting.', LEFT);" onMouseOut="return nd();">Default PPPoE Session:
           </td><td class="content_input_td"><select name="" class="content_input_fd" onChange="return change_common(this, 'PPPConnection', '')"><option class="content_input_fd" value="0" <% nvram_match_x("PPPConnection","", "0","selected"); %>>PPPoE Session 0</option><option class="content_input_fd" value="1" <% nvram_match_x("PPPConnection","", "1","selected"); %>>PPPoE Session 1</option><option class="content_input_fd" value="2" <% nvram_match_x("PPPConnection","", "2","selected"); %>>PPPoE Session 2</option></select></td>
</tr>
<tr class="content_section_header_tr">
<td class="content_section_header_td" colspan="2">PPPoE Session 1
            </td>
</tr>
<tr>
<td class="content_header_td">Enable PPPoE Session 1?
           </td><td class="content_input_td"><input type="radio" value="1" name="" class="content_input_fd" onClick="return change_common_radio(this, 'PPPConnection', '', '1')" <% nvram_match_x("PPPConnection","", "1", "checked"); %>>Yes</input><input type="radio" value="0" name="" class="content_input_fd" onClick="return change_common_radio(this, 'PPPConnection', '', '0')" <% nvram_match_x("PPPConnection","", "0", "checked"); %>>No</input></td>
</tr>
<tr>
<td class="content_header_td" onMouseOver="return overlib('This field is only available, when you set WAN Connection Type as PPPoE.', LEFT);" onMouseOut="return nd();">User Name:
           </td><td class="content_input_td"><input type="text" maxlength="64" class="content_input_fd" size="32" name="" value="<% nvram_get(""); %>" onKeyPress="return is_string(event, this)" onBlur="validate_string(this)"></td>
</tr>
<tr>
<td class="content_header_td" onMouseOver="return overlib('This field is only available, when you set WAN Connection Type as PPPoE', LEFT);" onMouseOut="return nd();">Password:
           </td><td class="content_input_td"><input type="password" maxlength="64" class="content_input_fd" size="32" name="" value="<% nvram_get(""); %>" onBlur="validate_string(this)"></td>
</tr>
<tr>
<td class="content_header_td" onMouseOver="return overlib('This field allows you to configure to terminate your ISP connection after a specified period of time. A value of zero allows infinite idle time. If Tx Only is checked, the data from Internet will be skipped for counting idle time.', LEFT);" onMouseOut="return nd();">Idle Disconnect Time in seconds(option):
           </td><td class="content_input_td"><input type="text" maxlength="10" class="content_input_fd" size="10" name="" value="<% nvram_get(""); %>" onBlur="validate_range(this, 0, 4294967295)" onKeyPress="return is_number(event, this)"><input type="checkbox" style="margin-left:30" name="_check" value="" onClick="return change_common_radio(this, 'PPPConnection', '', '1')">Tx Only</input></td>
</tr>
<input type="hidden" name="" value="<% nvram_get(""); %>">
<tr>
<td class="content_header_td" onMouseOver="return overlib('That is Maximum Transmission Unit(MTU) of PPPoE packet.', LEFT);" onMouseOut="return nd();">MTU:
           </td><td class="content_input_td"><input type="text" maxlength="5" size="5" name="" class="content_input_fd" value="<% nvram_get(""); %>" onBlur="validate_range(this, 576, 1492)" onKeyPress="return is_number(event, this)"></td>
</tr>
<tr>
<td class="content_header_td" onMouseOver="return overlib('That is Maximum Receive Unit(MRU) of PPPoE packet.', LEFT);" onMouseOut="return nd();">MRU:
           </td><td class="content_input_td"><input type="text" maxlength="5" size="5" name="" class="content_input_fd" value="<% nvram_get(""); %>" onBlur="validate_range(this, 576, 1492)" onKeyPress="return is_number(event, this)"></td>
</tr>
<tr>
<td class="content_header_td" onMouseOver="return overlib('This item may be specified by some ISPs. Check with your ISP and fill them in if required.', LEFT);" onMouseOut="return nd();">Service Name(option):
           </td><td class="content_input_td"><input type="text" maxlength="32" class="content_input_fd" size="32" name="" value="<% nvram_get(""); %>" onKeyPress="return is_string(event, this)" onBlur="validate_string(this)"></td>
</tr>
<tr>
<td class="content_header_td" onMouseOver="return overlib('This item may be specified by some ISPs. Check with your ISP and fill them in if required.', LEFT);" onMouseOut="return nd();">Access Concentrator Name(option):
           </td><td class="content_input_td"><input type="text" maxlength="32" class="content_input_fd" size="32" name="" value="<% nvram_get(""); %>" onKeyPress="return is_string(event, this)" onBlur="validate_string(this)"></td>
</tr>
<tr class="content_section_header_tr">
<td class="content_section_header_td" colspan="2">PPPoE Session 2
            </td>
</tr>
<tr>
<td class="content_header_td">Enable PPPoE Session 2?
           </td><td class="content_input_td"><input type="radio" value="1" name="" class="content_input_fd" onClick="return change_common_radio(this, 'PPPConnection', '', '1')" <% nvram_match_x("PPPConnection","", "1", "checked"); %>>Yes</input><input type="radio" value="0" name="" class="content_input_fd" onClick="return change_common_radio(this, 'PPPConnection', '', '0')" <% nvram_match_x("PPPConnection","", "0", "checked"); %>>No</input></td>
</tr>
<tr>
<td class="content_header_td" onMouseOver="return overlib('This field is only available, when you set WAN Connection Type as PPPoE.', LEFT);" onMouseOut="return nd();">User Name:
           </td><td class="content_input_td"><input type="text" maxlength="64" class="content_input_fd" size="32" name="" value="<% nvram_get(""); %>" onKeyPress="return is_string(event, this)" onBlur="validate_string(this)"></td>
</tr>
<tr>
<td class="content_header_td" onMouseOver="return overlib('This field is only available, when you set WAN Connection Type as PPPoE', LEFT);" onMouseOut="return nd();">Password:
           </td><td class="content_input_td"><input type="password" maxlength="64" class="content_input_fd" size="32" name="" value="<% nvram_get(""); %>" onBlur="validate_string(this)"></td>
</tr>
<tr>
<td class="content_header_td" onMouseOver="return overlib('This field allows you to configure to terminate your ISP connection after a specified period of time. A value of zero allows infinite idle time. If Tx Only is checked, the data from Internet will be skipped for counting idle time.', LEFT);" onMouseOut="return nd();">Idle Disconnect Time in seconds(option):
           </td><td class="content_input_td"><input type="text" maxlength="10" class="content_input_fd" size="10" name="" value="<% nvram_get(""); %>" onBlur="validate_range(this, 0, 4294967295)" onKeyPress="return is_number(event, this)"><input type="checkbox" style="margin-left:30" name="_check" value="" onClick="return change_common_radio(this, 'PPPConnection', '', '1')">Tx Only</input></td>
</tr>
<input type="hidden" name="" value="<% nvram_get(""); %>">
<tr>
<td class="content_header_td" onMouseOver="return overlib('That is Maximum Transmission Unit(MTU) of PPPoE packet.', LEFT);" onMouseOut="return nd();">MTU:
           </td><td class="content_input_td"><input type="text" maxlength="5" size="5" name="" class="content_input_fd" value="<% nvram_get(""); %>" onBlur="validate_range(this, 576, 1492)" onKeyPress="return is_number(event, this)"></td>
</tr>
<tr>
<td class="content_header_td" onMouseOver="return overlib('That is Maximum Receive Unit(MRU) of PPPoE packet.', LEFT);" onMouseOut="return nd();">MRU:
           </td><td class="content_input_td"><input type="text" maxlength="5" size="5" name="" class="content_input_fd" value="<% nvram_get(""); %>" onBlur="validate_range(this, 576, 1492)" onKeyPress="return is_number(event, this)"></td>
</tr>
<tr>
<td class="content_header_td" onMouseOver="return overlib('This item may be specified by some ISPs. Check with your ISP and fill them in if required.', LEFT);" onMouseOut="return nd();">Service Name(option):
           </td><td class="content_input_td"><input type="text" maxlength="32" class="content_input_fd" size="32" name="" value="<% nvram_get(""); %>" onKeyPress="return is_string(event, this)" onBlur="validate_string(this)"></td>
</tr>
<tr>
<td class="content_header_td" onMouseOver="return overlib('This item may be specified by some ISPs. Check with your ISP and fill them in if required.', LEFT);" onMouseOut="return nd();">Access Concentrator Name(option):
           </td><td class="content_input_td"><input type="text" maxlength="32" class="content_input_fd" size="32" name="" value="<% nvram_get(""); %>" onKeyPress="return is_string(event, this)" onBlur="validate_string(this)"></td>
</tr>
</table>

<table width="666" border="2" cellpadding="0" cellspacing="0" bordercolor="#E0E0E0">
<tr class="content_list_header_tr">
<td class="content_list_header_td" width="60%" id="PPPoERouteList">PPPoE Route List
         <input type="hidden" name="PPPoERouteCount_0" value="<% nvram_get("PPPoERouteCount"); %>" readonly></td><td width="10%">
<div align="center">
<input class="inputSubmit" type="submit" onMouseOut="buttonOut(this)" onMouseOver="buttonOver(this)" onClick="return markGroup(this, 'PPPoERouteList', 4, ' Add ');" name="PPPoERouteList" value="Add" size="12">
</div>
</td><td width="10%">
<div align="center">
<input class="inputSubmit" type="submit" onMouseOut="buttonOut(this)" onMouseOver="buttonOver(this)" onClick="return markGroup(this, 'PPPoERouteList', 4, ' Del ');" name="PPPoERouteList" value="Del" size="12">
</div>
</td><td width="5%"></td>
</tr>
<table class="content_list_table" width="640" border="0" cellspacing="0" cellpadding="0">
<tr>
<td colspan="3" height="10"></td>
</tr>
<tr>
<td colspan="3">
<div align="center">
<table class="content_list_value_table" border="1" cellspacing="0" cellpadding="0">
<tr>
<td></td><td class="content_list_field_header_td" colspan="">Session	                
           	        </td><td class="content_list_field_header_td" colspan="">IP Type	                
           	        </td><td class="content_list_field_header_td" colspan="">Network/Host IP	                
           	        </td><td class="content_list_field_header_td" colspan="">Netmask Bits	                
           	        </td><td></td>
</tr>
<tr>
<td></td><td class="content_list_input_td" colspan=""><select name="_0" class="content_input_list_fd"><option value="0" <% nvram_match_list_x("PPPConnection","", "0","selected", 0); %>>0</option><option value="1" <% nvram_match_list_x("PPPConnection","", "1","selected", 0); %>>1</option><option value="2" <% nvram_match_list_x("PPPConnection","", "2","selected", 0); %>>2</option></select></td><td class="content_list_input_td" colspan=""><select name="_0" class="content_input_list_fd"><option value="Source" <% nvram_match_list_x("PPPConnection","", "Source","selected", 0); %>>Source</option><option value="Destination" <% nvram_match_list_x("PPPConnection","", "Destination","selected", 0); %>>Destination</option></select></td><td class="content_list_input_td" colspan=""><input type="text" maxlength="15" class="content_input_list_fd" size="14" name="_0" onKeyPress="return is_ipaddr(event, this)" onKeyUp="change_ipaddr(this)"></td><td class="content_list_input_td" colspan=""><input type="text" maxlength="5" style="font-family: monospace; font-size: 10pt; width: 100%;" size="8" name="_0" onKeyPress="return is_number(event, this)"></td>
</tr>
<tr>
<td></td><td colspan="10"><select size="4" name="PPPoERouteList_s" multiple="true" class="content_list_body">
<% nvram_get_table_x("PPPConnection","PPPoERouteList"); %>
</select></td>
</tr>
</table>
</div>
</td>
</tr>
<tr>
<td colspan="3" height="10"></td>
</tr>
</table>
</table>

<% include("footer_buttons.inc"); %>

</form>
</body>
</html>
