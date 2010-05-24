<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.01//EN" "http://www.w3.org/TR/html4/strict.dtd">
<html>
<head>
	<title>ZVMODELVZ Web Manager</title>
	<meta http-equiv="Content-Type" content="text/html; charset=UTF-8">
	<link rel="stylesheet" type="text/css" href="style.css" media="screen">

	<script type="text/javascript" src="overlib.js"></script>

	<script type="text/javascript" src="general.js"></script>

	<script type="text/javascript" src="quick.js"></script>

	<script language="JavaScript" type="text/javascript">
	var sCDMA	= 'CDMA';
	var sGPRS	= 'GPRS';
	var sDialup	= 'Dial-Up';
	var sIPW	= 'IPW';
	var sZada	= 'ZadaCOM';
	var sUser	= 'User';
	var usb_connection_types= new Array();
	usb_connection_types[ -1 ] = "none";
	usb_connection_types[  1 ] = sCDMA;
	usb_connection_types[  2 ] = sGPRS;
	usb_connection_types[  3 ] = sDialup;
	usb_connection_types[  4 ] = sIPW;
	usb_connection_types[  5 ] = sZada;
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
		var idx = get_radio_value ( document.form.wan_modem_mode_x );
		try{
		    ctype = usb_connection_types [ idx ]; //"<% nvram_get_x("LANHostConfig","cdma_type"); %>";
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
		setDispModeById( sCDMA,		'none' );
		setDispModeById( sGPRS,		'none' );
		setDispModeById( sZada,		'none' );
		setDispModeById( sDialup,	'none' );
		setDispModeById( sIPW,		'none' );
		setDispModeById( sUser,		'none' );
		
		setDispModeById( 'UserPasswd',	'none' );
		setDispModeById( 'APN',			'none' );
		setDispModeById( 'DialupNO',	'none' );
		setDispModeById( 'USB',			'none' );
		setDispModeById( 'PortSpeed',	'none' );
		setDispModeById( 'ZeroCD',		'none' );
		
		if (isModeUsed( sCDMA ))
		{
			setDispModeById( sCDMA,	'block' );
			setDispModeById( 'UserPasswd',	'block' );
			setDispModeById( 'USB',	'block' );			
	//		appendChild(aux1,generateDocEntry(0, "CDMA Config","Advanced_CDMA_Content.asp", ""))
	//		if (CDMAType() == 'GTRAN')
	//			appendChild(aux1,generateDocEntry(0, "GTRAN Info", "Main_CDMAGStatus_Content.asp", ""))
	//		if (CDMAType() == 'AnyDATA')
	//		   appendChild(aux1,generateDocEntry(0, "AnyDATA Info", "Main_CDMAADStatus_Content.asp", ""))
	//		/if (CDMAType() == 'Axesstel')
	//		   appendChild(aux1,generateDocEntry(0, "Axesstel Info", "Main_CDMAAxStatus_Content.asp", ""))
		} else if (isModeUsed(sIPW))
		{
			setDispModeById( sIPW, 'block' );
			setDispModeById( 'UserPasswd',	'block' );
			setDispModeById( 'APN',	'block' );
			setDispModeById( 'USB',	'block' );
	//	  appendChild(aux1,generateDocEntry(0, "IP Wireless 4G Config","Advanced_IPW_Content.asp", ""))
	//	  appendChild(aux1,generateDocEntry(0, "IP Wireless 4G Info", "Main_IPWStatus_Content.asp", ""))
		} else if (isModeUsed(sGPRS))
		{
			setDispModeById( sGPRS,			'block' );
			setDispModeById( 'UserPasswd',	'block' );
			setDispModeById( 'APN',			'block' );
			setDispModeById( 'DialupNO',	'block' );
			setDispModeById( 'PortSpeed',	'block' );
			setDispModeById( 'USB',			'block' );
			setDispModeById( 'ZeroCD',		'block' );
	//		appendChild(aux1,generateDocEntry(0, "GPRS/EDGE/UMTS Config","Advanced_GPRS_Content.asp", ""))
		} else if (isModeUsed(sZada))
		{
			setDispModeById( sZada,			'block' );
			setDispModeById( 'UserPasswd',	'block' );
			setDispModeById( 'APN',	'block' );
			setDispModeById( 'DialupNO',	'block' );
			setDispModeById( 'USB',	'block' );

	//	   	  appendChild(aux1,generateDocEntry(0, "ZadaCOM Config","Advanced_ZadaCOM_Content.asp", ""))
		} else if (isModeUsed(sDialup))
		{
			setDispModeById( sDialup, 'block' );
			setDispModeById( 'DialupNO',	'block' );
			setDispModeById( 'PortSpeed',	'block' );
			setDispModeById( 'USB',	'block' );
	//	   	  appendChild(aux1,generateDocEntry(0, "Dial-Up Config","Advanced_Dialup_Content.asp", ""))
		} else if (isModeUsed(sUser))
		{
			setDispModeById( sUser, 'block' );
		
	//		appendChild(aux1,generateDocEntry(0, "User Dial-Up Config","Advanced_DialupUser_Content.asp", ""))
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
<!--<body onload="load_body()" onunload="return unload_body();">-->
	<div id="overDiv" style="position: absolute; visibility: hidden; z-index: 1000;">
	</div>
<body onload="changeUSBConnectionType();" onunload="return unload_body();">
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
	<input type="hidden" name="wan_proto" value="<% nvram_get_x("","wan_proto"); %>">
	<input type="hidden" name="dial_mode_org" value="<% nvram_get_x("","wan_modem_mode_x"); %>">
	<!-- Table for the content page -->
	<table class="content_table">
		<tr class="content_header_tr">
			<td class="content_header_td_title" colspan="2">
				USB Connection - PPP Connection Mode
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
				Enable automatic dial on boot
			</td>
			<td class="content_input_td" nowrap>
				<input type="radio" value="1" name="wan_modem_enable_x" class="content_input_fd" onchange="change_common(this, '', 'wan_modem_enable_x')"
					<% nvram_match_x("","wan_modem_enable_x", "1", "checked"); %>>Yes
				<input type="radio" value="0" name="wan_modem_enable_x" class="content_input_fd" onchange="change_common(this, '', 'wan_modem_enable_x')"
					<% nvram_match_x("","wan_modem_enable_x", "0", "checked"); %>>No
			</td>
		</tr>
		<tr>
			<td class="content_header_td">
				Use WAN port as LAN
			</td>
			<td class="content_input_td" nowrap>
				<input type="radio" value="1" name="wan_modem_wanaslan_x" class="content_input_fd" <% nvram_match_x("","wan_modem_wanaslan_x", "1", "checked"); %>>Yes
				<input type="radio" value="0" name="wan_modem_wanaslan_x" class="content_input_fd" <% nvram_match_x("","wan_modem_wanaslan_x", "0", "checked"); %>>No
			</td>
		</tr>
		<tr>
			<td class="content_header_td_30" onmouseover="return overlib('No USB modem connection', LEFT);"
				onmouseout="return nd();">
				<input type="radio" value="-1" name="wan_modem_mode_x" onclick="changeUSBConnectionType();"
					<% nvram_match_x("","wan_modem_mode_x", "-1", "checked"); %>>
				None
			</td>
			<td class="content_desc_td">
				No USB modem connection.
			</td>
		</tr>
		<tr>
			<td class="content_header_td_30" onmouseover="return overlib('Set CDMA connection via GTRAN GPC-6420 or AnyDATA ADU E100H or ADU 510L or Axesstel MV110H modem.', LEFT);"
				onmouseout="return nd();">
				<input type="radio" value="1" name="wan_modem_mode_x" onclick="changeUSBConnectionType();"
					<% nvram_match_x("","wan_modem_mode_x", "1", "checked"); %>>
				CDMA
			</td>
			<td class="content_desc_td">
				CDMA connection via GTRAN GPC-6420 or AnyDATA ADU E100H or ADU 510L or Axesstel
				MV110H modem.
			</td>
		</tr>
<!--		<tr>
			<td class="content_header_td_30" onmouseover="return overlib('Set IP Wireless 4G connection via IP Wireless USB modem.', LEFT);"
				onmouseout="return nd();">
				<input type="radio" value="4" name="wan_modem_mode_x" onclick="changeUSBConnectionType();"
					<% nvram_match_x("","wan_modem_mode_x", "4", "checked"); %>>
				IP Wireless 4G
			</td>
			<td class="content_desc_td">
				IP Wireless 4G connection via IP Wireless USB modem.
			</td>
		</tr>-->
		<tr>
			<td class="content_header_td_30" onmouseover="return overlib('Set GPRS/EDGE/UMTS connection via USB connected GSM USB-serial converter + GSM.', LEFT);"
				onmouseout="return nd();">
				<input type="radio" value="2" name="wan_modem_mode_x" onclick="changeUSBConnectionType();"
					<% nvram_match_x("","wan_modem_mode_x", "2", "checked"); %>>
				GPRS/EDGE/UMTS
			</td>
			<td class="content_desc_td">
				GPRS/EDGE/UMTS connection via USB connected GSM or
				<br>
				GPRS/EDGE/UMTS connection via USB-serial converter and GSM.
			</td>
		</tr>
<!--		<tr>
			<td class="content_header_td_30" onmouseover="return overlib('Set internet connection via USB connected ZadaCOM USB modem.', LEFT);"
				onmouseout="return nd();">
				<input type="radio" value="5" name="wan_modem_mode_x" onclick="changeUSBConnectionType();"
					<% nvram_match_x("","wan_modem_mode_x", "5", "checked"); %>>
				ZadaCOM
			</td>
			<td class="content_desc_td">
				Internet connection via ZadaCOM USB connected modem.
			</td>
		</tr>-->
		<tr>
			<td class="content_header_td_30" onmouseover="return overlib('Set Dial-Up connection via USB modem or USB-serial converter and modem.', LEFT);"
				onmouseout="return nd();">
				<input type="radio" value="3" name="wan_modem_mode_x" onclick="changeUSBConnectionType();"
					<% nvram_match_x("","wan_modem_mode_x", "3", "checked"); %>>
				Dial-Up
			</td>
			<td class="content_desc_td">
				Dial-Up connection via USB modem or<br>
				Dial-Up connection via USB-serial converter and modem.
			</td>
		</tr>
		<tr>
			<td class="content_header_td_30" onmouseover="return overlib('Set User defined Dial-Up connection.', LEFT);"
				onmouseout="return nd();">
				<input type="radio" value="99" name="wan_modem_mode_x" onclick="changeUSBConnectionType();"
					<% nvram_match_x("","wan_modem_mode_x", "99", "checked"); %>>
				User defined
			</td>
			<td class="content_desc_td">
				User defined Dial-Up connection.
			</td>
		</tr>
	</table>
	<!-- ********************************************************************** -->
	<div id='CDMA' class='vanishing'>
		<table class="content_table">
			<tr class="content_section_header_td">
				<td class="content_section_header_td" colspan="2">
					CDMA
				</td>
			</tr>
		</table>
	</div>
	<!-- ********************************************************************** -->
	<div id='GPRS' class='vanishing'>
		<table class="content_table">
			<tr class="content_header_tr">
				<td class="content_section_header_td" colspan="2">
					GPRS
				</td>
			</tr>
		</table>
	</div>
	<!-- ********************************************************************** -->
	<div id='ZadaCOM' class='vanishing'>
		<table class="content_table">
			<tr class="content_header_tr">
				<td class="content_section_header_td" colspan="2">
					ZadaCOM
				</td>
			</tr>
		</table>
	</div>
	<!-- ********************************************************************** -->
	<div id='Dial-Up' class='vanishing'>
		<table class="content_table">
			<tr class="content_header_tr">
				<td class="content_section_header_td" colspan="2">
					Dial-Up
				</td>
			</tr>
			<tr>
				<td class="content_header_td">
					Modem init AT Command:
				</td>
				<td class="content_input_td">
					<input type="text" maxlength="256" class="content_input_fd" size="32" name="wan_modem_dialup_init"
						value="<% nvram_get_x("LANHostConfig","wan_modem_dialup_init"); %>" onkeypress="return is_string(this)"
						onblur="validate_string(this)">
				</td>
			</tr>
		</table>
	</div>
	<!-- ********************************************************************** -->
	<div id='IPW' class='vanishing'>
		<table class="content_table">
			<tr class="content_header_tr">
				<td class="content_section_header_td" colspan="2">
					IP Wireless
				</td>
			</tr>
			<tr>
				<td class="content_header_td">
					PIN:
				</td>
				<td class="content_input_td">
					<input type="text" maxlength="4" class="content_input_fd" size="32" name="wan_modem_pin"
						value="<% nvram_get_x("LANHostConfig","wan_modem_pin"); %>" onkeypress="return is_number(this)"
						onblur="validate_string(this)">
				</td>
			</tr>
		</table>
	</div>
	<!-- ********************************************************************** -->
	<div id='User' class='vanishing'>
		<table class="content_table">

			<script type="text/javascript">
 dnam="<% nvram_dump("../usr/local/dial/param/name",""); %>"
 desc="<% nvram_dump("../usr/local/dial/param/desc",""); %>"
 par1="<% nvram_dump("../usr/local/dial/param/param1",""); %>"
 par2="<% nvram_dump("../usr/local/dial/param/param2",""); %>"
 par3="<% nvram_dump("../usr/local/dial/param/param3",""); %>"
 par4="<% nvram_dump("../usr/local/dial/param/param4",""); %>"
 par5="<% nvram_dump("../usr/local/dial/param/param5",""); %>"
 par6="<% nvram_dump("../usr/local/dial/param/param6",""); %>"
 
 if (dnam == "")
  dnam = "User Dial-Up Config"

 document.write('<tr class="content_header_tr"><td class="content_section_header_td" colspan="2">')
 document.write(dnam)
 document.write('</td></tr>')
  

 if (desc != "")
 {
   document.write('<tr><td class="content_desc_td" colspan="2">')
   document.write(desc)
   document.write('</td></tr>')
 }

 if (par1 != "")
 {
   document.write('<tr><td class="content_header_td">'+par1+'</td>')
   document.write('<td class="content_input_td"><input type="text" maxlength="256" class="content_input_fd" size="32"')
   document.write('name="dialup_par1"')
   document.write('value="<% nvram_get_x("LANHostConfig","wan_dialup_par1"); %>"')
   document.write('onKeyPress="return is_string(this)" onBlur="validate_string(this)">')
   document.write('</td></tr>')
 }
 if (par2 != "")
 {
   document.write('<tr><td class="content_header_td">'+par2+'</td>')
   document.write('<td class="content_input_td"><input type="text" maxlength="256" class="content_input_fd" size="32"')
   document.write('name="dialup_par2"')
   document.write('value="<% nvram_get_x("LANHostConfig","wan_dialup_par2"); %>"')
   document.write('onKeyPress="return is_string(this)" onBlur="validate_string(this)">')
   document.write('</td></tr>')
 }
 if (par3 != "")
 {
   document.write('<tr><td class="content_header_td">'+par3+'</td>')
   document.write('<td class="content_input_td"><input type="text" maxlength="256" class="content_input_fd" size="32"')
   document.write('name="dialup_par3"')
   document.write('value="<% nvram_get_x("LANHostConfig","wan_dialup_par3"); %>"')
   document.write('onKeyPress="return is_string(this)" onBlur="validate_string(this)">')
   document.write('</td></tr>')
 }
 if (par4 != "")
 {
   document.write('<tr><td class="content_header_td">'+par4+'</td>')
   document.write('<td class="content_input_td"><input type="text" maxlength="256" class="content_input_fd" size="32"')
   document.write('name="dialup_par4"')
   document.write('value="<% nvram_get_x("LANHostConfig","wan_dialup_par4"); %>"')
   document.write('onKeyPress="return is_string(this)" onBlur="validate_string(this)">')
   document.write('</td></tr>')
 }
 if (par5 != "")
 {
   document.write('<tr><td class="content_header_td">'+par5+'</td>')
   document.write('<td class="content_input_td"><input type="text" maxlength="256" class="content_input_fd" size="32"')
   document.write('name="dialup_par5"')
   document.write('value="<% nvram_get_x("LANHostConfig","wan_dialup_par5"); %>"')
   document.write('onKeyPress="return is_string(this)" onBlur="validate_string(this)">')
   document.write('</td></tr>')
 }
 if (par6 != "")
 {
   document.write('<tr><td class="content_header_td">'+par6+'</td>')
   document.write('<td class="content_input_td"><input type="text" maxlength="256" class="content_input_fd" size="32"')
   document.write('name="wan_dialup_par6"')
   document.write('value="<% nvram_get_x("LANHostConfig","wan_dialup_par6"); %>"')
   document.write('onKeyPress="return is_string(this)" onBlur="validate_string(this)">')
   document.write('</td></tr>')
 }

 if (par1 == "" && par2 == "" && par3 == "" && par4 == "" && par5 == "" && par6 == "")
 {
   document.write('<tr><td class="content_desc_td" colspan="2">')
   document.write('You must first upload user Dial-Up Files to use this feature!')
   document.write('</td></tr>')
 }
</script>

		</table>
	</div>
	<!-- ********************************************************************** -->
	<div id='UserPasswd' class='vanishing'>
		<table class="content_table">
			<tr>
				<td class="content_header_td">
					Username:
				</td>
				<td class="content_input_td">
					<input type="text" maxlength="256" class="content_input_fd" size="32" name="wan_modem_username"
						value="<% nvram_get_x("LANHostConfig","wan_modem_username"); %>" onkeypress="return is_string(this)"
						onblur="validate_string(this)">
				</td>
			</tr>
			<tr>
				<td class="content_header_td">
					Password:
				</td>
				<td class="content_input_td">
					<input type="password" maxlength="256" class="content_input_fd" size="32" name="wan_modem_password"
						value="<% nvram_get_x("LANHostConfig","wan_modem_password"); %>" onkeypress="return is_string(this)"
						onblur="validate_string(this)">
				</td>
			</tr>
		</table>
	</div>
	<div id='APN' class='vanishing'>
		<table class="content_table">
			<tr>
				<td class="content_header_td">
					APN:
				</td>
				<td class="content_input_td">
					<input type="text" maxlength="256" class="content_input_fd" size="32" name="wan_modem_apn"
						value="<% nvram_get_x("LANHostConfig","wan_modem_apn"); %>" onkeypress="return is_string(this)"
						onblur="validate_string(this)">
				</td>
			</tr>
		</table>
	</div>
	<div id='DialupNO' class='vanishing'>
		<table class="content_table">
			<tr id='Tr2' class='vanishing'>
				<td class="content_header_td">
					Dial Number (usually *99***1# or *99#):
				</td>
				<td class="content_input_td">
					<input type="text" maxlength="256" class="content_input_fd" size="32" name="wan_modem_dialno"
						value="<% nvram_get_x("LANHostConfig","wan_modem_dialno"); %>" onkeypress="return is_string(this)"
						onblur="validate_string(this)">
				</td>
			</tr>
		</table>
	</div>
	<div id='PortSpeed' class='vanishing'>
		<table class="content_table">
			<tr>
				<td class="content_header_td">
					USB device serial speed (usually 115200):
				</td>
				<td class="content_input_td">
					<input type="text" maxlength="256" class="content_input_fd" size="32" name="wan_modem_portspeed"
						value="<% nvram_get_x("LANHostConfig","wan_modem_portspeed"); %>" onkeypress="return is_number(this)"
						onblur="validate_range(this, 0, 921600)">
				</td>
			</tr>
		</table>
	</div>
	<div id='ZeroCD' class='vanishing'>
		<table class="content_table">
			<tr class="content_header_tr">
				<td class="content_section_header_td" colspan="2">
					Zero CD Configuration
				</td>
			</tr>
			<tr>
				<td class="content_header_td">
					Modem type:
				</td>
				<td class="content_input_td">
					<select name="wan_modem_zerocd_mode" class="content_input_fd">
						<option class="content_input_fd" value="" <%nvram_match_x("LANHostConfig","wan_modem_zerocd_mode",  "","selected"); %>>
							Not set</option>
						<option class="content_input_fd" value="OptionGlobeSurferIcon" <%nvram_match_x("LANHostConfig","wan_modem_zerocd_mode",  "OptionGlobeSurferIcon","selected"); %>>
							Option GlobeSurfer Icon (aka "Vodafone EasyBox")</option>
						<option class="content_input_fd" value="OptionGlobeSurferIcon_7_2" <%nvram_match_x("LANHostConfig","wan_modem_zerocd_mode",  "OptionGlobeSurferIcon_7_2","selected"); %>>
							Option GlobeSurfer Icon 7.2</option>
						<option class="content_input_fd" value="OptionGlobeSurferIcon_7_2_new" <%nvram_match_x("LANHostConfig","wan_modem_zerocd_mode",  "OptionGlobeSurferIcon_7_2_new","selected"); %>>
							Option GlobeSurfer Icon 7.2, new firmware</option>
						<option class="content_input_fd" value="OptionIcon_225" <%nvram_match_x("LANHostConfig","wan_modem_zerocd_mode",  "OptionIcon_225","selected"); %>>
							Option Icon 225 HSDPA</option>
						<option class="content_input_fd" value="OptionGlobeTrotter" <%nvram_match_x("LANHostConfig","wan_modem_zerocd_mode", "OptionGlobeTrotter","selected"); %>>
							Option GlobeTrotter HSUPA Modem (aka "T-Mobile CC III")</option>
						<option class="content_input_fd" value="OptionGlobeTrotterGTMAX_3_6" <%nvram_match_x("LANHostConfig","wan_modem_zerocd_mode", "OptionGlobeTrotterGTMAX_3_6","selected"); %>>
							Option GlobeTrotter GT MAX 3.6 (aka "T-Mobile CC II")</option>
						<option class="content_input_fd" value="OptionGlobeTrotterGTMAX_7_2" <%nvram_match_x("LANHostConfig","wan_modem_zerocd_mode", "OptionGlobeTrotterGTMAX_7_2","selected"); %>>
							Option GlobeTrotter GT MAX "7.2 Ready"</option>
						<option class="content_input_fd" value="OptionGlobeTrotterEXPRESS_7_2" <%nvram_match_x("LANHostConfig","wan_modem_zerocd_mode", "OptionGlobeTrotterEXPRESS_7_2","selected"); %>>
							Option GlobeTrotter EXPRESS 7.2 (aka "T-Mobile Express II")</option>
						<option class="content_input_fd" value="Huawei_E220" <%nvram_match_x("LANHostConfig","wan_modem_zerocd_mode", "Huawei_E220","selected"); %>>
							Huawei E220, E270, E870</option>
						<option class="content_input_fd" value="Huawei_E169" <%nvram_match_x("LANHostConfig","wan_modem_zerocd_mode", "Huawei_E169","selected"); %>>
							Huawei E169</option>
						<option class="content_input_fd" value="Huawei_E630" <%nvram_match_x("LANHostConfig","wan_modem_zerocd_mode", "Huawei_E630","selected"); %>>
							Huawei E630</option>
						<option class="content_input_fd" value="ZTE_MF620" <%nvram_match_x("LANHostConfig","wan_modem_zerocd_mode", "ZTE_MF620","selected"); %>>
							ZTE MF620 (aka "Onda MH600HS")</option>
						<option class="content_input_fd" value="ZTE_MF622" <%nvram_match_x("LANHostConfig","wan_modem_zerocd_mode", "ZTE_MF622","selected"); %>>
							ZTE MF622</option>
						<option class="content_input_fd" value="ZTE_MF626" <%nvram_match_x("LANHostConfig","wan_modem_zerocd_mode", "ZTE_MF626","selected"); %>>
							ZTE MF626</option>
						<option class="content_input_fd" value="ONDA_MT505UP" <%nvram_match_x("LANHostConfig","wan_modem_zerocd_mode", "ONDA_MT505UP","selected"); %>>
							ONDA MT505UP (most likely a ZTE model)</option>
						<option class="content_input_fd" value="NovatelWirelessOvation" <%nvram_match_x("LANHostConfig","wan_modem_zerocd_mode", "NovatelWirelessOvation","selected"); %>>
							Novatel Wireless Ovation MC950D and 930D, Merlin XU950D</option>
						<option class="content_input_fd" value="Novatel_U727" <%nvram_match_x("LANHostConfig","wan_modem_zerocd_mode", "Novatel_U727","selected"); %>>
							Novatel U727</option>
						<option class="content_input_fd" value="Alcatel_OT_X020" <%nvram_match_x("LANHostConfig","wan_modem_zerocd_mode", "Alcatel_OT_X020","selected"); %>>
							Alcatel OT-X020</option>
						<option class="content_input_fd" value="AnyDATA_ADU" <%nvram_match_x("LANHostConfig","wan_modem_zerocd_mode", "AnyDATA_ADU","selected"); %>>
							AnyDATA ADU-500A, ADU-510A, ADU-520A</option>
						<option class="content_input_fd" value="UserDefined" <%nvram_match_x("LANHostConfig","wan_modem_zerocd_mode", "UserDefined","selected"); %>>
							User config at /usr/local/etc/usb_modeswitch.conf</option>
					</select>
				</td>
			</tr>
		</table>
	</div>
	<!-- ********************************************************************** -->
	<div id='USB' class='vanishing'>
		<table class="content_table">
			<tr class="content_header_tr">
				<td class="content_section_header_td" colspan="2">
					Custom USB device parameters
				</td>
			</tr>
			<tr>
				<td class="content_header_td" onmouseover="return overlib('USB device location in USB tree. See Status &amp; Log -> Diagnostic Info -> USB devfs Devices and find the ID in the second column (eg 1.1.0). If blank, use default.')"
					onmouseout="return nd()">
					USB device location ID:
				</td>
				<td class="content_input_td">
					<input type="text" maxlength="256" class="content_input_fd" size="32" name="wan_modem_usbloc"
						value="<% nvram_get_x("LANHostConfig","wan_modem_usbloc"); %>" onkeypress="return is_string(this)"
						onblur="validate_string(this)">
				</td>
			</tr>
			<tr>
				<td class="content_header_td">
					USB device Vendor ID (0xabcd):
				</td>
				<td class="content_input_td">
					<input type="text" maxlength="256" class="content_input_fd" size="32" name="wan_modem_vid"
						value="<% nvram_get_x("LANHostConfig","wan_modem_vid"); %>" onkeypress="return is_string(this)"
						onblur="validate_string(this)">
				</td>
			</tr>
			<tr>
				<td class="content_header_td">
					USB device Product ID (0xefgh):
				</td>
				<td class="content_input_td">
					<input type="text" maxlength="256" class="content_input_fd" size="32" name="wan_modem_pid"
						value="<% nvram_get_x("LANHostConfig","wan_modem_pid"); %>" onkeypress="return is_string(this)"
						onblur="validate_string(this)">
				</td>
			</tr>
			<tr>
				<td class="content_header_td">
					USB device packet size (0 for default):
				</td>
				<td class="content_input_td">
					<input type="text" maxlength="256" class="content_input_fd" size="32" name="wan_modem_packetsize"
						value="<% nvram_get_x("LANHostConfig","wan_modem_packetsize"); %>" onkeypress="return is_number(this)"
						onblur="validate_range(this,0,16384)">
				</td>
			</tr>
			<tr>
				<td class="content_header_td_less">
					Action:
				</td>
				<td class="content_input_td">
					<input type="submit" class="content_input_fd_ro" onclick="return onSubmitApply('dhcpc_release')"
						name="PPPConnection_x_WANAction_button" value="Disconnect"><input type="submit" class="content_input_fd_ro"
							onclick="return onSubmitApply('dhcpc_renew')" name="PPPConnection_x_WANAction_button1"
							value="Connect">
				</td>
			</tr>
		</table>
	</div>
	<!-- ********************************************************************** -->
	<table class="content_table">
		<tr bgcolor="#CCCCCC">
			<td colspan="3" style='font-family: "Arial"; font-size: 2pt;'>
				<b>&nbsp;</b>
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
