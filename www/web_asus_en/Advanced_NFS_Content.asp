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
<input type="hidden" name="current_page" value="Advanced_NFS_Content.asp">
<input type="hidden" name="next_page" value="Advanced_WebCam_Content.asp">
<input type="hidden" name="next_host" value="">
<input type="hidden" name="sid_list" value="PrinterStatus;">
<input type="hidden" name="group_id" value="">
<input type="hidden" name="modified" value="0">
<input type="hidden" name="action_mode" value="">
<input type="hidden" name="first_time" value="">
<input type="hidden" name="action_script" value="">
<!-- Table for the conntent page -->	    
<table width="666" border="1" cellpadding="0" cellspacing="0" bordercolor="#E0E0E0">
<tr class="content_header_tr">
<td class="content_header_td_title" colspan="2">USB Application - NFS Server</td>
</tr>
<tr>
<td class="content_header_td" onMouseOver="return overlib('Selecting Yes enables NFS Server.', LEFT);" onMouseOut="return nd();">Enable NFS Server?
           </td><td class="content_input_td"><input type="radio" value="1" name="usb_nfsenable_x" class="content_input_fd" onClick="return change_common_radio(this, 'PrinterStatus', 'usb_nfsenable_x', '1')" <% nvram_match_x("PrinterStatus","usb_nfsenable_x", "1", "checked"); %>>Yes</input><input type="radio" value="0" name="usb_nfsenable_x" class="content_input_fd" onClick="return change_common_radio(this, 'PrinterStatus', 'usb_nfsenable_x', '0')" <% nvram_match_x("PrinterStatus","usb_nfsenable_x", "0", "checked"); %>>No</input></td>
</tr>
</table>

<!-- 1 --><table width="666" border="2" cellpadding="0" cellspacing="0" bordercolor="#E0E0E0">
<tr class="content_list_header_tr">
<td class="content_list_header_td" width="60%" id="ACLList">NFS Server Exports
         <input type="hidden" name="usb_nfsnum_x_0" value="<% nvram_get_x("PrinterStatus", "usb_nfsnum_x"); %>" readonly></td><td width="10%">
<div align="center">
<input class="inputSubmit" type="submit" onMouseOut="buttonOut(this)" onMouseOver="buttonOver(this)" onClick="return markGroup(this, 'x_ExportsList', 32, ' Add ');" name="x_ExportsList" value="Add" size="12">
</div>
</td><td width="10%">
<div align="center">
<input class="inputSubmit" type="submit" onMouseOut="buttonOut(this)" onMouseOver="buttonOver(this)" onClick="return markGroup(this, 'x_ExportsList', 32, ' Del ');" name="x_ExportsList" value="Del" size="12">
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
<td></td><td class="content_list_field_header_td" colspan="">Export	                
           	        </td><td></td>
</tr>
<tr>
<td></td><td class="content_list_input_td" colspan=""><input type="text" maxlength="160" class="content_input_list_fd" size="80" name="usb_nfslist_x_0" onKeyPress="return is_string(event, this)"></td>
</tr>
<tr>
<td></td><td colspan="10"><select size="8" name="x_ExportsList_s" multiple="true" class="content_list_body">
<% nvram_get_table_x("PrinterStatus","x_ExportsList"); %>
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

<% include("footer_buttons.inc"); %>

</form>
</body>
</html>
