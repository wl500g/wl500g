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
<input type="hidden" name="current_page" value="Advanced_GWStaticRoute_Content.asp">
<input type="hidden" name="next_page" value="Advanced_DDNS_Content.asp">
<input type="hidden" name="next_host" value="">
<input type="hidden" name="sid_list" value="RouterConfig;">
<input type="hidden" name="group_id" value="">
<input type="hidden" name="modified" value="0">
<input type="hidden" name="action_mode" value="">
<input type="hidden" name="first_time" value="">
<input type="hidden" name="action_script" value="">
<!-- Table for the conntent page -->	    
<table width="666" border="0" cellpadding="0" cellspacing="0">     	      
<tr>
<td>
<table width="666" border="1" cellpadding="0" cellspacing="0" bordercolor="#E0E0E0">
<tr class="content_header_tr">
<td class="content_header_td_title" colspan="2">IP Config - Route</td>
</tr>
<tr>
<td class="content_desc_td" colspan="2">This function allows you to add routing rules into ZVMODELVZ. It is useful if you connect several routers behind ZVMODELVZ to share the same connection to Internet.
         </td>
</tr>
<tr>
<tr>
<td class="content_header_td">Use DHCP routes?
           </td><td class="content_input_td"><input type="radio" value="1" name="dr_enable_x" class="content_input_fd" onClick="return change_common_radio(this, 'RouterConfig', 'dr_enable_x', '1')" <% nvram_match_x("RouterConfig","dr_enable_x", "1", "checked"); %>>Yes</input><input type="radio" value="0" name="dr_enable_x" class="content_input_fd" onClick="return change_common_radio(this, 'RouterConfig', 'dr_enable_x', '0')" <% nvram_match_x("RouterConfig","dr_enable_x", "0", "checked"); %>>No</input></td>
</tr>
<td class="content_header_td">Enable multicast routing?
           </td><td class="content_input_td"><input type="radio" value="1" name="mr_enable_x" class="content_input_fd" onClick="return change_common_radio(this, 'RouterConfig', 'mr_enable_x', '1')" <% nvram_match_x("RouterConfig","mr_enable_x", "1", "checked"); %>>Yes</input><input type="radio" value="0" name="mr_enable_x" class="content_input_fd" onClick="return change_common_radio(this, 'RouterConfig', 'mr_enable_x', '0')" <% nvram_match_x("RouterConfig","mr_enable_x", "0", "checked"); %>>No</input></td>
</tr>
<td class="content_header_td">Enable static routes?
           </td><td class="content_input_td"><input type="radio" value="1" name="sr_enable_x" class="content_input_fd" onClick="return change_common_radio(this, 'RouterConfig', 'sr_enable_x', '1')" <% nvram_match_x("RouterConfig","sr_enable_x", "1", "checked"); %>>Yes</input><input type="radio" value="0" name="sr_enable_x" class="content_input_fd" onClick="return change_common_radio(this, 'RouterConfig', 'sr_enable_x', '0')" <% nvram_match_x("RouterConfig","sr_enable_x", "0", "checked"); %>>No</input></td>
</tr>
</table>
</td>
</tr>
<tr>
<td>
<table width="666" border="2" cellpadding="0" cellspacing="0" bordercolor="#E0E0E0">
<tr class="content_list_header_tr">
<td class="content_list_header_td" width="60%" id="GWStatic">Static Route List
         <input type="hidden" name="sr_num_x_0" value="<% nvram_get_x("RouterConfig", "sr_num_x"); %>" readonly></td><td width="10%">
<div align="center">
<input class="inputSubmit" type="submit" onMouseOut="buttonOut(this)" onMouseOver="buttonOver(this)" onClick="return markGroup(this, 'GWStatic', 16, ' Add ');" name="GWStatic" value="Add" size="12">
</div>
</td><td width="10%">
<div align="center">
<input class="inputSubmit" type="submit" onMouseOut="buttonOut(this)" onMouseOver="buttonOver(this)" onClick="return markGroup(this, 'GWStatic', 16, ' Del ');" name="GWStatic" value="Del" size="12">
</div>
</td><td width="10%">
<div align="center">
<input class="inputSubmit" type="button" onMouseOut="buttonOut(this)" onMouseOver="buttonOver(this)" onClick="return openHelp(this, 'GRouterHelp');" name="GWStatic" value="Help" size="12">
</div>
</td><td width="5%"></td>
</tr>
<table class="content_list_table" width="640" border="0" cellspacing="0" cellpadding="0">
<tr>
<td colspan="3" height="10"></td>
</tr>
<tr>
<td colspan="3">
<div align="center">
<table class="content_list_value_table" border="1" cellspacing="0" cellpadding="0">
<tr>
<td></td><td class="content_list_field_header_td" colspan="">Network/Host IP	                
           	        </td><td class="content_list_field_header_td" colspan="">Netmask	                
           	        </td><td class="content_list_field_header_td" colspan="">Gateway	                
           	        </td><td class="content_list_field_header_td" colspan="">Metric	                
           	        </td><td class="content_list_field_header_td" colspan="">Interface	                
           	        </td><td></td>
</tr>
<tr>
<td></td><td class="content_list_input_td" colspan=""><input type="text" maxlength="15" class="content_input_list_fd" size="15" name="sr_ipaddr_x_0" onKeyPress="return is_ipaddr(this)" onKeyUp="change_ipaddr(this)"></td><td class="content_list_input_td" colspan=""><input type="text" maxlength="15" class="content_input_list_fd" size="15" name="sr_netmask_x_0" onKeyPress="return is_ipaddr(this)" onKeyUp="change_ipaddr(this)"></td><td class="content_list_input_td" colspan=""><input type="text" maxlength="15" class="content_input_list_fd" size="15" name="sr_gateway_x_0" onKeyPress="return is_ipaddr(this)" onKeyUp="change_ipaddr(this)"></td><td class="content_list_input_td" colspan=""><input type="text" maxlength="15" class="content_input_list_fd" size="3" name="sr_matric_x_0" onKeyPress="return is_ipaddr(this)" onKeyUp="change_ipaddr(this)"></td><td class="content_list_input_td" colspan=""><select name="sr_if_x_0" class="content_input_list_fd"><option value="LAN" <% nvram_match_list_x("RouterConfig","sr_if_x", "LAN","selected", 0); %>>LAN</option><option value="MAN" <% nvram_match_list_x("RouterConfig","sr_if_x", "MAN","selected", 0); %>>MAN</option><option value="WAN" <% nvram_match_list_x("RouterConfig","sr_if_x", "WAN","selected", 0); %>>WAN</option></select></td>
</tr>
<tr>
<td></td><td colspan="10"><select size="16" name="GWStatic_s" multiple="true" class="content_list_body">
<% nvram_get_table_x("RouterConfig","GWStatic"); %>
</select></td>
</tr>
</table>
</div>
</td>
</tr>
<tr>
<td colspan="3" height="10"></td>
</tr>
</table>
</table>
</td>
</tr>

<tr>
<td>		
<table width="666" border="1" cellpadding="0" cellspacing="0" bordercolor="#B0B0B0">
<tr bgcolor="#CCCCCC"><td colspan="3"><font face="arial" size="2"><b>&nbsp;</b></font></td></tr>
<tr bgcolor="#FFFFFF">  
   <td id ="Confirm" height="25" width="34%">  
   <div align="center"><font face="Arial"> <input class=inputSubmit onMouseOut="buttonOut(this)" onMouseOver="buttonOver(this)" onClick="onSubmitCtrl(this, ' Restore ')" type="submit" value=" Restore " name="action"></font></div> 
   </td>  
   <td height="25" width="33%">  
   <div align="center"><font face="Arial"> <input class=inputSubmit onMouseOut="buttonOut(this)" onMouseOver="buttonOver(this)" onClick="onSubmitCtrl(this, ' Finish ')" type="submit" value=" Finish " name="action"></font></div> 
   </td>
   <td height="25" width="33%">  
   <div align="center"><font face="Arial"> <input class=inputSubmit onMouseOut="buttonOut(this)" onMouseOver="buttonOver(this)" onClick="onSubmitCtrl(this, ' Apply ')" type="submit" value=" Apply " name="action"></font></div> 
   </td>    
</tr>
</table>
</td>
</tr>

<tr>
<td>
<table width="666" border="1" cellpadding="0" cellspacing="0" bordercolor="#B0B0B0">
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
</html>
