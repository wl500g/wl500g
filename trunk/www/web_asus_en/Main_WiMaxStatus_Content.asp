<head>
<title>ZVMODELVZ Web Manager</title>
<meta http-equiv="Content-Type" content="text/html; charset=ISO-8859-1">
<meta http-equiv="Content-Type" content="text/html; charset=UTF-8">
<meta http-equiv="Content-Type" content="text/html; charset=gb2312">
<link rel="stylesheet" type="text/css" href="style.css" media="screen"></link>
<script language="JavaScript" type="text/javascript" src="overlib.js"></script>
<script language="JavaScript" type="text/javascript" src="general.js"></script>
</head>  
<div id="overDiv" style="position:absolute; visibility:hidden; z-index:1000;"></div>    
<body onLoad="load_body()" onunLoad="return unload_body();">
<form method="GET" name="form" action="apply.cgi">
<!-- Table for the conntent page -->	    

<table width="666" border="0" cellpadding="0" cellspacing="0">     	      
    	
<input type="hidden" name="current_page" value="Main_WiMaxStatus_Content.asp">
<input type="hidden" name="next_page" value="Main_WiMaxStatus_Content.asp">
<input type="hidden" name="next_host" value="">
<input type="hidden" name="sid_list" value="WLANConfig11a;WLANConfig11b;">
<input type="hidden" name="group_id" value="">
<input type="hidden" name="modified" value="0">
<input type="hidden" name="action_mode" value="">
<input type="hidden" name="first_time" value="">
<input type="hidden" name="action_script" value="">

<tr>
<td>
<table width="666" border="1" cellpadding="0" cellspacing="0" bordercolor="E0E0E0">
<tr class="content_header_tr">
<td class="content_header_td_title" colspan="2">Status & Log - WiMAX</td>
</tr>
<tr class="content_header_tr">
<td colspan="2"><textarea class="content_log_td" cols="63" rows="10" wrap="off" readonly="1">
<% nvram_dump("madwimax.log","madwimax.sh"); %>              
            </textarea></td>
</tr>
</table>
</td>
</tr>
<table width="666" border="1" cellpadding="0" cellspacing="0" bordercolor="E0E0E0">
<tr class="content_section_header_tr">
<td class="content_section_header_td" colspan="2">WiMAX Interface</td>
</tr>
<tr>
<td class="content_header_td_less">Device:
           </td><td class="content_input_td"><input type="text" size="36" class="content_input_fd_ro" name="wmx_device" value="<% nvram_get_f("wan.log","wimax_device"); %>" readonly="1"></td>
</tr>
<tr>
<td class="content_header_td_less">Interface name:
           </td><td class="content_input_td"><input type="text" size="36" class="content_input_fd_ro" name="wmx_ifname_t" value="<% nvram_get_f("wan.log","wmx_ifname_t"); %>" readonly="1"></td>
</tr>
<tr>
<td class="content_header_td_less">IP Address:
           </td><td class="content_input_td"><input type="text" size="36" class="content_input_fd_ro" name="wmx_ipaddr_t" value="<% nvram_get_f("wan.log","wmx_ipaddr_t"); %>" readonly="1"></td>
</tr>
<tr>
<td class="content_header_td_less">Subnet Mask:
           </td><td class="content_input_td"><input type="text" size="36" class="content_input_fd_ro" name="wmx_netmask_t" value="<% nvram_get_f("wan.log","wmx_netmask_t"); %>" readonly="1"></td>
</tr>
<tr>
<td class="content_header_td_less">Gateway:
           </td><td class="content_input_td"><input type="text" size="36" class="content_input_fd_ro" name="wmx_gateway" value="<% nvram_get_f("wan.log","wmx_gateway_t"); %>" readonly="1"></td>
</tr>
<tr>
<td class="content_header_td_less">DNS Servers:
           </td><td class="content_input_td"><input type="text" size="36" class="content_input_fd_ro" name="wmx_dns_t" value="<% nvram_get_f("wan.log","wmx_dns_t"); %>" readonly="1"></td>
</tr>
<tr>
<td class="content_header_td_less">Ping time (min/avr/max):
           </td><td class="content_input_td"><input type="text" size="36" class="content_input_fd_ro" name="wmx_ping_t" value="<% nvram_get_f("wan.log","wmx_ping_t"); %>" readonly="1"></td>
</tr>
</table>

<tr>
<td>
<table width="666" border="2" cellpadding="0" cellspacing="0" bordercolor="E0E0E0"></table>
</td>
</tr>

<tr>
<td>
<table width="666" border="2" cellpadding="0" cellspacing="0" bordercolor="E0E0E0"></table>
</td>
</tr>

<tr>
<td>		
<table width="666" border="1" cellpadding="0" cellspacing="0" bordercolor="B0B0B0">
<tr bgcolor="#CCCCCC"><td colspan="3"><font face="arial" size="2"><b>&nbsp</b></font></td></tr>
<tr bgcolor="#FFFFFF">  
   <td height="25" width="34%">  
   </td>
   <td height="25" width="33%">  
   </td>
   <td height="25" width="33%">  
   <div align="center"><font face="Arial"> <input class=inputSubmit onMouseOut=buttonOut(this) onMouseOver="buttonOver(this)" onClick="onSubmitCtrl(this, ' Refresh ')" type="submit" value="Refresh" name="action"></font></div> 
   </td>
</tr>
</table>
</td>
</tr>

</table>
</form>
</body>
