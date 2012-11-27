<!--<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.01//EN" "http://www.w3.org/TR/html4/strict.dtd">-->
<html>
<head>
	<title>ZVMODELVZ Web Manager</title>
	<meta http-equiv="Content-Type" content="text/html; charset=UTF-8">
	<link rel="stylesheet" type="text/css" href="style.css" media="screen">

	<script type="text/javascript" src="overlib.js"></script>
	<script type="text/javascript" src="general.js"></script>
	<script type="text/javascript" src="quick.js"></script>
	
	<style type="text/css">
	div.c1 {text-align: center; font-family: Arial}
	</style>
</head>
<body style='background-color: #FFFFFF' onload="loadQuick()">
	<div id="overDiv" style="position: absolute; visibility: hidden; z-index: 1000;">
	</div>
	<form method="GET" name="form" action="apply.cgi">
	<input type="hidden" name="x_Mode" value="0">
	<input type="hidden" name="current_page" value="Basic_GOperation_Content.asp">
	<input type="hidden" name="next_page" value="SaveRestart.asp">
	<input type="hidden" name="next_host" value="">
	<input type="hidden" name="sid_list" value="Layer3Forwarding;IPConnection;PPPConnection;WLANConfig11a;WLANConfig11b;LANHostConfig;FirewallConfig;">
	<input type="hidden" name="action_mode" value="">
	<input type="hidden" name="modified" value="0">
	<input type="hidden" name="group_id" value="">
	<input type="hidden" name="wan_proto" value="<% nvram_get("wan_proto"); %>">
	<input type="hidden" name="wan_route_x" value="<% nvram_get("wan_route_x"); %>">
	<input type="hidden" name="wan_nat_x" value="<% nvram_get("wan_nat_x"); %>">
	<input type="hidden" name="wl_auth_mode" value="<% nvram_get("wl_auth_mode"); %>">
	<input type="hidden" name="wl_crypto" value="<% nvram_get("wl_crypto"); %>">
	<input type="hidden" name="wl_wep_x" value="<% nvram_get("wl_wep_x"); %>">
	<!-- Table for the conntent page -->
	<table width="666" border="0" cellpadding="0" cellspacing="0">
		<tr>
			<td>
				<table width="666" border="1" cellpadding="0" cellspacing="0" bordercolor="#E0E0E0">
					<tr id="Country" class="content_header_tr">
						<td class="content_header_td_title" colspan="2">
							Quick Setup
						</td>
					</tr>
					<tr class="content_section_header_tr">
						<td class="content_section_header_td" colspan="2">
							Select Time Zone
						</td>
					</tr>
					<tr>
						<td class="content_desc_td" colspan="2" height="50">
							Please choose the time zone where you are locating in.
						</td>
					</tr>
					<tr>
						<td class="content_header_td" onmouseover="return overlib('This field indicates time zone where you are locating in.', LEFT);"
							onmouseout="return nd();">
							Time Zone:
						</td>
						<td class="content_input_td">
							<select name="TimeZoneList" class="content_input_fd" onchange="return change_common(this, 'LANHostConfig', 'time_zone')">
								<option class="content_input_fd" value="manual">Manual</option>
							</select>
						</td>
					</tr>
					<tr>
						<td class="content_header_td" onmouseover="return overlib('This field indicates time zone where you are locating in.', LEFT);"
							onmouseout="return nd();">
							Time Zone Abbreviation:
						</td>
						<td class="content_input_td">
							<input type="text" maxlength="256" class="content_input_fd" size="32" name="time_zone"
								value="<% nvram_get("time_zone"); %>" onkeypress="return is_string(event, this)"
								onblur="validate_string(this)">
						</td>
					</tr>
					<tr>
						<td class="content_input_td" colspan="2">
							<table>
								<tr>
									<td width="500" height="100">
									</td>
									<td>
										<div class='c1'>
											<input class="inputSubmit" onmouseout="buttonOut(this)" onmouseover="buttonOver(this)"
												onclick="toTag('Country')" type="button" value="Next" name="action">
										</div>
									</td>
								</tr>
								<tr>
									<td colspan="2" height="360">
									</td>
								</tr>
							</table>
						</td>
					</tr>
					<tr id="Broadband" class="content_header_tr">
						<td class="content_header_td_title" colspan="2">
							Quick Setup
						</td>
					</tr>
					<tr class="content_section_header_tr">
						<td class="content_section_header_td" colspan="2">
							Select Internet Connection Type
						</td>
					</tr>
					<tr>
						<td class="content_desc_td" colspan="2" height="50">
							ZVMODELVZ supports several kinds of connection to Internet through its WAN port.
							Please select connection type you need. In addition, before getting on Internet,
							please make sure you have connected ZVMODELVZ's WAN port to your DSL or Cable Modem.
						</td>
					</tr>
					<tr>
						<td class="content_header_td_less" onmouseout="return nd();" colspan="2">
							<p>
							</p>
							<p>
								<input type="radio" checked name="x_WANType" value="0" class="content_input_fd" onclick="changeWANType()">
								Cable Modem or other connection type that gets IP automatically.</p>
							<p>
								<input type="radio" checked name="x_WANType" value="1" class="content_input_fd" onclick="changeWANType()">
								ADSL or other connection that requires username and password. It is known as PPPoE.</p>
							<p>
								<input type="radio" checked name="x_WANType" value="2" class="content_input_fd" onclick="changeWANType()">
								ADSL or other connection that requires username, password and IP address. It is
								known as PPTP.</p>
							<p>
								<input type="radio" checked name="x_WANType" value="3" class="content_input_fd" onclick="changeWANType()">
								ADSL or other connection type that uses static IP address.</p>
							<p>
								<input type="radio" checked name="x_WANType" value="4" class="content_input_fd" onclick="changeWANType()">
								Telstra BigPond Cable Modem Service.</p>
							<p>
								<input type="radio" checked name="x_WANType" value="5" class="content_input_fd" onclick="changeWANType()">
								ADSL or other connection that requires username, password and IP address. It is
								known as L2TP.</p>
							<p>
							</p>
							<p>
							</p>
						</td>
					</tr>
					<tr>
						<td class="content_input_td" colspan="2">
							<table>
								<tr>
									<td width="444" height="100">
									</td>
									<td>
										<div class='c1'>
											<input class="inputSubmit" onmouseout="buttonOut(this)" onmouseover="buttonOver(this)"
												onclick="toPrevTag('Broadband')" type="button" value="Prev" name="action">&nbsp;&nbsp;
											<input class="inputSubmit" onmouseout="buttonOut(this)" onmouseover="buttonOver(this)"
												onclick="toTag('Broadband')" type="button" value="Next" name="action">
										</div>
									</td>
								</tr>
								<tr>
									<td colspan="2" height="360">
									</td>
								</tr>
							</table>
						</td>
					</tr>
					<tr id="PPPoE" class="content_header_tr">
						<td class="content_header_td_title" colspan="2">
							Quick Setup
						</td>
					</tr>
					<tr class="content_section_header_tr">
						<td class="content_section_header_td" colspan="2">
							Set Your Account to ISP
						</td>
					</tr>
					<tr>
						<td class="content_desc_td" colspan="2" height="50">
							If you apply an account with dynamic IP, you must get user account and password
							from your ISP. Please fill this data into the following fields carefully. Or, if
							you apply an ADSL account with static IP, just ignore user name and password information.
						</td>
					</tr>
					<tr>
						<td class="content_header_td_less">
							User Name:
						</td>
						<td class="content_input_td">
							<input type="text" maxlength="64" size="32" name="wan_pppoe_username" class="content_input_fd"
								value="<% nvram_get("wan_pppoe_username"); %>">
						</td>
					</tr>
					<tr>
						<td class="content_header_td_less">
							Password:
						</td>
						<td class="content_input_td">
							<input type="password" maxlength="64" size="32" name="wan_pppoe_passwd" class="content_input_fd"
								value="<% nvram_get("wan_pppoe_passwd"); %>">
						</td>
					</tr>
					<tr>
						<td class="content_input_td" colspan="2">
							<table>
								<tr>
									<td width="444" height="100">
									</td>
									<td>
										<div class='c1'>
											<input class="inputSubmit" onmouseout="buttonOut(this)" onmouseover="buttonOver(this)"
												onclick="toPrevTag('PPPoE')" type="button" value="Prev" name="action">&nbsp;&nbsp;
											<input class="inputSubmit" onmouseout="buttonOut(this)" onmouseover="buttonOver(this)"
												onclick="toTag('PPPoE')" type="button" value="Next" name="action">
										</div>
									</td>
								</tr>
								<tr>
									<td colspan="2" height="360">
									</td>
								</tr>
							</table>
						</td>
					</tr>
					<tr id="MacHost" class="content_header_tr">
						<td class="content_header_td_title" colspan="2">
							Quick Setup
						</td>
					</tr>
					<tr class="content_section_header_tr">
						<td class="content_section_header_td" colspan="2">
							Fill Information Required by ISP
						</td>
					</tr>
					<tr>
						<td class="content_desc_td" colspan="2" height="50">
							Your ISP may require the following information to identify your account. If not,
							just press Next to ignore it.
						</td>
					</tr>
					<tr>
						<td class="content_header_td_less" onmouseover="return overlib('Please enter the server name or server ip of the authentication server of BigPond service.', LEFT);"
							onmouseout="return nd();">
							Heart-Beat or PPTP/L2TP (VPN) Server:
						</td>
						<td class="content_input_td">
							<input type="text" maxlength="256" size="32" name="wan_heartbeat_x" class="content_input_fd"
								value="<% nvram_get("wan_heartbeat_x"); %>">
						</td>
					</tr>
					<tr>
						<td class="content_header_td_less" onmouseover="return overlib('This field allows you to provide a host name for ZVMODELVZ. It is usually requested by your ISP.', LEFT);"
							onmouseout="return nd();">
							Host Name:
						</td>
						<td class="content_input_td">
							<input type="text" maxlength="32" size="32" name="wan_hostname" class="content_input_fd"
								value="<% nvram_get("wan_hostname"); %>">
						</td>
					</tr>
					<tr>
						<td class="content_header_td_less" onmouseover="return overlib('This field allows you to provide a unique MAC address for ZVMODELVZ to connect to Internet. It is usually requested by your ISP.', LEFT);"
							onmouseout="return nd();">
							MAC Address:
						</td>
						<td class="content_input_td">
							<input type="text" maxlength="12" size="12" name="wan_hwaddr_x" class="content_input_fd"
								value="<% nvram_get("wan_hwaddr_x"); %>" onblur="return validate_hwaddr(this)"
								onkeypress="return is_hwaddr(event, this)">
						</td>
					</tr>
					<tr>
						<td class="content_input_td_less" colspan="2">
							<table>
								<tr>
									<td width="444" height="100">
									</td>
									<td>
										<div class='c1'>
											<input class="inputSubmit" onmouseout="buttonOut(this)" onmouseover="buttonOver(this)"
												onclick="toPrevTag('MacHost')" type="button" value="Prev" name="action">&nbsp;&nbsp;
											<input class="inputSubmit" onmouseout="buttonOut(this)" onmouseover="buttonOver(this)"
												onclick="toTag('MacHost')" type="button" value="Next" name="action">
										</div>
									</td>
								</tr>
								<tr>
									<td colspan="2" height="360">
									</td>
								</tr>
							</table>
						</td>
					</tr>
					<tr id="WANSetting" class="content_header_tr">
						<td class="content_header_td_title" colspan="2">
							Quick Setup
						</td>
					</tr>
					<tr class="content_section_header_tr">
						<td class="content_section_header_td" colspan="2">
							WAN IP Setting
						</td>
					</tr>
					<tr>
						<td class="content_desc_td" colspan="2" height="50">
							Fill TCP/IP setting for ZVMODELVZ to connect to Internet through WAN port.
						</td>
					</tr>
					<tr>
						<td class="content_header_td_less">
							Get IP automatically?
						</td>
						<td class="content_input_td">
							<input type="radio" value="1" name="x_DHCPClient" class="content_input_fd" onclick="changeDHCPClient()">Yes
							<input type="radio" value="0" name="x_DHCPClient" class="content_input_fd" onclick="changeDHCPClient()">No
						</td>
					</tr>
					<tr>
						<td class="content_header_td_less" onmouseover="return overlib('IP address of WAN Interface. If you set it to 0.0.0.0, ZVMODELVZ will get IP address from DHCP Server automatically.', LEFT);"
							onmouseout="return nd();">
							IP Address:
						</td>
						<td class="content_input_td">
							<input type="text" maxlength="15" size="15" name="wan_ipaddr" class="content_input_fd"
								value="<% nvram_get("wan_ipaddr"); %>" onblur="return validate_ipaddr(this, 'wan_ipaddr')"
								onkeypress="return is_ipaddr(event, this)">
						</td>
					</tr>
					<tr>
						<td class="content_header_td_less">
							Subnet Mask:
						</td>
						<td class="content_input_td">
							<input type="text" maxlength="15" size="15" name="wan_netmask" class="content_input_fd"
								value="<% nvram_get("wan_netmask"); %>" onblur="return validate_ipaddr(this)"
								onkeypress="return is_ipaddr(event, this)">
						</td>
					</tr>
					<tr>
						<td class="content_header_td_less">
							Default Gateway:
						</td>
						<td class="content_input_td">
							<input type="text" maxlength="15" size="15" name="wan_gateway" class="content_input_fd"
								value="<% nvram_get("wan_gateway"); %>" onblur="return validate_ipaddr(this)"
								onkeypress="return is_ipaddr(event, this)">
						</td>
					</tr>
					<tr>
						<td class="content_header_td_less">
							Get DNS Server automatically?
						</td>
						<td class="content_input_td">
							<input type="radio" value="1" name="wan_dnsenable_x" class="content_input_fd" onclick="changeDNSServer()"
								<% nvram_match_x("IPConnection","wan_dnsenable_x", "1", "checked"); %>>Yes
							<input type="radio" value="0" name="wan_dnsenable_x" class="content_input_fd" onclick="changeDNSServer()"
								<% nvram_match_x("IPConnection","wan_dnsenable_x", "0", "checked"); %>>No
						</td>
					</tr>
					<tr>
						<td class="content_header_td_less">
							DNS Server 1:
						</td>
						<td class="content_input_td">
							<input type="text" maxlength="15" size="15" name="wan_dns1_x" class="content_input_fd"
								value="<% nvram_get("wan_dns1_x"); %>" onblur="return validate_ipaddr(this)"
								onkeypress="return is_ipaddr(event, this)">
						</td>
					</tr>
					<tr>
						<td class="content_header_td_less">
							DNS Server 2:
						</td>
						<td class="content_input_td">
							<input type="text" maxlength="15" size="15" name="wan_dns2_x" class="content_input_fd"
								value="<% nvram_get("wan_dns2_x"); %>" onblur="return validate_ipaddr(this)"
								onkeypress="return is_ipaddr(event, this)">
						</td>
					</tr>
					<tr>
						<td class="content_input_td" colspan="2">
							<table>
								<tr>
									<td width="444" height="100">
									</td>
									<td>
										<div class='c1'>
											<input class="inputSubmit" onmouseout="buttonOut(this)" onmouseover="buttonOver(this)"
												onclick="toPrevTag('WANSetting')" type="button" value="Prev" name="action">&nbsp;&nbsp;
											<input class="inputSubmit" onmouseout="buttonOut(this)" onmouseover="buttonOver(this)"
												onclick="toTag('WANSetting')" type="button" value="Next" name="action">
										</div>
									</td>
								</tr>
								<tr>
									<td colspan="2" height="360">
									</td>
								</tr>
							</table>
						</td>
					</tr>
					<tr id="Wireless" class="content_header_tr">
						<td class="content_header_td_title" colspan="2">
							Quick Setup
						</td>
					</tr>
					<tr class="content_section_header_tr">
						<td class="content_section_header_td" colspan="2">
							Configure Wireless Interface
						</td>
					</tr>
					<tr>
						<td class="content_desc_td" colspan="2" height="50">
							First step to set your wireless interface is to give it a name, called SSID. In
							addition, if you would like to protect transmitted data, please select the Security
							Level and assign a password for authentication and data transmission if it is required.
						</td>
					</tr>
					<tr>
						<td class="content_header_td_less" onmouseover="return overlib('Assign an identification string, consisting of up to 32 characters, for your WLAN.', LEFT);"
							onmouseout="return nd();">
							SSID:
						</td>
						<td class="content_input_td">
							<input type="text" maxlength="32" size="32" name="wl_ssid" class="content_input_fd"
								value="<% nvram_get("wl_ssid"); %>" onblur="validate_string(this)">
						</td>
					</tr>
					<tr>
						<td class="content_header_td_less" onmouseover="return overlib('Selecting Low allows any users to connect to this access point and to transmit data without encryption. Selecting Middle allows only those users use the same WEP key to connect to this access point and to transmit data with WEP encryption. Selecting High allows only those users use the same WPA pre-shared key to connect to this access point and to transmit data with TKIP encryption.', LEFT);"
							onmouseout="return nd();">
							Security Level:
						</td>
						<td class="content_input_td">
							<select name="SecurityLevel" class="content_input_fd" onchange="return change_security(this, 'WLANConfig11b', 0)">
								<option value="0">Low(None)</option>
								<option value="1">Medium(WEP-64bits)</option>
								<option value="2">Medium(WEP-128bits)</option>
								<option value="3">High(WPA-PSK)</option>
							</select>
						</td>
					</tr>
					<tr>
						<td class="content_header_td_less" onmouseover="return overlib('Selecting High Security Level, this filed will be used as a password to kicks off the TKIP encryption process. A 8~63 characters password is required. Selecting Middle Security Level, this field will be used to generate four WEP keys automatically.', LEFT);"
							onmouseout="return nd();">
							Passphrase:
						</td>
						<td class="content_input_td">

							<script type="text/javascript" src="md5.js"></script>

							<input type="password" maxlength="64" size="32" name="wl_wpa_psk" class="content_input_fd"
								value="<% nvram_get("wl_wpa_psk"); %>" onkeyup="return is_wlphrase_q('WLANConfig11b', this)"
								onblur="return validate_wlphrase_q('WLANConfig11b', this)">
						</td>
					</tr>
					<tr>
						<td class="content_header_td_less">
							WEP Key 1 (10 or 26 hex digits):
						</td>
						<td class="content_input_td">
							<input type="text" maxlength="32" size="32" name="wl_key1" class="content_input_fd"
								value="<% nvram_get("wl_key1"); %>" onblur="return validate_wlkey(this, 'WLANConfig11b')"
								onkeypress="return is_wlkey(event, this, 'WLANConfig11b')" onkeyup="return change_wlkey(this, 'WLANConfig11b')">
						</td>
					</tr>
					<tr>
						<td class="content_header_td_less">
							WEP Key 2 (10 or 26 hex digits):
						</td>
						<td class="content_input_td">
							<input type="text" maxlength="32" size="32" name="wl_key2" class="content_input_fd"
								value="<% nvram_get("wl_key2"); %>" onblur="return validate_wlkey(this, 'WLANConfig11b')"
								onkeypress="return is_wlkey(event, this, 'WLANConfig11b')" onkeyup="return change_wlkey(this, 'WLANConfig11b')">
						</td>
					</tr>
					<tr>
						<td class="content_header_td_less">
							WEP Key 3 (10 or 26 hex digits):
						</td>
						<td class="content_input_td">
							<input type="text" maxlength="32" size="32" name="wl_key3" class="content_input_fd"
								value="<% nvram_get("wl_key3"); %>" onblur="return validate_wlkey(this, 'WLANConfig11b')"
								onkeypress="return is_wlkey(event, this, 'WLANConfig11b')" onkeyup="return change_wlkey(this, 'WLANConfig11b')">
						</td>
					</tr>
					<tr>
						<td class="content_header_td_less">
							WEP Key 4 (10 or 26 hex digits):
						</td>
						<td class="content_input_td">
							<input type="text" maxlength="32" size="32" name="wl_key4" class="content_input_fd"
								value="<% nvram_get("wl_key4"); %>" onblur="return validate_wlkey(this, 'WLANConfig11b')"
								onkeypress="return is_wlkey(event, this, 'WLANConfig11b')" onkeyup="return change_wlkey(this, 'WLANConfig11b')">
						</td>
					</tr>
					<tr>
						<td class="content_header_td_less">
							Key Index:
						</td>
						<td class="content_input_td">
							<select name="wl_key" class="content_input_fd" onchange="return change_common(this, 'WLANConfig11b', 'wl_key')">
								<option value="1" <% nvram_match_x("WLANConfig11b","wl_key", "1","selected"); %>>1</option>
								<option value="2" <% nvram_match_x("WLANConfig11b","wl_key", "2","selected"); %>>2</option>
								<option value="3" <% nvram_match_x("WLANConfig11b","wl_key", "3","selected"); %>>3</option>
								<option value="4" <% nvram_match_x("WLANConfig11b","wl_key", "4","selected"); %>>4</option>
							</select>
						</td>
					</tr>
				</table>
			</td>
		</tr>
		<tr>
			<td>
				<table width="666" border="1" cellpadding="0" cellspacing="0" bordercolor="#B0B0B0">
					<tr bgcolor="#CCCCCC">
						<td colspan="3" style='font-family: Arial; font-size: 2pt'>
							&nbsp;
						</td>
					</tr>
					<tr bgcolor="#FFFFFF">
						<td height="25" width="75%">
						</td>
						<td height="25">
							<div class='c1'>
								<input class="inputSubmit" onmouseout="buttonOut(this)" onmouseover="buttonOver(this)"
									onclick="toPrevTag('Wireless')" type="button" value="Prev" name="action">&nbsp;&nbsp;&nbsp;&nbsp;
								<input class="inputSubmit" onmouseout="buttonOut(this)" onmouseover="buttonOver(this)"
									type="submit" value="Finish" name="action" onclick="saveQuick(this)"></div>
						</td>
					</tr>
				</table>
			</td>
		</tr>
		<tr>
			<td>
				<table width="666" border="1" cellpadding="0" cellspacing="0" bordercolor="#B0B0B0">
					<tr>
						<td colspan="2" width="666" height="25" bgcolor="#FFBB00">
						</td>
					</tr>
				</table>
			</td>
		</tr>
		<tr>
			<td colspan="2" height="240">
			</td>
		</tr>
	</table>
	</form>
</body>
</html>

