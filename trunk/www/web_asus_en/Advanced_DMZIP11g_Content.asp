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
<input type="hidden" name="current_page" value="Advanced_DMZIP_Content.asp">
<input type="hidden" name="next_page" value="Advanced_DMZDHCP_Content.asp">
<input type="hidden" name="next_host" value="">
<input type="hidden" name="sid_list" value="FirewallConfig;LANHostConfig;">
<input type="hidden" name="group_id" value="">
<input type="hidden" name="modified" value="0">
<input type="hidden" name="action_mode" value="">
<input type="hidden" name="first_time" value="">
<input type="hidden" name="action_script" value="">
<input type="hidden" name="" value="<% nvram_get_x("LANHostConfig",""); %>">
<!-- Table for the conntent page -->	      	      
<table width="666" border="1" cellpadding="0" cellspacing="0" bordercolor="#E0E0E0">
<tr class="content_header_tr">
<td class="content_header_td_title" colspan="2">Wireless Firewall - Basic Config</td>
</tr>
<tr>
<td class="content_desc_td" colspan="2">Wireless Firewall allows you to create a separated wireless local network. All packets from clients under this network are controlled by filter rules you set.
         <div align="center">
<img name="WirelessFirewall_img" src="graph/wf_none.gif"></div>
</td>
</tr>
<tr>
<td class="content_header_td">Enable Wireless Firewall?
           </td><td class="content_input_td"><input type="radio" value="1" name="" class="content_input_fd" onClick="return change_common_radio(this, 'FirewallConfig', '', '1')" <% nvram_match_x("FirewallConfig","", "1", "checked"); %>>Yes</input><input type="radio" value="0" name="" class="content_input_fd" onClick="return change_common_radio(this, 'FirewallConfig', '', '0')" <% nvram_match_x("FirewallConfig","", "0", "checked"); %>>No</input></td>
</tr>
<tr>
<td class="content_header_td" onMouseOver="return overlib('This is IP Address of ZVMODELVZ as seen in this separated wireless local network.', LEFT);" onMouseOut="return nd();">IP Address:
           </td><td class="content_input_td"><input type="text" maxlength="15" class="content_input_fd" size="15" name="" value="<% nvram_get_x("FirewallConfig",""); %>" onBlur="return validate_ipaddr(this, '')" onKeyPress="return is_ipaddr(event, this)" onKeyUp="change_ipaddr(this)"></td>
</tr>
<tr>
<td class="content_header_td" onMouseOver="return overlib('This is Subnet Mask of ZVMODELVZ as seen in this separated wireless local network.', LEFT);" onMouseOut="return nd();">Subnet mask:
           </td><td class="content_input_td"><input type="text" maxlength="15" class="content_input_fd" size="15" name="" value="<% nvram_get_x("FirewallConfig",""); %>" onBlur="return validate_ipaddr(this, '')" onKeyPress="return is_ipaddr(event, this)" onKeyUp="change_ipaddr(this)"></td>
</tr>
</table>

<% include("footer_buttons.inc"); %>

</form>
</body>
</html>
