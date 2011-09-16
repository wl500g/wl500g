<html>
<head>
<title>ZVMODELVZ Web Manager</title>
<link rel="stylesheet" type="text/css" href="style.css" media="screen">
<script type="text/javascript" src="overlib.js"></script>
<script type="text/javascript" src="general.js"></script>
</head>  
<body onLoad="load_body()" onunLoad="return unload_body();">
<div id="overDiv" style="position:absolute; visibility:hidden; z-index:1000;"></div>    
<form method="GET" name="form" action="apply.cgi">
<input type="hidden" name="current_page" value="Advanced_Authentication_Content.asp">
<input type="hidden" name="next_page" value="Advanced_UniqueKey_Content.asp">
<input type="hidden" name="next_host" value="">
<input type="hidden" name="sid_list" value="WLANAuthentication11a;">
<input type="hidden" name="group_id" value="">
<input type="hidden" name="modified" value="0">
<input type="hidden" name="action_mode" value="">
<input type="hidden" name="action_script" value="">
<!-- Table for the conntent page -->	    
<table width="666" border="1" cellpadding="0" cellspacing="0" bordercolor="#E0E0E0">
<tr class="content_header_tr">
<td class="content_header_td_title" colspan="2">Wireless - 802.1x</td>
</tr>
<tr>
<td class="content_header_td" onMouseOver="return overlib('Specifies the type of authentication of 802.1x', LEFT);" onMouseOut="return nd();">802.1x Authentication Type:</td><td class="content_input_td"><select name="WLANAuthentication11a_AuthDatabaseType" class="content_input_fd" onChange="return change_common(this, 'WLANAuthentication11a', 'AuthDatabaseType')"><option class="content_input_fd" value="EAP" <% nvram_match_x("WLANAuthentication11a","AuthDatabaseType", "EAP","selected"); %>>EAP</option><option class="content_input_fd" value="MAC" <% nvram_match_x("WLANAuthentication11a","AuthDatabaseType", "MAC","selected"); %>>MAC</option></select></td>
</tr>
<tr>
<td class="content_header_td" onMouseOver="return overlib('To specify the AP access a locally maintained (in the AP) authentication database when a supplicant authenticates with the AP', LEFT);" onMouseOut="return nd();">Enable Local Database?</td><td class="content_input_td"><input type="radio" value="1" name="WLANAuthentication11a_UseLocalAuthDatabase" class="content_input_fd" onClick="return change_common_radio(this, 'WLANAuthentication11a', 'UseLocalAuthDatabase', '1')" <% nvram_match_x("WLANAuthentication11a","UseLocalAuthDatabase", "1", "checked"); %>>Yes</input><input type="radio" value="0" name="WLANAuthentication11a_UseLocalAuthDatabase" class="content_input_fd" onClick="return change_common_radio(this, 'WLANAuthentication11a', 'UseLocalAuthDatabase', '0')" <% nvram_match_x("WLANAuthentication11a","UseLocalAuthDatabase", "0", "checked"); %>>No</input></td>
</tr>
</table>

<table width="666" border="2" cellpadding="0" cellspacing="0" bordercolor="#E0E0E0">
<tr class="content_list_header_tr">
<td class="content_list_header_td" width="60%" id="ExAuthDB">Authentication Server List<input type="hidden" name="WLANAuthentication11a_ExAuthDBCount_0" value="<% nvram_get_x("WLANAuthentication11a", "ExAuthDBCount"); %>" readonly></td><td width="10%">
<div align="center">
<input class="inputSubmit" type="submit" onMouseOut="buttonOut(this)" onMouseOver="buttonOver(this)" onClick="return markGroup(this, 'WLANAuthentication11a_ExAuthDB', 4);" name="WLANAuthentication11a_ExAuthDB" value=" Add " size="12">
</div>
</td><td width="10%">
<div align="center">
<input class="inputSubmit" type="submit" onMouseOut="buttonOut(this)" onMouseOver="buttonOver(this)" onClick="return markGroup(this, 'WLANAuthentication11a_ExAuthDB', 4);" name="WLANAuthentication11a_ExAuthDB" value=" Del " size="12">
</div>
</td><td width="5%"></td>
</tr>
<table class="content_list_table" width="640" border="0" cellspacing="0" cellpadding="0">
<tr>
<td colspan="3" height="10"></td>
</tr>
<tr>
<td colspan="3">
<div align="center">
<table class="content_list_value_table" border="1" cellspacing="0" cellpadding="0">
<tr>
<td></td><td class="content_list_field_header_td" colspan="" onMouseOver="return overlib('A string value that specifies the IP address of the external authentication database', ABOVE);" onMouseOut="return nd();">IP Address</td><td class="content_list_field_header_td" colspan="" onMouseOver="return overlib('A string that value specifies the port number of the external authentication database', ABOVE);" onMouseOut="return nd();">Port</td><td class="content_list_field_header_td" colspan="" onMouseOver="return overlib('Specifies the type of external authentication database', ABOVE);" onMouseOut="return nd();">Type</td><td class="content_list_field_header_td" colspan="" onMouseOver="return overlib('A string that value specifies the username to use during the login process to the external authentication database', ABOVE);" onMouseOut="return nd();">Login name</td><td class="content_list_field_header_td" colspan="" onMouseOver="return overlib('A string that value specifies the password to use during the login process to the external authentication database', ABOVE);" onMouseOut="return nd();">Secret</td><td></td>
</tr>
<tr>
<td></td><td class="content_list_input_td" colspan=""><input type="text" maxlength="15" class="content_input_list_fd" size="12" name="WLANAuthentication11a_ExAuthDBIPAddr_0" onBlur="return validate_ipaddr(this, 'ExAuthDBIPAddr')" onKeyPress="return is_ipaddr(this)" onKeyUp="change_ipaddr(this)"></td><td class="content_list_input_td" colspan=""><input type="text" maxlength="5" size="3" class="content_input_list_fd" name="WLANAuthentication11a_ExAuthDBPortNumber_0" onBlur="validate_range(this, 0, 65535)" onKeyPress="return is_number(this)"></td><td class="content_list_input_td" colspan=""><select name="WLANAuthentication11a_ExAuthDBType_0" class="content_input_list_fd"><option value="RADIUS" <% nvram_match_list_x("WLANAuthentication11a","ExAuthDBType", "RADIUS","selected", 0); %>>RADIUS</option></select></td><td class="content_list_input_td" colspan=""><input type="text" maxlength="15" size="15" name="WLANAuthentication11a_ExAuthDBUsername_0" class="content_input_list_fd" onKeyPress="is_string(this)"></td><td class="content_list_input_td" colspan=""><input type="text" maxlength="15" size="15" name="WLANAuthentication11a_ExAuthDBPassword_0" class="content_input_list_fd" onKeyPress="is_string(this)"></td>
</tr>
<tr>
<td></td><td colspan="10"><select size="4" name="WLANAuthentication11a_ExAuthDB_s" multiple="true" style="font-family: 'monospace'; font-size: '8pt'; width: 100%">
<% nvram_get_table_x("WLANAuthentication11a","ExAuthDB"); %>
</select></td>
</tr>
</table>
</div>
</td>
</tr>
<tr>
<td colspan="3" height="10"></td>
</tr>
</table>
</table>

<table width="666" border="2" cellpadding="0" cellspacing="0" bordercolor="#E0E0E0">
<tr class="content_list_header_tr">
<td class="content_list_header_td" width="60%" id="LocalAuthDB">Local Database<input type="hidden" name="WLANAuthentication11a_AuthDBNumberOfEntries_0" value="<% nvram_get_x("WLANAuthentication11a", "AuthDBNumberOfEntries"); %>" readonly></td><td width="10%">
<div align="center">
<input class="inputSubmit" type="submit" onMouseOut="buttonOut(this)" onMouseOver="buttonOver(this)" onClick="return markGroup(this, 'WLANAuthentication11a_LocalAuthDB', 32);" name="WLANAuthentication11a_LocalAuthDB" value=" Add " size="12">
</div>
</td><td width="10%">
<div align="center">
<input class="inputSubmit" type="submit" onMouseOut="buttonOut(this)" onMouseOver="buttonOver(this)" onClick="return markGroup(this, 'WLANAuthentication11a_LocalAuthDB', 32);" name="WLANAuthentication11a_LocalAuthDB" value=" Del " size="12">
</div>
</td><td width="5%"></td>
</tr>
<table class="content_list_table" width="640" border="0" cellspacing="0" cellpadding="0">
<tr>
<td colspan="3" height="10"></td>
</tr>
<tr>
<td colspan="3">
<div align="center">
<table class="content_list_value_table" border="1" cellspacing="0" cellpadding="0">
<tr>
<td></td><td class="content_list_field_header_td" colspan="">Secret Type</td><td class="content_list_field_header_td" colspan="">Auth Type</td><td class="content_list_field_header_td" colspan="" onMouseOver="return overlib('A string value that specifies a username string.  This string should be matched by the AP when a client supplies the EAP-Identity string', ABOVE);" onMouseOut="return nd();">User Name</td><td class="content_list_field_header_td" colspan="">Password</td><td></td>
</tr>
<tr>
<td></td><td class="content_list_input_td" colspan=""><select name="WLANAuthentication11a_LocalSecretType_0" class="content_input_list_fd"><option value="Password" <% nvram_match_list_x("WLANAuthentication11a","LocalSecretType", "Password","selected", 0); %>>Password</option><option value="Certificate" <% nvram_match_list_x("WLANAuthentication11a","LocalSecretType", "Certificate","selected", 0); %>>Certificate</option></select></td><td class="content_list_input_td" colspan=""><select name="WLANAuthentication11a_LocalSecretAuthType_0" class="content_input_list_fd"><option value="EAP_MD5" <% nvram_match_list_x("WLANAuthentication11a","LocalSecretAuthType", "EAP_MD5","selected", 0); %>>EAP_MD5</option><option value="EAP_TLS" <% nvram_match_list_x("WLANAuthentication11a","LocalSecretAuthType", "EAP_TLS","selected", 0); %>>EAP_TLS</option></select></td><td class="content_list_input_td" colspan=""><input type="text" maxlength="16" size="18" name="WLANAuthentication11a_AuthDBUsername_0" class="content_input_list_fd" onKeyPress="is_string(this)"></td><td class="content_list_input_td" colspan=""><input type="text" maxlength="16" size="18" name="WLANAuthentication11a_AuthDBSecret_0" class="content_input_list_fd" onKeyPress="is_string(this)"></td>
</tr>
<tr>
<td></td><td colspan="10"><select size="8" name="WLANAuthentication11a_LocalAuthDB_s" multiple="true" class="content_list_body">
<% nvram_get_table_x("WLANAuthentication11a","LocalAuthDB"); %>
</select></td>
</tr>
</table>
</div>
</td>
</tr>
<tr>
<td colspan="3" height="10"></td>
</tr>
</table>
</table>

<table width="666" border="1" cellpadding="0" cellspacing="0" bordercolor="#B0B0B0">
<tr bgcolor="#CCCCCC"><td colspan="3"><font face="arial" size="2"><b>&nbsp;</b></font></td></tr>
<tr bgcolor="#FFFFFF">  
   <td id ="Confirm" height="25" width="34%">  
   <div align="center"><font face="Arial"> <input class=inputSubmit onMouseOut="buttonOut(this)" onMouseOver="buttonOver(this)" onClick="onSubmit()" type="submit" value=" Restore " name="action"></font></div> 
   </td>  
   <td height="25" width="33%">  
   <div align="center"><font face="Arial"> <input class=inputSubmit onMouseOut="buttonOut(this)" onMouseOver="buttonOver(this)" onClick="onSubmit()" type="submit" value=" Finish " name="action"></font></div> 
   </td>
   <td height="25" width="33%">  
   <div align="center"><font face="Arial"> <input class=inputSubmit onMouseOut="buttonOut(this)" onMouseOver="buttonOver(this)" onClick="onSubmit()" type="submit" value=" Apply " name="action"></font></div> 
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
