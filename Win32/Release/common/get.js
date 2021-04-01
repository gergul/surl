function showUsage() {
	print("P.url    : string 必须\n");
	print("P.params : {string:string,...} 可选\n");
}
	
(function() {
	if (typeof(P) == "undefined" 
	 || typeof(P.url) == "undefined"
	) {
		showUsage();
		return;
	}
		
	P.method='GET';
	setCustomMothod(P.method);	

	var httpCode = 0;
	if(P.params != undefined){
		httpCode = get_a(P.url, P.params);	
	} else {
		httpCode = get(P.url);
	}
})();
