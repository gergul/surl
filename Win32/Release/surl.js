var Has = function () {
	for(var i=0; i<arguments.length; ++i) {
		var item = arguments[i];
		try {
			var o = eval(item);
			if (o == undefined || o == null || o == NaN)
				return false;
		} catch(err) {
			return false;
		}
	}
	return true;
}
