
function showUsage() {
	print("P.serialize_data   : string 必须\n");	
	print("P.serialize_file   : string 必须\n");
}

(function() {
	if (typeof(P) == "undefined" 
	 || typeof(P.serialize_data) == "undefined"
	 || typeof(P.serialize_file) == "undefined"
	) {
		showUsage();
		return;
	}

	writeText(P.serialize_file, JSON.stringify(P.serialize_data));

})();