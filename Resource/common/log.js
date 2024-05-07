
var logfilename = "";

if(P.method != undefined){
    logfilename = P.method + '-';
}

var myDate = new Date();
logfilename += myDate.getFullYear() + '-';
logfilename += (myDate.getMonth()+1).toString() + '-';
logfilename += myDate.getDate();

logfilename += '.log';

var requestText = JSON.stringify(P);
if (requestText.length > 1024) {
	requestText = requestText.substr(0, 1024) + '...';
}

var bodyText = getBody();
if (bodyText.length > 1024) {
	bodyText = bodyText.substr(0, 1024) + '...';
}

var text = '[' + myDate.toLocaleTimeString() + ']>>>\n'
text += 'request=' + requestText + '\n';
text += 'code=' + getCode() + '\n';
text += 'body=' + bodyText + '\n\n';
appendText('logs/' + logfilename, text);

