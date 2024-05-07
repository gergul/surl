
//设置进度条
function progressCallback(dltotal, dlnow, ultotal, ulnow, userData) 
{
	if (ultotal != 0)
	{
		print('上传' + userData + ulnow.toString() + '/' + ultotal.toString() + '=' + Math.floor((ulnow / ultotal) * 100).toString() + '%\n');
	}
	
	if (dltotal != 0)
	{
		print('下载' + userData + dlnow.toString() + '/' + dltotal.toString() + '=' + Math.floor((dlnow / dltotal) * 100).toString() + '%\n');
	}
}
setProgress('progressCallback', '进度:');