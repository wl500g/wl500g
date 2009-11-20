 
<html>
<head>
<title>ASUS ZVMODELVZ Web Manager</title>
<meta http-equiv="Content-Type" content="text/html; charset=ISO-8859-1">
<meta http-equiv="Content-Type" content="text/html; charset=UTF-8">
<meta http-equiv="Content-Type" content="text/html; charset=gb2312">
<meta http-equiv="Content-Type" content="text/html">
<meta http-equiv="Pragma" content="no-cache">
<script language="javascript">

function loadBody()
{
   document.formname.WANDetc[0].checked = true;
   document.formname.WANDetc[1].checked = false;
}

function redirect()
{  
   //setTimeout("redirect1()",1000)
   //alert(document.formname.WANDetc[0].checked);
   //alert(document.formname.WANDetc[1].checked);
   
   if (document.formname.WANDetc[0].checked == true)
        window.location.href="WizardDetect.asp"
   else
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

<style type="text/css">
<!--
.wounderline {  font-family: "Arial", "Helvetica", "sans-serif"; text-decoration: none; color: #FFFFFF; font-size: 9pt}
-->
</style>

</head>
<body bgcolor="#FFFFFF" onload="loadBody()">
<form name="formname" method="POST">
<input type="hidden" size="36" name="PPPConnection_x_WANStatusCode" value="<% nvram_get_f("wanstatus.log","WANStatusCode"); %>" readonly="1">
<span class="wounderline"></span> 
<table width="420" border="0" cellpadding="0" cellspacing="0" vspace="30" height="1">
  <tr bgcolor="#FFFFFF"> 
    <td width="420" height="125"> 
      <table width="420" height="125" border="0" cellpadding="0" cellspacing="0" border="0">
        <tbody> 
        <tr> 
          <td bgcolor="#FFFFFF" height="30"><b><font size="5" face="Arial" color="#FF0000">Setup Wizard</font></b></td>
        </tr>        
        <tr> 
          <td width="420" height="16" colspan="2"></td>          
        </tr>
        <tr> 
          <td width="420" height="30" bgcolor="#FFFFFF" colspan="2"><font face="Arial">Thank you for purchasing the ASUS Wireless Router. Pressing Next directly will guide to into Quick Setup.</font></td>
        </tr>
        <tr> 
          <td width="420" height="16" colspan="2"></td>          
        </tr>
        <tr> 
          <td width="420" height="30" bgcolor="#FFFFFF" colspan="2">
          <input type="radio" checked name="WANDetc" value="AutoDetc" tabindex="1"><font face="Arial">I'd like to start Quick Setup directly.</font>
          </td>
        </tr>
        <tr> 
          <td width="420" height="30" bgcolor="#FFFFFF" colspan="2">
          <input type="radio" checked name="WANDetc" value="AutoDetc" tabindex="2"><font face="Arial">I'd like to set up my Wireless Router manually.</font>
          </td>
        </tr>        
        <tr>           
          <td align="right" width="420" height="60" bgcolor="#FFFFFF" colspan="2">
          <p><input class=inputSubmit onMouseOut=buttonOut(this) onMouseOver=buttonOver(this) type="button" onClick="redirect()" value="Next" name="action"></p>
          </td>
        </tr>            
        </tbody> 
      </table>
    </td>
  </tr>
</table>
</form>
</body>
</html>