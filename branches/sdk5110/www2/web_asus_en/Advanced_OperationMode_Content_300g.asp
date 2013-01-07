<html>
<head>
<title>ZVMODELVZ Web Manager</title>
<meta http-equiv="Content-Type" content="text/html; charset=UTF-8">
<link rel="stylesheet" type="text/css" href="style.css" media="screen">
<script type="text/javascript" src="overlib.js"></script>
<script type="text/javascript" src="general.js"></script>
<script type="text/javascript" src="quick.js"></script>
</head>
<body bgcolor="#FFFFFF" onLoad="loadMode()">
<div id="overDiv" style="position:absolute; visibility:hidden; z-index:1000;"></div>
<form method="GET" name="form" action="apply.cgi">
<input type="hidden" name="current_page" value="Advanced_OperationMode_Content.asp">
<input type="hidden" name="next_page" value="SaveRestart.asp">
<input type="hidden" name="next_host" value="">
<input type="hidden" name="action_mode" value="">
<input type="hidden" name="sid_list" value="Layer3Forwarding;IPConnection;">
<input type="hidden" name="wan_proto" value="<% nvram_get_x("Layer3Forwarding","wan_proto"); %>">
<input type="hidden" name="wan_route_x" value="<% nvram_get_x("IPConnection","wan_route_x"); %>">
<input type="hidden" name="wan_nat_x" value="<% nvram_get_x("IPConnection","wan_nat_x"); %>">
<!-- Table for the conntent page -->
<table width="666" border="2" cellpadding="0" cellspacing="0" bordercolor="#E0E0E0">
<tr class="content_header_tr">
<td class="content_header_td_title" colspan="2">System Setup - Operation Mode</td>
</tr>
<tr>
<td class="content_desc_td" colspan="2">
ZVMODELVZ supports two operation modes to meet different requirements from different group of people. Please select the mode that match your situation.
</td>
</tr>

<tr>
<td class="content_header_td_30"><input type="radio" name="x_Mode" value="V3"> Access Point</td>
<td class="content_desc_td">
In Access Point mode, ethernet port and wireless devices are set to locate in the same local area network. Those WAN related functions are not supported here.
<p></p>
Explaining with technical terms, access point mode is, NAT is disabled, LAN port and wireless port of ZVMODELVZ are bridged together.
</td>
</tr>
<tr>
<td class="content_header_td_30"><input type="radio" name="x_Mode"> Home Gateway</td>
<td class="content_desc_td">
In this mode, we suppose you use the only ethernet port of ZVMODELVZ to connect to Internet through ADSL or Cable Modem. And, there are many people in your environment share the same IP to ISP.
<p></p>
Explaining with technical terms, gateway mode is , NAT is enabled, WAN connection is allowed by using PPPoE, or DHCP client, or static IP. In addition, some features which are useful for home user, such as UPnP and DDNS, are supported.
</td>
</tr>
<tr>
<td class="content_input_td_less" colspan="2" height="60">
  <p align="right">
  <input class="inputSubmit" onMouseOut="buttonOut(this)" onMouseOver="buttonOver(this)" type="submit" value="Apply" name="action" onClick="saveMode(this)">&nbsp;
  </p>
</td>
</tr></table>

</form>
</body>
</html>