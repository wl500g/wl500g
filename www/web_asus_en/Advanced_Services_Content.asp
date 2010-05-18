<head>
<title>ZVMODELVZ Web Manager</title>
<meta http-equiv="Content-Type" content="text/html; charset=ISO-8859-1">
<meta http-equiv="Content-Type" content="text/html; charset=UTF-8">
<meta http-equiv="Content-Type" content="text/html; charset=gb2312">
<link rel="stylesheet" type="text/css" href="style.css" media="screen"></link>
<script language="JavaScript" type="text/javascript" src="overlib.js"></script>
<script language="JavaScript" type="text/javascript" src="general.js"></script>
</head>  
<div id="overDiv" style="position:absolute; visibility:hidden; z-index:1000;"></div>    
<body onLoad="load_body()" onunLoad="return unload_body();">
<form method="GET" name="form" action="apply.cgi">
<!-- Table for the conntent page -->	    
<table width="666" border="0" cellpadding="0" cellspacing="0">     	      
    	
<input type="hidden" name="current_page" value="Advanced_Services_Content.asp">
<input type="hidden" name="next_page" value="Advanced_Services_Content.asp">
<input type="hidden" name="next_host" value=""><input type="hidden" name="sid_list" value="PrinterStatus;">
<input type="hidden" name="modified" value="0"><input type="hidden" name="action_mode" value=""><input type="hidden" name="first_time" value=""><input type="hidden" name="action_script" value="">
<tr>
<td>
<table width="666" border="1" cellpadding="0" cellspacing="0" bordercolor="E0E0E0">
<tr class="content_header_tr">
<td class="content_header_td_title" colspan="2">Advanced Services Configuration</td>
</tr>
<tr>
<td class="content_desc_td" colspan="2">ZVMODELVZ runs several services by default, this page allows
you to perform advanced configuration.</td>
</tr>
</table>
</td>
</tr>

<tr>
<td>
<table width="666" border="1" cellpadding="0" cellspacing="0" bordercolor="E0E0E0">
<tr class="content_section_header_tr">
<td class="content_section_header_td" colspan="2">Telnet/SSH</td>
</tr>
<tr>
<td class="content_header_td" onMouseOver="return overlib('Enables Telnet daemon.', LEFT);" onMouseOut="return nd();">Enable telnet access:</td>
<td class="content_input_td"><input type="radio" value="1" name="telnet_enable" class="content_input_fd" onClick="return change_common_radio(this, 'PrinterStatus', 'telnet_enable', '1')" <% nvram_match_x("PrinterStatus","telnet_enable", "1", "checked"); %>>Yes</input><input type="radio" value="0" name="telnet_enable" class="content_input_fd" onClick="return change_common_radio(this, 'PrinterStatus', 'telnet_enable', '0')" <% nvram_match_x("PrinterStatus","telnet_enable", "0", "checked"); %>>No</input></td>
</tr>
<tr>
<td class="content_header_td" onMouseOver="return overlib('Enables SSH daemon.', LEFT);" onMouseOut="return nd();">Enable SSH Server:</td>
<td class="content_input_td"><select name="ssh_enable" class="content_input_fd">
<option class="content_input_fd" value="0" <% nvram_match_x("PrinterStatus","ssh_enable","0","selected"); %>>No</option>
<option class="content_input_fd" value="1" <% nvram_match_x("PrinterStatus","ssh_enable","1","selected"); %>>Yes</option>
<option class="content_input_fd" value="2" <% nvram_match_x("PrinterStatus","ssh_enable","2","selected"); %>>Yes, LAN Only</option>
</select></td>
</tr>
<tr>
<td class="content_header_td" onMouseOver="return overlib('This field indicates the port number used by SSH protocol.', LEFT);" onMouseOut="return nd();">SSH Port:</td>
<td class="content_input_td"><input type="text" maxlength="5" size="5" name="ssh_port" class="content_input_fd" value="<% nvram_get_x("PrinterStatus", "ssh_port"); %>" onBlur="validate_range(this, 1, 65535)" onKeyPress="return is_number(this)"></td>
</tr>
</table>
</td>
</tr>

<tr>
<td>
<table width="666" border="1" cellpadding="0" cellspacing="0" bordercolor="E0E0E0">
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
</table>
</td>
</tr>

<tr>
<td>
<table width="666" border="1" cellpadding="0" cellspacing="0" bordercolor="E0E0E0">
<tr class="content_section_header_tr">
<td class="content_section_header_td" colspan="2">USB features</td>
</tr>
<tr>
<td class="content_header_td" onMouseOver="return overlib('Disables USB 2.0 controller.', LEFT);" onMouseOut="return nd();">Disable USB 2.0 controller:</td>
<td class="content_input_td"><input type="radio" value="1" name="usb20_disable_x" class="content_input_fd" onClick="return change_common_radio(this, 'PrinterStatus', 'usb20_disable_x', '1')" <% nvram_match_x("PrinterStatus","usb20_disable_x", "1", "checked"); %>>Yes</input><input type="radio" value="0" name="usb20_disable_x" class="content_input_fd" onClick="return change_common_radio(this, 'PrinterStatus', 'usb20_disable_x', '0')" <% nvram_match_x("PrinterStatus","usb20_disable_x", "0", "checked"); %>>No</input></td>
</tr>
<tr>
<td class="content_header_td" onMouseOver="return overlib('Enables USB Storage.', LEFT);" onMouseOut="return nd();">Enable USB Storage:</td>
<td class="content_input_td"><input type="radio" value="1" name="usb_storage_x" class="content_input_fd" onClick="return change_common_radio(this, 'PrinterStatus', 'usb_storage_x', '1')" <% nvram_match_x("PrinterStatus","usb_storage_x", "1", "checked"); %>>Yes</input><input type="radio" value="0" name="usb_storage_x" class="content_input_fd" onClick="return change_common_radio(this, 'PrinterStatus', 'usb_storage_x', '0')" <% nvram_match_x("PrinterStatus","usb_storage_x", "0", "checked"); %>>No</input></td>
</tr>
<tr><td class="content_header_td" onMouseOver="return overlib('Enables WaveServer daemon.', LEFT);" onMouseOut="return nd();">Enable WaveServer:</td><td class="content_input_td"><input type="radio" value="1" name="audio_enable" class="content_input_fd" onClick="return change_common_radio(this, 'PrinterStatus', 'audio_enable', '1')" <% nvram_match_x("PrinterStatus","audio_enable", "1", "checked"); %>>Yes</input><input type="radio" value="0" name="audio_enable" class="content_input_fd" onClick="return change_common_radio(this, 'PrinterStatus', 'audio_enable', '0')" <% nvram_match_x("PrinterStatus","audio_enable", "0", "checked"); %>>No</input></td></tr>
</table>
</td>
</tr>

<tr>
<td>
<table width="666" border="1" cellpadding="0" cellspacing="0" bordercolor="E0E0E0">
<tr class="content_section_header_tr">
<td class="content_section_header_td" colspan="2">Mount options</td>
</tr>
<tr>
<td class="content_header_td" onMouseOver="return overlib('Enables NTFS Read-Write support (ntfs-3g).', LEFT);" onMouseOut="return nd();">Read-Write NTFS Support:</td>
<td class="content_input_td"><input type="radio" value="1" name="usb_ntfs3g_enable" class="content_input_fd" onClick="return change_common_radio(this, 'PrinterStatus', 'usb_ntfs3g_enable', '1')" <% nvram_match_x("PrinterStatus","usb_ntfs3g_enable", "1", "checked"); %>>Yes</input><input type="radio" value="0" name="usb_ntfs3g_enable" class="content_input_fd" onClick="return change_common_radio(this, 'PrinterStatus', 'usb_ntfs3g_enable', '0')" <% nvram_match_x("PrinterStatus","usb_ntfs3g_enable", "0", "checked"); %>>No</input></td>
</tr>
</table>
</td>
</tr>

<tr>
<td>		
<table width="666" border="1" cellpadding="0" cellspacing="0" bordercolor="B0B0B0">
<tr bgcolor="#CCCCCC"><td colspan="3"><font face="arial" size="2"><b>&nbsp</b></font></td></tr>
<tr bgcolor="#FFFFFF">  
   <td id ="Confirm" height="25" width="34%">  
   <div align="center"><font face="Arial"> <input class=inputSubmit onMouseOut=buttonOut(this) onMouseOver="buttonOver(this)" onClick="onSubmitCtrl(this, ' Restore ')" type="submit" value=" Restore " name="action"></font></div> 
   </td>  
   <td height="25" width="33%">  
   <div align="center"><font face="Arial"> <input class=inputSubmit onMouseOut=buttonOut(this) onMouseOver="buttonOver(this)" onClick="onSubmitCtrl(this, ' Finish ')" type="submit" value=" Finish " name="action"></font></div> 
   </td>
   <td height="25" width="33%">  
   <div align="center"><font face="Arial"> <input class=inputSubmit onMouseOut=buttonOut(this) onMouseOver="buttonOver(this)" onClick="onSubmitCtrl(this, ' Apply ')" type="submit" value=" Apply " name="action"></font></div> 
   </td>    
</tr>
</table>
</td>
</tr>

<tr>
<td>
<table width="666" border="1" cellpadding="0" cellspacing="0" bordercolor="B0B0B0">
<tr>
    <td colspan="2" width="616" height="25" bgcolor="#FFBB00"></td> 
</tr>                   
<tr bgcolor="#FFFFFF">
    <td class="content_header_td_15" align="left">Restore: </td>
    <td class="content_input_td_padding" align="left">Clear the above settings and restore the settings in effect.</td>
</tr>
<tr bgcolor="#FFFFFF">
    <td class="content_header_td_15" align="left">Finish: </td>
    <td class="content_input_td_padding" align="left">Confirm all settings and restart ZVMODELVZ now.</td>
</tr>
<tr bgcolor="#FFFFFF">
    <td class="content_header_td_15" align="left">Apply: </td>
    <td class="content_input_td_padding" align="left">Confirm above settings and continue.</td>
</tr>
</table>
</td>
</tr>

</table>
</form>
</body>
