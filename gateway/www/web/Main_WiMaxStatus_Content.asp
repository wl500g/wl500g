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
<input type="hidden" name="current_page" value="Main_WiMaxStatus_Content.asp">
<input type="hidden" name="next_page" value="Main_WiMaxStatus_Content.asp">
<input type="hidden" name="next_host" value="">
<input type="hidden" name="sid_list" value="WLANConfig11a;WLANConfig11b;">
<input type="hidden" name="group_id" value="">
<input type="hidden" name="modified" value="0">
<input type="hidden" name="action_mode" value="">
<input type="hidden" name="first_time" value="">
<input type="hidden" name="action_script" value="">
<!-- Table for the conntent page -->
<table width="666" border="1" cellpadding="0" cellspacing="0" bordercolor="#E0E0E0">
<tr class="content_header_tr">
<td class="content_header_td_title" colspan="2">Status &amp; Log - WiMAX</td>
</tr>
<tr class="content_header_tr">
<td colspan="2"><textarea class="content_log_td" cols="63" rows="10" wrap="off" readonly>
<% nvram_dump("madwimax.log","madwimax.sh"); %>              
            </textarea></td>
</tr>
<tr class="content_section_header_tr">
<td class="content_section_header_td" colspan="2">WiMAX Interface</td>
</tr>
<tr>
<td class="content_header_td_less">Device:
           </td><td class="content_input_td"><input type="text" size="36" class="content_input_fd_ro" name="wan0_usb_device" value="<% nvram_get("wan0_usb_device"); %>" readonly></td>
</tr>
<tr>
<td class="content_header_td_less">IP Address:
           </td><td class="content_input_td"><input type="text" size="36" class="content_input_fd_ro" name="wan_ipaddr_t" value="<% nvram_get("wan_ipaddr_t"); %>" readonly></td>
</tr>
<tr>
<td class="content_header_td_less">Subnet Mask:
           </td><td class="content_input_td"><input type="text" size="36" class="content_input_fd_ro" name="wan_netmask_t" value="<% nvram_get("wan_netmask_t"); %>" readonly></td>
</tr>
<tr>
<td class="content_header_td_less">Gateway:
           </td><td class="content_input_td"><input type="text" size="36" class="content_input_fd_ro" name="wan_gateway_t" value="<% nvram_get("wan_gateway_t"); %>" readonly></td>
</tr>
<tr>
<td class="content_header_td_less">DNS Servers:
           </td><td class="content_input_td"><input type="text" size="36" class="content_input_fd_ro" name="wan_dns_t" value="<% nvram_get("wan_dns_t"); %>" readonly></td>
</tr>
<tr>
<td class="content_header_td_less">Ping time (min/avr/max):
           </td><td class="content_input_td"><input type="text" size="36" class="content_input_fd_ro" name="wan_wimax_ping_t" value="<% nvram_get("wan_wimax_ping_t"); %>" readonly></td>
</tr>
</table>
	
<table width="666" border="1" cellpadding="0" cellspacing="0" bordercolor="#B0B0B0">
<tr bgcolor="#CCCCCC"><td colspan="3"><font face="arial" size="2">&nbsp;</font></td></tr>
<tr bgcolor="#FFFFFF">  
   <td height="25" width="67%">  
   </td>
   <td height="25">  
   <div align="center"><font face="Arial"> <input class=inputSubmit onMouseOut="buttonOut(this)" onMouseOver="buttonOver(this)" onClick="onSubmitCtrl(this, ' Refresh ')" type="submit" value="Refresh" name="action"></font></div> 
   </td>
</tr>
</table>

</form>
</body>
</html>