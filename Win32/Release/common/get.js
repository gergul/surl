P.method='GET';

setCustomMothod(P.method);

var ready = true;
if(P.url == undefined){
    print('变量未定义:P.url\n');
	ready = false;
}

if (ready) {
	var httpCode = 0;
	if(P.params != undefined){
		httpCode = get_a(P.url, P.params);	
	} else {
		httpCode = get(P.url);
	}
	/*
	print('code=' + httpCode.toString() + '\n');
	if (httpCode == 200) {		
		print('body=' + getBody() + '\n');
	}*/
}