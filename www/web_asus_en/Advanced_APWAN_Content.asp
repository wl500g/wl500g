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
<input type="hidden" name="current_page" value="Advanced_APWAN_Content.asp">
<input type="hidden" name="next_page" value="SaveRestart.asp">
<input type="hidden" name="sid_list" value="IPConnection;">
<input type="hidden" name="group_id" value="">
<input type="hidden" name="modified" value="0">
<!-- Table for the conntent page -->	    

<table width="618" border="2" cellpadding="0" cellspacing="0" bordercolor="#E0E0E0">
<tr class="content_header_tr">
<td colspan="2">IP Config</td>
</tr>
<tr class="content_section_header_tr">
<td colspan="2">WAN IP Setting</td>
</tr>
<tr>
<td class="content_header_td" onMouseOver="return overlib('IP address of WAN Interface. If you leave it blank, WL600 will get IP address from DHCP Server automatically');" onMouseOut="return nd();">IP Address:</td><td class="content_input_td"><input type="text" maxlength="15" size="15" name="IPConnection_ExternalIPAddress" value="<% nvram_get("ExternalIPAddress"); %>" onBlur="return validate_ipaddr(this, 'ExternalIPAddress')" onKeyPress="return is_ipaddr(event, this)" onKeyUp="change_ipaddr(this)"></td>
</tr>
<tr>
<td class="content_header_td">Subnet Mask:</td><td class="content_input_td"><input type="text" maxlength="15" size="15" name="IPConnection_x_ExternalSubnetMask" value="<% nvram_get("x_ExternalSubnetMask"); %>" onBlur="return validate_ipaddr(this, 'x_ExternalSubnetMask')" onKeyPress="return is_ipaddr(event, this)" onKeyUp="change_ipaddr(this)"></td>
</tr>
<tr>
<td class="content_header_td">Gateway:</td><td class="content_input_td"><input type="text" maxlength="15" size="15" name="IPConnection_x_ExternalGateway" value="<% nvram_get("x_ExternalGateway"); %>" onBlur="return validate_ipaddr(this, 'x_ExternalGateway')" onKeyPress="return is_ipaddr(event, this)" onKeyUp="change_ipaddr(this)"></td>
</tr>
</table>

<table width="618" border="1" cellpadding="0" cellspacing="0" bordercolor="#B0B0B0">
<tr bgcolor="#CCCCCC"><td colspan="3"><font face="arial" size="2">&nbsp;</font></td></tr>
<tr bgcolor="#FFFFFF">  
   <td height="25" width="34%">  
   <div align="center"><font face="Arial"> <input class=inputSubmit onMouseOut="buttonOut(this)" onMouseOver="buttonOver(this)" type="submit" value="  Save  " name="action"></font></div> 
   </td>
   <td height="25" width="33%">  
   <div align="center"><font face="Arial"> <input class=inputSubmit onMouseOut="buttonOut(this)" onMouseOver="buttonOver(this)" type="submit" value=" Restore " name="action"></font></div> 
   </td>
   <td height="25" width="33%">  
   <div align="center"><font face="Arial"> <input class=inputSubmit onMouseOut="buttonOut(this)" onMouseOver="buttonOver(this)" type="submit" value=" Finish " name="action"></font></div> 
   </td>
</tr>
</table>

<table width="618" border="1" cellpadding="0" cellspacing="0" bordercolor="#B0B0B0">
<tr>
    <td colspan="2" width="616" height="25" bgcolor="#FFBB00"></td> 
</tr>                   
<tr bgcolor="#FFFFFF">
    <td align="left" height="25" width="12%">Save: </td>
    <td align="left" height="25" width="88%">Save the above settings and continue.</td>
</tr>
<tr bgcolor="#FFFFFF">
    <td align="left" height="25" width="12%">Restore: </td>
    <td align="left" height="25" width="88%">Clear the above settings and restore the settings in effect.</td>
</tr>
<tr bgcolor="#FFFFFF">
    <td align="left" height="25" width="12%">Finish: </td>
    <td align="left" height="25" width="88%">Finish all settings and restart the Home Gateway now.</td>
</tr>
</table>

</form>
</body>
</html>