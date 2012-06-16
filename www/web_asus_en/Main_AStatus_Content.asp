<html>
<head>
<title>ZVMODELVZ Web Manager</title>
<meta http-equiv="Content-Type" content="text/html; charset=UTF-8">
<link rel="stylesheet" type="text/css" href="style.css" media="screen">
<script type="text/javascript" src="overlib.js"></script>
<script type="text/javascript" src="general.js"></script>
</head>  
<body onLoad="load_body()" onunLoad="return unload_body();">
<div id="overDiv" style="position:absolute; visibility:hidden; z-index:1000;"></div>    
<form method="GET" name="form" action="apply.cgi">
<input type="hidden" name="current_page" value="Main_AStatus_Content.asp">
<input type="hidden" name="next_page" value="default value">
<input type="hidden" name="next_host" value="">
<input type="hidden" name="sid_list" value="default value">
<input type="hidden" name="group_id" value="">
<input type="hidden" name="modified" value="0">
<input type="hidden" name="action_mode" value="">
<input type="hidden" name="first_time" value="">
<input type="hidden" name="action_script" value="">
<input type="hidden" name="load_script" value="<% load_script("printer.sh"); %>">
<!-- Table for the conntent page -->	    
<table width="666" border="1" cellpadding="0" cellspacing="0" bordercolor="#E0E0E0">
<tr class="content_header_tr">
<td class="content_header_td_title" colspan="2">Status &amp; Log - Status</td>
</tr>
<tr>
<td class="content_header_td_less" onMouseOver="return overlib('Elapsed time since system boot', LEFT);" onMouseOut="return nd();">System Time:
           </td><td class="content_input_td"><input type="text" size="48" class="content_input_fd" value="<% uptime(); %>" readonly></td>
</tr>

<tr class="content_section_header_tr">
<td class="content_section_header_td" colspan="2">Printer</td>
</tr>
<tr>
<td class="content_header_td_less" onMouseOver="return overlib('Model of printer connect to ZVMODELVZ.', LEFT);" onMouseOut="return nd();">Printer Model:
           </td><td class="content_input_td"><input type="text" size="36" class="content_input_fd" name="" value="<% nvram_get("printer_model_t"); %>" readonly></td>
</tr>
<tr>
<td class="content_header_td_less" onMouseOver="return overlib('Current status of printer', LEFT);" onMouseOut="return nd();">Printer Status:
           </td><td class="content_input_td"><input type="text" size="36" class="content_input_fd" name="" value="<% nvram_get("printer_status_t"); %>" readonly></td>
</tr>
<tr>
<td class="content_header_td_less" onMouseOver="return overlib('IP address of user using this printer.', LEFT);" onMouseOut="return nd();">User:
           </td><td class="content_input_td"><input type="text" size="36" class="content_input_fd" name="" value="<% nvram_get("printer_user_t"); %>" readonly></td>
</tr>
<tr>
<td class="content_header_td_less">Action:
           </td><td class="content_input_td"><input type="submit" maxlength="15" class="content_input_fd_ro" onClick="return onSubmitApply('lpr_remove')" size="12" name="PrinterStatus_x_PrinterAction_button" value="Remove"></td>
</tr>

<tr class="content_section_header_tr">
<td class="content_section_header_td" colspan="2">LAN Interface</td>
</tr>
<tr>
<td class="content_header_td_less">IP Address:
           </td><td class="content_input_td"><input type="text" size="36" class="content_input_fd" name="lan_ipaddr_t" value="<% nvram_get("lan_ipaddr_t"); %>" readonly></td>
</tr>
<tr>
<td class="content_header_td_less">Subnet Mask:
           </td><td class="content_input_td"><input type="text" size="36" class="content_input_fd" name="lan_netmask_t" value="<% nvram_get("lan_netmask_t"); %>" readonly></td>
</tr>
<tr>
<td class="content_header_td_less">Default Gateway
           </td><td class="content_input_td"><input type="text" size="36" class="content_input_fd" name="lan_gateway_t" value="<% nvram_get("lan_gateway_t"); %>" readonly></td>
</tr>
</table>

<table width="666" border="1" cellpadding="0" cellspacing="0" bordercolor="#B0B0B0">
<tr bgcolor="#CCCCCC"><td colspan="2"><font face="arial" size="2">&nbsp;</font></td></tr>
<tr bgcolor="#FFFFFF">  
   <td height="25" width="75%">  
   </td>
   <td height="25">  
   <div align="center"><font face="Arial"> <input class=inputSubmit onMouseOut="buttonOut(this)" onMouseOver="buttonOver(this)" onClick="onSubmitCtrl(this, ' Refresh ')" type="submit" value="Refresh" name="action"></font></div> 
   </td>
</tr>
</table>

</form>
</body>
</html>