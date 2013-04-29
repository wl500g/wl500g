<html>
<head>
	<title><% model(2); %></title>
	<meta http-equiv="Content-Type" content="text/html; charset=UTF-8">
	<link rel="stylesheet" type="text/css" href="style.css" media="screen">

	<script type="text/javascript" src="overlib.js"></script>

	<script type="text/javascript" src="general.js"></script>

</head>
<body onload="load_body()" onunload="return unload_body();">
  <div id="overDiv" style="position: absolute; visibility: hidden; z-index: 1000;"></div>	
	<form method="GET" name="form" action="apply.cgi">
	<input type="hidden" name="current_page" value="Main_SysInfo_Content.asp">
	<input type="hidden" name="next_page" value="Main_ARPTable_Content.asp">
	<input type="hidden" name="next_host" value=""><input type="hidden" name="sid_list"
		value="LANHostConfig;">
	<input type="hidden" name="group_id" value="">
	<input type="hidden" name="modified" value="0">
	<input type="hidden" name="action_mode" value="">
	<input type="hidden" name="first_time" value="">
	<input type="hidden" name="action_script" value="">
	<!-- Table for the conntent page -->
	<table class="content_table">
		<tr class="content_header_tr">
		    <td class="content_header_td_title" colspan="2">Status &amp; Log - Diagnostic Information</td>
		</tr>

		<tr>
			<td>
				<% nvram_dump("sysinfo.htm","../usr/sbin/sysinfo > /tmp/sysinfo.htm"); %>
			</td>
		</tr>
	</table>
				<table width="666" border="1" cellpadding="0" cellspacing="0" bordercolor="#B0B0B0">
					<tr bgcolor="#CCCCCC">
						<td colspan="3" style="height:15px;">
						</td>
					</tr>
					<tr bgcolor="#FFFFFF">
						<td height="25" width="67%">
						</td>
						<td height="25" style="text-align: center; font-family: 'Arial';">
							<input class="inputSubmit" onmouseout="buttonOut(this)" onmouseover="buttonOver(this)"
								onclick="onSubmitCtrl(this, ' Refresh ')" type="submit" value="Refresh" name="action">
						</td>
					</tr>
				</table>
	</form>
</body>
</html>