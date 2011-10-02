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
<input type="hidden" name="current_page" value="Advanced_Samba_Content.asp">
<input type="hidden" name="next_page" value="Advanced_NFS_Content.asp">
<input type="hidden" name="next_host" value="">
<input type="hidden" name="sid_list" value="PrinterStatus;LANHostConfig;">
<input type="hidden" name="group_id" value="">
<input type="hidden" name="modified" value="0">
<input type="hidden" name="action_mode" value="">
<input type="hidden" name="first_time" value="">
<input type="hidden" name="action_script" value="">
<!-- Table for the conntent page -->	    
<table width="666" border="1" cellpadding="0" cellspacing="0" bordercolor="#E0E0E0">
<tr class="content_header_tr">
<td class="content_header_td_title" colspan="2">USB Application - Samba File Sharing</td>
</tr>

<tr>
<td class="content_header_td" onMouseOver="return overlib('Select network share mode for your USB disk.', LEFT);" onMouseOut="return nd();">Samba File Shares:</td>
<td class="content_input_td"><select name="usb_smbenable_x" class="content_input_fd" onChange="return change_common(this, '', 'usb_smbenable_x')">
<option class="content_input_fd" value="0" <% nvram_match_x("PrinterStatus","usb_smbenable_x","0","selected"); %>>Disabled</option>
<option class="content_input_fd" value="1" <% nvram_match_x("PrinterStatus","usb_smbenable_x","1","selected"); %>>First partition</option>
<option class="content_input_fd" value="2" <% nvram_match_x("PrinterStatus","usb_smbenable_x","2","selected"); %>>All partitions</option>
<option class="content_input_fd" value="3" <% nvram_match_x("PrinterStatus","usb_smbenable_x","3","selected"); %>>Manual share list</option>
</select></td>
</tr>

<tr>
<td class="content_header_td" onMouseOver="return overlib('Choose access level to Samba share', LEFT);" onMouseOut="return nd();">Default Access Level:</td>
<td class="content_input_td"><select name="usb_smbhidden_x" class="content_input_fd" onChange="return change_common(this, '', 'usb_smbhidden_x')">
<option class="content_input_fd" value="0" <% nvram_match_x("PrinterStatus","usb_smbhidden_x","0","selected"); %>>Read Only</option>
<option class="content_input_fd" value="2" <% nvram_match_x("PrinterStatus","usb_smbhidden_x","2","selected"); %>>Read/Write</option>
<option class="content_input_fd" value="1" <% nvram_match_x("PrinterStatus","usb_smbhidden_x","1","selected"); %>>Hidden Read/Write</option>
</select></td>
</tr>

<tr>
<td class="content_header_td" onMouseOver="return overlib('This field allows you to provide a LAN host name for ZVMODELVZ.', LEFT);" onMouseOut="return nd();">Host Name:
           </td><td class="content_input_td"><input type="text" maxlength="32" class="content_input_fd" size="32" name="lan_hostname" value="<% nvram_get_x("LANHostConfig","lan_hostname"); %>" onKeyPress="return is_string(event, this)" onBlur="validate_string(this)"></td>
</tr>

<tr>
<td class="content_header_td" onMouseOver="return overlib('Specifies workgroup name, which will be used by ZVMODELVZ to appear in.', LEFT);" onMouseOut="return nd();">Workgroup Name:</td><td class="content_input_td"><input type="text" maxlength="20" class="content_input_fd" size="32" name="usb_smbwrkgrp_x" value="<% nvram_get_x("PrinterStatus","usb_smbwrkgrp_x"); %>" onKeyPress="return is_string(event, this)" onBlur="validate_string(this)"></td>
</tr>

<tr>
<td class="content_header_td" onMouseOver="return overlib('Choose DOS code page (start cmd.exe and type chcp to see current code page)', LEFT);" onMouseOut="return nd();">DOS Codepage:</td>
<td class="content_input_td"><select name="usb_smbcpage_x" class="content_input_fd" onChange="return change_common(this, '', 'usb_smbcpage_x')">
<option class="content_input_fd" value="" <% nvram_match_x("PrinterStatus","usb_smbcpage_x","","selected"); %>>Unspecified</option>
<option class="content_input_fd" value="437" <% nvram_match_x("PrinterStatus","usb_smbcpage_x","437","selected"); %>>437 (United States, Canada)</option>
<option class="content_input_fd" value="850" <% nvram_match_x("PrinterStatus","usb_smbcpage_x","850","selected"); %>>850 (Western Europe)</option>
<option class="content_input_fd" value="852" <% nvram_match_x("PrinterStatus","usb_smbcpage_x","852","selected"); %>>852 (Central/Eastern Europe)</option>
<option class="content_input_fd" value="866" <% nvram_match_x("PrinterStatus","usb_smbcpage_x","866","selected"); %>>866 (Cyrillic/Russian)</option>
</select></td>
</tr>

</table>

<!-- 1 --><table width="666" border="2" cellpadding="0" cellspacing="0" bordercolor="#E0E0E0">
<tr class="content_list_header_tr">
<td class="content_list_header_td" width="60%" id="ACLList">Manual Network Shares List
         <input type="hidden" name="usb_smbnum_x_0" value="<% nvram_get_x("PrinterStatus", "usb_smbnum_x"); %>" readonly></td><td width="10%">
<div align="center">
<input class="inputSubmit" type="submit" onMouseOut="buttonOut(this)" onMouseOver="buttonOver(this)" onClick="return markGroup(this, 'x_SharesList', 32, ' Add ');" name="x_SharesList" value="Add" size="12">
</div>
</td><td width="10%">
<div align="center">
<input class="inputSubmit" type="submit" onMouseOut="buttonOut(this)" onMouseOver="buttonOver(this)" onClick="return markGroup(this, 'x_SharesList', 32, ' Del ');" name="x_SharesList" value="Del" size="12">
</div>
</td><td width="5%"></td>
</tr>
<!-- 2 --><table class="content_list_table" width="640" border="0" cellspacing="0" cellpadding="0">
<tr>
<td colspan="3" height="10"></td>
</tr>
<tr>
<td colspan="3">
<div align="center">
<!-- 3 --><table class="content_list_value_table" border="1" cellspacing="0" cellpadding="0">
<tr>
	<td></td>
	<td class="content_list_field_header_td" colspan="">Directory</td>
	<td class="content_list_field_header_td" colspan="">Share Name</td>
	<td class="content_list_field_header_td" colspan="">Access Level</td>
	<td class="content_list_field_header_td" colspan="">Description</td>
	<td></td>
</tr>
<tr>
	<td></td>
	<td class="content_list_input_td" colspan=""><input type="text" maxlength="80" class="content_input_list_fd" size="20" name="usb_smbdir_x_0" onKeyPress="return is_string(event, this)"></td>
	<td class="content_list_input_td" colspan=""><input type="text" maxlength="80" class="content_input_list_fd" size="20" name="usb_smbshare_x_0" onKeyPress="return is_string(event, this)"></td>
	<td class="content_list_input_td" colspan=""><select name="usb_smblevel_x_0" class="content_input_fd">
		<option class="content_input_fd" value="Read Only">Read Only</option>
		<option class="content_input_fd" value="Read/Write">Read/Write</option>
	</select></td>
	<td class="content_list_input_td" colspan=""><input type="text" maxlength="80" class="content_input_list_fd" size="20" name="usb_smbdesc_x_0" onKeyPress="return is_string(event, this)"></td>
</tr>

<tr>
<td></td><td colspan="10"><select size="8" name="x_SharesList_s" multiple="true" class="content_list_body">
<% nvram_get_table_x("PrinterStatus","x_SharesList"); %>
</select></td>
</tr>
<!-- 3 --></table>
</div>
</td>
</tr>
<tr>
<td colspan="3" height="10"></td>
</tr>
<!-- 2 --></table>
<!-- 1 --></table>
	
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
