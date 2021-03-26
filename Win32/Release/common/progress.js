
//设置进度条
function progressCallback(dltotal, dlnow, ultotal, ulnow, userData) 
{
	if (dltotal != 0)
	{
		print(userData + dlnow.toString() + '/' + dltotal.toString() + '=' + Math.floor((dlnow / dltotal) * 100).toString() + '%\n');
	}
}
setProgress('progressCallback', '进度:');