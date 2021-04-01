
function showUsage() {
	print("P.bodyToFile   : string 必须\n");
}

(function() {
	if (typeof(P) == "undefined" 
	 || typeof(P.bodyToFile) == "undefined"
	) {
		showUsage();
		return;
	}

	writeText(P.bodyToFile, getBody());

})();