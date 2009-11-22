<html>
<head>
<title>ASUS ZVMODELVZ Web Manager</title>
<meta http-equiv="Content-Type" content="text/html; charset=ISO-8859-1">
<meta http-equiv="Content-Type" content="text/html; charset=UTF-8">
<meta http-equiv="Content-Type" content="text/html; charset=gb2312">
<meta http-equiv="Content-Type" content="text/html">
<meta http-equiv="refresh" content="<% nvram_get_x("Storage","Remaining"); %>">
<meta http-equiv="Pragma" content="no-cache">
<link rel="stylesheet" type="text/css" href="style.css" media="screen"></link>
<script language="javascript">
function check()
{  
   if (document.form.status.value == "OK")
   {    	
       location="Main_AStatus_Content.asp";   
   }   
}
</script>
</head>
<body bgcolor="#FFFFFF" onLoad="check()">
<form method="GET" name="form" action="apply.cgi">  
<input type="hidden" name="tool" value="<% nvram_get_x("Storage","Tool"); %>">
<span class="wounderline"></span>
<table width="666" border="0" cellpadding="2" cellspacing="0" &ace="30" height="1">
  <tr bgcolor="#CCCCCC"> 
    <td width="666" height="125"> 
      <table width="660" height="125" cellpadding="2" cellspacing="0" border="0">
        <tbody>
        <tr  class="content_header_tr"> 
          <td class="content_header_td_title_red" colspan="2">DISK Utilities</td>
        </tr>
        <tr>
          <td class="content_desc_td" bgcolor="#FFFFFF" colspan="2">DISK tool is working, please wait until status page is shown.......</td>
         </tr>
        <tr> 
          <td width="652" height="25" bgcolor="#FFFFFF" colspan="2"></td>
        </tr>
        <tr>
          <td width="652" height="25" bgcolor="#FFFFFF" colspan="2"><font face="Arial"> 
            <font color="#FF0000">
            <b>Status</b><p></p><input type="text" name="status" value="<% nvram_get_x("Storage","Status"); %>">
            </font>
            </font>
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