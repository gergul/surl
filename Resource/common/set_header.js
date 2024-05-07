
if (Has(P.headers)){
	for (var key in P.headers) {
		setRequestHeader(key, P.headers[key]);
	}
}