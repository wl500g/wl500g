<html>
<head>
<title>IGD Device Description</title>
<meta http-equiv="Content-Type" content="text/html; charset=ISO-8859-1">
<meta http-equiv="Content-Type" content="text/html; charset=UTF-8">
<meta http-equiv="Content-Type" content="text/html; charset=gb2312">
<meta http-equiv="Expires" content="Fri, Jun 12 1981 00:00:00 GMT">
<meta http-equiv="Cache-control" content="no-cache,must-revalidate">
<meta http-equiv="Pragma" content="no-cache">
<link rel="stylesheet" type="text/css" href="style.css" media="screen">
<script language="JavaScript" type="text/javascript" src="overlib.js"></script><script language="JavaScript" type="text/javascript" src="general.js"></script>
</head>
<div id="overDiv" style="position:absolute; visibility:hidden; z-index:1000;"></div>
<form method="get" name="form" action="apply.cgi">
<body>
<input type="hidden" name="wan_route_x" value="<% nvram_get_x("IPConnection","wan_route_x"); %>">
<input type="hidden" name="wan_nat_x" value="<% nvram_get_x("IPConnection","wan_nat_x"); %>">
<input type="hidden" name="productid" value="<% nvram_get_f("general.log","productid"); %>">
<table class="content_table" border="0" cellspacing="0" cellpadding="0">
<tr>
<td>
<img src="./graph/iBox_title_all.jpg" width="666" height="80">
</td>
</tr>
</table>
</body>
</form>
</html>