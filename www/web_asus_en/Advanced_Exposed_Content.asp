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
<input type="hidden" name="current_page" value="Advanced_Exposed_Content.asp">
<input type="hidden" name="next_page" value="SaveRestart.asp">
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
<td class="content_header_td_title" colspan="2">NAT Setting - Virtual DMZ</td>
</tr>
<tr>
<td class="content_desc_td" colspan="2">Virtual DMZ allows you to expose one computer to Internet, so that all the inbounds packets will be redirected to the computer you set. It is useful while you run some applications that use uncertained incoming ports. Please use it carefully.
         </td>
</tr>
<tr>
<td class="content_header_td">IP Address of Exposed Station:
           </td><td class="content_input_td"><input type="text" maxlength="15" class="content_input_fd" size="15" name="dmz_ip" value="<% nvram_get("dmz_ip"); %>" onBlur="return validate_ipaddr(this, 'dmz_ip')" onKeyPress="return is_ipaddr(event, this)" onKeyUp="change_ipaddr(this)"></td>
</tr>
</table>

<% include("footer_buttons.inc"); %>

</form>
</body>
</html>
