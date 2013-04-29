<html>
<head>
<meta http-equiv="Content-Type" content="text/html; charset=UTF-8">
<title>Route Edit Tips</title>
<link rel="stylesheet" type="text/css" href="style.css" media="screen">

<SCRIPT LANGUAGE="JavaScript">

function loadWidzard()
{
   
}

function functionChange(o)
{   
     
}

function optionChange(o)
{      
   
}

function onClose()
{
    window.close();
}

</script>

</head>

<body  onLoad="loadWidzard()">
<form method="POST" name="form" action="--WEBBOT-SELF--">  
<!--webbot bot="SaveResults" U-File="fpweb:///_private/form_results.txt"
S-Format="TEXT/CSV" S-Label-Fields="TRUE" -->  
<table border="1" width="542">
  <tr>
    <td class="content_header_td_title"  width="532"><font face="Arial" size="5" color="#FFFFFF">
    <b>Route Edit Tips</b>
     </font>
  </td>
  </tr>  
  <tr>
    <td class="content_desc_td" width="532">   
    <p><font size="4"><b>Add/Delete entries</b></font></p>
	<ul>
      <li>
        <b>Add</b> : Input data into first row of table, then press <b>Add</b> to insert it. System will remind you if the table is full or required fields are incorrectly filled.
      </li> 
      <li>
        <b>Del</b> : Select one entry in table, or press <b>Ctrl</b> and select multiple entries in tables, then press <b>Del</b> to delete them.
      </li>
	  </ul>
    <p><font size="4"><b>Input Format</b></font></p>        
      <p>The fields of a route rule are listed below:</p> 
	<ul>	  
      <li>
        <b>Network/Host IP</b> : It stands for the destination network or host of a route roule. So it could be a host address, such as "192.168.123.11" or a network address, such as "192.168.0.0".
      </li>       
      <li>
        <b>Netmask Bits</b> : It indicates how many bits are for network ID and subnet ID. For example: if the dotted-decimal netmask is 255.255.255.0, then its netmask bits is 24. If the destination is a host, its netmask bits should be 32.
      </li> 
      <li>
        <b>Gateway</b> : It stands for the IP address of gateway where packets are routed to. The specified gateway must be reachable first. It means you have to set up a static route to the gateway beforehand.
      </li>
	</ul>	  
    <p align="right" style="margin-right:20"><input type="button" onClick="onClose()" value="Close" name="Close1">    
        <p></p>
    </td>
  </tr>
</table>
</form>
</body>

</html>