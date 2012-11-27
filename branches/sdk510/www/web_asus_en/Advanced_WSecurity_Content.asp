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
<input type="hidden" name="current_page" value="Advanced_WSecurity_Content.asp">
<input type="hidden" name="next_page" value="Advanced_WAdvanced_Content.asp">
<input type="hidden" name="next_host" value="">
<input type="hidden" name="sid_list" value="WLANAuthentication11a;WLANConfig11b;">
<input type="hidden" name="group_id" value="">
<input type="hidden" name="modified" value="0">
<input type="hidden" name="action_mode" value="">
<input type="hidden" name="first_time" value="">
<input type="hidden" name="action_script" value="">
<!-- Table for the conntent page -->	    
<table width="666" border="1" cellpadding="0" cellspacing="0" bordercolor="#E0E0E0">
<tr class="content_header_tr">
<td class="content_header_td_title" colspan="2">Wireless - RADIUS Setting</td>
</tr>
<tr>
<td class="content_desc_td" colspan="2">This section allows you to set up additional parameters for authorizing wireless clients through RADIUS server. It is required while you select "Authentication Method" in "Wireless - Interface" as "WPA" or "Radius with 802.1x".
         </td>
</tr>
<tr>
<td class="content_header_td" onMouseOver="return overlib('This field specifies the IP address of the RADIUS server to use for 802.1X wireless authentication and dynamic WEP key derivation.', LEFT);" onMouseOut="return nd();">Server IP Address:
           </td><td class="content_input_td"><input type="text" maxlength="15" class="content_input_fd" size="15" name="wl_radius_ipaddr" value="<% nvram_get("wl_radius_ipaddr"); %>" onBlur="return validate_ipaddr(this, 'wl_radius_ipaddr')" onKeyPress="return is_ipaddr(event, this)" onKeyUp="change_ipaddr(this)"></td>
</tr>
<tr>
<td class="content_header_td" onMouseOver="return overlib('This field specifies the UDP port number used by the RADIUS server.', LEFT);" onMouseOut="return nd();">Server Port:
           </td><td class="content_input_td"><input type="text" maxlength="5" class="content_input_fd" size="5" name="wl_radius_port" value="<% nvram_get("wl_radius_port"); %>" onBlur="validate_range(this, 0, 65535)" onKeyPress="return is_number(event, this)"></td>
</tr>
<tr>
<td class="content_header_td" onMouseOver="return overlib('This field specifies the password used to initialize a connection with the RADIUS server.', LEFT);" onMouseOut="return nd();">Connection Secret:
           </td><td class="content_input_td"><input type="password" maxlength="64" class="content_input_fd" size="32" name="wl_radius_key" value="<% nvram_get("wl_radius_key"); %>" onBlur="validate_string(this)"></td>
</tr>
</table>

<% include("footer_buttons.inc"); %>

</form>
</body>
</html>
