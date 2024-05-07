
var cookies_re = "";

var rc = getResponseHeader('Set-Cookie');
print('获得Cookies:' + rc + '\n');
var cookies = rc.split('\n');
for(var i=0; i<cookies.length; ++i) {	
	var cookieFields = cookies[i].split(';');
	for (var j=0; j<cookieFields.length; ++j) {		
		var kv = cookieFields[j].split('=');
		if (kv.length != 2)
			continue;
		var k = kv[0].trim();
		if (k != 'Domain' && k != 'Path') {
			if (cookies_re != "")
				cookies_re += ';';
			cookies_re += cookieFields[j];
		}
	}
}
print('设置新Cookies:' + cookies_re + '\n');

setRequestHeader('Cookie', cookies_re);