
function showUsage() {
	print("P.serialize_file   : string 必须\n");
	print("P.serialize_data   : object 返回\n");
}

(function() {
	if (typeof(P) == "undefined"
	 || typeof(P.serialize_file) == "undefined"
	) {
		showUsage();
		return;
	}
	
	var jsText = readText(P.serialize_file);
	P.serialize_data = JSON.parse(jsText);

})();