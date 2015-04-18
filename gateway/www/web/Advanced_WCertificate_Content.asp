<html>
<head>
<title><% model(2); %></title>
<link rel="stylesheet" type="text/css" href="style.css" media="screen">
<script type="text/javascript" src="overlib.js"></script>
<script type="text/javascript" src="general.js"></script>
</head>  
<body onLoad="load_body()" onunLoad="return unload_body();">
<div id="overDiv" style="position:absolute; visibility:hidden; z-index:1000;"></div>    

<form method="post" name="form" action="uploadCA.cgi" enctype="multipart/form-data">
<input type="hidden" name="current_page" value="Advanced_WCertificate_Content.asp">
<input type="hidden" name="next_page" value="SaveRestart.asp">
<input type="hidden" name="next_host" value="">
<input type="hidden" name="sid_list" value="WLANAuthentication11a;WLANConfig11b;">
<input type="hidden" name="group_id" value="">
<input type="hidden" name="modified" value="0">
<input type="hidden" name="action_mode" value="">
<input type="hidden" name="action_script" value="">
<!-- Table for the conntent page -->
<table width="666" border="2" cellpadding="0" cellspacing="0" bordercolor="#E0E0E0">
<tr class="content_list_header_tr">
<td class="content_list_header_td" width="60%" id="LocalCertDB">Certificate Database<input type="hidden" name="WLANAuthentication11a_CertDBNumberOfEntries_0" value="<% nvram_get("CertDBNumberOfEntries"); %>" readonly></td><td width="10%">
<div align="center">
<input class="inputSubmit" type="submit" onMouseOut="buttonOut(this)" onMouseOver="buttonOver(this)" onClick="return markGroup(this, 'WLANAuthentication11a_LocalCertDB', 32);" name="WLANAuthentication11a_LocalCertDB" value=" Add " size="12">
</div>
</td><td width="10%">
<div align="center">
<input class="inputSubmit" type="submit" onMouseOut="buttonOut(this)" onMouseOver="buttonOver(this)" onClick="return markGroup(this, 'WLANAuthentication11a_LocalCertDB', 32);" name="WLANAuthentication11a_LocalCertDB" value=" Del " size="12">
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
<td></td><td class="content_list_field_header_td" colspan="">Type</td><td class="content_list_field_header_td" colspan="">Certificate File</td><td></td>
</tr>
<tr>
<td></td><td class="content_list_input_td" colspan=""><select name="WLANAuthentication11a_CertDBType_0" class="content_input_list_fd"><option value="Server" <% nvram_match_list("CertDBType", "Server","selected", 0); %>>Server</option><option value="Client" <% nvram_match_list("CertDBType", "Client","selected", 0); %>>Client</option></select></td><td class="content_list_input_td" colspan=""><input type="file" maxlength="15" class="content_input_list_fd" size="32" name="WLANAuthentication11a_CertDBFile_0"></td>
</tr>
<tr>
<td></td><td colspan="10"><select size="8" name="WLANAuthentication11a_LocalCertDB_s" multiple="true" class="content_list_body">
<% nvram_get_table("LocalCertDB"); %>
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

<% include("footer_buttons.inc"); %>

</form>
</body>
</html>
