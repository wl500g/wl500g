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
<input type="hidden" name="current_page" value="Advanced_MACFilter_Content.asp">
<input type="hidden" name="next_page" value="SaveRestart.asp">
<input type="hidden" name="next_host" value="">
<input type="hidden" name="sid_list" value="FirewallConfig;">
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
<td class="content_header_td_title" colspan="2">Internet Firewall - MAC Filter</td>
</tr>
<tr>
<td class="content_desc_td" colspan="2">MAC filter allows you to block packets from devices with specified mac address in your LAN.
         </td>
</tr>
<tr>
<td class="content_header_td">MAC Filter Mode:
           </td><td class="content_input_td"><select name="macfilter_enable_x" class="content_input_fd" onChange="return change_common(this, 'FirewallConfig', 'macfilter_enable_x')"><option class="content_input_fd" value="0" <% nvram_match("macfilter_enable_x", "0","selected"); %>>Disable</option><option class="content_input_fd" value="1" <% nvram_match("macfilter_enable_x", "1","selected"); %>>Accept</option><option class="content_input_fd" value="2" <% nvram_match("macfilter_enable_x", "2","selected"); %>>Reject</option></select></td>
</tr>
</table>
</td>
</tr>

<tr>
<td>
<table width="666" border="2" cellpadding="0" cellspacing="0" bordercolor="#E0E0E0">
<tr class="content_list_header_tr">
<td class="content_list_header_td" width="60%" id="MFList">MAC Filter List
         <input type="hidden" name="macfilter_num_x_0" value="<% nvram_get("macfilter_num_x"); %>" readonly></td><td width="10%">
<div align="center">
<input class="inputSubmit" type="submit" onMouseOut="buttonOut(this)" onMouseOver="buttonOver(this)" onClick="return markGroup(this, 'MFList', 16, ' Add ');" name="MFList" value="Add" size="12">
</div>
</td><td width="10%">
<div align="center">
<input class="inputSubmit" type="submit" onMouseOut="buttonOut(this)" onMouseOver="buttonOver(this)" onClick="return markGroup(this, 'MFList', 16, ' Del ');" name="MFList" value="Del" size="12">
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
<td></td><td class="content_list_input_td" colspan=""><input type="text" maxlength="12" class="content_input_list_fd" size="32" name="macfilter_list_x_0" onKeyPress="return is_hwaddr(event, this)"></td>
</tr>
<tr>
<td></td><td colspan="10"><select size="8" name="MFList_s" multiple="true" class="content_list_body">
<% nvram_get_table("MFList"); %>
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
</table>

<% include("footer_buttons.inc"); %>

</form>
</body>
</html>
