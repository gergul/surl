
while ((Location = getResponseHeader('Location')) != "") {
	print('重定向到：' + Location + '\n');
	P.url = Location;
	include('common/get.js');
}
