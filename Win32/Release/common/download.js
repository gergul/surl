method='DOWNLOAD';

var ready = true;
if(P.url == undefined){
    print('变量未定义:P.url\n');
	ready = false;
}
if(P.file == undefined){
    print('变量未定义:P.file\n');
	ready = false;
}

if (ready) {
	var httpCode = download(P.url, P.file);
	//print('code=' + httpCode.toString() + '\n');
}