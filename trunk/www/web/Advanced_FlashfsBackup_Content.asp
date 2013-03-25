<html>
<head>
<title><% model(2); %></title>
<meta http-equiv="Content-Type" content="text/html; charset=UTF-8">
<link rel="stylesheet" type="text/css" href="style.css" media="screen">
<script type="text/javascript" src="overlib.js"></script>
<script type="text/javascript" src="general.js"></script>
<script type="text/javascript" src="quick.js"></script>
</head>  
<body bgcolor="#FFFFFF">
<div id="overDiv" style="position:absolute; visibility:hidden; z-index:1000;"></div>    
<form method="post" action="uploadflashfs.cgi" enctype="multipart/form-data">
<input type="hidden" name="action_mode" value="">
<input type="hidden" name="current_page" value="Advanced_FlashfsBackup_Content.asp">
<input type="hidden" name="next_page" value="SaveRestart.asp">
<!-- Table for the conntent page -->	    
<table width="666" border="2" cellpadding="0" cellspacing="0" bordercolor="#E0E0E0">
<tr class="content_header_tr">
<td class="content_header_td_title" colspan="2">System Setup - Flashfs Management</td>
</tr>
<tr>
<td class="content_desc_td" colspan="2" width="614">
                  <p style="margin-top: 7">This function allows you to save current flashfs content of the router to a file, or load flashfs content from a file.
</td>
</tr>

<tr class="content_section_header_tr">
<td id="Mode" class="content_section_header_td" colspan="2" width="614">Save As a File</td>
</tr>
<tr>
<td class="content_desc_td" colspan="2" width="614">
<p style="margin-top: 7"> 
                 Move your cursor over <a href="/<% model(); %>.tar.gz">HERE</a>. Then click the right button of mouse and select <b>"Save As..."</b> to save current flashfs content of the router into a file. 
<p style="margin-buttom: 7">                    
</td>
</tr>
<tr class="content_section_header_tr">
<td id="Mode" class="content_section_header_td" colspan="2" width="614">Load From a File</td>
</tr>
<tr>
<td class="content_desc_td" colspan="2" width="614">                           		                                     		
              <p style="margin-top: 7">
              Specify the path of and name of the downloaded file in the <b>"New Flashfs File"</b> below. Then, click <b>"Upload"</b> to write the file to the router. It takes a few time to finish the process and then the system will enable flashfs and reboot.            
</td>
</tr>
<tr>
<td class="content_header_td" width="30">New Flashfs File:</td><td class="content_input_td">   
                  <input class="inputSubmit" name="file" size="20" type="file" ></td>
</tr>
<tr>
<td class="content_header_td"></td><td class="content_input_td" width="595">
  <p align="left"><input class="inputSubmit" type="submit" onClick="onSubmitCtrlOnly(this, 'Upload')" value="Upload">
  </p>
</td>
</tr>
</table>
</form>
</body>
</html>