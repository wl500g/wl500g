<html>
<head>
<title>ASUS ZVMODELVZ Web Manager</title>
<meta http-equiv="Content-Type" content="text/html; charset=UTF-8">
<meta http-equiv="Pragma" content="no-cache">
<script language="javascript">

function loadBody()
{   
}

function redirect()
{  
   //setTimeout("redirect1()",1000)
   //alert(document.formname.WANDetc[0].checked);
   //alert(document.formname.WANDetc[1].checked);    
   window.location.href="index.asp"
   
}

function redirect1() 
{
   top.location="WizardDetect.asp"
}

function buttonOver(o) 
{
    o.style.backgroundColor = "#FFFFCC";
    o.style.cursor="hand";
}

function buttonOut(o) {
    o.style.backgroundColor = "#C0C0C0";
}
</script>

</head>
<body bgcolor="#FFFFFF" onload="loadBody()">
<form name="formname" method="POST">
      <table width="420" height="125" border="0" cellpadding="0" cellspacing="0">
        <tbody> 
        <tr> 
          <td bgcolor="#FFFFFF" height="50" colspan="2"><b><font size="5" face="Arial" color="#FF0000">Internet Connection Status</font></b></td>
        </tr>
        <tr>
	   <td height="30"><font face="Arial" color="#000000">Type</font></td>
	   <td><font face="Arial" color="#000000"><input type="text" size="36" name="PPPConnection_x_WANType" value="<% nvram_get_x("Layer3Forwarding","wan_proto_t"); %>" readonly></font></td>
	</tr>
	<tr>
	   <td height="30"><font face="Arial" color="#000000">Status:</font></td>
	   <td><font face="Arial" color="#000000"><input type="text" size="36" name="PPPConnection_x_WANLink" value="<% nvram_get_f("wanstatusshow.log","wan_status_t"); %>" readonly></font></td>
	</tr>	
	<tr>
	   <td height="30"><font face="Arial" color="#000000">Reason</font></td>
	   <td><font face="Arial" color="#000000">
	   <input type="text" size="36" name="PPPConnection_x_WANStatus" value="<% nvram_get_f("wanstatusshow.log","wan_reason_t"); %>" readonly>
	   <input type="hidden" size="36" name="PPPConnection_x_WANStatusCode" value="<% nvram_get_f("wanstatusshow.log","wan_reasoncode_t"); %>" readonly>
	   </font></td>	   
	</tr>	
	<tr> 
          <td width="420" height="30" bgcolor="#FFFFFF" colspan="2">          
          </td>
        </tr>               
        <tr>           
          <td align="right" width="420" height="60" bgcolor="#FFFFFF" colspan="2">
          <p><input class=inputSubmit onMouseOut="buttonOut(this)" onMouseOver=buttonOver(this) type="button" onClick="redirect()" value="Login" name="action"></p>
          </td>
        </tr>             
        </tbody> 
      </table>
</form>
</body>
</html>
