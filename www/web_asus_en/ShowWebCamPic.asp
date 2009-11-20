<html>
<head>
<meta http-equiv="refresh" content="<% nvram_get_x("PrinterStatus","usb_webfresh_x"); %>">
<title>ZVMODELVZ Web Camera Page</title>
<meta http-equiv="Content-Type" content="text/html; charset=ISO-8859-1">
<meta http-equiv="Content-Type" content="text/html; charset=UTF-8">
<meta http-equiv="Content-Type" content="text/html; charset=gb2312">
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
<input type="hidden" name="RefreshTime" value="<% nvram_get_x("PrinterStatus","usb_webfresh_x"); %>">
<input type="hidden" name="ImageSize" value="<% nvram_get_x("PrinterStatus","usb_webimage_x"); %>">
<input type="hidden" name="CaptionString" value="<% nvram_get_x("PrinterStatus","usb_webcaption_x"); %>">
<input type="hidden" name="CameraMode" value="<% nvram_get_x("PrinterStatus","usb_webmode_x"); %>">
<input type="hidden" name="CameraModel" value="<% nvram_get_x("PrinterStatus","usb_webdriver_x"); %>">
<input type="hidden" name="CameraPort" value="<% nvram_get_x("PrinterStatus","usb_webport_x"); %>">
<input type="hidden" name="xxx" value="<% nvram_get_x("LANHostConfig","lan_hwaddr"); %>">
<!--mstheme-->
<font>
<table align="center" BORDER=1 CELLSPACING=0 CELLPADDING=0 BGCOLOR=#FFFFCC>
<tr>
<td>
<table align="center" width="1" BORDER=0 CELLSPACING=0 CELLPADDING=0 BGCOLOR=#FFFFCC>
<script language="JavaScript">
if ( document.form.ImageSize.value == "0") // 640 X 480
{
     if (document.form.CameraModel.value == "0" ) //PWC
     {	
     	width = 320;
     	height = 240;
     }
     else
     {
     	width = 640;
     	height = 480;
     }			
}
else if ( document.form.ImageSize.value == "1") // 320 X 240
{    
     width = 320;
     height = 240;	
}
else 
{
     if (document.form.CameraModel.value == "0" ) //PWC
     {	
     	width = 160;
     	height = 120;
     }
     else
     {
     	width = 176;
     	height = 144;
     }		     
}

//Caption = '<% nvram_get_x("PrinterStatus","x_WCaption"); %>';
Caption = document.form.CaptionString.value;

      	document.write('<tr>');
	document.write('<td align="left" height="48" style="padding-top:2;padding-bottom:2;padding-left:2">');
	document.write('<font face="MS Sans Serif" size="2" color="#000000">');
	document.write('<b>');
	document.write(Caption);
	document.write('</b>');
	document.write('</font>');
	document.write('</td>');
	document.write('<td align="right" style="padding-right:2">');
	document.write('</td>     ');           
	document.write('</tr>');
	document.write('<tr>');
	document.write('<td align="center" colspan="3">');
      	document.write('<img name="campicture" border="0" src="display.jpg" alt="Image is not ready" width="' + width + '" height="' + height + '">');
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
