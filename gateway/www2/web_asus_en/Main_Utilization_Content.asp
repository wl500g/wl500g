<html>
<head>
<title>ZVMODELVZ Web Manager</title>
<meta http-equiv="Content-Type" content="text/html; charset=UTF-8">
<link rel="stylesheet" type="text/css" href="style.css" media="screen">
<script type="text/javascript" src="overlib.js"></script>
<script type="text/javascript" src="general.js"></script>
<script type="text/javascript">

    var wan_proto="<% nvram_get_f("wan.log","wan_proto"); %>";
    var wan_ifname="<% nvram_get_x("IPConnection","wan_ifname"); %>";
    var man_ifname="";
    var ipv6_proto="<% nvram_get_x("wan.log","ipv6_proto"); %>";
    var ipv6_ifname="";
    var wlan_ifname="<% nvram_get_x("IPConnection","wl0_ifname"); %>";
    var lan_ifname="<% nvram_get_x("IPConnection","lan_ifname"); %>";

    function interval_change(value)
    {
	document.getElementById("cpu_graph").src="graph_cpu.svg?#"+value;
	if (wan_ifname != "")
	    document.getElementById("wan_graph").src="graph_if.svg?"+wan_ifname+"#"+value;
	if (man_ifname != "")
	    document.getElementById("man_graph").src="graph_if.svg?"+man_ifname+"#"+value;
	if (ipv6_ifname != "")
	    document.getElementById("ipv6_graph").src="graph_if.svg?"+ipv6_ifname+"#"+value;
	if (wlan_ifname != "")
	    document.getElementById("wlan_graph").src="graph_if.svg?"+wlan_ifname+"#"+value;
	if (lan_ifname != "")
	    document.getElementById("lan_graph").src="graph_if.svg?"+lan_ifname+"#"+value;
    }

    function process_element(elementId, value)
    {
	if (value != "") {
	    document.getElementById(elementId+"_ifname").innerHTML=value;
	} else {
	    document.getElementById(elementId+"_data").style.display="none";
	}
    }

    function generate_page()
    {
	load_body();

	if (wan_proto=="l2tp" || wan_proto=="pptp" || wan_proto=="pppoe") {
	    man_ifname=wan_ifname;
	    wan_ifname="<% nvram_get_x("IPConnection","wan0_pppoe_ifname"); %>";
	} else
	if (wan_proto=="wimax") {
	    wan_ifname="<% nvram_get_x("IPConnection","wan0_wimax_ifname"); %>";
	} else
	if (wan_proto=="usbmodem") {
	    wan_ifname="<% nvram_get_x("IPConnection","wan0_pppoe_ifname"); %>";
	} else
	if (wan_proto=="usbnet") {
	    wan_ifname="<% nvram_get_x("IPConnection","wan0_ifname"); %>";
	}

	if (ipv6_proto=="tun6in4" || ipv6_proto=="tun6to4") {
	    ipv6_ifname="six0";
	}

	process_element("wan", wan_ifname);
	process_element("man", man_ifname);
	process_element("ipv6", ipv6_ifname);
	process_element("wlan", wlan_ifname);
	process_element("lan", lan_ifname);

	interval_change(1);
    }
</script>
</head>  
<body onLoad="generate_page();" onunLoad="return unload_body();">
<div id="overDiv" style="position:absolute; visibility:hidden; z-index:1000;"></div>
<form method="GET" name="form" action="apply.cgi">
<input type="hidden" name="current_page" value="Main_Utilization_Content.asp">
<input type="hidden" name="next_page" value="Main_Utilization_Content.asp">
<input type="hidden" name="next_host" value="">
<input type="hidden" name="sid_list" value="Utilization;">
<input type="hidden" name="group_id" value="">
<input type="hidden" name="modified" value="0">
<input type="hidden" name="action_mode" value="">
<input type="hidden" name="first_time" value="">
<input type="hidden" name="action_script" value="">
<!-- Table for the conntent page -->
<table width="666" border="1" cellpadding="0" cellspacing="0" bordercolor="#E0E0E0">
<tr class="content_header_tr">
    <td class="content_header_td_title" colspan="2">Status &amp; Log - System Utilization</td>
</tr>
<tr>
    <td class="content_header_td_less">Refresh Rate:</td>
    <td class="content_input_td">
	<select name="refresh" class="content_input_td" onchange="interval_change(this.value);">
	<option value="1">1 second</option>
	<option value="2">2 seconds</option>
	<option value="5">5 seconds</option>
	<option value="10">10 seconds</option>
	<option value="30">30 seconds</option>
	<option value="60">1 minute</option>
	</select></td>
</tr>
</table>

<table width="666" border="1" cellpadding="0" cellspacing="0" bordercolor="#E0E0E0">
<tr class="content_section_header_tr">
    <td class="content_section_header_td" colspan="2">System Load Monitoring</td>
</tr>
<tr>
    <td class="content_header_td">CPU:</td>
    <td class="content_header_td">
    <table align="right" border="0">
    <tr><td>
	<iframe id="cpu_graph" type="image/svg+xml" width="530" height="250" frameborder="0" src=""></iframe>
    </td></tr></table></td>
</tr>
<tr class="content_section_header_tr">
    <td class="content_section_header_td" colspan="2">Interface Bandwidth Monitoring</td>
</tr>
<tr id="wan_data">
    <td class="content_header_td">WAN (<span id="wan_ifname"></span>):</td>
    <td class="content_header_td">
    <table align="right" border="0">
    <tr><td>
	<iframe id="wan_graph" type="image/svg+xml" width="530" height="250" frameborder="0" src=""></iframe>
    </td></tr></table></td>
</tr>
<tr id="man_data">
    <td class="content_header_td">MAN (<span id="man_ifname"></span>):</td>
    <td class="content_header_td">
    <table align="right" border="0">
    <tr><td>
	<iframe id="man_graph" type="image/svg+xml" width="530" height="250" frameborder="0" src=""></iframe>
    </td></tr></table></td>
</tr>
<tr id="ipv6_data">
    <td class="content_header_td">IPV6 (<span id="ipv6_ifname"></span>):</td>
    <td class="content_header_td">
    <table align="right" border="0">
    <tr><td>
	<iframe id="ipv6_graph" type="image/svg+xml" width="530" height="250" frameborder="0" src=""></iframe>
    </td></tr></table></td>
</tr>
<tr id="wlan_data">
    <td class="content_header_td">Wireless (<span id="wlan_ifname"></span>):</td>
    <td class="content_header_td">
    <table align="right" border="0">
    <tr><td>
	<iframe id="wlan_graph" type="image/svg+xml" width="530" height="250" frameborder="0" src=""></iframe>
    </td></tr></table></td>
</tr>
<tr id="lan_data">
    <td class="content_header_td">LAN (<span id="lan_ifname"></span>):</td>
    <td class="content_header_td">
    <table align="right" border="0">
    <tr><td>
	<iframe id="lan_graph" type="image/svg+xml" width="530" height="250" frameborder="0" src=""></iframe>
    </td></tr></table></td>
</tr>
</table>

<table width="666" border="1" cellpadding="0" cellspacing="0" bordercolor="#B0B0B0">
<tr bgcolor="#CCCCCC"><td colspan="3"><font face="arial" size="2">&nbsp;</font></td></tr>
<tr bgcolor="#FFFFFF">  
    <td height="25" width="71%"></td>
    <td height="25" >
	<input type="hidden" name="action_mode" value=" Refresh ">
	<div align="center"><font face="Arial"> <input class=inputSubmit onMouseOut="buttonOut(this)" onMouseOver="buttonOver(this)" onClick="document.form3.next_host.value = location.host; onSubmitCtrl(this, ' Refresh ')" type="submit" value="Refresh" name="action"></font></div> 
	</td>
</tr>
</table>

</form>
</body>
</html>