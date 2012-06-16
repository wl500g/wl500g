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
<input type="hidden" name="current_page" value="Advanced_DHCP_Content.asp">
<input type="hidden" name="next_page" value="Advanced_GWStaticRoute_Content.asp">
<input type="hidden" name="next_host" value="">
<input type="hidden" name="sid_list" value="LANHostConfig;">
<input type="hidden" name="group_id" value="">
<input type="hidden" name="modified" value="0">
<input type="hidden" name="action_mode" value="">
<input type="hidden" name="first_time" value="">
<input type="hidden" name="action_script" value="">
<!-- Table for the conntent page -->	    
<table width="666" border="0" cellpadding="0" cellspacing="0">     	      
<tr>
<td>
<table width="666" border="1" cellpadding="0" cellspacing="0" bordercolor="#E0E0E0">
<tr class="content_header_tr">
<td class="content_header_td_title" colspan="2">IP Config - DHCP Server</td>
</tr>
<tr>
<td class="content_desc_td" colspan="2">ZVMODELVZ supports up to 253 IP addresses for your local network. The IP address of a local machine can be assigned manually by the network administrator or obtained automatically from ZVMODELVZ if the DHCP server is enabled.
         </td>
</tr>
<tr>
<td class="content_header_td">Enable the DHCP Server?
           </td><td class="content_input_td"><input type="radio" value="1" name="dhcp_enable_x" class="content_input_fd" onClick="return change_common_radio(this, 'LANHostConfig', 'dhcp_enable_x', '1')" <% nvram_match_x("LANHostConfig","dhcp_enable_x", "1", "checked"); %>>Yes</input><input type="radio" value="0" name="dhcp_enable_x" class="content_input_fd" onClick="return change_common_radio(this, 'LANHostConfig', 'dhcp_enable_x', '0')" <% nvram_match_x("LANHostConfig","dhcp_enable_x", "0", "checked"); %>>No</input></td>
</tr>
<tr>
<td class="content_header_td" onMouseOver="return overlib('This field indicates the Domain Name to provide to clients that request IP Address from DHCP Server.', LEFT);" onMouseOut="return nd();">Domain Name:
           </td><td class="content_input_td"><input type="text" maxlength="32" class="content_input_fd" size="32" name="lan_domain" value="<% nvram_get_x("LANHostConfig","lan_domain"); %>" onKeyPress="return is_string(event, this)" onBlur="validate_string(this)"></td>
</tr>
<tr>
<td class="content_header_td" onMouseOver="return overlib('This field specifies the first address in the pool to be assigned by the DHCP server in your local network.', LEFT);" onMouseOut="return nd();">IP Pool Starting Address:
           </td><td class="content_input_td"><input type="text" maxlength="15" class="content_input_fd" size="15" name="dhcp_start" value="<% nvram_get_x("LANHostConfig","dhcp_start"); %>" onBlur="return validate_ipaddr(this, 'dhcp_start')" onKeyPress="return is_ipaddr(event, this)" onKeyUp="change_ipaddr(this)"></td>
</tr>
<tr>
<td class="content_header_td" onMouseOver="return overlib('This field specifies the last address in the pool to be assigned by the DHCP server in your local network.', LEFT);" onMouseOut="return nd();">IP Pool Ending Address:
           </td><td class="content_input_td"><input type="text" maxlength="15" class="content_input_fd" size="15" name="dhcp_end" value="<% nvram_get_x("LANHostConfig","dhcp_end"); %>" onBlur="return validate_ipaddr(this, 'dhcp_end')" onKeyPress="return is_ipaddr(event, this)" onKeyUp="change_ipaddr(this)"></td>
</tr>
<tr>
<td class="content_header_td" onMouseOver="return overlib('This field specifies the amount of connection time a network user be allowed with their current dynamic IP address.', LEFT);" onMouseOut="return nd();">Lease Time:
           </td><td class="content_input_td"><input type="text" maxlength="6" size="6" name="dhcp_lease" class="content_input_fd" value="<% nvram_get_x("LANHostConfig", "dhcp_lease"); %>" onBlur="validate_range_time(this, 1, 604800)"></td>
</tr>
<tr>
<td class="content_header_td" onMouseOver="return overlib('This field indicates the IP address of gateway in your LAN. If you leave it blank, the IP address of ZVMODELVZ will be assigned.', LEFT);" onMouseOut="return nd();">Default Gateway:
           </td><td class="content_input_td"><input type="text" maxlength="15" class="content_input_fd" size="15" name="dhcp_gateway_x" value="<% nvram_get_x("LANHostConfig","dhcp_gateway_x"); %>" onBlur="return validate_ipaddr(this, 'dhcp_gateway_x')" onKeyPress="return is_ipaddr(event, this)" onKeyUp="change_ipaddr(this)"></td>
</tr>
<tr class="content_section_header_tr">
<td class="content_section_header_td" colspan="2">DNS and WINS Server Setting
            </td>
</tr>
<tr>
<td class="content_header_td" onMouseOver="return overlib('This field indicates the IP address of DNS to provide to clients that request IP Address from DHCP Server. You can leave it blank, then the DNS request will be processed by ZVMODELVZ.', LEFT);" onMouseOut="return nd();">DNS Server 1:
           </td><td class="content_input_td"><input type="text" maxlength="15" class="content_input_fd" size="15" name="dhcp_dns1_x" value="<% nvram_get_x("LANHostConfig","dhcp_dns1_x"); %>" onBlur="return validate_ipaddr(this, 'dhcp_dns1_x')" onKeyPress="return is_ipaddr(event, this)" onKeyUp="change_ipaddr(this)"></td>
</tr>
<tr>
<td class="content_header_td">DNS Server 2:
           </td><td class="content_input_td"><input type="text" maxlength="15" class="content_input_fd_ro" size="15" name="" value="<% nvram_get_x("LANHostConfig","lan_ipaddr"); %>" readonly></td>
</tr>
<tr>
<td class="content_header_td" onMouseOver="return overlib('The Windows Internet Naming Service manages interaction of each PC with the Internet. If you use a WINS server, enter IP Address of server here.', LEFT);" onMouseOut="return nd();">WINS Server:
           </td><td class="content_input_td"><input type="text" maxlength="15" class="content_input_fd" size="15" name="dhcp_wins_x" value="<% nvram_get_x("LANHostConfig","dhcp_wins_x"); %>" onBlur="return validate_ipaddr(this, 'dhcp_wins_x')" onKeyPress="return is_ipaddr(event, this)" onKeyUp="change_ipaddr(this)"></td>
</tr>
<tr class="content_section_header_tr">
<td class="content_section_header_td" colspan="2">Assign IP Address Manually
            </td>
</tr>
<tr>
<td class="content_header_td">Enable Manual Assignment?
           </td><td class="content_input_td"><input type="radio" value="1" name="dhcp_static_x" class="content_input_fd" onClick="return change_common_radio(this, 'LANHostConfig', 'dhcp_static_x', '1')" <% nvram_match_x("LANHostConfig","dhcp_static_x", "1", "checked"); %>>Yes</input><input type="radio" value="0" name="dhcp_static_x" class="content_input_fd" onClick="return change_common_radio(this, 'LANHostConfig', 'dhcp_static_x', '0')" <% nvram_match_x("LANHostConfig","dhcp_static_x", "0", "checked"); %>>No</input></td>
</tr>
</table>
</td>
</tr>
<tr>
<td>
<table width="666" border="2" cellpadding="0" cellspacing="0" bordercolor="#E0E0E0">
<tr class="content_list_header_tr">
<td class="content_list_header_td" width="60%" id="ManualDHCPList">Manually Assigned IP List
         <input type="hidden" name="dhcp_staticnum_x_0" value="<% nvram_get_x("LANHostConfig", "dhcp_staticnum_x"); %>" readonly></td><td width="10%">
<div align="center">
<input class="inputSubmit" type="submit" onMouseOut="buttonOut(this)" onMouseOver="buttonOver(this)" onClick="return markGroup(this, 'ManualDHCPList', 32, ' Add ');" name="ManualDHCPList" value="Add" size="12">
</div>
</td><td width="10%">
<div align="center">
<input class="inputSubmit" type="submit" onMouseOut="buttonOut(this)" onMouseOver="buttonOver(this)" onClick="return markGroup(this, 'ManualDHCPList', 32, ' Del ');" name="ManualDHCPList" value="Del" size="12">
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
	<td></td>
	<td class="content_list_field_header_td" colspan="">MAC Address</td>
	<td class="content_list_field_header_td" colspan="">IP Address</td>
	<td class="content_list_field_header_td" colspan="">Host Name</td>
	<td></td>
</tr>
<tr>
	<td></td>
	<td class="content_list_input_td" colspan=""><input type="text" maxlength="12" class="content_input_list_fd" size="12" name="dhcp_staticmac_x_0" onKeyPress="return is_hwaddr(event, this)"></td>
	<td class="content_list_input_td" colspan=""><input type="text" maxlength="15" class="content_input_list_fd" size="14" name="dhcp_staticip_x_0" onKeyPress="return is_ipaddr(event, this)" onKeyUp="change_ipaddr(this)"></td>
	<td class="content_list_input_td" colspan=""><input type="text" maxlength="32" class="content_input_list_fd" size="32" name="dhcp_staticname_x_0" onKeyPress="return is_string(event, this)"></td>
</tr>
<tr>
<td></td><td colspan="10"><select size="4" name="ManualDHCPList_s" multiple="true" class="content_list_body">
<% nvram_get_table_x("LANHostConfig","ManualDHCPList"); %>
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
</td>
</tr>
</table>

<% include("footer_buttons.inc"); %>

</form>
</body>
</html>
