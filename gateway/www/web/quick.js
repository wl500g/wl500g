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
	if (flag === 0)
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
	if (flag === 0)
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
	if (flag === 0)
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
	o[4].checked = false;    		    
	o[5].checked = false;    		    
	o[flag].checked = true;   
}   

function change_security(o, s, l)
{    	
	if (o.value == "0")
	{
		pflag = 0;
		wflag = 0;

		authTypeValue = "open"; 	// Open or Shared
		wepTypeValue = "0";	// None
		cryptoValue = "tkip";
	}
	else if (o.value == "1")
	{
		pflag = 1;
		wflag = 1;

		authTypeValue = "open";	// Shared
		wepTypeValue = "1";     // WEP-128 bits
		cryptoValue = "tkip";
	}
	else if (o.value == "2")
	{
		pflag = 1;
		wflag = 1;

		authTypeValue = "open";	// Shared
		wepTypeValue = "2";     // WEP-128 bits
		cryptoValue = "tkip";
	}
	else if (o.value == "3")    
	{
		pflag = 1;
		wflag = 0;

		authTypeValue = "psk";	// WPA-PSK only    	
		wepTypeValue= "0";	// TKIP
		cryptoValue = "tkip";
	}	   
	else if (o.value == "4")	
	{
		pflag = 0;
		wflag = 0;
		authTypeValue = "";
		wepTypeValue = "";
		cryptoValue = "";
	}

	if (o.options.length == 5 && o.value != "4")
	{
		if (confirm("You have chosen an additional wireless authentication and encryption option, do you really like to overwrite it?"))
		{
			o.options[4].value = null;
			o.options[4] = null;

			document.form.wl_auth_mode.value = authTypeValue;
			document.form.wl_crypto.value = cryptoValue;    	     
			document.form.wl_wep_x.value.value = wepTypeValue;
		}   
		else
		{
			pflag = 0;
			wflag = 0;
			o.value = "4"; 
			authTypeValue="";
			cryptoValue="";
			wepTypeValue="";
		}
	}


	if (authTypeValue!=="")
	{    	
		document.form.wl_auth_mode.value = authTypeValue;
		document.form.wl_crypto.value = cryptoValue;	     
		document.form.wl_wep_x.value = wepTypeValue;
	}


	if (l===0)
	{
		document.form.wl_wpa_psk.value = "";

		if (o.value == "1")
		{
			wep = "0000000000";
			is_wlphrase("WLANConfig11b", 'wl_phrase_x', document.form.wl_wpa_psk);
		}	
		else if (o.value == "2")
		{
			wep = "00000000000000000000000000";
			is_wlphrase("WLANConfig11b", 'wl_phrase_x', document.form.wl_wpa_psk);
		}	
		else
		{ 
			wep = "";                    
			document.form.wl_key1.value = wep;
			document.form.wl_key2.value = wep;
			document.form.wl_key3.value = wep;
			document.form.wl_key4.value = wep;
		}		       
	}  

	inputCtrl(document.form.wl_wpa_psk, pflag);
	inputCtrl(document.form.wl_key1, wflag);
	inputCtrl(document.form.wl_key2, wflag);
	inputCtrl(document.form.wl_key3, wflag);
	inputCtrl(document.form.wl_key4, wflag);
	inputCtrl(document.form.wl_key,  wflag);

	if (pflag == "1" && l === 0)
	{    	     	  
		document.form.wl_wpa_psk.focus();
	}
}

function loadSecurity()
{	
	authType = 	document.form.wl_auth_mode.value;
	cryptoType = document.form.wl_crypto.value;
	wepType = document.form.wl_wep_x.value;

	if (authType == "open" && wepType == "0")
	{
		document.form.SecurityLevel.value = "0";
	}	
	else if (authType == "open" && wepType == "1")
	{
		document.form.SecurityLevel.value = "1";
	}
	else if (authType == "open" && wepType == "2")
	{
		document.form.SecurityLevel.value = "2";
	}
	else if (authType == "psk" && cryptoType == "tkip")
	{
		document.form.SecurityLevel.value = "3";
	}
	else 
	{
		add_option_x(document.form.SecurityLevel, 4, "Extra", 1);
		document.form.SecurityLevel.value = "4";
	}

	change_security(document.form.SecurityLevel, "WLANConfig11b", 1);
}

function loadQuick()
{	    	    	
	/* Transfer NATEnabled, wan_proto to x_Mode, x_Connection Rule */
	//alert(document.form.wan_proto.value);              
	document.form.next_host.value = location.host;

	//alert(document.form.wan_dnsenable_x[0].checked);
	//alert(document.form.wan_dnsenable_x[1].checked);                    	         	

	if (document.form.wan_route_x.value == 'IP_Routed')
	{    	
		if (document.form.wan_nat_x.value == '1')
			document.form.x_Mode.value = 'Home Gateway';
		else document.form.x_Mode.value = 'Router';

		if (document.form.wan_proto.value == 'pppoe')
		{    	    
			inputCCtrl4(document.form.x_WANType, 1);    	 
			inputRCtrl2(document.form.x_DHCPClient, 0);
		}   
		else if (document.form.wan_proto.value == 'pptp')
		{    	   
			inputCCtrl4(document.form.x_WANType, 2);    	        	 
			inputRCtrl2(document.form.x_DHCPClient, 1);
		}   
		else if (document.form.wan_proto.value == 'l2tp')
		{    	   
			inputCCtrl4(document.form.x_WANType, 5);    	        	 
			inputRCtrl2(document.form.x_DHCPClient, 1);
		}   
		else   
		{
			if (document.form.wan_proto.value == 'static')
			{
				inputCCtrl4(document.form.x_WANType, 3);    		
				inputRCtrl2(document.form.x_DHCPClient, 1);
			}
			else 
			{   
				if (document.form.wan_proto.value == 'bigpond')     	    	
					inputCCtrl4(document.form.x_WANType, 4);
				else inputCCtrl4(document.form.x_WANType, 0);
				inputRCtrl2(document.form.x_DHCPClient, 0);
			}   	    	    
		}       	       	     	    	
	}    
	else
	{
		document.form.x_Mode.value = 'Access Point';    
	} 

	masq_wepkey();            
	loadSecurity();
	sameWireless=true;                                 	                                     
	TimeZoneList();
	changeMode();    
}

function loadMode()
{	    	
	/* Transfer NATEnabled, wan_proto to x_Mode, x_Connection Rule */
	//alert(document.form.wan_proto.value);
	document.form.next_host.value = location.host;

	//alert(document.form.wan_dnsenable_x[0].checked);
	//alert(document.form.wan_dnsenable_x[1].checked);
	if (window.top.isModel() == "WL300")
	{
		if (document.form.wan_nat_x.value == '1')
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
		if (document.form.wan_nat_x.value == '1')
		{
			document.form.x_Mode[0].checked = true;
			document.form.x_Mode[1].checked = false;
			document.form.x_Mode[2].checked = false;
		}
		else if (document.form.wan_route_x.value == 'IP_Routed')
		{
			document.form.x_Mode[0].checked = false;
			document.form.x_Mode[1].checked = true;
			document.form.x_Mode[2].checked = false;
		}
		else
		{
			document.form.x_Mode[0].checked = false;
			document.form.x_Mode[1].checked = false;
			document.form.x_Mode[2].checked = true;  
		}
	}
	//changeMode();    
}

function setSVCCheck(d1, d2, d3, d4, d5, d6)
{
	str = "";

	if (d6 === true ) str = "1" + str;
	else str = "0" + str;
	if (d5 === true ) str = "1" + str;
	else str = "0" + str;
	if (d4 === true ) str = "1" + str;
	else str = "0" + str;
	if (d3 === true ) str = "1" + str;
	else str = "0" + str;
	if (d2 === true ) str = "1" + str;
	else str = "0" + str;
	if (d1 === true ) str = "1" + str;
	else str = "0" + str;   

	return str;
}

function saveQuick(o)
{

	/* Copy Wireless 11b to Wireless 11a */   
	window.top.pageChanged = 0;
	window.top.pageChangedCount = 0;

	inputCtrl(document.form.wl_wpa_psk, 1);
	inputCtrl(document.form.wl_key1, 1);
	inputCtrl(document.form.wl_key2, 1);  
	inputCtrl(document.form.wl_key3, 1);     
	inputCtrl(document.form.wl_key4, 1); 
	inputCtrl(document.form.wl_key, 1);               	           	   
	unmasq_wepkey();
	//document.form.next_page.value = 'Main_Content.asp';

	if (document.form.x_Mode.value == 'Access Point')
	{           	                
		document.form.wan_nat_x.value = '0';
		document.form.wan_proto.value = 'static';
		document.form.wan_route_x.value = 'IP_Bridged';

	}             
	else
	{             
		if (document.form.x_Mode.value == 'Router')
		{              
			document.form.wan_nat_x.value = '0';
			document.form.wan_route_x.value = 'IP_Routed';
		}   
		else
		{             
			document.form.wan_nat_x.value = '1';
			document.form.wan_route_x.value = 'IP_Routed';
		}

		if (document.form.x_WANType[1].checked === true)
		{
			document.form.wan_proto.value = 'pppoe';
		}   
		else if (document.form.x_WANType[2].checked === true)
		{
			document.form.wan_proto.value = 'pptp';
		}   
		else if (document.form.x_WANType[3].checked === true)
		{                     	
			document.form.wan_proto.value = 'static';    
		} 
		else if (document.form.x_WANType[4].checked === true)
		{                     	
			document.form.wan_proto.value = 'bigpond';    
		}   
		else if (document.form.x_WANType[5].checked === true)
		{                     	
			document.form.wan_proto.value = 'l2tp';    
		}   
		else
		{        	
			document.form.wan_proto.value = 'dhcp';            
		} 

		document.form.SecurityLevel.disabled = true;
		document.form.x_DHCPClient.disabled = true;    	
		document.form.x_DHCPClient[0].checked = false;
		document.form.x_DHCPClient[1].checked = false;       	
		inputRCtrl1(document.form.wan_dnsenable_x, 1);
		inputCtrl1(document.form.wan_ipaddr, 1);
		inputCtrl1(document.form.wan_netmask, 1);
		inputCtrl1(document.form.wan_gateway, 1);    	         
	}             

	o.value = ' Save ';   
	document.form.action_mode.value = ' Save ';
	return true;
}

function changeMode()
{   	    		    	
	if (document.form.x_Mode.value == 'Home Gateway' || document.form.x_Mode.value == 'Router')
	{             	 	     	 
		if (document.form.x_WANType[0].checked === true)
		{    	     	     	         	         	        	
			inputCtrl1(document.form.wan_pppoe_username, 0); 
			inputCtrl1(document.form.wan_pppoe_passwd, 0);     	   

			inputRCtrl1(document.form.x_DHCPClient, 0);
			inputRCtrl2(document.form.x_DHCPClient, 0);
			inputCtrl1(document.form.wan_ipaddr, 1); 
			inputCtrl1(document.form.wan_netmask, 1);     
			inputCtrl1(document.form.wan_gateway, 1);

			//inputRCtrl1(document.form.wan_dnsenable_x, 1);
			//inputRCtrl2(document.form.wan_dnsenable_x, 0);
			//inputCtrl1(document.form.wan_dns1_x, 1);  
			//inputCtrl1(document.form.wan_dns2_x, 1);

			changeDHCPClient();    
			changeDNSServer();	    	    	    	
		}
		else 
		{

			// require account 	
			if (document.form.x_WANType[1].checked === true || document.form.x_WANType[2].checked === true || 
					document.form.x_WANType[4].checked === true || document.form.x_WANType[5].checked === true)
			{    	      	     	     	      	         	     	
				inputCtrl1(document.form.wan_pppoe_username, 1); 
				inputCtrl1(document.form.wan_pppoe_passwd, 1);     	         	   
			}   
			else
			{
				inputCtrl1(document.form.wan_pppoe_username, 0); 
				inputCtrl1(document.form.wan_pppoe_passwd, 0);     	         	   
			}

			// require IP
			if (document.form.x_WANType[0].checked === true || document.form.x_WANType[1].checked === true || 
					document.form.x_WANType[4].checked === true)
			{
				inputRCtrl1(document.form.x_DHCPClient, 0);
				inputRCtrl2(document.form.x_DHCPClient, 0);
				inputCtrl1(document.form.wan_ipaddr, 1); 
				inputCtrl1(document.form.wan_netmask, 1);     
				inputCtrl1(document.form.wan_gateway, 1);

				//inputRCtrl1(document.form.wan_dnsenable_x, 1);    	
				//inputRCtrl2(document.form.wan_dnsenable_x, 0);     	
				//inputCtrl1(document.form.wan_dns1_x, 1);  
				//inputCtrl1(document.form.wan_dns2_x, 1); 
			}	
			else if(document.form.x_WANType[2].checked === true || 
					document.form.x_WANType[5].checked === true) // pptp 	    	     
			{
				inputRCtrl1(document.form.x_DHCPClient, 1);
				inputRCtrl2(document.form.x_DHCPClient, 1);
				inputCtrl1(document.form.wan_ipaddr, 0); 
				inputCtrl1(document.form.wan_netmask, 0);     
				inputCtrl1(document.form.wan_gateway, 0);

				//inputRCtrl1(document.form.wan_dnsenable_x, 1);
				//inputRCtrl2(document.form.wan_dnsenable_x, 1);
				//inputCtrl1(document.form.wan_dns1_x, 1);
				//inputCtrl1(document.form.wan_dns2_x, 1);
				if (document.form.wan_ipaddr.value === "" || 
						document.form.wan_ipaddr.value == "0.0.0.0") 
				{
					inputRCtrl2(document.form.x_DHCPClient, 0);
				}
			}	
			else // static
			{    	     	
				inputRCtrl1(document.form.x_DHCPClient, 0);
				inputRCtrl2(document.form.x_DHCPClient, 1);
				inputCtrl1(document.form.wan_ipaddr, 0);
				inputCtrl1(document.form.wan_netmask, 0);
				inputCtrl1(document.form.wan_gateway, 0);

				//inputRCtrl1(document.form.wan_dnsenable_x, 1);
				//inputRCtrl2(document.form.wan_dnsenable_x, 1);
				//inputCtrl1(document.form.wan_dns1_x, 1);  
				//inputCtrl1(document.form.wan_dns2_x, 1); 
			}
			changeDHCPClient();
			changeDNSServer();
		}
	}
//	else if (document.form.x_Mode.value == 'Access Point')
//	{    	    	     	    	     
	//inputCtrl1(document.form.x_Connection, 0); 
	//inputCtrl1(document.form.wan_pppoe_username, 0); 
	//inputCtrl1(document.form.wan_pppoe_passwd, 0);     

	//inputRCtrl1(document.form.wan_dnsenable_x, 0);    	 
	//inputCtrl1(document.form.wan_dns1_x, 0);  
	//inputCtrl1(document.form.wan_dns2_x, 0);


	//inputCtrl1(document.form.lan_ipaddr, 1);
	//inputCtrl1(document.form.LANHostConfig_SubnetMask, 1);  	
	//inputRCtrl1(document.form.LANHostConfig_DHCPServerConfigurable, 0);  
	//inputCtrl1(document.form.dhcp_start, 0);
	//inputCtrl1(document.form.dhcp_end, 0);      	 

	//inputRCtrl1(document.form.x_DHCPClient, 0);     	   
	//inputCtrl1(document.form.wan_ipaddr, 0); 
	//inputCtrl1(document.form.wan_netmask, 0);     
	//inputCtrl1(document.form.wan_gateway, 0);
	//changeDHCPClient();  
//	}
	else if (document.form.x_Mode.value == 'Router')
	{    	     	 
		//inputCtrl1(document.form.x_Connection, 0); 
		//inputCtrl1(document.form.wan_pppoe_username, 0); 
		//inputCtrl1(document.form.wan_pppoe_passwd, 0); 

		//inputRCtrl1(document.form.x_DHCPClient, 0);     	   
		inputRCtrl2(document.form.x_DHCPClient, 1);
		inputRCtrl1(document.form.x_DHCPClient, 0);    
		inputCtrl1(document.form.wan_ipaddr, 1); 
		inputCtrl1(document.form.wan_netmask, 1);     
		inputCtrl1(document.form.wan_gateway, 1);

		inputRCtrl1(document.form.wan_dnsenable_x, 0);    	 
		inputCtrl1(document.form.wan_dns1_x, 0);  
		inputCtrl1(document.form.wan_dns2_x, 0);          	     

		changeDHCPClient();    
		//changeDNSServer();	    

		//inputCtrl1(document.form.lan_ipaddr, 1);  	    	    	 
		//inputCtrl1(document.form.LANHostConfig_SubnetMask, 1);  	
		//inputRCtrl1(document.form.LANHostConfig_DHCPServerConfigurable, 1);  
		//inputCtrl1(document.form.dhcp_start, 1);
		//inputCtrl1(document.form.dhcp_end, 1);        	     	     	 

		//changeDHCPServer();
	}                        
}

function changeWANType()
{
	if (document.form.x_WANType[0].checked === true)
	{    	     	     	         	         	        	
		inputCtrl1(document.form.wan_pppoe_username, 0); 
		inputCtrl1(document.form.wan_pppoe_passwd, 0);     	   

		inputRCtrl1(document.form.x_DHCPClient, 0);
		inputRCtrl2(document.form.x_DHCPClient, 0);
		inputCtrl1(document.form.wan_ipaddr, 1); 
		inputCtrl1(document.form.wan_netmask, 1);     
		inputCtrl1(document.form.wan_gateway, 1);

		inputRCtrl1(document.form.wan_dnsenable_x, 1);
		inputRCtrl2(document.form.wan_dnsenable_x, 0);
		inputCtrl1(document.form.wan_dns1_x, 1);  
		inputCtrl1(document.form.wan_dns2_x, 1);
	}
	else 
	{

		// require account 	
		if (document.form.x_WANType[1].checked === true || document.form.x_WANType[2].checked === true ||
				document.form.x_WANType[4].checked === true || document.form.x_WANType[5].checked === true)
		{    	      	     	     	      	         	     	
			inputCtrl1(document.form.wan_pppoe_username, 1); 
			inputCtrl1(document.form.wan_pppoe_passwd, 1);     	         	   
		}   
		else
		{
			inputCtrl1(document.form.wan_pppoe_username, 0); 
			inputCtrl1(document.form.wan_pppoe_passwd, 0);     	         	   
		}

		// require IP
		if (document.form.x_WANType[0].checked === true || document.form.x_WANType[1].checked === true || 
				document.form.x_WANType[4].checked === true)
		{
			inputRCtrl1(document.form.x_DHCPClient, 0);
			inputRCtrl2(document.form.x_DHCPClient, 0);
			inputCtrl1(document.form.wan_ipaddr, 1); 
			inputCtrl1(document.form.wan_netmask, 1);     
			inputCtrl1(document.form.wan_gateway, 1);

			inputRCtrl1(document.form.wan_dnsenable_x, 1);    	
			inputRCtrl2(document.form.wan_dnsenable_x, 0);     	
			inputCtrl1(document.form.wan_dns1_x, 1);  
			inputCtrl1(document.form.wan_dns2_x, 1); 
		}	
		else if(document.form.x_WANType[2].checked === true || document.form.x_WANType[5].checked === true) // pptp 	    	     
		{
			inputRCtrl1(document.form.x_DHCPClient, 1);
			inputRCtrl2(document.form.x_DHCPClient, 1);
			inputCtrl1(document.form.wan_ipaddr, 0); 
			inputCtrl1(document.form.wan_netmask, 0);     
			inputCtrl1(document.form.wan_gateway, 0);

			inputRCtrl1(document.form.wan_dnsenable_x, 1);
			inputRCtrl2(document.form.wan_dnsenable_x, 0);
			inputCtrl1(document.form.wan_dns1_x, 1);  
			inputCtrl1(document.form.wan_dns2_x, 1); 
			if (document.form.wan_ipaddr.value === "" || 
					document.form.wan_ipaddr.value == "0.0.0.0") 
			{
				inputRCtrl2(document.form.x_DHCPClient, 0);
			}
		}	
		else // static
		{    	     	
			inputRCtrl1(document.form.x_DHCPClient, 0);
			inputRCtrl2(document.form.x_DHCPClient, 1);
			inputCtrl1(document.form.wan_ipaddr, 0); 
			inputCtrl1(document.form.wan_netmask, 0);     
			inputCtrl1(document.form.wan_gateway, 0);

			inputRCtrl1(document.form.wan_dnsenable_x, 0);
			inputRCtrl2(document.form.wan_dnsenable_x, 1);
			inputCtrl1(document.form.wan_dns1_x, 1);  
			inputCtrl1(document.form.wan_dns2_x, 1); 
		}
	}	    	

	changeDHCPClient();    
	changeDNSServer();	    	    	    	
}

function changeDHCPClient()
{        
	if (document.form.x_DHCPClient[0].checked === true)
	{
		inputCtrl1(document.form.wan_ipaddr, 0);
		inputCtrl1(document.form.wan_netmask, 0);
		inputCtrl1(document.form.wan_gateway, 0);
		v = document.form.wan_proto.value;
		if (v == "l2tp" || v == "pptp" || v == "pppoe" || v == "wimax") 
		{
			document.form.wan_ipaddr.value = "0.0.0.0";
			document.form.wan_netmask.value = "0.0.0.0";
			document.form.wan_gateway.value = "0.0.0.0";
		}
	}	
	else   
	{    	    	
		inputCtrl1(document.form.wan_ipaddr, 1); 
		inputCtrl1(document.form.wan_netmask, 1);     
		inputCtrl1(document.form.wan_gateway, 1);
		if (document.form.wan_ipaddr.value == "0.0.0.0") 
		{
			document.form.wan_ipaddr.value = "";
			document.form.wan_netmask.value = "";
			document.form.wan_gateway.value = "";
		}
	}	   
}

function changeADSLType()
{        
	if (document.form.x_WANType[0].checked === true || document.form.x_WANType[3].checked === true)
	{
		inputCtrl1(document.form.wan_pppoe_username, 0); 
		inputCtrl1(document.form.wan_pppoe_passwd, 0);     
	}	
	else   
	{    	    	
		inputCtrl1(document.form.wan_pppoe_username, 1); 
		inputCtrl1(document.form.wan_pppoe_passwd, 1);         	
	}	   
}

function changeDNSServer()
{                       
	if (document.form.wan_dnsenable_x[0].checked === true)
	{   
		inputCtrl1(document.form.wan_dns1_x, 0);  
		inputCtrl1(document.form.wan_dns2_x, 0);    
	}	
	else   
	{

		inputCtrl1(document.form.wan_dns1_x, 1);  
		inputCtrl1(document.form.wan_dns2_x, 1);    
	}	
}

function changeDHCPServer()
{    

	//if (document.form.LANHostConfig_DHCPServerConfigurable[0].checked == true)
	//{   
	//	inputCtrl1(document.form.dhcp_start, 1);
	//    inputCtrl1(document.form.dhcp_end, 1);     
	//}	
	//else   
	//{
	//	inputCtrl1(document.form.dhcp_start, 0);
	//    inputCtrl1(document.form.dhcp_end, 0);     
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
			document.form.wan_nat_x.value = '0';
			document.form.wan_proto.value = 'static';
			document.form.wan_route_x.value = 'IP_Bridged';
		}
		else if (document.form.x_Mode[1].checked == true) //Home Gateway
		{
			document.form.wan_nat_x.value = '1';
			document.form.wan_route_x.value = 'IP_Routed';
			document.form.wan_proto.value = 'pppoe';
		}
	}
	else
	{
		if (document.form.x_Mode[2].checked === true) //Access Point
		{
			document.form.wan_nat_x.value = '0';
			document.form.wan_proto.value = 'static';
			document.form.wan_route_x.value = 'IP_Bridged';
		}
		else if (document.form.x_Mode[1].checked === true) //Router
		{
			document.form.wan_nat_x.value = '0';
			document.form.wan_proto.value = 'static';
			document.form.wan_route_x.value = 'IP_Routed';
		}
		else if (document.form.x_Mode[0].checked === true) //Home Gateway
		{
			document.form.wan_nat_x.value = '1';
			document.form.wan_route_x.value = 'IP_Routed';
			document.form.wan_proto.value = 'pppoe';
		}
	}
	o.value = " Save ";	
	document.form.action_mode.value = " Save ";
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
		else
		{
			t = 'Broadband';	      
		}   	       	       
	}   
	else if (s=='Broadband')
	{	    	   
		if (document.form.x_WANType[1].checked === true || document.form.x_WANType[2].checked === true || 
				document.form.x_WANType[4].checked === true || document.form.x_WANType[5].checked === true)
		{
			//inputRCtrl2(document.form.x_ADSLType, 1);
			changeADSLType();
			t = 'PPPoE';
		}	
		else if (document.form.x_WANType[0].checked === true || document.form.x_WANType[4].checked === true)
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
		if (document.form.x_WANType[1].checked === true || document.form.x_WANType[2].checked === true || 
				document.form.x_WANType[4].checked === true || document.form.x_WANType[5].checked === true)
		{
			if (document.form.wan_pppoe_username.value === "" || document.form.wan_pppoe_passwd.value === "")	   
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
			if (document.form.x_WANType[4].checked === true)
			{
				t = 'MacHost';       
			}   
			else if (document.form.x_WANType[2].checked === true || document.form.x_WANType[5].checked === true)
			{
				t = 'MacHost';       
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
		if ((document.form.x_DHCPClient[1].checked === true) && 
				(document.form.wan_ipaddr.value === "" || document.form.wan_netmask.value === ""))	   
		{	        
			alert("You should assign a static IP address to system!");
			return;		
		}		    

		if (window.top.isMode() == 'Gateway') 
		{	    	
			t='Wireless';	    	
		}	
		else t = 'Wireless';
	}        
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

	if ( prevTag !== "")
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
	if (!matchSubnet(document.form.lan_ipaddr.value, document.form.dhcp_start.value, 3) ||
			!matchSubnet(document.form.lan_ipaddr.value, document.form.dhcp_end.value, 3))
	{           	         	
		document.form.dhcp_start.value = subnetPrefix(document.form.lan_ipaddr.value, document.form.dhcp_start.value, 3);                  					
		document.form.dhcp_end.value = subnetPrefix(document.form.lan_ipaddr.value, document.form.dhcp_end.value, 3);                    							          						      
	}	
}

function changeCountry(o, s, v)
{
}

function changeFirewall(o, s)
{	  	
	if (s == "WLANConfig11b")
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

		document.form.wl_wpa_psk.value = '';
		document.form.wl_key1.value = wep;
		document.form.wl_key2.value = wep;
		document.form.wl_key3.value = wep;
		document.form.wl_key4.value = wep;    

		inputCtrl(document.form.wl_wpa_psk,  flag); 
		inputCtrl(document.form.wl_key1,  flag);        
		inputCtrl(document.form.wl_key2,  flag);  
		inputCtrl(document.form.wl_key3,  flag);     
		inputCtrl(document.form.wl_key4,  flag); 
		inputCtrl(document.form.wl_key,  flag);    	     		
	}	
	else
	{
		return false;
	} 	

	return true;   	
}

function is_wlphrase_q(s, o) 
{   
	if (document.form.SecurityLevel.value=="1" ||
			document.form.SecurityLevel.value=="2") return(is_wlphrase(s, 'wl_phrase_x', o));
	else return(is_wlphrase(s, 'wl_wpa_psk', o));
}

function validate_wlphrase_q(s, o)
{   	      	   	   	
	if (document.form.SecurityLevel.value=="1" ||
			document.form.SecurityLevel.value=="2") return(validate_wlphrase(s, 'wl_phrase_x', o));
	else return(validate_wlphrase(s, 'wl_wpa_psk', o));
}
