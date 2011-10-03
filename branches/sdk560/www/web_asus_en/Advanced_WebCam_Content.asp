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
<input type="hidden" name="current_page" value="Advanced_WebCam_Content.asp">
<input type="hidden" name="next_page" value="SaveRestart.asp">
<input type="hidden" name="next_host" value="">
<input type="hidden" name="sid_list" value="PrinterStatus;">
<input type="hidden" name="group_id" value="">
<input type="hidden" name="modified" value="0">
<input type="hidden" name="action_mode" value="">
<input type="hidden" name="first_time" value="">
<input type="hidden" name="action_script" value="">
<input type="hidden" name="usb_webhttpcheck_x" value="<% nvram_get_x("PrinterStatus","usb_webhttpcheck_x"); %>">
<!-- Table for the conntent page -->	    
<table width="666" border="1" cellpadding="0" cellspacing="0" bordercolor="#E0E0E0">
<tr class="content_header_tr">
<td class="content_header_td_title" colspan="2">USB Application - Web Camera</td>
</tr>
<tr>
<td class="content_header_td">Enable Web Camera?
           </td><td class="content_input_td"><select name="usb_webenable_x" class="content_input_fd" onChange="return change_common(this, 'PrinterStatus', 'usb_webenable_x')"><option class="content_input_fd" value="0" <% nvram_match_x("PrinterStatus","usb_webenable_x", "0","selected"); %>>Disabled</option><option class="content_input_fd" value="1" <% nvram_match_x("PrinterStatus","usb_webenable_x", "1","selected"); %>>LAN Only</option><option class="content_input_fd" value="2" <% nvram_match_x("PrinterStatus","usb_webenable_x", "2","selected"); %>>LAN and WAN</option></select></td>
</tr>
<!--<tr>
<td class="content_header_td" onMouseOver="return overlib('This field indicates the driver used for your web camera.', LEFT);" onMouseOut="return nd();">Web Camera Driver:
           </td><td class="content_input_td"><select name="usb_webdriver_x" class="content_input_fd" onChange="return change_common(this, 'PrinterStatus', 'usb_webdriver_x')"><option class="content_input_fd" value="0" <% nvram_match_x("PrinterStatus","usb_webdriver_x", "0","selected"); %>>PWC 9.0.2</option><option class="content_input_fd" value="1" <% nvram_match_x("PrinterStatus","usb_webdriver_x", "1","selected"); %>>OV51x 1.65-1.12</option></select></td>
</tr>-->
<tr>
<td class="content_header_td" onMouseOver="return overlib('This field indicates the format of image.', LEFT);" onMouseOut="return nd();">Image Format:
           </td><td class="content_input_td"><select name="usb_webformat_x" class="content_input_fd" onChange="return change_common(this, 'PrinterStatus', 'usb_webformat_x')"><option class="content_input_fd" value="0" <% nvram_match_x("PrinterStatus","usb_webformat_x", "0","selected"); %>>MJPEG</option><option class="content_input_fd" value="1" <% nvram_match_x("PrinterStatus","usb_webformat_x", "1","selected"); %>>YUV</option></select></td>
</tr>
<tr>
<td class="content_header_td" onMouseOver="return overlib('This field indicates the size of image.', LEFT);" onMouseOut="return nd();">Image Size:
           </td><td class="content_input_td"><select name="usb_webimage_x" class="content_input_fd" onChange="return change_common(this, 'PrinterStatus', 'usb_webimage_x')"><option class="content_input_fd" value="8" <% nvram_match_x("PrinterStatus","usb_webimage_x", "8","selected"); %>>1920 X 1080 HD</option><option class="content_input_fd" value="7" <% nvram_match_x("PrinterStatus","usb_webimage_x", "7","selected"); %>>1280 X 720 HD</option><option class="content_input_fd" value="6" <% nvram_match_x("PrinterStatus","usb_webimage_x", "6","selected"); %>>1600 X 1200</option><option class="content_input_fd" value="5" <% nvram_match_x("PrinterStatus","usb_webimage_x", "5","selected"); %>>1280 X 1024</option><option class="content_input_fd" value="4" <% nvram_match_x("PrinterStatus","usb_webimage_x", "4","selected"); %>>1024 X 768</option><option class="content_input_fd" value="3" <% nvram_match_x("PrinterStatus","usb_webimage_x", "3","selected"); %>>800 X 600</option><option class="content_input_fd" value="2" <% nvram_match_x("PrinterStatus","usb_webimage_x", "2","selected"); %>>640 X 480</option><option class="content_input_fd" value="1" <% nvram_match_x("PrinterStatus","usb_webimage_x", "1","selected"); %>>320 X 240</option><option class="content_input_fd" value="0" <% nvram_match_x("PrinterStatus","usb_webimage_x", "0","selected"); %>>160 X 120</option></select><a href="javascript:openLink('x_WImageStatic')" class="content_input_link" name="x_WImageStatic_link">Preview
             </a></td>
</tr>
<tr>
<td class="content_header_td" onMouseOver="return overlib('This field indicates the sense level that image movement will be detected.', LEFT);" onMouseOut="return nd();">Sense Level:
           </td><td class="content_input_td"><select name="usb_websense_x" class="content_input_fd" onChange="return change_common(this, 'PrinterStatus', 'usb_websense_x')"><option class="content_input_fd" value="0" <% nvram_match_x("PrinterStatus","usb_websense_x", "0","selected"); %>>Low</option><option class="content_input_fd" value="1" <% nvram_match_x("PrinterStatus","usb_websense_x", "1","selected"); %>>Medium</option><option class="content_input_fd" value="2" <% nvram_match_x("PrinterStatus","usb_websense_x", "2","selected"); %>>High</option></select></td>
</tr>
<tr>
<td class="content_header_td" onMouseOver="return overlib('This field indicates the image refresh rate per second. The range of value is 1~30.', LEFT);" onMouseOut="return nd();">Frames Per Second:
           </td><td class="content_input_td"><input type="text" maxlength="5" size="5" name="usb_webfresh_x" class="content_input_fd" value="<% nvram_get_x("PrinterStatus", "usb_webfresh_x"); %>" onBlur="validate_range(this, 1, 30)" onKeyPress="return is_number(event, this)"></td>
</tr>
<tr>
<td class="content_header_td" onMouseOver="return overlib('This field indicates the JPEG compression quality in percent. The range of value is 1~100.', LEFT);" onMouseOut="return nd();">JPEG compression quality:
           </td><td class="content_input_td"><input type="text" maxlength="5" size="5" name="usb_webquality_x" class="content_input_fd" value="<% nvram_get_x("PrinterStatus", "usb_webquality_x"); %>" onBlur="validate_range(this, 1, 100)" onKeyPress="return is_number(event, this)"></td>
</tr>
<tr>
<td class="content_header_td" onMouseOver="return overlib('This field indicates the port that http server listens on to communicate.', LEFT);" onMouseOut="return nd();">HTTP Port:
           </td><td class="content_input_td"><input type="text" maxlength="5" size="5" name="usb_webhttpport_x" class="content_input_fd" value="<% nvram_get_x("PrinterStatus", "usb_webhttpport_x"); %>" onBlur="validate_range(this, 1024, 65535)" onKeyPress="return is_number(event, this)"><input type="checkbox" style="margin-left:30" name="usb_webhttpport_x_check" value="" onClick="return change_common_radio(this, 'PrinterStatus', 'usb_webhttpport_x', '1')">Password Checking</input></td>
</tr>
<tr>
<td class="content_header_td">User Name:</td>
<td class="content_input_td"><input type="text" maxlength="32" class="content_input_fd" size="16" name="usb_webhttp_username" value="<% nvram_get_x("PrinterStatus","usb_webhttp_username"); %>" onkeypress="return is_string(event, this)" onblur="validate_string(this)"></td>
</tr>
<tr>
<td class="content_header_td">Password:</td>
<td class="content_input_td"><input type="password" maxlength="32" class="content_input_fd" size="16" name="usb_webhttp_passwd" value="<% nvram_get_x("PrinterStatus","usb_webhttp_passwd"); %>" onblur="validate_string(this)"></td>
</tr>
<tr class="content_section_header_tr">
<td class="content_section_header_td" colspan="2">Security Mode Setting
            </td>
</tr>
<tr>
<td class="content_desc_td" colspan="2">This function allows you to monitor your environment through Web Camera. If there is any motion detected, ZVMODELVZ will try to alert you by means of e-mail.
         </td>
</tr>
<tr>
<td class="content_header_td">Enable Security Mode?
           </td><td class="content_input_td"><input type="radio" value="1" name="usb_websecurity_x" class="content_input_fd" onClick="return change_common_radio(this, 'PrinterStatus', 'usb_websecurity_x', '1')" <% nvram_match_x("PrinterStatus","usb_websecurity_x", "1", "checked"); %>>Yes</input><input type="radio" value="0" name="usb_websecurity_x" class="content_input_fd" onClick="return change_common_radio(this, 'PrinterStatus', 'usb_websecurity_x', '0')" <% nvram_match_x("PrinterStatus","usb_websecurity_x", "0", "checked"); %>>No</input></td>
</tr>
<tr>
<td class="content_header_td" onMouseOver="return overlib('This field defines the dates that Security Mode will be enabled.', LEFT);" onMouseOut="return nd();">Date to Enable Security Mode:
           </td><td class="content_input_td"><input type="hidden" maxlength="7" class="content_input_fd" size="7" name="usb_websecurity_date_x" value="<% nvram_get_x("PrinterStatus","usb_websecurity_date_x"); %>">
<p style="word-spacing: 0; margin-top: 0; margin-bottom: 0">
<input type="checkbox" class="content_input_fd" name="usb_websecurity_date_x_Sun">Sun</input><input type="checkbox" class="content_input_fd" name="usb_websecurity_date_x_Mon">Mon</input><input type="checkbox" class="content_input_fd" name="usb_websecurity_date_x_Tue">Tue</input><input type="checkbox" class="content_input_fd" name="usb_websecurity_date_x_Wed">Wed</input>
</p>
<input type="checkbox" class="content_input_fd" name="usb_websecurity_date_x_Thu">Thu</input><input type="checkbox" class="content_input_fd" name="usb_websecurity_date_x_Fri">Fri</input><input type="checkbox" class="content_input_fd" name="usb_websecurity_date_x_Sat">Sat</input></td>
</tr>
<tr>
<td class="content_header_td" onMouseOver="return overlib('This field defines the time interval that Security Mode will be enabled.', LEFT);" onMouseOut="return nd();">Time to Enable Security Mode:
           </td><td class="content_input_td"><input type="hidden" maxlength="11" class="content_input_fd" size="11" name="usb_websecurity_time_x" value="<% nvram_get_x("PrinterStatus","usb_websecurity_time_x"); %>"><input type="text" maxlength="2" class="content_input_fd" size="2" name="usb_websecurity_time_x_starthour" onKeyPress="return is_number(event, this)" onBlur="return validate_timerange(this, 0)">:
                <input type="text" maxlength="2" class="content_input_fd" size="2" name="usb_websecurity_time_x_startmin" onKeyPress="return is_number(event, this)" onBlur="return validate_timerange(this, 1)">-
                <input type="text" maxlength="2" class="content_input_fd" size="2" name="usb_websecurity_time_x_endhour" onKeyPress="return is_number(event, this)" onBlur="return validate_timerange(this, 2)">:
                <input type="text" maxlength="2" class="content_input_fd" size="2" name="usb_websecurity_time_x_endmin" onKeyPress="return is_number(event, this)" onBlur="return validate_timerange(this, 3)"></td>
</tr>
<tr>
<td class="content_header_td" onMouseOver="return overlib('This field indicates the e-mail address you like to send to.', LEFT);" onMouseOut="return nd();">Send to:
           </td><td class="content_input_td"><input type="text" maxlength="256" class="content_input_fd" size="32" name="usb_websendto_x" value="<% nvram_get_x("PrinterStatus","usb_websendto_x"); %>" onKeyPress="return is_string(event, this)" onBlur="validate_string(this)"></td>
</tr>
<tr>
<td class="content_header_td" onMouseOver="return overlib('This field indicates the email server where you like to deliver your e-mail to. If you leave this field blank, ZVMODELVZ will find a Mail Exchanger from your e-mail address below.', LEFT);" onMouseOut="return nd();">Email Server:
           </td><td class="content_input_td"><input type="text" maxlength="256" class="content_input_fd" size="32" name="usb_webmserver_x" value="<% nvram_get_x("PrinterStatus","usb_webmserver_x"); %>" onKeyPress="return is_string(event, this)" onBlur="validate_string(this)"></td>
</tr>
<tr>
<td class="content_header_td" onMouseOver="return overlib('This field allows you to edit subject of e-mail.', LEFT);" onMouseOut="return nd();">Subject:
           </td><td class="content_input_td"><input type="text" maxlength="256" class="content_input_fd" size="32" name="usb_websubject_x" value="<% nvram_get_x("PrinterStatus","usb_websubject_x"); %>" onKeyPress="return is_string(event, this)" onBlur="validate_string(this)"></td>
</tr>
<tr>
<td class="content_header_td" onMouseOver="return overlib('This field allows you to attach detected image file into e-mail.', LEFT);" onMouseOut="return nd();">Attach Image File?
           </td><td class="content_input_td"><input type="radio" value="1" name="usb_webattach_x" class="content_input_fd" onClick="return change_common_radio(this, 'PrinterStatus', 'usb_webattach_x', '1')" <% nvram_match_x("PrinterStatus","usb_webattach_x", "1", "checked"); %>>Yes</input><input type="radio" value="0" name="usb_webattach_x" class="content_input_fd" onClick="return change_common_radio(this, 'PrinterStatus', 'usb_webattach_x', '0')" <% nvram_match_x("PrinterStatus","usb_webattach_x", "0", "checked"); %>>No</input></td>
</tr>
<tr class="content_section_header_tr">
<td class="content_section_header_td" colspan="2">Remote Monitor Setting
            </td>
</tr>
<tr>
<td class="content_desc_td" colspan="2">This function allows you to monitor up to 6 Web Cameras in your LAN. Please enter the IP addresses of the wireless routers which connect with Web Camera. If you want to expose these web camera to Internet, please set up virtual server for each HTTP port and ActiveX port manually.
         </td>
</tr>
<tr>
<td class="content_header_td" onMouseOver="return overlib('Selecting LAN Only you can only monitor within LAN environment. If you want to monitor from Internet, please set up http port and activex in virtual server list manually. Please consider security issue.', LEFT);" onMouseOut="return nd();">Remote Control Mode:
           </td><td class="content_input_td"><select name="usb_webremote_x" class="content_input_fd" onChange="return change_common(this, 'PrinterStatus', 'usb_webremote_x')"><option class="content_input_fd" value="0" <% nvram_match_x("PrinterStatus","usb_webremote_x", "0","selected"); %>>LAN Only</option></select><a href="javascript:openLink('x_WRemote')" class="content_input_link" name="x_WRemote_link">Preview
             </a></td>
</tr>
<tr>
<td class="content_header_td">Remote Site 1:
           </td><td class="content_input_td"><input type="text" maxlength="21" class="content_input_fd" size="15" name="usb_webremote1_x" value="<% nvram_get_x("PrinterStatus","usb_webremote1_x"); %>" onBlur="return validate_ipaddrport(this, 'usb_webremote1_x')" onKeyPress="return is_ipaddrport(event, this)" onKeyUp="change_ipaddrport(this)"></td>
</tr>
<tr>
<td class="content_header_td">Remote Site 2:
           </td><td class="content_input_td"><input type="text" maxlength="21" class="content_input_fd" size="15" name="usb_webremote2_x" value="<% nvram_get_x("PrinterStatus","usb_webremote2_x"); %>" onBlur="return validate_ipaddrport(this, 'usb_webremote2_x')" onKeyPress="return is_ipaddrport(event, this)" onKeyUp="change_ipaddrport(this)"></td>
</tr>
<tr>
<td class="content_header_td">Remote Site 3:
           </td><td class="content_input_td"><input type="text" maxlength="21" class="content_input_fd" size="15" name="usb_webremote3_x" value="<% nvram_get_x("PrinterStatus","usb_webremote3_x"); %>" onBlur="return validate_ipaddrport(this, 'usb_webremote3_x')" onKeyPress="return is_ipaddrport(event, this)" onKeyUp="change_ipaddrport(this)"></td>
</tr>
<tr>
<td class="content_header_td">Remote Site 4:
           </td><td class="content_input_td"><input type="text" maxlength="21" class="content_input_fd" size="15" name="usb_webremote4_x" value="<% nvram_get_x("PrinterStatus","usb_webremote4_x"); %>" onBlur="return validate_ipaddrport(this, 'usb_webremote4_x')" onKeyPress="return is_ipaddrport(event, this)" onKeyUp="change_ipaddrport(this)"></td>
</tr>
<tr>
<td class="content_header_td">Remote Site 5:
           </td><td class="content_input_td"><input type="text" maxlength="21" class="content_input_fd" size="15" name="usb_webremote5_x" value="<% nvram_get_x("PrinterStatus","usb_webremote5_x"); %>" onBlur="return validate_ipaddrport(this, 'usb_webremote5_x')" onKeyPress="return is_ipaddrport(event, this)" onKeyUp="change_ipaddrport(this)"></td>
</tr>
<tr>
<td class="content_header_td">Remote Site 6:
           </td><td class="content_input_td"><input type="text" maxlength="21" class="content_input_fd" size="15" name="usb_webremote6_x" value="<% nvram_get_x("PrinterStatus","usb_webremote6_x"); %>" onBlur="return validate_ipaddrport(this, 'usb_webremote6_x')" onKeyPress="return is_ipaddrport(event, this)" onKeyUp="change_ipaddrport(this)"></td>
</tr>
</table>

<table width="666" border="1" cellpadding="0" cellspacing="0" bordercolor="#B0B0B0">
<tr bgcolor="#CCCCCC"><td colspan="3"><font face="arial" size="2">&nbsp;</font></td></tr>
<tr bgcolor="#FFFFFF">  
   <td id ="Confirm" height="25" width="34%">  
   <div align="center"><font face="Arial"> <input class=inputSubmit onMouseOut="buttonOut(this)" onMouseOver="buttonOver(this)" onClick="onSubmitCtrl(this, ' Restore ')" type="submit" value=" Restore " name="action"></font></div> 
   </td>  
   <td height="25" width="33%">  
   <div align="center"><font face="Arial"> <input class=inputSubmit onMouseOut="buttonOut(this)" onMouseOver="buttonOver(this)" onClick="onSubmitCtrl(this, ' Finish ')" type="submit" value=" Finish " name="action"></font></div> 
   </td>
   <td height="25" width="33%">  
   <div align="center"><font face="Arial"> <input class=inputSubmit onMouseOut="buttonOut(this)" onMouseOver="buttonOver(this)" onClick="onSubmitCtrl(this, ' Apply ')" type="submit" value=" Apply " name="action"></font></div> 
   </td>    
</tr>
</table>

<table width="666" border="1" cellpadding="0" cellspacing="0" bordercolor="#B0B0B0">
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

</form>
</body>
</html>
