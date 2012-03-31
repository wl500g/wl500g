/*general*/

//var nav;
var change;
var keyPressed;
var wItem;
var ip = "";
var chanList = 0;
var wep1, wep2, wep3, wep4;

function NavKey(event)
{
	window.top.pageChanged = 1;

	if (typeof(event)=='undefined') event = window.event;

	if (event.which == null)
		return event.keyCode;
	else if (event.which != 0) {
		if (typeof(event.charCode) != 'undefined') {
			if (event.charCode != 0)
				return event.which;
		} else if (event.which > 9) // Opera workaround
			return event.which;
	}
	return 0;
}


function inputCtrl(o, flag)
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

function inputCtrl2(o, flag)
{
	if (flag === 0)
	{
		o.readOnly = 1;
		o.style.backgroundColor = "gray";
	}
	else
	{
		o.readOnly = 0;
		o.style.backgroundColor = "white";
	}
}

function get_radio_value(o)
{
	for (var i=0; i < o.length; i++){
		if (o[i].checked)
			return o[i].value;
	}
	return -1;
}


function inputRCtrl1(o, flag)
{
	if (flag === 0)
	{
		o[0].disabled = 1;
		o[1].disabled = 1;
//		o.style.backgroundColor = "gray";
	}
	else
	{
		o[0].disabled = 0;
		o[1].disabled = 0;
//		o.style.backgroundColor = "white";
	}
}

function inputRCtrl2(o, flag)
{
	if (flag === 0)
	{
		o[0].checked = true;
		o[1].checked = false;
//		o.style.backgroundColor = "gray";
	}
	else
	{
		o[0].checked = false;
		o[1].checked = true;
//		o.style.backgroundColor = "white";
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
//		o.value = " Save ";
		document.form.action_mode.value = " Save ";
		return true;
	}
	alert('Two password strings mismatched !!!');
	return false;
}

function markGroup(o, s, c, b) {
	var bFlag, cFlag;

	var frm = document.form;
	frm.group_id.value = s;

	bFlag = 0;
	cFlag = 0;

	if (b == " Add ")
	{
//		Check if inserted item is validated
		if (s=='RBRList') // Wireless Bridge
		{
			if (frm.wl_wdsnum_x_0.value >= c) cFlag=1;
			else if (!validate_hwaddr(frm.wl_wdslist_x_0)) return false;
			else if (frm.wl_wdslist_x_0.value==="") bFlag=1;
			else if (!validate_duplicate(frm.RBRList_s, frm.wl_wdslist_x_0.value, 12, 0)) return false;
		}
		else if (s == 'ACLList') // Access Control List
		{
			if (frm.wl_macnum_x_0.value >= c) cFlag=1;
			else if (!validate_hwaddr(frm.wl_maclist_x_0)) return false;
			else if (frm.wl_maclist_x_0.value==="") bFlag=1;
			else if (!validate_duplicate(frm.ACLList_s, frm.wl_maclist_x_0.value, 12, 0)) return false;
		}
		else if (s=='ManualDHCPList')
		{
			if (frm.dhcp_staticnum_x_0.value >= c) cFlag=1;
			else if (!validate_hwaddr(frm.dhcp_staticmac_x_0) ||
					!validate_ipaddr(frm.dhcp_staticip_x_0, "")) return false;
			else if (frm.dhcp_staticmac_x_0.value==="" ||
					frm.dhcp_staticip_x_0.value==="") bFlag=1;
			else if (!validate_duplicate(frm.ManualDHCPList_s, frm.dhcp_staticmac_x_0.value, 12, 0)) return false;
			else if (!validate_duplicate(frm.ManualDHCPList_s, frm.dhcp_staticip_x_0.value, 15, 14)) return false;
		}
		else if (s=='PPPoERouteList')
		{
			if (frm.PPPConnection_PPPoERouteCount_0.value > c) cFlag=1;
			else if (!validate_ipaddr(frm.PPPConnection_x_PPPoEIP_0, "") ||
					!validate_range(frm.PPPConnection_x_PPPoEMask_0, 0, 32)) return false;
			else if (frm.PPPConnection_x_PPPoEIP_0.value==="" ||
					frm.PPPConnection_x_PPPoEMask_0.value==="") bFlag=1;
		}
		else if (s=='GWStatic')
		{
			if (frm.sr_num_x_0.value > c) cFlag=1;
			else if (!validate_ipaddr(frm.sr_ipaddr_x_0, "") ||
					!validate_ipaddr(frm.sr_netmask_x_0, "") ||
					!validate_ipaddr(frm.sr_gateway_x_0, "")) return false;
			else if (frm.sr_ipaddr_x_0.value==="" ||
					frm.sr_netmask_x_0.value==="" ||
					frm.sr_gateway_x_0.value==="") bFlag=1;
		}
		else if (s=='VSList')
		{
			if (window.top.isModel()!="WL520" && window.top.isModel()!="SnapAP")
			{
				if (frm.vts_num_x_0.value >= c) cFlag=1;
				else if (frm.vts_ipaddr_x_0.value==="") bFlag=1;
				if (!validate_ipaddr(frm.vts_ipaddr_x_0, "") ||
						!validate_portrange(frm.vts_port_x_0, "") ||
						!validate_range_sp(frm.vts_lport_x_0, 1, 65535)) return false;
				if (frm.vts_proto_x_0.value == "OTHER")
				{
					if (!validate_range(frm.vts_protono_x_0, 0, 255)) return false;
					else if (frm.vts_protono_x_0.value==="") bFlag=1;
				}
				else
				{
					if (frm.vts_port_x_0.value==="") bFlag=1;
				}
			}
			else
			{
				if (frm.vts_num_x_0.value >= c) cFlag=1;
				else if (frm.vts_ipaddr_x_0.value==="") bFlag=1;
				else
				{
					if (!validate_ipaddr(frm.vts_ipaddr_x_0, "") ||
							!validate_portrange(frm.vts_port_x_0, "")) return false;
					else if (frm.vts_port_x_0.value==="") bFlag=1;
				}
			}
		}
		else if (s=='TriggerList')
		{
			if (frm.autofw_num_x_0.value >= c) cFlag=1;
			else if (!validate_portrange(frm.autofw_inport_x_0, "") ||
					!validate_portrange(frm.autofw_outport_x_0, "")) return false;
			else if (frm.autofw_inport_x_0.value==="" ||
					frm.autofw_outport_x_0.value==="") bFlag=1;
		}
		else if (s=='WLFilterList')
		{
			if (frm.filter_wl_num_x_0.value >= c) cFlag=1;
			else if (!validate_iprange(frm.filter_wl_srcip_x_0, "") ||
					!validate_portrange(frm.filter_wl_srcport_x_0, "") ||
					!validate_iprange(frm.filter_wl_dstip_x_0, "") ||
					!validate_portrange(frm.filter_wl_dstport_x_0, "")) return false;
			else if (frm.filter_wl_srcip_x_0.value==="" &&
					frm.filter_wl_srcport_x_0.value==="" &&
					frm.filter_wl_dstip_x_0.value==="" &&
					frm.filter_wl_dstport_x_0.value==="") bFlag=1;
		}
		else if (s=='LWFilterList')
		{
			if (window.top.isModel()!="WL520" && window.top.isModel()!="SnapAP")
			{
				if (frm.filter_lw_num_x_0.value >= c) cFlag=1;
				else if (!validate_iprange(frm.filter_lw_srcip_x_0, "") ||
						!validate_portrange(frm.filter_lw_srcport_x_0, "") ||
						!validate_iprange(frm.filter_lw_dstip_x_0, "") ||
						!validate_portrange(frm.filter_lw_dstport_x_0, "")) return false;
				else if (frm.filter_lw_srcip_x_0.value==="" &&
						frm.filter_lw_srcport_x_0.value==="" &&
						frm.filter_lw_dstip_x_0.value==="" &&
						frm.filter_lw_dstport_x_0.value==="") bFlag=1;
			}
			else
			{
				if (frm.filter_lw_num_x_0.value >= c) cFlag=1;
				else if (!validate_iprange(frm.filter_lw_srcip_x_0, "") ||
						!validate_portrange(frm.filter_lw_dstport_x_0, "")) return false;
				else if (frm.filter_lw_srcip_x_0.value==="" &&
						frm.filter_lw_dstport_x_0.value==="") bFlag=1;
			}
		}
		else if (s=='UrlList')
		{
			if (frm.url_num_x_0.value >= c) cFlag=1;
			else if (frm.url_keyword_x_0.value==="") bFlag=1;
			else if (!validate_duplicate(frm.UrlList_s, frm.url_keyword_x_0.value, 32, 0)) return false;
		}
		else if (s=='x_FUserList')
		{
			if (frm.usb_ftpnum_x_0.value >= c) cFlag=1;
			else if (!validate_string(frm.usb_ftpusername_x_0) ||
					!validate_string(frm.usb_ftppasswd_x_0)) return false;
			else if (frm.usb_ftpusername_x_0.value==="" ||
					frm.usb_ftppasswd_x_0.value==="") bFlag=1;
			else if (!validate_duplicate(frm.x_FUserList_s, frm.usb_ftpusername_x_0.value, 16, 0)) return false;
		}
		else if (s=='x_FBanIPList')
		{
			if (frm.usb_bannum_x_0.value >= c) cFlag=1;
			else if (!validate_iprange(frm.usb_ftpbanip_x_0, "")) return false;
			else if (frm.usb_ftpbanip_x_0.value==="") bFlag=1;
			else if (!validate_duplicate(frm.x_FBanIPList_s, frm.usb_ftpbanip_x_0.value, 15, 0)) return false;
		}
		else if (s == 'x_ExportsList') // NFS Exports List
		{
			if (frm.usb_nfsnum_x_0.value >= c) cFlag=1;
			else if (!validate_string(frm.usb_nfslist_x_0)) return false;
			else if (frm.usb_nfslist_x_0.value==="") bFlag=1;
			else if (!validate_duplicate(frm.x_ExportsList_s, frm.usb_nfslist_x_0.value, 80, 0)) return false;
		}
		else if (s=='x_QRuleList')
		{
			if (frm.qos_rulenum_x_0.value >= c) cFlag=1;
			else if (!validate_iprange(frm.qos_ipaddr_x_0) ||
					!validate_range_sp(frm.qos_port_x_0, 1, 65535) ||
					!validate_range_sp(frm.qos_maxbw_x_0, 0, 1000000) ||
					!validate_range_sp(frm.qos_minbw_x_0, 0, 1000000)) return false;
			else if (frm.qos_maxbw_x_0.value==="" &&
					frm.qos_minbw_x_0.value==="") bFlag=1;
			else if (frm.qos_maxbw_x_0.value!=="" &&
					frm.qos_minbw_x_0.value!=="" &&
					eval(frm.qos_minbw_x_0.value) > eval(frm.qos_maxbw_x_0.value))
			{
				alert('Please enter a value between 0 to ' + frm.qos_maxbw_x_0.value + '.');
				frm.qos_minbw_x_0.focus();
				return false;
			}

//			if (frm.qos_maxbw_x_0.value=="")
//			frm.qos_maxbw_x_0.value==frm.qos_minbw_x_0.value;
//			if (frm.qos_minbw_x_0.value=="")
//			frm.qos_minbw_x_0.value==frm.qos_maxbw_x_0.value
//			else if (!validate_duplicate(frm.x_QRuleList_s,
//			frm.qos_ipaddr_x_0.value, 16, 0)) return false;
		}
		else if (s=='x_UQRuleList')
		{
			if (frm.qos_urulenum_x_0.value >= c) cFlag=1;
			else if (!validate_range_sp(frm.qos_uport_x_0, 1, 65535) ||
					!validate_range_sp(frm.qos_umaxbw_x_0, 0, 1000000) ||
					!validate_range_sp(frm.qos_uminbw_x_0, 0, 1000000)) return false;
			else if (frm.qos_umaxbw_x_0.value==="" &&
					frm.qos_uminbw_x_0.value==="") bFlag=1;
			else if (frm.qos_umaxbw_x_0.value!=="" &&
					frm.qos_uminbw_x_0.value!=="" &&
					eval(frm.qos_uminbw_x_0.value) > eval(frm.qos_umaxbw_x_0.value))
			{
				alert('Please enter a value between 0 to ' + frm.qos_umaxbw_x_0.value + '.');
				frm.qos_uminbw_x_0.focus();
				return false;
			}

//			if (frm.qos_umaxbw_x_0.value=="")
//			frm.qos_umaxbw_x_0.value==frm.qos_uminbw_x_0.value;
//			if (frm.qos_uminbw_x_0.value=="")
//			frm.qos_uminbw_x_0.value==frm.qos_umaxbw_x_0.value
//			else if (!validate_duplicate(frm.x_QRuleList_s,
//			frm.qos_ipaddr_x_0.value, 16, 0)) return false;
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
//		else if (s=='WLANAuthentication11a_LocalCertDB')
//		{
//		alert(doucment.form.WLANAuthentication11a
//		}

		window.top.pageChanged = 0;
		window.top.pageChangedCount = 0;
		o.value = b;
		frm.action_mode.value = b;
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
//	url = parent.folderFrame.location.href;
	parent.location.href = 'index.asp';
//	parent.location.reload();
//	parent.folderFrame.location.href = url;
//	parent.titleFrame.location.href='Title.asp';
//	parent.treeFrame.location.href='index.asp';
}

function entry_cmp(entry, match, len)
{
	var j;

	if(entry.length<match.length) return (1);

	for(j=0; j<entry.length && j<len; j++)
	{
		c1 = entry.charCodeAt(j);

		if (j>=match.length) c2=160;
		else c2 = match.charCodeAt(j);

		if (c1==160) c1 = 32;
		if (c2==160) c2 = 32;

//		alert(c1 + '<-> ' + c2);

		if (c1>c2) return (1);
		else if (c1<c2) return(-1);
	}
	return 0;
}

//check duplicated
function validate_duplicate(o, v, l, off)
{
	var i;

	for (i=0; i<o.options.length; i++)
	{
		if (entry_cmp(o.options[i].text.substring(off).toLowerCase(), v.toLowerCase(), l)===0)
		{
			alert('This entry has been in table!!!');
			return false;
		}
	}
	return true;
}

function is_hwaddr(event, o)
{

	keyPressed=NavKey(event);

	if ((keyPressed>47 && keyPressed<58)||(keyPressed>64 && keyPressed<71)||(keyPressed>96 && keyPressed<103))
		return true;
	else if (keyPressed===0)
		return true;
	else
		return false;
}

function validate_hwaddr(o)
{
	if (o.value.length === 0) return true;

	if(o.value !== "")
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

function is_string(event,o)
{
	keyPressed = NavKey(event);

	if (keyPressed===0) return true;
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
		if (inv!=="")
		{
			alert("This string can not contain " + inv +"!");
			o.value="";
			o.focus();
			return false;
		}
	}

	if (document.form.current_page.value == "Advanced_Wireless_Content.asp" &&
			document.form.wl_ssid.value === "")
	{
		o.value = "default";
	}
	return true;
}

function is_number(event, o)
{
	keyPressed = NavKey(event);

	if (keyPressed===0) return true;

	if (keyPressed>47 && keyPressed<58)
	{
		if (keyPressed==48 && o.length === 0) return false;
		return true;
	}
	else
	{
		return false;
	}
}

function validate_range(o, min, max)
{
	if (isNaN(o.value) || o.value<min || o.value>max)
	{
		alert('Please enter a value between ' + min + ' to ' + max + '.');
//		o.value = min;
		o.focus();
		return false;
	}
	return true;
}

function validate_range_sp(o, min, max)
{
	if (o.value.length===0) return true;

	return validate_range(o, min, max);
}

function validate_range_time(o, min, max)
{
	tt = min-1;

	c = o.value.charAt(o.value.length-1).toLowerCase();
	if (c<'0' || c>'9')
	{
		fac = 1;

		switch(c)
		{
			case 'd':
				fac *= 24;
				/* fall though */
			case 'h':
				fac *= 60;
				/* fall though */
			case 'm':
				fac *= 60;
				/* fall though */
			case 's':
				tt = o.value.substring(0, o.value.length-1) * fac;
		}
	}
	else tt = o.value;
	if (!isNaN(tt) && tt>=min && tt<=max)
		return true;

	alert('Please enter a value between ' + min + ' to ' + max + ' seconds.');
	o.focus();
	return false;
}

function change_ipaddr(o)
{
	/*
	 * j = 0;
	 *
	 * for(i=0; i<o.value.length; i++) { if (o.value.charAt(i)=='.') { j++; } }
	 *
	 * if (j<3 && i>=3) { if (o.value.charAt(i-3)!='.' &&
	 * o.value.charAt(i-2)!='.' && o.value.charAt(i-1)!='.') o.value = o.value +
	 * '.'; }
	 *
	 * return;
	 */
}

function is_ipaddr(event, o)
{
//	alert(e.which);
//	if (nav) keyPressed = e.which;
//	alert('NAV' + e.which);

	keyPressed=NavKey(event);


	if (keyPressed===0)
	{
		return true;
	}


	if (o.value.length>=16) return false;

	if ((keyPressed>47 && keyPressed<58))
	{
//		o.value = o.value + String.fromCharCode(keyPressed);
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

function intoa(ip)
{
	n=0;
	vip=0;

	for(i=0;i<ip.length;i++)
	{
		c = ip.charAt(i);

		if (c == '.')
		{
			vip = vip * 256 + n;
			n = 0;
		}
		else if(c>='0' && c<='9')
		{
			n = n*10 + (c-'0');
		}
	}
	vip = vip*256 + n;
	return(vip);
}

function validate_ipaddr(o, v)
{
	num = -1;
	pos = 0;

	if (o.value.length===0)
	{
		if (v=='dhcp_start' || v=='dhcp_end' || v=='wan_ipaddr' ||
		    v=='ipv6_sit_remote' || v=='ipv6_sit_relay' || v=='ipv6_6rd_router')
		{
			alert("Fields can't be blank!!!");
			o.focus();
		}
		return true;
	}

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

			if (pos===0) v1=num;
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

	o.value = v1 + "." + v2 + "." + v3 + "." + v4;

	if ((v1 > 0) && (v1 < 127)) mask = "255.0.0.0";
	else if ((v1 > 127) && (v1 < 192)) mask = "255.255.0.0";
	else if ((v1 > 191) && (v1 < 224)) mask = "255.255.255.0";
	else mask = "0.0.0.0";

//	if (v=='ExternalIPAddress' && document.form.wan_netmask.value == '')
	if (v=='wan_ipaddr' && document.form.wan_netmask.value==="")
	{
//		document.form.wan_netmask.value = "255.255.255.0";
		document.form.wan_netmask.value = mask;
	}
//	else if (v=='IPRouters' && document.form.lan_netmask.value === '')
	else if (v=='lan_ipaddr' && document.form.lan_netmask.value==="" )
	{
//		document.form.lan_netmask.value = "255.255.255.0";
		document.form.lan_netmask.value = mask;
	}
	else if (v=='dhcp_start')
	{
		if (intoa(o.value)>intoa(document.form.dhcp_end.value))
		{
			tmp = document.form.dhcp_start.value;
			document.form.dhcp_start.value = document.form.dhcp_end.value;
			document.form.dhcp_end.value = tmp;
		}
	}
	else if (v=='dhcp_end')
	{
		if (intoa(document.form.dhcp_start.value)>intoa(o.value))
		{
			tmp = document.form.dhcp_start.value;
			document.form.dhcp_start.value = document.form.dhcp_end.value;
			document.form.dhcp_end.value = tmp;
		}
	}
	return true;
}

function validate_ip6addr(o, v)
{
	// thanks http://www.intermapper.com
	var regex = /^((([0-9a-f]{1,4}:){7}([0-9a-f]{1,4}|:))|(([0-9a-f]{1,4}:){6}(:[0-9a-f]{1,4}|((25[0-5]|2[0-4]\d|1\d\d|[1-9]?\d)(\.(25[0-5]|2[0-4]\d|1\d\d|[1-9]?\d)){3})|:))|(([0-9a-f]{1,4}:){5}(((:[0-9a-f]{1,4}){1,2})|:((25[0-5]|2[0-4]\d|1\d\d|[1-9]?\d)(\.(25[0-5]|2[0-4]\d|1\d\d|[1-9]?\d)){3})|:))|(([0-9a-f]{1,4}:){4}(((:[0-9a-f]{1,4}){1,3})|((:[0-9a-f]{1,4})?:((25[0-5]|2[0-4]\d|1\d\d|[1-9]?\d)(\.(25[0-5]|2[0-4]\d|1\d\d|[1-9]?\d)){3}))|:))|(([0-9a-f]{1,4}:){3}(((:[0-9a-f]{1,4}){1,4})|((:[0-9a-f]{1,4}){0,2}:((25[0-5]|2[0-4]\d|1\d\d|[1-9]?\d)(\.(25[0-5]|2[0-4]\d|1\d\d|[1-9]?\d)){3}))|:))|(([0-9a-f]{1,4}:){2}(((:[0-9a-f]{1,4}){1,5})|((:[0-9a-f]{1,4}){0,3}:((25[0-5]|2[0-4]\d|1\d\d|[1-9]?\d)(\.(25[0-5]|2[0-4]\d|1\d\d|[1-9]?\d)){3}))|:))|(([0-9a-f]{1,4}:){1}(((:[0-9a-f]{1,4}){1,6})|((:[0-9a-f]{1,4}){0,4}:((25[0-5]|2[0-4]\d|1\d\d|[1-9]?\d)(\.(25[0-5]|2[0-4]\d|1\d\d|[1-9]?\d)){3}))|:))|(:(((:[0-9a-f]{1,4}){1,7})|((:[0-9a-f]{1,4}){0,5}:((25[0-5]|2[0-4]\d|1\d\d|[1-9]?\d)(\.(25[0-5]|2[0-4]\d|1\d\d|[1-9]?\d)){3}))|:)))(%.+)?$/i;

	if (o.value.length===0)
	{
		p=document.form.ipv6_proto.value;
		if ((v=='ipv6_lan_addr' && (p=="native" || p=="ppp" || p=="tun6in4")) ||
		    (v=='ipv6_wan_addr' && p=="tun6in4") ||
		    (v=='ipv6_wan_router' && p=="tun6in4"))
		{
			alert("Fields can't be blank!!!");
			o.focus();
		}
		return true;
	}

	if (!regex.test(o.value))
	{
		alert(o.value + ' is not a valid IPv6 address!');
		o.focus();
		return false;
	}
	return true;
}

function change_ipaddrport(o)
{
	/*
	 * j = 0;
	 *
	 * for(i=0; i<o.value.length; i++) { if (o.value.charAt(i)=='.') { j++; } }
	 *
	 * if (j<3 && i>=3) { if (o.value.charAt(i-3)!='.' &&
	 * o.value.charAt(i-2)!='.' && o.value.charAt(i-1)!='.') o.value = o.value +
	 * '.'; }
	 *
	 * return;
	 */
}


function is_ipaddrport(event, o)
{
//	alert(e.which);
//	if (nav) keyPressed = e.which;
//	alert('NAV' + e.which);

	keyPressed=NavKey(event);


	if (keyPressed===0)
	{
		return true;
	}

//	if (o.value.length>=15) return false;
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

	if (o.value.length===0)
		return true;

	str = o.value;
	portidx = str.indexOf(":");

	if (portidx!=-1)
	{
		port = str.substring(portidx+1);
		len = portidx;

		if (isNaN(port) || port<1 || port>65535)
		{
			alert(port + ' is not a valid port number!');
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

	if (v=='ExternalIPAddress' && document.form.wan_netmask.value === '')
	{
		document.form.wan_netmask.value = "255.255.255.0";
	}
	else if (v=='IPRouters' && document.form.lan_netmask.value === '')
	{
		document.form.lan_netmask.value = "255.255.255.0";
	}
	return true;
}

function change_iprange(o)
{
	/*
	 * j = 0;
	 *
	 * for(i=0; i<o.value.length; i++) { if (o.value.charAt(i)=='.') { j++; } }
	 *
	 * if (j<3 && i>=3) { if (o.value.charAt(i-3)!='.' &&
	 * o.value.charAt(i-2)!='.' && o.value.charAt(i-1)!='.') o.value = o.value +
	 * '.'; }
	 *
	 * return;
	 */
}

function is_iprange(event, o)
{
	keyPressed=NavKey(event);

	if (keyPressed===0)
	{
		return true;
	}

	if (o.value.length>=15) return false;

	if ((keyPressed>47 && keyPressed<58))
	{
//		o.value = o.value + String.fromCharCode(keyPressed);

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

	if (o.value.length===0)
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

	if (v=='ExternalIPAddress' && document.form.wan_netmask.value === '')
	{
		document.form.wan_netmask.value = "255.255.255.0";
	}
	else if (v=='IPRouters' && document.form.lan_netmask.value === '')
	{
		document.form.lan_netmask.value = "255.255.255.0";
	}
	return true;
}

function is_portrange(event, o)
{
	keyPressed=NavKey(event);

	if (keyPressed===0) return true;

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
		if (o.value.length===0)
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
	if (o.value.length===0)
		return true;

	prev = -1;
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
			if (num>65535 || num<1 || (c!=':' && c!='>' && c!='<'))
			{
				alert(o.value.substring(0, i+1) + ' is not a valid port number!');
//				o.value = "";
				o.focus();
				return false;
			}
			if (c=='>') prev = -2;
			else if (c=='<') prev = -3;
			else
			{
				prev = num;
				num = 0;
			}
		}
	}
	if ((num>65535 && prev!=-3) || (num<1&&prev!=-2) || (prev>num) || (num>=65535&&prev==-2) || (num<=1&&prev==-3))
	{
		if (num>65535)
			alert(num + ' is not a valid port number!');
		else
			alert(o.value + ' is not a valid port range!');
//		o.value = "";
		o.focus();
		return false;
	}
	else
	{
		if (prev==-2)
		{
			if (num==65535) o.value = num;
			else o.value = (num+1) + ":65535";
		}
		else if (prev==-3)
		{
			if (num==1) o.value = num;
			else o.value = "1:" + (num-1);
		}
		else if (prev!=-1) o.value = prev + ":" + num;
		else o.value = num;
	}
	return true;

}

function is_portlist(event, o)
{
	keyPressed = NavKey(event);

	if (keyPressed === 0 ) return true;

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
	if (o.value.length===0)
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

function add_option_text(o, t, s, f)
{
	tail = o.options.length;

	o.options[tail] = new Option(s);
	o.options[tail].text = t;
	o.options[tail].value = s;

	if (f==1)
	{
		o.options[tail].selected = 1;
	}
}

function add_option_match(o, s, f)
{
	tail = o.options.length;

	o.options[tail] = new Option(s);
	o.options[tail].value = s;

//	alert(f);
//	alert(s);

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

function add_options_x2(o, varr, arr, orig)
{
	for (i = 0; i < arr.length; i++)
	{
		if (orig == varr[i])
			add_option_x(o, varr[i], arr[i], 1);
		else
			add_option_x(o, varr[i], arr[i], 0);
	}
}

function add_options_x3(o, arr, orig)
{
	for (i = 0; i < arr.length; i++)
	{
		if (orig == arr[i].split("|")[0])
			add_option_x(o, arr[i].split("|")[0], arr[i].split("|")[1], 1);
		else
			add_option_x(o, arr[i].split("|")[0], arr[i].split("|")[1], 0);
	}
}

function rcheck(o)
{
	if (o[0].checked === true)
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
	var orig = document.form.WLANConfig11a_Channel.value;

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
	var orig = document.form.WLANConfig11a_DataRate.value;

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
	var orig = document.form.WLANConfig11b_DataRate.value;

	free_options(document.form.WLANConfig11b_DataRate);

//	gmode :
//	0 : Auto
//	1 : G only
//	2 : LRS
//	3 : B only
	if (gmode != "3")
	{
		if (gmode == "1")
		{
			items = new Array("1 & 2 Mbps", "Default", "All");
		}
		else
		{
			items = new Array("1 & 2 Mbps", "Default", "All");
		}

		if (chg) orig = 1;
	}
	else
	{
		items = new Array("1 & 2 Mbps", "Default");

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
	if (pos === 0)
		return str.substring(0,2);
	else if (pos == 1)
		return str.substring(2,4);
	else if (pos == 2)
		return str.substring(4,6);
	else if (pos == 3)
		return str.substring(6,8);
}

function TimeZoneList()
{
	var TimeZone = new Array(
			"UCT11|(GMT-11) Midway Island, Samoa",
			"HST10|(GMT-10) Honolulu",
			"AKST9AKDT,M3.2.0,M11.1.0|(GMT-9DST) Anchorage",
			"PST8PDT,M3.2.0,M11.1.0|(GMT-8DST) Los Angeles",
			"MST7MDT,M3.2.0,M11.1.0|(GMT-7DST) Denver",
			"MST7|(GMT-7) Phoenix",
			"CST6CDT,M3.2.0,M11.1.0|(GMT-6DST) Chicago",
			"CST6|(GMT-6) San Salvador",
			"EST5EDT,M3.2.0,M11.1.0|(GMT-5DST) New York",
			"EST5|(GMT-5) Bogota,Lima",
			"UCT4:30|(GMT-4:30) Caracas",
			"UCT4|(GMT-4) La Paz",
			"UCT3|(GMT-3) Cayenne,Paramaribo",
			"UCT2|(GMT-2) Mid-Atlantic",
			"AZOT1AZOST,M3.5.0/0,M10.5.0/1|(GMT-1DST) Ponta Delgada",
			"GMT0BST,M3.5.0/1,M10.5.0|(GMTDST) Dublin,Lisbon,London",
			"UCT|(GMT) Monrovia,Rabat",
			"CET-1CEST,M3.5.0,M10.5.0/3|(GMT+1DST) Amsterdam,Berlin,Madrid,Paris,Rome",
			"UCT-1|(GMT+1) Algiers",
			"EET-2EEST,M3.5.0/3,M10.5.0/4|(GMT+2DST) Athens,Helsinki,Kiev",
			"EET-2EEST,M3.5.0,M10.5.0/3|(GMT+2DST) Minsk",
			"UCT-2|(GMT+2) Jerusalem,Pretoria",
			"KALT-3|(GMT+3) Kaliningrad",
			"AST-3|(GMT+3) Aden,Bahrain,Kuwait,Qatar,Riyadh",
			"IRST-3:30|(GMT+3:30) Tehran",
			"MSK-4|(GMT+4) Moscow,St.Petersburg,Volgograd",
			"SAMT-4|(GMT+4) Izhevsk,Samara",
			"AMT-4AMST,M3.5.0,M10.5.0/3|(GMT+4DST) Yerevan",
			"AZT-4AZST,M3.5.0/4,M10.5.0/5|(GMT+4DST) Baku",
			"UCT-4|(GMT+4) Dubai,Muscat,Tbilisi",
			"AFT-4:30|(GMT+4:30) Kabul",
			"UCT-5|(GMT+5) Aqtobe,Ashgabat,Dushanbe,Karachi,Oral,Tashkent",
			"IST-5:30|(GMT+5:30) Calcutta,Colombo",
			"YEKT-6|(GMT+6) Yekaterinburg",
			"UCT-6|(GMT+6) Almaty,Bishkek,Dhaka,Qyzylorda,Thimphu",
			"OMST-7|(GMT+7) Omsk, Tomsk, Altaj",
			"NOVT-7|(GMT+7) Novosibirsk, Novokuznetsk",
			"UCT-7|(GMT+7) Jakarta,Bangkok,Vientiane,Phnom_Penh",
			"KRAT-8|(GMT+8) Krasnoyarsk",
			"UCT-8|(GMT+8) Shanghai,Hong_Kong,Taipei,Singapore,Ulaanbaatar,Perth",
			"IRKT-9|(GMT+9) Irkutsk",
			"UCT-9|(GMT+9) Dili,Jayapura,Pyongyang,Seoul,Tokyo",
			"CST-9:30CST,M10.5.0,M3.5.0/3|(GMT+9:30DST) Adelaide",
			"CST-9:30|(GMT+9:30) Darwin",
			"YAKT-10|(GMT+10) Yakutsk",
			"EST-10EST,M10.5.0,M3.5.0/3|(GMT+10DST) Melbourne",
			"EST-10EST,M10.1.0,M3.5.0/3|(GMT+10DST) Hobart",
			"EST-10|(GMT+10) Brisbane",
			"VLAT-11|(GMT+11) Vladivostok",
			"SAKT-11|(GMT+11) Sakhalin",
			"MAGT-12|(GMT+12) Magadan",
			"PETT-12|(GMT+12) Kamchatka",
			"ANAT-12|(GMT+12) Anadyr",
			"NZST-12NZDT,M10.1.0,M3.3.0/3|(GMT+12DST) Auckland" );
	list = document.form.TimeZoneList;
	free_options(list);
	add_option_x(list, "manual", "Manual", 1);
	add_options_x3(list, TimeZone, document.form.time_zone.value);
	if (list.options[list.options.selectedIndex].value == "manual")
		inputCtrl2(document.form.time_zone, 1);
	else
		inputCtrl2(document.form.time_zone, 0);
}

function setDateCheck(d1, d2, d3, d4, d5, d6, d7)
{
	str = "";

	if (d7.checked === true ) str = "1" + str;
	else str = "0" + str;
	if (d6.checked === true ) str = "1" + str;
	else str = "0" + str;
	if (d5.checked === true ) str = "1" + str;
	else str = "0" + str;
	if (d4.checked === true ) str = "1" + str;
	else str = "0" + str;
	if (d3.checked === true ) str = "1" + str;
	else str = "0" + str;
	if (d2.checked === true ) str = "1" + str;
	else str = "0" + str;
	if (d1.checked === true ) str = "1" + str;
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
			setTimeout("to_quick()", 1000);
		}
	}
}

function load_body()
{
	var frm = document.form;
	frm.next_host.value = location.host;

	if (frm.current_page.value == "Advanced_Wireless_Content.asp")
	{
		if (window.top.isBand() == 'b')
		{
			inputCtrl(frm.wl_gmode, 0);
			inputCtrl(frm.wl_gmode_check, 0);
		}
		else if (window.top.isBand() == 'n')
		{
			insert_subchannel_options();
		}

		masq_wepkey();
		if (window.top.isModel()=="WL520" || window.top.isModel()=="SnapAP" || window.top.isCard()=='ralink')
		{
			wl_auth_mode_reconf();
		}
		wl_auth_mode_change(1);
		if (frm.wl_gmode_protection_x.value == "1")
		{
			frm.wl_gmode_check.checked = true;
		}
		else
		{
			frm.wl_gmode_check.checked = false;
		}

//		if (frm.wl_crypto.value=="tkip+aes")
//		{
//		frm.wl_wep_x.value = 0;
//		inputCtrl(frm.wl_wep_x, 0);
//		inputCtrl(frm.wl_phrase_x, 0);
//		inputCtrl(frm.wl_key1, 0);
//		inputCtrl(frm.wl_key2, 0);
//		inputCtrl(frm.wl_key3, 0);
//		inputCtrl(frm.wl_key4, 0);
//		inputCtrl(frm.wl_key, 0);

	}
	else if (frm.current_page.value == "Advanced_WMode_Content.asp")
	{
		if (window.top.isCard()=='ralink')
		{
//			no wds only mode
			frm.wl_mode_x.options[1].value = null;
			frm.wl_mode_x.options[1] = null;

			change_wireless_bridge2(frm.wl_mode_x.value,
					rcheck(frm.wl_wdsapply_x),
					1, 0);
		}
		else
		{
			change_wireless_bridge(frm.wl_mode_x.value,
					rcheck(frm.wl_wdsapply_x),
					rcheck(frm.wl_lazywds), 0);
		}
	}
	else if (frm.current_page.value == "Advanced_WAdvanced_Content.asp")
	{
		if (window.top.isCard()!='ralink')
			wl_rate_change();

		if (window.top.isModel()!="WL520" && window.top.isModel()!="SnapAP" && window.top.isCard()!='ralink')
		{
			if (window.top.isBand() == 'b') inputCtrl(frm.wl_frameburst, 0);

			frm.wl_radio_date_x_Sun.checked = getDateCheck(frm.wl_radio_date_x.value, 0);
			frm.wl_radio_date_x_Mon.checked = getDateCheck(frm.wl_radio_date_x.value, 1);
			frm.wl_radio_date_x_Tue.checked = getDateCheck(frm.wl_radio_date_x.value, 2);
			frm.wl_radio_date_x_Wed.checked = getDateCheck(frm.wl_radio_date_x.value, 3);
			frm.wl_radio_date_x_Thu.checked = getDateCheck(frm.wl_radio_date_x.value, 4);
			frm.wl_radio_date_x_Fri.checked = getDateCheck(frm.wl_radio_date_x.value, 5);
			frm.wl_radio_date_x_Sat.checked = getDateCheck(frm.wl_radio_date_x.value, 6);

			frm.wl_radio_time_x_starthour.value = getTimeRange(frm.wl_radio_time_x.value, 0);
			frm.wl_radio_time_x_startmin.value = getTimeRange(frm.wl_radio_time_x.value, 1);
			frm.wl_radio_time_x_endhour.value = getTimeRange(frm.wl_radio_time_x.value, 2);
			frm.wl_radio_time_x_endmin.value = getTimeRange(frm.wl_radio_time_x.value, 3);
		}
	}
	else if (frm.current_page.value == "Advanced_WAN_Content.asp")
	{
		if (frm.wan_dnsenable_x[0].checked === true)
		{
			inputCtrl(frm.wan_dns1_x, 0);
			inputCtrl(frm.wan_dns2_x, 0);
		}
		else
		{
			inputCtrl(frm.wan_dns1_x, 1);
			inputCtrl(frm.wan_dns2_x, 1);
		}

		if (window.top.isModel()!="SnapAP")
		{
			change_wan_type(frm.wan_proto.value);

			if (frm.wan_pppoe_txonly_x.value == "1")
			{
				frm.wan_pppoe_idletime_check.checked = true;
			}

			if (window.top.isModel()=='WL520')
			{
				frm.wan_mode_x.options[2].value = null;
				frm.wan_mode_x.options[2] = null;
			}
		}

//		change_common(frm.wan_proto, "Layer3Forwarding", "x_ConnectionType");
	}
	else if (frm.current_page.value == "Advanced_IPv6_Content.asp")
	{
		change_ipv6_type(frm.ipv6_proto.value);
		/* temporary until all options are supported */
		window.top.pageChanged = 0;
	}
	else if (frm.current_page.value == "Advanced_MultiPPPoE_Content.asp")
	{

		if (frm.PPPConnection_x_MultiPPPoEEnable1[0].checked === true)
		{
			flag=1;
		}
		else
		{
			flag=0;
		}

		inputCtrl(frm.PPPConnection_x_UserName1, flag);
		inputCtrl(frm.PPPConnection_x_Password1, flag);
		inputCtrl(frm.PPPConnection_x_IdleTime1, flag);
		inputCtrl(frm.PPPConnection_x_IdleTime1_check, flag);
		inputCtrl(frm.PPPConnection_x_PPPoEMTU1, flag);
		inputCtrl(frm.PPPConnection_x_PPPoEMRU1, flag);
		inputCtrl(frm.PPPConnection_x_ServiceName1, flag);
		inputCtrl(frm.PPPConnection_x_AccessConcentrator1, flag);

		if (frm.PPPConnection_x_MultiPPPoEEnable2[0].checked === true)
		{
			flag=1;
		}
		else
		{
			flag=0;
		}

		inputCtrl(frm.PPPConnection_x_UserName2, flag);
		inputCtrl(frm.PPPConnection_x_Password2, flag);
		inputCtrl(frm.PPPConnection_x_IdleTime2, flag);
		inputCtrl(frm.PPPConnection_x_IdleTime2_check, flag);
		inputCtrl(frm.PPPConnection_x_PPPoEMTU2, flag);
		inputCtrl(frm.PPPConnection_x_PPPoEMRU2, flag);
		inputCtrl(frm.PPPConnection_x_ServiceName2, flag);
		inputCtrl(frm.PPPConnection_x_AccessConcentrator2, flag);

		if (frm.PPPConnection_x_IdleTxOnly1.value == "1")
		{
			frm.PPPConnection_x_IdleTime1_check.checked = true;
		}
		if (frm.PPPConnection_x_IdleTxOnly2.value == "1")
		{
			frm.PPPConnection_x_IdleTime2_check.checked = true;
		}
	}
//	else if (frm.current_page.value == "Advanced_RLANWAN_Content.asp"){}
	else if (frm.current_page.value == "Advanced_PortTrigger_Content.asp")
	{
		wItem = new Array(
				new Array("Quicktime 4 Client", "554", "TCP", "6970:32000", "UDP"),
				new Array("Real Audio", "7070", "TCP", "6970:7170", "UDP"));
//		new Array("MS Terminal Server", "3389", "TCP", "3389", "TCP"));

		free_options(frm.TriggerKnownApps);
		add_option(frm.TriggerKnownApps, "User Defined", 1);
		for (i = 0; i < wItem.length; i++)
		{
			add_option(frm.TriggerKnownApps, wItem[i][0], 0);
		}
	}
	else if (frm.current_page.value == "Advanced_VirtualServer_Content.asp")
	{
		wItem = new Array(
				new Array("FTP", "20:21", "TCP"),
				new Array("TELNET", "23", "TCP"),
				new Array("SMTP", "25", "TCP"),
				new Array("DNS", "53", "UDP"),
				new Array("FINGER", "79", "TCP"),
				new Array("HTTP", "80", "TCP"),
				new Array("POP3", "110", "TCP"),
				new Array("SNMP", "161", "UDP"),
				new Array("SNMP TRAP", "162", "UDP"));

		free_options(frm.KnownApps);
		add_option(frm.KnownApps, "User Defined", 1);
		for (i = 0; i < wItem.length; i++)
		{
			add_option(frm.KnownApps, wItem[i][0], 0);
		}
	}
	else if (frm.current_page.value == "Advanced_BasicFirewall_Content.asp")
	{
		change_firewall(rcheck(frm.fw_enable_x));
	}
	else if (frm.current_page.value == "Advanced_Firewall_Content.asp")
	{
		wItem = new Array(
				new Array("WWW", "80", "TCP"),
				new Array("TELNET", "23", "TCP"),
				new Array("FTP", "20:21", "TCP"));

		if (window.top.isModel()!="WL520" && window.top.isModel()!="SnapAP")
		{
			free_options(frm.WLKnownApps);
			add_option(frm.WLKnownApps, "User Defined", 1);
			for (i = 0; i < wItem.length; i++)
			{
				add_option(frm.WLKnownApps, wItem[i][0], 0);
			}
		}
		free_options(frm.LWKnownApps);
		add_option(frm.LWKnownApps, "User Defined", 1);
		for (i = 0; i < wItem.length; i++)
		{
			add_option(frm.LWKnownApps, wItem[i][0], 0);
		}

		if (window.top.isModel()!="WL520" && window.top.isModel()!="SnapAP")
		{
//			Handle for Date and Time
			frm.filter_wl_date_x_Sun.checked = getDateCheck(frm.filter_wl_date_x.value, 0);
			frm.filter_wl_date_x_Mon.checked = getDateCheck(frm.filter_wl_date_x.value, 1);
			frm.filter_wl_date_x_Tue.checked = getDateCheck(frm.filter_wl_date_x.value, 2);
			frm.filter_wl_date_x_Wed.checked = getDateCheck(frm.filter_wl_date_x.value, 3);
			frm.filter_wl_date_x_Thu.checked = getDateCheck(frm.filter_wl_date_x.value, 4);
			frm.filter_wl_date_x_Fri.checked = getDateCheck(frm.filter_wl_date_x.value, 5);
			frm.filter_wl_date_x_Sat.checked = getDateCheck(frm.filter_wl_date_x.value, 6);

			frm.filter_wl_time_x_starthour.value = getTimeRange(frm.filter_wl_time_x.value, 0);
			frm.filter_wl_time_x_startmin.value = getTimeRange(frm.filter_wl_time_x.value, 1);
			frm.filter_wl_time_x_endhour.value = getTimeRange(frm.filter_wl_time_x.value, 2);
			frm.filter_wl_time_x_endmin.value = getTimeRange(frm.filter_wl_time_x.value, 3);
		}
		frm.filter_lw_date_x_Sun.checked = getDateCheck(frm.filter_lw_date_x.value, 0);
		frm.filter_lw_date_x_Mon.checked = getDateCheck(frm.filter_lw_date_x.value, 1);
		frm.filter_lw_date_x_Tue.checked = getDateCheck(frm.filter_lw_date_x.value, 2);
		frm.filter_lw_date_x_Wed.checked = getDateCheck(frm.filter_lw_date_x.value, 3);
		frm.filter_lw_date_x_Thu.checked = getDateCheck(frm.filter_lw_date_x.value, 4);
		frm.filter_lw_date_x_Fri.checked = getDateCheck(frm.filter_lw_date_x.value, 5);
		frm.filter_lw_date_x_Sat.checked = getDateCheck(frm.filter_lw_date_x.value, 6);

		frm.filter_lw_time_x_starthour.value = getTimeRange(frm.filter_lw_time_x.value, 0);
		frm.filter_lw_time_x_startmin.value = getTimeRange(frm.filter_lw_time_x.value, 1);
		frm.filter_lw_time_x_endhour.value = getTimeRange(frm.filter_lw_time_x.value, 2);
		frm.filter_lw_time_x_endmin.value = getTimeRange(frm.filter_lw_time_x.value, 3);

		change_internet_firewall("1");
	}
	else if (frm.current_page.value == "Advanced_LFirewall_Content.asp")
	{
		frm.FirewallConfig_WanLocalActiveDate_Sun.checked = getDateCheck(frm.FirewallConfig_WanLocalActiveDate.value, 0);
		frm.FirewallConfig_WanLocalActiveDate_Mon.checked = getDateCheck(frm.FirewallConfig_WanLocalActiveDate.value, 1);
		frm.FirewallConfig_WanLocalActiveDate_Tue.checked = getDateCheck(frm.FirewallConfig_WanLocalActiveDate.value, 2);
		frm.FirewallConfig_WanLocalActiveDate_Wed.checked = getDateCheck(frm.FirewallConfig_WanLocalActiveDate.value, 3);
		frm.FirewallConfig_WanLocalActiveDate_Thu.checked = getDateCheck(frm.FirewallConfig_WanLocalActiveDate.value, 4);
		frm.FirewallConfig_WanLocalActiveDate_Fri.checked = getDateCheck(frm.FirewallConfig_WanLocalActiveDate.value, 5);
		frm.FirewallConfig_WanLocalActiveDate_Sat.checked = getDateCheck(frm.FirewallConfig_WanLocalActiveDate.value, 6);

		frm.FirewallConfig_WanLocalActiveTime_starthour.value = getTimeRange(frm.FirewallConfig_WanLocalActiveTime.value, 0);
		frm.FirewallConfig_WanLocalActiveTime_startmin.value = getTimeRange(frm.FirewallConfig_WanLocalActiveTime.value, 1);
		frm.FirewallConfig_WanLocalActiveTime_endhour.value = getTimeRange(frm.FirewallConfig_WanLocalActiveTime.value, 2);
		frm.FirewallConfig_WanLocalActiveTime_endmin.value = getTimeRange(frm.FirewallConfig_WanLocalActiveTime.value, 3);
	}
	else if (frm.current_page.value == "Advanced_URLFilter_Content.asp")
	{
		frm.url_date_x_Sun.checked = getDateCheck(frm.url_date_x.value, 0);
		frm.url_date_x_Mon.checked = getDateCheck(frm.url_date_x.value, 1);
		frm.url_date_x_Tue.checked = getDateCheck(frm.url_date_x.value, 2);
		frm.url_date_x_Wed.checked = getDateCheck(frm.url_date_x.value, 3);
		frm.url_date_x_Thu.checked = getDateCheck(frm.url_date_x.value, 4);
		frm.url_date_x_Fri.checked = getDateCheck(frm.url_date_x.value, 5);
		frm.url_date_x_Sat.checked = getDateCheck(frm.url_date_x.value, 6);

		frm.url_time_x_starthour.value = getTimeRange(frm.url_time_x.value, 0);
		frm.url_time_x_startmin.value = getTimeRange(frm.url_time_x.value, 1);
		frm.url_time_x_endhour.value = getTimeRange(frm.url_time_x.value, 2);
		frm.url_time_x_endmin.value = getTimeRange(frm.url_time_x.value, 3);
	}
//	else if (frm.current_page.value == "Advanced_DHCP_Content.asp" ||
//	frm.current_page.value == "Advanced_RDHCP_Content.asp")
//	{
//	checkSubnet();
//	}
	else if (frm.current_page.value.indexOf("Advanced_DDNS_Content")!=-1)
	{
//		if (frm.LANHostConfig_x_DDNSStatus.value != "1")
//		frm.LANHostConfig_x_DDNSStatus_button.disabled = true;
		inputCtrl(document.form.upnp_proto, document.form.upnp_enable.value == 0 ? 0 : 1);
		TimeZoneList();
		if (frm.udpxy_wan_x.value == "1")
			frm.udpxy_wan_check.checked = true;
		else
			frm.udpxy_wan_check.checked = false;
		
	}
	else if (frm.current_page.value == "Advanced_APLAN_Content.asp")
	{
//		alert(frm.wan_dnsenable_x[0].checked);
//		alert(frm.wan_dnsenable_x[1].checked);
		if (frm.lan_proto_x[0].checked === true)
		{
			inputCtrl(frm.lan_ipaddr, 0);
			inputCtrl(frm.lan_netmask, 0);
			inputCtrl(frm.lan_gateway, 0);
		}
		else
		{
			inputCtrl(frm.lan_ipaddr, 1);
			inputCtrl(frm.lan_netmask, 1);
			inputCtrl(frm.lan_gateway, 1);
		}
	}
	else if (frm.current_page.value == "Advanced_WebCam_Content.asp")
	{
		if (window.top.isMode() == "AP")
		{
			if (frm.usb_webenable_x.value==2)
				frm.usb_webenable_x.value=1;
			frm.usb_webenable_x.options[2].value = null;
			frm.usb_webenable_x.options[2] = null;
		}

		frm.usb_websecurity_date_x_Sun.checked = getDateCheck(frm.usb_websecurity_date_x.value, 0);
		frm.usb_websecurity_date_x_Mon.checked = getDateCheck(frm.usb_websecurity_date_x.value, 1);
		frm.usb_websecurity_date_x_Tue.checked = getDateCheck(frm.usb_websecurity_date_x.value, 2);
		frm.usb_websecurity_date_x_Wed.checked = getDateCheck(frm.usb_websecurity_date_x.value, 3);
		frm.usb_websecurity_date_x_Thu.checked = getDateCheck(frm.usb_websecurity_date_x.value, 4);
		frm.usb_websecurity_date_x_Fri.checked = getDateCheck(frm.usb_websecurity_date_x.value, 5);
		frm.usb_websecurity_date_x_Sat.checked = getDateCheck(frm.usb_websecurity_date_x.value, 6);

		frm.usb_websecurity_time_x_starthour.value = getTimeRange(frm.usb_websecurity_time_x.value, 0);
		frm.usb_websecurity_time_x_startmin.value = getTimeRange(frm.usb_websecurity_time_x.value, 1);
		frm.usb_websecurity_time_x_endhour.value = getTimeRange(frm.usb_websecurity_time_x.value, 2);
		frm.usb_websecurity_time_x_endmin.value = getTimeRange(frm.usb_websecurity_time_x.value, 3);

		if (frm.usb_webhttpcheck_x.value == "1")
		{
			frm.usb_webhttpport_x_check.checked = true;
			inputCtrl(frm.usb_webhttp_username, 1);
			inputCtrl(frm.usb_webhttp_passwd, 1);
		}
		else
		{
			frm.usb_webhttpport_x_check.checked = false;
			inputCtrl(frm.usb_webhttp_username, 0);
			inputCtrl(frm.usb_webhttp_passwd, 0);
		}
		
		if (frm.usb_webformat_x.value == "0") /* MJPEG */
		{
			inputCtrl(frm.usb_webquality_x, 0);
		}
		else /* YUV */
		{
			inputCtrl(frm.usb_webquality_x, 1);
		}

	}
	else if (frm.current_page.value == "Advanced_DMZIP11g_Content.asp" ||
			frm.current_page.value == "Advanced_DMZIP_Content.asp")
	{
		change_wireless_firewall();
	}
	else if (frm.current_page.value == "Advanced_DMZIP_Content.asp")
	{
//		alert(frm.wan_dnsenable_x[0].checked);
//		alert(frm.wan_dnsenable_x[1].checked);
		if (frm.LANHostConfig_lan_proto_x[0].checked === true)
		{
			inputCtrl(frm.lan_ipaddr, 0);
			inputCtrl(frm.lan_netmask, 0);
			inputCtrl(frm.lan_gateway, 0);
		}
		else
		{
			inputCtrl(frm.lan_ipaddr, 1);
			inputCtrl(frm.lan_netmask, 1);
			inputCtrl(frm.lan_gateway, 1);
		}
	}
//	else if (frm.current_page.value == "Main_GStatus_Content.asp")
//	{
//	if (frm.wan_proto_t.value == "Static")
//	{
//	frm.PPPConnection_x_WANAction_button.disabled = 1;
//	frm.PPPConnection_x_WANAction_button1.disabled = 1;
//	}
//	}
	else if (frm.current_page.value == "Advanced_RMISC_Content.asp")
	{
		TimeZoneList();
	}
	else if (frm.current_page.value == "Advanced_WiMax_Content.asp")
	{
		changeWiMAXCheckConnection();
	}
	change = 0;
}

function unload_body()
{
	return true;
//	if (change==1 && !nav)
//	{
//	alert('This page has been changed, remember to press Save or Finish');
//	alert(document.form.current_page.value);
//	location = document.form.current_page.value+"#Confirm";
//	document.form.action_mode.value = " Save ";
//	document.form.next_page.value = document.form.current_page.value;
//	document.form.submit();
//	}
//	return false;
}


function change_internet_firewall(r)
{
//	if (window.top.isModel()!="WL520" && window.top.isModel()!="SnapAP")
//	{
//	if (r=="1")
//	{
//	document.form.InternetFirewall_img.src = "graph/internet_some.gif";
//	}
//	else
//	{
//	document.form.InternetFirewall_img.src = "graph/internet_none.gif";
//	}
//	}
}

function change_wireless_firewall()
{
	if (window.top.isModel() == "WL600")
	{
		if (document.form.FirewallConfig_DmzEnable[0].checked === true )
		{
			if (document.form.FirewallConfig_DmzDevices.value == "Both")
				document.form.WirelessFirewall_img.src = "graph/wf_both.gif";
			else if (document.form.FirewallConfig_DmzDevices.value == "802.11a only")
				document.form.WirelessFirewall_img.src = "graph/wf_a.gif";
			else if (document.form.FirewallConfig_DmzDevices.value == "802.11g only")
				document.form.WirelessFirewall_img.src = "graph/wf_g.gif";
		}
		else document.form.WirelessFirewall_img.src = "graph/wf_none.gif";
	}
	else
	{
		if (document.form.FirewallConfig_DmzEnable[0].checked === true)
		{
			document.form.WirelessFirewall_img.src = "graph/wf_g.gif";
		}
		else document.form.WirelessFirewall_img.src = "graph/wf_none.gif";
	}
}

function change_firewall(r)
{
	if (r == "0")
	{
		inputRCtrl1(document.form.misc_http_x, 0);
		inputRCtrl2(document.form.misc_http_x, 1);
		inputCtrl(document.form.misc_httpport_x, 0);

		if (window.top.isModel()!="WL520" && window.top.isModel()!="SnapAP")
		{
			inputRCtrl1(document.form.misc_lpr_x, 0);
			inputRCtrl2(document.form.misc_lpr_x, 1);
			inputRCtrl1(document.form.misc_ping_x, 0);
			inputRCtrl2(document.form.misc_ping_x, 1);
		}
	}
	else
	{
		inputRCtrl1(document.form.misc_http_x, 1);
		inputCtrl(document.form.misc_httpport_x, 1);

		if (window.top.isModel()!="WL520" && window.top.isModel()!="SnapAP")
		{
			inputRCtrl1(document.form.misc_lpr_x, 1);
			inputRCtrl1(document.form.misc_ping_x, 1);
		}
	}
}

function change_wireless_bridge(m, a, r, mflag)
{
	if (a=="0" && r == "0" && mflag != 1)
	{
		document.form.wl_mode_x.value = "0";
		m = "0";
	}


	if (m == "0")
	{
		wdsimage = "wds_ap";
		inputRCtrl2(document.form.wl_wdsapply_x, 1);
		inputRCtrl2(document.form.wl_lazywds, 1);
		inputRCtrl1(document.form.wl_wdsapply_x, 0);
		inputRCtrl1(document.form.wl_lazywds, 0);
	}
	else
	{
		if (a=="0" && r == "0")
		{
			inputRCtrl2(document.form.wl_wdsapply_x, 0);
			inputRCtrl2(document.form.wl_lazywds, 1);
		}

		inputRCtrl1(document.form.wl_wdsapply_x, 1);
		inputRCtrl1(document.form.wl_lazywds, 1);

		if (m == "1")
			wdsimage = "wds_wds_both";
		else
			wdsimage = "wds_mixed_both";

//		if (a == "0")
//		{
//		if (r == "0")
//		wdsimage += "_connect";
//		else
//		wdsimage += "_anony";
//		}
//		else
//		{
//		if (r == "0")
//		wdsimage += "_connect";
//		else
//		wdsimage += "_both";

//		}

		if (document.form.wl_channel.value == "0")
		{
			alert("Please set a fixed channel for WDS!!!");
			document.form.wl_channel.options[0].selected = 0;
			document.form.wl_channel.options[1].selected = 1;
		}

//		alert(document.form.WLANConfig11b_Channel.options[0].value);
//		if (document.form.WLANConfig11b_Channel.options[0].value == "0")
//		{
//		document.form.WLANConfig11b_Channel.options[0].value = null;
//		document.form.WLANConfig11b_Channel.options[0] = null;
//		}

	}
	wdsimage = "graph/" + wdsimage + ".gif";

//	if (window.top.isFlash() != '2MB')
//	document.form.WirelessBridge_img.src = wdsimage;
}

function change_wireless_bridge2(m, a, r, mflag)
{
	if (a=="0" && r == "0" && mflag != 1)
	{
		document.form.wl_mode_x.value = "0";
		m = "0";
	}


	if (m == "0")
	{
		wdsimage = "wds_ap";
		inputRCtrl2(document.form.wl_wdsapply_x, 1);
//		inputRCtrl2(document.form.wl_lazywds, 1);
		inputRCtrl1(document.form.wl_wdsapply_x, 0);
//		inputRCtrl1(document.form.wl_lazywds, 0);
	}
	else
	{
		if (a=="0" && r == "0")
		{
			inputRCtrl2(document.form.wl_wdsapply_x, 0);
//			inputRCtrl2(document.form.wl_lazywds, 1);
		}

		inputRCtrl1(document.form.wl_wdsapply_x, 1);
//		inputRCtrl1(document.form.wl_lazywds, 1);

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

		if (document.form.wl_channel.value == "0")
		{
			alert("Please set a fixed channel for WDS!!!");
			document.form.wl_channel.options[0].selected = 0;
			document.form.wl_channel.options[1].selected = 1;
		}

//		alert(document.form.WLANConfig11b_Channel.options[0].value);
//		if (document.form.WLANConfig11b_Channel.options[0].value == "0")
//		{
//		document.form.WLANConfig11b_Channel.options[0].value = null;
//		document.form.WLANConfig11b_Channel.options[0] = null;
//		}

	}
	wdsimage = "graph/" + wdsimage + ".gif";

//	if (window.top.isFlash() != '2MB')
//	document.form.WirelessBridge_img.src = wdsimage;
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
	else if (document.form.current_page.value == "Advanced_WAN_Content.asp")
	{
		if (document.form.wan_proto.value == "static")
		{
			if (document.form.wan_ipaddr.value==="" ||
					document.form.wan_netmask.value==="")
			{
				alert("As you leave IP or subnet mask fields blank, connection type will be set as Automatic IP!");
				document.form.wan_proto.value = "dhcp";
			}
		}
		checkSubnet();
		inputCtrl(document.form.wan_ipaddr, 1);
		inputCtrl(document.form.wan_netmask, 1);
		inputCtrl(document.form.wan_gateway, 1);
	}
	else if (document.form.current_page.value == "Advanced_LAN_Content.asp" )
	{
		checkSubnet();
	}
//	else if (document.form.current_page.value == "Advanced_IPv6_Content.asp")
//	{
//	checkSubnet();
//	}
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
	else if (document.form.current_page.value == "Advanced_BasicFirewall_Content.asp")
	{
		inputRCtrl1(document.form.misc_http_x, 1);

		if (window.top.isModel()!="WL520" && window.top.isModel()!="SnapAP")
		{
			inputRCtrl1(document.form.misc_lpr_x, 1);
			inputRCtrl1(document.form.misc_ping_x, 1);
		}
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
		if (document.form.wl_mode_x.value == "0")
		{
			inputRCtrl1(document.form.wl_wdsapply_x, 1);
			inputRCtrl1(document.form.wl_lazywds, 1);
			inputRCtrl2(document.form.wl_wdsapply_x, 1);
			inputRCtrl2(document.form.wl_lazywds, 1);
//			alert(rcheck(document.form.WLANConfig11b_x_BRApply));
//			alert(rcheck(document.form.WLANConfig11b_x_BRestrict));
		}
	}
	else if (document.form.current_page.value == "Advanced_Wireless_Content.asp")
	{
//		apply setting of key anyway
		inputCtrl(document.form.wl_phrase_x, 1);
		inputCtrl(document.form.wl_key1, 1);
		inputCtrl(document.form.wl_key2, 1);
		inputCtrl(document.form.wl_key3, 1);
		inputCtrl(document.form.wl_key4, 1);
		inputCtrl(document.form.wl_key, 1);
		unmasq_wepkey();
	}
//	alert(parent.folderFrame.src);
//	document.form.next_page.value = "Advanced_LANWAN_Content.asp";
//	Ugly solution
}

function onSubmitCtrl(o, s)
{
//	o.value = s;
	document.form.action_mode.value = s;
	onSubmit();
	return true;
}

function onSubmitCtrlOnly(o, s)
{
//	o.value = s;
	if (s!='Upload') document.form.action_mode.value = s;
//	onSubmit();
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
			else if (local.value==="" || port.value === "")
			{
				alert("As you leave IP or subnet mask fields blank, connection type will be set as Automatic IP!1");
				return false;
			}
		}

		document.form.action.value = action.value;
		document.form.action_mode.value = action.value;
		document.form.action_script.value = "portmapping.sh" + " " + action.value + " " + local.value + " " + port.value;
//		document.form.submit();
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

function insert_subchannel_options()
{
	var wl_mode = document.form.wl_gmode.value;
	var wl_nctrlsb = document.form.wl_nctrlsb;
	var wl_options = wl_nctrlsb.options;
	value = wl_nctrlsb.value;
	wl_options.length = 1;
	wl_options[0].text = "None";
	wl_options[0].value = "none";
	if ((wl_mode == "1" || wl_mode == "6") && document.form.wl_nbw.value == "40")
	{
		var wl_channel = 1*document.form.wl_channel.value;
		var wl_channels = document.form.wl_channel.options.length;
		if (value != "lower" && value != "upper") value = "lower";
		if (wl_channel >= 1+4 && wl_channel < wl_channels) {
			selected = (value == "upper" ||  wl_channel >= wl_channels-4) ? 1 : 0;
			add_option_text(wl_nctrlsb, wl_channel-4, "upper", selected);
		}
		if (wl_channel >= 1 && wl_channel < wl_channels-4) {
			selected = (value == "lower" ||  wl_channel < 1+4) ? 1 : 0;
			add_option_text(wl_nctrlsb, wl_channel+4, "lower", selected);
		}
		if (wl_options.length == 1)
			add_option_text(wl_nctrlsb, "Auto", value, 1);
		inputCtrl(wl_nctrlsb, 1);
	} else {
		wl_options[0].selected = 1;
		inputCtrl(wl_nctrlsb, 0);
	}
}

function change_common(o, s, v)
{
	change = 1;
	window.top.pageChanged = 1;

	if (v=="wan_proto")
	{
		change_wan_type(o.value);
	}
	else if (v=="ipv6_proto")
	{
		change_ipv6_type(o.value);
	}
	else if (s == "FirewallConfig")
	{
		if (v == "DmzDevices")
			change_wireless_firewall();
		else if (v == "WanLanDefaultAct" && o.value == "DROP")
			alert("Selecting DROP will drop all WAN to LAN packets except for those matched in filter table. Please use it carefully.");
		else if (v == "LanWanDefaultAct" && o.value == "DROP")
			alert("Selecting DROP will drop all LAN to WAN packets except for those matched in filter table. Please use it carefully.");
	}
	else if (s == "WLANConfig11b")
	{
		if (v == "wl_auth_mode") /* Handle AuthenticationMethod Change */
		{
			wl_auth_mode_change(0);

			if (o.value == "psk")
			{
				opts=document.form.wl_auth_mode.options;
				if (opts[opts.selectedIndex].text == "WPA-Personal")
					document.form.wl_wpa_mode.value="1";
				else if (opts[opts.selectedIndex].text == "WPA2-Personal")
					document.form.wl_wpa_mode.value="2";
				else if (opts[opts.selectedIndex].text == "WPA-Auto-Personal")
					document.form.wl_wpa_mode.value="0";
				document.form.wl_wpa_psk.focus();
			}
			else if (o.value == "shared" || o.value == "radius")
				document.form.wl_phrase_x.focus();
		}
		else if (v == "wl_crypto")
		{
			wl_auth_mode_change(0);
//			if (o.value=="tkip+aes")
//			{
//			document.form.wl_wep_x.value = 0;
//			//change_wlweptype(document.form.wl_wep_x, "WLANConfig11b");

//			inputCtrl(document.form.wl_wep_x, 0);
//			inputCtrl(document.form.wl_phrase_x, 0);
//			inputCtrl(document.form.wl_key1, 0);
//			inputCtrl(document.form.wl_key2, 0);
//			inputCtrl(document.form.wl_key3, 0);
//			inputCtrl(document.form.wl_key4, 0);
//			inputCtrl(document.form.wl_key, 0);
//			}
		}
		else if (v == "wl_mode_x")
		{
			if (window.top.isCard()=='ralink')
			{
			change_wireless_bridge2(document.form.wl_mode_x.value,
					rcheck(document.form.wl_wdsapply_x),
					1, 1);
			}
			else change_wireless_bridge(o.value, rcheck(document.form.wl_wdsapply_x), rcheck(document.form.wl_lazywds), 1);
		}
		else if (v == "wl_wep_x") /* Handle AuthenticationMethod Change */
		{
			change_wlweptype(o, "WLANConfig11b");
		}
		else if (v == "x_Mode11g")
		{
//			alert(document.form.wan_dnsenable_x[0].checked);
//			alert(document.form.wan_dnsenable_x[1].checked);
			RefreshRateSetList(document.form.WLANConfig11b_x_Mode11g.value, true);
		}
		else if (v == "Channel" && document.form.current_page.value == "Advanced_WMode_Content.asp"
					&& document.form.WLANConfig11b_x_APMode.value != "0" && document.form.WLANConfig11b_Channel.value == "0")
		{
			alert("Please set a fixed channel for WDS!!!");
			document.form.WLANConfig11b_Channel.options[0].selected = 0;
			document.form.WLANConfig11b_Channel.options[1].selected = 1;
		}
		else if (v == "wl_channel")
		{
			insert_subchannel_options();
		}
		else if (v == "wl_nbw")
		{
			insert_subchannel_options();
		}
		else if (v == "wl_nctrlsb")
		{
			if (o.value == "none") /* None */
			{
				document.form.wl_nbw.value = "20";
				inputCtrl(document.form.wl_nctrlsb, 0);
			}
		}
		else if (v == "wl_gmode")
		{
			if (o.value == "1" || o.value == "6")  /* Auto or 802.11n Only */
			{
				document.form.wl_nbw.value = "40";
				inputCtrl(document.form.wl_nbw, 1);
			}
			else /* 802.11g Only, 802.11b Only, ... */
			{
				document.form.wl_nbw.value = "20";
				inputCtrl(document.form.wl_nbw, 0);
			}
			insert_subchannel_options();
		}
	}
	else if (s=="LANHostConfig" && v=="time_zone")
	{
		opts = document.form.TimeZoneList.options;
		if (opts[opts.selectedIndex].value == "manual")
			inputCtrl2(document.form.time_zone, 1);
		else
		{
			inputCtrl2(document.form.time_zone, 0);
			document.form.time_zone.value = opts[opts.selectedIndex].value;
		}
	}
	else if (s=="LANHostConfig" && v=="upnp_enable")
	{
		inputCtrl(document.form.upnp_proto, o.value == 0 ? 0 : 1);
	}
	else if (v == "usb_webformat_x")
	{
		if (o.value == "0") /* MJPEG */
		{
			inputCtrl(document.form.usb_webquality_x, 0);
		}
		else /* YUV */
		{
			inputCtrl(document.form.usb_webquality_x, 1);
		}
	}
	return true;
}


function change_common_radio(o, s, v, r)
{
	change = 1;
	window.top.pageChanged = 1;
	var frm = document.form;

	if (v=='wl_wdsapply_x')
	{
		if (window.top.isCard()=='ralink')
		{
			change_wireless_bridge2(frm.wl_mode_x.value,
					rcheck(frm.wl_wdsapply_x),
					1, 0);
		}
		else change_wireless_bridge(frm.wl_mode_x.value, r, rcheck(frm.wl_lazywds), 0);
	}
	else if (v=='wl_lazywds')
	{
		change_wireless_bridge(frm.wl_mode_x.value, rcheck(frm.wl_wdsapply_x), r, 0);
	}
	else if (v=="wan_dnsenable_x")
	{
//		alert(frm.wan_dnsenable_x[0].checked);
//		alert(frm.wan_dnsenable_x[1].checked);
		if (r == '1')
		{
			inputCtrl(frm.wan_dns1_x, 0);
			inputCtrl(frm.wan_dns2_x, 0);
		}
		else
		{
			inputCtrl(frm.wan_dns1_x, 1);
			inputCtrl(frm.wan_dns2_x, 1);
		}
	}
	else if (v=="fw_enable_x")
	{
//		alert(frm.wan_dnsenable_x[0].checked);
//		alert(frm.wan_dnsenable_x[1].checked);
		change_firewall(r);
	}
	else if (v=="x_AESEnable")
	{
		if (r == '1')
		{
			if (frm.WLANConfig11a_AuthenticationMethod.value!="Open System")
			{
				rst = confirm("If you would like to enable AES unique, Authentication should be set as Open System!");

				if (rst)
					frm.WLANConfig11a_AuthenticationMethod.value = "Open System";
				else
				{
					inputRCtrl2(frm.WLANConfig11a_x_AESEnable, 1);
					return false;
				}
			}
			else
			{
				if (frm.WLANConfig11a_WEPType.value == "None")
				{
					rst = confirm("If you would like to enable AES unique, WEP should be enabled too!");

					if (rst)
					{
						frm.WLANConfig11a_WEPType.value = "64bits";
						change_wlweptype(frm.WLANConfig11a_WEPType, "WLANConfig11a");
					}
					else
					{
						inputRCtrl2(frm.WLANConfig11a_x_AESEnable, 1);
						return false;
					}
				}
			}
		}
	}
	else if (s=="WLANConfig11b" && v=="wl_gmode")
	{
//		alert(frm.wan_dnsenable_x[0].checked);
//		alert(frm.wan_dnsenable_x[1].checked);
		if (frm.wl_gmode_check.checked)
		{
			frm.wl_gmode_protection_x.value = "1";
		}
		else
		{
			frm.wl_gmode_protection_x.value = "0";
		}
	}
	else if (s=="PrinterStatus" && v=="usb_webhttpport_x")
	{
//		alert(frm.wan_dnsenable_x[0].checked);
//		alert(frm.wan_dnsenable_x[1].checked);
		if (frm.usb_webhttpport_x_check.checked)
		{
			frm.usb_webhttpcheck_x.value = "1";
			inputCtrl(frm.usb_webhttp_username, 1);
			inputCtrl(frm.usb_webhttp_passwd, 1);
		}
		else
		{
			frm.usb_webhttpcheck_x.value = "0";
			inputCtrl(frm.usb_webhttp_username, 0);
			inputCtrl(frm.usb_webhttp_passwd, 0);
		}
	}
	else if (v=="lan_proto_x")
	{
//		alert(frm.wan_dnsenable_x[0].checked);
//		alert(frm.wan_dnsenable_x[1].checked);
		if (r == '1')
		{
			inputCtrl(frm.lan_ipaddr, 0);
			inputCtrl(frm.lan_netmask, 0);
			inputCtrl(frm.lan_gateway, 0);
			inputCtrl(frm.lan_dns, 0);
		}
		else
		{
			inputCtrl(frm.lan_ipaddr, 1);
			inputCtrl(frm.lan_netmask, 1);
			inputCtrl(frm.lan_gateway, 1);
			inputCtrl(frm.lan_dns, 1);
		}
	}
	else if (s=='FirewallConfig' && v=='DmzEnable')
	{
		change_wireless_firewall();
	}
	else if (s=='FirewallConfig' && v=='WanLanFirewallEnable')
	{
		change_internet_firewall("1");
	}
	else if (s=="PPPConnection" && v=="wan_pppoe_idletime")
	{
		if (frm.wan_pppoe_idletime_check.checked)
		{
			frm.wan_pppoe_txonly_x.value = "1";
		}
		else
		{
			frm.wan_pppoe_txonly_x.value = "0";
		}
	}
	else if (s=="PPPConnection" && v=="x_IdleTime1")
	{
		if (frm.PPPConnection_x_IdleTime1_check.checked)
		{
			frm.PPPConnection_x_IdleTxOnly1.value = "1";
		}
		else
		{
			frm.PPPConnection_x_IdleTxOnly1.value = "0";
		}
	}
	else if (s=="PPPConnection" && v=="x_MultiPPPoEEnable1")
	{
		if (frm.PPPConnection_x_MultiPPPoEEnable1[0].checked === true)
		{
			flag=1;
		}
		else
		{
			flag=0;
		}

		inputCtrl(frm.PPPConnection_x_UserName1, flag);
		inputCtrl(frm.PPPConnection_x_Password1, flag);
		inputCtrl(frm.PPPConnection_x_IdleTime1, flag);
		inputCtrl(frm.PPPConnection_x_IdleTime1_check, flag);
		inputCtrl(frm.PPPConnection_x_PPPoEMTU1, flag);
		inputCtrl(frm.PPPConnection_x_PPPoEMRU1, flag);
		inputCtrl(frm.PPPConnection_x_ServiceName1, flag);
		inputCtrl(frm.PPPConnection_x_AccessConcentrator1, flag);
	}
	else if (s=="PPPConnection" && v=="x_IdleTime2")
	{
		if (frm.PPPConnection_x_IdleTime2_check.checked)
		{
			frm.PPPConnection_x_IdleTxOnly2.value = "1";
		}
		else
		{
			frm.PPPConnection_x_IdleTxOnly2.value = "0";
		}
	}
	else if (s=="PPPConnection" && v=="x_MultiPPPoEEnable2")
	{
		if (frm.PPPConnection_x_MultiPPPoEEnable2[0].checked === true)
		{
			flag=1;
		}
		else
		{
			flag=0;
		}

		inputCtrl(frm.PPPConnection_x_UserName2, flag);
		inputCtrl(frm.PPPConnection_x_Password2, flag);
		inputCtrl(frm.PPPConnection_x_IdleTime2, flag);
		inputCtrl(frm.PPPConnection_x_IdleTime2_check, flag);
		inputCtrl(frm.PPPConnection_x_PPPoEMTU2, flag);
		inputCtrl(frm.PPPConnection_x_PPPoEMRU2, flag);
		inputCtrl(frm.PPPConnection_x_ServiceName2, flag);
		inputCtrl(frm.PPPConnection_x_AccessConcentrator2, flag);
	}
	else if (s=="LANHostConfig" && v=="udpxy_wan")
	{
		if (frm.udpxy_wan_check.checked)
			frm.udpxy_wan_x.value = "1";
		else
			frm.udpxy_wan_x.value = "0";
	}
	else if (s=="WiMaxConfig" && v=="wan_wimax_check")
	{
		changeWiMAXCheckConnection();
	}
	else if (v=="wan_proto")
	{
		if (frm.wan_proto_x.checked)
		{
			if (s=="3GConfig")
				frm.wan_proto.value="usbmodem";
			else
				frm.wan_proto.value="wimax";
		}
		else
			frm.wan_proto.value="dhcp";
	}
	else if (v=="ipv6_dnsenable_x")
	{
		inputCtrl(frm.ipv6_dns1_x, (r == '1') ? 0 : 1);
//		inputCtrl(frm.ipv6_dns2_x, (r == '1') ? 0 : 1);
//		inputCtrl(frm.ipv6_dns3_x, (r == '1') ? 0 : 1);
	}
	else if (v=="ipv6_radvd_enable")
	{
//		inputCtrl(frm.ipv6_radvd_dns1_x, (r == '1') ? 1 : 0);
	}
	return true;
}

function valid_WPAPSK(o)
{
	if (o.value.length>=64)
	{
		o.value = o.value.substring(0, 63);
		alert("Pre-shared key should be less than 64 characters!!!");
		return false;
	}
	return true;
}

function encryptionType(authType, wepType)
{
	pflag = "1";

	if (authType.value == "1") // Shared Key
	{
		if (wepType.value == "0") wepLen = "64";
		else wepLen = "128";
	}
	else if (authType.value == "2") // WPA-PSK only
	{
		wepLen = "0";
	}
	else if (authType.value == "3") // WPA
	{
		wepLen = "0";
		pflag = "0";
	}
	else if (authType.value == "4") // Radius
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
	document.form.wl_phrase_x.value = "";

	if (o.value=="0")
	{
		wflag = 0;
		wep = "";

		document.form.wl_key1.value = wep;
		document.form.wl_key2.value = wep;
		document.form.wl_key3.value = wep;
		document.form.wl_key4.value = wep;
	}
	else
	{
		wflag = 1;

		if (o.value=="1")
		{
			wep = "0000000000";
		}
		else if (o.value=="2")
		{
			wep = "00000000000000000000000000";
		}
		else
		{
			wep = "00000000000000000000000000000000";
		}
		is_wlphrase("WLANConfig11b", "wl_phrase_x", document.form.wl_phrase_x);
	}

	inputCtrl(document.form.wl_phrase_x, wflag);
	inputCtrl(document.form.wl_key1, wflag);
	inputCtrl(document.form.wl_key2, wflag);
	inputCtrl(document.form.wl_key3, wflag);
	inputCtrl(document.form.wl_key4, wflag);
	inputCtrl(document.form.wl_key, wflag);

	wl_wep_change();

	if (wflag=="1")
	{
		document.form.wl_phrase_x.focus();
	}

	return true;
}

function change_widzard(o, id)
{
	if (document.form.current_page.value == "Advanced_PortTrigger_Content.asp")
	{
		for (i = 0; i < wItem.length; i++)
		{
			if (wItem[i][0]!==null)
			{
				if (o.value == wItem[i][0])
				{
					optIdx = i;

					document.form.autofw_outport_x_0.value = wItem[optIdx][1];
					document.form.autofw_outproto_x_0.value = wItem[optIdx][2];
					document.form.autofw_inport_x_0.value = wItem[optIdx][3];
					document.form.autofw_inproto_x_0.value = wItem[optIdx][4];
					document.form.autofw_desc_x_0.value = wItem[optIdx][0];
				}
			}
		}
	}
	else if (document.form.current_page.value == "Advanced_VirtualServer_Content.asp")
	{
		for (i = 0; i < wItem.length; i++)
		{
			if (wItem[i][0]!==null)
			{
				if (o.value == wItem[i][0])
				{
					optIdx = i;

					if (wItem[optIdx][2]=="TCP")
						document.form.vts_proto_x_0.options[0].selected = 1;
					else if (wItem[optIdx][2]=="UDP")
						document.form.vts_proto_x_0.options[1].selected = 1;
					else
						document.form.vts_proto_x_0.options[2].selected = 1;

					document.form.vts_ipaddr_x_0.value = ip;
					document.form.vts_port_x_0.value = wItem[optIdx][1];
					document.form.vts_desc_x_0.value = wItem[optIdx][0] + " Server (" + wItem[optIdx][1] + ")";
				}
			}
		}
	}
	else if (document.form.current_page.value == "Advanced_Firewall_Content.asp")
	{
		for (i = 0; i < wItem.length; i++)
		{
			if (wItem[i][0]!==null)
			{
				if (o.value == wItem[i][0])
				{
					optIdx = i;

					if ( id == "WLKnownApps")
					{
						if (wItem[optIdx][2]=="TCP")
							document.form.filter_wl_proto_x_0.options[0].selected = 1;
						else if (wItem[optIdx][2]=="UDP")
							document.form.filter_wl_proto_x_0.options[8].selected = 1;

						document.form.filter_wl_srcport_x_0.value = wItem[optIdx][1];
					}
					else // LWKnownApps
					{
						if (wItem[optIdx][2]=="TCP")
							document.form.filter_lw_proto_x_0.options[0].selected = 1;
						else if (wItem[optIdx][2]=="UDP")
							document.form.filter_lw_proto_x_0.options[8].selected = 1;

						document.form.filter_lw_dstport_x_0.value = wItem[optIdx][1];
					}
				}
			}
		}
	}
}




function is_wlkey(event, o, s)
{
	keyPressed = NavKey(event);

	if (keyPressed === 0) return true;

	window.top.pageChanged = 1;

	if ((keyPressed>47 && keyPressed<58)||(keyPressed>64 && keyPressed<71)||(keyPressed>96 && keyPressed<103))
	{
		if (document.form.wl_wep_x.value == "1")
		{
			if(o.value !== "" && o.value.length > 10) return false;
		}
		else if (document.form.wl_wep_x.value == "2")
		{
			if(o.value !== "" && o.value.length > 26) return false;
		}
		else return false;
	}
	else return false;
	return true;
}

function change_wlkey(o, s)
{
	if (document.form.wl_wep_x.value == "1")
	{
		if(o.value.length > 10) o.value = o.value.substring(0, 10);
	}
	else if (document.form.wl_wep_x.value == "2")
	{
		if(o.value.length > 26) o.value = o.value.substring(0, 26);
	}
	else return false;
	return true;
}

function validate_timerange(o, p)
{
	if (o.value.length===0) o.value = "00";
	else if (o.value.length==1) o.value = "0" + o.value;

	if (o.value.charAt(0)<'0' || o.value.charAt(0)>'9') o.value = "00";
	else if (o.value.charAt(1)<'0' || o.value.charAt(1)>'9') o.value = "00";
	else if (p===0 || p==2)
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
	if (document.form.wl_wep_x.value == "1")
	{
		if(o.value.length == 10) return true;
		o.value = "0000000000";
	}
	else if (document.form.wl_wep_x.value == "2")
	{
		if(o.value.length == 26) return true;
		o.value = "00000000000000000000000000";
	}
	else return true;

	alert('Please enter the complete WEP key.');
	return false;
}

/* Factory Reset Warning */
function confirmRestore(){
	if(confirm('WARNING:\nAll your settings will be lost!\nProceed or not?')) {
		/* top.location.href="apply.cgi"; */
		return true;
	}
	else
	{
		top.location.href = "Advanced_Content.html";
		return false;
	}
}

/*
 * function is_wlphrase(o) { var pseed = new Array(4); var wep_key = new
 * Array(8);
 *
 * if(document.form.WLANConfig11a_WEPType.value!='None') { for(i=0; i<o.value.length;
 * i++) { pseed[i%4]^= o.value.charAt(i); }
 *
 * randNumber = pseed[0]|(pseed[1]<<8)|(pseed[2]<<16)|(pseed[3]<<24);
 * document.form.WLANConfig11a_WEPKey1.value = '';
 *
 * for (j=0; j<5; j++) { randNumber *= 0x343fd; randNumber += 0x269ec3; keystr =
 * ((randNumber>>8) & 0xff);
 *
 * if (keystr>10) { if (keystr=10) keystr='A'; else if (keystr=11) keystr='B';
 * else if (keystr=12) keystr='C'; else if (keystr=13) keystr='D'; else if
 * (keystr=14) keystr='E'; else if (keystr=15) keystr='F'; }
 * document.form.WLANConfig11a_WEPKey1.value += keystr;
 *
 * keystr = ((randNumber>>16) & 0x7f);
 *
 * if (keystr>10) { if (keystr=10) keystr='A'; else if (keystr=11) keystr='B';
 * else if (keystr=12) keystr='C'; else if (keystr=13) keystr='D'; else if
 * (keystr=14) keystr='E'; else if (keystr=15) keystr='F'; }
 * document.form.WLANConfig11a_WEPKey1.value += keystr;
 * } } }
 */

function validate_wlphrase(s, v, o)
{
	if (v == "wl_wpa_psk")
	{
		if (document.form.wl_auth_mode.value == "psk")
		{
			if (o.value.length < 8)
			{
				alert("Pre-shared key should be more than 7 characters!!! If you leave this field blank, system will assign 00000000 as your passphrase.");

				document.form.wl_wpa_psk.value = "00000000";
				return false;
			}
		}

	}
	else
	{
		if (!validate_string(o))
		{
			is_wlphrase(s, v, o);
			return(false);
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
//	So Help page will be named as XXXXXX_Widzard.asp

//	urlstr = name + "Widzard.asp"
	urlstr = "Advanced_" + name + "_Widzard.asp";

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
	/*
	 * Rule : If addresses in pool are not match to subnet, change to
	 * subnet.2~subnet.254
	 */
//	if (!matchSubnet(document.form.LANHostConfig_x_LDNSServer2.value,
//	document.form.dhcp_start.value, 3) ||
//	!matchSubnet(document.form.LANHostConfig_x_LDNSServer2.value,
//	document.form.dhcp_end.value, 3))
//	{
//	document.form.dhcp_start.value =
//	subnetPrefix(document.form.LANHostConfig_x_LDNSServer2.value,
//	document.form.dhcp_start.value, 3);
//	document.form.dhcp_end.value =
//	subnetPrefix(document.form.LANHostConfig_x_LDNSServer2.value,
//	document.form.dhcp_end.value, 3);
//	}
	if (!matchSubnet(document.form.lan_ipaddr.value, document.form.dhcp_start.value, 3) ||
			!matchSubnet(document.form.lan_ipaddr.value, document.form.dhcp_end.value, 3))
	{
		if (confirm("The setting of DHCP server does not match current IP address of LAN. Would you like to change it automatically?"))
		{
			document.form.dhcp_start.value = subnetPrefix(document.form.lan_ipaddr.value, document.form.dhcp_start.value, 3);
			document.form.dhcp_end.value = subnetPrefix(document.form.lan_ipaddr.value, document.form.dhcp_end.value, 3);
		}
	}
}

function checkDmzSubnet()
{
	/* Rule : If addresses in pool are match to subnet, don't change */
	/*
	 * Rule : If addresses in pool are not match to subnet, change to
	 * subnet.2~subnet.254
	 */
//	if (!matchSubnet(document.form.LANHostConfig_x_DmzLDNSServer2.value,
//	document.form.LANHostConfig_DmzMinAddress.value, 3) ||
//	!matchSubnet(document.form.LANHostConfig_x_DmzLDNSServer2.value,
//	document.form.LANHostConfig_DmzMaxAddress.value, 3))
//	{
//	document.form.LANHostConfig_DmzMinAddress.value =
//	subnetPrefix(document.form.LANHostConfig_x_DmzLDNSServer2.value,
//	document.form.LANHostConfig_DmzMinAddress.value, 3);
//	document.form.LANHostConfig_DmzMaxAddress.value =
//	subnetPrefix(document.form.LANHostConfig_x_DmzLDNSServer2.value,
//	document.form.LANHostConfig_DmzMaxAddress.value, 3);
//	}
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
		if (window.top.isModel()!="WL520" && window.top.isModel()!="SnapAP")
		{
			document.form.filter_wl_date_x.value = setDateCheck(
					document.form.filter_wl_date_x_Sun,
					document.form.filter_wl_date_x_Mon,
					document.form.filter_wl_date_x_Tue,
					document.form.filter_wl_date_x_Wed,
					document.form.filter_wl_date_x_Thu,
					document.form.filter_wl_date_x_Fri,
					document.form.filter_wl_date_x_Sat);

			document.form.filter_wl_time_x.value = setTimeRange(
					document.form.filter_wl_time_x_starthour,
					document.form.filter_wl_time_x_startmin,
					document.form.filter_wl_time_x_endhour,
					document.form.filter_wl_time_x_endmin);
		}
		document.form.filter_lw_date_x.value = setDateCheck(
				document.form.filter_lw_date_x_Sun,
				document.form.filter_lw_date_x_Mon,
				document.form.filter_lw_date_x_Tue,
				document.form.filter_lw_date_x_Wed,
				document.form.filter_lw_date_x_Thu,
				document.form.filter_lw_date_x_Fri,
				document.form.filter_lw_date_x_Sat);

		document.form.filter_lw_time_x.value = setTimeRange(
				document.form.filter_lw_time_x_starthour,
				document.form.filter_lw_time_x_startmin,
				document.form.filter_lw_time_x_endhour,
				document.form.filter_lw_time_x_endmin);
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
		document.form.url_date_x.value = setDateCheck(
				document.form.url_date_x_Sun,
				document.form.url_date_x_Mon,
				document.form.url_date_x_Tue,
				document.form.url_date_x_Wed,
				document.form.url_date_x_Thu,
				document.form.url_date_x_Fri,
				document.form.url_date_x_Sat);

		document.form.url_time_x.value = setTimeRange(
				document.form.url_time_x_starthour,
				document.form.url_time_x_startmin,
				document.form.url_time_x_endhour,
				document.form.url_time_x_endmin);
	}
	else if (s == "Advanced_WebCam_Content.asp")
	{
		document.form.usb_websecurity_date_x.value = setDateCheck(
				document.form.usb_websecurity_date_x_Sun,
				document.form.usb_websecurity_date_x_Mon,
				document.form.usb_websecurity_date_x_Tue,
				document.form.usb_websecurity_date_x_Wed,
				document.form.usb_websecurity_date_x_Thu,
				document.form.usb_websecurity_date_x_Fri,
				document.form.usb_websecurity_date_x_Sat);

		document.form.usb_websecurity_time_x.value = setTimeRange(
				document.form.usb_websecurity_time_x_starthour,
				document.form.usb_websecurity_time_x_startmin,
				document.form.usb_websecurity_time_x_endhour,
				document.form.usb_websecurity_time_x_endmin);
	}
	else if (s == "Advanced_WAdvanced_Content.asp")
	{
		if (window.top.isModel()!="WL520" && window.top.isModel()!="SnapAP")
		{
			document.form.wl_radio_date_x.value = setDateCheck(
					document.form.wl_radio_date_x_Sun,
					document.form.wl_radio_date_x_Mon,
					document.form.wl_radio_date_x_Tue,
					document.form.wl_radio_date_x_Wed,
					document.form.wl_radio_date_x_Thu,
					document.form.wl_radio_date_x_Fri,
					document.form.wl_radio_date_x_Sat);

			document.form.wl_radio_time_x.value = setTimeRange(
					document.form.wl_radio_time_x_starthour,
					document.form.wl_radio_time_x_startmin,
					document.form.wl_radio_time_x_endhour,
					document.form.wl_radio_time_x_endmin);
		}
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
		if (document.form.ddns_server_x.value.indexOf("WWW.DYNDNS.ORG")!=-1)
			tourl = "https://www.dyndns.com/account/create.html";
		else if (document.form.ddns_server_x.value == 'WWW.TZO.COM')
			tourl = "http://signup.tzo.com";
		else if (document.form.ddns_server_x.value == 'WWW.ZONEEDIT.COM')
			tourl = "https://www.zoneedit.com/signup.html?";
		else if (document.form.ddns_server_x.value == 'WWW.EASYDNS.COM')
			tourl = "https://web.easydns.com/Open_Account/";
		else if (document.form.ddns_server_x.value == 'WWW.NO-IP.COM')
			tourl = "http://www.no-ip.com/newUser.php";
		else if (document.form.ddns_server_x.value == 'WWW.DNSOMATIC.COM')
			tourl = "https://www.dnsomatic.com/create/";
		else if (document.form.ddns_server_x.value == 'WWW.TUNNELBROKER.NET')
			tourl = "http://www.tunnelbroker.net/register.php";
		else if (document.form.ddns_server_x.value == 'DNS.HE.NET')
			tourl = "http://ipv6.he.net/certification/register.php";
		else
			return;

		link = window.open(tourl, "DDNSLink",
		"toolbar=yes,location=yes,directories=no,status=yes,menubar=yes,scrollbars=yes,resizable=yes,copyhistory=no,width=640,height=480");
	}
	else if (s=='x_NTPServer1')
	{
//		tourl = "http://ntp.isc.org/bin/view/Servers/WebHome";
		tourl = "http://support.ntp.org/bin/view/Servers/NTPPoolServers";

		link = window.open(tourl, "NTPLink",
		"toolbar=yes,location=yes,directories=no,status=yes,menubar=yes,scrollbars=yes,resizable=yes,copyhistory=no,width=640,height=480");
	}
	else if (s=='x_WImageStatic')
	{
		tourl = "ShowWebCam.asp";
		link = window.open(tourl, "WebCamera",
		"toolbar=yes,location=yes,directories=no,status=yes,menubar=yes,scrollbars=yes,resizable=yes,copyhistory=no,width=640,height=560");
	}
	else if (s=='x_WRemote')
	{
		tourl = "Advanced_RemoteControl_Widzard.asp";
		link = window.open(tourl, "RemoteMonitor",
				"toolbar=no,location=no,directories=no,status=no,menubar=yes,scrollbars=yes,resizable=yes,copyhistory=no,top=0,left=0,width=" + screen.width + ",height=" + screen.height);
	}
	else if (s=='x_FIsAnonymous' || s=='x_FIsSuperuser')
	{
		urlstr = location.href;
		url = urlstr.indexOf("http://");

//		user = '<% nvram_x("General", "x_Username"); %>';
//		LANIP= '<% nvram_x("lan.log", "LANIPAddress"); %>';
		port = document.form.usb_ftpport_x.value;

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

/* Used when WEP is changed */
function changeWEPType()
{
	if (document.form.wl_wep.value == "0")
	{
		flag = 0;
	}
	else
	{
		flag = 1;
	}
	inputCtrl(document.form.wl_phrase_x, flag);
	inputCtrl(document.form.wl_key1, flag);
	inputCtrl(document.form.wl_key2, flag);
	inputCtrl(document.form.wl_key3, flag);
	inputCtrl(document.form.wl_key4, flag);
	inputCtrl(document.form.wl_key, flag);
}

/* Used when Authentication Method is changed */
function changeAuthType()
{
	if (document.form.wl_auth_mode.value == "shared")
	{
		inputCtrl(document.form.wl_crypto, 0);
		inputCtrl(document.form.wl_wpa_psk, 0);
		inputCtrl(document.form.wl_wep, 1);
		inputCtrl(document.form.wl_phrase_x, 1);
		inputCtrl(document.form.wl_key1, 1);
		inputCtrl(document.form.wl_key2, 1);
		inputCtrl(document.form.wl_key3, 1);
		inputCtrl(document.form.wl_key4, 1);
		inputCtrl(document.form.wl_key, 1);
		inputCtrl(document.form.wl_wpa_gtk_rekey, 0);
	}
	else if (document.form.wl_auth_mode.value == "psk")
	{
		inputCtrl(document.form.wl_crypto, 1);
		inputCtrl(document.form.wl_wpa_psk, 1);
		inputCtrl(document.form.wl_wep, 1);

		inputCtrl(document.form.wl_phrase_x, 1);
		inputCtrl(document.form.wl_key1, 1);
		inputCtrl(document.form.wl_key2, 1);
		inputCtrl(document.form.wl_key3, 1);
		inputCtrl(document.form.wl_key4, 1);
		inputCtrl(document.form.wl_key, 1);
		inputCtrl(document.form.wl_wpa_gtk_rekey, 0);
	}
	else if (document.form.wl_auth_mode.value == "wpa" ||
			document.form.wl_auth_mode.value == "wpa2")
	{
		inputCtrl(document.form.wl_crypto, 0);
		inputCtrl(document.form.wl_wpa_psk, 0);
		inputCtrl(document.form.wl_wep, 0);

		inputCtrl(document.form.wl_phrase_x, 0);
		inputCtrl(document.form.wl_key1, 0);
		inputCtrl(document.form.wl_key2, 0);
		inputCtrl(document.form.wl_key3, 0);
		inputCtrl(document.form.wl_key4, 0);
		inputCtrl(document.form.wl_key, 0);
		inputCtrl(document.form.wl_wpa_gtk_rekey, 0);
	}
	else if (document.form.wl_auth_mode.value == "radius")
	{
		inputCtrl(document.form.wl_crypto, 1);
		inputCtrl(document.form.wl_wpa_psk, 1);
		inputCtrl(document.form.wl_wep, 1);

		inputCtrl(document.form.wl_phrase_x, 1);
		inputCtrl(document.form.wl_key1, 1);
		inputCtrl(document.form.wl_key2, 1);
		inputCtrl(document.form.wl_key3, 1);
		inputCtrl(document.form.wl_key4, 1);
		inputCtrl(document.form.wl_key, 1);
		inputCtrl(document.form.wl_wpa_gtk_rekey, 0);
	}
	else
	{
		inputCtrl(document.form.wl_crypto, 0);
		inputCtrl(document.form.wl_wpa_psk, 0);
		inputCtrl(document.form.wl_wep, 1);

		inputCtrl(document.form.wl_phrase_x, 1);
		inputCtrl(document.form.wl_key1, 1);
		inputCtrl(document.form.wl_key1, 1);
		inputCtrl(document.form.wl_key1, 1);
		inputCtrl(document.form.wl_key1, 1);
		inputCtrl(document.form.wl_key1, 1);
		inputCtrl(document.form.wl_key, 1);
		inputCtrl(document.form.wl_wpa_gtk_rekey, 0);
	}
}

/* input : s: service id, v: value name, o: current value */
/* output: wep key1~4 */
function is_wlphrase(s, v, o)
{
	var pseed = new Array;
	var wep_key = new Array(5);

	window.top.pageChanged = 1;

	if (v=='wl_wpa_psk') return(valid_WPAPSK(o));

	wepType = document.form.wl_wep_x.value;
	wepKey1 = document.form.wl_key1;
	wepKey2 = document.form.wl_key2;
	wepKey3 = document.form.wl_key3;
	wepKey4 = document.form.wl_key4;

	phrase = o.value;

	if(wepType == "1")
	{
		for (i=0; i<phrase.length; i++)
		{
			pseed[i%4] ^= phrase.charCodeAt(i);
		}

		randNumber = pseed[0] | (pseed[1]<<8) | (pseed[2]<<16) | (pseed[3]<<24);

		for (j=0; j<5; j++)
		{
			randNumber = ((randNumber * 0x343fd) % 0x1000000);
			randNumber = ((randNumber + 0x269ec3) % 0x1000000);
			wep_key[j] = ((randNumber>>16) & 0xff);
		}

		wepKey1.value = binl2hex_c(wep_key);

		for (j=0; j<5; j++)
		{
			randNumber = ((randNumber * 0x343fd) % 0x1000000);
			randNumber = ((randNumber + 0x269ec3) % 0x1000000);
			wep_key[j] = ((randNumber>>16) & 0xff);
		}

		wepKey2.value = binl2hex_c(wep_key);

		for (j=0; j<5; j++)
		{
			randNumber = ((randNumber * 0x343fd) % 0x1000000);
			randNumber = ((randNumber + 0x269ec3) % 0x1000000);
			wep_key[j] = ((randNumber>>16) & 0xff);
		}
		wepKey3.value = binl2hex_c(wep_key);

		for (j=0; j<5; j++)
		{
			randNumber = ((randNumber * 0x343fd) % 0x1000000);
			randNumber = ((randNumber + 0x269ec3) % 0x1000000);
			wep_key[j] = ((randNumber>>16) & 0xff);
		}
		wepKey4.value = binl2hex_c(wep_key);
	}
	else if (wepType == "2" || wepType == "3")
	{
		password = "";

		if (phrase.length>0)
		{
			for (i=0; i<64; i++)
			{
				ch = phrase.charAt(i%phrase.length);
				password = password + ch;
			}
		}

		password = calcMD5(password);

		if (wepType == "2")
		{
			wepKey1.value = password.substr(0, 26);
		}
		else
		{
			wepKey1.value = password.substr(0, 32);
		}

		wepKey2.value = wepKey1.value;
		wepKey3.value = wepKey1.value;
		wepKey4.value = wepKey1.value;
	}
	return true;
}

/*
 * function wl_channel_list_change(countr) { var phytype =
 * document.forms.wl_phytype[document.forms[0].wl_phytype.selectedIndex].value;
 * var country =
 * document.forms.wl_country_code[document.forms[0].wl_country_code.selectedIndex].value;
 * var channels = new Array(0); var cur = 0; var sel = 0;
 *
 *
 * for (i = 0; i < document.forms[0].wl_channel.length; i++) { if
 * (document.forms[0].wl_channel[i].selected) { cur =
 * document.forms[0].wl_channel[i].value; break; } }
 * <% wl_channel_list("b"); %>
 *
 *
 * document.forms[0].wl_channel.length = channels.length; for (var i in
 * channels) { if (channels[i] == 0) document.forms[0].wl_channel[i] = new
 * Option("Auto", channels[i]); else document.forms[0].wl_channel[i] = new
 * Option(channels[i], channels[i]); document.forms[0].wl_channel[i].value =
 * channels[i]; if (channels[i] == cur) {
 * document.forms[0].wl_channel[i].selected = true; sel = 1; } }
 *
 * if (sel == 0) document.forms[0].wl_channel[0].selected = true; }
 */


function wl_wep_change()
{
	var mode = document.form.wl_auth_mode.value;
	var wep = document.form.wl_wep_x.value;

	if (window.top.isModel()=="WL520" || window.top.isModel()=="SnapAP" || window.top.isCard()=="ralink")
	{
		if (mode == "wpa" || mode == "wpa2" || mode == "psk" || mode == "radius")
		{
			inputCtrl(document.form.wl_crypto, 1);
			inputCtrl(document.form.wl_wpa_psk, 1);
			inputCtrl(document.form.wl_wpa_gtk_rekey, 1);

			inputCtrl(document.form.wl_wep_x, 0);
			inputCtrl(document.form.wl_phrase_x, 0);
			inputCtrl(document.form.wl_key1, 0);
			inputCtrl(document.form.wl_key2, 0);
			inputCtrl(document.form.wl_key3, 0);
			inputCtrl(document.form.wl_key4, 0);
			inputCtrl(document.form.wl_key, 0);
		}
		else
		{
			inputCtrl(document.form.wl_crypto, 0);
			inputCtrl(document.form.wl_wpa_psk, 0);
			inputCtrl(document.form.wl_wpa_gtk_rekey, 0);
			inputCtrl(document.form.wl_wep_x, 1);

			if (wep != "0")
			{
				inputCtrl(document.form.wl_phrase_x, 1);
				inputCtrl(document.form.wl_key1, 1);
				inputCtrl(document.form.wl_key2, 1);
				inputCtrl(document.form.wl_key3, 1);
				inputCtrl(document.form.wl_key4, 1);
				inputCtrl(document.form.wl_key, 1);
			}
			else {
				inputCtrl(document.form.wl_phrase_x, 0);
				inputCtrl(document.form.wl_key1, 0);
				inputCtrl(document.form.wl_key2, 0);
				inputCtrl(document.form.wl_key3, 0);
				inputCtrl(document.form.wl_key4, 0);
				inputCtrl(document.form.wl_key, 0);
			}
		}
	}
	else
	{
		/* enable/disable network key 1 to 4 */
		if (wep != "0") {
			inputCtrl(document.form.wl_phrase_x, 1);
			if (mode == "wpa" || mode == "wpa2" || mode == "psk" || mode == "radius") {
				inputCtrl(document.form.wl_key1, 0);
				inputCtrl(document.form.wl_key4, 0);

			}
			else {
				inputCtrl(document.form.wl_key1, 1);
				inputCtrl(document.form.wl_key4, 1);
			}
			inputCtrl(document.form.wl_key2, 1);
			inputCtrl(document.form.wl_key3, 1);
		} else
		{
			inputCtrl(document.form.wl_phrase_x, 0);
			inputCtrl(document.form.wl_key1, 0);
			inputCtrl(document.form.wl_key2, 0);
			inputCtrl(document.form.wl_key3, 0);
			inputCtrl(document.form.wl_key4, 0);
		}

		/* enable/disable key index */
		if (wep != "0")
			inputCtrl(document.form.wl_key, 1);
		else
			inputCtrl(document.form.wl_key, 0);

		/* enable/disable gtk rotation interval */
		if (wep != "0")
			inputCtrl(document.form.wl_wpa_gtk_rekey, 0);
		else {
			if (mode == "wpa" || mode == "wpa2" || mode == "psk")
				inputCtrl(document.form.wl_wpa_gtk_rekey, 1);
			else
				inputCtrl(document.form.wl_wpa_gtk_rekey, 0);
		}
	}
}

function change_wep_type(mode)
{
	var orig = document.form.wl_wep_x.value;

	free_options(document.form.wl_wep_x);

	if (mode == "shared")
	{
		vitems = new Array("1", "2");
		items = new Array("WEP-64bits", "WEP-128bits");
	}
	else
	{
		vitems = new Array("0", "1", "2");
		items = new Array("None", "WEP-64bits", "WEP-128bits");

	}
	add_options_x2(document.form.wl_wep_x, vitems, items, orig);

	if (mode == "shared" && orig == "0")
	{
		change_wlweptype(document.form.wl_wep_x, "WLANConfig11b");
	}
}

function wl_auth_mode_reconf()
{
	if (document.form.wl_auth_mode.value=="radius" ||
			document.form.wl_auth_mode.value=="wpa" ||
			document.form.wl_auth_mode.value=="wpa2")
		document.form.wl_auth_mode.value="open";

	document.form.wl_auth_mode.options[3].value = null;
	document.form.wl_auth_mode.options[3] = null;
	document.form.wl_auth_mode.options[3].value = null;
	document.form.wl_auth_mode.options[3] = null;
}

function wl_auth_mode_change(isload)
{
	var mode = document.form.wl_auth_mode.value;
	var i, j, cur, algos;

	inputCtrl(document.form.wl_wep_x, 1);

	/* enable/disable crypto algorithm */
	if (mode == "wpa" || mode == "wpa2" || mode == "psk")
		inputCtrl(document.form.wl_crypto, 1);
	else
		inputCtrl(document.form.wl_crypto, 0);

	/* enable/disable psk passphrase */
	if (mode == "psk")
		inputCtrl(document.form.wl_wpa_psk, 1);
	else
		inputCtrl(document.form.wl_wpa_psk, 0);

	/* update wl_crypto */
	if (mode == "wpa" || mode == "wpa2" || mode == "psk") {
		/* Save current crypto algorithm */
		for (i = 0; i < document.form.wl_crypto.length; i++) {
			if (document.form.wl_crypto[i].selected) {
				cur = document.form.wl_crypto[i].value;
				break;
			}
		}

		if (window.top.isModel()=="WL520" || window.top.isModel()=="SnapAP" || window.top.isBand() == 'b' )
			algos = new Array("TKIP");
		else algos = new Array("TKIP", "AES", "TKIP+AES");

		/* Reconstruct algorithm array from new crypto algorithms */
		document.form.wl_crypto.length = algos.length;
		for (j in algos) {
			document.form.wl_crypto[j] = new Option(algos[j], algos[j].toLowerCase());
			document.form.wl_crypto[j].value = algos[j].toLowerCase();
			if (algos[j].toLowerCase() == cur)
				document.form.wl_crypto[j].selected = true;
		}
	}

	change_wep_type(mode);

	/* Save current network key index */
	for (i = 0; i < document.form.wl_key.length; i++)
	{
		if (document.form.wl_key[i].selected) {
			cur = document.form.wl_key[i].value;
			break;
		}
	}

	/* Define new network key indices */
	if (mode == "wpa" || mode == "wpa2" || mode == "psk" || mode == "radius")
		algos = new Array("2", "3");
	else
	{
		algos = new Array("1", "2", "3", "4");

		if (!isload) cur = "1";
	}

	/* Reconstruct network key indices array from new network key indices */
	document.form.wl_key.length = algos.length;
	for ( j in algos) {
		document.form.wl_key[j] = new Option(algos[j], algos[j]);
		document.form.wl_key[j].value = algos[j];
		if (algos[j] == cur)
			document.form.wl_key[j].selected = true;
	}
	wl_wep_change();

	if ((mode == "wpa" || mode == "wpa2" || mode == "psk"))
	{
//		if (window.top.isModel()=="WL520" || window.top.isModel()=="SnapAP" ||
//		window.top.isBand()=='b' )
//		{
//		} else
		if(document.form.wl_crypto[2].selected === true)
		{
			document.form.wl_wep_x.value = 0;
//			change_wlweptype(document.form.wl_wep_x, "WLANConfig11b");

			inputCtrl(document.form.wl_wep_x, 0);
			inputCtrl(document.form.wl_phrase_x, 0);
			inputCtrl(document.form.wl_key1, 0);
			inputCtrl(document.form.wl_key2, 0);
			inputCtrl(document.form.wl_key3, 0);
			inputCtrl(document.form.wl_key4, 0);
			inputCtrl(document.form.wl_key, 0);
		}
	}
}

function wl_rate_change()
{
	var orig = document.form.wl_rate.value;

	free_options(document.form.wl_rate);

//	gmode :
//	0 : Auto
//	1 : G only
//	2 : LRS
//	3 : B only
	if (document.form.wl_gmode.value == "1")
	{
		vitems = new Array("0", "1000000", "2000000", "5500000", "6000000", "9000000", "11000000", "12000000", "18000000", "24000000", "36000000", "48000000", "54000000");
		items = new Array("Auto", "1", "2", "5.5", "6", "9", "11", "12", "18", "24", "36", "48", "54");
	}
	else if (document.form.wl_gmode.value == "4")
	{
		vitems = new Array("0", "6000000", "9000000", "12000000", "18000000", "24000000", "36000000", "48000000", "54000000");
		items = new Array("Auto", "6", "9", "12", "18", "24", "36", "48", "54");
	}
	else
	{
		vitems = new Array("0", "1000000", "2000000", "5500000", "11000000");
		items = new Array("Auto", "1", "2", "5.5", "11");
	}
	add_options_x2(document.form.wl_rate, vitems, items, orig);
}

function change_wan_type(v)
{
	var frm = document.form;
	if (v == "static")
	{
		inputCtrl(frm.wan_ipaddr, 1);
		inputCtrl(frm.wan_netmask, 1);
		inputCtrl(frm.wan_gateway, 1);
		inputCtrl(frm.wan_auth_x, 1);
		inputCtrl(frm.wan_pppoe_username, frm.wan_auth_x.value == "" ? 0 : 1);
		inputCtrl(frm.wan_pppoe_passwd, frm.wan_auth_x.value == "" ? 0 : 1);
		inputCtrl(frm.wan_pppoe_idletime, 0);
		inputCtrl(frm.wan_pppoe_mtu, 0);
		inputCtrl(frm.wan_pppoe_mru, 0);
		inputRCtrl1(frm.wan_pppoe_relay_x, 1);
		inputCtrl(frm.wan_pppoe_idletime_check, 0);
		inputCtrl(frm.wan_pppoe_service, 0);
		inputCtrl(frm.wan_pppoe_ac, 0);
		inputCtrl(frm.wan_pppoe_options_x, 0);
		inputCtrl(frm.wan_pptp_options_x, 0);
//		inputCtrl(frm.wan_hostname, 0);
//		inputCtrl(frm.wan_hwaddr_x, 0);
		inputRCtrl1(frm.x_DHCPClient, 0);
		inputRCtrl2(frm.x_DHCPClient, 1);
	}
	else if(v == "pppoe")
	{
		inputCtrl(frm.wan_ipaddr, 1);
		inputCtrl(frm.wan_netmask, 1);
		inputCtrl(frm.wan_gateway, 1);
		inputCtrl(frm.wan_auth_x, 0);
		inputCtrl(frm.wan_pppoe_username, 1);
		inputCtrl(frm.wan_pppoe_passwd, 1);
		inputCtrl(frm.wan_pppoe_idletime, 1);
		inputCtrl(frm.wan_pppoe_idletime_check, 1);
		inputCtrl(frm.wan_pppoe_mtu, 1);
		inputCtrl(frm.wan_pppoe_mru, 1);
		inputCtrl(frm.wan_pppoe_service, 1);
		inputCtrl(frm.wan_pppoe_ac, 1);
		inputRCtrl1(frm.wan_pppoe_relay_x, 1);
		inputCtrl(frm.wan_pppoe_options_x, 1);
		inputCtrl(frm.wan_pptp_options_x, 0);
//		inputCtrl(frm.wan_hostname, 1);
//		inputCtrl(frm.wan_hwaddr_x, 1);
		inputRCtrl1(frm.x_DHCPClient, 1);
		inputRCtrl2(frm.x_DHCPClient, 1);
	}
	else if(v == "pptp")
	{
		inputCtrl(frm.wan_ipaddr, 1);
		inputCtrl(frm.wan_netmask, 1);
		inputCtrl(frm.wan_gateway, 1);
		inputCtrl(frm.wan_auth_x, 0);
		inputCtrl(frm.wan_pppoe_username, 1);
		inputCtrl(frm.wan_pppoe_passwd, 1);
		inputCtrl(frm.wan_pppoe_idletime, 1);
		inputCtrl(frm.wan_pppoe_idletime_check, 1);
		inputCtrl(frm.wan_pppoe_mtu, 0);
		inputCtrl(frm.wan_pppoe_mru, 0);
		inputCtrl(frm.wan_pppoe_service, 0);
		inputCtrl(frm.wan_pppoe_ac, 0);
		inputRCtrl1(frm.wan_pppoe_relay_x, 1);
		inputCtrl(frm.wan_pppoe_options_x, 1);
		inputCtrl(frm.wan_pptp_options_x, 1);
//		inputCtrl(frm.wan_hostname, 0);
//		inputCtrl(frm.wan_hwaddr_x, 0);
		inputRCtrl1(frm.x_DHCPClient, 1);
		inputRCtrl2(frm.x_DHCPClient, 1);
	} else if(v == "l2tp")
	{
		inputCtrl(frm.wan_ipaddr, 1);
		inputCtrl(frm.wan_netmask, 1);
		inputCtrl(frm.wan_gateway, 1);
		inputCtrl(frm.wan_auth_x, 0);
		inputCtrl(frm.wan_pppoe_username, 1);
		inputCtrl(frm.wan_pppoe_passwd, 1);
		inputCtrl(frm.wan_pppoe_idletime, 0);
		inputCtrl(frm.wan_pppoe_idletime_check, 0);
		inputCtrl(frm.wan_pppoe_mtu, 0);
		inputCtrl(frm.wan_pppoe_mru, 0);
		inputCtrl(frm.wan_pppoe_service, 0);
		inputCtrl(frm.wan_pppoe_ac, 0);
		inputRCtrl1(frm.wan_pppoe_relay_x, 1);
		inputCtrl(frm.wan_pppoe_options_x, 1);
		inputCtrl(frm.wan_pptp_options_x, 0);
//		inputCtrl(frm.wan_hostname, 0);
//		inputCtrl(frm.wan_hwaddr_x, 0);
		inputRCtrl1(frm.x_DHCPClient, 1);
		inputRCtrl2(frm.x_DHCPClient, 1);
	}
	else if (v == "bigpond")
	{
		inputCtrl(frm.wan_ipaddr, 0);
		inputCtrl(frm.wan_netmask, 0);
		inputCtrl(frm.wan_gateway, 0);
		inputCtrl(frm.wan_auth_x, 0);
		inputCtrl(frm.wan_pppoe_username, 1);
		inputCtrl(frm.wan_pppoe_passwd, 1);
		inputCtrl(frm.wan_pppoe_idletime, 0);
		inputCtrl(frm.wan_pppoe_idletime_check, 0);
		inputCtrl(frm.wan_pppoe_mtu, 0);
		inputCtrl(frm.wan_pppoe_mru, 0);
		inputCtrl(frm.wan_pppoe_service, 0);
		inputCtrl(frm.wan_pppoe_ac, 0);
		inputRCtrl1(frm.wan_pppoe_relay_x, 1);
		inputCtrl(frm.wan_pppoe_options_x, 0);
		inputCtrl(frm.wan_pptp_options_x, 0);
//		inputCtrl(frm.wan_hostname, 1);
//		inputCtrl(frm.wan_hwaddr_x, 1);
		inputRCtrl1(frm.x_DHCPClient, 0);
		inputRCtrl2(frm.x_DHCPClient, 0);
	}
	else if (v == "wimax" || v=="usbnet")
	{
		inputCtrl(frm.wan_ipaddr, 1);
		inputCtrl(frm.wan_netmask, 1);
		inputCtrl(frm.wan_gateway, 1);
		inputCtrl(frm.wan_auth_x, 0);
		inputCtrl(frm.wan_pppoe_username, 0);
		inputCtrl(frm.wan_pppoe_passwd, 0);
		inputCtrl(frm.wan_pppoe_idletime, 0);
		inputCtrl(frm.wan_pppoe_mtu, 0);
		inputCtrl(frm.wan_pppoe_mru, 0);
		inputRCtrl1(frm.wan_pppoe_relay_x, 1);
		inputCtrl(frm.wan_pppoe_idletime_check, 0);
		inputCtrl(frm.wan_pppoe_service, 0);
		inputCtrl(frm.wan_pppoe_ac, 0);
		inputCtrl(frm.wan_pppoe_options_x, 0);
		inputCtrl(frm.wan_pptp_options_x, 0);
//		inputCtrl(frm.wan_hostname, 0);
//		inputCtrl(frm.wan_hwaddr_x, 0);
		inputRCtrl1(frm.x_DHCPClient, 1);
		inputRCtrl2(frm.x_DHCPClient, 1);
	} else
	{
		inputCtrl(frm.wan_ipaddr, 0);
		inputCtrl(frm.wan_netmask, 0);
		inputCtrl(frm.wan_gateway, 0);
		inputCtrl(frm.wan_auth_x, 1);
		inputCtrl(frm.wan_pppoe_username, frm.wan_auth_x.value == "" ? 0 : 1);
		inputCtrl(frm.wan_pppoe_passwd, frm.wan_auth_x.value == "" ? 0 : 1);
		inputCtrl(frm.wan_pppoe_idletime, 0);
		inputCtrl(frm.wan_pppoe_idletime_check, 0);
		inputCtrl(frm.wan_pppoe_mtu, 0);
		inputCtrl(frm.wan_pppoe_mru, 0);
		inputCtrl(frm.wan_pppoe_service, 0);
		inputCtrl(frm.wan_pppoe_ac, 0);
		inputRCtrl1(frm.wan_pppoe_relay_x, 1);
		inputCtrl(frm.wan_pppoe_options_x, 0);
		inputCtrl(frm.wan_pptp_options_x, 0);
//		inputCtrl(frm.wan_hostname, 1);
//		inputCtrl(frm.wan_hwaddr_x, 1);
		inputRCtrl1(frm.x_DHCPClient, 0);
		inputRCtrl2(frm.x_DHCPClient, 0);
	}
	if ((v=="l2tp" || v=="pptp" || v=="wimax" || v=="usbmodem" || v=="usbnet") &&
			frm.wan_ipaddr.value === "")
	{
		inputRCtrl2(frm.x_DHCPClient, 0);
	}
	if ((v=="l2tp" || v=="pptp" || v=="pppoe" || v=="wimax" || v=="usbmodem" || v=="usbnet") &&
			frm.wan_ipaddr.value == "0.0.0.0")
	{
		inputRCtrl2(frm.x_DHCPClient, 0);
	}
	changeDHCPClient();
}

function changeDHCPClient()
{
	var frm = document.form;
	if (frm.x_DHCPClient[0].checked === true)
	{
		inputCtrl(frm.wan_ipaddr, 0);
		inputCtrl(frm.wan_netmask, 0);
		inputCtrl(frm.wan_gateway, 0);
		v = frm.wan_proto.value;
		if (v=="l2tp" || v=="pptp" || v=="pppoe" || v=="wimax" || v=="usbmodem" || v=="usbnet")
		{
			frm.wan_ipaddr.value = "0.0.0.0";
			frm.wan_netmask.value = "0.0.0.0";
			frm.wan_gateway.value = "0.0.0.0";
		}
	}
	else
	{
		inputCtrl(frm.wan_ipaddr, 1);
		inputCtrl(frm.wan_netmask, 1);
		inputCtrl(frm.wan_gateway, 1);
		if (frm.wan_ipaddr.value == "0.0.0.0")
		{
			frm.wan_ipaddr.value = "";
			frm.wan_netmask.value = "";
			frm.wan_gateway.value = "";
		}
	}
}

function change_ipv6_wanauto_x(enable)
{
	var frm = document.form;
	var val = (!enable || frm.ipv6_wanauto_x[0].checked) ? 0 : 1;

	inputCtrl(frm.ipv6_wan_addr, val);
	inputCtrl(frm.ipv6_wan_netsize, val);
	inputCtrl(frm.ipv6_wan_router, val);
}

function change_ipv6_lanauto_x(enable)
{
	var frm = document.form;
	var val = (!enable || frm.ipv6_lanauto_x[0].checked) ? 0 : 1;

	inputCtrl(frm.ipv6_lan_addr, val);
	inputCtrl(frm.ipv6_lan_netsize, val);
}

function change_ipv6_type(v)
{
	var frm = document.form;
	var enable = 1;
	var tunnel = 0;
	var ppp = (frm.wan_proto.value == "pptp" ||
		   frm.wan_proto.value == "l2tp" ||
		   frm.wan_proto.value == "pppoe") ? 1 : 0;

	if (v == "native")
	{
		inputCtrl(frm.ipv6_if_x, ppp);
		inputRCtrl1(frm.ipv6_wanauto_x, 0);
		inputRCtrl2(frm.ipv6_wanauto_x, 1);
		inputRCtrl1(frm.ipv6_dnsenable_x, 0);
		inputRCtrl2(frm.ipv6_dnsenable_x, 1);
		inputRCtrl1(frm.ipv6_lanauto_x, 0);
		inputRCtrl2(frm.ipv6_lanauto_x, 1);
	} else
	if (v == "slaac")
	{
		inputCtrl(frm.ipv6_if_x, ppp);
		inputRCtrl1(frm.ipv6_wanauto_x, 0);
		inputRCtrl2(frm.ipv6_wanauto_x, 0);
		inputRCtrl1(frm.ipv6_dnsenable_x, 0);
		inputRCtrl2(frm.ipv6_dnsenable_x, 1);
		inputRCtrl1(frm.ipv6_lanauto_x, 0);
		inputRCtrl2(frm.ipv6_lanauto_x, 1);
	} else
	if (v == "dhcp6")
	{
		inputCtrl(frm.ipv6_if_x, ppp);
		inputRCtrl1(frm.ipv6_wanauto_x, 0);
		inputRCtrl2(frm.ipv6_wanauto_x, 0);
		inputRCtrl1(frm.ipv6_dnsenable_x, 1);
		inputRCtrl1(frm.ipv6_lanauto_x, 1);
	} else
	if (v == "tun6in4")
	{
		inputCtrl(frm.ipv6_if_x, 0);
		inputRCtrl1(frm.ipv6_wanauto_x, 0);
		inputRCtrl2(frm.ipv6_wanauto_x, 1);
		inputRCtrl1(frm.ipv6_dnsenable_x, 0);
		inputRCtrl2(frm.ipv6_dnsenable_x, 1);
		inputRCtrl1(frm.ipv6_lanauto_x, 0);
		inputRCtrl2(frm.ipv6_lanauto_x, 1);
		tunnel = 1;
	} else
	if(v == "tun6to4" || v == "tun6rd")
	{
		inputCtrl(frm.ipv6_if_x, 0);
		inputRCtrl1(frm.ipv6_wanauto_x, 1);
		inputRCtrl1(frm.ipv6_dnsenable_x, 0);
		inputRCtrl2(frm.ipv6_dnsenable_x, 1);
		inputRCtrl1(frm.ipv6_lanauto_x, 1);
		tunnel = 1;
	} else
	{
		inputCtrl(frm.ipv6_if_x, 0);
		inputRCtrl1(frm.ipv6_wanauto_x, 0);
		inputRCtrl1(frm.ipv6_dnsenable_x, 0);
		inputRCtrl1(frm.ipv6_lanauto_x, 0);
		enable = 0;
	}

	change_ipv6_wanauto_x(enable);
	change_ipv6_lanauto_x(enable);

	val = (!enable || frm.ipv6_dnsenable_x[0].checked) ? '1' : '0';
	change_common_radio(frm.ipv6_dnsenable_x, 'IPv6Config', 'ipv6_dnsenable_x', val);

     	inputCtrl(frm.ipv6_sit_remote, (v == "tun6in4") ? enable : 0);
	inputCtrl(frm.ipv6_sit_relay, (v == "tun6to4") ? enable : 0);
	inputCtrl(frm.ipv6_6rd_router, (v == "tun6rd") ? enable : 0);
	inputCtrl(frm.ipv6_6rd_ip4size, (v == "tun6rd") ? enable : 0);
	inputCtrl(frm.ipv6_sit_mtu, tunnel);
	inputCtrl(frm.ipv6_sit_ttl, tunnel);

	inputRCtrl1(frm.ipv6_radvd_enable, enable);
	val = (enable && frm.ipv6_radvd_enable[0].checked) ? '1' : '0';
	change_common_radio(frm.ipv6_radvd_enable, 'IPv6Config', 'ipv6_radvd_enable', val);
}

function masq_wepkey()
{
	var frm = document.form;
	wep1 = frm.wl_key1.value;
	wep2 = frm.wl_key2.value;
	wep3 = frm.wl_key3.value;
	wep4 = frm.wl_key4.value;

	if (wep1.length == 10)
	{
		wep = "**********";
	}
	else if (wep1.length == 26)
	{
		wep = "**************************";
	}
	else wep = "";

	frm.wl_key1.value = wep;
	frm.wl_key2.value = wep;
	frm.wl_key3.value = wep;
	frm.wl_key4.value = wep;
}

function unmasq_wepkey()
{
	var frm = document.form;
	if (frm.wl_key1.value.indexOf("**********") != -1)
		frm.wl_key1.value = wep1;
	if (frm.wl_key2.value.indexOf("**********") != -1)
		frm.wl_key2.value = wep2;
	if (frm.wl_key3.value.indexOf("**********") != -1)
		frm.wl_key3.value = wep3;
	if (frm.wl_key4.value.indexOf("**********") != -1)
		frm.wl_key4.value = wep4;
}

function changeWiMAXCheckConnection()
{
	var frm = document.form;
	if (frm.wan_wimax_check[0].checked === false)
	{
		inputCtrl(frm.wan_wimax_interval, 0);
		inputRCtrl1(frm.wan_wimax_restart, 0);
	}
	else
	{
		inputCtrl(frm.wan_wimax_interval, 1);
		inputRCtrl1(frm.wan_wimax_restart, 1);
	}
}

function getCmdExecUrl( host, cmd )
{
	return	"http://" + host +
		"/apply.cgi?current_page=syscmd_out.asp&action_mode=+Refresh+&SystemCmd=" + encodeURIComponent( cmd );
}

function getHTTPRequest( url, func_res, func_fail )
{
	var xhr; 
	try {  xhr = new ActiveXObject('Msxml2.XMLHTTP');   }
	catch (e) 
	{
		try {   xhr = new ActiveXObject('Microsoft.XMLHTTP');    }
		catch (e2) 
		{
			try {  xhr = new XMLHttpRequest();     }
			catch (e3) {  xhr = false;   }
		}
	}
	xhr.onreadystatechange=function(){
		if(xhr.readyState != 4) return;
			clearTimeout(timeout);
			if(xhr.status==200 && func_res ){
				func_res(xhr);
			}
		};
	xhr.open("GET", url, true);
	xhr.send(null);
	var timeout = setTimeout( 
		function(){ 
			xhr.abort(); if(func_fail){ func_fail(url); };
		}, 10000);
}
