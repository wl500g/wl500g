<html>
<head>
<title>Entware</title>
<meta http-equiv="Content-Type" content="text/html; charset=UTF-8">
<meta http-equiv="Pragma" content="no-cache">
<!--<script type="text/javascript" src="overlib.js"></script>
<script type="text/javascript" src="general.js"></script>-->
<script language="javascript">
function redirect()
{  
   setTimeout("redirect1()",1000)
}
function redirect1() {
 top.location="index.asp"
}
</script>
</head>
<body bgcolor="#FFFFFF">
<!--<span class="wounderline"></span> -->
      <table width="660" height="125" cellpadding="2" cellspacing="0" border="0">
        <tbody> 
        <tr> 
          <td height="50" width="90%"><b><font size="5" face="Arial" color="#FF0000">Internet Connection Status</font></b></td>
        </tr>
        <tr> 
          <td width="652" height="60" colspan="2"><font face="Arial"><% nvram_get("StatusReason"); %></font></td>
        </tr>
        <tr>           
          <td align="center" width="400" height="60" colspan="2">
          <p><input class=inputSubmit onMouseOut=buttonOut(this) onMouseOver=buttonOver(this) type="button" onClick="redirect()" value="Next" name="action"></p>          
          </td>
        </tr>        
        </tbody> 
      </table>
</body>
</html>