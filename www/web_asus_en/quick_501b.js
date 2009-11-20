var prevTagLabel = new Array(
                       Array("Country",""),
                       Array("Broadband",""),
                       Array("PPPoE",""),
                       Array("MacHost",""),
                       Array("WANSetting",""),
                       Array("Firewall",""),
                       Array("Wireless",""));
                       
var countryTime = new Array();                       
var prevTag = 0;
var sameWireless=true;
var tempSVC;
var tempDate;
var tempTime;

function inputCtrl1(o, flag)
{
    if (flag == 0)
    {
       o.disabled = 1;   
       o.style.backgroundColor = "gray";   
    }
    else
    {	
       o.disabled = 0;
       o.style.backgroundColor = "white";	  	  
    }   
}

function inputRCtrl1(o, flag)
{
    if (flag == 0)
    {
       o[0].disabled = 1;   
       o[1].disabled = 1;
       //o.style.backgroundColor = "gray";   	
    }
    else
    {	
       o[0].disabled = 0;
       o[1].disabled = 0;
       //o.style.backgroundColor = "white";	  	  
    }   
}

function inputRCtrl2(o, flag)
{
    if (flag == 0)
    {
       o[0].checked = true;   
       o[1].checked = false;
       //o.style.backgroundColor = "gray";   	
    }
    else
    {	
       o[0].checked = false;
       o[1].checked = true;
       //o.style.backgroundColor = "white";
    }   
}

function inputCCtrl4(o, flag)
{ 
    o[0].checked = false;
    o[1].checked = false;
    o[2].checked = false;
    o[3].checked = false;
    o[flag].checked = true;   
}   

function change_security(o, s, l)
{    	
    if (o.value == "0")
    {
    	pflag = 0;
    	wflag = 0;
    	
    	authTypeValue = "0"; 	// Open or Shared
    	wepTypeValue = "0";	// None
    }
    else if (o.value == "1")
    {
    	pflag = 1;
    	wflag = 0;
    	
    	authTypeValue = "1";	// Shared
    	wepTypeValue = "1";     // WEP-128 bits
    }
    else if (o.value == "2")    
    {
    	pflag = 1;
    	wflag = 0;
    	
    	authTypeValue = "2";	// WPA-PSK only
    	wepTypeValue= "0";	// TKIP
    }	
   
    else if (o.value == "3")	
    {
    	pflag = 0;
    	wflag = 0;
    	authTypeValue = "";
    	wepTypeValue = "";
    }
    
    if (o.options.length == 4 && o.value != "3")
    {
    	if (confirm("You have chosen an additional wireless authenication and encryption option, do you really like to overwrite it?"))
    	{
    	     o.options[3].value = null;
    	     o.options[3] = null;
    	     
    	     document.form.WLANConfig11b_AuthenticationMethod.value = authTypeValue;
             document.form.WLANConfig11b_WEPType.value = wepTypeValue;
    	}   
    	else
    	{
    	     pflag = 0;
    	     wflag = 0;
    	     o.value = "3"; 
    	     authTypeValue="";
    	     wepTypeValue="";
    	}
    }
    
    if (authTypeValue!="")
    {
    	document.form.WLANConfig11b_AuthenticationMethod.value = authTypeValue;
        document.form.WLANConfig11b_WEPType.value = wepTypeValue;
    }
    
    
    if (l==0)
    {
       document.form.WLANConfig11b_x_Phrase.value = "";
       	
       if (o.value == "1") 
       {
       		wep = "00000000000000000000000000";
       		is_wlphrase("WLANConfig11b", document.form.WLANConfig11b_x_Phrase);
       }		
       else
       { 
       		wep = "";                    
       		document.form.WLANConfig11b_WEPKey1.value = wep;
       		document.form.WLANConfig11b_WEPKey2.value = wep;
       		document.form.WLANConfig11b_WEPKey3.value = wep;
       		document.form.WLANConfig11b_WEPKey4.value = wep;
       }		
       
    }  
    
    inputCtrl(document.form.WLANConfig11b_x_Phrase, pflag);
    inputCtrl(document.form.WLANConfig11b_WEPKey1, wflag);
    inputCtrl(document.form.WLANConfig11b_WEPKey2, wflag);
    inputCtrl(document.form.WLANConfig11b_WEPKey3, wflag);
    inputCtrl(document.form.WLANConfig11b_WEPKey4, wflag);
    inputCtrl(document.form.WLANConfig11b_WEPDefaultKey,  wflag);
    
    if (pflag == "1" && l == 0)
    {    	     	  
        document.form.WLANConfig11b_x_Phrase.focus();
    }
}

function loadSecurity()
{	
    authType = 	document.form.WLANConfig11b_AuthenticationMethod.value;
    wepType = document.form.WLANConfig11b_WEPType.value;
    
    if (authType == "0" && wepType == "0")
    {
    	document.form.SecurityLevel.value = "0";		
    }	
    else if (authType == "1" && wepType == "1")
    {
    	document.form.SecurityLevel.value = "1";
    }
    else if (authType == "2" && wepType == "0")
    {
    	document.form.SecurityLevel.value = "2";
    }
    else 
    {
    	add_option_x(document.form.SecurityLevel, 3, "Extra", 1);
    	document.form.SecurityLevel.value = "3";
    }
    
    change_security(document.form.SecurityLevel, "WLANConfig11b", 1);
}

function loadQuick()
{	    	    	
    /* Transfer NATEnabled, Layer3Forwarding_x_ConnectionType to x_Mode, x_Connection Rule */
     //alert(document.form.Layer3Forwarding_x_ConnectionType.value);              
    document.form.next_host.value = location.host;
            
    //alert(document.form.IPConnection_x_DNSServerEnable[0].checked);
    //alert(document.form.IPConnection_x_DNSServerEnable[1].checked);                    	         	
        	
    if (document.form.IPConnection_NATEnabled.value == '1')
    {    	
    	document.form.x_Mode.value = 'Home Gateway';
    	    	
    	if (document.form.Layer3Forwarding_x_ConnectionType.value == 'PPPoE')
    	{
    	    //document.form.x_Connection.value = 'ADSL';
    	    inputCCtrl4(document.form.x_WANType, 1);
    	    //inputRCtrl2(document.form.x_ADSLType, 1);
            inputRCtrl2(document.form.x_DHCPClient, 0);
    	}   
    	else if (document.form.Layer3Forwarding_x_ConnectionType.value == 'PPTP')
    	{
    	    //document.form.x_Connection.value = 'ADSL';
    	    inputCCtrl4(document.form.x_WANType, 2);    	    
    	    //inputRCtrl2(document.form.x_ADSLType, 1);
            inputRCtrl2(document.form.x_DHCPClient, 1);
    	}   
    	else   
    	{
    	    if (document.form.Layer3Forwarding_x_ConnectionType.value == 'Static IP')
    	    {
    	    	inputCCtrl4(document.form.x_WANType, 3);
    		//inputRCtrl2(document.form.x_ADSLType, 0);
    		inputRCtrl2(document.form.x_DHCPClient, 1);
    	    }
    	    else 
    	    {   
    	    	inputCCtrl4(document.form.x_WANType, 0);
    		//inputRCtrl2(document.form.x_ADSLType, 0); 	
        	inputRCtrl2(document.form.x_DHCPClient, 0);
    	    }   	
    	    //document.form.x_Connection.value = 'Cable';
    	}       	       	 
    	
    	/* For firewall setting */    	
    	/* Handle for LanWanActiveDateSVC */    	
    	//tempSVC = document.form.FirewallConfig_LanWanActiveSVC.value;
    	//tempDate = document.form.FirewallConfig_LanWanActiveDate.value;
	//tempTime = document.form.FirewallConfig_LanWanActiveTime.value;
    	//document.form.FirewallConfig_LanWanActiveSVC_0.checked = getDateCheck(document.form.FirewallConfig_LanWanActiveSVC.value, 0);
    	//document.form.FirewallConfig_LanWanActiveSVC_1.checked = getDateCheck(document.form.FirewallConfig_LanWanActiveSVC.value, 1);
    	//document.form.FirewallConfig_LanWanActiveSVC_2.checked = getDateCheck(document.form.FirewallConfig_LanWanActiveSVC.value, 2);
    	//document.form.FirewallConfig_LanWanActiveSVC_3.checked = getDateCheck(document.form.FirewallConfig_LanWanActiveSVC.value, 3);
    	//document.form.FirewallConfig_LanWanActiveSVC_4.checked = getDateCheck(document.form.FirewallConfig_LanWanActiveSVC.value, 4);
    	//document.form.FirewallConfig_LanWanActiveSVC_5.checked = getDateCheck(document.form.FirewallConfig_LanWanActiveSVC.value, 5);
    	    	
    	//document.form.FirewallConfig_LanWanActiveDate_Sun.checked = getDateCheck(document.form.FirewallConfig_LanWanActiveDate.value, 0);
    	//document.form.FirewallConfig_LanWanActiveDate_Mon.checked = getDateCheck(document.form.FirewallConfig_LanWanActiveDate.value, 1);
    	//document.form.FirewallConfig_LanWanActiveDate_Tue.checked = getDateCheck(document.form.FirewallConfig_LanWanActiveDate.value, 2);
    	//document.form.FirewallConfig_LanWanActiveDate_Wed.checked = getDateCheck(document.form.FirewallConfig_LanWanActiveDate.value, 3);
    	//document.form.FirewallConfig_LanWanActiveDate_Thu.checked = getDateCheck(document.form.FirewallConfig_LanWanActiveDate.value, 4);
    	//document.form.FirewallConfig_LanWanActiveDate_Fri.checked = getDateCheck(document.form.FirewallConfig_LanWanActiveDate.value, 5);
    	//document.form.FirewallConfig_LanWanActiveDate_Sat.checked = getDateCheck(document.form.FirewallConfig_LanWanActiveDate.value, 6);               	    	
    	
    	//document.form.FirewallConfig_LanWanActiveTime_starthour.value = getTimeRange(document.form.FirewallConfig_LanWanActiveTime.value, 0);
        //document.form.FirewallConfig_LanWanActiveTime_startmin.value = getTimeRange(document.form.FirewallConfig_LanWanActiveTime.value, 1);
        //document.form.FirewallConfig_LanWanActiveTime_endhour.value = getTimeRange(document.form.FirewallConfig_LanWanActiveTime.value, 2);
        //document.form.FirewallConfig_LanWanActiveTime_endmin.value = getTimeRange(document.form.FirewallConfig_LanWanActiveTime.value, 3);
    }
    else if (document.form.IPConnection_ConnectionType.value == 'IP_Routed')
    {
    	document.form.x_Mode.value = 'Router';
    	//document.form.x_Connection.value = '';
    	//inputRCtrl2(document.form.x_ADSLType, 0);
        inputRCtrl2(document.form.x_DHCPClient, 1);    	    	
    }
    else
    {
    	document.form.x_Mode.value = 'Access Point';
    	//document.form.x_Connection.value = '';
    } 
      
    if (window.top.isModel() == "WL600")     
    {
       if (document.form.WLANConfig11a_SSID.value != document.form.WLANConfig11b_SSID.value ||
          document.form.WLANConfig11a_WEPType.value != document.form.WLANConfig11b_WEPType.value ||   
          document.form.WLANConfig11a_WEPKey1.value != document.form.WLANConfig11b_WEPKey1.value ||
          document.form.WLANConfig11a_WEPKey2.value != document.form.WLANConfig11b_WEPKey2.value ||
          document.form.WLANConfig11a_WEPKey3.value != document.form.WLANConfig11b_WEPKey3.value ||
          document.form.WLANConfig11a_WEPKey4.value != document.form.WLANConfig11b_WEPKey4.value ||
          document.form.WLANConfig11a_WEPDefaultKey.value != document.form.WLANConfig11b_WEPDefaultKey.value)
       {
          sameWireless=false;       	   
       }
       else
       {
          sameWireless=true;
       }
    }
    else    
    {
       loadSecurity();
       sameWireless=true;              
    }   
        	                                     
    changeMode();    
}

function loadMode()
{	    	
    /* Transfer NATEnabled, Layer3Forwarding_x_ConnectionType to x_Mode, x_Connection Rule */
     //alert(document.form.Layer3Forwarding_x_ConnectionType.value);
    document.form.next_host.value = location.host;
            
    //alert(document.form.IPConnection_x_DNSServerEnable[0].checked);
    //alert(document.form.IPConnection_x_DNSServerEnable[1].checked);
    if (window.top.isModel() == "WL300")
    {
    	if (document.form.IPConnection_NATEnabled.value == '1')
    	{    	
    		document.form.x_Mode[0].checked = false;
    		document.form.x_Mode[1].checked = true;    	   
    	}
    	else
    	{
    		document.form.x_Mode[0].checked = true;
    		document.form.x_Mode[1].checked = false;         
    	}       
    	
    }        	         	
    else
    {    	
    	if (document.form.IPConnection_NATEnabled.value == '1')
    	{    	
    		document.form.x_Mode[0].checked = true;
    		document.form.x_Mode[1].checked = false;    	
    		//document.form.x_Mode[2].checked = false;    	    	    	    	  	       	    	    	    	
    	}
    	//else if (document.form.IPConnection_ConnectionType.value == 'IP_Routed')
    	//{
    	//	document.form.x_Mode[0].checked = false;
    	//	document.form.x_Mode[1].checked = true;
    	//	document.form.x_Mode[2].checked = false;      	    	
    	//}
    	else
    	{
    		document.form.x_Mode[0].checked = false;
    		document.form.x_Mode[1].checked = true;
        	//document.form.x_Mode[2].checked = true;  
    	}                                      
    }	
    //changeMode();    
}

function setSVCCheck(d1, d2, d3, d4, d5, d6)
{
   str = "";
  
   if (d6 == true ) str = "1" + str;
   else str = "0" + str;
   if (d5 == true ) str = "1" + str;
   else str = "0" + str;
   if (d4 == true ) str = "1" + str;
   else str = "0" + str;
   if (d3 == true ) str = "1" + str;
   else str = "0" + str;
   if (d2 == true ) str = "1" + str;
   else str = "0" + str;
   if (d1 == true ) str = "1" + str;
   else str = "0" + str;   
   
   return str;
}

function saveQuick(o)
{
/*	
    if (document.form.IPConnection_NATEnabled.value == '1')
    {    	
    	document.form.x_Mode.value == 'Home Gateway'
    	    	
    	if (document.form.Layer3Forwarding_x_ConnectionType.value == 'PPPoE')    	
    	   document.form.x_Connection.value == 'ADSL';
    	else   
    	   document.form.x_Connection.value == 'Cable';
    }
    else if (document.form.IPConnection_ConnectionType.value == 'IP_Routed')
    {
    	document.form.x_Mode.value == 'Router'
    }
    else
    {
    	document.form.x_Mode.value == 'Access Point'
    }       
*/
    /* Copy Wireless 11b to Wireless 11a */   
    window.top.pageChanged = 0;
    window.top.pageChangedCount = 0;	   	
    
    if (!sameWireless)
    {
       if (confirm('Your 802.11a setting is different from 802.11b setting. Would you like to apply the same setting to both interfaces? Or just press "Cancel" to ignore wireless setting.'))
       {	
           document.form.WLANConfig11a_SSID.value = document.form.WLANConfig11b_SSID.value;
    	   document.form.WLANConfig11a_WEPType.value = document.form.WLANConfig11b_WEPType.value;
    	   document.form.WLANConfig11a_WEPKey1.value = document.form.WLANConfig11b_WEPKey1.value;
    	   document.form.WLANConfig11a_WEPKey2.value = document.form.WLANConfig11b_WEPKey2.value;
    	   document.form.WLANConfig11a_WEPKey3.value = document.form.WLANConfig11b_WEPKey3.value;
    	   document.form.WLANConfig11a_WEPKey4.value = document.form.WLANConfig11b_WEPKey4.value;
    	   document.form.WLANConfig11a_WEPDefaultKey.value = document.form.WLANConfig11b_WEPDefaultKey.value;
       }	
    }
    else
    {
       document.form.WLANConfig11a_SSID.value = document.form.WLANConfig11b_SSID.value;
       document.form.WLANConfig11a_WEPType.value = document.form.WLANConfig11b_WEPType.value;
       document.form.WLANConfig11a_WEPKey1.value = document.form.WLANConfig11b_WEPKey1.value;
       document.form.WLANConfig11a_WEPKey2.value = document.form.WLANConfig11b_WEPKey2.value;
       document.form.WLANConfig11a_WEPKey3.value = document.form.WLANConfig11b_WEPKey3.value;
       document.form.WLANConfig11a_WEPKey4.value = document.form.WLANConfig11b_WEPKey4.value;
       document.form.WLANConfig11a_WEPDefaultKey.value = document.form.WLANConfig11b_WEPDefaultKey.value;
    }
    document.form.WLANConfig11a_x_AESEnable.value = "0";          	 
          	 
    inputCtrl(document.form.WLANConfig11b_x_Phrase, 1);          	 
    inputCtrl(document.form.WLANConfig11b_WEPKey1, 1);
    inputCtrl(document.form.WLANConfig11b_WEPKey2, 1);  
    inputCtrl(document.form.WLANConfig11b_WEPKey3, 1);     
    inputCtrl(document.form.WLANConfig11b_WEPKey4, 1); 
    inputCtrl(document.form.WLANConfig11b_WEPDefaultKey, 1);               	           	   
    //document.form.next_page.value = 'Main_Content.asp';
       	   
    if (document.form.x_Mode.value == 'Access Point')
    {           	                
        document.form.IPConnection_NATEnabled.value = '0';
        document.form.Layer3Forwarding_x_ConnectionType.value = 'Static IP';  
        document.form.IPConnection_ConnectionType.value = 'IP_Bridged';
      
    }       
    else if (document.form.x_Mode.value == 'Router')
    {              
        document.form.IPConnection_NATEnabled.value = '0';
        document.form.Layer3Forwarding_x_ConnectionType.value = 'Static IP';
        document.form.IPConnection_ConnectionType.value = 'IP_Routed';       
    }   
    else
    {             
        document.form.IPConnection_NATEnabled.value = '1';
        document.form.IPConnection_ConnectionType.value = 'IP_Routed';
        if (document.form.x_WANType[1].checked == true)
        {
           document.form.Layer3Forwarding_x_ConnectionType.value = 'PPPoE';
        }   
        else if (document.form.x_WANType[2].checked == true)
        {
           document.form.Layer3Forwarding_x_ConnectionType.value = 'PPTP';
        }   
        else if (document.form.x_WANType[3].checked == true)
        {                     	
           document.form.Layer3Forwarding_x_ConnectionType.value = 'Static IP';    
        }  
        else
        {        	
           document.form.Layer3Forwarding_x_ConnectionType.value = 'Automatic IP';            
        } 
                               
        //if (document.form.FirewallConfig_LanWanActiveSVCFlag.value!="")
        //{        	     	
        //	document.form.FirewallConfig_LanWanActiveSVC.value = setSVCCheck(
   	//   		document.form.FirewallConfig_LanWanActiveSVC_0.checked,
   	//   		document.form.FirewallConfig_LanWanActiveSVC_1.checked,
   	//   		document.form.FirewallConfig_LanWanActiveSVC_2.checked,
   	//   		document.form.FirewallConfig_LanWanActiveSVC_3.checked,
   	//   		document.form.FirewallConfig_LanWanActiveSVC_4.checked,
   	//   		document.form.FirewallConfig_LanWanActiveSVC_5.checked);
        //	     
       	//	document.form.FirewallConfig_LanWanActiveDate.value = setDateCheck(
   	//   		document.form.FirewallConfig_LanWanActiveDate_Sun,
   	//   		document.form.FirewallConfig_LanWanActiveDate_Mon,
   	//   		document.form.FirewallConfig_LanWanActiveDate_Tue,
   	//   		document.form.FirewallConfig_LanWanActiveDate_Wed,
   	//   		document.form.FirewallConfig_LanWanActiveDate_Thu,
   	//   		document.form.FirewallConfig_LanWanActiveDate_Fri,
   	//   		document.form.FirewallConfig_LanWanActiveDate_Sat);
   	
   	//		document.form.FirewallConfig_LanWanActiveTime.value = setTimeRange(
   	//   		document.form.FirewallConfig_LanWanActiveTime_starthour,
   	//   		document.form.FirewallConfig_LanWanActiveTime_startmin,
   	//   		document.form.FirewallConfig_LanWanActiveTime_endhour,
   	//   		document.form.FirewallConfig_LanWanActiveTime_endmin);    	         	     
   	   		
   	   		// Embeded rule here
	//		if (document.form.FirewallConfig_LanWanActiveSVC_5.checked == true )
   	//   		{
   	//   			
   	//   			document.form.FirewallConfig_WanLanFirewallEnable.value = "1";
   	//   			document.form.FirewallConfig_LanWanDefaultAct.value = "DROP";  
   	//   			document.form.FirewallConfig_LanWanActiveSVCFlag.value = setSVCCheck(
	//			!document.form.FirewallConfig_LanWanActiveSVC_0.checked,
	//			!document.form.FirewallConfig_LanWanActiveSVC_1.checked,
	//			!document.form.FirewallConfig_LanWanActiveSVC_2.checked,
	//			!document.form.FirewallConfig_LanWanActiveSVC_3.checked,
	//			!document.form.FirewallConfig_LanWanActiveSVC_4.checked,			
	//			false);
   	   			// insert rule of SVC that not checked    	   		
   	//   		}
   	//  		else
   	//   		{
   	//   			document.form.FirewallConfig_WanLanFirewallEnable.value = "1";
   	//   			document.form.FirewallConfig_LanWanDefaultAct.value = "ACCEPT";   	   	
   	//   			document.form.FirewallConfig_LanWanActiveSVCFlag.value = setSVCCheck(
	//			document.form.FirewallConfig_LanWanActiveSVC_0.checked,
	//			document.form.FirewallConfig_LanWanActiveSVC_1.checked,
	//			document.form.FirewallConfig_LanWanActiveSVC_2.checked,
	//			document.form.FirewallConfig_LanWanActiveSVC_3.checked,
	//			document.form.FirewallConfig_LanWanActiveSVC_4.checked,			
	//			false);
	//			if (document.form.FirewallConfig_LanWanActiveSVCFlag.value == "000000" )
	//			{
	//				document.form.FirewallConfig_WanLanFirewallEnable.value = "0";
	//			}	
   	//   			// insert rule of SVC that check
   	//   		}
	//		//alert(document.form.FirewallConfig_LanWanActiveSVCFlag.value);
   	//	}  		
   	//	else
   	//	{
   	//		document.form.FirewallConfig_LanWanActiveSVC.value = tempSVC;
   	//		document.form.FirewallConfig_LanWanActiveDate.value = tempDate;
	//		document.form.FirewallConfig_LanWanActiveTime.value = tempTime;
   	//	}
   		
   	//	document.form.FirewallConfig_LanWanActiveSVC_0.checked = false;
    	//	document.form.FirewallConfig_LanWanActiveSVC_1.checked = false;    		
    	//	document.form.FirewallConfig_LanWanActiveSVC_2.checked = false;    		
    	//	document.form.FirewallConfig_LanWanActiveSVC_3.checked = false;
    	//	document.form.FirewallConfig_LanWanActiveSVC_4.checked = false;    		
    	//	document.form.FirewallConfig_LanWanActiveSVC_5.checked = false;
    		document.form.SecurityLevel.disabled = true;
    		    		    		    		
    		//document.form.x_Connection.disabled = true;
    		document.form.x_DHCPClient.disabled = true;
    		//document.form.x_ADSLType.disabled = true;
    		document.form.x_DHCPClient[0].checked = false;
    		document.form.x_DHCPClient[1].checked = false;   
    		//document.form.x_ADSLType[0].checked = false;   
    		//document.form.x_ADSLType[1].checked = false;                             		    		
    		
    		    		    		    		    		
    		if (window.top.isModel() != "WL600")     
    		{
    		    document.form.WLANConfig11a_SSID.disabled = true;    		    
          	    document.form.WLANConfig11a_WEPType.disabled = true;
          	    document.form.WLANConfig11a_WEPKey1.disabled = true;
          	    document.form.WLANConfig11a_WEPKey2.disabled = true;          	    
          	    document.form.WLANConfig11a_WEPKey3.disabled = true;          	    
          	    document.form.WLANConfig11a_WEPKey4.disabled = true;          	    
          	    document.form.WLANConfig11a_WEPDefaultKey.disabled = true;          	      		    		
    		}
    }             
    
    //if (o.value == ' Finish ')
    //{
    //   o.value = '  Save  ';
    //}
    o.value = '  Save  ';   
    document.form.action_mode.value = '  Save  ';
    return true;
}

function changeMode()
{   	    		    	
    if (document.form.x_Mode.value == 'Home Gateway')
    {             	 	     	 
    	 if (document.form.x_WANType[0].checked == true)
    	 {    	     	     	         	         	        	
    	     inputCtrl1(document.form.PPPConnection_UserName, 0); 
    	     inputCtrl1(document.form.PPPConnection_Password, 0);     	   
    	         	         	   
    	     inputRCtrl1(document.form.x_DHCPClient, 1);
    	     inputRCtrl2(document.form.x_DHCPClient, 0);
    	     inputCtrl1(document.form.IPConnection_ExternalIPAddress, 1); 
    	     inputCtrl1(document.form.IPConnection_x_ExternalSubnetMask, 1);     
    	     inputCtrl1(document.form.IPConnection_x_ExternalGateway, 1);
    	     
    	     //inputRCtrl1(document.form.IPConnection_x_DNSServerEnable, 1);
    	     //inputRCtrl2(document.form.IPConnection_x_DNSServerEnable, 0);
    	     //inputCtrl1(document.form.IPConnection_x_DNSServer1, 1);  
    	     //inputCtrl1(document.form.IPConnection_x_DNSServer2, 1);
    	         	     
    	     changeDHCPClient();    
    	     changeDNSServer();	    	    	    	
    	 }
    	 else 
    	 {
    	 	
    	     // require account 	
    	     if (document.form.x_WANType[1].checked == true || document.form.x_WANType[2].checked == true)
    	     {    	      	     	     	      	         	     	
    	        inputCtrl1(document.form.PPPConnection_UserName, 1); 
    	        inputCtrl1(document.form.PPPConnection_Password, 1);     	         	   
    	     }   
    	     else
    	     {
    	     	inputCtrl1(document.form.PPPConnection_UserName, 0); 
    	        inputCtrl1(document.form.PPPConnection_Password, 0);     	         	   
    	     }
    	     
    	     // require IP
    	     if (document.form.x_WANType[0].checked == true || document.form.x_WANType[1].checked == true)    	      
    	     {
    	     	inputRCtrl1(document.form.x_DHCPClient, 1);
    	     	inputRCtrl2(document.form.x_DHCPClient, 0);
    	     	inputCtrl1(document.form.IPConnection_ExternalIPAddress, 1); 
    	     	inputCtrl1(document.form.IPConnection_x_ExternalSubnetMask, 1);     
    	     	inputCtrl1(document.form.IPConnection_x_ExternalGateway, 1);
    	     
    	     	//inputRCtrl1(document.form.IPConnection_x_DNSServerEnable, 1);    	
    	     	//inputRCtrl2(document.form.IPConnection_x_DNSServerEnable, 0);     	
    	     	//inputCtrl1(document.form.IPConnection_x_DNSServer1, 1);  
    	     	//inputCtrl1(document.form.IPConnection_x_DNSServer2, 1); 
    	     }	
    	     else if(document.form.x_WANType[2].checked == true) // PPTP 	    	     
    	     {
    	     	inputRCtrl1(document.form.x_DHCPClient, 1);
    	     	inputRCtrl2(document.form.x_DHCPClient, 1);
    	     	inputCtrl1(document.form.IPConnection_ExternalIPAddress, 0); 
    	     	inputCtrl1(document.form.IPConnection_x_ExternalSubnetMask, 0);     
    	     	inputCtrl1(document.form.IPConnection_x_ExternalGateway, 0);
    	     
    	     	//inputRCtrl1(document.form.IPConnection_x_DNSServerEnable, 1);
    	     	//inputRCtrl2(document.form.IPConnection_x_DNSServerEnable, 1);
    	     	//inputCtrl1(document.form.IPConnection_x_DNSServer1, 1);
    	     	//inputCtrl1(document.form.IPConnection_x_DNSServer2, 1);
    	     }	
    	     else // static IP
    	     {    	     	
    	     	inputRCtrl1(document.form.x_DHCPClient, 1);
    	     	inputRCtrl2(document.form.x_DHCPClient, 1);
    	     	inputCtrl1(document.form.IPConnection_ExternalIPAddress, 0);
    	     	inputCtrl1(document.form.IPConnection_x_ExternalSubnetMask, 0);
    	     	inputCtrl1(document.form.IPConnection_x_ExternalGateway, 0);
    	     
    	     	//inputRCtrl1(document.form.IPConnection_x_DNSServerEnable, 1);
    	     	//inputRCtrl2(document.form.IPConnection_x_DNSServerEnable, 1);
    	     	//inputCtrl1(document.form.IPConnection_x_DNSServer1, 1);  
    	     	//inputCtrl1(document.form.IPConnection_x_DNSServer2, 1); 
    	     }
    	     changeDHCPClient();
    	     changeDNSServer();
    	 }
    }
    else if (document.form.x_Mode.value == 'Access Point')
    {    	    	     	    	     
         //inputCtrl1(document.form.x_Connection, 0); 
    	 //inputCtrl1(document.form.PPPConnection_UserName, 0); 
    	 //inputCtrl1(document.form.PPPConnection_Password, 0);     
    	  	     	     	     	     	 
    	 //inputRCtrl1(document.form.IPConnection_x_DNSServerEnable, 0);    	 
    	 //inputCtrl1(document.form.IPConnection_x_DNSServer1, 0);  
    	 //inputCtrl1(document.form.IPConnection_x_DNSServer2, 0);
    	 
    	 
    	 //inputCtrl1(document.form.LANHostConfig_IPRouters, 1);
    	 //inputCtrl1(document.form.LANHostConfig_SubnetMask, 1);  	
    	 //inputRCtrl1(document.form.LANHostConfig_DHCPServerConfigurable, 0);  
    	 //inputCtrl1(document.form.LANHostConfig_MinAddress, 0);
    	 //inputCtrl1(document.form.LANHostConfig_MaxAddress, 0);      	 
    	 
    	 //inputRCtrl1(document.form.x_DHCPClient, 0);     	   
    	 //inputCtrl1(document.form.IPConnection_ExternalIPAddress, 0); 
    	 //inputCtrl1(document.form.IPConnection_x_ExternalSubnetMask, 0);     
    	 //inputCtrl1(document.form.IPConnection_x_ExternalGateway, 0);
    	 //changeDHCPClient();  
    }
    else if (document.form.x_Mode.value == 'Router')
    {    	     	 
    	 //inputCtrl1(document.form.x_Connection, 0); 
    	 //inputCtrl1(document.form.PPPConnection_UserName, 0); 
    	 //inputCtrl1(document.form.PPPConnection_Password, 0); 
    	       	    	     	 
    	 //inputRCtrl1(document.form.x_DHCPClient, 0);     	   
    	 inputRCtrl2(document.form.x_DHCPClient, 1);
    	 inputRCtrl1(document.form.x_DHCPClient, 0);    
    	 inputCtrl1(document.form.IPConnection_ExternalIPAddress, 1); 
    	 inputCtrl1(document.form.IPConnection_x_ExternalSubnetMask, 1);     
    	 inputCtrl1(document.form.IPConnection_x_ExternalGateway, 1);
    	     
    	 inputRCtrl1(document.form.IPConnection_x_DNSServerEnable, 0);    	 
    	 inputCtrl1(document.form.IPConnection_x_DNSServer1, 0);  
    	 inputCtrl1(document.form.IPConnection_x_DNSServer2, 0);          	     
    	         	     
    	 changeDHCPClient();    
    	 //changeDNSServer();	    
    	     	 
    	 //inputCtrl1(document.form.LANHostConfig_IPRouters, 1);  	    	    	 
    	 //inputCtrl1(document.form.LANHostConfig_SubnetMask, 1);  	
    	 //inputRCtrl1(document.form.LANHostConfig_DHCPServerConfigurable, 1);  
    	 //inputCtrl1(document.form.LANHostConfig_MinAddress, 1);
    	 //inputCtrl1(document.form.LANHostConfig_MaxAddress, 1);        	     	     	 
    	 
    	 //changeDHCPServer();
    }                        
}

function changeWANType()
{
         if (document.form.x_WANType[0].checked == true)
    	 {    	     	     	         	         	        	
    	     inputCtrl1(document.form.PPPConnection_UserName, 0); 
    	     inputCtrl1(document.form.PPPConnection_Password, 0);     	   
    	         	         	   
    	     inputRCtrl1(document.form.x_DHCPClient, 1);
    	     inputRCtrl2(document.form.x_DHCPClient, 0);
    	     inputCtrl1(document.form.IPConnection_ExternalIPAddress, 1); 
    	     inputCtrl1(document.form.IPConnection_x_ExternalSubnetMask, 1);     
    	     inputCtrl1(document.form.IPConnection_x_ExternalGateway, 1);
    	     
    	     inputRCtrl1(document.form.IPConnection_x_DNSServerEnable, 1);
    	     inputRCtrl2(document.form.IPConnection_x_DNSServerEnable, 0);
    	     inputCtrl1(document.form.IPConnection_x_DNSServer1, 1);  
    	     inputCtrl1(document.form.IPConnection_x_DNSServer2, 1);
    	         	     
    	     changeDHCPClient();    
    	     changeDNSServer();	    	    	    	
    	 }
    	 else 
    	 {
    	 	
    	     // require account 	
    	     if (document.form.x_WANType[1].checked == true || document.form.x_WANType[2].checked == true)
    	     {    	      	     	     	      	         	     	
    	        inputCtrl1(document.form.PPPConnection_UserName, 1); 
    	        inputCtrl1(document.form.PPPConnection_Password, 1);     	         	   
    	     }   
    	     else
    	     {
    	     	inputCtrl1(document.form.PPPConnection_UserName, 0); 
    	        inputCtrl1(document.form.PPPConnection_Password, 0);     	         	   
    	     }
    	     
    	     // require IP
    	     if (document.form.x_WANType[0].checked == true || document.form.x_WANType[1].checked == true)    	      
    	     {
    	     	inputRCtrl1(document.form.x_DHCPClient, 1);
    	     	inputRCtrl2(document.form.x_DHCPClient, 0);
    	     	inputCtrl1(document.form.IPConnection_ExternalIPAddress, 1); 
    	     	inputCtrl1(document.form.IPConnection_x_ExternalSubnetMask, 1);     
    	     	inputCtrl1(document.form.IPConnection_x_ExternalGateway, 1);
    	     
    	     	inputRCtrl1(document.form.IPConnection_x_DNSServerEnable, 1);    	
    	     	inputRCtrl2(document.form.IPConnection_x_DNSServerEnable, 0);     	
    	     	inputCtrl1(document.form.IPConnection_x_DNSServer1, 1);  
    	     	inputCtrl1(document.form.IPConnection_x_DNSServer2, 1); 
    	     }	
    	     else if(document.form.x_WANType[2].checked == true) // PPTP 	    	     
    	     {
    	     	inputRCtrl1(document.form.x_DHCPClient, 1);
    	     	inputRCtrl2(document.form.x_DHCPClient, 1);
    	     	inputCtrl1(document.form.IPConnection_ExternalIPAddress, 0); 
    	     	inputCtrl1(document.form.IPConnection_x_ExternalSubnetMask, 0);     
    	     	inputCtrl1(document.form.IPConnection_x_ExternalGateway, 0);
    	     
    	     	inputRCtrl1(document.form.IPConnection_x_DNSServerEnable, 1);
    	     	inputRCtrl2(document.form.IPConnection_x_DNSServerEnable, 1);
    	     	inputCtrl1(document.form.IPConnection_x_DNSServer1, 1);  
    	     	inputCtrl1(document.form.IPConnection_x_DNSServer2, 1); 
    	     }	
    	     else // static IP
    	     {    	     	
    	     	inputRCtrl1(document.form.x_DHCPClient, 1);
    	     	inputRCtrl2(document.form.x_DHCPClient, 1);
    	     	inputCtrl1(document.form.IPConnection_ExternalIPAddress, 0); 
    	     	inputCtrl1(document.form.IPConnection_x_ExternalSubnetMask, 0);     
    	     	inputCtrl1(document.form.IPConnection_x_ExternalGateway, 0);
    	     
    	     	inputRCtrl1(document.form.IPConnection_x_DNSServerEnable, 1);
    	     	inputRCtrl2(document.form.IPConnection_x_DNSServerEnable, 1);
    	     	inputCtrl1(document.form.IPConnection_x_DNSServer1, 1);  
    	     	inputCtrl1(document.form.IPConnection_x_DNSServer2, 1); 
    	     }
    	     changeDHCPClient();
    	     changeDNSServer();
    	 }	    	
}

function changeDHCPClient()
{        
    if (document.form.x_DHCPClient[0].checked == true)
    {
    	inputCtrl1(document.form.IPConnection_ExternalIPAddress, 0); 
    	inputCtrl1(document.form.IPConnection_x_ExternalSubnetMask, 0);     
    	inputCtrl1(document.form.IPConnection_x_ExternalGateway, 0);
    }	
    else   
    {    	    	
    	inputCtrl1(document.form.IPConnection_ExternalIPAddress, 1); 
    	inputCtrl1(document.form.IPConnection_x_ExternalSubnetMask, 1);     
    	inputCtrl1(document.form.IPConnection_x_ExternalGateway, 1);
    }	   
}

function changeADSLType()
{        
    if (document.form.x_WANType[0].checked == true || document.form.x_WANType[3].checked == true)
    {
    	inputCtrl1(document.form.PPPConnection_UserName, 0); 
    	inputCtrl1(document.form.PPPConnection_Password, 0);     
    }	
    else   
    {    	    	
    	inputCtrl1(document.form.PPPConnection_UserName, 1); 
    	inputCtrl1(document.form.PPPConnection_Password, 1);         	
    }	   
}

function changeDNSServer()
{                       
    if (document.form.IPConnection_x_DNSServerEnable[0].checked == true)
    {   
    	inputCtrl1(document.form.IPConnection_x_DNSServer1, 0);  
    	inputCtrl1(document.form.IPConnection_x_DNSServer2, 0);    
    }	
    else   
    {
    	
    	inputCtrl1(document.form.IPConnection_x_DNSServer1, 1);  
    	inputCtrl1(document.form.IPConnection_x_DNSServer2, 1);    
    }	
}

function changeDHCPServer()
{    
           
    //if (document.form.LANHostConfig_DHCPServerConfigurable[0].checked == true)
    //{   
    //	inputCtrl1(document.form.LANHostConfig_MinAddress, 1);
    //    inputCtrl1(document.form.LANHostConfig_MaxAddress, 1);     
    //}	
    //else   
    //{
    //	inputCtrl1(document.form.LANHostConfig_MinAddress, 0);
    //    inputCtrl1(document.form.LANHostConfig_MaxAddress, 0);     
    //}       	       	
}

function saveMode(o)
{        
    //changeMode();
    	
    document.form.next_page.value = 'Main_Operation.asp';
    
    if (window.top.isModel() == "WL300")
    {	
    if (document.form.x_Mode[0].checked == true) //Access Point
    {           	                
        document.form.IPConnection_NATEnabled.value = '0';
        document.form.Layer3Forwarding_x_ConnectionType.value = 'Static IP';
        document.form.IPConnection_ConnectionType.value = 'IP_Bridged';
    }           
    else if (document.form.x_Mode[1].checked == true) //Home Gateway
    {             
        document.form.IPConnection_NATEnabled.value = '1';
        document.form.IPConnection_ConnectionType.value = 'IP_Routed';       
        document.form.Layer3Forwarding_x_ConnectionType.value = 'PPPoE';
    }
    }
    else
    {	   
    if (document.form.x_Mode[1].checked == true) //Access Point
    {           	                
        document.form.IPConnection_NATEnabled.value = '0';
        document.form.Layer3Forwarding_x_ConnectionType.value = 'Static IP';
        document.form.IPConnection_ConnectionType.value = 'IP_Bridged';
    }       
    //else if (document.form.x_Mode[1].checked == true) //Router
    //{              
    //    document.form.IPConnection_NATEnabled.value = '0';
    //    document.form.Layer3Forwarding_x_ConnectionType.value = 'Static IP';
    //    document.form.IPConnection_ConnectionType.value = 'IP_Routed';
    //}   
    else if (document.form.x_Mode[0].checked == true) //Home Gateway
    {             
        document.form.IPConnection_NATEnabled.value = '1';
        document.form.IPConnection_ConnectionType.value = 'IP_Routed';       
        document.form.Layer3Forwarding_x_ConnectionType.value = 'PPPoE';
    }
    }
    o.value = "  Save  ";	
    document.form.action_mode.value = "  Save  ";
    document.form.submit();                       	
}

function toTag(s)
{			
	if (s == 'Mode')
	{	  	   
	    if (document.form.x_Mode.value == 'Access Point')
	    {	      
	       t = 'Wireless';	       
	    }   
	    else if (document.form.x_Mode.value == 'Router')
	    {
	       t = 'WANSetting'; 	       
	    }   
	    else
	    {
	       t = 'Broadband';	      
	    }   	       	       
	}   
	else if (s == 'Country')
	{	  	   
	    if (window.top.isMode() == 'Access Point')
	    {	      
	       t = 'Wireless';	       
	    }   
	    else if (window.top.isMode() == 'Router')
	    {
	       t = 'WANSetting'; 	       
	    }   
	    else
	    {
	       t = 'Broadband';	      
	    }   	       	       
	}   
	else if (s=='Broadband')
	{	    	   
	    if (document.form.x_WANType[1].checked == true || document.form.x_WANType[2].checked == true)
	    {
	       //inputRCtrl2(document.form.x_ADSLType, 1);
	       changeADSLType();
	       t = 'PPPoE';
	    }	
	    else if (document.form.x_WANType[0].checked == true)
	    {	
	       t = 'MacHost';
	    }	
	    else
	    {
	       t = 'WANSetting';	       
	    }
        }
        else if (s=='PPPoE')
	{	
	    if (document.form.x_WANType[1].checked == true || document.form.x_WANType[2].checked == true)
	    {
	       if (document.form.PPPConnection_UserName.value == "" || document.form.PPPConnection_Password.value == "")	   
	       {
	    	   alert("You can't leave these two fields blank, if you choose to connect to an ADSL modem.");
	    	   return;
	       }	      
	       // if (document.form.x_WANType[1].checked == true)
	       //{
	       //		inputRCtrl2(document.form.x_DHCPClient, 0);
	       //}
	       //else if(document.form.x_WANType[2].checked == true)		
	       //{
	       //		inputRCtrl2(document.form.x_DHCPClient, 1);
	       //}	      
	       //changeDHCPClient();
	       //t = 'Wireless';
	       if (document.form.x_WANType[2].checked == true)
	       {
	          t = 'WANSetting';       
	       }   
	       else
	       {
	          t = 'Wireless';
	       }   	       
	    }   	
	    else
	    {
	       inputRCtrl2(document.form.x_DHCPClient, 1);
	       changeDHCPClient();	
	       t = 'WANSetting';	       
	    }
	   
        }
        else if (s=='PPPoE_IP')
	{		    	    	   
	    t = 'WANSetting';	    
        }
        else if (s=='MacHost')
	{		    	    	   
	    t = 'WANSetting';	    
        }
        else if (s=='WANSetting')
	{
	    if ((document.form.x_DHCPClient[1].checked == true) && (document.form.IPConnection_ExternalIPAddress.value == "" || document.form.IPConnection_x_ExternalSubnetMask.value == ""))	   
	    {
	        //if (document.form.x_Mode.value == 'Router')
	        //{
	           alert("You should assign a static IP address to system!");
	           return;
		//}
		//else
		//{
	    	//   alert("As you leave IP or subnet mask fields blank, connection type will be set as Automatic IP!");
  	        //   inputRCtrl2(document.form.x_DHCPClient, 0); 
  	        //}   
	    }		    
	    	    	    
	    if (window.top.isMode() == 'Gateway') 
	    {	    	
	    	t='Wireless';	    	
	    }	
	    else t = 'Wireless';
        }
        //else if (s=='Firewall')
        //{
        //    if (document.form.FirewallConfig_WanLanFirewallEnable.value == "1" &&
	//    		(document.form.FirewallConfig_LanWanRuleCount.value != "0" ||
	//    		 document.form.FirewallConfig_LanWanDefaultAct.value == "DROP"))
	//    {
	//    	if (confirm('Internet Access setting has been modified before, would you like to overwrite your previous setting?'))
       	//    	{       		    
       	//	    document.form.FirewallConfig_LanWanActiveSVCFlag.value = "000000";
	//    	}
	//    	else
	//    	{
	//    	    document.form.FirewallConfig_LanWanActiveSVCFlag.value = "";
	//    	}	
	//    }	  
	//    else
	//    {
	//    	document.form.FirewallConfig_LanWanActiveSVCFlag.value = "000000";
	//    }  		
        //    t = 'Wireless';
        //}    
        else if (s=='LANSetting')
	{
            if (document.form.x_Mode.value == 'Access Point')
	       t = 'Wireless';
	    else  
	    {
	       checkSubnet();	
	       t = 'DHCPSetting';
	    }   
        }       
        else if (s=='DHCPSetting')
        {
            t = 'Wireless';	
        }
        else
        {
            t = 'Wireless';
        }      
        
        for(i=0; i< prevTagLabel.length; i++)
        {
           if (prevTagLabel[i][0] == t)
           {
              prevTagLabel[i][1]=s;
              break;
           }   
        } 
        location.href = document.form.current_page.value + '#' + t;
}

function toPrevTag(s)
{
	prevTag = "";
	
	for(i=0; i< prevTagLabel.length; i++)
        {
           if (prevTagLabel[i][0] == s)
           {
              prevTag=prevTagLabel[i][1];
              break;
           }   
        }  
	
	if ( prevTag != "")
	{
	    location.href = document.form.current_page.value + '#' + prevTag;	    
	}   
	else
	{ 
	    location.href = document.form.current_page.value;
	}    
}

function matchSubnet(ip1, ip2, count)
{
	c = 0;
	v1 = 0;
	v2 = 0;
	
	
	for(i=0;i<ip1.length;i++)
	{	  
	   if (ip1.charAt(i) == '.')
	   {
	      c++;	
	      
	      if (v1!=v2) return false;
	      v1 = 0;
	      v2 = 0;	      
	   }   
	   else
	   {
	      if (ip2.charAt(i)=='.') return false;
	      	
	      v1 = v1*10 + (ip1.charAt(i) - '0');
	      v2 = v2*10 + (ip2.charAt(i) - '0');	      
	   }      
	   if (c==count) return true;	         
	}
	return false;
}

function subnetPrefix(ip, orig, count)
{
	r='';
	c=0;
	
	for(i=0;i<ip.length;i++)
	{
	   if (ip.charAt(i) == '.')
	      c++;
	      
	   if (c==count) break;
	   
	   r = r + ip.charAt(i);
	}
	
	c=0;
	
	for(i=0;i<orig.length;i++)
	{
	   if (orig.charAt(i) == '.')	
	   {
	      c++;	      	
	   }   
	      
	   if (c>=count)	
	     r = r + orig.charAt(i);
	}
	return (r);
}

function checkSubnet()
{
        /* Rule : If addresses in pool are match to subnet, don't change */
        /* Rule : If addresses in pool are not match to subnet, change to subnet.2~subnet.254 */
        if (!matchSubnet(document.form.LANHostConfig_IPRouters.value, document.form.LANHostConfig_MinAddress.value, 3) ||
            !matchSubnet(document.form.LANHostConfig_IPRouters.value, document.form.LANHostConfig_MaxAddress.value, 3))
        {           	         	
            document.form.LANHostConfig_MinAddress.value = subnetPrefix(document.form.LANHostConfig_IPRouters.value, document.form.LANHostConfig_MinAddress.value, 3);                  					
            document.form.LANHostConfig_MaxAddress.value = subnetPrefix(document.form.LANHostConfig_IPRouters.value, document.form.LANHostConfig_MaxAddress.value, 3);                    							          						      
        }	
}

function changeCountry(o, s, v)
{
}

function changeFirewall(o, s)
{	  	
  if (s == "WLANConfig11a")
  {     	     	           
      if (document.form.WLANConfig11a_WEPType.value == 'None' || document.form.WLANConfig11a_WEPType.value == 'FromServer') 
      {      	       
      	 inputRCtrl2(document.form.WLANConfig11a_x_AESEnable, 1);
         flag = 0;         
         wep = "";
      }  
      else 
      {
      	 flag = 1;
      	 
      	 if (document.form.WLANConfig11a_WEPType.value == "64bits")
      	    wep = "0000000000";
      	 else  if (document.form.WLANConfig11a_WEPType.value == "128bits")
      	    wep = "00000000000000000000000000";
      	 else
      	    wep = "00000000000000000000000000000000";      	  
      }
      
      document.form.WLANConfig11a_x_Phrase.value = '';
      document.form.WLANConfig11a_WEPKey1.value = wep;
      document.form.WLANConfig11a_WEPKey2.value = wep;
      document.form.WLANConfig11a_WEPKey3.value = wep;
      document.form.WLANConfig11a_WEPKey4.value = wep;     
                              
      inputCtrl(document.form.WLANConfig11a_x_Phrase, flag); 
      inputCtrl(document.form.WLANConfig11a_WEPKey1, flag);        
      inputCtrl(document.form.WLANConfig11a_WEPKey2, flag);  
      inputCtrl(document.form.WLANConfig11a_WEPKey3, flag);     
      inputCtrl(document.form.WLANConfig11a_WEPKey4, flag);
      inputCtrl(document.form.WLANConfig11a_WEPDefaultKey,  flag);   
  }	
  else if (s == "WLANConfig11b")
  {                    
      if (document.form.WLANConfig11b_WEPType.value == 'None' || document.form.WLANConfig11b_WEPType.value == 'FromServer') 
      {
         flag = 0;
         wep = "";
      }  
      else 
      {
         flag = 1;
        
         if (document.form.WLANConfig11b_WEPType.value == "64bits")
      	    wep = "0000000000";
      	 else  if (document.form.WLANConfig11b_WEPType.value == "128bits")
      	    wep = "00000000000000000000000000";
      	 else
      	    wep = "00000000000000000000000000000000";
      }  
      
      document.form.WLANConfig11b_x_Phrase.value = '';
      document.form.WLANConfig11b_WEPKey1.value = wep;
      document.form.WLANConfig11b_WEPKey2.value = wep;
      document.form.WLANConfig11b_WEPKey3.value = wep;
      document.form.WLANConfig11b_WEPKey4.value = wep;    
        
      inputCtrl(document.form.WLANConfig11b_x_Phrase,  flag); 
      inputCtrl(document.form.WLANConfig11b_WEPKey1,  flag);        
      inputCtrl(document.form.WLANConfig11b_WEPKey2,  flag);  
      inputCtrl(document.form.WLANConfig11b_WEPKey3,  flag);     
      inputCtrl(document.form.WLANConfig11b_WEPKey4,  flag); 
      inputCtrl(document.form.WLANConfig11b_WEPDefaultKey,  flag);    	     		
  }	
  else
  {
 	return false;
  } 	
  
  return true;   	
}

function change_internet_access()
{
    if (!document.form.FirewallConfig_LanWanActiveSVC_0.checked &&
        !document.form.FirewallConfig_LanWanActiveSVC_1.checked &&
        !document.form.FirewallConfig_LanWanActiveSVC_2.checked &&
        !document.form.FirewallConfig_LanWanActiveSVC_3.checked &&
        !document.form.FirewallConfig_LanWanActiveSVC_4.checked &&
        !document.form.FirewallConfig_LanWanActiveSVC_5.checked)	      	    
    {
    	document.form.InternetAccess_img.src = "graph/internet_none.gif";   
    }	
    else if (document.form.FirewallConfig_LanWanActiveSVC_0.checked &&
        document.form.FirewallConfig_LanWanActiveSVC_1.checked &&
        document.form.FirewallConfig_LanWanActiveSVC_2.checked &&
        document.form.FirewallConfig_LanWanActiveSVC_3.checked &&
        document.form.FirewallConfig_LanWanActiveSVC_4.checked &&
        document.form.FirewallConfig_LanWanActiveSVC_5.checked &&
        document.form.FirewallConfig_LanWanActiveDate_Sun.checked &&
        document.form.FirewallConfig_LanWanActiveDate_Mon.checked &&
        document.form.FirewallConfig_LanWanActiveDate_Tue.checked &&
        document.form.FirewallConfig_LanWanActiveDate_Wed.checked &&
        document.form.FirewallConfig_LanWanActiveDate_Thu.checked &&
        document.form.FirewallConfig_LanWanActiveDate_Fri.checked &&
        document.form.FirewallConfig_LanWanActiveDate_Sun.checked &&
        document.form.FirewallConfig_LanWanActiveTime_starthour.value == "00" &&
        document.form.FirewallConfig_LanWanActiveTime_startmin.value == "00" &&
        document.form.FirewallConfig_LanWanActiveTime_endhour.value == "23" &&
        document.form.FirewallConfig_LanWanActiveTime_endmin.value == "59"        
        )	      	    
    {
    	document.form.InternetAccess_img.src = "graph/internet_all.gif";   
    }
    else	
    {
    	document.form.InternetAccess_img.src = "graph/internet_some.gif";
    }
}

