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
<input type="hidden" name="current_page" value="Advanced_PortTrigger_Content.asp">
<input type="hidden" name="next_page" value="Advanced_VirtualServer_Content.asp">
<input type="hidden" name="next_host" value="">
<input type="hidden" name="sid_list" value="IPConnection;PPPConnection;">
<input type="hidden" name="group_id" value="">
<input type="hidden" name="modified" value="0">
<input type="hidden" name="action_mode" value="">
<input type="hidden" name="first_time" value="">
<input type="hidden" name="action_script" value="">
<!-- Table for the conntent page -->
<table width="666" border="1" cellpadding="0" cellspacing="0" bordercolor="#E0E0E0">
<tr class="content_header_tr">
<td class="content_header_td_title" colspan="2">NAT Setting - Port Trigger</td>
</tr>
<tr>
<td class="content_desc_td" colspan="2">Port Trigger function allows you to open certain TCP or UDP ports to communicate with the computers connected to ZVMODELVZ. This is done by defining trigger ports and incoming ports. When the trigger port is detected, the inbound packets to the specified incoming port numbers are redirected to your computer.
         </td>
</tr>
<tr>
<td class="content_header_td">Enable Port Trigger?
           </td><td class="content_input_td"><input type="radio" value="1" name="autofw_enable_x" class="content_input_fd" onClick="return change_common_radio(this, 'IPConnection', 'autofw_enable_x', '1')" <% nvram_match_x("IPConnection","autofw_enable_x", "1", "checked"); %>>Yes</input><input type="radio" value="0" name="autofw_enable_x" class="content_input_fd" onClick="return change_common_radio(this, 'IPConnection', 'autofw_enable_x', '0')" <% nvram_match_x("IPConnection","autofw_enable_x", "0", "checked"); %>>No</input></td>
</tr>
</table>

<table width="666" border="2" cellpadding="0" cellspacing="0" bordercolor="#E0E0E0">
<tr class="content_list_header_tr">
<td class="content_list_header_td" width="60%" id="TriggerList">Trigger Port List
         <input type="hidden" name="autofw_num_x_0" value="<% nvram_get_x("IPConnection", "autofw_num_x"); %>" readonly></td><td width="10%">
<div align="center">
<input class="inputSubmit" type="submit" onMouseOut="buttonOut(this)" onMouseOver="buttonOver(this)" onClick="return markGroup(this, 'TriggerList', 10, ' Add ');" name="TriggerList" value="Add" size="12">
</div>
</td><td width="10%">
<div align="center">
<input class="inputSubmit" type="submit" onMouseOut="buttonOut(this)" onMouseOver="buttonOver(this)" onClick="return markGroup(this, 'TriggerList', 10, ' Del ');" name="TriggerList" value="Del" size="12">
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
<td></td><td class="content_list_field_header_td" colspan="3">Well-Known Applications:         
         </td><td class="content_list_field_header_td" colspan="2"><select name="TriggerKnownApps" class="content_input_fd" onChange="change_widzard(this, 'TriggerKnownApps');"><option value="User Defined">User Defined</option></select></td>
</tr>
<tr>
<td></td><td class="content_list_field_header_td" colspan="">Trigger Port	                
           	        </td><td class="content_list_field_header_td" colspan="">Protocol	                
           	        </td><td class="content_list_field_header_td" colspan="">Incoming Port	                
           	        </td><td class="content_list_field_header_td" colspan="">Protocol	                
           	        </td><td class="content_list_field_header_td" colspan="">Description	                
           	        </td><td></td>
</tr>
<tr>
<td></td><td class="content_list_input_td" colspan=""><input type="text" maxlength="11" class="content_input_list_fd" size="10" name="autofw_outport_x_0" value="" onKeyPress="return is_portrange(event, this)"></td><td class="content_list_input_td" colspan=""><select name="autofw_outproto_x_0" class="content_input_list_fd"><option value="TCP" <% nvram_match_list_x("IPConnection","autofw_outproto_x", "TCP","selected", 0); %>>TCP</option><option value="UDP" <% nvram_match_list_x("IPConnection","autofw_outproto_x", "UDP","selected", 0); %>>UDP</option></select></td><td class="content_list_input_td" colspan=""><input type="text" maxlength="11" class="content_input_list_fd" size="10" name="autofw_inport_x_0" value="" onKeyPress="return is_portrange(event, this)"></td><td class="content_list_input_td" colspan=""><select name="autofw_inproto_x_0" class="content_input_list_fd"><option value="TCP" <% nvram_match_list_x("IPConnection","autofw_inproto_x", "TCP","selected", 0); %>>TCP</option><option value="UDP" <% nvram_match_list_x("IPConnection","autofw_inproto_x", "UDP","selected", 0); %>>UDP</option></select></td><td class="content_list_input_td" colspan=""><input type="text" maxlength="18" size="18" name="autofw_desc_x_0" class="content_input_list_fd" onKeyPress="return is_string(event, this)"></td>
</tr>
<tr>
<td></td><td colspan="10"><select size="8" name="TriggerList_s" multiple="true" class="content_list_body">
<% nvram_get_table_x("IPConnection","TriggerList"); %>
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
		
<table width="666" border="1" cellpadding="0" cellspacing="0" bordercolor="#B0B0B0">
<tr bgcolor="#CCCCCC"><td colspan="3"><font face="arial" size="2">&nbsp;</font></td></tr>
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

</form>
</body>
</html>
