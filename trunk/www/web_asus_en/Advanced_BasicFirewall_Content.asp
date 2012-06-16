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
<input type="hidden" name="current_page" value="Advanced_BasicFirewall_Content.asp">
<input type="hidden" name="next_page" value="Advanced_Firewall_Content.asp">
<input type="hidden" name="next_host" value="">
<input type="hidden" name="sid_list" value="FirewallConfig;">
<input type="hidden" name="group_id" value="">
<input type="hidden" name="modified" value="0">
<input type="hidden" name="action_mode" value="">
<input type="hidden" name="first_time" value="">
<input type="hidden" name="action_script" value="">
<!-- Table for the conntent page -->	    
<table width="666" border="1" cellpadding="0" cellspacing="0" bordercolor="#E0E0E0">
<tr class="content_header_tr">
<td class="content_header_td_title" colspan="2">Internet Firewall - Basic Config</td>
</tr>
<tr>
<td class="content_desc_td" colspan="2">Enabling Firewall(SPI Firewall) will provide basic protection for ZVMODELVZ and devices behind it. If you want to filter out specified packets, please use WAN vs. LAN filter in next page.
         </td>
</tr>
<tr>
<td class="content_header_td">Enable Firewall?
           </td><td class="content_input_td"><input type="radio" value="1" name="fw_enable_x" class="content_input_fd" onClick="return change_common_radio(this, 'FirewallConfig', 'fw_enable_x', '1')" <% nvram_match_x("FirewallConfig","fw_enable_x", "1", "checked"); %>>Yes</input><input type="radio" value="0" name="fw_enable_x" class="content_input_fd" onClick="return change_common_radio(this, 'FirewallConfig', 'fw_enable_x', '0')" <% nvram_match_x("FirewallConfig","fw_enable_x", "0", "checked"); %>>No</input></td>
</tr>
<tr>
<td class="content_header_td">Enable DoS protection?
           </td><td class="content_input_td"><input type="radio" value="1" name="fw_dos_x" class="content_input_fd" onClick="return change_common_radio(this, 'FirewallConfig', 'fw_dos_x', '1')" <% nvram_match_x("FirewallConfig","fw_dos_x", "1", "checked"); %>>Yes</input><input type="radio" value="0" name="fw_dos_x" class="content_input_fd" onClick="return change_common_radio(this, 'FirewallConfig', 'fw_dos_x', '0')" <% nvram_match_x("FirewallConfig","fw_dos_x", "0", "checked"); %>>No</input></td>
</tr>
<tr>
<td class="content_header_td" onMouseOver="return overlib('This field indicates what kind of packets between LAN and WAN will be logged.', LEFT);" onMouseOut="return nd();">Logged packets type:
           </td><td class="content_input_td"><select name="fw_log_x" class="content_input_fd" onChange="return change_common(this, 'FirewallConfig', 'fw_log_x')"><option class="content_input_fd" value="none" <% nvram_match_x("FirewallConfig","fw_log_x", "none","selected"); %>>None</option><option class="content_input_fd" value="drop" <% nvram_match_x("FirewallConfig","fw_log_x", "drop","selected"); %>>Dropped</option><option class="content_input_fd" value="accept" <% nvram_match_x("FirewallConfig","fw_log_x", "accept","selected"); %>>Accepted</option><option class="content_input_fd" value="both" <% nvram_match_x("FirewallConfig","fw_log_x", "both","selected"); %>>Both</option></select></td>
</tr>
<tr>
<td class="content_header_td" onMouseOver="return overlib('This feature allows you to manage ZVMODELVZ from Internet. If you are in Home Gateway mode, please access ZVMODELVZ with 8080 port(it means entering URL, such as http://192.168.123.1:8080, in your browser). Because the default port of browser(80) is reserved specially for Web server within your local network. This option works only when Firewall is enabled. If Firewall is disabled, you can access ZVMODELVZ with 80 port directly.', LEFT);" onMouseOut="return nd();">Enable Web Access from WAN?
           </td><td class="content_input_td"><input type="radio" value="1" name="misc_http_x" class="content_input_fd" onClick="return change_common_radio(this, 'FirewallConfig', 'misc_http_x', '1')" <% nvram_match_x("FirewallConfig","misc_http_x", "1", "checked"); %>>Yes</input><input type="radio" value="0" name="misc_http_x" class="content_input_fd" onClick="return change_common_radio(this, 'FirewallConfig', 'misc_http_x', '0')" <% nvram_match_x("FirewallConfig","misc_http_x", "0", "checked"); %>>No</input></td>
</tr>
<tr>
<td class="content_header_td" onMouseOver="return overlib('This field allows you to specify the port used to access Web server from Internet.', LEFT);" onMouseOut="return nd();">Port of Web Access from WAN:
           </td><td class="content_input_td"><input type="text" maxlength="5" size="5" name="misc_httpport_x" class="content_input_fd" value="<% nvram_get("misc_httpport_x"); %>" onBlur="validate_range(this, 1024, 65535)" onKeyPress="return is_number(event, this)"></td>
</tr>
<tr>
<td class="content_header_td" onMouseOver="return overlib('This feature allows you to respond LPR request from Internet.', LEFT);" onMouseOut="return nd();">Respond LPR Request from WAN?
           </td><td class="content_input_td"><input type="radio" value="1" name="misc_lpr_x" class="content_input_fd" onClick="return change_common_radio(this, 'FirewallConfig', 'misc_lpr_x', '1')" <% nvram_match_x("FirewallConfig","misc_lpr_x", "1", "checked"); %>>Yes</input><input type="radio" value="0" name="misc_lpr_x" class="content_input_fd" onClick="return change_common_radio(this, 'FirewallConfig', 'misc_lpr_x', '0')" <% nvram_match_x("FirewallConfig","misc_lpr_x", "0", "checked"); %>>No</input></td>
</tr>
<tr>
<td class="content_header_td" onMouseOver="return overlib('This feature allows you to respond ping request from Internet.', LEFT);" onMouseOut="return nd();">Respond Ping Request from WAN?
           </td><td class="content_input_td"><input type="radio" value="1" name="misc_ping_x" class="content_input_fd" onClick="return change_common_radio(this, 'FirewallConfig', 'misc_ping_x', '1')" <% nvram_match_x("FirewallConfig","misc_ping_x", "1", "checked"); %>>Yes</input><input type="radio" value="0" name="misc_ping_x" class="content_input_fd" onClick="return change_common_radio(this, 'FirewallConfig', 'misc_ping_x', '0')" <% nvram_match_x("FirewallConfig","misc_ping_x", "0", "checked"); %>>No</input></td>
</tr>
<tr>
<td class="content_header_td" onMouseOver="return overlib('Specifies number of simultaneous connections tracked by router (256-65535).', LEFT);" onMouseOut="return nd();">Number of connections to track:
           </td><td class="content_input_td"><input type="text" maxlength="5" class="content_input_fd" size="5" name="misc_conntrack_x" value="<% nvram_get("misc_conntrack_x"); %>" onKeyPress="return is_number(event, this)" onBlur="validate_range(this, 256, 65535)"></td>
</tr>

<tr class="content_section_header_tr">
<td class="content_section_header_td" colspan="2">Protection against Brute Force Attacks from WAN for SSH and FTP Servers</td>
</tr>
<tr>
<td class="content_desc_td" colspan="2">Blocks attempts to login from WAN more than hitcount times within a period of time.
Remember: all attempts are counted, not only unsuccessful.</td>
</tr>
<tr>
<td class="content_header_td" onMouseOver="return overlib('Enables Protection Against WAN Brute Force Attacks for SSH Server.', LEFT);" onMouseOut="return nd();">Enable Brute Force Protection for SSH Server:</td>
<td class="content_input_td"><input type="radio" value="1" name="recent_ssh_enable" class="content_input_fd" onClick="return change_common_radio(this, 'FirewallConfig', 'recent_ssh_enable', '1')" <% nvram_match_x("FirewallConfig", "recent_ssh_enable", "1", "checked"); %>>Yes</input><input type="radio" value="0" name="recent_ssh_enable" class="content_input_fd" onClick="return change_common_radio(this, 'FirewallConfig', 'recent_ssh_enable', '0')" <% nvram_match_x("FirewallConfig", "recent_ssh_enable", "0", "checked"); %>>No</input></td>
</tr>
<tr>
<td class="content_header_td" onMouseOver="return overlib('Enables Protection Against WAN Brute Force Attacks for FTP Server.', LEFT);" onMouseOut="return nd();">Enable Brute Force Protection for FTP Server:</td>
<td class="content_input_td"><input type="radio" value="1" name="recent_ftp_enable" class="content_input_fd" onClick="return change_common_radio(this, 'FirewallConfig', 'recent_ftp_enable', '1')" <% nvram_match_x("FirewallConfig", "recent_ftp_enable", "1", "checked"); %>>Yes</input><input type="radio" value="0" name="recent_ftp_enable" class="content_input_fd" onClick="return change_common_radio(this, 'FirewallConfig', 'recent_ftp_enable', '0')" <% nvram_match_x("FirewallConfig", "recent_ftp_enable", "0", "checked"); %>>No</input></td>
</tr>
<tr>
<td class="content_header_td" onMouseOver="return overlib('Hitcount.', LEFT);" onMouseOut="return nd();">Hitcount:</td>
<td class="content_input_td"><input type="text" maxlength="5" size="5" name="recent_hitcount" class="content_input_fd" value="<% nvram_get("recent_hitcount"); %>" onBlur="validate_range(this, 1, 65535)" onKeyPress="return is_number(event, this)"></td>
</tr>
<tr>
<td class="content_header_td" onMouseOver="return overlib('Period of time in seconds.', LEFT);" onMouseOut="return nd();">Period, seconds:</td>
<td class="content_input_td"><input type="text" maxlength="5" size="5" name="recent_seconds" class="content_input_fd" value="<% nvram_get("recent_seconds"); %>" onBlur="validate_range(this, 1, 65535)" onKeyPress="return is_number(event, this)"></td>
</tr>
</table>

<% include("footer_buttons.inc"); %>

</form>
</body>
</html>
