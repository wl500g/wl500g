 
<html>
<head>
<title>ASUS ZVMODELVZ Web Manager</title>
<meta http-equiv="Content-Type" content="text/html; charset=ISO-8859-1">
<meta http-equiv="Content-Type" content="text/html; charset=UTF-8">
<meta http-equiv="Content-Type" content="text/html; charset=gb2312">
<meta http-equiv="Content-Type" content="text/html">
<meta http-equiv="Pragma" content="no-cache">
<script language="JavaScript" type="text/javascript" src="overlib.js"></script>
<script language="JavaScript" type="text/javascript" src="general.js"></script>
<script language="javascript">
function redirect()
{  
   setTimeout("redirect1()",1000)
}
function redirect1() {
 top.location="index.asp"
}
</script>

<style type="text/css">
<!--
.wounderline {  font-family: "Arial", "Helvetica", "sans-serif"; text-decoration: none; color: #FFFFFF; font-size: 9pt}
-->
</style>

</head>
<body bgcolor="#FFFFFF">
<span class="wounderline"></span> 
<table width="666" border="0" cellpadding="0" cellspacing="0" &ace="30" height="1">
  <tr bgcolor="#FFFFFF"> 
    <td width="666" height="125"> 
      <table width="660" height="125" cellpadding="2" cellspacing="0" border="0">
        <tbody> 
        <tr> 
          <td bgcolor="#FFFFFF" height="50" width="90%"><b><font size="5" face="Arial" color="#FF0000">Internet Connection Status</font></b></td>
        </tr>
        <tr> 
          <td width="652" height="60" bgcolor="#FFFFFF" colspan="2"><font face="Arial"><% nvram_get_f("wan.log","StatusReason"); %></font></td>
        </tr>
        <tr>           
          <td align="center" width="400" height="60" bgcolor="#FFFFFF" colspan="2">
          <p><input class=inputSubmit onMouseOut=buttonOut(this) onMouseOver=buttonOver(this) type="button" onClick="redirect()" value="Next" name="action"></p>          
          </td>
        </tr>        
        </tbody> 
      </table>
    </td>
  </tr>
</table>
</body>
</html>