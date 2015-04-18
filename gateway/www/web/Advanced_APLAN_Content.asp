<html>
<head>
<title><% model(2); %></title>
<meta http-equiv="Content-Type" content="text/html; charset=UTF-8">
<link rel="stylesheet" type="text/css" href="style.css" media="screen">
<script type="text/javascript" src="overlib.js"></script>
<script type="text/javascript" src="general.js"></script>
</head>  
<body onLoad="load_body()" onunLoad="return unload_body();">
<div id="overDiv" style="position: absolute; visibility: hidden; z-index: 1000;"></div>
<form method="GET" name="form" action="apply.cgi">
<input type="hidden" name="current_page" value="Advanced_APLAN_Content.asp"><input type="hidden" name="next_page" value="SaveRestart.asp"><input type="hidden" name="next_host" value=""><input type="hidden" name="sid_list" value="LANHostConfig;"><input type="hidden" name="group_id" value=""><input type="hidden" name="modified" value="0"><input type="hidden" name="action_mode" value=""><input type="hidden" name="first_time" value=""><input type="hidden" name="action_script" value="">
<!-- Table for the conntent page -->	    
<table width="666" border="0" cellpadding="0" cellspacing="0">

<tr>
<td>
<table width="666" border="1" cellpadding="0" cellspacing="0" bordercolor="#E0E0E0">
<tr class="content_header_tr">
<td class="content_header_td_title" colspan="2">IP Config - LAN</td>
</tr>
<tr class="content_section_header_tr">
<td class="content_section_header_td" colspan="2">LAN IP Setting</td>
</tr>
<tr>
<td class="content_header_td" onMouseOver="return overlib('This field allows you to provide a LAN host name for the router.', LEFT);" onMouseOut="return nd();">Host Name:</td>
<td class="content_input_td"><input type="text" maxlength="32" class="content_input_fd" size="32" name="lan_hostname" value="<% nvram_get("lan_hostname"); %>" onKeyPress="return is_string(event, this)" onBlur="validate_string(this)"></td>
</tr>
<tr>
<td class="content_header_td">Get IP Automatically?</td>
<td class="content_input_td"><input type="radio" value="1" name="lan_proto_x" class="content_input_fd" onClick="return change_common_radio(this, 'LANHostConfig', 'lan_proto_x', '1')" <% nvram_match("lan_proto_x", "1", "checked"); %>>Yes<input type="radio" value="0" name="lan_proto_x" class="content_input_fd" onClick="return change_common_radio(this, 'LANHostConfig', 'lan_proto_x', '0')" <% nvram_match("lan_proto_x", "0", "checked"); %>>No</td>
</tr>
<tr>
<td class="content_header_td" onMouseOver="return overlib('This is IP Address of the router as seen in your local network. The default value is 192.168.1.1.', LEFT);" onMouseOut="return nd();">IP Address:</td>
<td class="content_input_td"><input type="text" maxlength="15" class="content_input_fd" size="15" name="lan_ipaddr" value="<% nvram_get("lan_ipaddr"); %>" onBlur="return validate_ipaddr(this, 'lan_ipaddr')" onKeyPress="return is_ipaddr(event, this)" onKeyUp="change_ipaddr(this)"></td>
</tr>
<tr>
<td class="content_header_td" onMouseOver="return overlib('This is Subnet Mask of the router as seen in your local network. The default value is 255.255.255.0.', LEFT);" onMouseOut="return nd();">Subnet Mask:</td>
<td class="content_input_td"><input type="text" maxlength="15" class="content_input_fd" size="15" name="lan_netmask" value="<% nvram_get("lan_netmask"); %>" onBlur="return validate_ipaddr(this, 'lan_netmask')" onKeyPress="return is_ipaddr(event, this)" onKeyUp="change_ipaddr(this)"></td>
</tr>
<tr>
<td class="content_header_td" onMouseOver="return overlib('This is the IP address of default gateway for Access Point', LEFT);" onMouseOut="return nd();">Default Gateway:</td>
<td class="content_input_td"><input type="text" maxlength="15" class="content_input_fd" size="15" name="lan_gateway" value="<% nvram_get("lan_gateway"); %>" onBlur="return validate_ipaddr(this, 'lan_gateway')" onKeyPress="return is_ipaddr(event, this)" onKeyUp="change_ipaddr(this)"></td>
</tr>
<tr>
<td class="content_header_td" onMouseOver="return overlib('This is the IP address of DNS Server for Access Point', LEFT);" onMouseOut="return nd();">DNS Server:</td>
<td class="content_input_td"><input type="text" maxlength="15" class="content_input_fd" size="15" name="lan_dns" value="<% nvram_get("lan_dns"); %>" onBlur="return validate_ipaddr(this, 'lan_dns')" onKeyPress="return is_ipaddr(event, this)" onKeyUp="change_ipaddr(this)"></td>
</tr>
</table>
</td>
</tr>
</table>

<% include("footer_buttons.inc"); %>

</form>
</body>
</html>
