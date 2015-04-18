<!--<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.01//EN" "http://www.w3.org/TR/html4/strict.dtd">-->
<!--- Header include file for dynamic pages by  dlukanidin ver. 1.100 from 30.03.2012 --->
<html>
<head>
	<title>WL500g - Web Interface</title>
	<meta http-equiv="Content-Type" content="text/html; charset=UTF-8">
	<link rel="stylesheet" type="text/css" href="new/ui/ui_<%nvram_get_x("www","www_skin");%>.css" media="screen">
	<script src="new/interface.js" type="text/javascript"></script>
	<script src="new/navi.js" type="text/javascript"></script>
</head>
<body onload="loadBody();">
	<div id="initDiv">
		<p>
		You must enable Java Script in browser options to control ROUTER with Web interface.
		</p>
	</div>
	<script type="text/javascript">
		document.getElementById('initDiv').innerHTML='';
		locale_name = '<%nvram_get_x("www","www_locale");%>';
		skin_name = '<%nvram_get_x("www","www_skin");%>';
		var router_state = {
			<% nvram_get_json("Layer3Forwarding","wan_proto"); %>,
			<% nvram_get_json("IPConnection","wan_route_x","wan_nat_x","wl_ssid"); %>
		};

	</script>
	<script type="text/javascript">
	//<dynamic>
