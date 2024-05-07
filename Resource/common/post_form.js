function showUsage() {
	print('P.url : string 必须\n');
	print('P.fields : string 必须 eg.[{"type":0,"name":"field1","value":"text1"},{"type":1,"name":"file","value":"C:/1.txt","fileName":"1.txt"},...]\n');
}


(function() {
	if (typeof(P) == "undefined" 
	 || typeof(P.url) == "undefined"
	) {
		showUsage();
		return;
	}
		
	if(P.fields == undefined){
		P.fields = '[]';
	} else if (Array.isArray(P.fields)) {
		P.fields = JSON.stringify(P.fields);
	} else if (typeof P.fields === 'string') {
		//...
	} else {
		showUsage();
		return;
	}

	
	P.method='POST';
	setCustomMothod(P.method);
	
	var httpCode = postForm(P.url, P.fields);
	
})();
