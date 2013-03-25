<html>
<head>
<title><% model(2); %></title>
<meta http-equiv="Content-Type" content="text/html; charset=UTF-8">
<link rel="stylesheet" type="text/css" href="style.css" media="screen">
<script type="text/javascript" src="overlib.js"></script>
<script type="text/javascript" src="general.js"></script>
</head>  
<body onLoad="load_body()" onunLoad="return unload_body();">
<div id="overDiv" style="position:absolute; visibility:hidden; z-index:1000;"></div>    
<form method="GET" name="form" action="apply.cgi">
<input type="hidden" name="current_page" value="Main_GStatus_Content.asp">
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
<td class="content_header_td_less">Router model:
           </td><td class="content_input_td"><input type="text" size="48" class="content_input_fd" value="<% model(2); %>" readonly></td>
</tr>
<tr>
<td class="content_header_td_less" onMouseOver="return overlib('Elapsed time since system boot', LEFT);" onMouseOut="return nd();">System Time:
           </td><td class="content_input_td"><input type="text" size="48" class="content_input_fd" value="<% uptime(); %>" readonly></td>
</tr>

<tr class="content_section_header_tr">
<td class="content_section_header_td" colspan="2">WAN Interface</td>
</tr>
<tr>
<td class="content_header_td_less">WAN Type:
           </td><td class="content_input_td"><input type="text" size="36" class="content_input_fd" name="wan_proto_t" value="<% nvram_get("wan_proto_t"); %>" readonly></td>
</tr>
<tr>
<td class="content_header_td_less">IP Address:
           </td><td class="content_input_td"><input type="text" size="36" class="content_input_fd" name="wan_ipaddr_t" value="<% nvram_get("wan_ipaddr_t"); %>" readonly></td>
</tr>
<tr>
<td class="content_header_td_less">Subnet Mask:
           </td><td class="content_input_td"><input type="text" size="36" class="content_input_fd" name="wan_netmask_t" value="<% nvram_get("wan_netmask_t"); %>" readonly></td>
</tr>
<tr>
<td class="content_header_td_less">Gateway:
           </td><td class="content_input_td"><input type="text" size="36" class="content_input_fd" name="wan_gateway_t" value="<% nvram_get("wan_gateway_t"); %>" readonly></td>
</tr>
<tr>
<td class="content_header_td_less">DNS Servers:
           </td><td class="content_input_td"><input type="text" size="36" class="content_input_fd" name="wan_dns_t" value="<% nvram_get("wan_dns_t"); %>" readonly></td>
</tr>
<tr>
<td class="content_header_td_less">Link Status:
           </td><td class="content_input_td"><input type="text" size="36" class="content_input_fd" name="wan_status_t" value="<% nvram_get("wan_status_t"); %>" readonly></td>
</tr>
<tr>
<td class="content_header_td_less">Action:
           </td><td class="content_input_td"><input type="submit" maxlength="15" class="content_input_fd_ro" onClick="return onSubmitApply('dhcpc_release')" size="12" name="PPPConnection_x_WANAction_button" value="Disconnect"><input type="submit" maxlength="15" class="content_input_fd_ro" onClick="return onSubmitApply('dhcpc_renew')" size="12" name="PPPConnection_x_WANAction_button1" value="Connect"></td>
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