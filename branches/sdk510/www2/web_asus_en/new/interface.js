// Web interface general script
// written by  dlukanidin
var interface_ver = 'ver. 1.100 from 30.03.2012';

// Global variables
var timeoutHTTPRequest = 10000;			// 10 sec
var timeoutWaitLoading = 100;			// 0.1 sec
var timeoutWaitCount = 100;				// Total 10 sec wait
var pathSys = 'new/';
var loadFlags = { 'lc_navi':false, 'lc_page':false,'skin':false,'page':false}; 

// Main document load
function loadBody()
{
	initNavi();
}

// Try to unload document
function checkChanged()
{
	// don't work. need to be written and inserted in menu code
	alert('function substitution from index.js');
	return false;
}

// Get tag content from source
function getTag(src,tag)
{
	var i = src.indexOf('<'+tag);
	if ( i == -1 ) i = src.indexOf('<'+tag.toUpperCase());
	var j = src.indexOf('</'+tag);
	if ( j == -1 ) j = src.indexOf('</'+tag.toUpperCase());
	if ( i != -1 ) {
		while ( src.charAt(i) != '>' ) i++;
		if ( j != -1 ) return src.substring(i+1,j);
		else return src.substring(i+1);
	}
	return '';
}

// Load dynamic part of page from server
function loadDynamic(id,name,ready)
{
	var host = location.host;
	var url = 'http://' + host + '/'+name+'.asp';
	sendHTTPRequest(url,'',function(content) {
		var dynamic = getTag(content,'dynamic');
		if ( dynamic !== '' ) {
			window[id] = changeModule(window[id],dynamic,true);
			if( typeof ready == 'function' ) ready();
		}
	},onError,'GET');
}

// Send request to server
function sendJSON(str,call_back)
{
	var host = window.location.host;
	var url = 'http://' + host + '/json.cgi';
	sendHTTPRequest(url,str,call_back,onError);
}

// Show Error
function onError(code,str)
{
	if( typeof showError === "function" ) showError(code,str);
	else alert('Error: '+code+((str)?' on <<< '+str+' >>> ':''));
}

// Send request to server
function sendHTTPRequest( url, data, func_res, func_fail, method )
{
	var xhr;
	try {xhr = new ActiveXObject('Msxml2.XMLHTTP');}
	catch (e) {
		try {xhr = new ActiveXObject('Microsoft.XMLHTTP');}
		catch (e2) {
			try {xhr = new XMLHttpRequest();}
			catch (e3) {xhr = false;}
		}
	}
	xhr.onreadystatechange=function(){
		if(xhr.readyState != 4) return;
		clearTimeout(timeout);
		if ( typeof showWaiting === 'function' ) showWaiting(false);
		if(xhr.status==200 && func_res ){
			func_res(xhr.responseText);
		} else {
			func_fail('no_answer_from_server',xhr.statusText);
//			func_res('["'+xhr.statusText+'"]');
		}
	};
	xhr.open(method?method:'POST', url, true);
	xhr.send(data);
	if ( typeof showWaiting === 'function' ) showWaiting(true);
	var timeout = setTimeout(function(){
		xhr.abort();
		if ( typeof showWaiting === 'function' ) showWaiting(false);
		if(func_fail){ func_fail('no_answer_from_server',url); };
		}, timeoutHTTPRequest);
}

// Parse JSON string
function parseJSON( data )
{
	var vc = /^[\],:{}\s]*$/,
	ve = /\\(?:["\\\/bfnrt]|u[0-9a-fA-F]{4})/g,
	vt = /"[^"\\\n\r]*"|true|false|null|-?\d+(?:\.\d*)?(?:[eE][+\-]?\d+)?/g,
	vb = /(?:^|:|,)(?:\s*\[)+/g;
	var res = {};
	if ( vc.test(data.replace(ve, "@").replace(vt, "]").replace(vb, "")) ) {
		try { res = window.JSON && window.JSON.parse ?
			window.JSON.parse( data ) :
			(new Function("return " + data))();
		} catch(e) {}
	} else {
		onError('invalid_json',data);
	}
	return res;
}

// Send JSON command and return answer
function queryServer(query,func)
{
	sendJSON(query,function(str){
		var answer = parseJSON(str);
		if ( typeof func === 'function' ) func(answer);
	});
}

// Load style and return it's id
function loadStyle(src){
	var call = 'call_'+Math.random().toString().replace(/\./g,'');
	var head = document.head || document.getElementsByTagName( "head" )[0] || document.documentElement;
	var link = document.createElement('link');
	link.id = call;
	link.rel='stylesheet';
	link.type = 'text/css';
	link.media='screen';
	link.href=src;
	head.insertBefore(link, head.firstChild);
	return call;
}

// Remove previous module and load new
function changeModule(id,href,inner) {
	removeModule(id);
	var new_id = loadModule(href,inner);
	return new_id;
}

// Load module and return it's id
function loadModule(src,inner)
{
	var call = 'call_'+Math.random().toString().replace(/\./g,'');
	var head = document.head || document.getElementsByTagName( "head" )[0] || document.documentElement;
	var script = document.createElement('script');
	script.id = call;
	script.type = 'text/javascript';
	if( inner )
		script.text = src;
	else 
		script.src = src;
	head.insertBefore(script, head.firstChild);
	return call;
}

// Remove module with id
function removeModule(id)
{
	if ( ! id ) return false;
	var e = document.getElementById(id);
	if ( e ) {
		e.parentNode.removeChild(e);
		return true;
	}
	return false;
}

// Call func if modules are loaded and wait if not
function waitLoading(flags,func)
{
	function doWait() {
		for ( var i=0; i < flags.length; i++){
			var flag = flags[i];
			if ( ! loadFlags[flag] ) {
				if ( callCount-- > 0 ) {
					if ( typeof showWaiting === 'function' ) showWaiting(false);
					if ( typeof showWaiting === 'function' ) showWaiting(true);
					setTimeout(doWait, timeoutWaitLoading);
				} else {
					if ( typeof showWaiting === 'function' ) showWaiting(false);
					onError('error_load_module',flag);
				}
				return;
			}
		}
		if ( typeof showWaiting === 'function' ) showWaiting(false);
		if ( typeof func === 'function' ) func();
	}
	var callCount = timeoutWaitCount;
//	setTimeout(doWait, 10);
	doWait();
}

// Localization of page
function trPage(page,func)
{
	if ( typeof func === 'function' ) for ( var p in page ) {
		if ( p == 'text' || p == 'label' || p == 'help' ) page[p] = func(page[p]);
		if ( typeof page[p] === 'object' ) trPage(page[p],func);
	}
}

// Set page definition values from data
function setPageValues(item,data)
{
	var nvram = item.id;
	if ( item.type && nvram ) switch( item.type ) {
		case 'password':
		case 'text':
		case 'textarea':
			if( ! data[nvram] ) data[nvram] = '';
			item.value = data[nvram];
			break;
		case 'checkbox':
			if( ! data[nvram] ) data[nvram] = '0';
			if( data[nvram] != '0' ) {
				item.value = data[nvram];
				item.state = 'on';
			} else item.state = 'off';
			break;
		case 'select':
			var options=item.options;
			if( options ) for ( var j=0; j<options.length; j++ ){
				var opt=options[j];
				if ( ! opt ) continue; 
				if ( opt.value == data[nvram] ) opt.state = 'on';
				else opt.state = 'off';
			}
			break;
		case 'radio':
			var buttons=item.buttons;
			if( !buttons ) break;
			for(var j=0; j<buttons.length; j++){
				var btn=buttons[j];
				if( ! btn ) continue; 
				if ( btn.value == data[nvram] ) btn.state = 'on';
				else btn.state = 'off';
			}
			break;
		default: break;
	}
	for ( var p in item ) {
		if ( typeof item[p] === 'object' ) setPageValues(item[p],data);
	}
}

// Get checked from fields array
function getCheckedValue(o)
{
	for (var i=0; i < o.length; i++){
		if (o[i].checked)
			return o[i].value;
	}
	return -1;
}

// Get data from page
function getPageValues(item,data)
{
	if ( typeof data !== 'object' ) return;
	var nvram = item.id;
	var val;
	if ( item.type && nvram ) switch( item.type ) {
		case 'password':
		case 'text':
		case 'textarea':
		case 'select':
			val = document.getElementById(nvram).value;
			if( val ) data[nvram] = val;
			else data[nvram] = '';
			break;
		case 'checkbox':
			val = document.getElementById(nvram).checked;
			if( val ) data[nvram] = '1';
			else data[nvram] = '0';
			break;
		case 'radio':
			var buttons=document.getElementsByName(nvram);
			if( !buttons ) break;
			val = getCheckedValue(buttons);
			if( val !== -1 ) delete data[nvram];
			break;
		default: break;
	}
	for ( var p in item ) {
		if ( typeof item[p] === 'object' ) getPageValues(item[p],data);
	}
}

// Trace page for validation object
function getValidationObj(id)
{
	var obj;
	var page = getPage();
	function doTrace(page)
	{
		for ( var p in page ) {
			if ( page[p].id == id ) obj = page[p].validate;
			else if ( typeof page[p] === 'object' ) doTrace(page[p]);
		}
	}
	doTrace(page);
	return obj;
}

// Validation functions
function stringValidate(o,valid)
{
	if ( o.value.charAt(0)=='"' ) {
		onError('error_str_start');
		if ( valid.defvalue ) o.value = valid.defvalue;
		else o.value = '';
		o.focus();
		return false;
	} else {
		var inv='';
		for ( var i=0; i<o.value.length; i++ ) {
			if ( o.value.charAt(i)<' ' || o.value.charAt(i)>'~' ) inv += o.value.charAt(i);
 		}
		if ( inv !== '' ) {
			onError('error_in_string',inv);
			if ( valid.defvalue ) o.value = valid.defvalue;
			else o.value = '';
			o.focus();
			return false;
		}
	}
	return true;
}
function ssidValidate(o,valid)
{
	if ( ! stringValidate(o,valid) || o.value == '' ) {
		if ( valid.defvalue ) o.value = valid.defvalue;
		else o.value = 'default';
	}
}
function ipv4Trace(str)
{
	var ip = {};
	var num = -1;
	var pos = 0;
	for( var i=0; i<str.length; i++) {
		var c=str.charAt(i);
		if( c>='0' && c<='9' ) {
			if ( num == -1 ) num = (c-'0');
			else num = num*10 + (c-'0');
		} else {
			if ( num<0 || num>255 || c!='.') return null;
			if ( pos === 0 ) ip.v1=num;
			else if ( pos == 1 ) ip.v2=num;
			else if ( pos == 2 ) ip.v3=num;
			num = -1;
			pos++;
		}
	}
	if ( pos!=3 || num<0 || num>255 ) return null;
	else ip.v4=num;
	ip.value = ip.v1 + '.' + ip.v2 + '.' + ip.v3 + '.' + ip.v4;
	return ip;
}
function ipv4Validate(o,valid)
{
	var val = o.value;
	if ( ! val || val == '' ) return true;
	var ip = ipv4Trace(val);
	if ( ! ip ) {
		onError('error_in_ip',val);
		if ( valid.defvalue ) o.value = valid.defvalue;
		else o.value = '';
		o.focus();
		return false;
	}
	o.value = ip.value;
	return true;
}
function ipv4FillValidate(o,valid)
{
	var val = o.value;
	if ( ! val || val == '' ) {
		onError('error_blank_ip');
		if ( valid.defvalue ) o.value = valid.defvalue;
		else o.value = '192.168.1.254';
		o.focus();
		return false;
	} else return ipv4Validate(o,valid);
}
function ipv6Validate(o,valid)
{
	var regex = /^((([0-9a-f]{1,4}:){7}([0-9a-f]{1,4}|:))|(([0-9a-f]{1,4}:){6}(:[0-9a-f]{1,4}|((25[0-5]|2[0-4]\d|1\d\d|[1-9]?\d)(\.(25[0-5]|2[0-4]\d|1\d\d|[1-9]?\d)){3})|:))|(([0-9a-f]{1,4}:){5}(((:[0-9a-f]{1,4}){1,2})|:((25[0-5]|2[0-4]\d|1\d\d|[1-9]?\d)(\.(25[0-5]|2[0-4]\d|1\d\d|[1-9]?\d)){3})|:))|(([0-9a-f]{1,4}:){4}(((:[0-9a-f]{1,4}){1,3})|((:[0-9a-f]{1,4})?:((25[0-5]|2[0-4]\d|1\d\d|[1-9]?\d)(\.(25[0-5]|2[0-4]\d|1\d\d|[1-9]?\d)){3}))|:))|(([0-9a-f]{1,4}:){3}(((:[0-9a-f]{1,4}){1,4})|((:[0-9a-f]{1,4}){0,2}:((25[0-5]|2[0-4]\d|1\d\d|[1-9]?\d)(\.(25[0-5]|2[0-4]\d|1\d\d|[1-9]?\d)){3}))|:))|(([0-9a-f]{1,4}:){2}(((:[0-9a-f]{1,4}){1,5})|((:[0-9a-f]{1,4}){0,3}:((25[0-5]|2[0-4]\d|1\d\d|[1-9]?\d)(\.(25[0-5]|2[0-4]\d|1\d\d|[1-9]?\d)){3}))|:))|(([0-9a-f]{1,4}:){1}(((:[0-9a-f]{1,4}){1,6})|((:[0-9a-f]{1,4}){0,4}:((25[0-5]|2[0-4]\d|1\d\d|[1-9]?\d)(\.(25[0-5]|2[0-4]\d|1\d\d|[1-9]?\d)){3}))|:))|(:(((:[0-9a-f]{1,4}){1,7})|((:[0-9a-f]{1,4}){0,5}:((25[0-5]|2[0-4]\d|1\d\d|[1-9]?\d)(\.(25[0-5]|2[0-4]\d|1\d\d|[1-9]?\d)){3}))|:)))(%.+)?$/i;
	if (!regex.test(o.value)) {
		onError('error_in_ipv6',val);
		if ( valid.defvalue ) o.value = valid.defvalue;
		else o.value = '';
		o.focus();
		return false;
	}
	o.value = ip.value;
	return true;
}
function ipv6FillValidate(o,valid)
{
	var val = o.value;
	if ( ! val || val == '' ) {
		onError('error_blank_ip');
		if ( valid.defvalue ) o.value = valid.defvalue;
		else o.value = '192.168.1.254';
		o.focus();
		return false;
	} else return ipv6Validate(o,valid);
}
function intValidate(o,valid)
{
	if ( isNaN(o.value) || o.value<valid.min || o.value>valid.max ) {
		onError('error_in_int',valid.min+' - '+valid.max);
		if ( valid.defvalue ) o.value = valid.defvalue;
		else o.value = '';
		o.focus();
		return false;
	}
	return true;
}
function macValidate(o,valid)
{
	var val = o.value;
	var len = val.length;
	if ( len === 0 ) return true;
	if ( val !== '' ) {
		if ( len == 12 ) {
			for ( var i=0; i<len; i++ ) {
				var c = val.charAt(i);
				if ( !(c>='0'&&c<='9') && !(c>='a'&&c<='f') && !(c>='A'&&c<='F') ) {
					onError('error_in_mac',val);
					o.value = "";
					o.focus();
					return false;
				}
			}
			return true;
		}
		onError('error_in_mac',val);
		o.value = "";
		o.focus();
		return false;
	}
	return true;
}

// Field behavior control
function ctrlEnable(o, flag, cols)
{
	if ( flag === 0 ) {
		o.disabled = 1;
		o.style.backgroundColor = cols[0];
	} else {
		o.disabled = 0;
		o.style.backgroundColor = cols[1];
	}
}
function ctrlReadOnly(o, flag, cols)
{
	if ( flag === 0 ) {
		o.readOnly = 1;
		o.style.backgroundColor = cols[0];
	} else {
		o.readOnly = 0;
		o.style.backgroundColor = cols[1];
	}
}
function ctrlArrayEnable(o, flag)
{
	for (var i=0; i < o.length; i++){
		if (flag === 0) o[i].disabled = 1;
		else o[i].disabled = 0;
	}
}
function ctrlArrayCheck(o, flag)
{
	for (var i=0; i < o.length; i++){
		if ( i == flag ) o[i].checked = true;
		else o[i].checked = false;
	}
}

// Check that password is '******'
function isAsterix(str)
{
	for( var i=0; i<str.length; i++) {
		var c=str.charAt(i);
		if( c != '*' ) return false;
	}
	return true;
}

// Element offset on screen
function getOffsetSum(elem) {
    var top=0, left=0;
    while(elem) {
        top = top + parseFloat(elem.offsetTop);
        left = left + parseFloat(elem.offsetLeft);
        elem = elem.offsetParent;
    }
    
    return {top: Math.round(top), left: Math.round(left)};
}
function getElemRect(elem) {
    var body = document.body;
    var docElem = document.documentElement;
    var scrollTop = window.pageYOffset || docElem.scrollTop || body.scrollTop;
    var scrollLeft = window.pageXOffset || docElem.scrollLeft || body.scrollLeft;
    var clientTop = docElem.clientTop || body.clientTop || 0;
    var clientLeft = docElem.clientLeft || body.clientLeft || 0;
	if (elem.getBoundingClientRect) {
 		var box = elem.getBoundingClientRect();
	} else {
		var box = getOffsetSum(elem);
		box.top = box.top -  scrollTop + clientTop;
		box.left = box.left - scrollLeft + clientLeft;
	}
    return { t: Math.round(box.top), l: Math.round(box.left),
		h:elem.clientHeight, w:elem.clientWidth,
		height:body.clientHeight, width:body.clientWidth};
}

// Get element or aray of elements by id
function $id(){
	var elements = new Array();
	for(var i = 0; i < arguments.length; ++i){
		var element = arguments[i];
		if(typeof element == 'string')
			element = document.getElementById(element);
		if(arguments.length == 1)
			return element;
		elements.push(element);
	}
	return elements;
}

// Return router operation mode
function operationMode(state)
{
	var mode;
	if (state.wan_nat_x == '1') mode = 'Gateway';
	else if (state.wan_route_x == 'IP_Routed') mode = 'Router';
	else mode = 'AP';
	return mode;
}

