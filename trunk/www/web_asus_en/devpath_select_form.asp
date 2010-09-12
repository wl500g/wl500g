<html>
	<head>
	<script type="text/javascript" src="general.js"></script>
	<script type="text/javascript">
		function get_syscmd_out(xhr)
		{
			var url = "http://"+window.location.host+"/syscmd_out.asp";
			getHTTPRequest( url, fill_table );
		}
		function fill_table(xhr)
		{
			generate_table(xhr.responseText);
		}
		function get_html()
		{
			var url=getCmdExecUrl( window.location.host, "/usr/sbin/check_devpath" );
			getHTTPRequest( url, get_syscmd_out );
		}
		function onfail()
		{
			self.close();
		}

		function generate_table( data_str )
		{

			var str="<table class='sel_devpath'>\n\
				<tr><th>Device path</th><th>USB Location</th><th colspan='2'>Info</th></tr>";

			var ar=data_str.split( "\n" );
			for(var i in ar ){
				var str1=ar[i];
				if(i==0 || !str1.length)continue;
				str+="<tr>";
				var col_ar=str1.split( " : " );
				if( col_ar.length ){
				    for(var j in col_ar ){
					str+="<td onclick='select_td(this)' onmouseover='fill_row(this, 1)' onmouseout='fill_row(this, 0)'>";
					str+=col_ar[j];
					str+="</td>";
				    }
				};
				str+="</tr>";
			}
			str+="</table>";

			document.getElementById("tb1").innerHTML=str;
		}
		function select_td(el)
		{
			var row=el.parentNode;
			var path=row.children[1];
			opener.document.forms[0].wan_modem_usbloc.value=path.innerHTML;
			self.close();
		}
		function fill_row(el, fill){
			var row=el.parentNode;
			if( fill === 0 ){
				row.bgColor="#FFFFFF";
			} else {
				row.bgColor="#6666FF";
			}
		}
	</script>
	<style type="text/css">
		.sel_devpath{
			border: solid 1px #E0E0E0; background:#FFFFFF; margin: 0; border-collapse:separate; padding: 0; border-spacing:0; 
			width: 100%;
		}
		.sel_devpath td, th {
			border: solid 1px #E0E0E0;
		}
		.sel_devpath th {
			background:#FFBB00; 
		}
	</style>
	</head>	
	<body onload="return get_html()">
		<div id="tb1" style="text-align:center; vertical-align:middle; font-size:32px;">Please wait...</div>
	</body>
</html>
