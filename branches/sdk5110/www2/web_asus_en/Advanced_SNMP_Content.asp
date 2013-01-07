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
<input type="hidden" name="current_page" value="Advanced_SNMP_Content.asp">
<input type="hidden" name="next_page" value="Advanced_SNMP_Content.asp">
<input type="hidden" name="next_host" value="">
<input type="hidden" name="sid_list" value="LANHostConfig;">
<input type="hidden" name="group_id" value="">
<input type="hidden" name="modified" value="0">
<input type="hidden" name="action_mode" value="">
<input type="hidden" name="first_time" value="">
<input type="hidden" name="action_script" value="">
<!-- Table for the conntent page -->	    
<table width="666" border="1" cellpadding="0" cellspacing="0" bordercolor="#E0E0E0">
<tr class="content_header_tr">
<td class="content_header_td_title" colspan="2">IP Config - SNMP</td>
</tr>

<tr>
<td class="content_header_td" onMouseOver="return overlib('Selecting Yes enables SNMP.', LEFT);" onMouseOut="return nd();">Enable SNMP? </td><td class="content_input_td"><input type="radio" value="1" name="snmp_enable" class="content_input_fd" onClick="return change_common_radio(this, 'LANHostConfig', 'snmp_enable', '1')" <% nvram_match_x("LANHostConfig","snmp_enable", "1", "checked"); %>>Yes</input><input type="radio" value="0" name="snmp_enable" class="content_input_fd" onClick="return change_common_radio(this, 'LANHostConfig', 'snmp_enable', '0')" <% nvram_match_x("LANHostConfig","snmp_enable", "0", "checked"); %>>No</input></td>
</tr>

<tr>
<td class="content_header_td" onMouseOver="return overlib('Specifies SNMP community name, which will be used to check for authorization', LEFT);" onMouseOut="return nd();">Community name:</td><td class="content_input_td"><input type="text" maxlength="32" class="content_input_fd" size="32" name="snmp_community" value="<% nvram_get_x("LANHostConfig","snmp_community"); %>" onKeyPress="return is_string(event, this)" onBlur="validate_string(this)"></td>
</tr>

<tr>
<td class="content_header_td" onMouseOver="return overlib('Specifies SNMP system location', LEFT);" onMouseOut="return nd();">System location:</td><td class="content_input_td"><input type="text" maxlength="80" class="content_input_fd" size="32" name="snmp_location" value="<% nvram_get_x("LANHostConfig","snmp_location"); %>" onKeyPress="return is_string(event, this)" onBlur="validate_string(this)"></td>
</tr>

<tr>
<td class="content_header_td" onMouseOver="return overlib('Specifies SNMP system contact person (e.g. e-mail address)', LEFT);" onMouseOut="return nd();">System contact:</td><td class="content_input_td"><input type="text" maxlength="80" class="content_input_fd" size="32" name="snmp_contact" value="<% nvram_get_x("LANHostConfig","snmp_contact"); %>" onKeyPress="return is_string(event, this)" onBlur="validate_string(this)"></td>
</tr>
</table>

<% include("footer_buttons.inc"); %>

</form>
</body>
</html>
