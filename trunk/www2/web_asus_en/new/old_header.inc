<!--<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.01//EN" "http://www.w3.org/TR/html4/strict.dtd">-->
<!--- Header include file for static pages by  dlukanidin ver. 1.100 from 30.03.2012 --->
<html>
<head>
	<title>ZVMODELVZ Web Manager</title>
	<meta http-equiv="Content-Type" content="text/html; charset=UTF-8">
	<link rel="stylesheet" type="text/css" href="new/old_style.css" media="screen">
	<link rel="stylesheet" type="text/css" href="new/ui/ui_<%nvram_get_x("www","www_skin");%>.css" media="screen">

	<script type="text/javascript" src="overlib.js"></script>
	<script type="text/javascript" src="general.js"></script>
	<script src="new/interface.js" type="text/javascript"></script>
	<script src="new/navi.js" type="text/javascript"></script>
	<script src="new/ui/ui_wl500g.js" type="text/javascript"></script>
	<script src="new/locales/en/lc_navi.js" type="text/javascript"></script>

</head>
<body onload="load_body()" onunload="return unload_body();">
	<div id="contentDiv"> 
	<script type="text/javascript">
		var router_state = {
			<% nvram_get_json("Layer3Forwarding","wan_proto"); %>,
			<% nvram_get_json("IPConnection","wan_route_x","wan_nat_x","wl_ssid"); %>
		};
//		loadFlags.lc_navi = true; // loaded in head
//		loadFlags.skin = true; // loaded in head
//		initNavi();
		showHeader();
	</script>
