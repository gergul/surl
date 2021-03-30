
//设置post的参数
P.url='http://localhost/posttest.php';
P.content='{ "gType": 0, "projectNos": [ "100452001" ] }';
P.contentType='application/json;charset=utf-8';

setRequestHeader('apikey', 'testkey');