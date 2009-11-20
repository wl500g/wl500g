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
<input type="hidden" name="current_page" value="Advanced_WiMax_Content.asp">
<input type="hidden" name="next_page" value="Advanced_DHCP_Content.asp">
<input type="hidden" name="next_host" value="">
<input type="hidden" name="sid_list" value="WiMaxConfig;Layer3Forwarding;LANHostConfig;IPConnection;PPPConnection;">
<input type="hidden" name="group_id" value="">
<input type="hidden" name="modified" value="0">
<input type="hidden" name="action_mode" value="">
<input type="hidden" name="first_time" value="">
<input type="hidden" name="action_script" value="">
<tr>
<td>
<table width="666" border="1" cellpadding="0" cellspacing="0" bordercolor="E0E0E0">
<tr class="content_header_tr">
<td class="content_header_td_title" colspan="2">IP Config - WiMAX</td>
</tr>
<tr>
<td class="content_desc_td" colspan="2">ZVMODELVZ supports WiMAX connection to WAN.</td>
</tr>

<tr>
<td class="content_header_td">WiMAX autorun?</td>
<td class="content_input_td">
<input type="radio" value="1" name="wimax_enable" class="content_input_fd" <% nvram_match_x("WiMaxConfig","wimax_enable", "1", "checked"); %>>Yes</input>
<input type="radio" value="0" name="wimax_enable" class="content_input_fd" <% nvram_match_x("WiMaxConfig","wimax_enable", "0", "checked"); %>>No</input>
</td>
</tr>

<tr>
<td class="content_header_td" onMouseOver="return overlib('This field allows you to provide a SSID for for WiMAX network. Default: @yota.ru', LEFT);" onMouseOut="return nd();">SSID:</td>
<td class="content_input_td">
<input type="text" maxlength="32" class="content_input_fd" size="32" name="wimax_ssid" value="<% nvram_get_x("WiMaxConfig","wimax_ssid"); %>" onKeyPress="return is_string(this)" onBlur="validate_string(this)">
</td>
</tr>

<tr>
<td class="content_header_td" onMouseOver="return overlib('This is the priority of default gateway (1-10).', LEFT);" onMouseOut="return nd();">Priority:</td>
<td class="content_input_td">
<input type="text" maxlength="3" class="content_input_fd" size="3" name="wimax_priority" value="<% nvram_get_x("WiMaxConfig","wimax_priority"); %>" onBlur="return validate_range(this, 1, 10)" onKeyPress="return is_number(this)"</td>
</tr>

</table>
</td></tr>

<tr><td>		
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
</td></tr>
</table>
</td></tr>

<tr><td>
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
</td></tr>

</table>
</form>
</body>
