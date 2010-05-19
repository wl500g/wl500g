<head>
	<title>ZVMODELVZ Web Manager</title>
	<meta http-equiv="Content-Type" content="text/html; charset=UTF-8">
	<link rel="stylesheet" type="text/css" href="style.css" media="screen">

	<script language="JavaScript" type="text/javascript" src="overlib.js"></script>

	<script language="JavaScript" type="text/javascript" src="general.js"></script>

</head>
	<div id="overDiv" style="position: absolute; visibility: hidden; z-index: 1000;">
	</div>
<body onload="load_body()" onunload="return unload_body();">
	<!-- Table for the conntent page -->
	<form method="GET" name="form" action="apply.cgi">
	<input type="hidden" name="current_page" value="Main_ChatStatus_Content.asp">
	<input type="hidden" name="next_page" value="Main_ChatStatus_Content.asp">
	<input type="hidden" name="next_host" value=""><input type="hidden" name="sid_list"
		value="FirewallConfig;">
	<input type="hidden" name="group_id" value=""><input type="hidden" name="modified"
		value="0">
	<input type="hidden" name="action_mode" value="">
	<input type="hidden" name="first_time" value=""><input type="hidden" name="action_script"
		value="">
	</form>
	<table width="666" border="0" cellpadding="0" cellspacing="0">
		<tr>
			<td>
				<table width="666" border="1" cellpadding="0" cellspacing="0" bordercolor="#E0E0E0">
					<tr class="content_header_tr">
						<td class="content_header_td_title" colspan="2">
							Status &amp; Log - PPP Chat Log
						</td>
					</tr>
					<tr class="content_header_tr">
						<td colspan="2">
							<textarea class="content_log_td" cols="63" rows="25" wrap="off" readonly>
<% nvram_dump("chat.log",""); %>
            </textarea>
						</td>
					</tr>
				</table>
			</td>
		</tr>
		<tr>
			<td bordercolor="#E0E0E0">
				<br>
			</td>
		</tr>
		<tr>
			<td>
				<table width="666" border="1" cellpadding="0" cellspacing="0" bordercolor="#B0B0B0">
					<tr bgcolor="#CCCCCC">
						<td colspan="3" style="font-family: 'Arial'; font-size: 2pt;">
							<b>&nbsp;</b>
						</td>
					</tr>
					<tr bgcolor="#FFFFFF">
						<td height="25" width="34%">
						</td>
						<td height="25" width="33%">
						</td>
						<td height="25" width="33%" style="text-align: center; font-family: 'Arial';">
							<input class="inputSubmit" onmouseout="buttonOut(this)" onmouseover="buttonOver(this)"
								onclick="onSubmitCtrl(this, ' Refresh ')" type="submit" value="Refresh" name="action">
						</td>
					</tr>
				</table>
			</td>
		</tr>
	</table>
</body>
