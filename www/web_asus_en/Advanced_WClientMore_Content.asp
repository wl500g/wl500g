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
    	
<input type="hidden" name="current_page" value="Advanced_WClientMore_Content.asp"><input type="hidden" name="next_page" value="Advanced_WClient_Content.asp"><input type="hidden" name="next_host" value=""><input type="hidden" name="sid_list" value="WLANAuthentication11a;WLANConfig11b;"><input type="hidden" name="group_id" value=""><input type="hidden" name="modified" value="0"><input type="hidden" name="action_mode" value=""><input type="hidden" name="first_time" value=""><input type="hidden" name="action_script" value="">
<tr>
<td>
<table width="666" border="1" cellpadding="0" cellspacing="0" bordercolor="E0E0E0">
<tr class="content_header_tr">
<td class="content_header_td_title" colspan="2">Wireless - Client Mode</td>
</tr>
<input type="hidden" name="WLANConfig11b_Country" value="<% nvram_get_x("WLANConfig11b","Country"); %>"><input type="hidden" name="WLANConfig11b_x_Mode11g" value="<% nvram_get_x("WLANConfig11b","x_Mode11g"); %>">
<tr>
<td class="content_header_td">SSID
           </td><td class="content_input_td"><input type="text" maxlength="256" class="content_input_fd" size="32" name="WLANConfig11b_x_CSSID" value="<% nvram_get_x("WLANConfig11b","x_CSSID"); %>" onKeyPress="return is_string(this)" onBlur="validate_string(this)"></td>
</tr>
<tr>
<td class="content_header_td" onMouseOver="return overlib('WLANConfig11b_x_CChannel_itemdesc', LEFT);" onMouseOut="return nd();">Channel
           </td><td class="content_input_td"><select name="WLANConfig11b_x_CChannel" class="content_input_fd" onChange="return change_common(this, 'WLANConfig11b', 'x_CChannel')">   
<% select_channel("WLANConfig11b"); %>
                </select><input type="hidden" maxlength="15" size="15" name="WLANConfig11b_x_RegulatoryDomain" value="<% nvram_get_x("Regulatory","x_RegulatoryDomain"); %>" readonly="1"></td>
</tr>
<tr>
<td class="content_header_td">Network Type
           </td><td class="content_input_td"><select name="WLANConfig11b_x_CNetType" class="content_input_fd" onChange="return change_common(this, 'WLANConfig11b', 'x_CNetType')"><option class="content_input_fd" value="0" <% nvram_match_x("WLANConfig11b","x_CNetType", "0","selected"); %>>Infra Structure</option><option class="content_input_fd" value="1" <% nvram_match_x("WLANConfig11b","x_CNetType", "1","selected"); %>>Ad Hoc</option></select></td>
</tr>
<tr>
<td class="content_header_td" onMouseOver="return overlib('WLANConfig11b_x_CAuth_itemdesc', LEFT);" onMouseOut="return nd();">Authenication Method
           </td><td class="content_input_td"><select name="WLANConfig11b_x_CAuth" class="content_input_fd" onChange="return change_common(this, 'WLANConfig11b', 'x_CAuth')"><option class="content_input_fd" value="0" <% nvram_match_x("WLANConfig11b","x_CAuth", "0","selected"); %>>Open System</option><option class="content_input_fd" value="1" <% nvram_match_x("WLANConfig11b","x_CAuth", "1","selected"); %>>Shared Key</option></select></td>
</tr>
<tr>
<td class="content_header_td" onMouseOver="return overlib('WLANConfig11b_x_CWEPType_itemdesc', LEFT);" onMouseOut="return nd();">Encryption Type
           </td><td class="content_input_td"><select name="WLANConfig11b_x_CWEPType" class="content_input_fd" onChange="return change_wlweptype(this, 'WLANConfig11b')"><option class="content_input_fd" value="0" <% nvram_match_x("WLANConfig11b","x_CWEPType", "0","selected"); %>>None</option><option class="content_input_fd" value="1" <% nvram_match_x("WLANConfig11b","x_CWEPType", "1","selected"); %>>WEP-64bits</option><option class="content_input_fd" value="2" <% nvram_match_x("WLANConfig11b","x_CWEPType", "2","selected"); %>>WEP-128bits</option></select></td>
</tr>
<tr>
<td class="content_header_td" onMouseOver="return overlib('WLANConfig11b_x_CPhrase_itemdesc', LEFT);" onMouseOut="return nd();">Passphrase
           </td><td class="content_input_td"><script language="JavaScript" type="text/javascript" src="phrase.js"></script><script language="JavaScript" type="text/javascript" src="md5.js"></script><input type="password" maxlength="64" class="content_input_fd" size="32" name="WLANConfig11b_x_CPhrase" value="<% nvram_get_x("WLANConfig11b","x_CPhrase"); %>" onKeyUp="return is_wlphrase('WLANConfig11b',this)" onBlur="return validate_wlphrase('WLANConfig11b', this)"></td>
</tr>
<tr>
<td class="content_header_td">WEP Key 1
           </td><td class="content_input_td"><input type="pssword" maxlength="32" class="content_input_fd" size="32" name="WLANConfig11b_x_CWEPKey1" value="<% nvram_get_x("WLANConfig11b","x_CWEPKey1"); %>" onBlur="return validate_wlkey(this, 'WLANConfig11b')" onKeyPress="return is_wlkey(this, 'WLANConfig11b')" onKeyUp="return change_wlkey(this, 'WLANConfig11b')"></td>
</tr>
<tr>
<td class="content_header_td">WEP Key 2
           </td><td class="content_input_td"><input type="pssword" maxlength="32" class="content_input_fd" size="32" name="WLANConfig11b_x_CWEPKey2" value="<% nvram_get_x("WLANConfig11b","x_CWEPKey2"); %>" onBlur="return validate_wlkey(this, 'WLANConfig11b')" onKeyPress="return is_wlkey(this, 'WLANConfig11b')" onKeyUp="return change_wlkey(this, 'WLANConfig11b')"></td>
</tr>
<tr>
<td class="content_header_td">WEP Key 3
           </td><td class="content_input_td"><input type="pssword" maxlength="32" class="content_input_fd" size="32" name="WLANConfig11b_x_CWEPKey3" value="<% nvram_get_x("WLANConfig11b","x_CWEPKey3"); %>" onBlur="return validate_wlkey(this, 'WLANConfig11b')" onKeyPress="return is_wlkey(this, 'WLANConfig11b')" onKeyUp="return change_wlkey(this, 'WLANConfig11b')"></td>
</tr>
<tr>
<td class="content_header_td">WEP Key 4
           </td><td class="content_input_td"><input type="pssword" maxlength="32" class="content_input_fd" size="32" name="WLANConfig11b_x_CWEPKey4" value="<% nvram_get_x("WLANConfig11b","x_CWEPKey4"); %>" onBlur="return validate_wlkey(this, 'WLANConfig11b')" onKeyPress="return is_wlkey(this, 'WLANConfig11b')" onKeyUp="return change_wlkey(this, 'WLANConfig11b')"></td>
</tr>
<tr>
<td class="content_header_td" onMouseOver="return overlib('WLANConfig11b_x_CWEPDefaultKey_itemdesc', LEFT);" onMouseOut="return nd();">Default Key
           </td><td class="content_input_td"><select name="WLANConfig11b_x_CWEPDefaultKey" class="content_input_fd" onChange="return change_common(this, 'WLANConfig11b', 'x_CWEPDefaultKey')"><option class="content_input_fd" value="1" <% nvram_match_x("WLANConfig11b","x_CWEPDefaultKey", "1","selected"); %>>Key1</option><option class="content_input_fd" value="2" <% nvram_match_x("WLANConfig11b","x_CWEPDefaultKey", "2","selected"); %>>Key2</option><option class="content_input_fd" value="3" <% nvram_match_x("WLANConfig11b","x_CWEPDefaultKey", "3","selected"); %>>Key3</option><option class="content_input_fd" value="4" <% nvram_match_x("WLANConfig11b","x_CWEPDefaultKey", "4","selected"); %>>Key4</option></select></td>
</tr>
</table>
</td>
</tr>
<tr>
<td>
<table width="666" border="2" cellpadding="0" cellspacing="0" bordercolor="E0E0E0"></table>
</td>
</tr>
<tr>
<td>
<table width="666" border="2" cellpadding="0" cellspacing="0" bordercolor="E0E0E0"></table>
</td>
</tr>
<tr>
<td>		
<table width="666" border="1" cellpadding="0" cellspacing="0" bordercolor="B0B0B0">
<tr bgcolor="#CCCCCC"><td colspan="3"><font face="arial" size="2"><b>&nbsp</b></font></td></tr>
<tr bgcolor="#FFFFFF">  
   <td id ="Confirm" height="25" width="34%">     
   </td>  
   <td height="25" width="33%">     
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
    <td class="content_header_td_15" align="left">Apply: </td>
    <td class="content_input_td_padding" align="left">Confirm above settings and continue.</td>
</tr>
</table>
</td>
</tr>

</table>
</form>
</body>
