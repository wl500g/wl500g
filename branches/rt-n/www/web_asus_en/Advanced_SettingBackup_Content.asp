<head>
<title>ZVMODELVZ Web Manager</title>
<meta http-equiv="Content-Type" content="text/html; charset=ISO-8859-1">
<meta http-equiv="Content-Type" content="text/html; charset=UTF-8">
<meta http-equiv="Content-Type" content="text/html; charset=gb2312">
<meta http-equiv="Content-Type" content="text/html">
<link rel="stylesheet" type="text/css" href="style.css" media="screen">
<script language="JavaScript" type="text/javascript" src="overlib.js"></script>
<script language="JavaScript" type="text/javascript" src="general.js"></script>
<script language="JavaScript" type="text/javascript" src="quick.js"></script>
</head>  
<body bgcolor="#FFFFFF">
<div id="overDiv" style="position:absolute; visibility:hidden; z-index:1000;"></div>    
<form method="post" action="upload.cgi" enctype="multipart/form-data">
<input type="hidden" name="action_mode" value="">
<!-- Table for the conntent page -->	    
<table width="660" border="0" cellpadding="1" cellspacing="0">     	      
    	
<tr>
<td>
<table width="666" border="2" cellpadding="0" cellspacing="0" bordercolor="E0E0E0">
<tr class="content_header_tr">
<td class="content_header_td_title" colspan="2">System Setup - Setting Management</td>
<input type="hidden" name="current_page" value="Basic_Operation_Content.asp">
<input type="hidden" name="next_page" value="Basic_SaveRestart.asp">
</tr>
<tr>
<td class="content_desc_td" colspan="2" width="614">
                  <p style="margin-top: 7">This function allows you to save  current settings of ZVMODELVZ to a file, or load settings from a file.
</td>
</tr>

<tr class="content_section_header_tr">
<td id="Mode" class="content_section_header_td" colspan="2" width="614">Save As a File</td>
</tr>
<tr>
<td class="content_desc_td" colspan="2" width="614">
<p style="margin-top: 7"> 
                 Move your cursor over <a href="/ZVMODELVZ.CFG">HERE</a>. Then click the right button of mouse and select <b>"Save As..."</b> to save current setting of ZVMODELVZ into a file. (Note: While you save current settings to a file, it will be saved to flash as well.)
<p style="margin-buttom: 7">                    
</td>
</tr>
<tr class="content_section_header_tr">
<td id="Mode" class="content_section_header_td" colspan="2" width="614">Load From a File</td>
</tr>
<tr>
<td class="content_desc_td" colspan="2" width="614">                           		                                     		
              <p style="margin-top: 7">
              Specify the path of and name of the downloaded file in the <b>"New Setting File"</b> below. Then, click <b>"Upload"</b> to write the file to ZVMODELVZ. It takes a few time to finish the process and then the system will reboot.            
</td>
</tr>
<tr>
<td class="content_header_td" width="30">New Setting File:</td><td class="content_input_td">   
                  <input class="inputSubmit" name="file" size="20" type="file" ></td>
</tr>
<tr>
<td class="content_header_td"></td><td class="content_input_td" width="595">
  <p align="left"><input class="inputSubmit" type="submit" onClick="onSubmitCtrlOnly(this, 'Upload')" value="Upload">
  </p>
</td>
</tr>
</table>
</td>
</tr>
</table>
</form>
</body>
