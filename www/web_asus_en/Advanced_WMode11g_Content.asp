<html>
<head>
<title>ZVMODELVZ Web Manager</title>
<link rel="stylesheet" type="text/css" href="style.css" media="screen">
<script type="text/javascript" src="overlib.js"></script>
<script type="text/javascript" src="general.js"></script>
</head>  
<body onLoad="load_body()" onunLoad="return unload_body();">
<div id="overDiv" style="position:absolute; visibility:hidden; z-index:1000;"></div>    
<form method="GET" name="form" action="apply.cgi">
<input type="hidden" name="current_page" value="Advanced_WMode11g_Content.asp">
<input type="hidden" name="next_page" value="Advanced_ACL_Content.asp">
<input type="hidden" name="next_host" value="">
<input type="hidden" name="sid_list" value="WLANConfig11a;WLANConfig11b;">
<input type="hidden" name="group_id" value="">
<input type="hidden" name="modified" value="0">
<input type="hidden" name="action_mode" value="">
<input type="hidden" name="action_script" value="">
<!-- Table for the conntent page -->	    
<table width="666" border="1" cellpadding="0" cellspacing="0" bordercolor="#E0E0E0">
<tr class="content_header_tr">
<td class="content_header_td_title" colspan="2">Wireless - Bridge</td>
</tr>
<tr>
<td class="content_header_td" onMouseOver="return overlib('Selecting Wireless Bridge disables access point functionality. Only wireless bridge (also known as Wireless Distribution System or WDS) functionality will be available. Selecting Access Point enables access point functionality. Wireless bridge functionality will still be available and wireless stations will be able to associate to the AP.', LEFT);" onMouseOut="return nd();">AP Mode:</td><td class="content_input_td"><select name="WLANConfig11b_x_APMode" class="content_input_fd" onChange="return change_common(this, 'WLANConfig11b', 'x_APMode')"><option class="content_input_fd" value="0" <% nvram_match("x_APMode", "0","selected"); %>>Access Point</option><option class="content_input_fd" value="1" <% nvram_match("x_APMode", "1","selected"); %>>Wireless Bridge</option></select></td>
</tr>
<tr>
<td class="content_header_td" onMouseOver="return overlib('Select the operating radio channel', LEFT);" onMouseOut="return nd();">Channel:</td><td class="content_input_td"><select name="WLANConfig11b_Channel" class="content_input_fd" onChange="return change_common(this, 'WLANConfig11b', 'Channel')">   
<% select_channel(); %>
                </select><input type="hidden" maxlength="15" size="15" name="WLANConfig11b_x_RegulatoryDomain" value="<% nvram_get("x_RegulatoryDomain"); %>" readonly></td>
</tr>
<tr>
<td class="content_header_td" onMouseOver="return overlib('Selecting No disables wireless bridge restriction. Any wireless bridge (including the ones listed in Remote Bridges) will be granted access. Selecting Yes enables wireless bridge restriction. Only those bridges listed in Remote Bridges will be granted access.', LEFT);" onMouseOut="return nd();">Restrict Bridge to MACs below:</td><td class="content_input_td"><input type="radio" value="1" name="WLANConfig11b_x_BRestrict" class="content_input_fd" onClick="return change_common_radio(this, 'WLANConfig11b', 'x_BRestrict', '1')" <% nvram_match("x_BRestrict", "1", "checked"); %>>Yes</input><input type="radio" value="0" name="WLANConfig11b_x_BRestrict" class="content_input_fd" onClick="return change_common_radio(this, 'WLANConfig11b', 'x_BRestrict', '0')" <% nvram_match("x_BRestrict", "0", "checked"); %>>No</input></td>
</tr>
<tr>
<td class="content_header_td">Remote Bridge 1:</td><td class="content_input_td"><input type="text" maxlength="12" class="content_input_fd" size="12" name="WLANConfig11b_x_BRhwaddr1" value="<% nvram_get("x_BRhwaddr1"); %>" onBlur="return validate_hwaddr(this)" onKeyPress="return is_hwaddr(event, this)"></td>
</tr>
<tr>
<td class="content_header_td">Remote Bridge 2:</td><td class="content_input_td"><input type="text" maxlength="12" class="content_input_fd" size="12" name="WLANConfig11b_x_BRhwaddr2" value="<% nvram_get("x_BRhwaddr2"); %>" onBlur="return validate_hwaddr(this)" onKeyPress="return is_hwaddr(event, this)"></td>
</tr>
<tr>
<td class="content_header_td">Remote Bridge 3:</td><td class="content_input_td"><input type="text" maxlength="12" class="content_input_fd" size="12" name="WLANConfig11b_x_BRhwaddr3" value="<% nvram_get("x_BRhwaddr3"); %>" onBlur="return validate_hwaddr(this)" onKeyPress="return is_hwaddr(event, this)"></td>
</tr>
<tr>
<td class="content_header_td">Remote Bridge 4:</td><td class="content_input_td"><input type="text" maxlength="12" class="content_input_fd" size="12" name="WLANConfig11b_x_BRhwaddr4" value="<% nvram_get("x_BRhwaddr4"); %>" onBlur="return validate_hwaddr(this)" onKeyPress="return is_hwaddr(event, this)"></td>
</tr>
</table>

<% include("footer_buttons.inc"); %>

</form>
</body>
</html>
