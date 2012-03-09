<!--<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.01//EN" "http://www.w3.org/TR/html4/strict.dtd">-->
<html>
<head>
	<title>ZVMODELVZ Web Manager</title>
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

	<input type="hidden" name="dhcp_start" value="<% nvram_get_x("LANHostConfig","dhcp_start"); %>">
	<input type="hidden" name="dhcp_end" value="<% nvram_get_x("LANHostConfig","dhcp_end"); %>">
	<input type="hidden" name="wan_ipaddr" value="<% nvram_get_x("IPConnection","wan_ipaddr"); %>">

	<!-- Table for the conntent page -->
				<table width="666" border="1" cellpadding="0" cellspacing="0" bordercolor="#E0E0E0">
					<tr class="content_header_tr">
						<td class="content_header_td_title" colspan="2">
							IP Config - LAN
						</td>
					</tr>
					<tr>
						<td class="content_desc_td" colspan="2">
							Input LAN-settings for ZVMODELVZ.
						</td>
					</tr>

					<tr class="content_section_header_tr">
						<td class="content_section_header_td" colspan="2">
							LAN IP Setting
						</td>
					</tr>
					<tr>
						<td class="content_header_td" onmouseover="return overlib('This field allows you to provide a LAN host name for ZVMODELVZ.', LEFT);"
							onmouseout="return nd();">
							Host Name:
						</td>
						<td class="content_input_td">
							<input type="text" maxlength="32" class="content_input_fd" size="32" name="lan_hostname"
								value="<% nvram_get_x("LANHostConfig","lan_hostname"); %>" onkeypress="return is_string(event, this)"
								onblur="validate_string(this)">
						</td>
					</tr>
					<tr>
						<td class="content_header_td" onmouseover="return overlib('This is IP Address of ZVMODELVZ as seen in your local network. The default value is 192.168.1.1.', LEFT);"
							onmouseout="return nd();">
							IP Address:
						</td>
						<td class="content_input_td">
							<input type="text" maxlength="15" class="content_input_fd" size="15" name="lan_ipaddr"
								value="<% nvram_get_x("LANHostConfig","lan_ipaddr"); %>" onblur="return validate_ipaddr(this, 'lan_ipaddr')"
								onkeypress="return is_ipaddr(event, this)" onkeyup="change_ipaddr(this)">
						</td>
					</tr>
					<tr>
						<td class="content_header_td" onmouseover="return overlib('This is Subnet Mask of ZVMODELVZ as seen in your local network. The default value is 255.255.255.0.', LEFT);"
							onmouseout="return nd();">
							Subnet Mask:
						</td>
						<td class="content_input_td">
							<input type="text" maxlength="15" class="content_input_fd" size="15" name="lan_netmask"
								value="<% nvram_get_x("LANHostConfig","lan_netmask"); %>" onblur="return validate_ipaddr(this, 'lan_netmask')"
								onkeypress="return is_ipaddr(event, this)" onkeyup="change_ipaddr(this)">
						</td>
					</tr>
				</table>

				<table width="666" border="1" cellpadding="0" cellspacing="0" bordercolor="#B0B0B0">
					<tr bgcolor="#CCCCCC"><td colspan="3"><font face="arial" size="2">&nbsp;</font></td></tr>
					<tr bgcolor="#FFFFFF">
						<td id="Confirm" height="25" width="34%">
							<div style='text-align: center; font-family: "Arial";'>
								<input class="inputSubmit" onmouseout="buttonOut(this)" onmouseover="buttonOver(this)"
									onclick="onSubmitCtrl(this, ' Restore ')" type="submit" value=" Restore " name="action"></div>
						</td>
						<td height="25" width="33%">
							<div style='text-align: center; font-family: "Arial";'>
								<input class="inputSubmit" onmouseout="buttonOut(this)" onmouseover="buttonOver(this)"
									onclick="onSubmitCtrl(this, ' Finish ')" type="submit" value=" Finish " name="action"></div>
						</td>
						<td height="25" width="33%">
							<div style='text-align: center; font-family: "Arial";'>
								<input class="inputSubmit" onmouseout="buttonOut(this)" onmouseover="buttonOver(this)"
									onclick="onSubmitCtrl(this, ' Apply ')" type="submit" value=" Apply " name="action"></div>
						</td>
					</tr>
				</table>

				<table width="666" border="1" cellpadding="0" cellspacing="0" bordercolor="#B0B0B0">
					<tr>
						<td colspan="2" width="616" height="25" bgcolor="#FFBB00">
						</td>
					</tr>
					<tr bgcolor="#FFFFFF">
						<td class="content_header_td_15" align="left">
							Restore:
						</td>
						<td class="content_input_td_padding" align="left">
							Clear the above settings and restore the settings in effect.
						</td>
					</tr>
					<tr bgcolor="#FFFFFF">
						<td class="content_header_td_15" align="left">
							Finish:
						</td>
						<td class="content_input_td_padding" align="left">
							Confirm all settings and restart ZVMODELVZ now.
						</td>
					</tr>
					<tr bgcolor="#FFFFFF">
						<td class="content_header_td_15" align="left">
							Apply:
						</td>
						<td class="content_input_td_padding" align="left">
							Confirm above settings and continue.
						</td>
					</tr>
				</table>
	</form>
</body>
</html>
