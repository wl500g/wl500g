// Web interface user interface script
// New skin on background of RT-N66 and original
// written by  dlukanidin
var ui_wl500g_ver = 'ver. 1.100 from 30.03.2012';

// Global variables
var enable_bckgrds = ['silver','#576D73'];
var ui_path = pathSys+'ui/'
var copyright_str = '2012 wl500g.info. Images from service.asus.com/demoui.';

var menu_images = {
	'index':'icon_home1.png',
	'Basic_Operation_Content.asp':'icon_wizard1.png',
	'Advanced_Wireless_Content':'icon_wifi1.png',
	'Advanced_WAN_Content':'icon_wan1.png',
	'Advanced_PortTrigger_Content':'icon_nat1.png',
	'Advanced_BasicFirewall_Content':'icon_fwall1.png',
	'Advanced_QOS_Content':'icon_qos1.png',
	'Advanced_USBStorage_Content':'icon_usb1.png',
	'Advanced_ConnectionMode_Content':'icon_usb11.png',
	'Advanced_OperationMode_Content':'icon_admin1.png',
	'Main_GStatus_Content':'icon_log1.png',
	'def':'icon_setting.png'
};

// Tooltip 
function tooltip(el) {
	for(var i=0; i<el.childNodes.length; i++) {
	    var child = el.childNodes[i]
		if ( child.tagName == 'SPAN' ) {
			if ( child.style.display == 'block' ) return;
			child.style.display='block';
			move_pic(child);
			break;
		}
	}
}
function hide_info(el) {
	for(var i=0; i<el.childNodes.length; i++) {
	    var child = el.childNodes[i]
		if ( child.tagName == 'SPAN' ) {
			if ( child.style.display != 'block' ) return;
			child.style.left='';
			child.style.top='';
			child.style.display='none';
			break;
		}
	}
}
function move_pic(e){
	var r = getElemRect(e);
	var dx = 5;
	if(dx+r.l+r.w > r.width) e.style.left=-(r.w+dx);
	if(dx+r.t+r.h > r.height) e.style.top=-(r.h+dx);
}

// Wait indicator

// Show Error
function showError(code,str)
{
	if ( typeof trNavi === "function" ) var err = trNavi(code);
	else var err = code.replace(/_/g,' ');
	if ( str ) err += ' <'+str+'>';
	alert(err);
}

// Table generation
function table(cl)
{
	return '<table'+((cl)?' class="'+cl+'"':'')+' cellspacing="0" cellpadding="0" border="0">';
}

// Page generation
function getPageHtml(page)
{
	var str = '';
	if ( page.header || page.subheader ) {
		str+='<tr><td valign="top"><div>&nbsp;</div>';
		if ( page.header ) {
			str+='<div class="form_header">'+page.header.text+'</div>';
			str+='<div class="form_header_line">';
				str+='<img src="'+ui_path+'images/header_line.png">';
			str+='</div>';
		}
		str+=((page.subheader)?'<div class="form_subheader">'+page.subheader.text+'</div>':'');
		str+='</td></tr>';
	}
	var cont=page.content;
	if ( cont ) for ( var i=0; i<cont.length; i++ ) {
		var item=cont[i];
		if ( !item ) continue;
		if ( item.type != 'section' ) {
			str+='<tr><td valign="top">';
				str+=table('form_table');
				str+=getItemHTML(item);
				str+='</table>';
			str+='</td></tr>';
			continue;
		}
		var sect=item.content;
		str+='<tr><td valign="top">';
			str+='<table class="form_table" width="100%" cellspacing="0" cellpadding="4" bordercolor="#6b8fa3" border="1" align="center">';
			if ( item.text ) str+='<thead><tr><td colspan="2">'+item.text+'</td></tr></thead>';
			if ( !sect ) continue;
			for ( var j=0; j<sect.length; j++ ) {
				var elem=sect[j];
				if ( !elem ) continue;
				str+=getItemHTML(elem);
			}
			str+='</table>';
		str+='</td></tr>';
	}
	return str;	
}

function getItemHTML(item)
{
	var str = '<tr>';
	if ( item.label )
		str='<th width="30%">'+(( item.help )?'<a class="tooltip" href="javascript:void(0);'+
		'" onClick="tooltip(this)" onmouseout="hide_info(this)">'+
		item.label+'<span class="custom help"><img src="'+ui_path+'images/help_icon.png" height="28" width="28" /><em>'+
		item.label+'</em>'+item.help+'</span></a>':item.label)+'</th>';
	str+='<td>';
		str+=getElemItemHTML(item);
		var cont=item.content;
		if ( cont ) for ( var i=0; i<cont.length; i++ ) {
			var elem=cont[i];
			if ( elem ) str+=getElemItemHTML(elem);
		}
	str+='</td>';
	str+='</tr>';
	return str;	
}

function getAttrib(item)
{
	var str='';
	str+=((item.id)?' id="'+item.id+'"':'');
	str+=((item.on)?' '+item.on:'');
	str+=((item.size)?' size="'+item.size+'"':'');
	str+=((item.rows)?' rows="'+item.rows+'"':'');
	str+=((item.cols)?' cols="'+item.cols+'"':'');
	str+=((item.max)?' maxlength="'+item.max+'"':'');
	str+=((item.width)?' width="'+item.width+'"':'');
	str+=((item.height)?' height="'+item.height+'"':'');
	str+=((item.style)?' style="'+item.style+'"':'');
	str+=((item.readonly)?' readonly':'');
	str+=((item.disabled)?' disabled':'');
	str+=((item.validate)?' onblur="validateItem(this)"':'');
	return str;
}
function getElemItemHTML(item,new_cls)
{
	var str='';
	var cls_types={'html':'','str':'','ref':'','button':'button_gen','icon':item.cls,'password':'input_table','text':'input_table',
		'textarea':'','iframe':'','checkbox':'','select':'input_option','radio':'input'};
	var cls=((new_cls)?' class="'+new_cls+'"':((cls_types[item.type] != '')?' class="'+cls_types[item.type]+'"':'') );
	
	if ( ! item ) return str;
	switch( item.type ) {
		case 'html': str+=item.value; break;
		case 'str': str+=item.text;	break;
		case 'ref': str+='<a href="'+item.href+'"><span'+cls+getAttrib(item)+'>'+item.text+'</span></a>'; break;
		case 'button': str+='<input'+cls+getAttrib(item)+' value="'+item.text+'" type="submit">'; break;
		case 'icon': str+='<td'+cls+getAttrib(item)+'></td>'; break;
		case 'password':
		case 'text': str+='<input type="'+item.type+'"'+cls+getAttrib(item)+' value="'+item.value+'">'; break;
		case 'textarea': str+='<textarea'+cls+getAttrib(item)+'>'+item.value+'</textarea>'; break;
		case 'iframe': str+='<iframe seamless frameborder="0"'+cls+getAttrib(item)+((item.src)?' src="'+item.src+'"':'')+'></iframe>'; 			break;
		case 'checkbox':
			str+='<input type="checkbox" value="'+item.value+'"'+cls+getAttrib(item)+
				((item.state == 'on')?' checked':'')+'>'+item.text;
			break;
		case 'select':
			str+='<select'+cls+getAttrib(item)+'>';
			var options=item.options;
			if( options ) for ( var j=0; j<options.length; j++ ){
				var opt=options[j];
				if( ! opt ) continue; 
				str+='<option'+cls+' value="'+opt.value+'"'+((opt.state == 'on')?' selected="selected"':'')+'>'+opt.text+'</option>';
			}
			str+='</select>';
			break;
		case 'radio':
			var buttons=item.buttons;
			if( !buttons ) break;
			for(var j=0; j<buttons.length; j++){
				var btn=buttons[j];
				if( ! btn ) continue; 
				str+='<input type="radio"'+cls+getAttrib(item)+' value="'+btn.value+'" name="'+item.id+'"'+
					((btn.state == 'on')?' checked':'')+'>'+btn.text;
			}
			break;
		default: break;
	}
	return str;	
}

// Field validation
function validateItem(obj)
{
	var valid = getValidationObj(obj.id);
	if ( valid ) switch( valid.type ) {
		case 'string': return stringValidate(obj,valid);
		case 'ssid': return ssidValidate(obj,valid);
		case 'ipv4': return ipv4Validate(obj,valid);
		case 'ipv4fill': return ipv4FillValidate(obj,valid);
		case 'ipv6': return ipv4Validate(obj,valid);
		case 'ipv6fill': return ipv4FillValidate(obj,valid);
		case 'int': return intValidate(obj,valid);
		case 'mac': return macValidate(obj,valid);
		default: break;
	}
	return true;
}

// Return menu image
function getMenuImg(page)
{
	var path = ui_path+'images/';
	if ( menu_images[page] ) return path+menu_images[page];
	else return path+menu_images['def'];
}

// Show menu 
function getMenuHtml()
{
	var str = '';
	var menu = getMenu();
	var active = getActiveMenu();
	for ( var i=0; i<menu.length; i++ ) {
		var elem=menu[i];
		if ( elem.type == 'item' ) {
			str+='<tr height="60"><td>';
				str+='<a href="'+elem.page+'.asp" onclick="setPage(\''+elem.page+'\')">';
					str+=table(((active.mainitem == i)?'menu_main_item_active':'menu_main_item'))+'<tr>';
						str+='<td width="50"><img width="50px" height="50px" border="0" src="'+getMenuImg(elem.page)+'"></td>';
                        str+='<td class="menu_text">'+elem.text+'</td>';
					str+='</tr></table>';
				str+='</a>';
			str+='</td></tr>';
		} else if ( elem.type == 'section' ) {
			str+='<tr><td class="menu_section">'+elem.text+'</td></tr>';
			for ( var j=0; j<elem.content.length; j++ ) {
				var menu_item=elem.content[j];
				if ( ! menu_item ) continue;
				var page = ((menu_item.pages)?menu_item.pages[0].page:menu_item.page);
				str+='<tr><td>';
					str+='<a href="'+page+'.asp" onclick="setPage(\''+page+'\')">';
						str+=table(((active.item == j && active.section == i)?'menu_item_active':'menu_item'))+'<tr>';
							str+='<td width="50"><img width="50px" height="50px" border="0" src="'+getMenuImg(page)+'"></td>';
							str+='<td class="menu_text">'+menu_item.text+'</td>';
						str+='</tr></table>';
					str+='</a>';
				str+='</td></tr>';
			}
		}
		str+='<tr height="16"><td></td></tr>';
	}
	return str;
}

// Show submenu 
function getSubMenuHtml()
{
	var str = '';
	var menu = getMenu();
	var active = getActiveMenu();
	if ( active.section !== undefined && active.item  !== undefined ) {
		var i = active.section;
		var j = active.item;
		var item=menu[i].content[j];
		if ( item.pages ) for ( var k=0; k<item.pages.length; k++ ) {
			var subitem=item.pages[k];
			str+='<td>';
			if ( active.subitem == k ) {
/*				str+='<div class="menu_tab_active">';
					str+='<span>'+subitem.text+'</span>';
				str+='</div>';
*/				str+=table()+'<tr><td class="menu_tab_left_active">';
					str+='<span>'+subitem.text+'</span>';
				str+='</td><td class="menu_tab_right_active"></td></tr></table>';
			} else {
				str+='<a href="'+subitem.page+'.asp" onclick="setPage(\''+subitem.page+'\')">';
/*					str+='<div class="menu_tab">';
						str+='<span>'+subitem.text+'</span>';
					str+='</div>';
*/					str+=table()+'<tr><td class="menu_tab_left">';
						str+='<span>'+subitem.text+'</span>';
					str+='</td><td class="menu_tab_right"></td></tr></table>';
				str+='</a>';
			}
			str+='</td>';
		}
	}
	return str;
}

// Show top status
function getStatusHtml(cont)
{
	var str='<td class="status_main">';
	if ( cont.status[0].content ) for ( var i=0; i<cont.status[0].content.length; i++ ) {
		var elem=cont.status[0].content[i];
		if ( elem && elem.type == 'str' ) {
			str+='<span class="status_text">'+getElemItemHTML(elem)+'</span>';
		} else {
			str+=getElemItemHTML(elem,'status_ref');
		}
	}
	str+='</td>';
	if ( cont.status ) for ( var i=1; i<cont.status.length; i++ ) {
		var elem=cont.status[i];
		str+=getElemItemHTML(elem);
	}
	return str;
}

// Show bottom page buttons
function getButtonsHtml(cont)
{
	var str='';
	var page = getPage();
	if ( page.buttons ) {
		str+='<tr><td valign="top">';
			str+='<table class="bottom_buttons" width="100%" cellspacing="0" cellpadding="0" border="0"><tr>';
			if ( cont.buttons ) for ( var i=0; i<cont.buttons.length; i++ ) {
				var elem=cont.buttons[i];
				str+='<td><div class="bottom_apply">';
					str+=getElemItemHTML(elem);
				str+='</div></td>';
			}
			str+='</tr></table>';
		str+='</td></tr>';
		str+='<tr><td valign="top">';
			str+='<table class="form_table" width="100%" cellspacing="0" cellpadding="4" bordercolor="#6b8fa3" border="1" align="center">';
			if ( cont.help ) for ( var i=0; i<cont.help.length; i++ ) {
				var elem=cont.help[i];
				str+='<tr><th width="30%">'+elem.label;
				str+='</th><td>'+elem.text+'</td></tr>';
			}
			str+='</tr></table>';
		str+='</td></tr>';
	}
	return str;
}

// Show header and navi
function getHeaderHtml(cont)
{
	var str='<table class="main" cellpadding="0" cellspacing="0" border="0" align="center">';
	str+='<tr ><td>';
		//<!--- top banner start --->
    	str+=table('top_banner')+'<tr>';
    		str+='<td class="logo_img"></td>';
			if ( cont.actions ) for ( var i=0; i<cont.actions.length; i++ ) {
				var elem=cont.actions[i];
				if ( ! elem ) continue;
				str+='<td class="top_banner_buttons">';
				str+=getElemItemHTML(elem);
				str+='</td>';
			}
			str+='<td class="top_banner_buttons"></td>';
			if ( cont.settings[0] ) {
				var elem=cont.settings[0];
				str+='<td class="top_banner_buttons">';
				str+=getElemItemHTML(elem,'input_option_banner');
				str+='</td>';
			}
		str+='</tr></table>';
       	//<!--- top banner end --->
		str+='<div id="Loading" class="popup_bg"><img src="'+ui_path+'images/loading.gif"></div>'; // <!--- Wait indicator --->
    str+='</td></tr>';
	str+='<tr><td class="bkgrd">';
		str+=table('content')+'<tr>';
			str+='<td width="20"></td>'; // <!--- shift menu from left --->
			str+='<td width="202" valign="top">';
				//<!--- left menu start --->
				str+=table('left_menu');
					str+='<tr height="48"><td></td></tr>'; // <!--- shift menu from top --->
					str+=getMenuHtml();
				str+='</table>';
				//<!--- left menu end --->
			str+='</td>';
			str+='<td valign="top">';
				str+=table();
					str+='<tr><td>';
						//<!--- top status line start --->
						str+=table('status_line')+'<tr>';
							str+=getStatusHtml(cont);
						str+='</tr></table>';
						//<!--- top status line end --->
					str+='</td></tr>';
					str+='<tr><td>';
						//<!--- top submenu start --->
						str+=table('sub_menu')+'<tr>';
							str+=getSubMenuHtml();
						str+='</tr></table>';
						//<!--- top submenu end --->
					str+='</td></tr>';
					str+='<tr><td class="form_title" valign="top">';
                   	//<!--- main table start --->
						str+='<table cellspacing="0" cellpadding="5" border="0">';
	return str;
}

// Show footer and navi_footer
function getFooterHtml(cont)
{
						var str='</table>';
					//<!--- main table end --->
					str+='</td></tr>';
				str+='</table>';
			str+='</td>';
			str+='<td width="20"></td>'; //<!--- shift table from right --->
		str+='</tr></table>';
	str+='</td></tr>';
	str+='<tr class="bottom-banner"><td></td></tr>';
	str+='<tr><td class="copyright">'+copyright_str+'</td></tr>';
	str+='</table>';
	return str;
}

// Show control buttons and help 
function getContentHTML(cont)
{
	var str='';
	var page = getPage();
	str+=getHeaderHtml(cont);
	str+=getPageHtml(page);
	str+=getButtonsHtml(cont);
	str+=getFooterHtml(cont);
	return str;
}

// Show menu and common page elements
function showCommon()
{
	var doc = document.getElementById('contentDiv');
	var content = getCommon();
	doc.innerHTML = getContentHTML(content);
}

// Show page navigation and common
function showNavi()
{
	if ( ! $id('contentDiv') ) {
		var bodyID = document.getElementsByTagName("body")[0];
		var div = document.createElement('div');
		div.id = 'contentDiv';
		bodyID.appendChild(div);
	}
	var common = getCommon();
	trPage(common,trNavi);
	var menu = getMenu();
	trPage(menu,trNavi);
	showCommon();
	updateStatus(function(obj){
		$id('stat_mode').innerHTML = obj.mode;
		$id('stat_vers').innerHTML = obj.firmware_ver;
		$id('stat_ssid').innerHTML = obj.ssid;
	})
}

// Show specific page elements
function showPage()
{
	showNavi();
}

// Show navi, header and footer for old pages
function showHeader(cont)
{
	var common = getCommon();
	trPage(common,trNavi);
	var menu = getMenu();
	trPage(menu,trNavi);
	var str = getHeaderHtml(common);
	str+='<tr><td>';
	document.write(str);
}
function showFooter(cont)
{
	var common = getCommon();
	var str='</td></tr>';
	str += getFooterHtml(common);
	document.write(str);
	updateStatus(function(obj){
		$id('stat_mode').innerHTML = obj.mode;
		$id('stat_vers').innerHTML = obj.firmware_ver;
		$id('stat_ssid').innerHTML = obj.ssid;
	})
}

// Return array of colors for enabled/disabled fields
function getEnableColors()
{
	return enable_bckgrds;
}

// Show or hide wait indicator
var show_waiting_flag = 0;
function showWaiting(flag){
	if( flag ) {
		var obj = $id("Loading");
		if ( obj ) obj.style.visibility = "visible";
		show_waiting_flag++;
	} else {
		if ( show_waiting_flag > 0 ) show_waiting_flag--;
		if ( show_waiting_flag == 0 ) {
			var obj = $id("Loading");
			if ( obj ) obj.style.visibility = "hidden";
		}
	}
}
// Init module
function initUI()
{
	loadFlags.skin = true;						// skin is loaded
//	loadStyle(ui_path+'wl500g.css');	// need new CSS for new skin
}

// Init module
initUI();
