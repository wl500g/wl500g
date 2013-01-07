<html>
<head>
<title>ZVMODELVZ Web Manager</title>
<link rel="stylesheet" type="text/css" href="style.css" media="screen">
<script type="text/javascript" src="overlib.js"></script>
<script type="text/javascript" src="general.js"></script>
<script type="text/javascript" src="quick.js"></script>
</head>  
<body bgcolor="#FFFFFF">
<div id="overDiv" style="position:absolute; visibility:hidden; z-index:1000;"></div>    
<form method="post" action="upgrade.cgi" enctype="multipart/form-data">
<input type="hidden" name="current_page" value="Advanced_PrinterSetup_Content.asp">
<input type="hidden" name="next_page" value="SaveRestart.asp">
<!-- Table for the conntent page -->	    
<table width="666" border="2" cellpadding="0" cellspacing="0" bordercolor="#E0E0E0">
<tr class="content_header_tr">
<td colspan="2"> System Setup - Printer Setup</td>
</tr>
<tr class="content_section_header_tr">
<td id="Mode" colspan="2" width="614"> Follow instructions listed below:</td>
</tr>

<tr>
		<td class="content_header_td" width="30">Printer Model:</td><td class="content_input_td" width="595"><input type="text" value="<% nvram_get("productid"); %>" readonly></td>
</tr>
<tr>
<td class="content_input_td" colspan="2" width="614">
              	<ol>
              		<li>
              <p style="margin-top: 7"><font size="2">Connect your printer to 
              printer port of WL600 and turn on its power.</font>
              </li>
              		<li>
              <p style="margin-top: 7">
              <font size="2">
                    Press  
              	</font>
              <input type="submit" value="Refresh"> <font size="2">to detect 
              printer. 
              	</font>
              </li>
              		<li>
              <p style="margin-top: 7"><font size="2">If you have installed ASUS 
              WLAN Homegateway Utilities, then execute 'Printer Setup Wizard' 
              to complete your printer setting.</font>
              </li>
              	</ol>
</td>
</tr>
</table>
</form>
</body>
</html>