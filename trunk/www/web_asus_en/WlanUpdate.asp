<head>
<title>ZVMODELVZ Web Manager</title>
<meta http-equiv="Content-Type" content="text/html; charset=ISO-8859-1">
<meta http-equiv="Content-Type" content="text/html; charset=UTF-8">
<meta http-equiv="Content-Type" content="text/html; charset=gb2312">
<link rel="stylesheet" type="text/css" href="style.css" media="screen">
<script language="JavaScript" type="text/javascript" src="overlib.js"></script>
<script language="JavaScript" type="text/javascript" src="general.js"></script>
<script language="JavaScript">

function loadWLANUpdate()
{
   if (document.form.productid.value=='WL500g' ||
       document.form.productid.value=='WL500g.Deluxe')
   {	

	document.form.action_script.value="wlan_update";
	return;
   }
   document.form.action.disabled = 1;
   return;
}

</script>
</head>  
<body bgcolor="#FFFFFF" onLoad="loadWLANUpdate()">
<div id="overDiv" style="position:absolute; visibility:hidden; z-index:1000;"></div>
<form method="GET" name="form" action="apply.cgi">
<input type="hidden" name="action_mode" value="">
<!-- Table for the conntent page -->	    
<table width="660" border="0" cellpadding="1" cellspacing="0">
<tr>
<td height="96" width="666">		
<table width="666" border="1" cellpadding="0" cellspacing="0" bordercolor="#B0B0B0">
<tr class="content_header_tr">
<td colspan="2" height="25" width="666">WLAN Update</td>
<input type="hidden" name="current_page" value="WlanUpdate.asp">
<input type="hidden" name="next_page" value="Basic_SaveRestart.asp">
<input type="hidden" name="sid_list" value="General;">
<input type="hidden" name="group_id" value="">
<input type="hidden" name="modified" value="0">
<input type="hidden" name="action_script" value="">
<input type="hidden" name="productid" value="<% nvram_get_x("", "productid"); %>">
<input type="hidden" name="wleeprom1" value="<% nvram_get_x("Layer3Forwarding","wleeprom1"); %>">
<input type="hidden" name="wleeprom2" value="<% nvram_get_x("Layer3Forwarding","wleeprom2"); %>">
</tr>
<tr bgcolor="#FFFFFF"><td class="content_desc_td"><font face="arial" size="2"><b>&nbsp</b></font>
    <p style="margin-top: 0; margin-bottom: 0">Press Update to save new EEPROM value for this firmware.</p>      
    <p style="margin-top: 0; margin-bottom: 0">Please DO NOT turn off the Wireless Router while you are updating the EEPROM value.</p>     
    <p align="center" style="margin-bottom: 16"> <input class=inputSubmit onMouseOut=buttonOut(this) onMouseOver="buttonOver(this)" onClick="onSubmitCtrl(this, 'WlanUpdate ')" type="submit" value="Update" name="action"></p>
  </td>
</tr>
</table>
</td>
</tr>

<tr>
<td height="29" width="618">
</td>
</tr>

</table>
</form>
</body>
