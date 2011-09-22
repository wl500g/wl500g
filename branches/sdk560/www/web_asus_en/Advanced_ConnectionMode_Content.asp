<!--<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.01//EN" "http://www.w3.org/TR/html4/strict.dtd">-->
<html>
<head>
	<title>ZVMODELVZ Web Manager</title>
	<meta http-equiv="Content-Type" content="text/html; charset=UTF-8">
	<link rel="stylesheet" type="text/css" href="style.css" media="screen">

	<script type="text/javascript" src="overlib.js"></script>

	<script type="text/javascript" src="general.js"></script>

	<script type="text/javascript" src="quick.js"></script>

	<script type="text/javascript">
	var sCDMA	= "C";
	var sGPRS	= "W";
	var sUser	= "USR";

	function isModeUsed(mode)
	{
		if ( document.form.wan_modem_type.value == mode )
			return 1;
		else
			return 0;
	}

	function setDispModeById( id, mode )
	{
		try{
			var section = document.getElementById( id );
			section.style.display = mode;
		} catch (e){}
	}

	function changeUSBConnectionType()
	{
		setDispModeById( 'APN',		'none' );
		setDispModeById( 'DialupNO',	'none' );

		if (isModeUsed(sGPRS) || isModeUsed(sUser))
		{
			setDispModeById( 'APN',			'block' );
			setDispModeById( 'DialupNO',	'block' );
		}
	}

	function change_autodetect_dev()
	{
		var frm = document.form;
		var flag=0;
		if(get_radio_value(frm.wan_modem_autodetect)=="0") flag=1;
		inputCtrl2(frm.wan_modem_usbloc, flag);
		inputCtrl2(frm.wan_modem_pdata, flag);
		inputCtrl2(frm.wan_modem_pui, flag);
		inputCtrl2(frm.wan_modem_vid, flag);
		inputCtrl2(frm.wan_modem_pid, flag);
	}

	function disable_autodetect()
	{
		inputRCtrl2(document.form.wan_modem_autodetect, 0);
		change_autodetect_dev();
	}

	function load_this_page()
	{
//		setUSBtts(document.form.wan_modem_usbloc.value);
		changeUSBConnectionType();
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
			frm.wan_modem_type.value = value.type;
			changeUSBConnectionType();
			disable_autodetect();
		}
	}
	</script>

	<style type="text/css">
		div.vanishing
		{
			display: none;
		}
	</style>
</head>
<!--<body onload="load_body()" onunload="return unload_body();"> -->
<body onload="load_this_page();" onunload="return unload_body();">
	<div id="overDiv" style="position: absolute; visibility: hidden; z-index: 1000;">
	</div>
	<form method="GET" name="form" action="apply.cgi">
	<input type="hidden" name="current_page" value="Advanced_ConnectionMode_Content.asp">
	<input type="hidden" name="next_page" value="Advanced_ConnectionMode_Content.asp">
	<input type="hidden" name="next_host" value="">
	<input type="hidden" name="sid_list" value="3GConfig;">
	<input type="hidden" name="group_id" value="">
	<input type="hidden" name="modified" value="0">
	<input type="hidden" name="action_mode" value="">
	<input type="hidden" name="first_time" value="">
	<input type="hidden" name="action_script" value="">
	<input type="hidden" name="wan_proto" value="<% nvram_get_x("3GConfig","wan_proto"); %>">
	<!-- Table for the content page -->
	<table class="content_table">
		<tr class="content_header_tr">
			<td class="content_header_td_title" colspan="2">
				USB Network Devices - 3G/CDMA Modem
			</td>
		</tr>
		<tr>
			<td class="content_desc_td" colspan="2">
				ZVMODELVZ supports following connection methods. Please select the mode that match
				your situation.
			</td>
		</tr>
		<tr>
			<td class="content_header_td">
				Set as WAN Connection Type by default
			</td>
			<td class="content_input_td" nowrap>
				<input type="checkbox" value="dhcp" name="wan_proto_x" class="content_input_fd" 
				onchange="return change_common_radio(this, '3GConfig', 'wan_proto', '1')" <% nvram_match_x("3GConfig","wan_proto", "usbmodem", "checked"); %>>
			</td>
		</tr>
		<tr>
			<td class="content_header_td">
				Zero CD Configuration
			</td>
			<td class="content_input_td">
				<select name="wan_modem_zerocd_mode" class="content_input_fd" onChange="return change_common(this, '3GConfig', 'wan_modem_zerocd_mode')">
					<option class="content_input_fd" value="" <%nvram_match_x("3GConfig","wan_modem_zerocd_mode",  "","selected"); %>>
						Not set</option>
					<option class="content_input_fd" value="Auto" <%nvram_match_x("3GConfig","wan_modem_zerocd_mode", "Auto","selected"); %>>
						Auto</option>
					<option class="content_input_fd" value="UserDefined" <%nvram_match_x("3GConfig","wan_modem_zerocd_mode", "UserDefined","selected"); %>>
						Config at /usr/local/etc/usb_modeswitch.conf</option>
				</select>
			</td>
		</tr>
		<tr class="content_header_tr">
			<td class="content_section_header_td" colspan="2">
				Modem options
			</td>
		</tr>
		<tr>
			<td class="content_header_td">
				Modem type:
			</td>
			<td class="content_input_td">
				<select name="wan_modem_type" class="content_input_fd" onchange="changeUSBConnectionType();window.top.pageChanged = 1;disable_autodetect();">
					<option class="content_input_fd" value="USR" <%nvram_match_x("3GConfig","wan_modem_type", "USR","selected"); %>>
						User defined</option>
					<option class="content_input_fd" value="C" <%nvram_match_x("3GConfig","wan_modem_type",  "C","selected"); %>>
						CDMA/EVDO</option>
					<option class="content_input_fd" value="W" <%nvram_match_x("3GConfig","wan_modem_type", "W","selected"); %>>
						GPRS/EDGE/UMTS/HSPDA</option>
				</select>
			</td>
		</tr>
		<tr>
			<td class="content_header_td">
				Username:
			</td>
			<td class="content_input_td">
				<input type="text" maxlength="256" class="content_input_fd" size="32" name="wan_modem_username"
					value="<% nvram_get_x("3GConfig","wan_modem_username"); %>" onkeypress="return is_string(this)"
					onblur="validate_string(this)">
			</td>
		</tr>
		<tr>
			<td class="content_header_td">
				Password:
			</td>
			<td class="content_input_td">
				<input type="password" maxlength="256" class="content_input_fd" size="32" name="wan_modem_passwd"
					value="<% nvram_get_x("3GConfig","wan_modem_passwd"); %>" onkeypress="return is_string(this)"
					onblur="validate_string(this)">
			</td>
		</tr>
	</table>
	<div id='APN' class='vanishing'>
		<table class="content_table">
			<tr>
				<td class="content_header_td">
					APN:
				</td>
				<td class="content_input_td">
					<input type="text" maxlength="256" class="content_input_fd" size="32" name="wan_modem_apn"
						value="<% nvram_get_x("3GConfig","wan_modem_apn"); %>" onkeypress="return is_string(this)"
						onblur="validate_string(this)">
				</td>
			</tr>
		</table>
	</div>
	<div id='DialupNO' class='vanishing'>
		<table class="content_table">
			<tr>
				<td class="content_header_td">
					Dial Number (usually *99***1# or *99#):
				</td>
				<td class="content_input_td">
					<input type="text" maxlength="256" class="content_input_fd" size="32" name="wan_modem_dialno"
						value="<% nvram_get_x("3GConfig","wan_modem_dialno"); %>" onkeypress="return is_string(this)"
						onblur="validate_string(this)">
				</td>
			</tr>
		</table>
	</div>
	<table class="content_table">
		<tr>
			<td class="content_header_td">
				Call on Demand
			</td>
			<td class="content_input_td">
				<input type="radio" value="0" name="wan_modem_demand" class="content_input_fd" onClick="return change_common_radio(this, '3GConfig', 'wan_modem_demand', '0')" <% nvram_match_x("3GConfig","wan_modem_demand", "0", "checked"); %>>No
				<input type="radio" value="1" name="wan_modem_demand" class="content_input_fd" onClick="return change_common_radio(this, '3GConfig', 'wan_modem_demand', '1')" <% nvram_match_x("3GConfig","wan_modem_demand", "1", "checked"); %>>Yes
			</td>
		</tr>
		<tr>
			<td class="content_header_td" onmouseover="return overlib('This field allows you to configure to terminate your ISP connection after a specified period of time.', LEFT);"
				onmouseout="return nd();">
				Idle time before disconnect (sec):
			</td>
			<td class="content_input_td">
				<input type="text" maxlength="32" class="content_input_fd" size="32" name="wan_modem_idle"
					value="<% nvram_get_x("3GConfig","wan_modem_idle"); %>" onkeypress="return is_number(this)"
					onblur="validate_string(this)">
			</td>
		</tr>
		<tr>
			<td class="content_header_td" onmouseover="return overlib('That is Maximum Transmission Unit(MTU) of PPP packet.', LEFT);"
				onmouseout="return nd()">
				MTU
			</td>
			<td class="content_input_td">
				<input type="text" maxlength="32" class="content_input_fd" size="32" name="wan_modem_mtu"
					value="<% nvram_get_x("3GConfig","wan_modem_mtu"); %>" onkeypress="return is_number(this)"
					onblur="validate_string(this)">
			</td>
		</tr>
		<tr>
			<td class="content_header_td" onmouseover="return overlib('That is Maximum Receive Unit(MRU) of PPP packet.', LEFT);"
				onmouseout="return nd();">
				MRU
			</td>
			<td class="content_input_td">
				<input type="text" maxlength="32" class="content_input_fd" size="32" name="wan_modem_mru"
					value="<% nvram_get_x("3GConfig","wan_modem_mru"); %>" onkeypress="return is_number(this)"
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
				<input type="radio" value="0" name="wan_modem_autodetect" class="content_input_fd" onClick="return change_autodetect_dev()" <% nvram_match_x("3GConfig","wan_modem_autodetect", "0", "checked"); %> >No
				<input type="radio" value="1" name="wan_modem_autodetect" class="content_input_fd" onClick="return change_autodetect_dev()" <% nvram_match_x("3GConfig","wan_modem_autodetect", "1", "checked"); %> >Yes
			</td>
		</tr>

		<tr>
			<td class="content_header_td" onmouseover="return overlib('Select parameters for connected USB modem')" 
				onmouseout="return nd()"> View connected modems</td>
		<td class="content_input_td">
			<input type="button" value="View" onClick="window.open('devpath_select_form.asp','Select_devpath','width=800,height=200,left=150,top=200,scrollbars=1')"> 
		</td>
		</tr>

		<tr>
			<td class="content_header_td" onmouseover="return overlib('USB device location in USB tree. Press View button or see Status &amp; Log -> Diagnostic Info -> USB devfs Devices and find the ID in the second column (eg 1.1.0). If blank, use default.')" 
				onmouseout="return nd()"> USB device location ID: </td>
		<td class="content_input_td">
			<input type="text" maxlength="256" class="content_input_fd" size="32" name="wan_modem_usbloc"
				value="<% nvram_get_x("LANHostConfig","wan_modem_usbloc"); %>" onkeypress="return is_string(this)"
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
					value="<% nvram_get_x("3GConfig","wan_modem_pdata"); %>" onkeypress="return is_string(this)"
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
					value="<% nvram_get_x("3GConfig","wan_modem_pui"); %>" onkeypress="return is_string(this)"
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
					value="<% nvram_get_x("3GConfig","wan_modem_vid"); %>" onkeypress="return is_string(this)"
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
					value="<% nvram_get_x("3GConfig","wan_modem_pid"); %>" onkeypress="return is_string(this)"
					onblur="validate_string(this)">
			</td>
		</tr>

		<tr>
			<td class="content_header_td">
				USB device serial speed (usually 921600):
			</td>
			<td class="content_input_td">
				<input type="text" maxlength="256" class="content_input_fd" size="32" name="wan_modem_portspeed"
					value="<% nvram_get_x("3GConfig","wan_modem_portspeed"); %>" onkeypress="return is_number(this)"
					onblur="validate_range(this, 0, 921600)">
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
					value="<% nvram_get_x("3GConfig","wan_modem_init_script"); %>" onkeypress="return is_string(this)"
					onblur="validate_string(this)">
			</td>
		</tr>
		<tr>
			<td class="content_header_td">
				Additional AT commands:
			</td>
			<td class="content_input_td">
				<input type="text" maxlength="256" class="content_input_fd" size="32" name="wan_modem_dialup_init"
					value="<% nvram_get_x("3GConfig","wan_modem_dialup_init"); %>" onkeypress="return is_string(this)"
					onblur="validate_string(this)">
			</td>
		</tr>
		<tr>
			<td class="content_header_td" onmouseover="return overlib('This item may be specified by some ISPs. Check with your ISP and fill them in if required.', LEFT);"
				onmouseout="return nd();">
				Additional pppd options:
			</td>
			<td class="content_input_td">
				<input type="text" maxlength="256" class="content_input_fd" size="32" name="wan_modem_options"
					value="<% nvram_get_x("3GConfig","wan_modem_options"); %>" onkeypress="return is_string(this)"
					onblur="validate_string(this)">
			</td>
		</tr>
		<tr>
			<td class="content_header_td">
				Failure event script name:
			</td>
			<td class="content_input_td">
				<input type="text" maxlength="256" class="content_input_fd" size="32" name="wan_modem_onfailure"
					value="<% nvram_get_x("3GConfig","wan_modem_onfailure"); %>" onkeypress="return is_string(this)"
					onblur="validate_string(this)">
			</td>
		</tr>
		<tr>
			<td class="content_header_td" onmouseover="return overlib('If this parameter is not zero, then Failure event script will be started after max number of reconnections.', LEFT);"
				onmouseout="return nd();">
				Max number of reconnections (0 for unlimit):
			</td>
			<td class="content_input_td">
				<input type="text" maxlength="10" class="content_input_fd" size="32" name="wan_modem_maxfail"
					value="<% nvram_get_x("3GConfig","wan_modem_maxfail"); %>" onkeypress="return is_number(this)">
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
	<!-- ********************************************************************** -->
	<table class="content_table">
		<tr bgcolor="#CCCCCC">
			<td colspan="3" style="height: 15px;">
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
	</form>
</body>
</html>
