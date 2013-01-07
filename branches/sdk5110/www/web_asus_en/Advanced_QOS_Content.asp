<html>
<head>
<title>ZVMODELVZ Web Manager</title>
<meta http-equiv="Content-Type" content="text/html; charset=UTF-8">
<link rel="stylesheet" type="text/css" href="style.css" media="screen">
<script type="text/javascript" src="overlib.js"></script>
<script type="text/javascript" src="general.js"></script>
</head>  
<body onLoad="load_body()" onunLoad="return unload_body();">
<div id="overDiv" style="position:absolute; visibility:hidden; z-index:1000;"></div>    
<form method="GET" name="form" action="apply.cgi">
<input type="hidden" name="current_page" value="Advanced_QOS_Content.asp">
<input type="hidden" name="next_page" value="SaveRestart.asp">
<input type="hidden" name="next_host" value="">
<input type="hidden" name="sid_list" value="PrinterStatus;">
<input type="hidden" name="group_id" value="">
<input type="hidden" name="modified" value="0">
<input type="hidden" name="action_mode" value="">
<input type="hidden" name="first_time" value="">
<input type="hidden" name="action_script" value="">
<!-- Table for the conntent page -->	    
<table width="666" border="1" cellpadding="0" cellspacing="0" bordercolor="#E0E0E0">
<tr class="content_header_tr">
<td class="content_header_td_title" colspan="2">Bandwidth Management - Basic Config</td>
</tr>
<tr>
<td class="content_desc_td" colspan="2">Bandwidth Management allows you to control the bandwidth for different applications.
         </td>
</tr>
<tr>
<td class="content_header_td">Enable Bandwidth Management?
           </td><td class="content_input_td"><input type="radio" value="1" name="qos_enable_x" class="content_input_fd" onClick="return change_common_radio(this, 'PrinterStatus', 'qos_enable_x', '1')" <% nvram_match("qos_enable_x", "1", "checked"); %>>Yes</input><input type="radio" value="0" name="qos_enable_x" class="content_input_fd" onClick="return change_common_radio(this, 'PrinterStatus', 'qos_enable_x', '0')" <% nvram_match("qos_enable_x", "0", "checked"); %>>No</input></td>
</tr>
</table>

<table width="666" border="2" cellpadding="0" cellspacing="0" bordercolor="#E0E0E0">
<tr class="content_list_header_tr">
<td class="content_list_header_td" width="60%" id="x_QRuleList">Download Policy List
         <input type="hidden" name="qos_rulenum_x_0" value="<% nvram_get("qos_rulenum_x"); %>" readonly></td><td width="10%">
<div align="center">
<input class="inputSubmit" type="submit" onMouseOut="buttonOut(this)" onMouseOver="buttonOver(this)" onClick="return markGroup(this, 'x_QRuleList', 32, ' Add ');" name="x_QRuleList" value="Add" size="12">
</div>
</td><td width="10%">
<div align="center">
<input class="inputSubmit" type="submit" onMouseOut="buttonOut(this)" onMouseOver="buttonOver(this)" onClick="return markGroup(this, 'x_QRuleList', 32, ' Del ');" name="x_QRuleList" value="Del" size="12">
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
<td></td><td class="content_list_field_header_td" colspan="">IP Address	                
           	        </td><td class="content_list_field_header_td" colspan="">Port	                
           	        </td><td class="content_list_field_header_td" colspan="">Max.(kbps)	                
           	        </td><td class="content_list_field_header_td" colspan="">Min.(kbps)	                
           	        </td><td></td>
</tr>
<tr>
<td></td><td class="content_list_input_td" colspan=""><input type="text" maxlength="15" class="content_input_list_fd" size="14" name="qos_ipaddr_x_0" onKeyPress="return is_iprange(event, this)" onKeyUp="change_iprange(this)"></td><td class="content_list_input_td" colspan=""><input type="text" maxlength="10" size="10" class="content_input_list_fd" name="qos_port_x_0" onKeyPress="return is_number(event, this)"></td><td class="content_list_input_td" colspan=""><input type="text" maxlength="10" size="7" class="content_input_list_fd" name="qos_maxbw_x_0" onKeyPress="return is_number(event, this)"></td><td class="content_list_input_td" colspan=""><input type="text" maxlength="10" size="7" class="content_input_list_fd" name="qos_minbw_x_0" onKeyPress="return is_number(event, this)"></td>
</tr>
<tr>
<td></td><td colspan="10"><select size="8" name="x_QRuleList_s" multiple="true" class="content_list_body">
<% nvram_get_table("x_QRuleList"); %>
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
<td class="content_list_header_td" width="60%" id="x_UQRuleList">Upload Policy List
         <input type="hidden" name="qos_urulenum_x_0" value="<% nvram_get("qos_urulenum_x"); %>" readonly></td><td width="10%">
<div align="center">
<input class="inputSubmit" type="submit" onMouseOut="buttonOut(this)" onMouseOver="buttonOver(this)" onClick="return markGroup(this, 'x_UQRuleList', 32, ' Add ');" name="x_UQRuleList" value="Add" size="12">
</div>
</td><td width="10%">
<div align="center">
<input class="inputSubmit" type="submit" onMouseOut="buttonOut(this)" onMouseOver="buttonOver(this)" onClick="return markGroup(this, 'x_UQRuleList', 32, ' Del ');" name="x_UQRuleList" value="Del" size="12">
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
<td></td><td class="content_list_field_header_td" colspan="">Port	                
           	        </td><td class="content_list_field_header_td" colspan="">Max.(kbps)	                
           	        </td><td class="content_list_field_header_td" colspan="">Min.(kbps)	                
           	        </td><td></td>
</tr>
<tr>
<td></td><td class="content_list_input_td" colspan=""><input type="text" maxlength="10" size="10" class="content_input_list_fd" name="qos_uport_x_0" onKeyPress="return is_number(event, this)"></td><td class="content_list_input_td" colspan=""><input type="text" maxlength="10" size="7" class="content_input_list_fd" name="qos_umaxbw_x_0" onKeyPress="return is_number(event, this)"></td><td class="content_list_input_td" colspan=""><input type="text" maxlength="10" size="7" class="content_input_list_fd" name="qos_uminbw_x_0" onKeyPress="return is_number(event, this)"></td>
</tr>
<tr>
<td></td><td colspan="10"><select size="8" name="x_UQRuleList_s" multiple="true" style="font-family: 'monospace'; font-size: '8pt'; width: 100% ">
<% nvram_get_table("x_UQRuleList"); %>
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
