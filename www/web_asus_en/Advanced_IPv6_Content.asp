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
<input type="hidden" name="current_page" value="Advanced_IPv6_Content.asp">
<input type="hidden" name="next_page" value="SaveRestart.asp">
<input type="hidden" name="next_host" value="">
<input type="hidden" name="sid_list" value="IPv6Config;">
<input type="hidden" name="group_id" value="">
<input type="hidden" name="modified" value="0">
<input type="hidden" name="action_mode" value="">
<input type="hidden" name="first_time" value="">
<input type="hidden" name="action_script" value="">
<input type="hidden" name="wan_proto" value="<% nvram_get("wan_proto"); %>">
<!-- Table for the conntent page -->
<table width="666" border="1" cellpadding="0" cellspacing="0" bordercolor="#E0E0E0">
<tr class="content_header_tr">
<td class="content_header_td_title" colspan="2">IP Config - IPv6</td>
</tr>
<tr>
<td class="content_desc_td" colspan="2">ZVMODELVZ supports several IPv6 connection types to WAN. These types are selected from the drop-down menu beside WAN Connection Type. The setting fields will differ depending on what kind of connection type you select.
	    </td>
</tr>
<tr>
<td class="content_header_td">IPv6 Connection Type:
	    </td><td class="content_input_td"><select name="ipv6_proto" class="content_input_fd" onChange="return change_common(this, 'IPv6Config', 'ipv6_proto')">
	    <option class="content_input_fd" value="" <% nvram_match_x("IPv6Config","ipv6_proto", "","selected"); %>>Disabled</option>
	    <option class="content_input_fd" value="native" <% nvram_match_x("IPv6Config","ipv6_proto", "native","selected"); %>>Static</option>
	    <option class="content_input_fd" value="slaac" <% nvram_match_x("IPv6Config","ipv6_proto", "slaac","selected"); %>>Stateless</option>
	    <option class="content_input_fd" value="dhcp6" <% nvram_match_x("IPv6Config","ipv6_proto", "dhcp6","selected"); %>>DHCPv6</option>
	    <option class="content_input_fd" value="tun6in4" <% nvram_match_x("IPv6Config","ipv6_proto", "tun6in4","selected"); %>>Tunnel 6in4</option>
	    <option class="content_input_fd" value="tun6to4" <% nvram_match_x("IPv6Config","ipv6_proto", "tun6to4","selected"); %>>Tunnel 6to4</option>
	    <option class="content_input_fd" value="tun6rd" <% nvram_match_x("IPv6Config","ipv6_proto", "tun6rd","selected"); %>>Tunnel 6rd</option>
	    </select></td>
</tr>
<tr>
<td class="content_header_td">IPv6 Interface:
	    </td><td class="content_input_td"><select name="ipv6_if_x" class="content_input_fd" onChange="return change_common(this, 'IPv6Config', 'ipv6_if_x')">
	    <option class="content_input_fd" value="0" <% nvram_match_x("IPv6Config","ipv6_if_x", "0","selected"); %>>WAN</option>
	    <option class="content_input_fd" value="1" <% nvram_match_x("IPv6Config","ipv6_if_x", "1","selected"); %>>MAN</option>
	    </select></td>
</tr>

<tr class="content_section_header_tr">
<td class="content_section_header_td" colspan="2">LAN IPv6 Setting
            </td>
</tr>
<tr>
<td class="content_header_td_less">Get IPv6 automatically?
           </td><td class="content_input_td">
		<input type="radio" value="1" name="ipv6_lanauto_x" class="content_input_fd" onclick="change_ipv6_lanauto_x(1);"
			<% nvram_match_x("IPv6Config","ipv6_lanauto_x", "1", "checked"); %>>Yes
		<input type="radio" value="0" name="ipv6_lanauto_x" class="content_input_fd" onclick="change_ipv6_lanauto_x(1);"
			<% nvram_match_x("IPv6Config","ipv6_lanauto_x", "0", "checked"); %>>No</td>
</tr>
<tr>
<td class="content_header_td" onMouseOver="return overlib('This is IPv6 Address of ZVMODELVZ as seen in your local network. If not specified, only auto-configured addresses will be seen.', LEFT);" onMouseOut="return nd();">IPv6 Address:
           </td><td class="content_input_td"><input type="text" maxlength="40" class="content_input_fd" size="40" name="ipv6_lan_addr" value="<% nvram_get("ipv6_lan_addr"); %>" onKeyPress="return is_string(event, this)" onBlur="validate_ip6addr(this, 'ipv6_lan_addr')"></td>
</tr>
<tr>
<td class="content_header_td" onMouseOver="return overlib('This is the number of bits that make up the host part. Most common value is 64', LEFT);" onMouseOut="return nd();">IPv6 Netsize:
           </td><td class="content_input_td"><input type="text" maxlength="3" class="content_input_fd" size="5" name="ipv6_lan_netsize" value="<% nvram_get("ipv6_lan_netsize"); %>" onBlur="return validate_range(this, 1, 128)" onKeyPress="return is_number(event, this)"></td>
</tr>
<tr>
<td class="content_header_td" onMouseOver="return overlib('Router advertisements allow systems in your LAN to auto-configure them for IPv6-usage', LEFT);" onMouseOut="return nd();">Advertise LAN Prefix?
           </td><td class="content_input_td">
		<input type="radio" value="1" name="ipv6_radvd_enable" class="content_input_fd" onClick="return change_common_radio(this, 'IPv6Config', 'ipv6_radvd_enable', '1')"
			<% nvram_match_x("IPv6Config","ipv6_radvd_enable","1", "checked"); %>>Yes
		<input type="radio" value="0" name="ipv6_radvd_enable" class="content_input_fd" onClick="return change_common_radio(this, 'IPv6Config', 'ipv6_radvd_enable', '0')"
			<% nvram_match_x("IPv6Config","ipv6_radvd_enable", "0", "checked"); %>>No</td>
</tr>
<!-- TODO: additinal RDNSS servers
<tr>
<td class="content_header_td" onMouseOver="return overlib('This field indicates the IPv6 address of DNS that ZVMODELVZ contact to. If not specified, only IPv4 servers will be used.', LEFT);" onMouseOut="return nd();">Advertise DNSv6 Server:
          </td><td class="content_input_td"><input type="text" maxlength="40" class="content_input_fd" size="40" name="ipv6_radvd_dns1_x" value="<% nvram_get("ipv6_radvd_dns1_x"); %>" onKeyPress="return is_string(event, this)" onBlur="validate_ip6addr(this, 'ipv6_radvd_dns1_x')">
	  </td>
</tr>-->

<tr class="content_section_header_tr">
<td class="content_section_header_td" colspan="2">WAN IPv6 Setting
           </td>
</tr>
<tr>
<td class="content_header_td_less">Get IPv6 automatically?
           </td><td class="content_input_td">
		<input type="radio" value="1" name="ipv6_wanauto_x" class="content_input_fd" onclick="change_ipv6_wanauto_x(1);"
			<% nvram_match_x("IPv6Config","ipv6_lanauto_x", "1", "checked"); %>>Yes
		<input type="radio" value="0" name="ipv6_wanauto_x" class="content_input_fd" onclick="change_ipv6_wanauto_x(1);"
			<% nvram_match_x("IPv6Config","ipv6_lanauto_x", "0", "checked"); %>>No</td>
</tr>
<tr>
<td class="content_header_td" onMouseOver="return overlib('This is IPv6 Address of ZVMODELVZ as seen on the WAN-port. If not specified, only auto-configured addresses will be seen.', LEFT);" onMouseOut="return nd();">IPv6 Address:
           </td><td class="content_input_td"><input type="text" maxlength="40" class="content_input_fd" size="40" name="ipv6_wan_addr" value="<% nvram_get("ipv6_wan_addr"); %>" onKeyPress="return is_string(event, this)" onBlur="validate_ip6addr(this, 'ipv6_wan_addr')"></td>
</tr>
<tr>
<td class="content_header_td" onMouseOver="return overlib('This is the number of bits that make up the host part.', LEFT);" onMouseOut="return nd();">IPv6 Netsize:
           </td><td class="content_input_td"><input type="text" maxlength="3" class="content_input_fd" size="5" name="ipv6_wan_netsize" value="<% nvram_get("ipv6_wan_netsize"); %>" onBlur="return validate_range(this, 1, 128)" onKeyPress="return is_number(event, this)"></td>
</tr>
<tr>
<td class="content_header_td" onMouseOver="return overlib('The remote IPv6 gateway. All IPv6 traffic flowing out of the network will be directed to this host', LEFT);" onMouseOut="return nd();">IPv6 Default Gateway:
          </td><td class="content_input_td"><input type="text" maxlength="40" class="content_input_fd" size="40" name="ipv6_wan_router" value="<% nvram_get("ipv6_wan_router"); %>" onKeyPress="return is_string(event, this)" onBlur="validate_ip6addr(this, 'ipv6_wan_router')">
	  </td>
</tr>
<tr class="content_section_header_tr">
<td class="content_section_header_td" colspan="2">WAN DNSv6 Setting
           </td>
</tr>
<tr>
<td class="content_header_td_less">Get DNS Server automatically?
           </td><td class="content_input_td">
		<input type="radio" value="1" name="ipv6_dnsenable_x" class="content_input_fd" onclick="return change_common_radio(this, 'IPv6Config', 'ipv6_dnsenable_x', '1')"
			<% nvram_match_x("IPv6Config","ipv6_dnsenable_x", "1", "checked"); %>>Yes
		<input type="radio" value="0" name="ipv6_dnsenable_x" class="content_input_fd" onclick="return change_common_radio(this, 'IPv6Config', 'ipv6_dnsenable_x', '0')"
			<% nvram_match_x("IPv6Config","ipv6_dnsenable_x", "0", "checked"); %>>No</td>
</tr>
<tr>
<td class="content_header_td" onMouseOver="return overlib('This field indicates the IPv6 address of DNS that ZVMODELVZ contact to. If not specified, only IPv4 servers will be used.', LEFT);" onMouseOut="return nd();">DNSv6 Server1:
          </td><td class="content_input_td"><input type="text" maxlength="40" class="content_input_fd" size="40" name="ipv6_dns1_x" value="<% nvram_get("ipv6_dns1_x"); %>" onKeyPress="return is_string(event, this)" onBlur="validate_ip6addr(this, 'ipv6_dns1_x')">
	  </td>
</tr>
<!-- TODO: additinal DNSv6 servers
<td class="content_header_td" onMouseOver="return overlib('This field indicates the IPv6 address of DNS that ZVMODELVZ contact to. If not specified, only IPv4 servers will be used.', LEFT);" onMouseOut="return nd();">DNSv6 Server2:
          </td><td class="content_input_td"><input type="text" maxlength="40" class="content_input_fd" size="40" name="ipv6_dns2_x" value="<% nvram_get("ipv6_dns2_x"); %>" onKeyPress="return is_string(event, this)" onBlur="validate_ip6addr(this, 'ipv6_dns2_x')">
	  </td>
</tr>
<td class="content_header_td" onMouseOver="return overlib('This field indicates the IPv6 address of DNS that ZVMODELVZ contact to. If not specified, only IPv4 servers will be used.', LEFT);" onMouseOut="return nd();">DNSv6 Server3:
          </td><td class="content_input_td"><input type="text" maxlength="40" class="content_input_fd" size="40" name="ipv6_dns3_x" value="<% nvram_get("ipv6_dns3_x"); %>" onKeyPress="return is_string(event, this)" onBlur="validate_ip6addr(this, 'ipv6_dns3_x')">
	  </td>
</tr>-->

<tr class="content_section_header_tr">
<td class="content_section_header_td" colspan="2">Tunnel IPv6 Setting
           </td>
</tr>
<tr>
<td class="content_header_td" onMouseOver="return overlib('The remote IPv4 endpoint for the 6in4 tunnel.', LEFT);" onMouseOut="return nd();">6in4 Remote Endpoint:
          </td><td class="content_input_td"><input type="text" maxlength="15" class="content_input_fd" size="15" name="ipv6_sit_remote" value="<% nvram_get("ipv6_sit_remote"); %>" onBlur="return validate_ipaddr(this, 'ipv6_sit_remote')" onKeyPress="return is_ipaddr(event, this)" onKeyUp="change_ipaddr(this)">
	  </td>
</tr>
<tr>
<td class="content_header_td" onMouseOver="return overlib('The IPv4 anycast relay address for the 6to4 tunnel. Common value is 192.88.99.1', LEFT);" onMouseOut="return nd();">6to4 Anycast Relay:
          </td><td class="content_input_td"><input type="text" maxlength="15" class="content_input_fd" size="15" name="ipv6_sit_relay" value="<% nvram_get("ipv6_sit_relay"); %>" onBlur="return validate_ipaddr(this, 'ipv6_sit_relay')" onKeyPress="return is_ipaddr(event, this)" onKeyUp="change_ipaddr(this)">
	  </td>
</tr>
<tr>
<tr>
<td class="content_header_td" onMouseOver="return overlib('The IPv4 border router address for the 6RD tunnel.', LEFT);" onMouseOut="return nd();">6rd Border Router:
          </td><td class="content_input_td"><input type="text" maxlength="15" class="content_input_fd" size="15" name="ipv6_6rd_router" value="<% nvram_get("ipv6_6rd_router"); %>" onBlur="return validate_ipaddr(this, 'ipv6_6rd_router')" onKeyPress="return is_ipaddr(event, this)" onKeyUp="change_ipaddr(this)">
	  </td>
</tr>
<tr>
<tr>
<td class="content_header_td" onMouseOver="return overlib('The number of high-order bits of IPv4 WAN addresses within a given 6RD domain', LEFT);" onMouseOut="return nd();">6rd IPv4 Mask Length:
          </td><td class="content_input_td"><input type="text" maxlength="3" class="content_input_fd" size="5" name="ipv6_6rd_ip4size" value="<% nvram_get("ipv6_6rd_ip4size"); %>" onKeyPress="return is_number(event, this)" onBlur="validate_range(this, 0, 32)">
	  </td>
</tr>
<tr>
<td class="content_header_td" onMouseOver="return overlib('The Maximum Transfer Unit for the tunnel. Common value is 1280.', LEFT);" onMouseOut="return nd();">Tunnel MTU:
          </td><td class="content_input_td"><input type="text" maxlength="4" class="content_input_fd" size="5" name="ipv6_sit_mtu" value="<% nvram_get("ipv6_sit_mtu"); %>" onKeyPress="return is_number(event, this)" onBlur="validate_range(this, 1280, 1492)">
	  </td>
</tr>
<tr>
<td class="content_header_td" onMouseOver="return overlib('The Time To Life for the tunnel. Common value is 64.', LEFT);" onMouseOut="return nd();">Tunnel TTL:
          </td><td class="content_input_td"><input type="text" maxlength="3" class="content_input_fd" size="5" name="ipv6_sit_ttl" value="<% nvram_get("ipv6_sit_ttl"); %>" onKeyPress="return is_number(event, this)" onBlur="validate_range(this, 1, 128)">
          </td>
</tr>
</table>

<% include("footer_buttons.inc"); %>

</form>
</body>
</html>
