<html>
<head>
	<title>ZVMODELVZ Web Manager</title>
	<meta http-equiv="Content-Type" content="text/html; charset=UTF-8">
	<link rel="stylesheet" type="text/css" href="style.css" media="screen">

	<script type="text/javascript" src="overlib.js"></script>

	<script type="text/javascript" src="general.js"></script>

	<script type="text/javascript">
        function get_syscmd_out(xhr)
        {
            var url = "http://"+window.location.host+"/syscmd_out.asp";
			var ifrm = document.getElementById("outIframe");
			ifrm.src=url;
        }
        function onfail()
        {
			var ifrm = document.getElementById("outIframe");
			ifrm.src="";
            self.close();
        }

        function show_result(xhr)
        {
            var str=xhr.responseText;
            document.form.result.value=str;
        }

        function performQuery(query){
            var frm=document.form;

            // var str="state_modem "+frm.port.value+" "+query;
            var str="state_modem "+frm.wan_modem_usbloc.value+"."+frm.wan_modem_pui.value+" "+query;
            var url=getCmdExecUrl( window.location.host, str );
            getHTTPRequest( url, get_syscmd_out );

            return false;
        }
        function performQueryUSSD(){
            var frm=document.form;
            performQuery("r "+frm.ussd.value);
            return false;
        }
        function performQueryInfo(){
            performQuery("i");
        }
        function performQuerySignal(){
            performQuery("s");
        }
        function performQueryTraffic(){
            performQuery("t");
        }

		window.changeUSBparams = function (value)
		{
		//value is associated array
			var frm=document.form;
			if(value){
				frm.wan_modem_usbloc.value = value.loc;
				frm.wan_modem_pui.value = value.ui;
			}
		}
        </script>

</head>
<body>
	<input type="hidden" name="action_mode" value="">
	<div id="overDiv" style="position: absolute; visibility: hidden; z-index: 1000;">
	</div>
	<form onsubmit="return performQueryUSSD();" name="form">
	<table width="666" border="1" cellpadding="0" cellspacing="0" bordercolor="#E0E0E0">
		<tr class="content_header_tr">
			<td class="content_header_td_title" colspan="2">
				Status &amp; Log - USSD
			</td>
		</tr>
		<tr>
			<td class="content_header_td" onmouseover="return overlib('Select parameters for connected USB modem')"
				onmouseout="return nd()">
				View connected modems
			</td>
			<td class="content_input_td">
				<input type="button" value="View" onclick="window.open('devpath_select_form.asp','Select_devpath','width=800,height=200,left=150,top=200,scrollbars=1')">
			</td>
			<tr>
				<td class="content_header_td" onmouseover="return overlib('USB device location in USB tree. Press View button or see Status &amp; Log -> Diagnostic Info -> USB devfs Devices and find the ID in the second column (eg 1.1.0). If blank, use default.')"
					onmouseout="return nd()">
					USB device location ID:
				</td>
				<td class="content_input_td">
					<input type="text" maxlength="256" class="content_input_fd" size="32" name="wan_modem_usbloc"
						value="<% nvram_get_x("LANHostConfig","wan0_modem_usbloc"); %>" onkeypress="return is_string(event, this)"
						onblur="validate_string(this)">
				</td>
			</tr>
			<tr>
				<td class="content_header_td" onmouseover="return overlib('This field allows you to specify user interface port manually. If Autodetect device option is enabled, the router will define it automatically.')"
					onmouseout="return nd()">
					User interface port:
				</td>
				<td class="content_input_td">
					<input type="text" maxlength="256" class="content_input_fd" size="32" name="wan_modem_pui"
						value="<% nvram_get_x("3GConfig","wan0_modem_pui"); %>" onkeypress="return is_string(event, this)"
						onblur="validate_string(this)">
				</td>
			</tr>
<!--			<tr>
				<td class="content_header_td" onmouseover="return overlib('This field specifies the modem port for USSD', LEFT);"
					onmouseout="return nd();">
					USB port:
				</td>
				<td class="content_input_td">
					<input type="text" maxlength="256" class="content_input_fd" name="port" value="<% nvram_get_x("port","wan0_modem_ussd_port"); %>"
						onkeypress="return is_string(event, this)" onblur="validate_string(this)">
				</td>
			</tr>	-->
			<tr>
				<td class="content_header_td" onmouseover="return overlib('This field is for entering a query USSD ', LEFT);"
					onmouseout="return nd();">
					USSD:
				</td>
				<td class="content_input_td">
					<input type="text" maxlength="256" class="content_input_fd" name="ussd" value="<% nvram_get_x("port","ussd_query"); %>"
						onkeypress="return is_string(event, this)" onblur="validate_string(this)">
				</td>
			</tr>
			<tr>
				<td>
					<input type="button" value="Get Info" onclick="return performQueryInfo();">
					<input type="button" value="Get Signal" onclick="return performQuerySignal();">
					<input type="button" value="Get Traffic" onclick="return performQueryTraffic();">
				</td>
				<td>
					<input type="submit">
				</td>
			</tr>
			<tr>
				<td colspan="2">
					<iframe id="outIframe" width="100%" src=""></iframe>
				</td>
			</tr>
	</table>
	</form>
	<pre></pre>
</body>
</html>
