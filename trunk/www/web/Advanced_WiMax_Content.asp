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
	<input type="hidden" name="current_page" value="Advanced_WiMax_Content.asp">
	<input type="hidden" name="next_page" value="SaveRestart.asp">
	<input type="hidden" name="next_host" value="">
	<input type="hidden" name="sid_list" value="WiMaxConfig;">
	<input type="hidden" name="group_id" value="">
	<input type="hidden" name="modified" value="0">
	<input type="hidden" name="action_mode" value="">
	<input type="hidden" name="first_time" value="">
	<input type="hidden" name="action_script" value="">
	<input type="hidden" name="wan_proto" value="<% nvram_get("wan_proto"); %>">
		<!-- Table for the conntent page -->
		<table width="666" border="1" cellpadding="0" cellspacing="0" bordercolor="#E0E0E0">
			<tr class="content_header_tr">
				<td class="content_header_td_title" colspan="2">
					USB Network Devices - WiMAX
				</td>
			</tr>
			<tr>
				<td class="content_desc_td" colspan="2">
					The router supports WiMAX connection to WAN.
				</td>
			</tr>
			<tr>
				<td class="content_header_td">
					Set as WAN Connection Type by default
				</td>
				<td class="content_input_td" nowrap>
					<input type="checkbox" value="dhcp" name="wan_proto_x" class="content_input_fd"
				onchange="return change_common_radio(this, 'WiMaxConfig', 'wan_proto', '1')" <% nvram_match("wan_proto", "wimax", "checked"); %>>
				</td>
			</tr>

			<tr>
				<td class="content_header_td" onmouseover="return overlib('This field allows you to provide a SSID for the WiMAX network. Default: @yota.ru', LEFT);"
					onmouseout="return nd();">
					SSID:
				</td>
				<td class="content_input_td">
					<input type="text" maxlength="100" class="content_input_fd" size="32" name="wan_wimax_ssid"
				value="<% nvram_get("wan_wimax_ssid"); %>" onkeypress="return is_string(event, this)"
				onblur="validate_string(this)">
				</td>
			</tr>
			<tr>
				<td class="content_header_td" onmouseover="return overlib('It makes periodically ping to the default gateway of ISP.', LEFT);"
					onmouseout="return nd();">
					Check connection?
				</td>
				<td class="content_input_td">
					<input type="radio" value="1" name="wan_wimax_check" class="content_input_fd" onclick="return change_common_radio(this, 'WiMaxConfig', 'wan_wimax_check', '1')" <% nvram_match("wan_wimax_check", "1", "checked"); %>>Yes
					<input type="radio" value="0" name="wan_wimax_check" class="content_input_fd" onclick="return change_common_radio(this, 'WiMaxConfig', 'wan_wimax_check', '0')" <% nvram_match("wan_wimax_check", "0", "checked"); %>>No
				</td>
			</tr>
			<tr>
				<td class="content_header_td" onmouseover="return overlib('Input the time period between connection checking (in seconds).', LEFT);"
					onmouseout="return nd();">
					Time period, sec:
				</td>
				<td class="content_input_td">
					<input type="text" maxlength="5" class="content_input_fd" size="3" name="wan_wimax_interval"
				value="<% nvram_get("wan_wimax_interval"); %>" onblur="return validate_range(this, 1, 10000)"
				onkeypress="return is_number(event, this)">
				</td>
			</tr>
			<tr>
				<td class="content_header_td" onmouseover="return overlib('Restart WiMAX then connection checking signals fail. It does not work without Check connection.', LEFT);"
					onmouseout="return nd();">
					Restart dead connection?
				</td>
				<td class="content_input_td">
					<input type="radio" value="1" name="wan_wimax_restart" class="content_input_fd" onclick="return change_common_radio(this, 'WiMaxConfig', 'wan_wimax_restart', '1')" <% nvram_match("wan_wimax_restart", "1", "checked"); %>>Yes
					<input type="radio" value="0" name="wan_wimax_restart" class="content_input_fd" onclick="return change_common_radio(this, 'WiMaxConfig', 'wan_wimax_restart', '0')" <% nvram_match("wan_wimax_restart", "0", "checked"); %>>No
				</td>
			</tr>
		</table>

<% include("footer_buttons.inc"); %>

	</form>
</body>
</html>
