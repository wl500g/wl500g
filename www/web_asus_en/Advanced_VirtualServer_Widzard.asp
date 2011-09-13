<html>

<head>
<meta http-equiv="Content-Type" content="text/html; charset=UTF-8">
<title>Port Forwarding Wizard</title>

<SCRIPT LANGUAGE="JavaScript">

wItem = new Array(	
    new Array(
      new Array("User Defined", "0", "TCP"),     
      new Array("FTP", "20:21", "TCP"),
      new Array("TELNET", "23", "TCP"),
      new Array("SMTP", "25", "TCP"),
      new Array("DNS", "53", "BOTH"),
      new Array("FINGER", "79", "TCP"),
      new Array("HTTP", "80", "TCP"),
      new Array("POP3", "110", "TCP"),
      new Array("SNMP", "161", "UDP"),
      new Array("SNMP TRAP", "162", "UDP")
    ),
           
    new Array(
      new Array("User Defined", "0", "TCP")
    )
    );

var funcIdx, optIdx;


function loadWizard()
{
   funcIdx = "0";
   document.form.LocalIP.disabled = 0;
   document.form.LocalIP.style.backgroundColor = "white";	    
   document.form.LocalIP.focus();     
   document.form.Description.value = "Virtual Server";  
}

function functionChange(o)
{   
  c = document.form.option.options.length;
          
  for (i=0; i<c; i++)
  {       
    	document.form.option.options[0].value = null;
    	document.form.option.options[0] = null;
  } 
  
  funcIdx = o.value;
    
  if (o.value>=0 && o.value<=1)
  {                           
     for (i = 0; i < wItem[funcIdx].length; i++) {
        if (wItem[funcIdx][i][0]!=null)
        {
	   document.form.option.options[i] = new Option(wItem[funcIdx][i][0]);	    
	   document.form.option.options[i].value = wItem[funcIdx][i][0];	   
	}  
	document.form.option.options[0].selected=1; 
     }
     document.form.option.length = wItem[funcIdx].length;
  }  
  else 
  { 
     document.form.option.length = 0;
  }  
     
    document.form.LocalPort.value = "";
    
    switch (funcIdx)
    {
        case "-1":
       document.form.LocalIP.disabled = 0;
	   document.form.LocalIP.style.backgroundColor = "white";	     	         	   
	   document.form.LocalPort.disabled = 0;
	   document.form.LocalPort.style.backgroundColor = "white";	 
	   document.form.Description.value = "User Defined";
	   document.form.LocalIP.focus();   
	   break
 	case "0": 
 	   document.form.LocalIP.disabled = 0;
	   document.form.LocalIP.style.backgroundColor = "white"; 
	   document.form.LocalPort.disabled = 0;
	   document.form.LocalPort.style.backgroundColor = "white";	 	   
	   document.form.Description.value = "Virtual Server";  
	   document.form.LocalIP.focus();   
	   break	
	case "1":
	   document.form.LocalIP.disabled = 0;
	   document.form.LocalIP.style.backgroundColor = "white";	 
	   document.form.LocalPort.disabled = 1;
	   document.form.LocalPort.style.backgroundColor = "gray";	 	   	 
	   document.form.Description.value = "Software DMZ Host";	   	   
	   document.form.LocalIP.focus();   
	   break
	default: 
	   break;		
    }              
}

function optionChange(o)
{      
    for (i = 0; i < wItem[funcIdx].length; i++) {
        if (wItem[funcIdx][i][0]!=null)
        {
	    if (o.value == document.form.option.options[i].value)
	    {
	         optIdx = i;	
	                	        	        
	         if (wItem[funcIdx][optIdx][2]=="TCP")
	            document.form.Protocol.options[0].selected = 1;
	         else if (wItem[funcIdx][optIdx][2]=="UDP")
	            document.form.Protocol.options[1].selected = 1;
	         else 
	            document.form.Protocol.options[2].selected = 1;
   	            
	         document.form.LocalPort.value = wItem[funcIdx][optIdx][1];	 	               
	         
	         switch (funcIdx)
    		 { 
 			case "0":
	   		document.form.Description.value = "";  
	   		document.form.LocalIP.focus();    
	   		break			
			case "1":
	   		document.form.Description.value = "Software DMZ Host: ";
	   		document.form.LocalIP.focus();
	   		break
			default: 
	   		break;		
    		 }       	         	         
	         document.form.Description.value += wItem[funcIdx][optIdx][0] + " Server (" + wItem[funcIdx][optIdx][1] + ")"; 	         
	    }	    
	}   
    } 	
}

function apply()
{
    /* Find empty */
    xform = opener.document.form;
    
    if (document.form.LocalIP.value != "")
    {        
       opener.document.form.IPConnection_VServerIP_0.value = document.form.LocalIP.value;
       opener.document.form.IPConnection_VServerPort_0.value = document.form.LocalPort.value
       opener.document.form.IPConnection_VServerProto_0.value = document.form.Protocol.value;  
       opener.document.form.IPConnection_VServerDescript_0.value = document.form.Description.value;      
       window.close();
    }   
    else alert("IP can't be blank");    
}

function cancel()
{
    window.close();
}

</script>

</head>

<body  onLoad="loadWizard()">
<form method="POST" name="form" action="--WEBBOT-SELF--">  
<!--webbot bot="SaveResults" U-File="fpweb:///_private/form_results.txt"
S-Format="TEXT/CSV" S-Label-Fields="TRUE" -->  
<table border="1" width="542">
  <tr>
    <td bgcolor="#FF9933" bordercolor="#FF9933" width="532"><font face="Arial" size="5" color="#FFFFFF"><b>Virtual 
      Server Wizard</b></font></td>
  </tr>
  <tr>
    <td width="532"><p><font face="Arial" size="2">To make services, like WWW, FTP, provided  
      by a server in your local network accessible for outside users, you should  
      specify a local IP address to the server. Then, add the IP address and  
      network protocol type, port number, and name of the service in the  
      following list. Based on the list, the gateway will forward service  
      request from outside users to the corresponding local server.</font></p> 
    </td>
  </tr>
  <tr>
    <td width="532">
        <center>
        <table border="2" width="98%" height="26" bordercolorlight="#008080" bordercolordark="#008080" bordercolor="#008080">
          <tr>
            <td width="37%" height="1"><font size="2"><b>Type</b></font></td>
            <td width="126%" height="1" valign="middle" colspan="3">
              
                <p><select size="1" name="function" onChange="functionChange(this);">                                                 
                  <option value="0"  selected>Virtual Server</option>          
                </select></p>
             
            </td>
          </tr>
          <tr>
            <td width="37%" height="20"><font size="2"><b>Service</b></font></td>
            <td width="126%" height="20" colspan="3">
              <select size="1" name="option" onChange="optionChange(this);">
                  <option value="User Defined" selected>User Defined</option>                  
                  <option value="FTP">FTP</option>
                  <option value="TELNET">TELNET</option>
                  <option value="SMTP">SMTP</option>
                  <option value="DNS">DNS</option>
                  <option value="FINGER">FINGER</option>
                  <option value="HTTP">HTTP</option>
                  <option value="POP3">POP3</option>
                  <option value="SNMP">SNMP</option>
                  <option value="SNMP TRAP">SNMP TRAP</option>                            
              </select>
            </td>
          </tr>
          <tr>
            <td width="37%" height="20"><font size="2"><b>Protocol</b></font></td>
            <td width="126%" height="20" colspan="3"><select size="1" name="Protocol">
                <option value="TCP">TCP</option>
                <option value="UDP">UDP</option>
                <option value="BOTH">BOTH</option>
              </select></td>
          </tr>
          <tr>
            <td width="37%" height="20"><font size="2"><b>Local IP Address</b></font></td>
            <td width="126%" height="20" colspan="3">
              
                <!--webbot bot="SaveResults"
                U-File="fpweb:///_private/form_results.txt" S-Format="TEXT/CSV"
                S-Label-Fields="TRUE" -->
                <p><input type="text" name="LocalIP" size="26"></p>
             
            </td>
          </tr>
          <tr>
            <td width="37%" height="20"><font size="2"><b>Port Range</b></font></td> 
            <td width="126%" height="20" colspan="3"><input type="text" name="LocalPort" size="26"></td>
          </tr>                    
          <tr>
            <td width="37%" height="20"><font size="2"><b>Description</b></font></td>
            <td width="126%" height="20" colspan="3"><input type="text" name="Description" size="26"></td>
          </tr>
          <tr>
            <td width="37%" height="20"></td>
            <td width="57%" height="20"></td>
            <td width="35%" height="20"><input type="button" value="Apply" name="Apply" onClick="apply();"></td>
            <td width="34%" height="20"><input type="button" value="Cancel" name="Cancel" onClick="cancel();"></td>
          </tr>
        </table>
        </center>       
     </td>
  </tr>
</table>
</form>
</body>

</html>
