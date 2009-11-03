function gID(s) { return document.getElementById(s); }
/* �إ�XMLHttpRequest���� */
function JSONXMLHttpReq(){
	var objxml = false;
	/*@cc_on @*/
	/*@if (@_jscript_version >= 5)
	try{
		objxml = new ActiveXObject("Msxml2.XMLHTTP");
	}catch(e){
		try{
			objxml = new ActiveXObject("Microsoft.XMLHTTP");
		}catch(e2){ objxml = false; }
	}
	@end @*/
	if(!objxml && typeof XMLHttpRequest!='undefined') {
		objxml = new XMLHttpRequest();
		if(objxml.overrideMimeType) objxml.overrideMimeType('text/plain');
	}
	return objxml;
}
var xhttpjson=JSONXMLHttpReq();

function ParseProgress(){
	if(xhttpjson.readyState==4){ // Ū������
		percent = xhttpjson.responseText;
		if(percent > 0) {
			gID("progressbarborder").style.display="block";
			gID("progressbartext").innerHTML = percent+"%";
			gID("progressbar").style.width = percent+"%";
		}
		if(percent < 100){
			setTimeout("getProgress()", 200);
		}
	}
}
function getProgress(){
	if(xhttpjson){
		xhttpjson.open('GET','progress.php?progress_key='+up_uid, true);
		xhttpjson.onreadystatechange = ParseProgress;
		xhttpjson.send(null);
	}
}
 
function startProgress(){
		gID("up_progress").style.display="block";
		setTimeout("getProgress()", 1000);
}

/* Ū��Cookies�� */
function getCookie(key){
	var tmp1, tmp2, xx1 = 0, xx2 = 0, xx3;
	tmp1 = ' '+document.cookie+';';
	var len = tmp1.length;
	while(xx1 < len){
		xx2 = tmp1.indexOf(';', xx1);
		tmp2 = tmp1.substring(xx1 + 1, xx2);
		xx3 = tmp2.indexOf('=');
		if(tmp2.substring(0, xx3)==key){ return unescape(tmp2.substring(xx3 + 1, xx2 - xx1 - 1)); }
		xx1 = xx2 + 1;
	}
	return '';
}

/* �g�JCookies�� */
function setCookie(name, value){
	var exp = new Date();
	exp.setTime(exp.getTime() + 86400000 * 7);
	document.cookie = name+'='+escape(value)+'; expires='+exp.toGMTString();
}
