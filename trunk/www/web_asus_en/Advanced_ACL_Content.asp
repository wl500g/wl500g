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
<input type="hidden" name="current_page" value="Advanced_ACL_Content.asp">
<input type="hidden" name="next_page" value="SaveRestart.asp">
<input type="hidden" name="next_host" value="">
<input type="hidden" name="sid_list" value="DeviceSecurity11a;">
<input type="hidden" name="group_id" value="">
<input type="hidden" name="modified" value="0">
<input type="hidden" name="action_mode" value="">
<input type="hidden" name="first_time" value="">
<input type="hidden" name="action_script" value="">
<!-- Table for the conntent page -->
<table width="666" border="1" cellpadding="0" cellspacing="0" bordercolor="#E0E0E0">
<tr class="content_header_tr">
<td class="content_header_td_title" colspan="2">Wireless - Access Control</td>
</tr>
<tr>
<td class="content_desc_td" colspan="2">Access Control allows you to block the access from certain wireless stations or to bypass access from certain wireless stations only. In Accept mode, ZVMODELVZ will only accept wireless access from stations with MAC address in the control list. In Reject mode, ZVMODELVZ will reject wireless access from stations with MAC address in the control list.
         </td>
</tr>
<tr>
<td class="content_header_td">MAC Access Mode:
           </td><td class="content_input_td"><select name="wl_macmode" class="content_input_fd" onChange="return change_common(this, 'DeviceSecurity11a', 'wl_macmode')"><option class="content_input_fd" value="disabled" <% nvram_match("wl_macmode", "disabled","selected"); %>>Disable</option><option class="content_input_fd" value="allow" <% nvram_match("wl_macmode", "allow","selected"); %>>Accept</option><option class="content_input_fd" value="deny" <% nvram_match("wl_macmode", "deny","selected"); %>>Reject</option></select></td>
</tr>
<tr>
<td class="content_header_td" onMouseOver="return overlib('This field indicates which devices will be under the control of ACL.', LEFT);" onMouseOut="return nd();">Applied Devices:
           </td><td class="content_input_td"><select name="wl_macapply_x" class="content_input_fd" onChange="return change_common(this, 'DeviceSecurity11a', 'wl_macapply_x')"><option class="content_input_fd" value="Both" <% nvram_match("wl_macapply_x", "Both","selected"); %>>Both</option><option class="content_input_fd" value="802.11a only" <% nvram_match("wl_macapply_x", "802.11a only","selected"); %>>802.11a only</option><option class="content_input_fd" value="802.11g only" <% nvram_match("wl_macapply_x", "802.11g only","selected"); %>>802.11g only</option></select></td>
</tr>
</table>

<table width="666" border="2" cellpadding="0" cellspacing="0" bordercolor="#E0E0E0">
<tr class="content_list_header_tr">
<td class="content_list_header_td" width="60%" id="ACLList">Access Control List
         <input type="hidden" name="wl_macnum_x_0" value="<% nvram_get("wl_macnum_x"); %>" readonly></td><td width="10%">
<div align="center">
<input class="inputSubmit" type="submit" onMouseOut="buttonOut(this)" onMouseOver="buttonOver(this)" onClick="return markGroup(this, 'ACLList', 64, ' Add ');" name="ACLList" value="Add" size="12">
</div>
</td><td width="10%">
<div align="center">
<input class="inputSubmit" type="submit" onMouseOut="buttonOut(this)" onMouseOver="buttonOver(this)" onClick="return markGroup(this, 'ACLList', 64, ' Del ');" name="ACLList" value="Del" size="12">
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
<td></td><td class="content_list_field_header_td" colspan="">MAC Address	                
           	        </td><td></td>
</tr>
<tr>
<td></td><td class="content_list_input_td" colspan=""><input type="text" maxlength="12" class="content_input_list_fd" size="32" name="wl_maclist_x_0" onKeyPress="return is_hwaddr(event, this)"></td>
</tr>
<tr>
<td></td><td colspan="10"><select size="8" name="ACLList_s" multiple="true" class="content_list_body">
<% nvram_get_table("ACLList"); %>
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

<% include("footer_buttons.inc"); %>

</form>
</body>
</html>
