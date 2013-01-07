<html>
<head>
<title>ZVMODELVZ Web Camera Page</title>
<meta http-equiv="Content-Type" content="text/html; charset=UTF-8">
<link rel="stylesheet" type="text/css" href="style.css" media="screen">
<SCRIPT LANGUAGE="JavaScript">

function onClose()
{
   window.close();
}

function loadWidzard()
{
      
}
</script>

</head>
<body onLoad="loadWidzard()" bgcolor="#FFFFFF" text="#000066" link="#3333CC" vlink="#666699" alink="#990099">
<form method="GET" name="form" action="webcam.cgi">  
<input type="hidden" name="ImageSize" value="<% nvram_get("usb_webimage_x"); %>">
<input type="hidden" name="CameraModel" value="<% nvram_get("usb_webdriver_x"); %>">
<!--mstheme-->
<font>
<table align="center" BORDER=1 CELLSPACING=0 CELLPADDING=0 BGCOLOR=#FFFFCC>
<tr>
<td>
<table align="center" width="1" BORDER=0 CELLSPACING=0 CELLPADDING=0 BGCOLOR=#FFFFCC>
<script language="JavaScript">
if ( document.form.ImageSize.value == "8") // 1920 X 1080
{
	width = 1920;
	height = 1080;
} 
else if ( document.form.ImageSize.value == "7") // 1280 X 720
{
	width = 1280;
	height = 720;
}
else if ( document.form.ImageSize.value == "6") // 1600 X 1200
{
	width = 1600;
	height = 1200;
} 
else if ( document.form.ImageSize.value == "5") // 1280 X 1024
{
	width = 1280;
	height = 1024;
}
else if ( document.form.ImageSize.value == "4") // 1024 X 768
{
	width = 1024;
	height = 768;
}
else if ( document.form.ImageSize.value == "3") // 800 X 600
{
	width = 800;
	height = 600;
}
else if ( document.form.ImageSize.value == "2") // 640 X 480
{
	width = 640;
	height = 480;
}
else if ( document.form.ImageSize.value == "1") // 320 X 240
{
	width = 320;
	height = 240;
}
else
{
        width = 160;
	height = 120;
}

var image = 'http://' + '<% nvram_get("lan_ipaddr"); %>' + ':' + '<% nvram_get("usb_webhttpport_x"); %>' + '/?action=stream';

      	document.write('<tr>');
	document.write('<td align="left" height="48" style="padding-top:2;padding-bottom:2;padding-left:2">');
	document.write('<font face="MS Sans Serif" size="2" color="#000000">');
	document.write('<b>ZVMODELVZ Web Camera</b>');
	document.write('</font>');
	document.write('</td>');
	document.write('<td align="right" style="padding-right:2">');
	document.write('</td>     ');           
	document.write('</tr>');
	document.write('<tr>');
	document.write('<td align="center" colspan="3">');
      	document.write('<img name="campicture" border="0" src="' + image + '" alt="Image is not ready" width="' + width + '" height="' + height + '">');
      	document.write('</td>');
	document.write('</tr>');       
</script>       
</table>
</td>
</tr>
</table>
<!--mstheme-->
</font>
</form>
</body>
</html>
