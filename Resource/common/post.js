function showUsage() {
	print('P.url         : string 必须\n');
	print('P.content     : string<""> 可选\n');
	print('P.contentType : string<"application/x-www-form-urlencoded"> 可选\n');
}


(function() {
	if (typeof(P) == "undefined" 
	 || typeof(P.url) == "undefined"
	) {
		showUsage();
		return;
	}
	
	if(P.content == undefined){
		P.content = '';
	}
	if(P.contentType == undefined){
		P.contentType = 'application/x-www-form-urlencoded';
	}

	
	P.method='POST';
	setCustomMothod(P.method);
	
	var httpCode = post(P.url, P.content, P.contentType);
	
})();
