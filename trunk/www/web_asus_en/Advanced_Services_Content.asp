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
<input type="hidden" name="current_page" value="Advanced_Services_Content.asp">
<input type="hidden" name="next_page" value="Advanced_Services_Content.asp">
<input type="hidden" name="next_host" value="">
<input type="hidden" name="sid_list" value="PrinterStatus;">
<input type="hidden" name="modified" value="0">
<input type="hidden" name="action_mode" value="">
<input type="hidden" name="first_time" value="">
<input type="hidden" name="action_script" value="">
<!-- Table for the conntent page -->	    
<table width="666" border="1" cellpadding="0" cellspacing="0" bordercolor="#E0E0E0">
<tr class="content_header_tr">
<td class="content_header_td_title" colspan="2">Advanced Services Configuration</td>
</tr>
<tr>
<td class="content_desc_td" colspan="2">ZVMODELVZ runs several services by default, this page allows
you to perform advanced configuration.</td>
</tr>

<tr class="content_section_header_tr">
<td class="content_section_header_td" colspan="2">Network services</td>
</tr>
<tr>
<td class="content_header_td" onMouseOver="return overlib('Enables Telnet daemon.', LEFT);" onMouseOut="return nd();">Enable telnet access:</td>
<td class="content_input_td"><input type="radio" value="1" name="telnet_enable" class="content_input_fd" onClick="return change_common_radio(this, 'PrinterStatus', 'telnet_enable', '1')" <% nvram_match_x("PrinterStatus","telnet_enable", "1", "checked"); %>>Yes</input><input type="radio" value="0" name="telnet_enable" class="content_input_fd" onClick="return change_common_radio(this, 'PrinterStatus', 'telnet_enable', '0')" <% nvram_match_x("PrinterStatus","telnet_enable", "0", "checked"); %>>No</input></td>
</tr>
<tr>
<td class="content_header_td" onMouseOver="return overlib('Enables SSH daemon.', LEFT);" onMouseOut="return nd();">Enable SSH access:</td>
<td class="content_input_td"><select name="ssh_enable" class="content_input_fd">
<option class="content_input_fd" value="0" <% nvram_match_x("PrinterStatus","ssh_enable","0","selected"); %>>No</option>
<option class="content_input_fd" value="1" <% nvram_match_x("PrinterStatus","ssh_enable","1","selected"); %>>Yes</option>
<option class="content_input_fd" value="2" <% nvram_match_x("PrinterStatus","ssh_enable","2","selected"); %>>Yes, LAN Only</option>
</select></td>
</tr>
<tr>
<td class="content_header_td" onMouseOver="return overlib('This field indicates the port number used by SSH protocol.', LEFT);" onMouseOut="return nd();">SSH Port:</td>
<td class="content_input_td"><input type="text" maxlength="5" size="5" name="ssh_port" class="content_input_fd" value="<% nvram_get_x("PrinterStatus", "ssh_port"); %>" onBlur="validate_range(this, 1, 65535)" onKeyPress="return is_number(event, this)"></td>
</tr>
<tr>
<td class="content_header_td"
    onMouseOver="return overlib('This is disallow password login. You must use public key to login.', LEFT);"
    onMouseOut="return nd();"
    >Disable SSH password login:</td>
<td class="content_input_td">
    <input type="radio" name="ssh_password_logins" value="0"
        class="content_input_fd"
        onClick="return change_common_radio(this, 'PrinterStatus', 'ssh_password_logins', '1')"
        <% nvram_match_x("PrinterStatus","ssh_password_logins", "0", "checked"); %>
        >Yes</input>
    <input type="radio" name="ssh_password_logins" value="1"
        class="content_input_fd"
        onClick="return change_common_radio(this, 'PrinterStatus', 'ssh_password_logins', '0')"
        <% nvram_match_x("PrinterStatus","ssh_password_logins", "1", "checked"); %>
        >No</input></td>
</tr>

<tr>
<td class="content_header_td"
    onMouseOver="return overlib('Input one key per row. Press \'Clear\' button to delete the keys.', LEFT);"
    onMouseOut="return nd();"
	>Change SSH public keys:</td>
<td class="content_input_td">
	<input type="radio" name="ssh_keys_change" value=1
		class="content_input_fd"
		onclick="this.form.ssh_keys.disabled=this.form.ssh_keys_clear.disabled=false;true;";
		>Yes</input>
	<input type="radio" name="ssh_keys_change" value=0 checked
		class="content_input_fd"
		onclick="this.form.ssh_keys.disabled=this.form.ssh_keys_clear.disabled=true;";
		>No</input></td>
</tr>

<tr>
<td class="content_content_td" colspan=2>
	<!--<span class="content_header_td">Input one key per row:</span>-->
	<input type="hidden" name="ssh_keys_changed" value=0>
	<table border=0><tr>
		<td class="content_content_td">
			<textarea name="ssh_keys" cols=72 rows=8 disabled 
				onchange="this.form.ssh_keys_changed.value=1;"
				><% print_text_file("/usr/local/root/.ssh/authorized_keys"); %></textarea></td>
		<td class="content_content_td" valign="top">
			<input type="button" name="ssh_keys_clear" value="Clear" disabled
				onclick="this.form.ssh_keys.value='';this.form.ssh_keys_changed.value=1;"
				></td>
	</tr></table>
</tr>

<tr><td class="content_header_td" onMouseOver="return overlib('Enables Link Layer Topology Discovery daemon.', LEFT);" onMouseOut="return nd();">Enable LLTD responder:</td><td class="content_input_td"><input type="radio" value="1" name="lltd_enable" class="content_input_fd" onClick="return change_common_radio(this, 'PrinterStatus', 'lltd_enable', '1')" <% nvram_match_x("PrinterStatus","lltd_enable", "1", "checked"); %>>Yes</input><input type="radio" value="0" name="lltd_enable" class="content_input_fd" onClick="return change_common_radio(this, 'PrinterStatus', 'lltd_enable', '0')" <% nvram_match_x("PrinterStatus","lltd_enable", "0", "checked"); %>>No</input></td></tr>

<tr class="content_section_header_tr">
<td class="content_section_header_td" colspan="2">Printing services</td>
</tr>
<tr>
<td class="content_header_td" onMouseOver="return overlib('Enables LPR protocol support.', LEFT);" onMouseOut="return nd();">Enable LPR printing:</td>
<td class="content_input_td"><input type="radio" value="1" name="lpr_enable" class="content_input_fd" onClick="return change_common_radio(this, 'PrinterStatus', 'lpr_enable', '1')" <% nvram_match_x("PrinterStatus","lpr_enable", "1", "checked"); %>>Yes</input><input type="radio" value="0" name="lpr_enable" class="content_input_fd" onClick="return change_common_radio(this, 'PrinterStatus', 'lpr_enable', '0')" <% nvram_match_x("PrinterStatus","lpr_enable", "0", "checked"); %>>No</input></td>
</tr>
<tr>
<td class="content_header_td" onMouseOver="return overlib('Enables RAW (Socket) protocol support.', LEFT);" onMouseOut="return nd();">Enable RAW printing:</td>
<td class="content_input_td"><input type="radio" value="1" name="raw_enable" class="content_input_fd" onClick="return change_common_radio(this, 'PrinterStatus', 'raw_enable', '1')" <% nvram_match_x("PrinterStatus","raw_enable", "1", "checked"); %>>Yes</input><input type="radio" value="0" name="raw_enable" class="content_input_fd" onClick="return change_common_radio(this, 'PrinterStatus', 'raw_enable', '0')" <% nvram_match_x("PrinterStatus","raw_enable", "0", "checked"); %>>No</input></td>
</tr>

<tr class="content_section_header_tr">
<td class="content_section_header_td" colspan="2">USB features</td>
</tr>
<tr>
<td class="content_header_td" onMouseOver="return overlib('USB controller mode.', LEFT);" onMouseOut="return nd();">USB controller mode:</td>
<td class="content_input_td"><select name="usb20_disable_x" class="content_input_fd">
<option class="content_input_fd" value="0" <% nvram_match_x("PrinterStatus","usb20_disable_x","0","selected"); %>>USB 1.1 &amp; USB 2.0</option>
<option class="content_input_fd" value="1" <% nvram_match_x("PrinterStatus","usb20_disable_x","1","selected"); %>>USB 1.1 Only</option>
<option class="content_input_fd" value="2" <% nvram_match_x("PrinterStatus","usb20_disable_x","2","selected"); %>>USB 2.0 Only</option>
</select></td>
</tr>
<tr>
<td class="content_header_td" onMouseOver="return overlib('Enables USB Storage.', LEFT);" onMouseOut="return nd();">Enable USB Storage:</td>
<td class="content_input_td"><input type="radio" value="1" name="usb_storage_x" class="content_input_fd" onClick="return change_common_radio(this, 'PrinterStatus', 'usb_storage_x', '1')" <% nvram_match_x("PrinterStatus","usb_storage_x", "1", "checked"); %>>Yes</input><input type="radio" value="0" name="usb_storage_x" class="content_input_fd" onClick="return change_common_radio(this, 'PrinterStatus', 'usb_storage_x', '0')" <% nvram_match_x("PrinterStatus","usb_storage_x", "0", "checked"); %>>No</input></td>
</tr>
</table>

<tr>
<td>
<table width="666" border="1" cellpadding="0" cellspacing="0" bordercolor="E0E0E0">
<tr class="content_section_header_tr">
<td class="content_section_header_td" colspan="2">Filesystem options</td>
</tr>
<tr>
<td class="content_header_td" onMouseOver="return overlib('Enables NTFS Read-Write support (ntfs-3g). Disabling will increase the amount of the RAM memory.', LEFT);" onMouseOut="return nd();">NTFS Support:</td>
<td class="content_input_td">
<select name="usb_ntfs3g_enable" class="content_input_fd">
<option class="content_input_fd" value="0" <% nvram_match_x("PrinterStatus","usb_ntfs3g_enable","0","selected"); %>>Read only</option>
<option class="content_input_fd" value="1" <% nvram_match_x("PrinterStatus","usb_ntfs3g_enable","1","selected"); %>>Read-Write</option>
<option class="content_input_fd" value="2" <% nvram_match_x("PrinterStatus","usb_ntfs3g_enable","2","selected"); %>>Off</option>
</select></td></tr>
</table>
</td>
</tr>
</table>

<% include("footer_buttons.inc"); %>

</form>
</body>
</html>
