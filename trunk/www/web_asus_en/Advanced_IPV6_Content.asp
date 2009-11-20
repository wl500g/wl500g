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
    	
<input type="hidden" name="current_page" value="Advanced_IPV6_Content.asp"><input type="hidden" name="next_page" value="SaveRestart.asp"><input type="hidden" name="next_host" value=""><input type="hidden" name="sid_list" value="IPv6Config;"><input type="hidden" name="group_id" value=""><input type="hidden" name="modified" value="0"><input type="hidden" name="action_mode" value=""><input type="hidden" name="first_time" value=""><input type="hidden" name="action_script" value="">
<tr>
<td>
<table width="666" border="1" cellpadding="0" cellspacing="0" bordercolor="E0E0E0">
<tr class="content_header_tr">
<td class="content_header_td_title" colspan="2">IP Config - IPv6</td>
</tr>
<tr class="content_section_header_tr">
<td class="content_section_header_td" colspan="2">LAN IPv6 Setting
            </td>
</tr>
<tr>
<td class="content_header_td" onMouseOver="return overlib('This is IPv6 Address of ZVMODELVZ as seen in your local network. If not specified, only auto-configured addresses will be seen.', LEFT);" onMouseOut="return nd();">Static IPv6 address:
           </td><td class="content_input_td"><input type="text" maxlength="40" class="content_input_fd" size="40" name="ipv6_lan_addr" value="<% nvram_get_x("IPv6Config","ipv6_lan_addr"); %>" onKeyPress="return is_string(this)" onBlur="validate_string(this)"></td>
</tr>
<tr>
<td class="content_header_td" onMouseOver="return overlib('This is the number of bits that make up the host part. Most common value is 64', LEFT);" onMouseOut="return nd();">Netsize (bits of hostpart):
           </td><td class="content_input_td"><input type="text" maxlength="3" class="content_input_fd" size="5" name="ipv6_lan_netsize" value="<% nvram_get_x("IPv6Config","ipv6_lan_netsize"); %>" onBlur="return validate_string(this)" onKeyPress="return is_string(this)"></td>
</tr>
<tr class="content_header_tr">
<td class="content_header_td" onMouseOver="return overlib('Router advertisements allow systems in your LAN to auto-configure them for IPv6-usage', LEFT);" onMouseOut="return nd();">Enable router advertisements:
           </td><td class="content_input_td"><input type="radio" value="1" name="ipv6_radvd_enable" class="content_input_fd" onClick="return change_common_radio(this, 'IPv6Config', 'ipv6_radvd_enable', '1')" <% nvram_match_x("IPv6Config","ipv6_radvd_enable","1", "checked"); %>>Yes</input><input type="radio" value="0" name="ipv6_radvd_enable" class="content_input_fd" onClick="return change_common_radio(this, 'IPv6Config', 'ipv6_radvd_enable', '0')" <% nvram_match_x("IPv6Config","ipv6_radvd_enable", "0", "checked"); %>>No</input></td>
</tr>

<tr class="content_section_header_tr">
<td class="content_section_header_td" colspan="2">WAN IPv6 Setting
           </td>
</tr>
<tr>
<td class="content_desc_td" colspan="2">Only use these values when your WAN-port is connected to a native IPv6-network. If you need a tunnel to connect to the public IPv6-network, leave these fields empty and configure the next section.
           </td>
</tr>
<tr class="content_header_tr">
<td class="content_header_td" onMouseOver="return overlib('This is IPv6 Address of ZVMODELVZ as seen on the WAN-port. If not specified, only auto-configured addresses will be seen.', LEFT);" onMouseOut="return nd();">Static IPv6 address:
           </td><td class="content_input_td"><input type="text" maxlength="40" class="content_input_fd" size="40" name="ipv6_wan_addr" value="<% nvram_get_x("IPv6Config","ipv6_wan_addr"); %>" onKeyPress="return is_string(this)" onBlur="validate_string(this)"></td>
</tr>
<tr class="content_header_tr">
<td class="content_header_td" onMouseOver="return overlib('This is the number of bits that make up the host part.', LEFT);" onMouseOut="return nd();">Netsize (bits of hostpart):
           </td><td class="content_input_td"><input type="text" maxlength="3" class="content_input_fd" size="5" name="ipv6_wan_netsize" value="<% nvram_get_x("IPv6Config","ipv6_wan_netsize"); %>" onBlur="return validate_string(this)" onKeyPress="return is_string(this)"></td>
</tr>
<tr class="content_header_tr">
<td class="content_header_td" onMouseOver="return overlib('The remote IPv6 gateway for the tunnel. All IPv6 traffic flowing out of the network will be directed to this host', LEFT);" onMouseOut="return nd();">Remote IPv6 gateway:
          </td><td class="content_input_td"><input type="text" maxlength="40" class="content_input_fd" size="40" name="ipv6_wan_router" value="<% nvram_get_x("IPv6Config","ipv6_wan_router"); %>" onKeyPress="return is_string(this)" onBlur="validate_string(this)">
	  </td>
</tr>
<tr class="content_section_header_tr">
<td class="content_section_header_td" colspan="2">Tunnel IPv6 Setting
           </td>
</tr>
<tr class="content_header_tr">
<td class="content_header_td" onMouseOver="return overlib('Enabling the IPv6-tunnel will allow you to connect to the public IPv6-internet', LEFT);" onMouseOut="return nd();">Enable IPv6-tunnel:
           </td><td class="content_input_td"><input type="radio" value="1" name="ipv6_sit_enable" class="content_input_fd" onClick="return change_common_radio(this, 'IPv6Config', 'ipv6_sit_enable', '1')" <% nvram_match_x("IPv6Config","ipv6_sit_enable","1", "checked"); %>>Yes</input><input type="radio" value="0" name="ipv6_sit_enable" class="content_input_fd" onClick="return change_common_radio(this, 'IPv6Config', 'ipv6_sit_enable', '0')" <% nvram_match_x("IPv6Config","ipv6_sit_enable", "0", "checked"); %>>No</input></td>
</tr>
<tr class="content_header_tr">
<td class="content_header_td" onMouseOver="return overlib('The remote IPv4 endpoint for the tunnel', LEFT);" onMouseOut="return nd();">Remote endpoint:
          </td><td class="content_input_td"><input type="text" maxlength="15" class="content_input_fd" size="15" name="ipv6_sit_remote" value="<% nvram_get_x("IPv6Config","ipv6_sit_remote"); %>" onKeyPress="return is_string(this)" onBlur="validate_string(this)">
	  </td>
</tr>
<tr class="content_header_tr">
<td class="content_header_td" onMouseOver="return overlib('The local IPv6 address for the tunnel', LEFT);" onMouseOut="return nd();">Local IPv6 address:
          </td><td class="content_input_td"><input type="text" maxlength="40" class="content_input_fd" size="40" name="ipv6_sit_localaddr" value="<% nvram_get_x("IPv6Config","ipv6_sit_localaddr"); %>" onKeyPress="return is_string(this)" onBlur="validate_string(this)">
	  </td>
</tr>
<tr class="content_header_tr">
<td class="content_header_td" onMouseOver="return overlib('The number of bits that make up the host part.', LEFT);" onMouseOut="return nd();">Netsize (bits of hostpart):
          </td><td class="content_input_td"><input type="text" maxlength="3" class="content_input_fd" size="5" name="ipv6_sit_netsize" value="<% nvram_get_x("IPv6Config","ipv6_sit_netsize"); %>" onKeyPress="return is_string(this)" onBlur="validate_string(this)">
          </td>
</tr>
<tr class="content_header_tr">
<td class="content_header_td" onMouseOver="return overlib('The remote IPv6 gateway for the tunnel. All IPv6 traffic flowing out of the network will be directed to this host', LEFT);" onMouseOut="return nd();">Remote IPv6 gateway:
          </td><td class="content_input_td"><input type="text" maxlength="40" class="content_input_fd" size="40" name="ipv6_sit_remoteaddr" value="<% nvram_get_x("IPv6Config","ipv6_sit_remoteaddr"); %>" onKeyPress="return is_string(this)" onBlur="validate_string(this)">
	  </td>
</tr>
<tr class="content_header_tr">
<td class="content_header_td" onMouseOver="return overlib('The Maximum Transfer Unit for the tunnel. Common value is 1280.', LEFT);" onMouseOut="return nd();">Tunnel MTU:
          </td><td class="content_input_td"><input type="text" maxlength="4" class="content_input_fd" size="5" name="ipv6_sit_mtu" value="<% nvram_get_x("IPv6Config","ipv6_sit_mtu"); %>" onKeyPress="return is_string(this)" onBlur="validate_string(this)">
	  </td>
</tr>
<tr class="content_header_tr">
<td class="content_header_td" onMouseOver="return overlib('The Time To Life for the tunnel. Common value is 64.', LEFT);" onMouseOut="return nd();">Tunnel TTL:
          </td><td class="content_input_td"><input type="text" maxlength="3" class="content_input_fd" size="5" name="ipv6_sit_ttl" value="<% nvram_get_x("IPv6Config","ipv6_sit_ttl"); %>" onKeyPress="return is_string(this)" onBlur="validate_string(this)">
          </td>
</tr>
</table>
</td>
</tr>
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
<table width="666" border="2" cellpadding="0" cellspacing="0" bordercolor="E0E0E0"></table>
</td>
</tr>
<tr>
<td>		
<table width="666" border="1" cellpadding="0" cellspacing="0" bordercolor="B0B0B0">
<tr bgcolor="#CCCCCC"><td colspan="3"><font face="arial" size="2"><b>&nbsp</b></font></td></tr>
<tr bgcolor="#FFFFFF">  
   <td id ="Confirm" height="25" width="34%">  
   <div align="center"><font face="Arial"> <input class=inputSubmit onMouseOut=buttonOut(this) onMouseOver="buttonOver(this)" onClick="onSubmitCtrl(this, ' Restore ')" type="submit" value=" Restore " name="action"></font></div> 
   </td>  
   <td height="25" width="33%">  
   <div align="center"><font face="Arial"> <input class=inputSubmit onMouseOut=buttonOut(this) onMouseOver="buttonOver(this)" onClick="onSubmitCtrl(this, ' Finish ')" type="submit" value=" Finish " name="action"></font></div> 
   </td>
   <td height="25" width="33%">  
   <div align="center"><font face="Arial"> <input class=inputSubmit onMouseOut=buttonOut(this) onMouseOver="buttonOver(this)" onClick="onSubmitCtrl(this, ' Apply ')" type="submit" value=" Apply " name="action"></font></div> 
   </td>    
</tr>
</table>
</td>
</tr>

<tr>
<td>
<table width="666" border="1" cellpadding="0" cellspacing="0" bordercolor="B0B0B0">
<tr>
    <td colspan="2" width="616" height="25" bgcolor="#FFBB00"></td> 
</tr>                   
<tr bgcolor="#FFFFFF">
    <td class="content_header_td_15" align="left">Restore: </td>
    <td class="content_input_td_padding" align="left">Clear the above settings and restore the settings in effect.</td>
</tr>
<tr bgcolor="#FFFFFF">
    <td class="content_header_td_15" align="left">Finish: </td>
    <td class="content_input_td_padding" align="left">Confirm all settings and restart ZVMODELVZ now.</td>
</tr>
<tr bgcolor="#FFFFFF">
    <td class="content_header_td_15" align="left">Apply: </td>
    <td class="content_input_td_padding" align="left">Confirm above settings and continue.</td>
</tr>
</table>
</td>
</tr>

</table>
</form>
</body>
