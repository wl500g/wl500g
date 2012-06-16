<html>
<head>
<title>ZVMODELVZ Web Manager</title>
<meta http-equiv="Content-Type" content="text/html; charset=UTF-8">
<link rel="stylesheet" type="text/css" href="style.css" media="screen">
<script type="text/javascript" src="overlib.js"></script>
<script type="text/javascript" src="general.js"></script>
<script type="text/javascript" src="quick.js"></script>
</head>  
<body bgcolor="#FFFFFF">
<div id="overDiv" style="position:absolute; visibility:hidden; z-index:1000;"></div>    
<form method="post" action="upgrade.cgi" enctype="multipart/form-data">
<input type="hidden" name="current_page" value="Advanced_FirmwareUpgrade_Content.asp">
<input type="hidden" name="next_page" value="Updating.asp">
<input type="hidden" name="action_mode" value="">
<!-- Table for the conntent page -->
<table width="666" border="2" cellpadding="0" cellspacing="0" bordercolor="#E0E0E0">
<tr class="content_header_tr">
<td class="content_header_td_title" colspan="2">System Setup - Firmware Upgrade</td>
</tr>
<tr class="content_section_header_tr">
<td id="Mode" class="content_section_header_td" colspan="2" width="614"> Follow instructions listed below:</td>
</tr>

<tr>
<td class="content_desc_td" colspan="2" width="614">
              	<ol>
              		<li>
              <p style="margin-top: 7">
                    Check if any new version of firmware is available on <a href="http://wl500g.googlecode.com" target="_parent">WL500g project</a> website.
              </li>
              		<li>
              <p style="margin-top: 7">
                    Download a proper version to your local machine.
              </li>
              		<li>
              <p style="margin-top: 7">
                    Specify the path of and name of the downloaded file in the "New Firmware File".
              </li>
              		<li>
              <p style="margin-top: 7">
                    Click "Upload" to upload the file to ZVMODELVZ. It spends about 10 seconds.
              </li>
              <li>
              <p style="margin-top: 7">
                    After receiving a correct firmware file, ZVMODELVZ will automatically start the upgrade process. It takes a few time to finish the process and then the system will reboot.
 	      </li>
              </ol>
</td>
</tr>
<tr>
		<td class="content_header_td" width="30">Product ID:</td><td class="content_input_td" width="595"><input type="text" class="content_input_fd_ro" value="<% nvram_get_x("general.log","productid"); %>" readonly></td>
</tr>
<tr>
		<td class="content_header_td" width="30">Firmware Version:</td><td class="content_input_td" width="595"><input type="text" class="content_input_fd_ro" value="<% nvram_get_x("general.log","firmver"); %>" readonly></td>
</tr>
<tr>
		<td class="content_header_td" width="30">Firmware Release:</td><td class="content_input_td" width="595"><input type="text" class="content_input_fd_ro" value="<% nvram_dump("../.version",""); %>" readonly></td>
</tr>
<tr>
<td class="content_header_td" width="30">New Firmware File:</td><td class="content_input_td">  
                  <input class="inputSubmit" name="file" size="20" type="file" ></td>
</tr>
<tr>
<td class="content_header_td"></td><td class="content_input_td" width="595">
  <p align="left"><input class="inputSubmit" onClick="onSubmitCtrlOnly(this, 'Upload')" type="submit" value="Upload"> 
  </p>
</td>
</tr>
<tr>
<td class="content_desc_td" colspan="2" width="614">
	             <b>Note:</b>
                     <ol>
                     	<li>For a configuration parameter existing both in the old and new firmware, its setting will be kept during the upgrade process.</li>
                     	<li>In case the upgrade process fails, ZVMODELVZ will enter an emergent mode automatically. The LED signals at the front of ZVMODELVZ will indicate such situation. Use the Firmware Restoration utility on the CD to do system recovery.</li>
                     </ol>
</td>
</tr>
</table>
</form>
</body>
</html>