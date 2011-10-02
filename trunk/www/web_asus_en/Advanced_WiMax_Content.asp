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
	<input type="hidden" name="current_page" value="Advanced_WiMax_Content.asp">
	<input type="hidden" name="next_page" value="SaveRestart.asp">
	<input type="hidden" name="next_host" value="">
	<input type="hidden" name="sid_list" value="WiMaxConfig;">
	<input type="hidden" name="group_id" value="">
	<input type="hidden" name="modified" value="0">
	<input type="hidden" name="action_mode" value="">
	<input type="hidden" name="first_time" value="">
	<input type="hidden" name="action_script" value="">
	<input type="hidden" name="wan_proto" value="<% nvram_get_x("WiMaxConfig","wan_proto"); %>">
		<!-- Table for the conntent page -->
		<table width="666" border="1" cellpadding="0" cellspacing="0" bordercolor="#E0E0E0">
			<tr class="content_header_tr">
				<td class="content_header_td_title" colspan="2">
					USB Network Devices - WiMAX
				</td>
			</tr>
			<tr>
				<td class="content_desc_td" colspan="2">
					ZVMODELVZ supports WiMAX connection to WAN.
				</td>
			</tr>
			<tr>
				<td class="content_header_td">
					Set as WAN Connection Type by default
				</td>
				<td class="content_input_td" nowrap>
					<input type="checkbox" value="dhcp" name="wan_proto_x" class="content_input_fd"
				onchange="return change_common_radio(this, 'WiMaxConfig', 'wan_proto', '1')" <% nvram_match_x("WiMaxConfig","wan_proto", "wimax", "checked"); %>>
				</td>
			</tr>

			<tr>
				<td class="content_header_td" onmouseover="return overlib('This field allows you to provide a SSID for the WiMAX network. Default: @yota.ru', LEFT);"
					onmouseout="return nd();">
					SSID:
				</td>
				<td class="content_input_td">
					<input type="text" maxlength="100" class="content_input_fd" size="32" name="wan_wimax_ssid"
				value="<% nvram_get_x("WiMaxConfig","wan_wimax_ssid"); %>" onkeypress="return is_string(event, this)"
				onblur="validate_string(this)">
				</td>
			</tr>
			<tr>
				<td class="content_header_td" onmouseover="return overlib('It makes periodically ping to the default gateway of ISP.', LEFT);"
					onmouseout="return nd();">
					Check connection?
				</td>
				<td class="content_input_td">
					<input type="radio" value="1" name="wan_wimax_check" class="content_input_fd" onclick="return change_common_radio(this, 'WiMaxConfig', 'wan_wimax_check', '1')" <% nvram_match_x("WiMaxConfig", "wan_wimax_check", "1", "checked"); %>>Yes
					<input type="radio" value="0" name="wan_wimax_check" class="content_input_fd" onclick="return change_common_radio(this, 'WiMaxConfig', 'wan_wimax_check', '0')" <% nvram_match_x("WiMaxConfig", "wan_wimax_check", "0", "checked"); %>>No
				</td>
			</tr>
			<tr>
				<td class="content_header_td" onmouseover="return overlib('Input the time period between connection checking (in seconds).', LEFT);"
					onmouseout="return nd();">
					Time period, sec:
				</td>
				<td class="content_input_td">
					<input type="text" maxlength="5" class="content_input_fd" size="3" name="wan_wimax_interval"
				value="<% nvram_get_x("WiMaxConfig","wan_wimax_interval"); %>" onblur="return validate_range(this, 1, 10000)"
				onkeypress="return is_number(event, this)">
				</td>
			</tr>
			<tr>
				<td class="content_header_td" onmouseover="return overlib('Restart WiMAX then connection checking signals fail. It does not work without Check connection.', LEFT);"
					onmouseout="return nd();">
					Restart dead connection?
				</td>
				<td class="content_input_td">
					<input type="radio" value="1" name="wan_wimax_restart" class="content_input_fd" onclick="return change_common_radio(this, 'WiMaxConfig', 'wan_wimax_restart', '1')" <% nvram_match_x("WiMaxConfig", "wan_wimax_restart", "1", "checked"); %>>Yes
					<input type="radio" value="0" name="wan_wimax_restart" class="content_input_fd" onclick="return change_common_radio(this, 'WiMaxConfig', 'wan_wimax_restart', '0')" <% nvram_match_x("WiMaxConfig", "wan_wimax_restart", "0", "checked"); %>>No
				</td>
			</tr>
		</table>

		<table width="666" border="1" cellpadding="0" cellspacing="0" bordercolor="#B0B0B0">
			<tr bgcolor="#CCCCCC">
				<td colspan="3" style='font-family: "Arial"; font-size: 2pt;'>
					&nbsp;
				</td>
			</tr>
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
