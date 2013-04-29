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
<input type="hidden" name="current_page" value="Advanced_RMISC_Content.asp">
<input type="hidden" name="next_page" value="SaveRestart.asp">
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
<td class="content_header_td_title" colspan="2">IP Config - Miscellaneous</td>
</tr>
<tr>
<td class="content_header_td" onMouseOver="return overlib('This feature allows you to assign a remote server to record log messages of the router. If you leave it blank, system will record up to 1024 mesages on the router only.', LEFT);" onMouseOut="return nd();">Remote Log Server:
           </td><td class="content_input_td"><input type="text" maxlength="15" class="content_input_fd" size="15" name="log_ipaddr" value="<% nvram_get("log_ipaddr"); %>" onBlur="return validate_ipaddr(this, 'log_ipaddr')" onKeyPress="return is_ipaddr(event, this)" onKeyUp="change_ipaddr(this)"></td>
</tr>
<tr>
<td class="content_header_td" onMouseOver="return overlib('This field indicates time zone where you are locating in.', LEFT);" onMouseOut="return nd();">Time Zone:
           </td><td class="content_input_td"><select name="TimeZoneList" class="content_input_fd" onChange="return change_common(this, 'LANHostConfig', 'time_zone')"><option class="content_input_fd" value="manual">Manual</option></select></td>
</tr>
<tr>
<td class="content_header_td" onMouseOver="return overlib('This field indicates time zone where you are locating in.', LEFT);" onMouseOut="return nd();">Time Zone Abbreviation:
	   </td><td class="content_input_td"><input type="text" maxlength="256" class="content_input_fd" size="32" name="time_zone" value="<% nvram_get("time_zone"); %>" onKeyPress="return is_string(event, this)" onBlur="validate_string(this)"></td>
</tr>
<tr>
<td class="content_header_td" onMouseOver="return overlib('The NTP Server used to synchronize your system time.', LEFT);" onMouseOut="return nd();">NTP Server
           </td><td class="content_input_td"><input type="text" maxlength="256" class="content_input_fd" size="32" name="ntp_server0" value="<% nvram_get("ntp_server0"); %>" onKeyPress="return is_string(event, this)" onBlur="validate_string(this)"><a href="javascript:openLink('x_NTPServer1')" class="content_input_link" name="x_NTPServer1_link">NTP Link
             </a></td>
</tr>
<tr>
<td class="content_header_td" onMouseOver="return overlib('This field specifies interval between time synchronization requests.', LEFT);" onMouseOut="return nd();">NTP synchronization interval (hours):
           </td><td class="content_input_td"><input type="text" maxlength="3" size="3" name="ntp_interval_x" class="content_input_fd" value="<% nvram_get("ntp_interval_x"); %>" onBlur="validate_range(this, 1, 144)" onKeyPress="return is_number(event, this)"></td>
</tr>
</table>

<% include("footer_buttons.inc"); %>

</form>
</body>
</html>
