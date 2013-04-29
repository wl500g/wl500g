<html>
<head>
<meta http-equiv="Content-Type" content="text/html; charset=UTF-8">
<title>the router Web Camera Page</title>
<link rel="stylesheet" type="text/css" href="style.css" media="screen">

<SCRIPT LANGUAGE="JavaScript">

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

function loadWidzard()
{      	    					
    	R1 = '<% nvram_get("usb_webremote1_x"); %>'
	R2 = '<% nvram_get("usb_webremote2_x"); %>'
	R3 = '<% nvram_get("usb_webremote3_x"); %>'
	R4 = '<% nvram_get("usb_webremote4_x"); %>'
	R5 = '<% nvram_get("usb_webremote5_x"); %>'
	R6 = '<% nvram_get("usb_webremote6_x"); %>'
	NAT = '<% nvram_get("wan_nat_x"); %>'
	
	aspName = "/ShowWebCam.asp";
	
	LANIP = '<% nvram_get("lan_ipaddr"); %>'
	//alert('LANIP ' + LANIP);
	
	urlstr = location.href;	
	url = urlstr.indexOf("http://");
	
	if (url==-1) urlpref = LANIP;
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
			
	if (urlpref != LANIP && urlpref!="my.router" && urlpref.indexOf("my.WL")==-1 && NAT=='1')
	{
		port1 = R1.indexOf(":");
	   	if (port1!=-1) R1=R1.substring(url);
	   	port2 = R2.indexOf(":");	
	   	if (port2!=-1) R2=R2.substring(url);
	   	port3 = R3.indexOf(":");
	   	if (port3!=-1) R3=R3.substring(url);
	   	port4 = R4.indexOf(":");	
	   	if (port4!=-1) R4=R4.substring(url);
	   	port5 = R5.indexOf(":");
	   	if (port5!=-1) R5=R5.substring(url);
	   	port6 = R6.indexOf(":");
       	   	if (port6!=-1) R6=R6.substring(url);
			
	   	HREF1="http://" + urlpref + R1 + aspName;
	   	HREF2="http://" + urlpref + R2 + aspName;
	   	HREF3="http://" + urlpref + R3 + aspName;
	   	HREF4="http://" + urlpref + R4 + aspName;
	   	HREF5="http://" + urlpref + R5 + aspName;
	   	HREF6="http://" + urlpref + R6 + aspName;
	}
	else
	{	   		
	   	HREF1="http://" + R1 + aspName;
	   	HREF2="http://" + R2 + aspName;
	   	HREF3="http://" + R3 + aspName;
	   	HREF4="http://" + R4 + aspName;
	   	HREF5="http://" + R5 + aspName;
	   	HREF6="http://" + R6 + aspName;
	}
		
						
	if (R4=="" && R5=="" && R6=="")	
	   document.write('<frameset frameborder="0" rows="*">');
	else 
	   document.write('<frameset frameborder="0" rows="*,*">');            
	   
	if (R4!=""&&R5==""&&R6=="")
	   col2 = 1;
	else
	   col2 = 0;		       
			
	if (R1!="" || R2!="" || R3!="")
	{
		if (col2==1)
		   document.write('<frameset frameborder="0" cols="*,*">');
		else if (R3=="")
		{
		   if (R2=="") document.write('<frameset frameborder="0" cols="*">');
		   else document.write('<frameset frameborder="0" cols="*,*">');  
		}   
		else   			   
  		   document.write('<frameset frameborder="0" cols="*,*,*">');
  		   
  		if (R1!="")
     		   document.write('<frame name="r1" src="' + HREF1 + '">');     		
     		if (R2!="")   
     		   document.write('<frame name="r2" src="' + HREF2 + '">');     		
     		if (col2==0&&R3!="")
     		   document.write('<frame name="r3" src="' + HREF3 + '">');
  		document.write('</frameset>');
  	}
  	
  	if (R4!="" || R5!="" || R6!="")
	{
  		if (col2==1)
		   document.write('<frameset frameborder="0" cols="*,*">');
		else if (R6=="")
		{
		   if (R5=="") document.write('<frameset frameborder="0" cols="*">');
		   else document.write('<frameset frameborder="0" cols="*,*">');  
		}   
		else   			   
  		   document.write('<frameset frameborder="0" cols="*,*,*">');   
  		   
  		if (col2==1&&R3!="")
  		   document.write('<frame name="r3" src="' + HREF3 + '">');   
  		if (R4!="")
     		   document.write('<frame name="r4" src="' + HREF4 + '">');
     		if (R5!="")   
     		   document.write('<frame name="r5" src="' + HREF5 + '">');
     		if (R6!="")   
     		   document.write('<frame name="r6" src="' + HREF6 + '">');
  		document.write('</frameset>');
  	}  	  		  	
  	
  	document.write('<noframes>');
  	document.write('<body>');
  	document.write('<p>This page use frame, but your browser not support it.</p>');
  	document.write('</body>');
  	document.write('</noframes>');
	document.write('</frameset>');
}	
</script>
</head>
<body onLoad="loadWidzard()" background="graph/sumtextb.jpg" bgcolor="#FFFFFF" text="#000066" link="#3333CC" vlink="#666699" alink="#990099">
<form method="GET" name="form" action="webcamremote.cgi">  
<input type="hidden" name="Remote1" value="<% nvram_get("usb_webremote1_x"); %>">
<input type="hidden" name="Remote2" value="<% nvram_get("usb_webremote2_x"); %>">
<input type="hidden" name="Remote3" value="<% nvram_get("usb_webremote3_x"); %>">
<input type="hidden" name="Remote4" value="<% nvram_get("usb_webremote4_x"); %>">
<input type="hidden" name="Remote5" value="<% nvram_get("usb_webremote5_x"); %>">
<input type="hidden" name="Remote6" value="<% nvram_get("usb_webremote6_x"); %>">
</form>
</body>

</html>