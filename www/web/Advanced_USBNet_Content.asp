<!--<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.01//EN" "http://www.w3.org/TR/html4/strict.dtd">-->
<html>
<head>
	<title><% model(2); %></title>
	<meta http-equiv="Content-Type" content="text/html; charset=UTF-8">
	<link rel="stylesheet" type="text/css" href="style.css" media="screen">

	<script type="text/javascript" src="overlib.js"></script>

	<script type="text/javascript" src="general.js"></script>

	<script type="text/javascript" src="quick.js"></script>

	<script type="text/javascript">

	function setDispModeById( id, mode )
	{
		try{
			var section = document.getElementById( id );
			section.style.display = mode;
		} catch (e){}
	}

	function change_subtype()
	{
		var frm = document.form;
		var flag = (frm.wan_usbnet_subtype.value != "") ? 1 : 0;
		inputCtrl2(frm.wan_modem_usbloc, flag);
		inputCtrl2(frm.wan_modem_pdata, flag);
		inputCtrl2(frm.wan_modem_pui, flag);
		inputCtrl2(frm.wan_modem_vid, flag);
		inputCtrl2(frm.wan_modem_pid, flag);

		flag = (frm.wan_usbnet_subtype.value == "user") &&
			(get_radio_value(frm.wan_modem_autodetect)=="0")? 1 : 0;
		inputCtrl2(frm.wan_modem_at_connect, flag);
		inputCtrl2(frm.wan_modem_at_disconnect, flag);
	}


	function change_autodetect_dev()
	{
		var frm = document.form;
		var flag = (get_radio_value(frm.wan_modem_autodetect)=="0") ? 1 : 0;

		if (flag == 0) change_subtype(); // autodetect switched on

		inputCtrl2(frm.wan_modem_usbloc, flag);
		inputCtrl2(frm.wan_usbnet_subtype, flag);
		inputCtrl2(frm.wan_modem_pdata, flag);
		inputCtrl2(frm.wan_modem_pui, flag);
		inputCtrl2(frm.wan_modem_vid, flag);
		inputCtrl2(frm.wan_modem_pid, flag);

		if (flag == 1) change_subtype(); // autodetect switched off
	}

	function disable_autodetect()
	{
		inputRCtrl2(document.form.wan_modem_autodetect, 0);
		change_autodetect_dev();
	}

	function load_this_page()
	{
//		setUSBtts(document.form.wan_modem_usbloc.value);
//		changeUSBConnectionType();
		change_autodetect_dev();
	}
	window.changeUSBparams = function (value)
	{
		//value is associated array
		var frm=document.form;
		if(value){
			frm.wan_modem_usbloc.value = value.loc;
			frm.wan_modem_vid.value = "0x"+value.vid;
			frm.wan_modem_pid.value = "0x"+value.pid;
			frm.wan_modem_pdata.value = value.data;
			frm.wan_modem_pui.value = value.ui;
			disable_autodetect();
		}
	}
	</script>

	<style type="text/css">
		div.vanishing { display: none; }
	</style>
</head>
<!--<body onload="load_body()" onunload="return unload_body();"> -->
<body onload="load_this_page();" onunload="return unload_body();">
	<div id="overDiv" style="position: absolute; visibility: hidden; z-index: 1000;">
	</div>
	<form method="GET" name="form" action="apply.cgi">
	<input type="hidden" name="current_page" value="Advanced_USBNet_Content.asp">
	<input type="hidden" name="next_page" value="Advanced_USBNet_Content.asp">
	<input type="hidden" name="next_host" value="">
	<input type="hidden" name="sid_list" value="3GConfig;">
	<input type="hidden" name="group_id" value="">
	<input type="hidden" name="modified" value="0">
	<input type="hidden" name="action_mode" value="">
	<input type="hidden" name="first_time" value="">
	<input type="hidden" name="action_script" value="">
	<input type="hidden" name="wan_proto" value="<% nvram_get("wan_proto"); %>">
	<!-- Table for the content page -->
	<table class="content_table">
		<tr class="content_header_tr">
			<td class="content_header_td_title" colspan="2">
				USB Network Devices - Ethernet over USB
			</td>
		</tr>
		<tr>
			<td class="content_desc_td" colspan="2">
				The router supports following connection methods. Please select the mode that match
				your situation.
			</td>
		</tr>
		<tr>
			<td class="content_header_td">
				Set as WAN Connection Type by default
			</td>
			<td class="content_input_td" nowrap>
				<input type="checkbox" value="dhcp" name="wan_proto_x" class="content_input_fd" 
				onchange="return change_common_radio(this, 'usbnetConfig', 'wan_proto', '1')" <% nvram_match("wan_proto", "usbnet", "checked"); %>>
			</td>
		</tr>
		<tr>
			<td class="content_header_td">
				Zero CD Configuration
			</td>
			<td class="content_input_td">
				<select name="wan_modem_zerocd_mode" class="content_input_fd" onChange="return change_common(this, '3GConfig', 'wan_modem_zerocd_mode')">
					<option class="content_input_fd" value="" <%nvram_match("wan_modem_zerocd_mode",  "","selected"); %>>
						Not set</option>
					<option class="content_input_fd" value="Auto" <%nvram_match("wan_modem_zerocd_mode", "Auto","selected"); %>>
						Auto</option>
					<option class="content_input_fd" value="UserDefined" <%nvram_match("wan_modem_zerocd_mode", "UserDefined","selected"); %>>
						Config at /usr/local/etc/usb_modeswitch.conf</option>
				</select>
			</td>
		</tr>
		<tr class="content_header_tr">
			<td class="content_section_header_td" colspan="2">
				Network options
			</td>
		</tr>
		<tr>
			<td class="content_header_td">
				APN:
			</td>
			<td class="content_input_td">
				<input type="text" maxlength="256" class="content_input_fd" size="32" name="wan_modem_apn"
					value="<% nvram_get("wan_modem_apn"); %>" onkeypress="return is_string(event, this)"
					onblur="validate_string(this)">
			</td>
		</tr>

<!-- ******************* Not realized yet ********************  -->
		<tr style="display:none">
			<td class="content_header_td">
				Username:
			</td>
			<td class="content_input_td">
				<input type="text" maxlength="256" class="content_input_fd" size="32" name="wan_modem_username"
					value="<% nvram_get("wan_modem_username"); %>" onkeypress="return is_string(event, this)"
					onblur="validate_string(this)">
			</td>
		</tr>
		<tr style="display:none">
			<td class="content_header_td">
				Password:
			</td>
			<td class="content_input_td">
				<input type="password" maxlength="256" class="content_input_fd" size="32" name="wan_modem_passwd"
					value="<% nvram_get("wan_modem_passwd"); %>" onkeypress="return is_string(event, this)"
					onblur="validate_string(this)">
			</td>
		</tr>
<!-- ******************* End of not realized  ********************  -->
		<tr>
			<td class="content_header_td" onmouseover="return overlib('That is Maximum Transmission Unit(MTU). 0 is default value for automatic MTU detection', LEFT);"
				onmouseout="return nd()">
				MTU
			</td>
			<td class="content_input_td">
				<input type="text" maxlength="32" class="content_input_fd" size="32" name="wan_usbnet_mtu"
					value="<% nvram_get("wan_usbnet_mtu"); %>" onkeypress="return is_number(event, this)"
					onblur="validate_string(this)">
			</td>
		</tr>
		<tr class="content_header_tr">
			<td class="content_section_header_td" colspan="2">
				Custom USB device parameters
			</td>
		</tr>
		<tr>
			<td class="content_header_td" onmouseover="return overlib('Detect VID:PID every time at usb-modem plugging.')"
				onmouseout="return nd()">
				Autodetect device
			</td>
			<td class="content_input_td" nowrap>
				<input type="radio" value="0" name="wan_modem_autodetect" class="content_input_fd" onClick="return change_autodetect_dev()" <% nvram_match("wan_modem_autodetect", "0", "checked"); %> >No
				<input type="radio" value="1" name="wan_modem_autodetect" class="content_input_fd" onClick="return change_autodetect_dev()" <% nvram_match("wan_modem_autodetect", "1", "checked"); %> >Yes
			</td>
		</tr>

		<tr>
			<td class="content_header_td" onmouseover="return overlib('Select parameters for connected USB modem')" 
				onmouseout="return nd()">View connected modems</td>
		<td class="content_input_td">
			<input type="button" value="View" onClick="window.open('devpath_select_form.asp','Select_devpath','width=800,height=200,left=150,top=200,scrollbars=1')"> 
		</td>
		</tr>

		<tr>
			<td class="content_header_td">
				Device control type:
			</td>
			<td class="content_input_td">
				<select name="wan_usbnet_subtype" class="content_input_fd" onchange="window.top.pageChanged = 1; change_subtype();">
					<option class="content_input_fd" value="" <%nvram_match("wan_usbnet_subtype", "","selected"); %>>
						USB net only</option>
					<option class="content_input_fd" value="qmi" <%nvram_match("wan_usbnet_subtype",  "qmi","selected"); %>>
						QMI</option>
					<option class="content_input_fd" value="ncm" <%nvram_match("wan_usbnet_subtype", "ncm","selected"); %>>
						NCM</option>
					<option class="content_input_fd" value="user" <%nvram_match("wan_usbnet_subtype", "user","selected"); %>>
						User defined AT</option>
				</select>
			</td>
		</tr>

		<tr>
			<td class="content_header_td" onmouseover="return overlib('USB device location in USB tree. Press View button or see Status &amp; Log -> Diagnostic Info -> USB devfs Devices and find the ID in the second column (eg 1.1.0). If blank, use default.')" 
				onmouseout="return nd()"> USB device location ID: </td>
		<td class="content_input_td">
			<input type="text" maxlength="256" class="content_input_fd" size="32" name="wan_modem_usbloc"
				value="<% nvram_get("wan_modem_usbloc"); %>" onkeypress="return is_string(event, this)"
				onblur="validate_string(this)">
		</td>
		</tr>

		<tr>
			<td class="content_header_td" onmouseover="return overlib('This field allows you to specify data port manually. If Autodetect device option is enabled, the router will define it automatically.')"
				onmouseout="return nd()">
				Data port:
			</td>
			<td class="content_input_td">
				<input type="text" maxlength="256" class="content_input_fd" size="32" name="wan_modem_pdata"
					value="<% nvram_get("wan_modem_pdata"); %>" onkeypress="return is_string(event, this)"
					onblur="validate_string(this)">
			</td>
		</tr>
		<tr>
			<td class="content_header_td" onmouseover="return overlib('This field allows you to specify user interface port manually. If Autodetect device option is enabled, the router will define it automatically.')"
				onmouseout="return nd()">
				User interface port:
			</td>
			<td class="content_input_td">
				<input type="text" maxlength="256" class="content_input_fd" size="32" name="wan_modem_pui"
					value="<% nvram_get("wan_modem_pui"); %>" onkeypress="return is_string(event, this)"
					onblur="validate_string(this)">
			</td>
		</tr>

		<tr>
			<td class="content_header_td" onmouseover="return overlib('This field allows you to specify USB vendor ID manually. If Autodetect device option is enabled, the router will define it automatically.')"
				onmouseout="return nd()">
				USB device Vendor ID (0xabcd):
			</td>
			<td class="content_input_td">
				<input type="text" maxlength="256" class="content_input_fd" size="32" name="wan_modem_vid"
					value="<% nvram_get("wan_modem_vid"); %>" onkeypress="return is_string(event, this)"
					onblur="validate_string(this)">
			</td>
		</tr>
		<tr>
			<td class="content_header_td" onmouseover="return overlib('This field allows you to specify USB product ID manually. If Autodetect device option is enabled, the router will define it automatically.')"
				onmouseout="return nd()">
				USB device Product ID (0xef01):
			</td>
			<td class="content_input_td">
				<input type="text" maxlength="256" class="content_input_fd" size="32" name="wan_modem_pid"
					value="<% nvram_get("wan_modem_pid"); %>" onkeypress="return is_string(event, this)"
					onblur="validate_string(this)">
			</td>
		</tr>

		<tr class="content_header_tr">
			<td class="content_section_header_td" colspan="2">
				Additional parameters
			</td>
		</tr>
		<tr>
			<td class="content_header_td">
				Device initial script:
			</td>
			<td class="content_input_td">
				<input type="text" maxlength="256" class="content_input_fd" size="32" name="wan_modem_init_script"
					value="<% nvram_get("wan_modem_init_script"); %>" onkeypress="return is_string(event, this)"
					onblur="validate_string(this)">
			</td>
		</tr>

		<tr>
			<td class="content_header_td">
				AT command to connect:
			</td>
			<td class="content_input_td">
				<input type="text" maxlength="256" class="content_input_fd" size="32" name="wan_modem_at_connect"
					value="<% nvram_get("wan_modem_at_connect"); %>" onkeypress="return is_string(event, this)"
					onblur="validate_string(this)">
			</td>
		</tr>
		<tr>
			<td class="content_header_td">
				AT command to disconnect:
			</td>
			<td class="content_input_td">
				<input type="text" maxlength="256" class="content_input_fd" size="32" name="wan_modem_at_disconnect"
					value="<% nvram_get("wan_modem_at_disconnect"); %>" onkeypress="return is_string(event, this)"
					onblur="validate_string(this)">
			</td>
		</tr>

		<tr>
			<td class="content_header_td_less">
				Action:
			</td>
			<td class="content_input_td">
				<input type="submit" class="content_input_fd_ro" onclick="return onSubmitApply('dhcpc_release')"
					name="PPPConnection_x_WANAction_button" value="Disconnect">
				<input type="submit" class="content_input_fd_ro" onclick="return onSubmitApply('dhcpc_renew')"
					name="PPPConnection_x_WANAction_button1" value="Connect">
			</td>
		</tr>
	</table>

	<% include("footer_buttons.inc"); %>

	</form>
</body>
</html>
