
var ready = true;
if(P.bodyToFile == undefined || P.bodyToFile == null || P.bodyToFile == ""){
    print('变量未定义:P.bodyToFile\n');
	ready = false;
}

if (ready) {
	writeText(P.bodyToFile, getBody());
}