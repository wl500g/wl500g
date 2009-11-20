/*general*/

var nav;
var change;
var keyPressed;
var wItem;
var ip = "";
var ip_addr = '<!--#echo var="REMOTE_ADDR"-->';
var chanList = 0;


if (navigator.appName == 'Netscape') 
{
   nav = true;		
   //window.captureEvents(event.KEYPRESS);
   //window.onkeydown = NetscapeEventHandler_KeyDown;
} 
else 
{ 
   nav = false;	
   document.onkeydown = MicrosoftEventHandler_KeyDown;
}

//if(document.all)
//{
//	var tags=document.all.tags("a")
//	for (var i=0;i<tags.length;i++)
//	tags(i).outerHTML=tags(i).outerHTML.replace(">"," hidefocus=true>")
//}

function NetscapeEventHandler_KeyDown(e) 
{	  	   	
  keyPressed = e.which;		
   
  if (keyPressed<48 && keyPressed!=16) 
  {
      keyPressed = 0;
  }   
  //if (e.which == 13 && e.target.type != 'textarea' && e.target.type != 'submit') 
  //{ 
  //	return false; 
  //}
  return true;
}

function IEKey()
{
   window.top.pageChanged = 1;	
   return (event.keyCode);	
}

function NSKey()
{
   window.top.pageChanged = 1;	 
   return(0);	
}

function MicrosoftEventHandler_KeyDown() 
{  
  /* if (keyPressed == 13  && event.srcElement.type != 'button' && event.srcElement.type != 'textarea' && event.srcElement.type != 'submit')
    return false;
  return true;*/

  //keyPressed = event.keyCode;
  
  return true;
}

function inputCtrl(o, flag)
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
 
function buttonOver(o) 
{
    o.style.backgroundColor = "#FFFFCC";
    o.style.cursor="hand";
}

function buttonOut(o) {
    o.style.backgroundColor = "#C0C0C0";
}

function checkPass(o, o1, o2)
{
   if (o1.value==o2.value)
   {
      o.value = "  Save  ";	
      document.form.action_mode.value = "  Save  ";
      return true;
   }   
   alert('Two password strings mismatched !!!'); 
   return false;
}   

function markGroup(o, s, c, b) {
    var bFlag, cFlag;
    	
    document.form.group_id.value = s;	
    
    bFlag = 0;
    cFlag = 0;
    
    if (b == " Add ")
    {
        // Check if inserted item is validated
        if (s=='WLANConfig11b_RBRList') // Wireless Bridge
    	{
    	    if (document.form.WLANConfig11b_RBRListCount_0.value >= c) cFlag=1;
    	    else if (!validate_hwaddr(document.form.WLANConfig11b_x_BRhwaddr_0)) return false;
            else if (document.form.WLANConfig11b_x_BRhwaddr_0.value=="") bFlag=1;            	    	    
        }   
        else if (s == 'DeviceSecurity11a_ACLList') // Access Control List 
        {
            if (document.form.DeviceSecurity11a_ACLListCount_0.value >= c) cFlag=1;
            else if (!validate_hwaddr(document.form.DeviceSecurity11a_MacAddr_0)) return false;
            else if (document.form.DeviceSecurity11a_MacAddr_0.value=="") bFlag=1;
        }
        else if (s=='LANHostConfig_ManualDHCPList')
        {
            if (document.form.LANHostConfig_ManualDHCPCount_0.value >= c) cFlag=1;
            else if (!validate_hwaddr(document.form.LANHostConfig_ManualMac_0) ||
            	     !validate_ipaddr(document.form.LANHostConfig_ManualIP_0, "")) return false;
            else if (document.form.LANHostConfig_ManualMac_0.value=="" ||
            	     document.form.LANHostConfig_ManualIP_0.value=="") bFlag=1; 	 
        }
        else if (s=='LANHostConfig_DmzManualDHCPList')
        {
            if (document.form.LANHostConfig_DmzManualDHCPCount_0.value >= c) cFlag=1;
            else if (!validate_hwaddr(document.form.LANHostConfig_DmzManualMac_0) ||
            	     !validate_ipaddr(document.form.LANHostConfig_DmzManualIP_0, "")) return false;
            else if (document.form.LANHostConfig_DmzManualMac_0.value=="" ||
            	     document.form.LANHostConfig_DmzManualIP_0.value=="") bFlag=1;
        }
        else if (s=='PPPConnection_PPPoERouteList')
        {
            if (document.form.PPPConnection_PPPoERouteCount_0.value > c) cFlag=1;
            else if (!validate_ipaddr(document.form.PPPConnection_x_PPPoEIP_0, "") ||
            	!validate_range(document.form.PPPConnection_x_PPPoEMask_0, 0, 32)) return false;
            else if (document.form.PPPConnection_x_PPPoEIP_0.value=="" ||
            	document.form.PPPConnection_x_PPPoEMask_0.value=="") bFlag=1;
        }
        //The following items is for 802.11a, or not used
        //else if (s == 'DeviceSecurity11a_AESList')
        //{
        //   if (document.form.DeviceSecurity11a_AESListCount_0.value >= c) cFlag=1;		           
        //    if (document.form.DeviceSecurity11a_AESMacAddr_0.value=="") bFlag=1;	
        //}   
        //else if (s=='WLANAuthentication11a_ExAuthDB')
        //{
        //    if (document.form.WLANAuthentication11a_ExAuthDBCount_0.value >= c) cFlag=1;	
        //    if (document.form.WLANAuthentication11a_ExAuthDBIPAddr_0.value=="") bFlag=1;	
        //} 
        //else if (s=='WLANAuthentication11a_LocalAuthDB')
        //{
        //    if (document.form.WLANAuthentication11a_AuthDBNumberOfEntries_0.value >= c) cFlag=1;		
        //    if (document.form.WLANAuthentication11a_LocalSecretType_0.value=="") bFlag=1;	
        //} 
        //else if (s=='LANHostConfig_DNSServerList')
        //{
        //    if (document.form.LANHostConfig_DNSServerListCount_0.value >=c ) cFlag=1; 		
        //    if (document.form.LANHostConfig_DNSServers_0.value=="") bFlag=1; 	 
        //} 
        //else if (s=='LANHostConfig_ReservedAddressList')
        //{
        //    if (document.form.LANHostConfig_ReservedAddressListCount_0.value >=c ) cFlag=1;
        //    if (document.form.LANHostConfig_ReservedAddresses_0.value=="") bFlag=1; 	 
        //} 
        else if (s=='RouterConfig_GWStatic')
        {
            if (document.form.RouterConfig_GWStaticCount_0.value > c) cFlag=1;	
            else if (!validate_ipaddr(document.form.RouterConfig_GWStaticIP_0, "") ||
            	!validate_ipaddr(document.form.RouterConfig_GWStaticMask_0, "") ||
            	!validate_ipaddr(document.form.RouterConfig_GWStaticGW_0, "")) return false;           
            else if (document.form.RouterConfig_GWStaticIP_0.value=="" ||
                document.form.RouterConfig_GWStaticMask_0.value=="" ||
                document.form.RouterConfig_GWStaticGW_0.value=="") bFlag=1;	
        } 
        else if (s=='IPConnection_VSList')
        {           
            if (document.form.IPConnection_VServerCount_0.value >= c) cFlag=1;
            else if (document.form.IPConnection_VServerIP_0.value=="") bFlag=1;
            else if (document.form.IPConnection_VServerProto_0.value == "OTHER")
            {
                if (!validate_ipaddr(document.form.IPConnection_VServerIP_0, "") ||
            	    !validate_portrange(document.form.IPConnection_VServerPort_0, "") ||
            	    !validate_range(document.form.IPConnection_VServerPNo_0, 0, 255)) return false;
            	else if (document.form.IPConnection_VServerPNo_0.value=="") bFlag=1;
            	document.form.IPConnection_VServerPort_0.value = "";
            }	 
            else
            { 
                if (!validate_ipaddr(document.form.IPConnection_VServerIP_0, "") ||
            	!validate_portrange(document.form.IPConnection_VServerPort_0, "")) return false;
                else if (document.form.IPConnection_VServerPort_0.value=="") bFlag=1;
                
            	document.form.IPConnection_VServerPNo_0.value = "";
            }	
        } 
        else if (s=='IPConnection_TriggerList')
        {           
            if (document.form.IPConnection_autofwCount_0.value >= c) cFlag=1;
            else if (!validate_portrange(document.form.IPConnection_autofwInPort_0, "") ||
            	!validate_portrange(document.form.IPConnection_autofwOutPort_0, "")) return false;
            else if (document.form.IPConnection_autofwInPort_0.value=="" ||
            	document.form.IPConnection_autofwOutPort_0.value=="") bFlag=1;	           
        }       
        else if (s=='IPConnection_ExposedIPList')
        {           
            if (document.form.IPConnection_ExposedIPCount_0.value >= c) cFlag=1;
            else if (!validate_ipaddr(document.form.IPConnection_ExposedLanIP_0, "") ||
            	!validate_ipaddr(document.form.IPConnection_ExposedWanIP_0, "")) return false;
            else if (document.form.IPConnection_ExposedLanIP_0.value=="") bFlag=1;	           
        }    
        else if (s=='FirewallConfig_WLFilterList')
        {           	
            if (document.form.FirewallConfig_WanLanRuleCount_0.value >= c) cFlag=1;	
            else if (!validate_iprange(document.form.FirewallConfig_WanLanSrcIP_0, "") ||
            	!validate_portrange(document.form.FirewallConfig_WanLanSrcPort_0, "") ||
            	!validate_iprange(document.form.FirewallConfig_WanLanDstIP_0, "") ||
            	!validate_portrange(document.form.FirewallConfig_WanLanDstPort_0, "")) return false;
            else if (document.form.FirewallConfig_WanLanSrcIP_0.value=="" &&
                document.form.FirewallConfig_WanLanSrcPort_0.value=="" &&
                document.form.FirewallConfig_WanLanDstIP_0.value=="" &&
                document.form.FirewallConfig_WanLanDstPort_0.value=="") bFlag=1; 	                                                 	    			    			    		            
        } 
        else if (s=='FirewallConfig_LWFilterList')
        {
            if (document.form.FirewallConfig_LanWanRuleCount_0.value >= c) cFlag=1;	
            else if (!validate_iprange(document.form.FirewallConfig_LanWanSrcIP_0, "") ||
            	!validate_portrange(document.form.FirewallConfig_LanWanSrcPort_0, "") ||
            	!validate_iprange(document.form.FirewallConfig_LanWanDstIP_0, "") ||
            	!validate_portrange(document.form.FirewallConfig_LanWanDstPort_0, "")) return false;
            else if (document.form.FirewallConfig_LanWanSrcIP_0.value=="" &&
                document.form.FirewallConfig_LanWanSrcPort_0.value=="" &&
                document.form.FirewallConfig_LanWanDstIP_0.value=="" &&
                document.form.FirewallConfig_LanWanDstPort_0.value=="") bFlag=1;                  
        } 
        else if (s=='FirewallConfig_DWFilterList')
        {
            if (document.form.FirewallConfig_DmzWanRuleCount_0.value >= c) cFlag=1;
            else if (!validate_iprange(document.form.FirewallConfig_DmzWanSrcIP_0, "") ||
            	!validate_portrange(document.form.FirewallConfig_DmzWanSrcPort_0, "") ||
            	!validate_iprange(document.form.FirewallConfig_DmzWanDstIP_0, "") ||
            	!validate_portrange(document.form.FirewallConfig_DmzWanDstPort_0, "")) return false;	
            else if (document.form.FirewallConfig_DmzWanSrcIP_0.value=="" &&
                document.form.FirewallConfig_DmzWanSrcPort_0.value=="" &&
                document.form.FirewallConfig_DmzWanDstIP_0.value=="" &&
                document.form.FirewallConfig_DmzWanDstPort_0.value=="") bFlag=1; 	 	 
           
        } 
        else if (s=='FirewallConfig_WDFilterList')
        {
            if (document.form.FirewallConfig_WanDmzRuleCount_0.value >= c) cFlag=1;	
            else if (!validate_iprange(document.form.FirewallConfig_WanDmzSrcIP_0, "") ||
            	!validate_portrange(document.form.FirewallConfig_WanDmzSrcPort_0, "") ||
            	!validate_iprange(document.form.FirewallConfig_WanDmzDstIP_0, "") ||
            	!validate_portrange(document.form.FirewallConfig_WanDmzDstPort_0, "")) return false;	
            else if (document.form.FirewallConfig_WanDmzSrcIP_0.value=="" &&
                document.form.FirewallConfig_WanDmzSrcPort_0.value=="" &&
                document.form.FirewallConfig_WanDmzDstIP_0.value=="" &&
                document.form.FirewallConfig_WanDmzDstPort_0.value=="") bFlag=1; 	  	 
           
        } 
        else if (s=='FirewallConfig_DLFilterList')
        {
            if (document.form.FirewallConfig_DmzLanRuleCount_0.value >= c) cFlag=1;
            else if (!validate_iprange(document.form.FirewallConfig_DmzLanSrcIP_0, "") ||
            	!validate_portrange(document.form.FirewallConfig_DmzLanSrcPort_0, "") ||
            	!validate_iprange(document.form.FirewallConfig_DmzLanDstIP_0, "") ||
            	!validate_portrange(document.form.FirewallConfig_DmzLanDstPort_0, "")) return false;	
            else if (document.form.FirewallConfig_DmzLanSrcIP_0.value=="" &&
                document.form.FirewallConfig_DmzLanSrcPort_0.value=="" &&
                document.form.FirewallConfig_DmzLanDstIP_0.value=="" &&
                document.form.FirewallConfig_DmzLanDstPort_0.value=="") bFlag=1; 	           
        }         
        else if (s=='FirewallConfig_LDFilterList')
        {
            if (document.form.FirewallConfig_LanDmzRuleCount_0.value >= c) cFlag=1;	
            else if (!validate_iprange(document.form.FirewallConfig_LanDmzSrcIP_0, "") ||
            	!validate_portrange(document.form.FirewallConfig_LanDmzSrcPort_0, "") ||
            	!validate_iprange(document.form.FirewallConfig_LanDmzDstIP_0, "") ||
            	!validate_portrange(document.form.FirewallConfig_LanDmzDstPort_0, "")) return false;	  
            else if (document.form.FirewallConfig_LanDmzSrcIP_0.value=="" &&
                document.form.FirewallConfig_LanDmzSrcPort_0.value=="" &&
                document.form.FirewallConfig_LanDmzDstIP_0.value=="" &&
                document.form.FirewallConfig_LanDmzDstPort_0.value=="") bFlag=1; 	 	            
        } 
        else if (s=='FirewallConfig_UrlList')
        {
            if (document.form.FirewallConfig_UrlListCount_0.value >= c) cFlag=1;		
            if (document.form.FirewallConfig_URLEntry_0.value=="") bFlag=1;                           	   
        }         
        else if (s=='RouterConfig_StaticRoute')
        {
            if (document.form.RouterConfig_StaticRouteCount_0.value > c) cFlag=1;
            else if (!validate_ipaddr(document.form.RouterConfig_StaticIP_0, "") ||
            	!validate_range(document.form.RouterConfig_StaticMask_0, 0, 32) ||
            	!validate_ipaddr(document.form.RouterConfig_StaticGW_0, "")) return false;
            else if (document.form.RouterConfig_StaticIP_0.value=="" ||
            	document.form.RouterConfig_StaticMask_0.value=="" ||
            	document.form.RouterConfig_StaticGW_0.value=="") bFlag=1;
        } 
        else if (s=='RouterConfig_RipSRoute')
        {
            if (document.form.RouterConfig_RipSRouteCount_0.value > c) cFlag=1;	
            else if (!validate_ipaddr(document.form.RouterConfig_RipSRouteIP_0,"") ||
            	!validate_range(document.form.RouterConfig_RipSRouteMask_0, 0, 32)) return false;
            else if (document.form.RouterConfig_RipSRouteIP_0.value=="" ||
            	document.form.RouterConfig_RipSRouteMask_0.value=="") bFlag=1;	
        } 
        else if (s=='RouterConfig_RouteFilter') 
        {
            if (document.form.RouterConfig_RipDRouteCount_0.value > c) cFlag=1;
            else if (!validate_ipaddr(document.form.RouterConfig_RipDRouteIP_0, "") ||
            	!validate_range(document.form.RouterConfig_RipDRouteMask_0, 0, 32)) return false;
            else if (document.form.RouterConfig_RipDRouteIP_0.value=="" ||
            	document.form.RouterConfig_RipDRouteMask_0.value=="") bFlag=1;                                   
        } 
    	else if (s=='RouterConfig_RipOffset')
    	{
    	    if (document.form.RouterConfig_RipOffsetCount_0.value >= c) cFlag=1;
    	    else if (!validate_ipaddr(document.form.RouterConfig_RipOffsetIP_0, "") ||
            	!validate_range(document.form.RouterConfig_RipOffsetMask_0, 0, 32)) return false;
            else if (document.form.RouterConfig_RipOffsetIP_0.value=="" ||
            	document.form.RouterConfig_RipOffsetMask_0.value=="") bFlag=1;    	        	    	       	        	        	      	    	
        }          
        else if (s=='PrinterStatus_x_FUserList')
    	{
    	    if (document.form.PrinterStatus_x_FUserCount_0.value >= c) cFlag=1;
    	    else if (!validate_string(document.form.PrinterStatus_x_FUserName_0) ||
    	    	!validate_string(document.form.PrinterStatus_x_FUserPass_0) ||
            	!validate_range(document.form.PrinterStatus_x_FUserLogin_0, 0, 12)) return false;
            else if (document.form.PrinterStatus_x_FUserName_0.value=="" ||
            	document.form.PrinterStatus_x_FUserPass_0.value=="" ||
            	document.form.PrinterStatus_x_FUserLogin_0.value=="") bFlag=1;
        }   
        else if (s=='PrinterStatus_x_FBanIPList')
    	{
    	    if (document.form.PrinterStatus_x_FBanIPCount_0.value >= c) cFlag=1;
            else if (!validate_iprange(document.form.PrinterStatus_x_FBanIP_0, "")) return false;
            else if (document.form.PrinterStatus_x_FBanIP_0.value=="") bFlag=1;
        }          	
    }
    
    if (bFlag==1)
        alert("Fields can't be blank!!!"); 
    else if(cFlag==1)
        alert("This table only allow " + c + " items!!!");
    else 
    {    	
       if (s=='FirewallConfig_WLFilterList')
       {           	        	                                 
           updateDateTime("Advanced_Firewall_Content.asp");    	    			    			    		            
       } 
       else if (s=='FirewallConfig_LWFilterList')
       {                            
            updateDateTime("Advanced_Firewall_Content.asp");        	 	           
       } 	
       else if (s=='FirewallConfig_UrlList')
       {                           
   	    updateDateTime("Advanced_URLFilter_Content.asp");       	   	            
       } 
       else if (s=='WLANAuthentication11a_LocalCertDB')
       {
       	    //alert(doucment.form.WLANAuthentication11a           	    
       }
           	    	
       window.top.pageChanged = 0;
       window.top.pageChangedCount = 0;	   	
       o.value = b;
       document.form.action_mode.value = b;
       return true;
    }   
    return false;
}


function isBlank(s) {
 for(i=0; i<s.length; i++) {
  c=s.charAt(i);
  if((c!=' ')&&(c!='\n')&&(c!='\t'))return false;}
 return true;
}

function numbersonly() 
{  	
 if (keyPressed>47 && keyPressed<58)
 	return true;
 else
 	return false;
}
function check_ptl()
{
	if(keyPressed==38)
		return false;
	else
		return true;
}

function linkOver(o) 
{
    o.style.cursor="hand";
}

function linkOut(o) {
}

function linkTo(s)
{
   window.top.location=s;
}

function refreshList()
{
   //url = parent.folderFrame.location.href;   
   parent.location.href = 'index.asp';
   //parent.location.reload();
   //parent.folderFrame.location.href = url;
   //   parent.titleFrame.location.href='Title.asp';   
   //   parent.treeFrame.location.href='index.asp';
}

function onApply(o)
{
   parent.upperframe.location.href="Restarting.html";
   parent.upperframe.location.href="Restarting.html";
}

function is_hwaddr() 
{  	   	
	
if (!nav) keyPressed = IEKey();
else keyPressed=NSKey();
	
 if ((keyPressed>47 && keyPressed<58)||(keyPressed>64 && keyPressed<71)||(keyPressed>96 && keyPressed<103))
 	return true;
 else if (keyPressed==0)
 	return true;
 else
 	return false;		
}

function validate_hwaddr(o) 
{
  if (o.value.length == 0) return true;
  	
  if(o.value != "") 
  {
    if(o.value.length == 12)      
    {
       for(i=0; i<o.value.length; i++) 
       {
          c=o.value.charAt(i);
          if (!(c>='0'&&c<='9') && !(c>='a'&&c<='f') && !(c>='A'&&c<='F')) 
          {
             alert('Please enter the complete MAC address which contains 12 hexdecimal letters.');
             o.value = "";
             o.focus();	
             return false;
          }   
       }   
       return true;
    }
  }
  alert('Please enter the complete MAC address which contains 12 hexdecimal letters.');
  o.value = "";
  o.focus();
  return false;
}

function is_string(o)
{ 
  if (!nav) keyPressed = IEKey();
  else keyPressed = NSKey();

  if (keyPressed==0) return true;
  else if (keyPressed>=0&&keyPressed<=126) return true;

  alert('Invalid character!');
  return false;	
}

function validate_string(o) 
{
  if (o.value.charAt(0)=='"')	
  {
  	alert("This string can not start with '\"'");
  	o.value="";
  	o.focus();
  	return false;
  }
  else
  {  	
  	inv="";
  	for(i=0; i<o.value.length; i++)
  	{
  		if (o.value.charAt(i)<' '||o.value.charAt(i)>'~') 
  		{  			  			
  			inv = inv + o.value.charAt(i);  			
  		}
  	}
  	if (inv!="")
  	{
  		 alert("This string can not contain " + inv +"!");
  		 o.value="";
  		 o.focus();
  		 return false;
  	}
  }  
    	
  return true;  
}

function is_number(o) 
{  
  if (!nav) keyPressed = IEKey();
  else keyPressed = NSKey();

  if (keyPressed==0) return true;
			
  if (keyPressed>47 && keyPressed<58)
  {
 	return true;
  }	
  else
  {
 	return false;
  }	
}

function validate_range(o, min, max)
{
   if(o.value<min || o.value>max)
   {
      alert('Please enter a value between ' + min + ' to ' + max + '.');
      o.value = "";
      o.focus();
      return false;    
   }
   else
   {
  
      return true;   
   }
}

function change_ipaddr(o)
{   
/*
   j = 0;
   	
   for(i=0; i<o.value.length; i++) 
   {                      
       if (o.value.charAt(i)=='.')
       {
           j++;            	
       }
   }   
        
   if (j<3 && i>=3)
   {
       if (o.value.charAt(i-3)!='.' && o.value.charAt(i-2)!='.' && o.value.charAt(i-1)!='.')
          o.value = o.value + '.';
   }
      
   return;
*/
}

function is_ipaddr(o) 
{	
   //alert(e.which);	
   //if (nav) keyPressed = e.which;
   //alert('NAV' + e.which);
 
   if (!nav) keyPressed = IEKey();
   else keyPressed=NSKey();

   
   if (keyPressed==0) 
   {	
      return true;
   }   
        	
   
   if (o.value.length>=16) return false;
         
   if ((keyPressed>47 && keyPressed<58))
   {   	
        //o.value = o.value + String.fromCharCode(keyPressed);       
        j = 0;
   	
        for(i=0; i<o.value.length; i++) 
   	{                      
            if (o.value.charAt(i)=='.')
            {
               j++;            	
            }
        }           
                
        if (j<3 && i>=3)
        {
            if (o.value.charAt(i-3)!='.' && o.value.charAt(i-2)!='.' && o.value.charAt(i-1)!='.')
            {
                o.value = o.value + '.';
            }    
        }
        
        return true;
   }
   else if (keyPressed == 46)
   {
        j = 0;
   	
        for(i=0; i<o.value.length; i++) 
   	{                      
            if (o.value.charAt(i)=='.')
            {
               j++;            	
            }
        }   	                         
        
        if (o.value.charAt(i-1)=='.' || j==3)
        {          
 	    return false; 
        }        
        return true; 
   }		
   else
   {
 	return false;
   }
   return false;
}

function validate_ipaddr(o, v) 
{
  num = -1;
  pos = 0;
      
  if (o.value.length==0)
     return true;
  
  for(i=0; i<o.value.length; i++) {
     c=o.value.charAt(i);
     if(c>='0' && c<='9')
     {
        if ( num==-1 ) 
        {
            num = (c-'0');
        }
        else
        {   
            num = num*10 + (c-'0');
        }
     } 
     else 
     {           
        if ( num<0 || num>255 || c!='.') 
        {
            alert(o.value + ' is not a valid IP address!');
            o.value = "";
            o.focus();
            return false;
        }    
        
        if (pos==0) v1=num;
        else if (pos==1) v2=num;
        else if (pos==2) v3=num;        
        num = -1;
        pos++;
     }      
  }
  if (pos!=3 || num<0 || num>255)
  {
     alert(o.value + ' is not a valid IP address!');
     o.value = "";
     o.focus();
     return false;
  } 
  else v4=num;
  
  if ((v1 > 0) && (v1 < 127)) mask = "255.0.0.0";	
  else if ((v1 > 127) && (v1 < 192)) mask = "255.255.0.0";        
  else if ((v1 > 191) && (v1 < 224)) mask = "255.255.255.0";        
  
  //if (v=='ExternalIPAddress' && document.form.IPConnection_x_ExternalSubnetMask.value == '')
  if (v=='ExternalIPAddress')
  {	
  	//document.form.IPConnection_x_ExternalSubnetMask.value = "255.255.255.0";
  	document.form.IPConnection_x_ExternalSubnetMask.value = mask;
  }   
  //else if (v=='IPRouters' && document.form.LANHostConfig_SubnetMask.value == '')
  else if (v=='IPRouters')
  {	
  	//document.form.LANHostConfig_SubnetMask.value = "255.255.255.0";
  	document.form.LANHostConfig_SubnetMask.value = mask;
  }   
  return true;
}

function change_ipaddrport(o)
{   
/*
   j = 0;
   	
   for(i=0; i<o.value.length; i++) 
   {                      
       if (o.value.charAt(i)=='.')
       {
           j++;            	
       }
   }   
        
   if (j<3 && i>=3)
   {
       if (o.value.charAt(i-3)!='.' && o.value.charAt(i-2)!='.' && o.value.charAt(i-1)!='.')
          o.value = o.value + '.';
   }
      
   return;
*/
}


function is_ipaddrport(o) 
{	
   //alert(e.which);	
   //if (nav) keyPressed = e.which;
   //alert('NAV' + e.which);
 
   if (!nav) keyPressed = IEKey();
   else keyPressed=NSKey();

   
   if (keyPressed==0) 
   {	
      return true;
   }   
   	
   
   //if (o.value.length>=15) return false;
   
   if ((keyPressed>47 && keyPressed<58) || keyPressed == 46 || keyPressed == 58)
   {
   	return true;   	
   }	       
   return false;
}

function validate_ipaddrport(o, v) 
{
  num = -1;
  pos = 0;
      
  if (o.value.length==0)
     return true;
     
  str = o.value;  
  portidx = str.indexOf(":");
	
  if (portidx!=-1)
  {
      port = str.substring(portidx+1);
      len = portidx;
      
      if (port>65535) 
      {
      	 alert(port + ' is not a valid port number!');
         o.value = "";
         o.focus();         
         return false;
      }   
  }   
  else
  {      	
      len = o.value.length;	
  } 
  
  for(i=0; i<len; i++) {
     c=o.value.charAt(i);
     if(c>='0' && c<='9')
     {
        if ( num==-1 ) 
        {
            num = (c-'0');
        }
        else
        {   
            num = num*10 + (c-'0');
        }
     } 
     else 
     {           
        if ( num<0 || num>255 || c!='.') 
        {
            alert(o.value + ' is not a valid IP address!');
            o.value = "";
            o.focus();
            return false;
        }    
        num = -1;
        pos++;
     }      
  }
  if (pos!=3 || num<0 || num>255)
  {
     alert(o.value + ' is not a valid IP address!');
     o.value = "";
     o.focus();
     return false;
  } 
  
  if (v=='ExternalIPAddress' && document.form.IPConnection_x_ExternalSubnetMask.value == '')
  {	
  	document.form.IPConnection_x_ExternalSubnetMask.value = "255.255.255.0";
  }   
  else if (v=='IPRouters' && document.form.LANHostConfig_SubnetMask.value == '')
  {	
  	document.form.LANHostConfig_SubnetMask.value = "255.255.255.0";
  }   
  return true;
}

function change_iprange(o)
{   
/*
   j = 0;
   	
   for(i=0; i<o.value.length; i++) 
   {                      
       if (o.value.charAt(i)=='.')
       {
           j++;            	
       }
   }   
        
   if (j<3 && i>=3)
   {
       if (o.value.charAt(i-3)!='.' && o.value.charAt(i-2)!='.' && o.value.charAt(i-1)!='.')
          o.value = o.value + '.';
   }
      
   return;
*/
}

function is_iprange(o) 
{     	
if (!nav) keyPressed = IEKey();
else keyPressed=NSKey();
	
   if (keyPressed==0) 
   {	
      return true;
   }   
  	
   if (o.value.length>=15) return false;
   
   if ((keyPressed>47 && keyPressed<58))
   {   	
        //o.value = o.value + String.fromCharCode(keyPressed);       

        j = 0;
   	
        for(i=0; i<o.value.length; i++) 
   	{                      
            if (o.value.charAt(i)=='.')
            {
               j++;            	
            }
        }   
        
        if (j<3 && i>=3)
        {
           if (o.value.charAt(i-3)!='.' && o.value.charAt(i-2)!='.' && o.value.charAt(i-1)!='.')
               o.value = o.value + '.';
        }
        return true;        
   }
   else if (keyPressed == 46)
   {
        j = 0;
   	
        for(i=0; i<o.value.length; i++) 
   	{                      
            if (o.value.charAt(i)=='.')
            {
               j++;            	
            }
        }   	                         
        
        if (o.value.charAt(i-1)=='.' || j==3)
        {          
 	    return false; 
        }        
        return true; 
   }
   else if (keyPressed == 42) /* '*' */
   {   
        return true; 
   }		
   else
   {
 	return false;
   }
   return false;
}

function validate_iprange(o, v) 
{
  num = -1;
  pos = 0;
  
  if (o.value.length==0)
     return true;
  
  for(i=0; i<o.value.length; i++) 
  {
     c=o.value.charAt(i);
     
     if(c>='0'&&c<='9')
     {
        if ( num==-1 ) 
        {
            num = (c-'0');
        }
        else
        {   
            num = num*10 + (c-'0');
        }
     } 
     else if (c=='*'&&num==-1)
     {
     	num = 0;
     }     
     else 
     {      
        if ( num<0 || num>255 || (c!='.'))
        {
            alert(o.value + ' is not a valid IP address!');
            o.value = "";
            o.focus();
            return false;
        }    
                
        num = -1;
        pos++;
     }      
  }
  if (pos!=3 || num<0 || num>255)
  {
     alert(o.value + ' is not a valid IP address!');
     o.value = "";
     o.focus();
     return false;
  } 
  
  if (v=='ExternalIPAddress' && document.form.IPConnection_x_ExternalSubnetMask.value == '')
  {	
  	document.form.IPConnection_x_ExternalSubnetMask.value = "255.255.255.0";
  }   
  else if (v=='IPRouters' && document.form.LANHostConfig_SubnetMask.value == '')
  {	
  	document.form.LANHostConfig_SubnetMask.value = "255.255.255.0";
  }   
  return true;
}

function is_portrange(o) 
{       
if (!nav) keyPressed = IEKey();
else keyPressed=NSKey();
	
   if (keyPressed==0) return true;
   	    	
   if (o.value.length>11) return false;
        
   if ((keyPressed>47 && keyPressed<58))
   {              	   
        return true;        
   }
   else if (keyPressed == 58 && o.value.length>0) // ':'
   {   	
        for(i=0; i<o.value.length; i++) 
        {
            c=o.value.charAt(i);
            if (c==':' || c=='>' || c=='<' || c=='=') 
               return false;
        }              
   	return true;   	
   }
   else if (keyPressed==60 || keyPressed==62) // '<' or '>'
   {
   	if (o.value.length==0)
   	{   	  	
   	   return true;
   	}   
   	else
   	   return false;   	          
   }	
   else
   {
 	return false;
   }   
}

function validate_portrange(o, v)
{       
  if (o.value.length==0)
     return true;
  
  prev = 0;
  num = 0;
  
  for(i=0; i<o.value.length; i++) 
  {
     c=o.value.charAt(i);
     
     if (c>='0'&&c<='9')
     {
        num = num*10 + (c-'0');
     }   
     else 
     {
     	if (num>65535 || (c!=':' && c!='>' && c!='<'))
        {
     	   alert(num + ' is not a valid port number!');
     	   o.value = "";
           o.focus();
           return false;
        } 
        prev = num;              
        num = 0;
     }   
  }
  if (num>65535 || (prev>num))
  {
     if (num>65535)
        alert(num + ' is not a valid port number!');
     else 	
        alert(o.value + ' is not a valid port number!range');
     o.value = "";   
     o.focus();
     return false;
  } 
  return true;
  
}

function is_portlist(o) 
{  
if (!nav) keyPressed = IEKey();
else keyPressed = NSKey();
	
   if (keyPressed == 0 ) return true;
   	   	
   if (o.value.length>36) return false;
        
   if ((keyPressed>47 && keyPressed<58) || keyPressed == 32)
   {              	   
        return true;        
   }
   else
   {
 	return false;
   }   
}

function validate_portlist(o, v)
{       
  if (o.value.length==0)
     return true;
  
  num = 0;
  
  for(i=0; i<o.value.length; i++) 
  {
     c=o.value.charAt(i);
     
     if (c>='0'&&c<='9')
     {
        num = num*10 + (c-'0');
     }   
     else 
     {
     	if (num>255) 
        {
     	   alert(num + ' is not a valid port number!');
     	   o.value = "";
           o.focus();
           return false;
        }                       
        num = 0;
     }   
  }
  if (num>255)
  {
     alert(num + ' is not a valid port number!');
     o.value = "";
     o.focus();
     return false;
  } 
  return true;  
}


function add_option(o, s, f)
{
  tail = o.options.length;
  
  o.options[tail] = new Option(s);
  o.options[tail].value = s; 	
  
  if (f==1)
  {
     o.options[tail].selected = f;
  }   
}

function add_option_match(o, s, f)
{
  tail = o.options.length;
  
  o.options[tail] = new Option(s);
  o.options[tail].value = s; 	
  
  //alert(f);
  //alert(s);
  
  if (f==s)
  {
     o.options[tail].selected = 1;
     return(1);
  }   
  else return(0);
}

function add_option_match_x(o, s, f)
{
  tail = o.options.length;
  
  o.options[tail] = new Option(s);
  o.options[tail].value = tail;
  
  if (tail==f)
  {
  	o.options[tail].selected = 1;
  	return(1);
  }	
  else return(0);
}

function add_option_x(o, i, s, f)
{
  tail = o.options.length;
  
  o.options[tail] = new Option(s); 	
  o.options[tail].value = i; 	
  
  if (f==1)
  {
     o.options[tail].selected = f;
  }   
}
 
function free_options(o)
{ 
   count = o.options.length;
   
   for (i=0; i<count; i++)
   {   	
    	o.options[0].value = null;
    	o.options[0] = null;
   } 
}

function find_option(o)
{
   count = o.options.length;
   
   for (i=0; i<count; i++)
   {
   	if (o.options[i].selected)
   		return(o.options[i].value);
   }   		
   return(null);
}


function add_options(o, arr, orig)
{       
  	                         
  for (i = 0; i < arr.length; i++) 
  {        
  	if (orig == arr[i])
	   add_option(o, arr[i], 1);	   
	else
	   add_option(o, arr[i], 0);   
  }     
}  

function add_options_x(o, arr, orig)
{       
  	                         
  for (i = 0; i < arr.length; i++) 
  {        
  	if (orig == i)
	   add_option_x(o, i, arr[i], 1);	   
	else
	   add_option_x(o, i, arr[i], 0);   
  }     
}  

function rcheck(o)
{
   if (o[0].checked == true) 
   	return("1");
   else
        return("0");    		
}

function SaveChannelList(o)
{
   count = o.options.length;
            
   chanList = new Array;
            
   for (i=0; i<count; i++)
   {   	   	
   	chanList[chanList.length] = o.options[i].value;    
   } 	
}

function RestoreChannelList(d, isOut, isTurbo)
{
   for (i=0; i< chanList.length; i++)
   {
   	if (isOut=="0"&&chanList[i]>=100) return;
   	if (chanList[i] == 999) return;
   	d[d.length] = chanList[i];
   } 	
}

function RefreshChannelList(isOut, isTurbo)
{
   orig = document.form.WLANConfig11a_Channel.value;
   	
   free_options(document.form.WLANConfig11a_Channel);
   
   if (isTurbo=="1")
   {   	 
   	if (isOut == "1")
   	{
   	    items = new Array("42","50","58","152","160");
   	}
   	else
   	{
   	    items = new Array("42","50","58");
   	}    
   }
   else
   {
   	items = new Array; 
   	      	   	
	RestoreChannelList(items, isOut, isTurbo);
   }
    
   add_options(document.form.WLANConfig11a_Channel, items, orig);
}

function RefreshRateList(isTurbo)
{
   orig = document.form.WLANConfig11a_DataRate.value;
   	
   free_options(document.form.WLANConfig11a_DataRate);
     
   if (isTurbo=="1")
   {   	
       items = new Array("Best","12","18","24","36","48","72","96","108");   	   	
   } 
   else
   {
       items = new Array("Best","6","9","12","18","24","36","48","54");   	   			 
   }      
   add_options_x(document.form.WLANConfig11a_DataRate, items, orig);		
}

function RefreshRateSetList(gmode, chg)
{

   // There is no this field in WL501b	
   return;
	
	
   orig = document.form.WLANConfig11b_DataRate.value;
   	
   free_options(document.form.WLANConfig11b_DataRate);
     
   // gmode : 
   // 0 : Auto
   // 1 : G only
   // 2 : LRS
   // 3 : B only	  
   if (gmode != "3")
   {   	      
       if (gmode == "1")
       {
          items = new Array("1 & 2 Mbps", "1, 2, 5.5, 6, 11, 12 & 24 Mbps", "All");
       }   
       else
       {
          items = new Array("1 & 2 Mbps", "1, 2, 5.5 & 11 Mbps", "All");
       }      
       
       if (chg) orig = 1;
   } 
   else
   {
       items = new Array("1 & 2 Mbps", "1, 2, 5.5 & 11 Mbps");
       
       if (chg) orig = 1;
   }      
   
   add_options_x(document.form.WLANConfig11b_DataRate, items, orig);
}

function getDateCheck(str, pos)
{
   if (str.charAt(pos) == '1') 
      return true;
   else 
      return false;
}

function getTimeRange(str, pos)
{   
   if (pos == 0) 
      return str.substring(0,2);
   else if (pos == 1) 
      return str.substring(2,4);  
   else if (pos == 2) 
      return str.substring(4,6);
   else if (pos == 3) 
      return str.substring(6,8);
}

function setDateCheck(d1, d2, d3, d4, d5, d6, d7)
{
   str = "";
      	
   if (d7.checked == true ) str = "1" + str;
   else str = "0" + str;
   if (d6.checked == true ) str = "1" + str;
   else str = "0" + str;
   if (d5.checked == true ) str = "1" + str;
   else str = "0" + str;
   if (d4.checked == true ) str = "1" + str;
   else str = "0" + str;
   if (d3.checked == true ) str = "1" + str;
   else str = "0" + str;
   if (d2.checked == true ) str = "1" + str;
   else str = "0" + str;
   if (d1.checked == true ) str = "1" + str;
   else str = "0" + str;   
   
   return str;
}

function setTimeRange(sh, sm, eh, em)
{	 
   return(sh.value+sm.value+eh.value+em.value);	
}

function to_quick() 
{
   location.href = "Basic_GOperation_Content.asp";
} 	
       
function load_wizard()
{
   if (document.form.first_time.value != "1")
   {
       rst = confirm("Thanks for purchasing ASUS Wireless Router. Do you want to start Quick Setup directly?");
       if (rst)        
       {       	   
 	   setTimeout("to_quick()", 1000)
       }
   }       	
}

function load_body()
{
   document.form.next_host.value = location.host;	
   
   if (document.form.current_page.value == "Advanced_Wireless_Content.asp")
   {
       if (window.top.isModel() == "WL600")                 	        	      
       {
                	        	           
       	if (document.form.WLANConfig11a_WEPType.value == 'None' || document.form.WLANConfig11a_WEPType.value == 'FromServer')      
         	flag = 0;         
       	else       
      	 	flag = 1;
      	                                    
       	inputCtrl(document.form.WLANConfig11a_x_Phrase, flag); 
       	inputCtrl(document.form.WLANConfig11a_WEPKey1, flag);        
       	inputCtrl(document.form.WLANConfig11a_WEPKey2, flag);  
       	inputCtrl(document.form.WLANConfig11a_WEPKey3, flag);     
       	inputCtrl(document.form.WLANConfig11a_WEPKey4, flag);
       	inputCtrl(document.form.WLANConfig11a_WEPDefaultKey,  flag);   
                  
       	if (document.form.WLANConfig11a_x_RegulatoryDomain.value == 'IN')
       	{
       	   // disable turbo mode and channel 
       	   // inputRCtrl1(document.form.WLANConfig11a_x_OutdoorChannel, 0);
       	   // inputRCtrl1(document.form.WLANConfig11a_x_TurboMode, 0);       	          	        	   
       	}
       
       // No out door channel
       	if (document.form.WLANConfig11a_x_RegulatoryDomain.value == 'IT' ||
           document.form.WLANConfig11a_x_RegulatoryDomain.value == 'IN')
       	{
           document.form.WLANConfig11a_Channel_check.disabled = true;
           document.form.WLANConfig11a_Channel_check.checked = false;
           document.form.WLANConfig11a_x_OutdoorChannel.value = "0";
       	}
       	else if (document.form.WLANConfig11a_x_RegulatoryDomain.value == 'oT' ||
             document.form.WLANConfig11a_x_RegulatoryDomain.value == 'oN')
       	{
       	   document.form.WLANConfig11a_Channel_check.disabled = true;
       	   document.form.WLANConfig11a_Channel_check.checked = true;      
       	}  
       	else if (document.form.WLANConfig11a_x_OutdoorChannel.value == "1")
    	   document.form.WLANConfig11a_Channel_check.checked = true;

        // No turbo mode
       	if (document.form.WLANConfig11a_x_RegulatoryDomain.value == 'IN' ||
           document.form.WLANConfig11a_x_RegulatoryDomain.value == 'ON' ||
           document.form.WLANConfig11a_x_RegulatoryDomain.value == 'oN')
       	{
           document.form.WLANConfig11a_DataRate_check.disabled = true;           
           document.form.WLANConfig11a_DataRate_check.checked = false;
           document.form.WLANConfig11a_x_TurboMode.value = "0";
       	} 
       	else if (document.form.WLANConfig11a_x_TurboMode.value == "1")   
           document.form.WLANConfig11a_DataRate_check.checked = true;
     
       	SaveChannelList(document.form.WLANConfig11a_Channel);
       	RefreshChannelList(document.form.WLANConfig11a_x_OutdoorChannel.value, document.form.WLANConfig11a_x_TurboMode.value);
       	RefreshRateList(document.form.WLANConfig11a_x_TurboMode.value);
                                           
        if (document.form.WLANConfig11b_WEPType.value == 'None' || document.form.WLANConfig11b_WEPType.value == 'FromServer')      
          	flag = 0;        
       	else 
         	flag = 1;                            
        
       	inputCtrl(document.form.WLANConfig11b_x_Phrase,  flag);
       	inputCtrl(document.form.WLANConfig11b_WEPKey1,  flag);
       	inputCtrl(document.form.WLANConfig11b_WEPKey2,  flag);
       	inputCtrl(document.form.WLANConfig11b_WEPKey3,  flag);
       	inputCtrl(document.form.WLANConfig11b_WEPKey4,  flag);
       	inputCtrl(document.form.WLANConfig11b_WEPDefaultKey,  flag);
   	//change_common(document.form.WLANConfig11a_AuthenticationMethod, "WLANConfig11a", "AuthenticationMethod");
   	//change_common(document.form.WLANConfig11b_AuthenticationMethod, "WLANConfig11b", "AuthenticationMethod");
      	
      	//RefreshRateSetList("0", "0");
      	if (document.form.WLANConfig11b_x_GProtection.value == "1") 
       	{
            document.form.WLANConfig11b_x_Mode11g_check.checked = true;           
        }
        
        RefreshRateSetList(document.form.WLANConfig11b_x_Mode11g.value, false);
      }
      else
      {
      	
      	wepStr = find_option(document.form.WLANConfig11b_WEPType);
      	changeWPA(wepStr, true);
            	      	      	       	               
        if (window.top.isBand() == "b")                 	        	      
        {
             RefreshRateSetList("3", false);
        }	
        else 
        {
             if (document.form.WLANConfig11b_x_GProtection.value == "1")   
       	     {
           	document.form.WLANConfig11b_x_Mode11g_check.checked = true;           
             }	
             RefreshRateSetList(document.form.WLANConfig11b_x_Mode11g.value, false);	                         
        }     
      }		   	   		
   }
   else if (document.form.current_page.value == "Advanced_LANWAN_Content.asp")
   {
        if (document.form.Layer3Forwarding_x_ConnectionType.value == "Static IP")
  	{
  	   inputCtrl(document.form.IPConnection_ExternalIPAddress, 1); 
  	   inputCtrl(document.form.IPConnection_x_ExternalSubnetMask, 1); 
  	   inputCtrl(document.form.IPConnection_x_ExternalGateway, 1); 
  	   inputCtrl(document.form.PPPConnection_UserName, 0); 
  	   inputCtrl(document.form.PPPConnection_Password, 0); 
  	   inputCtrl(document.form.PPPConnection_IdleDisconnectTime, 0);
  	   inputCtrl(document.form.PPPConnection_x_PPPoEMTU, 0);  	
  	   inputCtrl(document.form.PPPConnection_x_PPPoEMRU, 0);  	  	
  	   inputRCtrl1(document.form.PPPConnection_x_PPPoERelay, 0);  	
  	   inputCtrl(document.form.PPPConnection_IdleDisconnectTime_check, 0); 
  	   inputCtrl(document.form.PPPConnection_x_ServiceName, 0);  	  	   
  	   inputCtrl(document.form.PPPConnection_x_AccessConcentrator, 0);  	  	   
  	   
  	}
  	else if(document.form.Layer3Forwarding_x_ConnectionType.value == "PPPoE")
  	{
  	   inputCtrl(document.form.IPConnection_ExternalIPAddress, 0); 
  	   inputCtrl(document.form.IPConnection_x_ExternalSubnetMask, 0); 
  	   inputCtrl(document.form.IPConnection_x_ExternalGateway, 0); 
  	   inputCtrl(document.form.PPPConnection_UserName, 1); 
  	   inputCtrl(document.form.PPPConnection_Password, 1); 	 
  	   inputCtrl(document.form.PPPConnection_IdleDisconnectTime, 1); 
  	   inputCtrl(document.form.PPPConnection_x_PPPoEMTU, 1);  	
  	   inputCtrl(document.form.PPPConnection_x_PPPoEMRU, 1);  	  	
  	   inputRCtrl1(document.form.PPPConnection_x_PPPoERelay, 1);  	 	
  	    inputCtrl(document.form.PPPConnection_IdleDisconnectTime_check, 1); 
  	   inputCtrl(document.form.PPPConnection_x_ServiceName, 1);  	  	   
  	   inputCtrl(document.form.PPPConnection_x_AccessConcentrator, 1);  	  	   
  	}
  	else if(document.form.Layer3Forwarding_x_ConnectionType.value == "PPTP")
  	{
  	   inputCtrl(document.form.IPConnection_ExternalIPAddress, 1); 
  	   inputCtrl(document.form.IPConnection_x_ExternalSubnetMask, 1); 
  	   inputCtrl(document.form.IPConnection_x_ExternalGateway, 1); 
  	   inputCtrl(document.form.PPPConnection_UserName, 1); 
  	   inputCtrl(document.form.PPPConnection_Password, 1); 	 
  	   inputCtrl(document.form.PPPConnection_IdleDisconnectTime, 1); 
  	   inputCtrl(document.form.PPPConnection_x_PPPoEMTU, 0);  	
  	   inputCtrl(document.form.PPPConnection_x_PPPoEMRU, 0);  	  	
  	   inputRCtrl1(document.form.PPPConnection_x_PPPoERelay, 0);  	   
  	   inputCtrl(document.form.PPPConnection_IdleDisconnectTime_check, 0); 
  	   inputCtrl(document.form.PPPConnection_x_ServiceName, 0);  	  	   
  	   inputCtrl(document.form.PPPConnection_x_AccessConcentrator, 0);  	
  	}
  	else
  	{
  	   inputCtrl(document.form.IPConnection_ExternalIPAddress, 0); 
  	   inputCtrl(document.form.IPConnection_x_ExternalSubnetMask, 0); 
  	   inputCtrl(document.form.IPConnection_x_ExternalGateway, 0);   	     	   
  	   inputCtrl(document.form.PPPConnection_UserName, 0); 
  	   inputCtrl(document.form.PPPConnection_Password, 0); 	  
  	   inputCtrl(document.form.PPPConnection_IdleDisconnectTime, 0);  	
  	   inputCtrl(document.form.PPPConnection_x_PPPoEMTU, 0);  	
  	   inputCtrl(document.form.PPPConnection_x_PPPoEMRU, 0);  	  			
  	   inputRCtrl1(document.form.PPPConnection_x_PPPoERelay, 0);
  	   inputCtrl(document.form.PPPConnection_IdleDisconnectTime_check, 0); 
  	   inputCtrl(document.form.PPPConnection_x_ServiceName, 0);  	  	   
  	   inputCtrl(document.form.PPPConnection_x_AccessConcentrator, 0);  	
  	}	
  	
  	if (document.form.IPConnection_x_DNSServerEnable[0].checked == true)
    	{   
    		inputCtrl(document.form.IPConnection_x_DNSServer1, 0);  
    		inputCtrl(document.form.IPConnection_x_DNSServer2, 0);    
    	}	
    	else  
    	{    	
    		inputCtrl(document.form.IPConnection_x_DNSServer1, 1);  
	    	inputCtrl(document.form.IPConnection_x_DNSServer2, 1);    
    	}
    	if (document.form.PPPConnection_x_IdleTxOnly.value == "1") 
       	{
               document.form.PPPConnection_IdleDisconnectTime_check.checked = true;
        }
    	  	  	
   	//change_common(document.form.Layer3Forwarding_x_ConnectionType, "Layer3Forwarding", "x_ConnectionType");
   }
   else if (document.form.current_page.value == "Advanced_MultiPPPoE_Content.asp")
   {   	
   	
   	if (document.form.PPPConnection_x_MultiPPPoEEnable1[0].checked == true)
  	{
  	        flag=1;	
  	}	
  	else
  	{
  		flag=0;
  	}
  	
  	inputCtrl(document.form.PPPConnection_x_UserName1, flag); 
  	inputCtrl(document.form.PPPConnection_x_Password1, flag); 
  	inputCtrl(document.form.PPPConnection_x_IdleTime1, flag);
  	inputCtrl(document.form.PPPConnection_x_IdleTime1_check, flag); 
  	inputCtrl(document.form.PPPConnection_x_PPPoEMTU1, flag);  	
  	inputCtrl(document.form.PPPConnection_x_PPPoEMRU1, flag);  	  	  	  	
  	inputCtrl(document.form.PPPConnection_x_ServiceName1, flag);  	  	   
  	inputCtrl(document.form.PPPConnection_x_AccessConcentrator1, flag);  	  	     	  	  	  		  	  
   	   	   	   	  	
  	if (document.form.PPPConnection_x_MultiPPPoEEnable2[0].checked == true)
  	{
  	        flag=1;	
  	}	
  	else
  	{
  		flag=0;
  	}
  	
  	inputCtrl(document.form.PPPConnection_x_UserName2, flag); 
  	inputCtrl(document.form.PPPConnection_x_Password2, flag); 
  	inputCtrl(document.form.PPPConnection_x_IdleTime2, flag);
  	inputCtrl(document.form.PPPConnection_x_IdleTime2_check, flag);
  	inputCtrl(document.form.PPPConnection_x_PPPoEMTU2, flag);  	
  	inputCtrl(document.form.PPPConnection_x_PPPoEMRU2, flag);  	  	  	  	
  	inputCtrl(document.form.PPPConnection_x_ServiceName2, flag);  	  	   
  	inputCtrl(document.form.PPPConnection_x_AccessConcentrator2, flag);
  	  	      	
   	if (document.form.PPPConnection_x_IdleTxOnly1.value == "1") 
       	{
               document.form.PPPConnection_x_IdleTime1_check.checked = true;
        }
        if (document.form.PPPConnection_x_IdleTxOnly2.value == "1") 
       	{
               document.form.PPPConnection_x_IdleTime2_check.checked = true;
        }
   }
   else if (document.form.current_page.value == "Advanced_RLANWAN_Content.asp")
   {
   	
   }	
   else if (document.form.current_page.value == "Advanced_PortTrigger_Content.asp") 
   {
   	 wItem = new Array(
      	 	 new Array("Quicktime 4 Client", "554", "TCP", "6970:32000", "UDP"),
      	 	 new Array("Real Audio", "7070", "TCP", "6970:7170", "UDP"));
      	 //new Array("MS Terminal Server", "3389", "TCP", "3389", "TCP"));      
      	 	      		
      	 free_options(document.form.IPConnection_TriggerKnownApps);
      	 add_option(document.form.IPConnection_TriggerKnownApps, "User Defined", 1);
         for (i = 0; i < wItem.length; i++) 
         {
             add_option(document.form.IPConnection_TriggerKnownApps, wItem[i][0], 0);
         }                  				   	
   } 
   else if (document.form.current_page.value == "Advanced_PortMapping_Content.asp") 
   {
   	 wItem = new Array(
      		 new Array("Quicktime 4 Client", "554", "6970:32000"),
      		 new Array("Real Audio", "7070", "6970:7170"),
      		 new Array("Windows 2000 Terminal Server", "3389", "3389"));      
      		      		
      	 free_options(document.form.IPConnection_MappedKnownApps);
      	 add_option(document.form.IPConnection_MappedKnownApps, "User Defined", 1);
         for (i = 0; i < wItem.length; i++) 
         {
             add_option(document.form.IPConnection_MappedKnownApps, wItem[i][0], 0);
         }              	   	   	   
         
   	 if (document.form.IPConnection_MappedAction_0.value == "Unset")
   	 {
   	    document.form.IPConnection_MappedIP_0.disabled = true;
   	    document.form.IPConnection_MappedInPort_0.disabled = true;
   	    document.form.IPConnection_MappedDescript_0.disabled = true;
   	 }
   	  
   	 if (document.form.IPConnection_MappedAction_1.value == "Unset")
   	 {
   	    document.form.IPConnection_MappedIP_1.disabled = true;
   	    document.form.IPConnection_MappedInPort_1.disabled = true;
   	    document.form.IPConnection_MappedDescript_1.disabled = true;
   	 }
   	 
   	 if (document.form.IPConnection_MappedAction_2.value == "Unset")
   	 {
   	    document.form.IPConnection_MappedIP_2.disabled = true;
   	    document.form.IPConnection_MappedInPort_2.disabled = true;
   	    document.form.IPConnection_MappedDescript_2.disabled = true;
   	 }
   	      	   	               		
   	 if (document.form.IPConnection_MappedAction_3.value == "Unset")
   	 {
   	    document.form.IPConnection_MappedIP_3.disabled = true;
   	    document.form.IPConnection_MappedInPort_3.disabled = true;
   	    document.form.IPConnection_MappedDescript_3.disabled = true;
   	 }
   	 
   	 if (document.form.IPConnection_MappedAction_4.value == "Unset")
   	 {
   	    document.form.IPConnection_MappedIP_4.disabled = true;
   	    document.form.IPConnection_MappedInPort_4.disabled = true;
   	    document.form.IPConnection_MappedDescript_4.disabled = true;
   	 }
   	 
   	 if (document.form.IPConnection_MappedAction_5.value == "Unset")
   	 {
   	    document.form.IPConnection_MappedIP_5.disabled = true;
   	    document.form.IPConnection_MappedInPort_5.disabled = true;
   	    document.form.IPConnection_MappedDescript_5.disabled = true;
   	 }
   }	
   else if (document.form.current_page.value == "Advanced_VirtualServer_Content.asp") 
   {
   	 wItem = new Array(
      		 new Array("FTP", "20:21", "TCP"),
      		 new Array("TELNET", "23", "TCP"),
      		 new Array("SMTP", "25", "TCP"),
      		 new Array("DNS", "53", "BOTH"),
      		 new Array("FINGER", "79", "TCP"),
      		 new Array("HTTP", "80", "TCP"),
      		 new Array("POP3", "110", "TCP"),
      		 new Array("SNMP", "161", "UDP"),
      		 new Array("SNMP TRAP", "162", "UDP"));      
      		      		
      	 free_options(document.form.IPConnection_KnownApps);
      	 add_option(document.form.IPConnection_KnownApps, "User Defined", 1);
         for (i = 0; i < wItem.length; i++) 
         {
            add_option(document.form.IPConnection_KnownApps, wItem[i][0], 0);
         }   
         
      				   	
   } 
   else if (document.form.current_page.value == "Advanced_Firewall_Content.asp") 
   {
   	 wItem = new Array(
   	  	 new Array("WWW", "80", "TCP"),      		 
      		 new Array("ICQ", "5190", "TCP"),      		
      		 new Array("REAL PLAYER/QUICK TIME", "554", "TCP"),
      		 new Array("TELNET", "23", "TCP"),       		 
      		 new Array("FTP", "20:21", "TCP"), 
      		 new Array("MSN Messenger", "1863", "TCP"),     		 
      		 new Array("MIRC", "6667", "TCP"),      		
      		 new Array("NETMEETING(1)", "389", "TCP"),
      		 new Array("NETMEETING(2)", "522", "TCP"),
      		 new Array("NETMEETING(3)", "1720", "TCP"),
      		 new Array("NETMEETING(4)", "1731", "TCP"));      
      		      		
      	 free_options(document.form.FirewallConfig_WLKnownApps);
      	 add_option(document.form.FirewallConfig_WLKnownApps, "User Defined", 1);
         for (i = 0; i < wItem.length; i++) 
         {
            add_option(document.form.FirewallConfig_WLKnownApps, wItem[i][0], 0);
         }   
         
         free_options(document.form.FirewallConfig_LWKnownApps);
      	 add_option(document.form.FirewallConfig_LWKnownApps, "User Defined", 1);
         for (i = 0; i < wItem.length; i++) 
         {
            add_option(document.form.FirewallConfig_LWKnownApps, wItem[i][0], 0);
         }   
         
         // Handle for Date and Time
         document.form.FirewallConfig_WanLanActiveDate_Sun.checked = getDateCheck(document.form.FirewallConfig_WanLanActiveDate.value, 0);
         document.form.FirewallConfig_WanLanActiveDate_Mon.checked = getDateCheck(document.form.FirewallConfig_WanLanActiveDate.value, 1);
         document.form.FirewallConfig_WanLanActiveDate_Tue.checked = getDateCheck(document.form.FirewallConfig_WanLanActiveDate.value, 2);
         document.form.FirewallConfig_WanLanActiveDate_Wed.checked = getDateCheck(document.form.FirewallConfig_WanLanActiveDate.value, 3);
         document.form.FirewallConfig_WanLanActiveDate_Thu.checked = getDateCheck(document.form.FirewallConfig_WanLanActiveDate.value, 4);
         document.form.FirewallConfig_WanLanActiveDate_Fri.checked = getDateCheck(document.form.FirewallConfig_WanLanActiveDate.value, 5);
         document.form.FirewallConfig_WanLanActiveDate_Sat.checked = getDateCheck(document.form.FirewallConfig_WanLanActiveDate.value, 6);
                                  
         document.form.FirewallConfig_WanLanActiveTime_starthour.value = getTimeRange(document.form.FirewallConfig_WanLanActiveTime.value, 0);
         document.form.FirewallConfig_WanLanActiveTime_startmin.value = getTimeRange(document.form.FirewallConfig_WanLanActiveTime.value, 1);
         document.form.FirewallConfig_WanLanActiveTime_endhour.value = getTimeRange(document.form.FirewallConfig_WanLanActiveTime.value, 2);
         document.form.FirewallConfig_WanLanActiveTime_endmin.value = getTimeRange(document.form.FirewallConfig_WanLanActiveTime.value, 3);
   	
   	 document.form.FirewallConfig_LanWanActiveDate_Sun.checked = getDateCheck(document.form.FirewallConfig_LanWanActiveDate.value, 0);
         document.form.FirewallConfig_LanWanActiveDate_Mon.checked = getDateCheck(document.form.FirewallConfig_LanWanActiveDate.value, 1);
         document.form.FirewallConfig_LanWanActiveDate_Tue.checked = getDateCheck(document.form.FirewallConfig_LanWanActiveDate.value, 2);
         document.form.FirewallConfig_LanWanActiveDate_Wed.checked = getDateCheck(document.form.FirewallConfig_LanWanActiveDate.value, 3);
         document.form.FirewallConfig_LanWanActiveDate_Thu.checked = getDateCheck(document.form.FirewallConfig_LanWanActiveDate.value, 4);
         document.form.FirewallConfig_LanWanActiveDate_Fri.checked = getDateCheck(document.form.FirewallConfig_LanWanActiveDate.value, 5);
         document.form.FirewallConfig_LanWanActiveDate_Sat.checked = getDateCheck(document.form.FirewallConfig_LanWanActiveDate.value, 6);
                                  
         document.form.FirewallConfig_LanWanActiveTime_starthour.value = getTimeRange(document.form.FirewallConfig_LanWanActiveTime.value, 0);
         document.form.FirewallConfig_LanWanActiveTime_startmin.value = getTimeRange(document.form.FirewallConfig_LanWanActiveTime.value, 1);
         document.form.FirewallConfig_LanWanActiveTime_endhour.value = getTimeRange(document.form.FirewallConfig_LanWanActiveTime.value, 2);
         document.form.FirewallConfig_LanWanActiveTime_endmin.value = getTimeRange(document.form.FirewallConfig_LanWanActiveTime.value, 3);
                   	  	
         change_internet_firewall(rcheck(document.form.FirewallConfig_WanLanFirewallEnable));                  
   } 
   else if (document.form.current_page.value == "Advanced_LFirewall_Content.asp")
   {
         document.form.FirewallConfig_WanLocalActiveDate_Sun.checked = getDateCheck(document.form.FirewallConfig_WanLocalActiveDate.value, 0);
         document.form.FirewallConfig_WanLocalActiveDate_Mon.checked = getDateCheck(document.form.FirewallConfig_WanLocalActiveDate.value, 1);
         document.form.FirewallConfig_WanLocalActiveDate_Tue.checked = getDateCheck(document.form.FirewallConfig_WanLocalActiveDate.value, 2);
         document.form.FirewallConfig_WanLocalActiveDate_Wed.checked = getDateCheck(document.form.FirewallConfig_WanLocalActiveDate.value, 3);
         document.form.FirewallConfig_WanLocalActiveDate_Thu.checked = getDateCheck(document.form.FirewallConfig_WanLocalActiveDate.value, 4);
         document.form.FirewallConfig_WanLocalActiveDate_Fri.checked = getDateCheck(document.form.FirewallConfig_WanLocalActiveDate.value, 5);
         document.form.FirewallConfig_WanLocalActiveDate_Sat.checked = getDateCheck(document.form.FirewallConfig_WanLocalActiveDate.value, 6);
                                  
         document.form.FirewallConfig_WanLocalActiveTime_starthour.value = getTimeRange(document.form.FirewallConfig_WanLocalActiveTime.value, 0);
         document.form.FirewallConfig_WanLocalActiveTime_startmin.value = getTimeRange(document.form.FirewallConfig_WanLocalActiveTime.value, 1);
         document.form.FirewallConfig_WanLocalActiveTime_endhour.value = getTimeRange(document.form.FirewallConfig_WanLocalActiveTime.value, 2);
         document.form.FirewallConfig_WanLocalActiveTime_endmin.value = getTimeRange(document.form.FirewallConfig_WanLocalActiveTime.value, 3);
   }    
   else if (document.form.current_page.value == "Advanced_URLFilter_Content.asp")
   {
         document.form.FirewallConfig_URLActiveDate_Sun.checked = getDateCheck(document.form.FirewallConfig_URLActiveDate.value, 0);
         document.form.FirewallConfig_URLActiveDate_Mon.checked = getDateCheck(document.form.FirewallConfig_URLActiveDate.value, 1);
         document.form.FirewallConfig_URLActiveDate_Tue.checked = getDateCheck(document.form.FirewallConfig_URLActiveDate.value, 2);
         document.form.FirewallConfig_URLActiveDate_Wed.checked = getDateCheck(document.form.FirewallConfig_URLActiveDate.value, 3);
         document.form.FirewallConfig_URLActiveDate_Thu.checked = getDateCheck(document.form.FirewallConfig_URLActiveDate.value, 4);
         document.form.FirewallConfig_URLActiveDate_Fri.checked = getDateCheck(document.form.FirewallConfig_URLActiveDate.value, 5);
         document.form.FirewallConfig_URLActiveDate_Sat.checked = getDateCheck(document.form.FirewallConfig_URLActiveDate.value, 6);
                                  
         document.form.FirewallConfig_URLActiveTime_starthour.value = getTimeRange(document.form.FirewallConfig_URLActiveTime.value, 0);
         document.form.FirewallConfig_URLActiveTime_startmin.value = getTimeRange(document.form.FirewallConfig_URLActiveTime.value, 1);
         document.form.FirewallConfig_URLActiveTime_endhour.value = getTimeRange(document.form.FirewallConfig_URLActiveTime.value, 2);
         document.form.FirewallConfig_URLActiveTime_endmin.value = getTimeRange(document.form.FirewallConfig_URLActiveTime.value, 3);
   }               
   else if (document.form.current_page.value == "Advanced_DHCP_Content.asp" ||
            document.form.current_page.value == "Advanced_RDHCP_Content.asp") 
   {
   	//checkSubnet();
   }  
   else if (document.form.current_page.value == "Advanced_DMZDHCP_Content.asp")
   {   	
   	//checkDmzSubnet();
   }   
   else if (document.form.current_page.value == "Advanced_DDNS_Content.asp")
   {
   	if (document.form.LANHostConfig_x_DDNSStatus.value != "1")
   	    document.form.LANHostConfig_x_DDNSStatus_button.disabled = true;
   }
   else if (document.form.current_page.value == "Advanced_APLAN_Content.asp")
   {
  	//alert(document.form.IPConnection_x_DNSServerEnable[0].checked);
  	//alert(document.form.IPConnection_x_DNSServerEnable[1].checked);
  	if (document.form.LANHostConfig_x_LANDHCPClient[0].checked == true)
    	{   
    		inputCtrl(document.form.LANHostConfig_IPRouters, 0);  
    		inputCtrl(document.form.LANHostConfig_SubnetMask, 0);   
    		inputCtrl(document.form.LANHostConfig_x_Gateway, 0);     
    	}	
    	else   
    	{    	
    		inputCtrl(document.form.LANHostConfig_IPRouters, 1);  
    		inputCtrl(document.form.LANHostConfig_SubnetMask, 1);   
    		inputCtrl(document.form.LANHostConfig_x_Gateway, 1);    
    	}	  	
   }    
   else if (document.form.current_page.value == "Advanced_WebCam_Content.asp") 
   {   	          
         // Handle for Date and Time
	 if (document.form.PrinterStatus_x_WModel.value == 0)
	 {
		// If type == PWC, then only 320x240 or 160x120 could be selected
		document.form.PrinterStatus_x_WImageSize.value = 1;
		document.form.PrinterStatus_x_WImageSize.options[0].value = null;
		document.form.PrinterStatus_x_WImageSize.options[0] = null;		
	 }
         document.form.PrinterStatus_x_WSecurityDate_Sun.checked = getDateCheck(document.form.PrinterStatus_x_WSecurityDate.value, 0);
         document.form.PrinterStatus_x_WSecurityDate_Mon.checked = getDateCheck(document.form.PrinterStatus_x_WSecurityDate.value, 1);
         document.form.PrinterStatus_x_WSecurityDate_Tue.checked = getDateCheck(document.form.PrinterStatus_x_WSecurityDate.value, 2);
         document.form.PrinterStatus_x_WSecurityDate_Wed.checked = getDateCheck(document.form.PrinterStatus_x_WSecurityDate.value, 3);
         document.form.PrinterStatus_x_WSecurityDate_Thu.checked = getDateCheck(document.form.PrinterStatus_x_WSecurityDate.value, 4);
         document.form.PrinterStatus_x_WSecurityDate_Fri.checked = getDateCheck(document.form.PrinterStatus_x_WSecurityDate.value, 5);
         document.form.PrinterStatus_x_WSecurityDate_Sat.checked = getDateCheck(document.form.PrinterStatus_x_WSecurityDate.value, 6);
                                  
         document.form.PrinterStatus_x_WSecurityTime_starthour.value = getTimeRange(document.form.PrinterStatus_x_WSecurityTime.value, 0);
         document.form.PrinterStatus_x_WSecurityTime_startmin.value = getTimeRange(document.form.PrinterStatus_x_WSecurityTime.value, 1);
         document.form.PrinterStatus_x_WSecurityTime_endhour.value = getTimeRange(document.form.PrinterStatus_x_WSecurityTime.value, 2);
         document.form.PrinterStatus_x_WSecurityTime_endmin.value = getTimeRange(document.form.PrinterStatus_x_WSecurityTime.value, 3);   	   	
   }
   else if (document.form.current_page.value == "Advanced_WAdvanced_Content.asp" && window.top.isModel()!= "WL300") 
   {   	                   
         //document.form.WLANConfig11b_x_RadioEnableDate_Sun.checked = getDateCheck(document.form.WLANConfig11b_x_RadioEnableDate.value, 0);
         //document.form.WLANConfig11b_x_RadioEnableDate_Mon.checked = getDateCheck(document.form.WLANConfig11b_x_RadioEnableDate.value, 1);
         //document.form.WLANConfig11b_x_RadioEnableDate_Tue.checked = getDateCheck(document.form.WLANConfig11b_x_RadioEnableDate.value, 2);
         //document.form.WLANConfig11b_x_RadioEnableDate_Wed.checked = getDateCheck(document.form.WLANConfig11b_x_RadioEnableDate.value, 3);
         //document.form.WLANConfig11b_x_RadioEnableDate_Thu.checked = getDateCheck(document.form.WLANConfig11b_x_RadioEnableDate.value, 4);
         //document.form.WLANConfig11b_x_RadioEnableDate_Fri.checked = getDateCheck(document.form.WLANConfig11b_x_RadioEnableDate.value, 5);
         //document.form.WLANConfig11b_x_RadioEnableDate_Sat.checked = getDateCheck(document.form.WLANConfig11b_x_RadioEnableDate.value, 6);
                                  
         //document.form.WLANConfig11b_x_RadioEnableTime_starthour.value = getTimeRange(document.form.WLANConfig11b_x_RadioEnableTime.value, 0);
         //document.form.WLANConfig11b_x_RadioEnableTime_startmin.value = getTimeRange(document.form.WLANConfig11b_x_RadioEnableTime.value, 1);
         //document.form.WLANConfig11b_x_RadioEnableTime_endhour.value = getTimeRange(document.form.WLANConfig11b_x_RadioEnableTime.value, 2);
         //document.form.WLANConfig11b_x_RadioEnableTime_endmin.value = getTimeRange(document.form.WLANConfig11b_x_RadioEnableTime.value, 3);   	   	
   } 
   else if (document.form.current_page.value == "Advanced_DMZIP11g_Content.asp" ||
            document.form.current_page.value == "Advanced_DMZIP_Content.asp")
   {
  	change_wireless_firewall();	
   }    
   else if (document.form.current_page.value == "Advanced_WMode_Content.asp")
   {   	
    	//change_wireless_bridge(document.form.WLANConfig11b_x_APMode.value,
  	//			rcheck(document.form.WLANConfig11b_x_BRApply),
  	//			rcheck(document.form.WLANConfig11b_x_BRestrict), 0);  				
  	
  	// WL501b, there is no BRestrict  	
  	change_wireless_bridge(document.form.WLANConfig11b_x_APMode.value,
  				rcheck(document.form.WLANConfig11b_x_BRApply),
  				1, 0);  				
  				
   }       
   else if (document.form.current_page.value == "Advanced_DMZIP_Content.asp")
   {
  	//alert(document.form.IPConnection_x_DNSServerEnable[0].checked);
  	//alert(document.form.IPConnection_x_DNSServerEnable[1].checked);
  	if (document.form.LANHostConfig_x_LANDHCPClient[0].checked == true)
    	{   
    		inputCtrl(document.form.LANHostConfig_IPRouters, 0);  
    		inputCtrl(document.form.LANHostConfig_SubnetMask, 0);   
    		inputCtrl(document.form.LANHostConfig_x_Gateway, 0);     
    	}	
    	else   
    	{    	
    		inputCtrl(document.form.LANHostConfig_IPRouters, 1);  
    		inputCtrl(document.form.LANHostConfig_SubnetMask, 1);   
    		inputCtrl(document.form.LANHostConfig_x_Gateway, 1);    
    	}	  	
   }
   else if (document.form.current_page.value == "Main_GStatus_Content.asp")
   {  	
   	if (document.form.PPPConnection_x_WANType.value != "Automatic IP")
   	{
   		document.form.PPPConnection_x_WANAction_button.disabled = 1;
   		document.form.PPPConnection_x_WANAction_button1.disabled = 1;  	
   	}	   
   }
       
   change = 0;
}

function unload_body()
{  
   return true;		   			
   //if (change==1 && !nav)
   //{
       //alert('This page has been changed, remember to press Save or Finish');   
       //alert(document.form.current_page.value);
       //location = document.form.current_page.value+"#Confirm";
       //document.form.action_mode.value = "  Save  ";
       //document.form.next_page.value = document.form.current_page.value;       	   
       //document.form.submit();         
   //}
   //return false;
}


function change_internet_firewall(r)
{
    if (r=="1")	      	    
    {
    	document.form.InternetFirewall_img.src = "graph/internet_some.gif";
    }	
    else
    {
    	document.form.InternetFirewall_img.src = "graph/internet_none.gif";
    }       
}

function change_wireless_firewall()
{
    if (window.top.isModel() == "WL600")	
    {
    	if (document.form.FirewallConfig_DmzEnable[0].checked == true )
    	{
    	      if (document.form.FirewallConfig_DmzDevices.value == "Both") 
    	         document.form.WirelessFirewall_img.src = "graph/wf_both.gif";	
    	      else if (document.form.FirewallConfig_DmzDevices.value == "802.11a only") 
    	         document.form.WirelessFirewall_img.src = "graph/wf_a.gif";	
    	      else if (document.form.FirewallConfig_DmzDevices.value == "802.11g only") 
    	         document.form.WirelessFirewall_img.src = "graph/wf_g.gif";	
    	}	
    	else  document.form.WirelessFirewall_img.src = "graph/wf_none.gif";	
    }
    else
    {
    	if (document.form.FirewallConfig_DmzEnable[0].checked == true)
    	{
    	    document.form.WirelessFirewall_img.src = "graph/wf_g.gif";		
    	}
    	else document.form.WirelessFirewall_img.src = "graph/wf_none.gif";	
    }		
}

function change_wireless_bridge(m, a, r, mflag)
{    
    if (a=="0" && r == "0" && mflag != 1)
    {
    	document.form.WLANConfig11b_x_APMode.value = "0";
    	m = "0";
    }
    
	
    if (m == "0")
    {
        wdsimage = "wds_ap";
        inputRCtrl2(document.form.WLANConfig11b_x_BRApply, 1);
        // WL501b, there is no BRestrict  	
        //inputRCtrl2(document.form.WLANConfig11b_x_BRestrict, 1);
        inputRCtrl1(document.form.WLANConfig11b_x_BRApply, 0);
        //inputRCtrl1(document.form.WLANConfig11b_x_BRestrict, 0);        
    }   
    else
    {    
    	// WL501b, there is no BRestrict  		        
        if (a=="0" && r == "0")
        {
            inputRCtrl2(document.form.WLANConfig11b_x_BRApply, 0);		
            //inputRCtrl2(document.form.WLANConfig11b_x_BRestrict, 1);            	
	}
	
	//inputRCtrl1(document.form.WLANConfig11b_x_BRestrict, 1);
        inputRCtrl1(document.form.WLANConfig11b_x_BRApply, 1);

        if (m == "1")	
  	   wdsimage = "wds_wds";
  	else
  	   wdsimage = "wds_mixed";
  	   	
  	if (a == "0")
        {
           if (r == "0")  	 
  	     wdsimage += "_connect";               	
  	   else  
  	     wdsimage += "_anony";
        }
        else
        {
            if (r == "0")            
  	     wdsimage += "_connect";   
  	    else 
  	     wdsimage += "_both";
  	    
	}
		  	
	if (document.form.WLANConfig11b_Channel.value == "0") 
	{
	     alert("Please set a fixed channel for WDS!!!");
	     document.form.WLANConfig11b_Channel.options[0].selected = 0;
	     document.form.WLANConfig11b_Channel.options[1].selected = 1;	     
	}   
	
	//alert(document.form.WLANConfig11b_Channel.options[0].value);	
	//if (document.form.WLANConfig11b_Channel.options[0].value == "0")
	//{
	//	document.form.WLANConfig11b_Channel.options[0].value = null;
	//	document.form.WLANConfig11b_Channel.options[0] = null;	
	//}
	   
    }	    	  	       	   
    wdsimage = "graph/" + wdsimage + ".gif";
//    document.form.WirelessBridge_img.src = wdsimage;
}

function onSubmit()
{
   change = 0;	   
   window.top.pageChanged = 0;
   window.top.pageChangedCount = 0;
   
   if (document.form.current_page.value == "Advanced_ACL_Content.asp")
   {
   	if (window.top.isMode() == "AP")
   	   document.form.next_page.value = "Advanced_APLAN_Content.asp";
   	else if (window.top.isMode() == "Router")
   	   document.form.next_page.value = "Advanced_RLANWAN_Content.asp";      	
   }
   else if (document.form.current_page.value == "Advanced_LANWAN_Content.asp")
   {
        if (document.form.Layer3Forwarding_x_ConnectionType.value == "Static IP")
  	{
  	    if (document.form.IPConnection_ExternalIPAddress.value=="" ||   	     
  	        document.form.IPConnection_x_ExternalSubnetMask.value=="")
  	    {    
  	       alert("As you leave IP or subnet mask fields blank, connection type will be set as Automatic IP!");
  	       document.form.Layer3Forwarding_x_ConnectionType.value = "Automatic IP";
  	    }   
  	}  
  	checkSubnet();
   } 	
   else if (document.form.current_page.value == "Advanced_RLANWAN_Content.asp")
   {
   	checkSubnet();
   }
   else if (document.form.current_page.value == "Advanced_DMZIP_Content.asp")
   {
   	checkDmzSubnet();
   }   
   else if (document.form.current_page.value == "Advanced_Firewall_Content.asp")   		
   {
   	updateDateTime(document.form.current_page.value);   	
   }
   else if (document.form.current_page.value == "Advanced_LFirewall_Content.asp")   		
   {   	   
   	updateDateTime(document.form.current_page.value);   	      	
   }
   else if (document.form.current_page.value == "Advanced_URLFilter_Content.asp")   		
   {   	
   	updateDateTime(document.form.current_page.value);       	
   }
   else if (document.form.current_page.value == "Advanced_WebCam_Content.asp") 
   {   	             	
   	updateDateTime(document.form.current_page.value); 	        
   }
   else if (document.form.current_page.value == "Advanced_WAdvanced_Content.asp") 
   {   	             	
   	updateDateTime(document.form.current_page.value);
   } 
   else if (document.form.current_page.value == "Advanced_WMode_Content.asp") 
   {  
   	if (document.form.WLANConfig11b_x_APMode.value == "0")
   	{
   	   inputRCtrl1(document.form.WLANConfig11b_x_BRApply, 1);
   	   // WL501b, there is no BRestrict  	
           //inputRCtrl1(document.form.WLANConfig11b_x_BRestrict, 1);   	
   	   inputRCtrl2(document.form.WLANConfig11b_x_BRApply, 1);
           //inputRCtrl2(document.form.WLANConfig11b_x_BRestrict, 1);       
           //alert(rcheck(document.form.WLANConfig11b_x_BRApply));   
           //alert(rcheck(document.form.WLANConfig11b_x_BRestrict));   
        }         
   }     
   else if (document.form.current_page.value == "Advanced_Wireless_Content.asp") 
   {
   	// apply setting of key anyway   
   	inputCtrl(document.form.WLANConfig11b_x_Phrase, 1);
   	inputCtrl(document.form.WLANConfig11b_WEPKey1, 1);
    	inputCtrl(document.form.WLANConfig11b_WEPKey2, 1);  
    	inputCtrl(document.form.WLANConfig11b_WEPKey3, 1);     
    	inputCtrl(document.form.WLANConfig11b_WEPKey4, 1); 
    	inputCtrl(document.form.WLANConfig11b_WEPDefaultKey, 1);
   }
   //alert(parent.folderFrame.src);
   //document.form.next_page.value = "Advanced_LANWAN_Content.asp";
   // Ugly solution   
}

function onSubmitCtrl(o, s)
{      	
   //o.value = s;
   document.form.action_mode.value = s;
   onSubmit();
   return true;
}

function onSubmitCtrlOnly(o, s)
{
   //o.value = s;
   if (s!='Upload') document.form.action_mode.value = s;
   //onSubmit();
   return true;
}

function onSubmitApply(s)
{   
   window.top.pageChanged = 0;
   window.top.pageChangedCount = 0;	   
   	
   if (document.form.current_page.value == "Advanced_PortMapping_Content.asp")
   {      	   	
   	if (s == "0")
   	{
   	   action = document.form.IPConnection_MappedAction_0;
   	   local = document.form.IPConnection_MappedIP_0;
   	   port = document.form.IPConnection_MappedInPort_0;   	      	
   	   desc = document.form.IPConnection_MappedDescript_0;
   	}
   	else if (s == "1")
   	{
   	   action = document.form.IPConnection_MappedAction_1;
   	   local = document.form.IPConnection_MappedIP_1;
   	   port = document.form.IPConnection_MappedInPort_1;
   	   desc = document.form.IPConnection_MappedDescript_1;
   	}
   	else if (s == "2")
   	{
   	   action = document.form.IPConnection_MappedAction_2;
   	   local = document.form.IPConnection_MappedIP_2;
   	   port = document.form.IPConnection_MappedInPort_2;
   	   desc = document.form.IPConnection_MappedDescript_2;
   	}
   	else if (s == "3")
   	{
   	   action = document.form.IPConnection_MappedAction_3;
   	   local = document.form.IPConnection_MappedIP_3;
   	   port = document.form.IPConnection_MappedInPort_3;
   	   desc = document.form.IPConnection_MappedDescript_3;
   	}
   	else if (s == "4")
   	{
   	   action = document.form.IPConnection_MappedAction_4;
   	   local = document.form.IPConnection_MappedIP_4;
   	   port = document.form.IPConnection_MappedInPort_4; 
   	   desc = document.form.IPConnection_MappedDescript_4;  	      	
   	}
   	else if (s == "5")
   	{
   	   action = document.form.IPConnection_MappedAction_5;
   	   local = document.form.IPConnection_MappedIP_5;
   	   port = document.form.IPConnection_MappedInPort_5; 
   	   desc = document.form.IPConnection_MappedDescript_5;  	      	 
   	}
   	
   	if (action.value == "Set")
   	{
   	   if (!validate_ipaddr(local, "") || 
   	   	!validate_portrange(port, "")) 
   	   {	
   	   	return false;
   	   }	
   	   else if (local.value=="" || port.value == "")
   	   {
   		alert("As you leave IP or subnet mask fields blank, connection type will be set as Automatic IP!1");	
   	   	return false;	
   	   }	   	     	   
   	}
   	   	   	   	
   	document.form.action.value = action.value;   	
   	document.form.action_mode.value = action.value;
   	document.form.action_script.value = "portmapping.sh" + " " + action.value + " " + local.value + " " + port.value;
   	//document.form.submit();
   } 	
   else if (document.form.current_page.value == "Advanced_DDNS_Content.asp")
   {
   	document.form.action.value = document.form.LANHostConfig_x_DDNSStatus_button.value; 
   	document.form.action_mode.value = document.form.LANHostConfig_x_DDNSStatus_button.value;;
   	document.form.action_script.value = s;
   } 
   else if (document.form.current_page.value == "Advanced_USBStorage_Content.asp")
   {
   	document.form.action.value = document.form.PrinterStatus_x_FEject_button.value; 
   	document.form.action_mode.value = document.form.PrinterStatus_x_FEject_button.value;;
   	document.form.action_script.value = s;
   }
   else if (document.form.current_page.value == "Main_GStatus_Content.asp")
   {
   	document.form.action.value = "Update"; 
   	document.form.action_mode.value = "Update";
   	document.form.action_script.value = s;
   }
   else    
   {
   	document.form.action.value = "Update"; 
   	document.form.action_mode.value = "Update";
   	document.form.action_script.value = s;
   } 
   return true;
}   

function setup_script(s)
{
   if (document.form.current_page.value == "Advanced_ACL_Content.asp")
   {   	
   	document.form.action_script.value = s;      
   } 	
}

function change_common(o, s, v)
{
  change = 1;	
  window.top.pageChanged = 1;
	
  if (v == "AuthenticationMethod") /* Handle AuthenticationMethod Change */
  {     	         	
      if(s == "WLANConfig11a")
      {
      	 if (o.value != "Open System" && rcheck(document.form.WLANConfig11a_x_AESEnable)=="1")
      	 {
      	     rst = confirm("If you change it, AES will be disabled!");
      	     if (rst)
      	     {
      	        inputRCtrl2(document.form.WLANConfig11a_x_AESEnable, 1);
      	     }   
      	     else
      	     {
      	     	document.form.WLANConfig11a_AuthenticationMethod.value = "Open System";
      	        return false;   
      	     }   
      	 }   
      	       	       	       	       	  
         wepType = document.form.WLANConfig11a_WEPType;
         
         wepStr = find_option(wepType);
         cflag = 0;
         
         free_options(wepType);
         
         if (o.value != "Shared Key")         
            cflag += add_option_match(wepType, "None", wepStr);     
            
         if (o.value != "AES Key") 
         {               
            cflag += add_option_match(wepType, "64bits", wepStr);
            cflag += add_option_match(wepType, "128bits", wepStr);
            cflag += add_option_match(wepType, "152bits", wepStr);   
         
            if (o.value == "802.1x")     
               cflag += add_option_match(wepType, "FromServer", wepStr);   
         }      
         if (!cflag)
            change_wlweptype(o, s);	       	                                      
      }   
      else
      {      	       	 
      	 wepStr = find_option(document.form.WLANConfig11b_WEPType);
      	       	       	 
      	 changeWPA(wepStr, false);      	       	                       
      }                                                   	
  }  
  else if (v=="x_ConnectionType")
  {
  	if (o.value == "Static IP")
  	{
  	   inputCtrl(document.form.IPConnection_ExternalIPAddress, 1); 
  	   inputCtrl(document.form.IPConnection_x_ExternalSubnetMask, 1); 
  	   inputCtrl(document.form.IPConnection_x_ExternalGateway, 1); 
  	   inputCtrl(document.form.PPPConnection_UserName, 0); 
  	   inputCtrl(document.form.PPPConnection_Password, 0); 
  	   inputCtrl(document.form.PPPConnection_IdleDisconnectTime, 0);  	  	  
  	   inputCtrl(document.form.PPPConnection_x_PPPoEMTU, 0);  	
  	   inputCtrl(document.form.PPPConnection_x_PPPoEMRU, 0);  	  	
  	   inputRCtrl1(document.form.PPPConnection_x_PPPoERelay, 0);  	
  	   inputCtrl(document.form.PPPConnection_IdleDisconnectTime_check, 0); 
  	   inputCtrl(document.form.PPPConnection_x_ServiceName, 0);  	  	   
  	   inputCtrl(document.form.PPPConnection_x_AccessConcentrator, 0);  	
  	}
  	else if(o.value == "PPPoE")
  	{
  	   inputCtrl(document.form.IPConnection_ExternalIPAddress, 0); 
  	   inputCtrl(document.form.IPConnection_x_ExternalSubnetMask, 0); 
  	   inputCtrl(document.form.IPConnection_x_ExternalGateway, 0); 
  	   inputCtrl(document.form.PPPConnection_UserName, 1); 
  	   inputCtrl(document.form.PPPConnection_Password, 1); 	  	
  	   inputCtrl(document.form.PPPConnection_IdleDisconnectTime, 1);
  	   inputCtrl(document.form.PPPConnection_x_PPPoEMTU, 1);  	
  	   inputCtrl(document.form.PPPConnection_x_PPPoEMRU, 1);  	  	
  	   inputRCtrl1(document.form.PPPConnection_x_PPPoERelay, 1);
  	   inputCtrl(document.form.PPPConnection_IdleDisconnectTime_check, 1); 
  	   inputCtrl(document.form.PPPConnection_x_ServiceName, 1);  	  	   
  	   inputCtrl(document.form.PPPConnection_x_AccessConcentrator, 1);  	  	
  	}
  	else if(o.value == "PPTP")
  	{
  	   inputCtrl(document.form.IPConnection_ExternalIPAddress, 1); 
  	   inputCtrl(document.form.IPConnection_x_ExternalSubnetMask, 1); 
  	   inputCtrl(document.form.IPConnection_x_ExternalGateway, 1); 
  	   inputCtrl(document.form.PPPConnection_UserName, 1); 
  	   inputCtrl(document.form.PPPConnection_Password, 1); 	 
  	   inputCtrl(document.form.PPPConnection_IdleDisconnectTime, 1); 
  	   inputCtrl(document.form.PPPConnection_x_PPPoEMTU, 0);  	
  	   inputCtrl(document.form.PPPConnection_x_PPPoEMRU, 0);  	  	
  	   inputRCtrl1(document.form.PPPConnection_x_PPPoERelay, 0);
  	   inputCtrl(document.form.PPPConnection_IdleDisconnectTime_check, 0); 
  	   inputCtrl(document.form.PPPConnection_x_ServiceName, 0);  	  	   
  	   inputCtrl(document.form.PPPConnection_x_AccessConcentrator, 0);  	
  	}
  	else
  	{
  	   inputCtrl(document.form.IPConnection_ExternalIPAddress, 0); 
  	   inputCtrl(document.form.IPConnection_x_ExternalSubnetMask, 0); 
  	   inputCtrl(document.form.IPConnection_x_ExternalGateway, 0); 
  	   inputCtrl(document.form.PPPConnection_UserName, 0); 
  	   inputCtrl(document.form.PPPConnection_Password, 0); 	  	
  	   inputCtrl(document.form.PPPConnection_IdleDisconnectTime, 0);  
  	   inputCtrl(document.form.PPPConnection_x_PPPoEMTU, 0);  	
  	   inputCtrl(document.form.PPPConnection_x_PPPoEMRU, 0);  	  		  	
  	   inputRCtrl1(document.form.PPPConnection_x_PPPoERelay, 0);  	
  	   inputCtrl(document.form.PPPConnection_IdleDisconnectTime_check, 0); 
  	   inputCtrl(document.form.PPPConnection_x_ServiceName, 0);  	  	   
  	   inputCtrl(document.form.PPPConnection_x_AccessConcentrator, 0);  	
  	}
  }    
  else if (v=="x_APMode")
  {	  
  	// WL501b, there is no BRestrict  	 			
 	//change_wireless_bridge(o.value, rcheck(document.form.WLANConfig11b_x_BRApply), rcheck(document.form.WLANConfig11b_x_BRestrict), 1);
 	change_wireless_bridge(o.value, rcheck(document.form.WLANConfig11b_x_BRApply), 1, 1);
  }  
  else if (s == "FirewallConfig" && v=="DmzDevices")
  {
        change_wireless_firewall();
  }
  else if (s == "FirewallConfig" && v=="WanLanDefaultAct")
  {
  	if (o.value == "DROP")
           alert("Selecting DROP will drop all WAN to LAN packets except for those matched in filter table. Please use it carefully.");
  }
  else if (s == "FirewallConfig" && v=="LanWanDefaultAct")
  {
  	if (o.value == "DROP")
           alert("Selecting DROP will drop all LAN to WAN packets except for those matched in filter table. Please use it carefully.");        
  }   
  else if (s=="WLANConfig11b" && v=="x_Mode11g")
  {
  	//alert(document.form.IPConnection_x_DNSServerEnable[0].checked);
  	//alert(document.form.IPConnection_x_DNSServerEnable[1].checked);  	  	 	
    	RefreshRateSetList(document.form.WLANConfig11b_x_Mode11g.value, true);
  }  
  else if (s=="WLANConfig11b" && v=="Channel" && document.form.current_page.value=="Advanced_WMode_Content.asp")
  {
  	if (document.form.WLANConfig11b_x_APMode.value != "0" && document.form.WLANConfig11b_Channel.value == "0")
   	{
	     alert("Please set a fixed channel for WDS!!!");
	     document.form.WLANConfig11b_Channel.options[0].selected = 0;
	     document.form.WLANConfig11b_Channel.options[1].selected = 1;
  	}
  }
  return true;   	
}


function change_common_radio(o, s, v, r)
{
  change = 1;	
  window.top.pageChanged = 1;
    	   
  if (v=="x_DNSServerEnable")
  {
  	//alert(document.form.IPConnection_x_DNSServerEnable[0].checked);
  	//alert(document.form.IPConnection_x_DNSServerEnable[1].checked);
  	if (r == '1')
    	{   
    		inputCtrl(document.form.IPConnection_x_DNSServer1, 0);  
    		inputCtrl(document.form.IPConnection_x_DNSServer2, 0);    
    	}	
    	else   
    	{    	
    		inputCtrl(document.form.IPConnection_x_DNSServer1, 1);  
	    	inputCtrl(document.form.IPConnection_x_DNSServer2, 1);    
    	}	  	
  }  
  else if (v=="x_AESEnable")
  {
  	if (r == '1')
  	{
  	   if (document.form.WLANConfig11a_AuthenticationMethod.value!="Open System")
  	   {
  		rst = confirm("If you would like to enable AES unique, Authentication should be set as Open System!");
  		  		 
 		if (rst)  		  		   		 		   
  		    document.form.WLANConfig11a_AuthenticationMethod.value = "Open System";  		
  		else
  		{
  		    inputRCtrl2(document.form.WLANConfig11a_x_AESEnable, 1);	
  		    return false;	
  		}    
           }
           else
           {
           	if (document.form.WLANConfig11a_WEPType.value == "None")
           	{
           	    rst = confirm("If you would like to enable AES unique, WEP should be enabled too!");
           	      		 
 		    if (rst)  		  		   		 
 		    {		   
  		       document.form.WLANConfig11a_WEPType.value = "64bits";
  		       change_wlweptype(document.form.WLANConfig11a_WEPType, "WLANConfig11a");	
  		    }   
  		    else
  		    {
  		       inputRCtrl2(document.form.WLANConfig11a_x_AESEnable, 1);	
  		       return false;	
  		    }   			
           	} 
           } 		
  		
  	}
  	
  	
  } 
  //else if (v=="x_OutdoorChannel")
  //{
  	//alert(document.form.IPConnection_x_DNSServerEnable[0].checked);
  	//alert(document.form.IPConnection_x_DNSServerEnable[1].checked);
  // 	if (r == '1')
  //  	{   
  //  		RefreshChannelList("1", rcheck(document.form.WLANConfig11a_x_TurboMode));      		
  //  	}	
  //  	else   
  //  	{    	
  //  		RefreshChannelList("0", rcheck(document.form.WLANConfig11a_x_TurboMode));    		  
  //  	}	  	
  //}  
  //else if (v=="x_TurboMode")
  //{
  // 	//alert(document.form.IPConnection_x_DNSServerEnable[0].checked);
  //	//alert(document.form.IPConnection_x_DNSServerEnable[1].checked);
  // 	if (r == '1')
  //  	{   
  //  		RefreshChannelList(rcheck(document.form.WLANConfig11a_x_OutdoorChannel), "1");
  //  		RefreshRateList("1"); 
  //  	}	
  //  	else   
  //  	{    	
  //  		RefreshChannelList(rcheck(document.form.WLANConfig11a_x_OutdoorChannel), "0");
  //  		RefreshRateList("0");
  //  	}	  	  
  //}  
  else if (s=="WLANConfig11a" && v=="DataRate")
  {
  	//alert(document.form.IPConnection_x_DNSServerEnable[0].checked);
  	//alert(document.form.IPConnection_x_DNSServerEnable[1].checked);  	
  	if (document.form.WLANConfig11a_DataRate_check.checked)
    	{   
    		document.form.WLANConfig11a_x_TurboMode.value = "1";
    		RefreshChannelList(document.form.WLANConfig11a_x_OutdoorChannel.value, document.form.WLANConfig11a_x_TurboMode.value);
    		RefreshRateList("1"); 
    	}	
    	else   
    	{    	
    		document.form.WLANConfig11a_x_TurboMode.value = "0";
    		RefreshChannelList(document.form.WLANConfig11a_x_OutdoorChannel.value, document.form.WLANConfig11a_x_TurboMode.value);		      
    		RefreshRateList("0"); 
    	}	  	
  }  
  else if (s=="WLANConfig11b" && v=="x_Mode11g")
  {
  	//alert(document.form.IPConnection_x_DNSServerEnable[0].checked);
  	//alert(document.form.IPConnection_x_DNSServerEnable[1].checked);  	
  	if (document.form.WLANConfig11b_x_Mode11g_check.checked)
    	{   
    		document.form.WLANConfig11b_x_GProtection.value = "1";    		
    	}	
    	else   
    	{    	
    		document.form.WLANConfig11b_x_GProtection.value = "0";    		
    	}	  	    
  }  
  else if (s=="WLANConfig11a" && v=="Channel")
  {
  	//alert(document.form.IPConnection_x_DNSServerEnable[0].checked);
  	//alert(document.form.IPConnection_x_DNSServerEnable[1].checked);
  	if (document.form.WLANConfig11a_Channel_check.checked)
    	{   
    		document.form.WLANConfig11a_x_OutdoorChannel.value = "1";
    		RefreshChannelList(document.form.WLANConfig11a_x_OutdoorChannel.value, document.form.WLANConfig11a_x_TurboMode.value);
    	}	
    	else   
    	{    	
    		document.form.WLANConfig11a_x_OutdoorChannel.value = "0";   
    		RefreshChannelList(document.form.WLANConfig11a_x_OutdoorChannel.value, document.form.WLANConfig11a_x_TurboMode.value);
    	}	  	  
  }
  else if (v=="x_LANDHCPClient")
  {
  	//alert(document.form.IPConnection_x_DNSServerEnable[0].checked);
  	//alert(document.form.IPConnection_x_DNSServerEnable[1].checked);
  	if (r == '1')
    	{   
    		inputCtrl(document.form.LANHostConfig_IPRouters, 0);  
    		inputCtrl(document.form.LANHostConfig_SubnetMask, 0);   
    		inputCtrl(document.form.LANHostConfig_x_Gateway, 0);     
    	}	
    	else   
    	{    	
    		inputCtrl(document.form.LANHostConfig_IPRouters, 1);  
    		inputCtrl(document.form.LANHostConfig_SubnetMask, 1);   
    		inputCtrl(document.form.LANHostConfig_x_Gateway, 1);    
    	}	  	
  }      
  else if (s=='FirewallConfig' && v=='DmzEnable')
  {  	  	
  	change_wireless_firewall();
  } 
  else if (s=='FirewallConfig' && v=='WanLanFirewallEnable')
  {  	  	
        change_internet_firewall(r);
  } 
  else if (v=='x_BRApply')
  {
  	//change_wireless_bridge(document.form.WLANConfig11b_x_APMode.value, r, rcheck(document.form.WLANConfig11b_x_BRestrict), 0);
  	change_wireless_bridge(document.form.WLANConfig11b_x_APMode.value, r, 1, 0);
  }
  else if (v=='x_BRestrict')
  {
  	change_wireless_bridge(document.form.WLANConfig11b_x_APMode.value, rcheck(document.form.WLANConfig11b_x_BRApply), r, 0);
  }  
  else if (s=="PPPConnection" && v=="IdleDisconnectTime")
  {  	
  	if (document.form.PPPConnection_IdleDisconnectTime_check.checked)
    	{   
    		document.form.PPPConnection_x_IdleTxOnly.value = "1";
    	}	
    	else   
    	{    	
    		document.form.PPPConnection_x_IdleTxOnly.value = "0";
    	}	  	  
  }
  else if (s=="PPPConnection" && v=="x_IdleTime1")
  {  	
  	if (document.form.PPPConnection_x_IdleTime1_check.checked)
    	{   
    		document.form.PPPConnection_x_IdleTxOnly1.value = "1";
    	}	
    	else   
    	{    	
    		document.form.PPPConnection_x_IdleTxOnly1.value = "0";
    	}	  	  
  }
  else if (s=="PPPConnection" && v=="x_MultiPPPoEEnable1")
  {  	
  	if (document.form.PPPConnection_x_MultiPPPoEEnable1[0].checked == true)
  	{
  	        flag=1;	
  	}	
  	else
  	{
  		flag=0;
  	}
  	
  	inputCtrl(document.form.PPPConnection_x_UserName1, flag); 
  	inputCtrl(document.form.PPPConnection_x_Password1, flag); 
  	inputCtrl(document.form.PPPConnection_x_IdleTime1, flag);
  	inputCtrl(document.form.PPPConnection_x_IdleTime1_check, flag); 
  	inputCtrl(document.form.PPPConnection_x_PPPoEMTU1, flag);  	
  	inputCtrl(document.form.PPPConnection_x_PPPoEMRU1, flag);  	  	  	  	
  	inputCtrl(document.form.PPPConnection_x_ServiceName1, flag);  	  	   
  	inputCtrl(document.form.PPPConnection_x_AccessConcentrator1, flag);  	  	     	  	  	  		  	  
  }
  else if (s=="PPPConnection" && v=="x_IdleTime2")
  {  	
  	if (document.form.PPPConnection_x_IdleTime2_check.checked)
    	{   
    		document.form.PPPConnection_x_IdleTxOnly2.value = "1";
    	}	
    	else   
    	{    	
    		document.form.PPPConnection_x_IdleTxOnly2.value = "0";
    	}  	  
  }
  else if (s=="PPPConnection" && v=="x_MultiPPPoEEnable2")
  {  	
  	if (document.form.PPPConnection_x_MultiPPPoEEnable2[0].checked == true)
  	{
  	        flag=1;	
  	}	
  	else
  	{
  		flag=0;
  	}
  	
  	inputCtrl(document.form.PPPConnection_x_UserName2, flag); 
  	inputCtrl(document.form.PPPConnection_x_Password2, flag); 
  	inputCtrl(document.form.PPPConnection_x_IdleTime2, flag);
  	inputCtrl(document.form.PPPConnection_x_IdleTime2_check, flag); 
  	inputCtrl(document.form.PPPConnection_x_PPPoEMTU2, flag);  	
  	inputCtrl(document.form.PPPConnection_x_PPPoEMRU2, flag);  	  	  	  	
  	inputCtrl(document.form.PPPConnection_x_ServiceName2, flag);  	  	   
  	inputCtrl(document.form.PPPConnection_x_AccessConcentrator2, flag);  	  	     	  	  	  		  	  
  }
  return true; 	
}

function valid_WPAPSK(authType, o)
{
	if (authType.value=="2")
	{
		if (o.value.length>=64) 
		{
			o.value = o.value.substring(0, 63);			
			alert("Pre-shared key should be less than 64 characters!!!");
			//alert(o.value);
			return false;
		}	
	}
	return true;	
}

function encryptionType(authType, wepType)
{
      pflag = "1";      
      	
      if (authType.value == "1") //Shared Key
      {
          if (wepType.value == "0") wepLen = "64";
          else wepLen = "128";
      }	
      else if (authType.value == "2") //WPA-PSK only
      {
      	  wepLen = "0";      	  
      }
      else if (authType.value == "3") //WPA
      {
      	  wepLen = "0";
      	  pflag = "0";
      }
      else if (authType.value == "4") //Radius
      {
      	  if (wepType.value == "0") 
      	  {
      	  	wepLen = "0";
      	  	pflag = "0";
      	  }	
      	  else if (wepType.value == "1") wepLen = "64";
          else wepLen = "128";
      }
      else
      {
      	  if (wepType.value == "0")
      	  {
      	     wepLen = "0";
      	     pflag = "0";
      	  }   
      	  else if (wepType.value == "1") wepLen = "64";	
          else wepLen = "128";   
      }	
      
      return(pflag + wepLen);
}

function change_wlweptype(o, s)
{	
  change = 1;  
  window.top.pageChanged = 1;
          	
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
      is_wlphrase("WLANConfig11a", document.form.WLANConfig11a_x_Phrase);
      //document.form.WLANConfig11a_WEPKey1.value = wep;
      //document.form.WLANConfig11a_WEPKey2.value = wep;
      //document.form.WLANConfig11a_WEPKey3.value = wep;
      //document.form.WLANConfig11a_WEPKey4.value = wep;     
                              
      inputCtrl(document.form.WLANConfig11a_x_Phrase, flag); 
      inputCtrl(document.form.WLANConfig11a_WEPKey1, flag);        
      inputCtrl(document.form.WLANConfig11a_WEPKey2, flag);  
      inputCtrl(document.form.WLANConfig11a_WEPKey3, flag);     
      inputCtrl(document.form.WLANConfig11a_WEPKey4, flag);
      inputCtrl(document.form.WLANConfig11a_WEPDefaultKey,  flag);   
            
  }	
  else if (s == "WLANConfig11b")
  {                    
      authType = document.form.WLANConfig11b_AuthenticationMethod;
      wepType = document.form.WLANConfig11b_WEPType;
      
      encStr = encryptionType(authType, wepType);                 
      pflag = encStr.charAt(0);
      wepLen = encStr.substring(1);
      document.form.WLANConfig11b_x_Phrase.value = "";
           	      
	      
      if (wepLen=="0") 
      {
         wflag = 0;
         wep = "";
         
         document.form.WLANConfig11b_WEPKey1.value = wep;
         document.form.WLANConfig11b_WEPKey2.value = wep;
         document.form.WLANConfig11b_WEPKey3.value = wep;
         document.form.WLANConfig11b_WEPKey4.value = wep;    
      }  
      else 
      {
         wflag = 1;
        
         if (wepLen=="64")
         {
      	    wep = "0000000000";      	 
      	 }   
      	 else  if (wepLen=="128")
      	 {
      	    wep = "00000000000000000000000000";
      	 }   
      	 else
      	 {
      	    wep = "00000000000000000000000000000000";
      	 }         	 
      	 is_wlphrase("WLANConfig11b", document.form.WLANConfig11b_x_Phrase);
      }                          
                    
      inputCtrl(document.form.WLANConfig11b_x_Phrase,  pflag); 
      inputCtrl(document.form.WLANConfig11b_WEPKey1,  wflag);        
      inputCtrl(document.form.WLANConfig11b_WEPKey2,  wflag);  
      inputCtrl(document.form.WLANConfig11b_WEPKey3,  wflag);     
      inputCtrl(document.form.WLANConfig11b_WEPKey4,  wflag); 
      inputCtrl(document.form.WLANConfig11b_WEPDefaultKey,  wflag);
      
      if (pflag=="1")
      {      	  
          document.form.WLANConfig11b_x_Phrase.focus();
      }    
  }	
  else
  {
 	return false;
  } 	
  
  return true;   	
}

function change_widzard(o, id)
{ 
  if (document.form.current_page.value == "Advanced_PortTrigger_Content.asp") 
  {
     for (i = 0; i < wItem.length; i++) 
     {     
         if (wItem[i][0]!=null)
         {   	
	     if (o.value == wItem[i][0])
	     {	     	 
	         optIdx = i;
	         	         
                 document.form.IPConnection_autofwOutPort_0.value = wItem[optIdx][1];
	         document.form.IPConnection_autofwOutProto_0.value = wItem[optIdx][2];
	         document.form.IPConnection_autofwInPort_0.value = wItem[optIdx][3];
	         document.form.IPConnection_autofwInProto_0.value = wItem[optIdx][4];
	         document.form.IPConnection_autofwDesc_0.value = wItem[optIdx][0]; 	         
	     }	    
         }   
     }	 
  }	  
  else if (document.form.current_page.value == "Advanced_PortMapping_Content.asp") 
  {
     for (i = 0; i < wItem.length; i++) 
     {     
         if (wItem[i][0]!=null)
         {   	
	     if (o.value == wItem[i][0])
	     {	     	 
	         optIdx = i;

		 if (document.form.IPConnection_MappedIP_0.value == "")
		 {
                    document.form.IPConnection_MappedIP_0.value = ip;	                	        	        	            	            
	            document.form.IPConnection_MappedInPort_0.value = wItem[optIdx][2];
	            document.form.IPConnection_MappedDescript_0.value = wItem[optIdx][0]; 	         
	         }   
	         else if (document.form.IPConnection_MappedIP_1.value == "")
		 {
                    document.form.IPConnection_MappedIP_1.value = ip;	                	        	        	            	            
	            document.form.IPConnection_MappedInPort_1.value = wItem[optIdx][2];
	            document.form.IPConnection_MappedDescript_1.value = wItem[optIdx][0]; 	         
	         }  
	         else if (document.form.IPConnection_MappedIP_2.value == "")
		 {
                    document.form.IPConnection_MappedIP_2.value = ip;	                	        	        	            	            
	            document.form.IPConnection_MappedInPort_2.value = wItem[optIdx][2];
	            document.form.IPConnection_MappedDescript_2.value = wItem[optIdx][0]; 	         
	         }  
	         else if (document.form.IPConnection_MappedIP_3.value == "")
		 {
                    document.form.IPConnection_MappedIP_3.value = ip;	                	        	        	            	            
	            document.form.IPConnection_MappedInPort_3.value = wItem[optIdx][2];
	            document.form.IPConnection_MappedDescript_3.value = wItem[optIdx][0]; 	         
	         }  
	         else if (document.form.IPConnection_MappedIP_4.value == "")
		 {
                    document.form.IPConnection_MappedIP_4.value = ip;	                	        	        	            	            
	            document.form.IPConnection_MappedInPort_4.value = wItem[optIdx][2];
	            document.form.IPConnection_MappedDescript_4.value = wItem[optIdx][0]; 	         
	         }  
	         else if (document.form.IPConnection_MappedIP_5.value == "")
		 {
                    document.form.IPConnection_MappedIP_5.value = ip;	                	        	        	            	            
	            document.form.IPConnection_MappedInPort_5.value = wItem[optIdx][2];
	            document.form.IPConnection_MappedDescript_5.value = wItem[optIdx][0]; 	         
	         }  
	         else	         
		 {
		    alert("The table only allow 6 items!");	                        
	         }  
	     }	    
         }   
     }	 
  }		  
  else if (document.form.current_page.value == "Advanced_VirtualServer_Content.asp") 
  {
     for (i = 0; i < wItem.length; i++) 
     {     
         if (wItem[i][0]!=null)
         {   	
	     if (o.value == wItem[i][0])
	     {	     	 
	         optIdx = i;
 	                	        	        
	         if (wItem[optIdx][2]=="TCP")
	            document.form.IPConnection_VServerProto_0.options[0].selected = 1;
	         else if (wItem[optIdx][2]=="UDP")
	            document.form.IPConnection_VServerProto_0.options[1].selected = 1;
	         else 
	            document.form.IPConnection_VServerProto_0.options[2].selected = 1;
	                     
   	         document.form.IPConnection_VServerIP_0.value = ip;   
	         document.form.IPConnection_VServerPort_0.value = wItem[optIdx][1];	 	               	         	           	         	         
	         document.form.IPConnection_VServerDescript_0.value = wItem[optIdx][0] + " Server (" + wItem[optIdx][1] + ")"; 	         
	     }	    
         }   
     }	 
  }	 
  else if (document.form.current_page.value == "Advanced_Firewall_Content.asp") 
  {
     for (i = 0; i < wItem.length; i++) 
     {     
         if (wItem[i][0]!=null)
         {   	
	     if (o.value == wItem[i][0])
	     {	     	 
	         optIdx = i;
 	                	        
 	         if ( id == "WLKnownApps")
 	         {       	        	        
	         	if (wItem[optIdx][2]=="TCP")
	            		document.form.FirewallConfig_WanLanProFlag_0.options[0].selected = 1;
	         	else if (wItem[optIdx][2]=="UDP")
	            		document.form.FirewallConfig_WanLanProFlag_0.options[8].selected = 1;	         	
   	            
		        document.form.FirewallConfig_WanLanDstPort_0.value = wItem[optIdx][1];
		 }
		 else // LWKnownApps
		 {
		 	if (wItem[optIdx][2]=="TCP")
	            		document.form.FirewallConfig_LanWanProFlag_0.options[0].selected = 1;
	         	else if (wItem[optIdx][2]=="UDP")
	            		document.form.FirewallConfig_LanWanProFlag_0.options[8].selected = 1;	         	
   	            
		        document.form.FirewallConfig_LanWanDstPort_0.value = wItem[optIdx][1];
		 }       
	     }	    
         }   
     }	 
  }	 	 	 	 
}




function is_wlkey(o, s) 
{	
if (!nav) keyPressed = IEKey();
else keyPressed = NSKey();
	
 if (keyPressed == 0) return true;
 
 window.top.pageChanged = 1;
 	   	
 if ((keyPressed>47 && keyPressed<58)||(keyPressed>64 && keyPressed<71)||(keyPressed>96 && keyPressed<103))
 {
  if (s == "WLANConfig11a")
  {     	
     	if (document.form.WLANConfig11a_WEPType.value == "64bits")
     	{
     	  if(o.value != "" && o.value.length > 10) return false;          
     	}	
     	else if (document.form.WLANConfig11a_WEPType.value == "128bits")
     	{
     	  if(o.value != "" && o.value.length > 26) return false;          
     	}	
     	else if (document.form.WLANConfig11a_WEPType.value == "152bits")
     	{
     	  if(o.value != "" && o.value.length > 32) return false;    
     	}	
     	else return false;
  }	
  else if (s == "WLANConfig11b")
  {
  	encStr = encryptionType(document.form.WLANConfig11b_AuthenticationMethod, document.form.WLANConfig11b_WEPType);
        wepLen = encStr.substring(1);
        
     	if (wepLen == "64")
     	{
     	  if(o.value != "" && o.value.length > 10) return false;           
     	}	
     	else if (wepLen == "128")
     	{
     	  if(o.value != "" && o.value.length > 26) return false;           
     	}	    
     	else return false;	  	
  }
  else if (s == "DeviceSecurity11a")
  {     	
     	if (document.form.DeviceSecurity11a_AESType_0.value == "64bits")
     	{
     	  if(o.value != "" && o.value.length > 10) return false;          
     	}	
     	else if (document.form.DeviceSecurity11a_AESType_0.value == "128bits")
     	{
     	  if(o.value != "" && o.value.length > 26) return false;          
     	}	
     	else if (document.form.DeviceSecurity11a_AESType_0.value == "152bits")
     	{
     	  if(o.value != "" && o.value.length > 32) return false;    
     	}	
     	else return false;
  }		
  else
  {
 	return false;
  } 	
 }
 else return false; 
 return true;
}

function change_wlkey(o, s) 
{
  if (s == "WLANConfig11a")
  {     	
     	if (document.form.WLANConfig11a_WEPType.value == "64bits")
     	{
     	   if(o.value.length > 10) o.value = o.value.substring(0, 10);          
     	}	
     	else if (document.form.WLANConfig11a_WEPType.value == "128bits")
     	{
     	   if(o.value.length > 26) o.value = o.value.substring(0, 26);         
     	}	
     	else if (document.form.WLANConfig11a_WEPType.value == "152bits")
     	{
     	  if(o.value.length > 32) o.value = o.value.substring(0, 32);
     	}	
     	else return false;
  }	
  else if (s == "WLANConfig11b")
  {
  	encStr = encryptionType(document.form.WLANConfig11b_AuthenticationMethod, document.form.WLANConfig11b_WEPType);                   
        wepLen = encStr.substring(1);
    
     	if (wepLen == "64")
     	{
     	  //alert(o.value.length);
     	  
     	  if(o.value.length > 10) o.value = o.value.substring(0, 10);           
     	}	
     	else if (wepLen == "128")
     	{
     	  if(o.value.length > 26) o.value = o.value.substring(0, 26);        
     	}	    
     	else return false;	
  }
  else if (s == "DeviceSecurity11a")
  {     	
     	if (document.form.DeviceSecurity11a_AESType_0.value == "64bits")
     	{
     	  if(o.value.length > 10) o.value = o.value.substring(0, 10);           
     	}	
     	else if (document.form.DeviceSecurity11a_AESType_0.value == "128bits")
     	{
     	  if(o.value.length > 26) o.value = o.value.substring(0, 26);         
     	}	
     	else if (document.form.DeviceSecurity11a_AESType_0.value == "152bits")
     	{
     	  if(o.value.length > 32) o.value = o.value.substring(0, 32);   
     	}	
     	else return false;
  }		
  else
  {
 	return false;
  } 	
 
  return true; 
}

function validate_timerange(o, p) 
{
  if (o.value.length==0) o.value = "00";
  else if (o.value.length==1) o.value = "0" + o.value; 	
  
  if (o.value.charAt(0)<'0' || o.value.charAt(0)>'9') o.value = "00";
  else if (o.value.charAt(1)<'0' || o.value.charAt(1)>'9') o.value = "00";  
  else if (p==0 || p==2)
  {     	
     if(o.value>23) o.value = "00";
  }   	
  else
  {
     if(o.value>59) o.value = "00";
  }
  return true;
}     	
     	

function validate_wlkey(o, s) 
{
  if (s == "WLANConfig11a")
  {     	
     if (document.form.WLANConfig11a_WEPType.value == "64bits")
     {
     	  if(o.value != "" && o.value.length == 10) return;          
     }	
     else if (document.form.WLANConfig11a_WEPType.value == "128bits")
     {
     	  if(o.value != "" && o.value.length == 26) return;          
     }	
     else if (document.form.WLANConfig11a_WEPType.value == "152bits")
     {
     	  if(o.value != "" && o.value.length == 32) return;    
     }	
     else 
     {     	
        return;
     }   
  }	
  else if (s == "WLANConfig11b")
  {
     encStr = encryptionType(authType, wepType);                   
     wepLen = encStr.substring(1);
    	
     if (wepLen == "64")
     {
     	  if(o.value != "" && o.value.length == 10) return;          
     }	
     else if (wepLen == "128")
     {
     	  if(o.value != "" && o.value.length == 26) return;          
     }	    
     else return;	
  }
  else if (s == "DeviceSecurity11a")
  {     	
     if (document.form.DeviceSecurity11a_AESType_0.value == "64bits")
     {
          if(o.value != "" && o.value.length == 10) return;          
     }	
     else if (document.form.DeviceSecurity11a_AESType_0.value == "128bits")
     {
          if(o.value != "" && o.value.length == 26) return;          
     }	
     else if (document.form.DeviceSecurity11a_AESType_0.value == "152bits")
     {
          if(o.value != "" && o.value.length == 32) return;    
     }	
     else return;
  }	
 
  alert('Please enter the complete WEP key.'); 
  return false;  
}

/* Factory Reset Warning */
function confirmRestore(){
	if(confirm('WARNING:\nAll your settings will be lost!\nProceed or not?')) {		
		/* top.location.href="apply.cgi";*/
		return true;
	}
	else 
	{
	   top.location.href = "Advanced_Content.html";
	   return false;
	}   
}

/*
function is_wlphrase(o) 
{          
    var pseed = new Array(4);
    var wep_key = new Array(8);
	           
    if(document.form.WLANConfig11a_WEPType.value!='None')    
    {       	 	
        for(i=0; i<o.value.length; i++) {
            pseed[i%4]^= o.value.charAt(i);
        }
       
        randNumber = pseed[0]|(pseed[1]<<8)|(pseed[2]<<16)|(pseed[3]<<24);          
        document.form.WLANConfig11a_WEPKey1.value = '';  
        
        for (j=0; j<5; j++) 
        {
            randNumber *= 0x343fd;
            randNumber += 0x269ec3;                
            keystr = ((randNumber>>8) & 0xff);
            
            if (keystr>10)
            {
            	if (keystr=10) keystr='A';
            	else if (keystr=11) keystr='B';
            	else if (keystr=12) keystr='C';
            	else if (keystr=13) keystr='D';
            	else if (keystr=14) keystr='E';
            	else if (keystr=15) keystr='F';            	            	
            }
            document.form.WLANConfig11a_WEPKey1.value += keystr;   
            
            keystr = ((randNumber>>16) & 0x7f);
            
            if (keystr>10)
            {
            	if (keystr=10) keystr='A';
            	else if (keystr=11) keystr='B';
            	else if (keystr=12) keystr='C';
            	else if (keystr=13) keystr='D';
            	else if (keystr=14) keystr='E';
            	else if (keystr=15) keystr='F';            	            	
            }
            document.form.WLANConfig11a_WEPKey1.value += keystr;
            
        }                     	             
    }         
}
*/

function validate_wlphrase(s, o)
{   	
   if (!validate_string(o)) 
   {
   	is_wlphrase(s, o);
   	return(false); 
   }	
   	
   if (s == "WLANConfig11b")
   {
   	if (document.form.WLANConfig11b_AuthenticationMethod.value == "2")
   	{   	   
   	   if (o.value.length < 8)
   	   {	
   	      alert("Pre-shared key should be more than 7 characters!!! If you leave this field blank, system will assign 00000000 as your passphrase.");
   	      
   	      document.form.WLANConfig11b_x_Phrase.value = "00000000";   	         	   	
   	      return false;
   	   }   
        }   	
  
   }
   return true;	
}


function add_portmapping()
{
	
    widzard = window.open("Advanced_PortForwarding_Widzard.asp", "PortForwarding_Widzard",
                "toolbar=no,location=no,directories=no,status=no,menubar=no,scrollbars=yes,resizable=no,copyhistory=no,width=600,height=400");
    if (!widzard.opener) widzard.opener = self;     
}

function add_vsmapping()
{
	
    widzard = window.open("Advanced_VirtualServer_Widzard.asp", "VirtualServer_Widzard",
                "toolbar=no,location=no,directories=no,status=no,menubar=no,scrollbars=yes,resizable=no,copyhistory=no,width=600,height=400");
    if (!widzard.opener) widzard.opener = self;     
}

function openHelp(o, name)
{	
    // So Help page will be named as XXXXXX_Widzard.asp
    	
    //urlstr = name + "Widzard.asp"
    urlstr = "Advanced_" + name + "_Widzard.asp"
       
    widzard = window.open(urlstr, "Help_Widzard",
                "toolbar=no,location=no,directories=no,status=no,menubar=no,scrollbars=yes,resizable=no,copyhistory=no,width=600,height=500");
    if (!widzard.opener) widzard.opener = self;     
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
        //if (!matchSubnet(document.form.LANHostConfig_x_LDNSServer2.value, document.form.LANHostConfig_MinAddress.value, 3) ||
        //    !matchSubnet(document.form.LANHostConfig_x_LDNSServer2.value, document.form.LANHostConfig_MaxAddress.value, 3))
        //{           	         	        
        //    document.form.LANHostConfig_MinAddress.value = subnetPrefix(document.form.LANHostConfig_x_LDNSServer2.value, document.form.LANHostConfig_MinAddress.value, 3);                  					
        //    document.form.LANHostConfig_MaxAddress.value = subnetPrefix(document.form.LANHostConfig_x_LDNSServer2.value, document.form.LANHostConfig_MaxAddress.value, 3);                    							      
        //}	
        if (!matchSubnet(document.form.LANHostConfig_IPRouters.value, document.form.LANHostConfig_MinAddress.value, 3) ||
            !matchSubnet(document.form.LANHostConfig_IPRouters.value, document.form.LANHostConfig_MaxAddress.value, 3))
        {   
            if (confirm("The setting of DHCP server does not match current IP address of LAN. Would you like to change it automatically?"))
            {	        	         	        
               document.form.LANHostConfig_MinAddress.value = subnetPrefix(document.form.LANHostConfig_IPRouters.value, document.form.LANHostConfig_MinAddress.value, 3);
               document.form.LANHostConfig_MaxAddress.value = subnetPrefix(document.form.LANHostConfig_IPRouters.value, document.form.LANHostConfig_MaxAddress.value, 3);
            }   
        }	
}

function checkDmzSubnet()
{                                                     
        /* Rule : If addresses in pool are match to subnet, don't change */
        /* Rule : If addresses in pool are not match to subnet, change to subnet.2~subnet.254 */
        //if (!matchSubnet(document.form.LANHostConfig_x_DmzLDNSServer2.value, document.form.LANHostConfig_DmzMinAddress.value, 3) ||
        //    !matchSubnet(document.form.LANHostConfig_x_DmzLDNSServer2.value, document.form.LANHostConfig_DmzMaxAddress.value, 3))
        //{           	         	        
        //    document.form.LANHostConfig_DmzMinAddress.value = subnetPrefix(document.form.LANHostConfig_x_DmzLDNSServer2.value, document.form.LANHostConfig_DmzMinAddress.value, 3);                  					
        //    document.form.LANHostConfig_DmzMaxAddress.value = subnetPrefix(document.form.LANHostConfig_x_DmzLDNSServer2.value, document.form.LANHostConfig_DmzMaxAddress.value, 3);                    							      
        //}	
        if (!matchSubnet(document.form.FirewallConfig_DmzIP.value, document.form.LANHostConfig_DmzMinAddress.value, 3) ||
            !matchSubnet(document.form.FirewallConfig_DmzIP.value, document.form.LANHostConfig_DmzMaxAddress.value, 3))
        {   
            if (confirm("The setting of DHCP server does not match current IP address of Wireless Firewall. Would you like to change it automatically?"))
            {	        	         	        
               document.form.LANHostConfig_DmzMinAddress.value = subnetPrefix(document.form.FirewallConfig_DmzIP.value, document.form.LANHostConfig_DmzMinAddress.value, 3);                  					
               document.form.LANHostConfig_DmzMaxAddress.value = subnetPrefix(document.form.FirewallConfig_DmzIP.value, document.form.LANHostConfig_DmzMaxAddress.value, 3);                    							      
            }   
        }
}

function updateDateTime(s)
{
   if (s == "Advanced_Firewall_Content.asp")   		
   {   	
   	document.form.FirewallConfig_WanLanActiveDate.value = setDateCheck(
   	   document.form.FirewallConfig_WanLanActiveDate_Sun,
   	   document.form.FirewallConfig_WanLanActiveDate_Mon,
   	   document.form.FirewallConfig_WanLanActiveDate_Tue,
   	   document.form.FirewallConfig_WanLanActiveDate_Wed,
   	   document.form.FirewallConfig_WanLanActiveDate_Thu,
   	   document.form.FirewallConfig_WanLanActiveDate_Fri,
   	   document.form.FirewallConfig_WanLanActiveDate_Sat);
   	
   	document.form.FirewallConfig_WanLanActiveTime.value = setTimeRange(
   	   document.form.FirewallConfig_WanLanActiveTime_starthour,
   	   document.form.FirewallConfig_WanLanActiveTime_startmin,
   	   document.form.FirewallConfig_WanLanActiveTime_endhour,
   	   document.form.FirewallConfig_WanLanActiveTime_endmin);
   	   
   	document.form.FirewallConfig_LanWanActiveDate.value = setDateCheck(
   	   document.form.FirewallConfig_LanWanActiveDate_Sun,
   	   document.form.FirewallConfig_LanWanActiveDate_Mon,
   	   document.form.FirewallConfig_LanWanActiveDate_Tue,
   	   document.form.FirewallConfig_LanWanActiveDate_Wed,
   	   document.form.FirewallConfig_LanWanActiveDate_Thu,
   	   document.form.FirewallConfig_LanWanActiveDate_Fri,
   	   document.form.FirewallConfig_LanWanActiveDate_Sat);
   	
   	document.form.FirewallConfig_LanWanActiveTime.value = setTimeRange(
   	   document.form.FirewallConfig_LanWanActiveTime_starthour,
   	   document.form.FirewallConfig_LanWanActiveTime_startmin,
   	   document.form.FirewallConfig_LanWanActiveTime_endhour,
   	   document.form.FirewallConfig_LanWanActiveTime_endmin);   
   }
   else if (s == "Advanced_LFirewall_Content.asp")   		
   {
   	document.form.FirewallConfig_WanLocalActiveDate.value = setDateCheck(
   	   document.form.FirewallConfig_WanLocalActiveDate_Sun,
   	   document.form.FirewallConfig_WanLocalActiveDate_Mon,
   	   document.form.FirewallConfig_WanLocalActiveDate_Tue,
   	   document.form.FirewallConfig_WanLocalActiveDate_Wed,
   	   document.form.FirewallConfig_WanLocalActiveDate_Thu,
   	   document.form.FirewallConfig_WanLocalActiveDate_Fri,
   	   document.form.FirewallConfig_WanLocalActiveDate_Sat);
   	
   	document.form.FirewallConfig_WanLocalActiveTime.value = setTimeRange(
   	   document.form.FirewallConfig_WanLocalActiveTime_starthour,
   	   document.form.FirewallConfig_WanLocalActiveTime_startmin,
   	   document.form.FirewallConfig_WanLocalActiveTime_endhour,
   	   document.form.FirewallConfig_WanLocalActiveTime_endmin);   	      	
   }
   else if (s == "Advanced_URLFilter_Content.asp")   		
   {
   	document.form.FirewallConfig_URLActiveDate.value = setDateCheck(
   	   document.form.FirewallConfig_URLActiveDate_Sun,
   	   document.form.FirewallConfig_URLActiveDate_Mon,
   	   document.form.FirewallConfig_URLActiveDate_Tue,
   	   document.form.FirewallConfig_URLActiveDate_Wed,
   	   document.form.FirewallConfig_URLActiveDate_Thu,
   	   document.form.FirewallConfig_URLActiveDate_Fri,
   	   document.form.FirewallConfig_URLActiveDate_Sat);
   	
   	document.form.FirewallConfig_URLActiveTime.value = setTimeRange(
   	   document.form.FirewallConfig_URLActiveTime_starthour,
   	   document.form.FirewallConfig_URLActiveTime_startmin,
   	   document.form.FirewallConfig_URLActiveTime_endhour,
   	   document.form.FirewallConfig_URLActiveTime_endmin);   	      	
   }
   else if (s == "Advanced_WebCam_Content.asp") 
   {   	          
   	 document.form.PrinterStatus_x_WSecurityDate.value = setDateCheck(
   	   document.form.PrinterStatus_x_WSecurityDate_Sun,
   	   document.form.PrinterStatus_x_WSecurityDate_Mon,
   	   document.form.PrinterStatus_x_WSecurityDate_Tue,
   	   document.form.PrinterStatus_x_WSecurityDate_Wed,
   	   document.form.PrinterStatus_x_WSecurityDate_Thu,
   	   document.form.PrinterStatus_x_WSecurityDate_Fri,
   	   document.form.PrinterStatus_x_WSecurityDate_Sat);
   	
   	document.form.PrinterStatus_x_WSecurityTime.value = setTimeRange(
   	   document.form.PrinterStatus_x_WSecurityTime_starthour,
   	   document.form.PrinterStatus_x_WSecurityTime_startmin,
   	   document.form.PrinterStatus_x_WSecurityTime_endhour,
   	   document.form.PrinterStatus_x_WSecurityTime_endmin);   	        
   }
   else if (s == "Advanced_WAdvanced_Content.asp" && window.top.isModel()!="WL300")
   {   	          
   	 //document.form.WLANConfig11b_x_RadioEnableDate.value = setDateCheck(
   	 //  document.form.WLANConfig11b_x_RadioEnableDate_Sun,
   	 //  document.form.WLANConfig11b_x_RadioEnableDate_Mon,
   	 //  document.form.WLANConfig11b_x_RadioEnableDate_Tue,
   	 //  document.form.WLANConfig11b_x_RadioEnableDate_Wed,
   	 //  document.form.WLANConfig11b_x_RadioEnableDate_Thu,
   	 //  document.form.WLANConfig11b_x_RadioEnableDate_Fri,
   	 //  document.form.WLANConfig11b_x_RadioEnableDate_Sat);
   	
   	//document.form.WLANConfig11b_x_RadioEnableTime.value = setTimeRange(
   	//   document.form.WLANConfig11b_x_RadioEnableTime_starthour,
   	//   document.form.WLANConfig11b_x_RadioEnableTime_startmin,
   	//   document.form.WLANConfig11b_x_RadioEnableTime_endhour,
   	//   document.form.WLANConfig11b_x_RadioEnableTime_endmin);   	        
   } 
}

function startMenu(s)
{
	if (s=='Quick')
	{
	   document.form.action_mode.value = "Next";
           document.form.submit();  
        }
        else
        {
           widzard = window.open("Advanced_PrinterSetup_Widzard.asp", "Printer Setup Widzard",
                "toolbar=no,location=no,directories=no,status=no,menubar=no,scrollbars=yes,resizable=no,copyhistory=no,width=600,height=400");
    	   if (!widzard.opener) widzard.opener = self;   
        }   
}

function openWidzard(u, t)
{    
    url = '"' + u + '"';
    title = '"' + t + '"';
    widzard = window.open(u, "Widzard",
                "toolbar=no,location=no,directories=no,status=no,menubar=no,scrollbars=yes,resizable=no,copyhistory=no,width=600,height=400");
    if (!widzard.opener) widzard.opener = self;         
}

function openWidzardNone(u, t)
{    
}

function openLink(s)
{         
   if (s=='x_DDNSServer')
   {   
   	if (document.form.LANHostConfig_x_DDNSServer.value == 'WWW.DYNDNS.ORG')
   	    	//tourl = "https://members.dyndns.org/policy.shtml"
   	    	tourl = "https://www.dyndns.org/account/create.html"
    	else if (document.form.LANHostConfig_x_DDNSServer.value == 'WWW.ZONEEDIT.COM')
    		tourl = "https://www.zoneedit.com/signup.html"   	       	
    	else    	
   	    	tourl = "https://controlpanel.tzo.com/cgi-bin/tzopanel.exe"   	       	
   	    
   		link = window.open(tourl, "DDNSLink",
               "toolbar=yes,location=yes,directories=no,status=yes,menubar=yes,scrollbars=yes,resizable=yes,copyhistory=no,width=640,height=480");    
    }
    else if (s=='x_WImageSize')
    {
    	tourl = "Advanced_ShowTime_Widzard.asp"
    	link = window.open(tourl, "WebCamera", 
                "toolbar=yes,location=yes,directories=no,status=yes,menubar=yes,scrollbars=yes,resizable=yes,copyhistory=no,width=640,height=560");
    }
    else if (s=='x_WImageStatic')
    {    	   
    	tourl = "ShowWebCamPic.asp"
    	link = window.open(tourl, "WebCamera", 
                "toolbar=yes,location=yes,directories=no,status=yes,menubar=yes,scrollbars=yes,resizable=yes,copyhistory=no,width=640,height=560");
    }
    else if (s=='x_WRemote')
    {
    	tourl = "Advanced_RemoteControl_Widzard.asp"
    	
    	link = window.open(tourl, "RemoteMonitor",
                "toolbar=no,location=no,directories=no,status=no,menubar=yes,scrollbars=yes,resizable=yes,copyhistory=no,top=0,left=0,width=" + screen.width + ",height=" + screen.height);
    }
    else if (s=='x_FIsAnonymous' || s=='x_FIsSuperuser')
    {
    	urlstr = location.href;	
      	url = urlstr.indexOf("http://");
      	
      	//user = '<% nvram_x("General", "x_Username"); %>';
      	//LANIP= '<% nvram_x("lan.log", "LANIPAddress"); %>';      	      
      	port = document.form.PrinterStatus_x_FPort.value;
      	
      	if (url == -1) urlpref = LANIP;
      	else
      	{        
         	urlstr = urlstr.substring(7, urlstr.length);
         	url = urlstr.indexOf(":");
               
         	if (url!=-1)
         	{
	         	urlpref = urlstr.substring(0, url);
         	}
         	else
         	{
	         	url = urlstr.indexOf("/");
	        	if (url!=-1) urlpref = urlstr.substring(0, url);
	        	else urlpref = urlstr;
         	}
      	}   
      	if (s=='x_FIsAnonymous')
      	{
      	   user = 'anonymous';
      	   tourl = "ftp://" + urlpref;
   
      	}   
      	else
      	{
      	   user = 'admin';
      	   tourl = "ftp://" + user + "@" + urlpref;
      	}   
      	
      	if (port!=21) tourl = tourl + ":" + port;
      	
    	link = window.open(tourl, "FTPServer", 
                "toolbar=yes,location=yes,directories=no,status=yes,menubar=yes,scrollbars=yes,resizable=yes,copyhistory=no,width=640,height=560");
    }
       
    if (!link.opener) link.opener = self;
}


function blur_body()
{
    alert('Out of focus!!!');	
}


function changeWPA(wepStr, loadflag)
{
    // Start handling WPA option
    authType = document.form.WLANConfig11b_AuthenticationMethod;
    wepType = document.form.WLANConfig11b_WEPType;
    authStr = find_option(authType);    
    
    if (!loadflag)
    {
    	wepStr = "";
    }
                
    free_options(wepType);
         
    cflag = 0;
         
    if (authType.value == "1") // WPA-PSK or Shared
    {
       	cflag += add_option_match_x(wepType, "WEP-64bits", wepStr);
       	cflag += add_option_match_x(wepType, "WEP-128bits", wepStr);     
    }
    else if (authType.value == "2") // WPA-PSK
    {
    	if (window.top.isBand() == "b" )
    	{
    	   cflag += add_option_match_x(wepType, "TKIP", wepStr);       	   
    	}	    	
    	else
    	{	
       	   cflag += add_option_match_x(wepType, "TKIP", wepStr);
       	   cflag += add_option_match_x(wepType, "AES", wepStr);
       	   //cflag += add_option_match_x(wepType, "TKIP or AES", wepStr);
       	}   
    }
    else if (authType.value == "3") // WPA
    {
    	if (window.top.isBand() == "b" )
    	{
    	   cflag += add_option_match_x(wepType, "TKIP", wepStr);       	   
    	}	  
    	else
    	{
       	   cflag += add_option_match_x(wepType, "TKIP", wepStr);
       	   cflag += add_option_match_x(wepType, "AES", wepStr);
       	   //cflag += add_option_match_x(wepType, "TKIP or AES", wepStr);
       	}   
       	
       	if (!loadflag) alert('Selecting this option requires connection with Raidus Server. Remember to set up related parameters in "Wireless - Radius"');
    }
    else if (authType.value == "4") // Radius with 802.1x				
    {
      	cflag += add_option_match_x(wepType, "Auto", wepStr);
       	cflag += add_option_match_x(wepType, "WEP-64bits", wepStr);
       	cflag += add_option_match_x(wepType, "WEP-128bits", wepStr);       	
       	
        if (!loadflag) alert('Selecting this option requires connection with Raidus Server. Remember to set up related parameters in "Wireless - Radius"');
    }
    else if (authType.value == "0" ) // Open System or Shared 
    {         
       	cflag += add_option_match_x(wepType, "None", wepStr);
       	cflag += add_option_match_x(wepType, "WEP-64bits", wepStr);
       	cflag += add_option_match_x(wepType, "WEP-128bits", wepStr);
    }
               
    if (!cflag)
    {
 	wepType.options[0].selected = 1;
    }
    
    if (!loadflag)
    {
        change_wlweptype(document.form.WLANConfig11b_WEPType, "WLANConfig11b");
    }
    else
    {
    	      
        encStr = encryptionType(authType, wepType);           
        pflag = encStr.charAt(0);
        wepLen = encStr.substring(1);
           	      
	      
        if (wepLen=="0") 
        {
           wflag = 0;         
        }  
        else 
        {
           wflag = 1;                  
        }  
    	
    	inputCtrl(document.form.WLANConfig11b_x_Phrase,  pflag); 
        inputCtrl(document.form.WLANConfig11b_WEPKey1,  wflag);        
        inputCtrl(document.form.WLANConfig11b_WEPKey2,  wflag);  
        inputCtrl(document.form.WLANConfig11b_WEPKey3,  wflag);     
        inputCtrl(document.form.WLANConfig11b_WEPKey4,  wflag); 
        inputCtrl(document.form.WLANConfig11b_WEPDefaultKey,  wflag);    	                   
    }    
       
    // End handling WPA option
}
