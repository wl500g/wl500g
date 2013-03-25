<!--<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.01//EN" "http://www.w3.org/TR/html4/strict.dtd">-->
<html>
<head>
	<title><% model(2); %></title>
	<meta http-equiv="Content-Type" content="text/html; charset=UTF-8">
	<link rel="stylesheet" type="text/css" href="style.css" media="screen">

	<script type="text/javascript" src="overlib.js"></script>

	<script type="text/javascript" src="general.js"></script>

</head>
<body onload="load_body()" onunload="return unload_body();">
	<div id="overDiv" style="position: absolute; visibility: hidden; z-index: 1000;">
	</div>
	<form method="GET" name="form" action="apply.cgi">
	<input type="hidden" name="current_page" value="Advanced_LAN_Content.asp">
	<input type="hidden" name="next_page" value="Advanced_DHCP_Content.asp">
	<input type="hidden" name="next_host" value="">
	<input type="hidden" name="sid_list" value="Layer3Forwarding;LANHostConfig;IPConnection;PPPConnection;">
	<input type="hidden" name="group_id" value=""><input type="hidden" name="modified"
		value="0">
	<input type="hidden" name="action_mode" value=""><input type="hidden" name="first_time"
		value="">
	<input type="hidden" name="action_script" value="">

	<input type="hidden" name="dhcp_start" value="<% nvram_get("dhcp_start"); %>">
	<input type="hidden" name="dhcp_end" value="<% nvram_get("dhcp_end"); %>">
	<input type="hidden" name="wan_ipaddr" value="<% nvram_get("wan_ipaddr"); %>">

	<!-- Table for the conntent page -->
				<table width="666" border="1" cellpadding="0" cellspacing="0" bordercolor="#E0E0E0">
					<tr class="content_header_tr">
						<td class="content_header_td_title" colspan="2">
							IP Config - LAN
						</td>
					</tr>
					<tr>
						<td class="content_desc_td" colspan="2">
							Configure the LAN IP of the router.
						</td>
					</tr>

					<tr class="content_section_header_tr">
						<td class="content_section_header_td" colspan="2">
							LAN IP Setting
						</td>
					</tr>
					<tr>
						<td class="content_header_td" onmouseover="return overlib('This field allows you to provide a LAN host name for the router.', LEFT);"
							onmouseout="return nd();">
							Host Name:
						</td>
						<td class="content_input_td">
							<input type="text" maxlength="32" class="content_input_fd" size="32" name="lan_hostname"
								value="<% nvram_get("lan_hostname"); %>" onkeypress="return is_string(event, this)"
								onblur="validate_string(this)">
						</td>
					</tr>
					<tr>
						<td class="content_header_td" onmouseover="return overlib('This is IP Address of the router as seen in your local network. The default value is 192.168.1.1.', LEFT);"
							onmouseout="return nd();">
							IP Address:
						</td>
						<td class="content_input_td">
							<input type="text" maxlength="15" class="content_input_fd" size="15" name="lan_ipaddr"
								value="<% nvram_get("lan_ipaddr"); %>" onblur="return validate_ipaddr(this, 'lan_ipaddr')"
								onkeypress="return is_ipaddr(event, this)" onkeyup="change_ipaddr(this)">
						</td>
					</tr>
					<tr>
						<td class="content_header_td" onmouseover="return overlib('This is Subnet Mask of the router as seen in your local network. The default value is 255.255.255.0.', LEFT);"
							onmouseout="return nd();">
							Subnet Mask:
						</td>
						<td class="content_input_td">
							<input type="text" maxlength="15" class="content_input_fd" size="15" name="lan_netmask"
								value="<% nvram_get("lan_netmask"); %>" onblur="return validate_ipaddr(this, 'lan_netmask')"
								onkeypress="return is_ipaddr(event, this)" onkeyup="change_ipaddr(this)">
						</td>
					</tr>
				</table>

<% include("footer_buttons.inc"); %>

	</form>
</body>
</html>
