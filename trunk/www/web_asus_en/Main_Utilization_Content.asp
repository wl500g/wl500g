<head>
<title>ZVMODELVZ Web Manager</title>
<meta http-equiv="Content-Type" content="text/html; charset=ISO-8859-1">
<meta http-equiv="Content-Type" content="text/html; charset=UTF-8">
<meta http-equiv="Content-Type" content="text/html; charset=gb2312">
<link rel="stylesheet" type="text/css" href="style.css" media="screen"></link><script language="JavaScript" type="text/javascript" src="overlib.js"></script><script language="JavaScript" type="text/javascript" src="general.js"></script>
<script>
  function intervalChange(value)
  {
     document.getElementById("cpu").data="graph_cpu.svg?#"+value;
     document.getElementById("ppp0").data="graph_if.svg?ppp0#"+value;
     document.getElementById("vlan1").data="graph_if.svg?vlan1#"+value;
     document.getElementById("eth1").data="graph_if.svg?eth11#"+value;
     document.getElementById("br0").data="graph_if.svg?br0#"+value;
  }
</script>
</head>  
<div id="overDiv" style="position:absolute; visibility:hidden; z-index:1000;"></div>    
<body onLoad="load_body()" onunLoad="return unload_body();">

<!-- Table for the conntent page -->	    
<table width="666" border="0" cellpadding="0" cellspacing="0">     	      
<tr>
<td>
<table width="666" border="1" cellpadding="0" cellspacing="0" bordercolor="E0E0E0">
<tr class="content_header_tr">
<td class="content_header_td_title" colspan="2">
Status & Log - System Utilization
</td>
</tr>
<tr class="content_header_tr">
<td colspan="2">
 <form>Refresh: <select  name="refresh"  onchange="intervalChange(this.value);">
 <option value="1" selected="selected">1 second</option><option value="2">2 seconds</option><option value="5">5 seconds</option>
 <option value="10">10 seconds</option><option value="30">30 seconds</option><option value="60">1 minute</option></select></form>
 <table align=center border=0>
 <tr><th>CPU Monitoring</th></tr>
 <tr><td><object id="cpu" type="image/svg+xml" width=530 height=250 data="graph_cpu.svg?#1"></object></td></tr>
 <tr><th>Bandwidth monitoring - Internet (ppp0)</th></tr>
 <tr><td><object id="ppp0" type="image/svg+xml" width=530 height=250 data="graph_if.svg?ppp0#1"></object></td></tr>
 <tr><th>Bandwidth monitoring - WAN (vlan1)</th></tr>
 <tr><td><object id="vlan1" type="image/svg+xml" width=530 height=250 data="graph_if.svg?vlan1#1"></object></td></tr>
 <tr><th>Bandwidth monitoring - Wireless (eth1)</th></tr>
 <tr><td><object id="eth1" type="image/svg+xml" width=530 height=250 data="graph_if.svg?eth1#1"></object></td></tr>
 <tr><th>Bandwidth monitoring - LAN (br0)</th></tr>
 <tr><td><object id="br0" type="image/svg+xml" width=530 height=250 data="graph_if.svg?br0#1"></object></td></tr>
 </table>
</td>
</tr>
</table>
</td>
</tr>
<tr>
<td>
<table width="666" border="2" cellpadding="0" cellspacing="0" bordercolor="E0E0E0"></table>
</td>
</tr>
<tr>
<td>
<table width="666" border="2" cellpadding="0" cellspacing="0" bordercolor="E0E0E0"></table>
</td>
</tr>
<tr>
<td>
<table width="666" border="2" cellpadding="0" cellspacing="0" bordercolor="E0E0E0"></table>
</td>
</tr>
<tr>
<td>
<table width="666" border="2" cellpadding="0" cellspacing="0" bordercolor="E0E0E0"></table>
</td>
</tr>
<tr>
<td>
<table width="666" border="2" cellpadding="0" cellspacing="0" bordercolor="E0E0E0"></table>
</td>
</tr>
<tr>
<td>
<table width="666" border="2" cellpadding="0" cellspacing="0" bordercolor="E0E0E0"></table>
</td>
</tr>
<tr>
<td>
<table width="666" border="2" cellpadding="0" cellspacing="0" bordercolor="E0E0E0"></table>
</td>
</tr>
<tr>
<td>
<table width="666" border="2" cellpadding="0" cellspacing="0" bordercolor="E0E0E0"></table>
</td>
</tr>
<tr>
<td>
<table width="666" border="2" cellpadding="0" cellspacing="0" bordercolor="E0E0E0"></table>
</td>
</tr>
</form>
<tr>
<td>		
<table width="666" border="1" cellpadding="0" cellspacing="0" bordercolor="B0B0B0">
<tr bgcolor="#CCCCCC"><td colspan="3"><font face="arial" size="2"><b>&nbsp</b></font></td></tr>
<tr bgcolor="#FFFFFF">  
   <td height="25" width="34%">
   </td>
   <td height="25" width="33%">
   </td>
   <td height="25" width="33%">
   <form method="get" name="form3" action="apply.cgi">		
   <input type="hidden" name="current_page" value="Main_LogStatus_Content.asp"><input type="hidden" name="action_mode" value=" Refresh ">
   <input type="hidden" name="next_host" value="">
   <div align="center"><font face="Arial"> <input class=inputSubmit onMouseOut=buttonOut(this) onMouseOver="buttonOver(this)" onClick="document.form3.next_host.value = location.host; onSubmitCtrl(this, ' Refresh ')" type="submit" value="Refresh" name="action"></font></div> 
   </form>
   </td>
</tr>
</table>
</td>
</tr>
</table>
</body>
