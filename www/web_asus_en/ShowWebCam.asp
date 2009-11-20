<html>
<head>
<title>ZVMODELVZ Web Camera Page</title>
<meta http-equiv="Content-Type" content="text/html; charset=ISO-8859-1">
<meta http-equiv="Content-Type" content="text/html; charset=UTF-8">
<meta http-equiv="Content-Type" content="text/html; charset=gb2312">
<link rel="stylesheet" type="text/css" href="style.css" media="screen">
<SCRIPT LANGUAGE="JavaScript">

var cameraMode = '<% nvram_get_x("PrinterStatus","usb_webmode_x"); %>';
var the_timeout;

function browserType()
{
  if (navigator.appName == "Netscape")
  {
  	return ("Netscape");
  }
  else if (navigator.appName == "Microsoft Internet Explorer")
  {
  	if (navigator.userAgent.indexOf("Opera") > -1) return ("Opera");
  	else return("MSIE");
  }
  else return("Unknown");	  
}

function addbookmark()
{  
   bookmarkurl="http://" + document.form.lan_ipaddr.value + "/ShowWebCam.asp";  
     
   if (document.form.wan_proto.value == "static") 
   {
      rst = confirm("You have set Static IP, would you like to add this url to your bookmark, so that you can use it from WAN?");
      if (rst) bookmarkurl="http://" + document.form.IPConnection_ExternalIPAddress.value + ":8080/ShowWebCam.asp";
   }   
   else if (document.form.ddns_server_x.value == "1")
   {
      rst = confirm("You have set an DDNS account, would you like to add this url to your bookmark, so that you can use it from WAN?");
      if (rst) bookmarkurl="http://" + document.form.ddns_hostname_x.value + ":8080/ShowWebCam.asp";
   }   
       
   alert("Add " + bookmarkurl + " to bookmark");
   bookmarktitle=document.form.CaptionString.value;
   if (document.all)
	window.external.AddFavorite(bookmarkurl,bookmarktitle)
}

function change_image()
{
   //document.campicture.src = "webcam.cgi" + "?" + document.form.imageidx.value;   
   //window.status = "Connecting to Server...";
   //document.body.style.cursor = "default";
   
   redirect1();
   //if (document.form.imageidx.value=="0")
   //   setTimeout("redirect1()",document.form.RefreshTime.value)
   //else 
   //   setTimeout("redirect1()", 0);
}

function redirect() 
{
    the_timeout = setTimeout("redirect1()",1000)
}

function redirect1() 
{
   var now = new Date();
   document.campicture.src = "webcam.cgi" + "?" + "0";
   //document.form.imageidx.value;   
   //document.form.CurrentTime.value = now;
   window.status = "Connecting to Server...";
   //document.body.style.cursor = "default";
   //if (document.form.imageidx.value=="0")
   the_timeout = setTimeout("redirect1()",((document.form.RefreshTime.value==0)?1:document.form.RefreshTime.value)*1000)
   //else
   //clearTimeout(the_timeout);   
}

function onClose()
{
   window.close();
}

function loadWidzard()
{
   //alert(document);
   //.document.form.PrinterStatus_x_WImageSize.value
  
   if ( browserType() != "MSIE" || cameraMode == "2")
   {      
      redirect(); 
   }   
   
}
</script>

</head>
<body onLoad="loadWidzard()" background="graph/sumtextb.jpg" bgcolor="#FFFFFF" text="#000066" link="#3333CC" vlink="#666699" alink="#990099">
<form method="GET" name="form" action="webcam.cgi">  
<input type="hidden" name="RefreshTime" value="<% nvram_get_x("PrinterStatus","usb_webfresh_x"); %>">
<input type="hidden" name="ImageSize" value="<% nvram_get_x("PrinterStatus","usb_webimage_x"); %>">
<input type="hidden" name="CaptionString" value="<% nvram_get_x("PrinterStatus","usb_webcaption_x"); %>">
<input type="hidden" name="CameraMode" value="<% nvram_get_x("PrinterStatus","usb_webmode_x"); %>">
<input type="hidden" name="CameraModel" value="<% nvram_get_x("PrinterStatus","usb_webdriver_x"); %>">
<input type="hidden" name="WebPort" value="<% nvram_get_x("PrinterStatus","usb_webhttpport_x"); %>">
<input type="hidden" name="CameraPort" value="<% nvram_get_x("PrinterStatus","usb_webactivex_x"); %>">
<input type="hidden" name="wan_proto" value="<% nvram_get_x("Layer3Forwarding","wan_proto"); %>">
<input type="hidden" name="wan_ipaddr" value="<% nvram_get_x("IPConnection","wan_ipaddr"); %>">
<input type="hidden" name="lan_ipaddr" value="<% nvram_get_x("LANHostConfig","lan_ipaddr"); %>">
<input type="hidden" name="ddns_server_x" value="<% nvram_get_x("LANHostConfig","ddns_server_x"); %>">
<input type="hidden" name="ddns_hostname_x" value="<% nvram_get_x("LANHostConfig","ddns_hostname_x"); %>">
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

if ( browserType() != "MSIE" || cameraMode == "2" )
{	
      	document.write('<tr>');
	document.write('<td align="left" height="48" style="padding-top:2;padding-bottom:2;padding-left:2">');
	document.write('<font face="MS Sans Serif" size="2" color="#000000">');
	document.write('<b>');
	document.write(Caption);
	document.write('</b>');
	document.write('</font>');
	document.write('</td>');
	document.write('<td align="right" style="padding-right:2">');
	//document.write('<font face="MS Sans Serif" size="2" color="#000000">');
	//document.write('<b>		  			');
	//document.write('<select name="imageidx" onChange="return change_image()">');
	//document.write('<option value="0">Current</option>');
	//document.write('<option value="1">Last 1th</option>');
	//document.write('<option value="2">Last 2th</option>');
	//document.write('<option value="3">Last 3th</option>');
	//document.write('<option value="4">Last 4th</option>');
	//document.write('<option value="5">Last 5th</option>');
	//document.write('<option value="6">Last 6th</option>');
	//document.write('<option value="7">Last 7th</option>');
	//document.write('<option value="8">Last 8th</option>');
	//document.write('<option value="9">Last 9th</option>');
	//document.write('<option value="10">Last 10th</option> ');  
	//document.write('<option value="11">Last 11th</option>');
	//document.write('<option value="12">Last 12th</option>');
	//document.write('</select>   ');
	//document.write('</b>');
	//document.write('</font>');
	document.write('</td>     ');           
	document.write('</tr>');
	document.write('<tr>');
	document.write('<td align="center" colspan="3">');
      	document.write('<img name="campicture" border="0" src="display.jpg" alt="Image is not ready" width="' + width + '" height="' + height + '">');
      	document.write('</td>');
	document.write('</tr>');    
}    
else
{
 	urlstr = location.href;	
      	url = urlstr.indexOf("http://");
      
      	if (url == -1) urlpref = document.form.lan_ipaddr.value;
      	else
      	{        
         	urlstr = urlstr.substring(7, urlstr.length);
         	url = urlstr.indexOf(":");
               
         	if (url!=-1)
         	{
	         	urlpref = urlstr.substring(0, url);
         	}
         	else
         	{
	         	url = urlstr.indexOf("/");
	        	if (url!=-1) urlpref = urlstr.substring(0, url);
	        	else urlpref = urlstr;
         	}
      	}   
      	
      	
                              
      	output = '';
      	output += '<object ID="mycamera" WIDTH="' + width + '" HEIGHT="' + height + '" CLASSID="CLSID:8569771E-4AFA-4FA9-A90F-AB98FC6403D9" CODEBASE="netcam_mfc_activeX.cab">';
      	output += '<PARAM NAME="NetCamip" VALUE="' + urlpref + ':' + document.form.CameraPort.value + ':' + width + ':' + height + ':12' + '">'
      	output += '</object>';
      
      
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
      	document.write(output);
      	document.write('</td>');
	document.write('</tr>');              
}    
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
