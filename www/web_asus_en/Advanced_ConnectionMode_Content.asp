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
	var sCDMA	= 0;
	var sGPRS	= 1;
	var sDialup	= 3;
	var sUser	= 99;

	var usb_connection_types= new Array();
	usb_connection_types[  0 ] = sCDMA;
	usb_connection_types[  1 ] = sGPRS;
	usb_connection_types[  3 ] = sDialup;
	usb_connection_types[ 99 ] = sUser;
	
	function isModeUsed(mode)
	{
		if ( USBConType() == mode )
				return 1;
		else
				return 0;
	}		
	function USBConType()
	{
		var ctype = "";
		var idx = document.form.wan_modem_type.value;
		try{
		    ctype = usb_connection_types [ idx ];
		} catch (e) {
		}
		
		return ctype;
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
		} else if (isModeUsed(sDialup))
		{
			setDispModeById( 'DialupNO',	'block' );
		}
	}
	function setUSBtts( val )
	{
		if(val===''){
			inputCtrl(document.form.wan_modem_tts_port, 1);
		}else{
			inputCtrl(document.form.wan_modem_tts_port, 0);
		}
	}
	function load_this_page()
	{
		setUSBtts(document.form.wan_modem_usbloc.value);
		changeUSBConnectionType();
	}
	window.changeUSBLoc = function (value)
	{
		document.form.wan_modem_usbloc.value = value;
		setUSBtts( value );
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
				<select name="wan_modem_type" class="content_input_fd" onchange="changeUSBConnectionType();window.top.pageChanged = 1;">
					<option class="content_input_fd" value="0" <%nvram_match_x("3GConfig","wan_modem_type",  "0","selected"); %>>
						CDMA/EVDO</option>
					<option class="content_input_fd" value="1" <%nvram_match_x("3GConfig","wan_modem_type", "1","selected"); %>>
						GPRS/EDGE/UMTS/HSPDA</option>
					<option class="content_input_fd" value="3" <%nvram_match_x("3GConfig","wan_modem_type", "3","selected"); %>>
						Dialup</option>
					<option class="content_input_fd" value="99" <%nvram_match_x("3GConfig","wan_modem_type", "99","selected"); %>>
						User defined</option>
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
				<input type="password" maxlength="256" class="content_input_fd" size="32" name="wan_modem_password"
					value="<% nvram_get_x("3GConfig","wan_modem_password"); %>" onkeypress="return is_string(this)"
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
				<input type="radio" value="1" name="wan_modem_demand" class="content_input_fd" onClick="return change_common_radio(this, '3GConfig', 'wan_modem_demand', '1')" <% nvram_match_x("3GConfig","wan_modem_demand", "1", "checked"); %>>Yes
				<input type="radio" value="0" name="wan_modem_demand" class="content_input_fd" onClick="return change_common_radio(this, '3GConfig', 'wan_modem_demand', '0')" <% nvram_match_x("3GConfig","wan_modem_demand", "0", "checked"); %>>No
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
			<td class="content_header_td" onmouseover="return overlib('USB device location in USB tree. Press View button or see Status &amp; Log -> Diagnostic Info -> USB devfs Devices and find the ID in the second column (eg 1.1.0). If blank, use default.')" 
				onmouseout="return nd()"> USB device location ID: </td>
		<td class="content_input_td">
			<input type="text" maxlength="256" class="content_input_fd" size="32" name="wan_modem_usbloc"
				value="<% nvram_get_x("LANHostConfig","wan_modem_usbloc"); %>" onkeypress="return is_string(this)"
				onblur="validate_string(this)"
				onchange="setUSBtts(this.value)">
			<input type="button" value="View" onClick="window.open('devpath_select_form.asp','Select_devpath','width=800,height=200,left=150,top=200,scrollbars=1')"> 
		</td>
		</tr>
		<tr>
			<td class="content_header_td" onmouseover="return overlib('Detect VID:PID every time at usb-modem plugging.')"
				onmouseout="return nd()">
				Autodetect device
			</td>
			<td class="content_input_td" nowrap>
				<input type="radio" value="1" name="wan_modem_autodetect" class="content_input_fd" onClick="return change_common_radio(this, '3GConfig', 'wan_modem_autodetect', '1')" <% nvram_match_x("3GConfig","wan_modem_autodetect", "1", "checked"); %>>Yes
				<input type="radio" value="0" name="wan_modem_autodetect" class="content_input_fd" onClick="return change_common_radio(this, '3GConfig', 'wan_modem_autodetect', '0')" <% nvram_match_x("3GConfig","wan_modem_autodetect", "0", "checked"); %>>No
			</td>
		</tr>
		<tr>
			<td class="content_header_td">
				USB tts(ac) port:
			</td>
			<td class="content_input_td">
				<select name="wan_modem_tts_port" class="content_input_fd" onChange="return change_common(this, '3GConfig', 'wan_modem_tts_port')">
					<option class="content_input_fd" value="0" <%nvram_match_x("3GConfig","wan_modem_tts_port", "0","selected"); %>>
						0</option>
					<option class="content_input_fd" value="1" <%nvram_match_x("3GConfig","wan_modem_tts_port", "1","selected"); %>>
						1</option>
					<option class="content_input_fd" value="2" <%nvram_match_x("3GConfig","wan_modem_tts_port", "2","selected"); %>>
						2</option>
					<option class="content_input_fd" value="3" <%nvram_match_x("3GConfig","wan_modem_tts_port", "3","selected"); %>>
						3</option>
					<option class="content_input_fd" value="4" <%nvram_match_x("3GConfig","wan_modem_tts_port", "4","selected"); %>>
						4</option>
					<option class="content_input_fd" value="5" <%nvram_match_x("3GConfig","wan_modem_tts_port", "5","selected"); %>>
						5</option>
				</select>
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
				USB device Product ID (0xefgh):
			</td>
			<td class="content_input_td">
				<input type="text" maxlength="256" class="content_input_fd" size="32" name="wan_modem_pid"
					value="<% nvram_get_x("3GConfig","wan_modem_pid"); %>" onkeypress="return is_string(this)"
					onblur="validate_string(this)">
			</td>
		</tr>
		<tr>
			<td class="content_header_td">
				USB device packet size (0 for default):
			</td>
			<td class="content_input_td">
				<input type="text" maxlength="256" class="content_input_fd" size="32" name="wan_modem_packetsize"
					value="<% nvram_get_x("3GConfig","wan_modem_packetsize"); %>" onkeypress="return is_number(this)"
					onblur="validate_range(this,0,16384)">
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
				Enable USB-Serial drivers
			</td>
			<td class="content_input_td">
				<input type="radio" value="1" name="wan_modem_serial_enable" class="content_input_fd"
					onClick="return change_common_radio(this, '3GConfig', 'wan_modem_serial_enable', '1')" <% nvram_match_x("3GConfig","wan_modem_serial_enable", "1", "checked"); %>>Yes
				<input type="radio" value="0" name="wan_modem_serial_enable" class="content_input_fd"
					onClick="return change_common_radio(this, '3GConfig', 'wan_modem_serial_enable', '0')" <% nvram_match_x("3GConfig","wan_modem_serial_enable", "0", "checked"); %>>No
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
