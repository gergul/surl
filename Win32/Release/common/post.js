P.method='POST';

setCustomMothod(P.method);

var ready = true;
if(P.url == undefined){
    print('变量未定义:P.url\n');
	ready = false;
}
if(P.content == undefined){
    P.content = '';
}
if(P.contentType == undefined){
    P.contentType = 'application/x-www-form-urlencoded';
}

if (ready) {
	var httpCode = post(P.url, P.content, P.contentType);
	/*print('code=' + httpCode.toString() + '\n');
	if (httpCode == 200) {		
		print('body=' + getBody() + '\n');
	}*/
}
