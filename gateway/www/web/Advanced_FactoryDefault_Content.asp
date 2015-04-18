<html>
<head>
<title><% model(2); %></title>

<meta http-equiv="Content-Type" content="text/html; charset=UTF-8">

<link rel="stylesheet" type="text/css" href="style.css" media="screen">
<script type="text/javascript" src="overlib.js"></script>
<script type="text/javascript" src="general.js"></script>
</head>
<body>  
<div id="overDiv" style="position:absolute; visibility:hidden; z-index:1000;"></div>    
<form method="GET" name="form" action="apply.cgi">
<input type="hidden" name="action_mode" value="">
<input type="hidden" name="current_page" value="Advanced_FactoryDefault_Content.asp">
<input type="hidden" name="next_page" value="SaveRestart.asp">
<input type="hidden" name="sid_list" value="General;">
<input type="hidden" name="group_id" value="">
<input type="hidden" name="modified" value="0">
<!-- Table for the conntent page -->	    
<table width="666" border="1" cellpadding="0" cellspacing="0" bordercolor="#B0B0B0">
<tr class="content_header_tr">
<td class="content_header_td_title" colspan="2" height="25" width="666">System Setup - Factory Default</td>
</tr>
<tr bgcolor="#FFFFFF">
    <td class="content_desc_td">
    <p>Click the <b>Restore</b> button to clear all settings and restore the factory defaults. Then, wait for the home gateway to reboot.</p>       
    <p align="center" style="margin-bottom: 16"> <input class=inputSubmit onMouseOut="buttonOut(this)" onMouseOver="buttonOver(this)" onClick="onSubmitCtrlOnly(this, 'Restore')" type="submit" value="Restore" name="action"></p>
  </td></tr>
</table>
</form>
</body>
</html>
