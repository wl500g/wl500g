 
<html>
<head>
<title>ASUS iBox | Cgange Password</title>
<meta http-equiv="Content-Type" content="text/html; charset=big5">
<style type="text/css">
<!--
.wounderline {  font-family: "Arial", "Helvetica", "sans-serif"; text-decoration: none; color: #FFFFFF; font-size: 9pt}
-->
</style>
<script src="/js/general.js"></script>
<SCRIPT src="/js/changepswd.js" type=text/javascript></SCRIPT>
</head>
<body bgcolor="#FFFFFF">
<span class="wounderline"></span> 
<form method="GET" action="/cgi-bin/chkpass" name="chgpw_form" onSubmit="return launch_chgpw_form('chgpw');">
<table width="620" border="0" cellpadding="2" cellspacing="0" vspace="30" height="60">
  <tr> 
    <td width="620" height="30" bgcolor="#FFBB00"><font color="#FFFFFF"><font face="Arial, Helvetica, sans-serif"><b> 
      Login Password</b></font></font color="#008000"></td>
  </tr>
  <tr bgcolor="#CCCCCC"> 
    <td width="620"> 
         <input type="hidden" name="pwdFunc" value="chgpswd">
        <center>
        </center>
        <table border="0" width="620" cellspacing="1" cellpadding="0">
          <center>
            <tr bgcolor="#FFFFFF"> 
              <td width="185" align="left"><b><font size="2" face="Arial">Current 
                Password:</font></b></td>
              <td width="431" height="25"><font face="Arial"></font> 
                <input maxlength=32 
                        name=userpwd type=password 
                        message="Password is Required You Nerd!" 
                        required="yes" size="20">
              </td>
            </tr>
            <tr bgcolor="#FFFFFF"> 
              <td width="185" align="left"><b><font size="2" face="Arial">New 
                Password:</font></b></td>
              <td width="431" height="25"> 
                <input maxlength=32 
                        name=newpwd type=password 
                        message="Password is Required You Nerd!" 
                        required="yes" size="20">
              </td>
            </tr>
            <tr bgcolor="#FFFFFF"> 
              <td width="185" align="left" height="25"><b><font size="2" face="Arial">Retype 
                New Password:</font></b></td>
              <td width="431" height="25"> 
                <input maxlength=32 
                        name=verpwd type=password 
                        message="Password is Required You Nerd!" 
                        required="yes" size="20">
              </td>
            </tr>
          </center>
          <tr bgcolor="#FFFFFF"> 
            <center>
              <td colspan="2" align="left" height="25"> 
                <p align="right"> 
                  <input class=inputSubmit onMouseOut="buttonOut(this)" onMouseOver=buttonOver(this) type=submit value=' Modify ' name="chgpwdbutton1">
                  <input class=inputSubmit onMouseOut="buttonOut(this)" onMouseOver=buttonOver(this) type=reset value='   Clear   ' name="reset">
                </p>
              </td>
            </center>
          </tr>
        </table>
    </td>
  </tr>
</table>
</form>
</body>
</html>
