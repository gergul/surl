
function showUsage() {
	print("P.url    : string 必须\n");
	print("P.file   : string 必须\n");
}

(function() {
	if (typeof(P) == "undefined" 
	 || typeof(P.url) == "undefined"
	 || typeof(P.file) == "undefined"
	) {
		showUsage();
		return;
	}
	
	P.method='DOWNLOAD';

	var httpCode = download(P.url, P.file);
	
})();