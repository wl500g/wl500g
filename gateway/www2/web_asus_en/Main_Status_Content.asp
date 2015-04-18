<html>
<head>
<title>ZVMODELVZ Web Manager</title>
<link rel="stylesheet" type="text/css" href="style.css" media="screen">
<script type="text/javascript" src="overlib.js"></script>
<script type="text/javascript" src="general.js"></script>
</head>  
<body>
<div id="overDiv" style="position:absolute; visibility:hidden; z-index:1000;"></div>    
<form method="GET" name="form" action="apply.cgi">
<input type="hidden" name="current_page" value="Main_Status_Content.asp">
<input type="hidden" name="next_page" value="default value">
<input type="hidden" name="sid_list" value="default value">
<input type="hidden" name="group_id" value="">
<input type="hidden" name="modified" value="0">
<!-- Table for the conntent page -->	    
<table width="620" border="0" cellpadding="1" cellspacing="0">     	      
    	
<tr class="content_header_tr">
<td colspan="2">Status</td>
</tr>
<tr>
<td>
<table width="618" border="2" cellpadding="0" cellspacing="0" bordercolor="#E0E0E0">
<tr>
<td class="content_header_td" onMouseOver="return overlib('Elapsed time since system boot');" onMouseOut="return nd();">System Up Time:</td><td class="content_input_td"><input type="text" value="<% nvram_get_f("general.log","UpTime"); %>" readonly></td>
</tr>
<tr>
<td class="content_header_td">Product ID:</td><td class="content_input_td"><input type="text" value="<% nvram_get_f("general.log","ProductID"); %>" readonly></td>
</tr>
<tr>
<td class="content_header_td">Firmware Version:</td><td class="content_input_td"><input type="text" value="<% nvram_get_f("general.log","FirmwareVer"); %>" readonly></td>
</tr>
<tr>
<td class="content_header_td">Hardware Version:</td><td class="content_input_td"><input type="text" value="<% nvram_get_f("general.log","HardwareVer"); %>" readonly></td>
</tr>
<tr>
<td class="content_header_td">Boot Loader Version:</td><td class="content_input_td"><input type="text" value="<% nvram_get_f("general.log","BootloaderVer"); %>" readonly></td>
</tr>
</table>
</td>
</tr>
<tr>
<td>
<table width="618" border="2" cellpadding="0" cellspacing="0" bordercolor="#E0E0E0">
<tr class="content_section_header_tr">
<td colspan="2">11.a Interface</td>
</tr>
<tr>
<td class="content_header_td" onMouseOver="return overlib('Assign an identification string, consisting of up to 32 characters, for your WLAN');" onMouseOut="return nd();">SSID:</td><td class="content_input_td"><input type="text" value="<% nvram_get_x("WLANConfig11a","SSID"); %>" readonly></td>
</tr>
<tr>
<td class="content_header_td" onMouseOver="return overlib('Select the operating radio channel');" onMouseOut="return nd();">Channel:</td><td class="content_input_td"><input type="text" value="<% nvram_get_x("WLANConfig11a","Channel"); %>" readonly></td>
</tr>
<tr>
<td class="content_header_td" onMouseOver="return overlib('Enable WEP to encrypt data which can avoid disclosure to eavesdroppers');" onMouseOut="return nd();">Encryption(WEP):</td><td class="content_input_td"><input type="text" value="<% nvram_get_x("WLANConfig11a","WEPType"); %>" readonly></td>
</tr>
</table>
</td>
</tr>
<tr>
<td>
<table width="618" border="2" cellpadding="0" cellspacing="0" bordercolor="#E0E0E0">
<tr class="content_section_header_tr">
<td colspan="2">11.b Interface</td>
</tr>
<tr>
<td class="content_header_td" onMouseOver="return overlib('Assign an identification string, consisting of up to 32 characters, for your WLAN');" onMouseOut="return nd();">SSID:</td><td class="content_input_td"><input type="text" value="<% nvram_get_x("WLANConfig11b","SSID"); %>" readonly></td>
</tr>
<tr>
<td class="content_header_td" onMouseOver="return overlib('Select the operating radio channel');" onMouseOut="return nd();">Channel:</td><td class="content_input_td"><input type="text" value="<% nvram_get_x("WLANConfig11b","Channel"); %>" readonly></td>
</tr>
<tr>
<td class="content_header_td" onMouseOver="return overlib('Select 1,2,5.5,11 Mbps to maximize performace. Use 1,2 Mbps only when the backward compatibility is needed for some old WLAN cards which maximal bit rate is 2 Mbps');" onMouseOut="return nd();">Rate:</td><td class="content_input_td"><input type="text" value="<% nvram_get_x("WLANConfig11b","DataRate"); %>" readonly></td>
</tr>
<tr>
<td class="content_header_td" onMouseOver="return overlib('Enable WEP to encrypt data which can avoid disclosure to eavesdroppers');" onMouseOut="return nd();">Encryption(WEP):</td><td class="content_input_td"><input type="text" value="<% nvram_get_x("WLANConfig11b","WEPType"); %>" readonly></td>
</tr>
</table>
</td>
</tr>
<tr>
<td>
<table width="618" border="2" cellpadding="0" cellspacing="0" bordercolor="#E0E0E0">
<tr class="content_section_header_tr">
<td colspan="2">LAN Interface</td>
</tr>
<tr>
<td class="content_header_td" onMouseOver="return overlib('IP address for the Home Gateway:');" onMouseOut="return nd();">IP Address:</td><td class="content_input_td"><input type="text" value="<% nvram_get_x("LANHostConfig","IPRouters"); %>" readonly></td>
</tr>
<tr>
<td class="content_header_td" onMouseOver="return overlib('Subnet mask for the Home Gateway:');" onMouseOut="return nd();">Subnet Mask:</td><td class="content_input_td"><input type="text" value="<% nvram_get_x("LANHostConfig","SubnetMask"); %>" readonly></td>
</tr>
<tr>
<td class="content_header_td" onMouseOver="return overlib('Host name for the Home Gateway:');" onMouseOut="return nd();">Host Name:</td><td class="content_input_td"><input type="text" value="<% nvram_get_x("LANHostConfig","x_HostName"); %>" readonly></td>
</tr>
</table>
</td>
</tr>

<tr>
<td>
<table width="618" border="2" cellpadding="0" cellspacing="0" bordercolor="#E0E0E0">
<tr class="content_section_header_tr">
<td colspan="2">IP Connection</td>
</tr>
<tr>
<td class="content_header_td">WAN Port Link State:</td><td class="content_input_td"><input type="text" value="<% nvram_get_f("wan.log","IPLink"); %>" readonly></td>
</tr>
<tr>
<td class="content_header_td">WAN Port Connecting Time:</td><td class="content_input_td"><input type="text" value="<% nvram_get_f("wan.log","Uptime"); %>" readonly></td>
</tr>
<tr>
<td class="content_header_td" onMouseOver="return overlib('IP address of WAN Interface. If you leave it blank, WL600 will get IP address from DHCP Server automatically');" onMouseOut="return nd();">IP Address:</td><td class="content_input_td"><input type="text" value="<% nvram_get_x("IPConnection","ExternalIPAddress"); %>" readonly></td>
</tr>
<tr>
<td class="content_header_td">Subnet Mask:</td><td class="content_input_td"><input type="text" value="<% nvram_get_x("IPConnection","x_ExternalSubnetMask"); %>" readonly></td>
</tr>
<tr>
<td class="content_header_td">Gateway:</td><td class="content_input_td"><input type="text" value="<% nvram_get_x("IPConnection","x_ExternalGateway"); %>" readonly></td>
</tr>
</table>
</td>
</tr>

<tr>
<td>
<table width="618" border="2" cellpadding="0" cellspacing="0" bordercolor="#E0E0E0">
<tr class="content_section_header_tr">
<td colspan="2">PPP Connection</td>
</tr>
<tr>
<td class="content_header_td">WAN Port Link State:</td><td class="content_input_td"><input type="text" value="<% nvram_get_f("wan.log","PPPLink"); %>" readonly></td>
</tr>
<tr>
<td class="content_header_td" onMouseOver="return overlib('IP address of WAN Interface. If you leave it blank, WL600 will get IP address from DHCP Server automatically');" onMouseOut="return nd();">IP Address:</td><td class="content_input_td"><input type="text" value="<% nvram_get_x("PPPConnection","ExternalIPAddress"); %>" readonly></td>
</tr>
<tr>
<td class="content_header_td">Subnet Mask:</td><td class="content_input_td"><input type="text" value="<% nvram_get_x("PPPConnection","x_ExternalSubnetMask"); %>" readonly></td>
</tr>
<tr>
<td class="content_header_td">Gateway:</td><td class="content_input_td"><input type="text" value="<% nvram_get_x("PPPConnection","x_ExternalGateway"); %>" readonly></td>
</tr>
</table>
</td>
</tr>
<tr>
<td>
<table width="618" border="2" cellpadding="0" cellspacing="0" bordercolor="#E0E0E0">
<tr class="content_section_header_tr">
<td colspan="2">Printer Status</td>
</tr>
<tr>
<td class="content_header_td" onMouseOver="return overlib('Status of currently connected printer');" onMouseOut="return nd();">Connected Printer Status:</td><td class="content_input_td"><input type="text" value="<% nvram_get_f("printer.log","Status"); %>" readonly></td>
</tr>
<tr>
<td class="content_header_td" onMouseOver="return overlib('User currently use the printer');" onMouseOut="return nd();">User in service:</td><td class="content_input_td"><input type="text" value="<% nvram_get_f("printer.log","User"); %>" readonly></td>
</tr>
</table>
</td>
</tr>
<tr>
<td>		
<table width="618" border="1" cellpadding="0" cellspacing="0" bordercolor="#B0B0B0">
<tr bgcolor="#CCCCCC"><td colspan="3"><font face="arial" size="2">&nbsp;</font></td></tr>
<tr bgcolor="#FFFFFF">  
   <td height="25" width="67%">  
   </td>
   <td height="25">  
   <div align="center"><font face="Arial"> <input class=inputSubmit onMouseOut="buttonOut(this)" onMouseOver="buttonOver(this)" type="submit" value=" Refresh " name="action"></font></div> 
   </td>
</tr>
</table>
</td>
</tr>

</table>
</form>
</body>
</html>