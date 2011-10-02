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
<input type="hidden" name="current_page" value="Advanced_DynamicRoute_Content.asp">
<input type="hidden" name="next_page" value="SaveRestart.asp">
<input type="hidden" name="next_host" value="">
<input type="hidden" name="sid_list" value="RouterConfig;">
<input type="hidden" name="group_id" value="">
<input type="hidden" name="modified" value="0">
<input type="hidden" name="action_mode" value="">
<input type="hidden" name="first_time" value="">
<input type="hidden" name="action_script" value="">
<!-- Table for the conntent page -->	    
<table width="666" border="1" cellpadding="0" cellspacing="0" bordercolor="#E0E0E0">
<tr class="content_header_tr">
<td class="content_header_td_title" colspan="2">Router - Dynamic Route</td>
</tr>
<tr>
<td class="content_header_td" onMouseOver="return overlib('RouterConfig_GWStaticRIP_itemdesc', LEFT);" onMouseOut="return nd();">Enable Dynamic Route(RIP)?
           </td><td class="content_input_td"><select name="sr_rip_x" class="content_input_fd" onChange="return change_common(this, 'RouterConfig', 'sr_rip_x')"><option class="content_input_fd" value="0" <% nvram_match_x("RouterConfig","sr_rip_x", "0","selected"); %>>Disabled</option><option class="content_input_fd" value="1" <% nvram_match_x("RouterConfig","sr_rip_x", "1","selected"); %>>LAN</option><option class="content_input_fd" value="2" <% nvram_match_x("RouterConfig","sr_rip_x", "2","selected"); %>>WAN</option><option class="content_input_fd" value="3" <% nvram_match_x("RouterConfig","sr_rip_x", "3","selected"); %>>BOTH</option></select></td>
</tr>
<tr class="content_section_header_tr">
<td class="content_section_header_td" colspan="2">WAN
            </td>
</tr>
<tr>
<td class="content_header_td" onMouseOver="return overlib('Both the sending and receiving of RIP packets will be enabled or disabled on the WAN port.', LEFT);" onMouseOut="return nd();">Enable RIP on WAN?
           </td><td class="content_input_td"><input type="radio" value="1" name="" class="content_input_fd" onClick="return change_common_radio(this, 'RouterConfig', '', '1')" <% nvram_match_x("RouterConfig","", "1", "checked"); %>>Yes</input><input type="radio" value="0" name="" class="content_input_fd" onClick="return change_common_radio(this, 'RouterConfig', '', '0')" <% nvram_match_x("RouterConfig","", "0", "checked"); %>>No</input></td>
</tr>
<tr>
<td class="content_header_td" onMouseOver="return overlib('This field enables the selected interface to send and receive packets with RIP Version 1, RIP Version 2, or both. In the case of both, packets will be both broadcast and multicast.', LEFT);" onMouseOut="return nd();">RIP Version:
           </td><td class="content_input_td"><select name="" class="content_input_fd" onChange="return change_common(this, 'RouterConfig', '')"><option class="content_input_fd" value="RIP1" <% nvram_match_x("RouterConfig","", "RIP1","selected"); %>>RIP1</option><option class="content_input_fd" value="RIP2" <% nvram_match_x("RouterConfig","", "RIP2","selected"); %>>RIP2</option><option class="content_input_fd" value="Both" <% nvram_match_x("RouterConfig","", "Both","selected"); %>>Both</option></select></td>
</tr>
<tr>
<td class="content_header_td" onMouseOver="return overlib('Control if split-horizon routing mechanism is applied on the port.', LEFT);" onMouseOut="return nd();">Enable Split-horizon?
           </td><td class="content_input_td"><input type="radio" value="1" name="" class="content_input_fd" onClick="return change_common_radio(this, 'RouterConfig', '', '1')" <% nvram_match_x("RouterConfig","", "1", "checked"); %>>Yes</input><input type="radio" value="0" name="" class="content_input_fd" onClick="return change_common_radio(this, 'RouterConfig', '', '0')" <% nvram_match_x("RouterConfig","", "0", "checked"); %>>No</input></td>
</tr>
<tr>
<td class="content_header_td" onMouseOver="return overlib('Select which authentication mechanism and corresponding key are applied on the port.', LEFT);" onMouseOut="return nd();">Authentication Method:
           </td><td class="content_input_td"><select name="" class="content_input_fd" onChange="return change_common(this, 'RouterConfig', '')"><option class="content_input_fd" value="Disable" <% nvram_match_x("RouterConfig","", "Disable","selected"); %>>Disable</option><option class="content_input_fd" value="Text" <% nvram_match_x("RouterConfig","", "Text","selected"); %>>Text</option><option class="content_input_fd" value="MD5" <% nvram_match_x("RouterConfig","", "MD5","selected"); %>>MD5</option></select></td>
</tr>
<tr>
<td class="content_header_td">Authentication Key:
           </td><td class="content_input_td"><input type="text" maxlength="16" class="content_input_fd" size="32" name="" value="<% nvram_get_x("RouterConfig",""); %>" onKeyPress="return is_string(event, this)" onBlur="validate_string(this)"></td>
</tr>
<tr class="content_section_header_tr">
<td class="content_section_header_td" colspan="2">LAN
            </td>
</tr>
<tr>
<td class="content_header_td" onMouseOver="return overlib('Both the sending and receiving of RIP packets will be enabled or disabled on the LAN port.', LEFT);" onMouseOut="return nd();">Enable RIP on LAN?
           </td><td class="content_input_td"><input type="radio" value="1" name="" class="content_input_fd" onClick="return change_common_radio(this, 'RouterConfig', '', '1')" <% nvram_match_x("RouterConfig","", "1", "checked"); %>>Yes</input><input type="radio" value="0" name="" class="content_input_fd" onClick="return change_common_radio(this, 'RouterConfig', '', '0')" <% nvram_match_x("RouterConfig","", "0", "checked"); %>>No</input></td>
</tr>
<tr>
<td class="content_header_td" onMouseOver="return overlib('This field enables the selected interface to send and receive packets with RIP Version 1, RIP Version 2, or both. In the case of both, packets will be both broadcast and multicast.', LEFT);" onMouseOut="return nd();">RIP Version:
           </td><td class="content_input_td"><select name="" class="content_input_fd" onChange="return change_common(this, 'RouterConfig', '')"><option class="content_input_fd" value="RIP1" <% nvram_match_x("RouterConfig","", "RIP1","selected"); %>>RIP1</option><option class="content_input_fd" value="RIP2" <% nvram_match_x("RouterConfig","", "RIP2","selected"); %>>RIP2</option><option class="content_input_fd" value="Both" <% nvram_match_x("RouterConfig","", "Both","selected"); %>>Both</option></select></td>
</tr>
<tr>
<td class="content_header_td" onMouseOver="return overlib('Control if split-horizon routing mechanism is applied on the port.', LEFT);" onMouseOut="return nd();">Enable Split-horizon?
           </td><td class="content_input_td"><input type="radio" value="1" name="" class="content_input_fd" onClick="return change_common_radio(this, 'RouterConfig', '', '1')" <% nvram_match_x("RouterConfig","", "1", "checked"); %>>Yes</input><input type="radio" value="0" name="" class="content_input_fd" onClick="return change_common_radio(this, 'RouterConfig', '', '0')" <% nvram_match_x("RouterConfig","", "0", "checked"); %>>No</input></td>
</tr>
<tr>
<td class="content_header_td" onMouseOver="return overlib('Select which authentication mechanism and corresponding key are applied on the port.', LEFT);" onMouseOut="return nd();">Authentication Method:
           </td><td class="content_input_td"><select name="" class="content_input_fd" onChange="return change_common(this, 'RouterConfig', '')"><option class="content_input_fd" value="Disable" <% nvram_match_x("RouterConfig","", "Disable","selected"); %>>Disable</option><option class="content_input_fd" value="Text" <% nvram_match_x("RouterConfig","", "Text","selected"); %>>Text</option><option class="content_input_fd" value="MD5" <% nvram_match_x("RouterConfig","", "MD5","selected"); %>>MD5</option></select></td>
</tr>
<tr>
<td class="content_header_td">Authentication Key:
           </td><td class="content_input_td"><input type="text" maxlength="16" class="content_input_fd" size="32" name="" value="<% nvram_get_x("RouterConfig",""); %>" onKeyPress="return is_string(event, this)" onBlur="validate_string(this)"></td>
</tr>
<tr class="content_section_header_tr">
<td class="content_section_header_td" colspan="2">Timer
            </td>
</tr>
<tr>
<td class="content_header_td" onMouseOver="return overlib('Every update timer seconds, the RIP process is awakened to send an unsolicited Response message containing the complete routing table to all neighboring RIP routers.', LEFT);" onMouseOut="return nd();">Update time:
           </td><td class="content_input_td"><input type="text" maxlength="5" class="content_input_fd" size="5" name="" value="<% nvram_get_x("RouterConfig",""); %>" onBlur="validate_range(this, 0, 65535)" onKeyPress="return is_number(event, this)"></td>
</tr>
<tr>
<td class="content_header_td" onMouseOver="return overlib('Upon expiration of the timeout, the route is no longer valid; however, it is retained in the routing table for a short time so that neighbors can be notified that the route has been dropped.', LEFT);" onMouseOut="return nd();">Timeout time:
           </td><td class="content_input_td"><input type="text" maxlength="5" class="content_input_fd" size="5" name="" value="<% nvram_get_x("RouterConfig",""); %>" onBlur="validate_range(this, 0, 65535)" onKeyPress="return is_number(event, this)"></td>
</tr>
<tr>
<td class="content_header_td" onMouseOver="return overlib('Upon expiration of the garbage-collection timer, the route is finally removed from the routing table.', LEFT);" onMouseOut="return nd();">Garbage-collection time:
           </td><td class="content_input_td"><input type="text" maxlength="5" class="content_input_fd" size="5" name="" value="<% nvram_get_x("RouterConfig",""); %>" onBlur="validate_range(this, 0, 65535)" onKeyPress="return is_number(event, this)"></td>
</tr>
<tr class="content_section_header_tr">
<td class="content_section_header_td" colspan="2">Route Distribution Rules
            </td>
</tr>
<tr>
<td class="content_desc_td" colspan="2">These rules can be used to filter the RIP path. We define four basic route filter types to stand for the incoming or outgoing data of WAN and LAN port. For each type, we can set only those matched networks specified in the Route Distribution Filter table are processed or dropped.
         </td>
</tr>
<tr>
<td class="content_header_td">For type 0 routes, which are received from WAN, only those specified below will be:
           </td><td class="content_input_td"><select name="" class="content_input_fd" onChange="return change_common(this, 'RouterConfig', '')"><option class="content_input_fd" value="1" <% nvram_match_x("RouterConfig","", "1","selected"); %>>processed</option><option class="content_input_fd" value="0" <% nvram_match_x("RouterConfig","", "0","selected"); %>>dropped</option></select></td>
</tr>
<tr>
<td class="content_header_td">For type 1 routes, which are sent to WAN, only those specified below will be:
           </td><td class="content_input_td"><select name="" class="content_input_fd" onChange="return change_common(this, 'RouterConfig', '')"><option class="content_input_fd" value="1" <% nvram_match_x("RouterConfig","", "1","selected"); %>>processed</option><option class="content_input_fd" value="0" <% nvram_match_x("RouterConfig","", "0","selected"); %>>dropped</option></select></td>
</tr>
<tr>
<td class="content_header_td">For type 2 routes, which are received from LAN, only those specified below will be:
           </td><td class="content_input_td"><select name="" class="content_input_fd" onChange="return change_common(this, 'RouterConfig', '')"><option class="content_input_fd" value="1" <% nvram_match_x("RouterConfig","", "1","selected"); %>>processed</option><option class="content_input_fd" value="0" <% nvram_match_x("RouterConfig","", "0","selected"); %>>dropped</option></select></td>
</tr>
<tr>
<td class="content_header_td">For type 3 routes, which are sent to LAN, only those specified below will be:
           </td><td class="content_input_td"><select name="" class="content_input_fd" onChange="return change_common(this, 'RouterConfig', '')"><option class="content_input_fd" value="1" <% nvram_match_x("RouterConfig","", "1","selected"); %>>processed</option><option class="content_input_fd" value="0" <% nvram_match_x("RouterConfig","", "0","selected"); %>>dropped</option></select></td>
</tr>
</table>

<table width="666" border="2" cellpadding="0" cellspacing="0" bordercolor="#E0E0E0">
<tr class="content_list_header_tr">
<td class="content_list_header_td" width="60%" id="RouteFilter">Route Distribution Filter
         <input type="hidden" name="RipDRouteCount_0" value="<% nvram_get_x("RouterConfig", "RipDRouteCount"); %>" readonly></td><td width="10%">
<div align="center">
<input class="inputSubmit" type="submit" onMouseOut="buttonOut(this)" onMouseOver="buttonOver(this)" onClick="return markGroup(this, 'RouteFilter', 16, ' Add ');" name="RouteFilter" value="Add" size="12">
</div>
</td><td width="10%">
<div align="center">
<input class="inputSubmit" type="submit" onMouseOut="buttonOut(this)" onMouseOver="buttonOver(this)" onClick="return markGroup(this, 'RouteFilter', 16, ' Del ');" name="RouteFilter" value="Del" size="12">
</div>
</td><td width="10%">
<div align="center">
<input class="inputSubmit" type="button" onMouseOut="buttonOut(this)" onMouseOver="buttonOver(this)" onClick="return openHelp(this, 'RouterHelp');" name="RouteFilter" value="Help" size="12">
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
<td></td><td class="content_list_field_header_td" colspan="">Route Type	                
           	        </td><td class="content_list_field_header_td" colspan="">Network/Host IP	                
           	        </td><td class="content_list_field_header_td" colspan="">Netmask Bits	                
           	        </td><td></td>
</tr>
<tr>
<td></td><td class="content_list_input_td" colspan=""><select name="_0" class="content_input_list_fd"><option value="0" <% nvram_match_list_x("RouterConfig","", "0","selected", 0); %>>0</option><option value="1" <% nvram_match_list_x("RouterConfig","", "1","selected", 0); %>>1</option><option value="2" <% nvram_match_list_x("RouterConfig","", "2","selected", 0); %>>2</option><option value="3" <% nvram_match_list_x("RouterConfig","", "3","selected", 0); %>>3</option></select></td><td class="content_list_input_td" colspan=""><input type="text" maxlength="15" class="content_input_list_fd" size="15" name="_0" onKeyPress="return is_ipaddr(event, this)" onKeyUp="change_ipaddr(this)"></td><td class="content_list_input_td" colspan=""><input type="text" maxlength="5" style="font-family: monospace; font-size: 10pt; width: 100%" size="8" name="_0" onKeyPress="return is_number(event, this)"></td>
</tr>
<tr>
<td></td><td colspan="10"><select size="8" name="RouteFilter_s" multiple="true" style="font-family: 'monospace'; font-size: '8pt'; width: 100%">
<% nvram_get_table_x("RouterConfig","RouteFilter"); %>
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

<table width="666" border="1" cellpadding="0" cellspacing="0" bordercolor="#E0E0E0">
<tr class="content_section_header_tr">
<td class="content_section_header_td" colspan="2">Route Metric Rules
            </td>
</tr>
<tr>
<td class="content_desc_td" colspan="2">RIP metric is a value of distance for the network. Usually RIP increments the metric when the network information is received. Redistributed routes' default metric offset is set to 1. These rules can be used to change the metric offset only for the matched networks specified or excluded in the Route Metric Offset table. But the metric offset of other networks is still set to 1.
         </td>
</tr>
<tr>
<td class="content_header_td">Route metric offset:
           </td><td class="content_input_td"><input type="text" maxlength="5" size="5" name="" class="content_input_fd" value="<% nvram_get_x("RouterConfig", ""); %>" onBlur="validate_range(this, 1, 16)" onKeyPress="return is_number(event, this)"></td>
</tr>
<tr>
<td class="content_header_td">For incoming routes, add metric offset to
           </td><td class="content_input_td"><select name="" class="content_input_fd" onChange="return change_common(this, 'RouterConfig', '')"><option class="content_input_fd" value="0" <% nvram_match_x("RouterConfig","", "0","selected"); %>>none</option><option class="content_input_fd" value="1" <% nvram_match_x("RouterConfig","", "1","selected"); %>>routes specified below</option><option class="content_input_fd" value="2" <% nvram_match_x("RouterConfig","", "2","selected"); %>>routes not specified below</option></select></td>
</tr>
<tr>
<td class="content_header_td">For outgoing routes, set metric offset to
           </td><td class="content_input_td"><select name="" class="content_input_fd" onChange="return change_common(this, 'RouterConfig', '')"><option class="content_input_fd" value="0" <% nvram_match_x("RouterConfig","", "0","selected"); %>>none</option><option class="content_input_fd" value="1" <% nvram_match_x("RouterConfig","", "1","selected"); %>>routes specified below</option><option class="content_input_fd" value="2" <% nvram_match_x("RouterConfig","", "2","selected"); %>>routes not specified below</option></select></td>
</tr>
</table>

<table width="666" border="2" cellpadding="0" cellspacing="0" bordercolor="#E0E0E0">
<tr class="content_list_header_tr">
<td class="content_list_header_td" width="60%" id="RipOffset">Route Metric Offset
         <input type="hidden" name="RipOffsetCount_0" value="<% nvram_get_x("RouterConfig", "RipOffsetCount"); %>" readonly></td><td width="10%">
<div align="center">
<input class="inputSubmit" type="submit" onMouseOut="buttonOut(this)" onMouseOver="buttonOver(this)" onClick="return markGroup(this, 'RipOffset', 16, ' Add ');" name="RipOffset" value="Add" size="12">
</div>
</td><td width="10%">
<div align="center">
<input class="inputSubmit" type="submit" onMouseOut="buttonOut(this)" onMouseOver="buttonOver(this)" onClick="return markGroup(this, 'RipOffset', 16, ' Del ');" name="RipOffset" value="Del" size="12">
</div>
</td><td width="10%">
<div align="center">
<input class="inputSubmit" type="button" onMouseOut="buttonOut(this)" onMouseOver="buttonOver(this)" onClick="return openHelp(this, 'RouterHelp');" name="RipOffset" value="Help" size="12">
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
<td></td><td class="content_list_field_header_td" colspan="">Direction	                
           	        </td><td class="content_list_field_header_td" colspan="">Network/Host IP	                
           	        </td><td class="content_list_field_header_td" colspan="">Netmask Bits	                
           	        </td><td></td>
</tr>
<tr>
<td></td><td class="content_list_input_td" colspan=""><select name="_0" class="content_input_list_fd"><option value="IN" <% nvram_match_list_x("RouterConfig","", "IN","selected", 0); %>>IN</option><option value="OUT" <% nvram_match_list_x("RouterConfig","", "OUT","selected", 0); %>>OUT</option></select></td><td class="content_list_input_td" colspan=""><input type="text" maxlength="15" class="content_input_list_fd" size="14" name="_0" onKeyPress="return is_ipaddr(event, this)" onKeyUp="change_ipaddr(this)"></td><td class="content_list_input_td" colspan=""><input type="text" maxlength="5" style="font-family: monospace; font-size: 10pt; width: 100%" size="8" name="_0" onKeyPress="return is_number(event, this)"></td>
</tr>
<tr>
<td></td><td colspan="10"><select size="4" name="RipOffset_s" multiple="true" class="content_list_body">
<% nvram_get_table_x("RouterConfig","RipOffset"); %>
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

<table width="666" border="1" cellpadding="0" cellspacing="0" bordercolor="#B0B0B0">
<tr bgcolor="#CCCCCC"><td colspan="3"><font face="arial" size="2">&nbsp;</font></td></tr>
<tr bgcolor="#FFFFFF">  
   <td id ="Confirm" height="25" width="34%">  
   <div align="center"><font face="Arial"> <input class=inputSubmit onMouseOut="buttonOut(this)" onMouseOver="buttonOver(this)" onClick="onSubmitCtrl(this, ' Restore ')" type="submit" value=" Restore " name="action"></font></div> 
   </td>  
   <td height="25" width="33%">  
   <div align="center"><font face="Arial"> <input class=inputSubmit onMouseOut="buttonOut(this)" onMouseOver="buttonOver(this)" onClick="onSubmitCtrl(this, ' Finish ')" type="submit" value=" Finish " name="action"></font></div> 
   </td>
   <td height="25" width="33%">  
   <div align="center"><font face="Arial"> <input class=inputSubmit onMouseOut="buttonOut(this)" onMouseOver="buttonOver(this)" onClick="onSubmitCtrl(this, ' Apply ')" type="submit" value=" Apply " name="action"></font></div> 
   </td>    
</tr>
</table>

<table width="666" border="1" cellpadding="0" cellspacing="0" bordercolor="#B0B0B0">
<tr>
    <td colspan="2" width="616" height="25" bgcolor="#FFBB00"></td> 
</tr>                   
<tr bgcolor="#FFFFFF">
    <td class="content_header_td_15" align="left">Restore: </td>
    <td class="content_input_td_padding" align="left">Clear the above settings and restore the settings in effect.</td>
</tr>
<tr bgcolor="#FFFFFF">
    <td class="content_header_td_15" align="left">Finish: </td>
    <td class="content_input_td_padding" align="left">Confirm all settings and restart ZVMODELVZ now.</td>
</tr>
<tr bgcolor="#FFFFFF">
    <td class="content_header_td_15" align="left">Apply: </td>
    <td class="content_input_td_padding" align="left">Confirm above settings and continue.</td>
</tr>
</table>

</form>
</body>
</html>
