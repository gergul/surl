// surl.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include <windows.h>
#include <dukglue.h>
#include <json/json.h>
#include <iostream>
#include <sstream>
#include "LibcurlHttp.h"
#include <algorithm>
#include "InputBoxW.h"

#pragma execution_character_set("utf-8")


char illegalCharset[127] = {
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,1,1,0,1,1,1,1,1,1,1,
	0,1,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0,1,0,1,0,
	0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,1,0,1,1,0,1,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,1
};

std::string readText(const char * path);
size_t writeFile(const char * path, const char * writeContent, size_t & in_outLen, int start = -1, bool bInsert = true, bool bDelTail = true);
std::string validUrl(const char* url);
void StringSplit(const std::string & s, const std::string & delim, std::vector<std::string>& ret);
void StringTrim(std::string& str, const std::string& trimChars = " \n\r\t");
std::string GetFileNameFromPath(const std::string & sPath);
bool IsFileExitst(const std::string & filePath);


std::string jsProgressCallbackName;
std::string sProgressCallbackUserData;
int ProgressCallback(
	double dltotal,
	double dlnow,
	double ultotal,
	double ulnow,
	void* userData)
{
	if (jsProgressCallbackName.empty() || jsProgressCallbackName == "")
		return 0;

	duk_context *ctx = (duk_context *)userData;
	std::stringstream fmt;
	fmt << jsProgressCallbackName << "(" << dltotal << ',' << dlnow << ',' << ultotal << ',' << ulnow << ',' << "'" << sProgressCallbackUserData << "')";
	dukglue_peval<void>(ctx, fmt.str().c_str());

	return 0;
}


void js_setHttpTimeout(int second)
{
	HTTP_CLIENT::Ins().setTimeout(second);
}

void js_setRequestHeader(const char* key, const char* value)
{
	HTTP_CLIENT::Ins().setRequestHeader(key, value);
}

void js_setUserAgent(const char* val)
{
	HTTP_CLIENT::Ins().setUserAgent(val);
}

void js_setCustomMothod(const char* mothod)
{
	HTTP_CLIENT::Ins().setCustomMothod(mothod);
}

/*
progressCallbackName(
	double dltotal,
	double dlnow,
	double ultotal,
	double ulnow,
	string userData)
*/
void js_setProgress(const char* progressCallbackName, const char* userData)
{
	jsProgressCallbackName = progressCallbackName;
	sProgressCallbackUserData = userData;
}

void js_setAutoRedirect(bool autoRedirect)
{
	HTTP_CLIENT::Ins().setAutoRedirect(autoRedirect);
}
void js_setMaxRedirect(int maxRedirect)
{
	HTTP_CLIENT::Ins().setMaxRedirect(maxRedirect);
}

int js_get(const char* url)
{
	return HTTP_CLIENT::Ins().get(validUrl(url).c_str());
}

int js_get_a(const char* url, const std::map<std::string, std::string>& params)
{
	std::string sParam;
	for (auto p = params.begin(); p != params.end(); ++p)
	{
		if (!sParam.empty())
			sParam += '&';
		sParam += p->first + "=" + p->second;
	}
	std::string sUrl = validUrl(url) + std::string("?") + sParam;
	return HTTP_CLIENT::Ins().get(sUrl.c_str());
}

int js_post(const char* url, const char* content, const char* contentType)
{
	return HTTP_CLIENT::Ins().post(validUrl(url).c_str(), content, strlen(content), contentType);
}

int js_post_a(const char* url, const std::map<std::string, std::string>& params)
{
	std::string sParam;
	for (auto p = params.begin(); p != params.end(); ++p)
	{
		if (!sParam.empty())
			sParam += '&';
		sParam += p->first + "=" + p->second;
	}

	return HTTP_CLIENT::Ins().post(validUrl(url).c_str(), sParam.data(), sParam.size(), "application/x-www-form-urlencoded");
}

/*
[
	{
		"name" : "field1", 
		"value" : "text1", 
		"type" : 0 //0为普通字段（可选）
	 },

	{
		"type" : 1, //1为文件字段（必选）
		"name" : "file", 
		"value" : "C:/1.txt", //文件路径
		"fileName" : "1.txt" //文件名（可选）
	},
]
*/
int js_postForm(const char* url, const char* fieldsJsonStr)
{
	Json::Value js;
	Json::Reader jsReader;
	if (!jsReader.parse(fieldsJsonStr, js, false))
		return -1;

	if (!js.isArray())
		return -2;

	int nFieldCount = js.size();
	FORM_FIELD* formFields = new FORM_FIELD[nFieldCount];

	for (int i = 0; i < js.size(); ++i)
	{
		if (!js[i]["name"].isString() || !js[i]["value"].isString())
		{
			--nFieldCount;
			continue;
		}
		int ftype = 0;
		std::string sFileName;
		if (js[i]["type"].isNumeric() && js[i]["type"].asInt() == 1)
		{//是文件
			ftype = 1;
			if (js[i]["fileName"].isString())
				sFileName = js[i]["fileName"].asString();
		}
		FillFormField(formFields[i], ftype, js[i]["name"].asCString(), js[i]["value"].asCString(), sFileName.c_str());
	}
	
	int code = 0;
	if (nFieldCount > 0)
		code = HTTP_CLIENT::Ins().postForm(validUrl(url).c_str(), formFields, nFieldCount);

	delete[] formFields;

	return code;
}

int js_download(const char* url, const char* localFileName)
{
	return HTTP_CLIENT::Ins().download(validUrl(url).c_str(), localFileName);
}

std::string js_getBody()
{
	int len = 0;
	const char* pBody = HTTP_CLIENT::Ins().getBody(len);
	return std::string(pBody, len);
}

int js_getCode()
{
	return HTTP_CLIENT::Ins().getCode();
}

std::string js_getResponseHeader(const char* key)
{
	std::string sValues = "";

	int nValCount = HTTP_CLIENT::Ins().getResponseHeadersCount(key);
	for (int i = 0; i < nValCount; ++i)
	{
		if (sValues != "")
			sValues += "\n";
		sValues += HTTP_CLIENT::Ins().getResponseHeader(key, i);
	}

	return sValues;
}

const char* js_urlGB2312Encode(const char * strIn)
{
	return HTTP_CLIENT::Ins().UrlGB2312Encode(strIn);
}
const char* js_urlGB2312Decode(const char * strIn)
{
	return HTTP_CLIENT::Ins().UrlGB2312Decode(strIn);
}
const char* js_urlUTF8Encode(const char * strIn)
{
	return HTTP_CLIENT::Ins().UrlUTF8Encode(strIn);
}
const char* js_urlUTF8Decode(const char * strIn)
{
	return HTTP_CLIENT::Ins().UrlUTF8Decode(strIn);
}
const char* js_utf8ToAnsi(const char * strIn)
{
	return HTTP_CLIENT::Ins().UTF8ToAnsi(strIn);
}
const char* js_ansiToUTF8(const char * strIn)
{
	return HTTP_CLIENT::Ins().AnsiToUTF8(strIn);
}


void js_print(const char* text)
{
	std::string s = HTTP_CLIENT::Ins().UTF8ToAnsi(text);
	std::cout << s;
}

void js_alert(const char* text)
{
	std::string s = HTTP_CLIENT::Ins().UTF8ToAnsi(text);
	MessageBoxA(NULL, s.c_str(), "surl", MB_OK);
}

std::string js_input()
{
	std::string s;
	std::getline(std::cin, s);
	s = HTTP_CLIENT::Ins().AnsiToUTF8(s.c_str());
	return s;
}

std::string js_inputBox(const char* tip, const char* defVal, const char* title)
{
	std::wstring sTip = HTTP_CLIENT::Ins().UTF8ToWidebyte(tip);
	std::wstring sDefVal = HTTP_CLIENT::Ins().UTF8ToWidebyte(defVal);
	std::wstring sTitle = HTTP_CLIENT::Ins().UTF8ToWidebyte(title);
	std::wstring s = _InputBoxW(sTip.c_str(), sTitle.c_str(), sDefVal.c_str());
	return HTTP_CLIENT::Ins().WidebyteToUTF8(s.c_str());
}

std::string js_readText(const char * path)
{
	return readText(HTTP_CLIENT::Ins().UTF8ToAnsi(path));
}

size_t js_writeText(const char * path, const char * writeContent)
{
	size_t in_outLen = strlen(writeContent);	
	return writeFile(HTTP_CLIENT::Ins().UTF8ToAnsi(path), writeContent, in_outLen, 0, false, true);
}

size_t js_appendText(const char * path, const char * writeContent)
{
	size_t in_outLen = strlen(writeContent);
	return writeFile(path, writeContent, in_outLen, -1, true, false);
}

duk_ret_t js_include(duk_context * ctx)
{
	int n = duk_get_top(ctx);  /* #argc */
	if (n < 1)
		return 0;

	bool bCanReInclude = true;
	if (duk_is_boolean(ctx, n - 1))
	{
		bCanReInclude = (bool)duk_get_boolean(ctx, n - 1);
		--n;
	}

	std::string sResult;
	for (int i = 0; i < n; ++i)
	{
		const char *pFileName = duk_to_string(ctx, i);
		if (!pFileName)
			continue;

		//寻找包含目录
		std::string sFileInclude = pFileName;

		//执行包含
		do
		{
			try
			{
				char buff[1024];

				std::string sIncludeName = GetFileNameFromPath(sFileInclude.c_str()).c_str();
				
				//include once
				std::string sOnceIncludeName = sIncludeName;
				//过滤掉文件名中不能组成变量的字符
				for (int c = 0; c < sOnceIncludeName.length(); ++c)
				{
					if (0 <= sOnceIncludeName[c] && sOnceIncludeName[c] < 127 && illegalCharset[sOnceIncludeName[c]])
						sOnceIncludeName[c] = '_';
				}
				std::transform(sOnceIncludeName.begin(), sOnceIncludeName.end(), sOnceIncludeName.begin(), ::toupper);
				sOnceIncludeName = "INCLUDED_" + sOnceIncludeName;
				
				if (!bCanReInclude)
				{//检测是否已经被包含过
					sprintf(buff,
						"if (typeof %s != 'undefined' && %s)"
						"	true;"
						"else"
						"	false;",
						sOnceIncludeName.c_str(), sOnceIncludeName.c_str());
					bool bIncluded = dukglue_peval<bool>(ctx, buff);
					if (bIncluded)
					{
						break;
					}
				}
				
				//include
				dukglue_peval<void>(ctx, readText(sFileInclude.c_str()).c_str());

				//mark as included
				sprintf(buff, "var %s=true;", sOnceIncludeName.c_str());
				dukglue_peval<DukValue>(ctx, buff);
			}
			catch (const std::exception& e)
			{
				printf("Include error (%s)\n[ %s ]\n", sFileInclude.c_str(), e.what());
				dukglue_push(ctx, false);
				return 1;
			}
		} while (0);

	}

	dukglue_push(ctx, true);
	return 1;
}

typedef struct ParamsInfo
{
	std::string data;
	bool isFile;
} ParamsInfo;

int main(int argc, char** argv)
{
	if (argc < 2)
	{
		printf("surl scriptFile1 [scriptFile2 scriptFile3 ...] [-d define1 -d define2 ...]\n");
		return 1;
	}

	std::vector<ParamsInfo> vctParams;
	for (int i = 1; i < argc; ++i)
	{
		ParamsInfo pi;
		if (strcmp(argv[i], "-d") == 0)
		{
			if (i < argc - 1)
			{
				pi.data = argv[++i];
				pi.isFile = false;
			}
		}
		else
		{
			pi.data = argv[i];
			pi.isFile = true;
		}

		vctParams.push_back(pi);
	}

	duk_context *ctx = NULL;
	ctx = duk_create_heap_default();

	//初始化http
	HTTP_CLIENT::Ins().setProgress(ProgressCallback, ctx);

	std::string sCurScriptFile;
	try
	{
		//初始化js
		dukglue_register_function(ctx, &js_setHttpTimeout,		"setHttpTimeout");
		dukglue_register_function(ctx, &js_setRequestHeader,	"setRequestHeader");
		dukglue_register_function(ctx, &js_setUserAgent,		"setUserAgent");
		dukglue_register_function(ctx, &js_setCustomMothod,		"setCustomMothod");
		dukglue_register_function(ctx, &js_setProgress,			"setProgress");
		dukglue_register_function(ctx, &js_setAutoRedirect,		"setAutoRedirect");
		dukglue_register_function(ctx, &js_setMaxRedirect,		"setMaxRedirect");
		dukglue_register_function(ctx, &js_get,					"get");
		dukglue_register_function(ctx, &js_get_a,				"get_a");
		dukglue_register_function(ctx, &js_post,				"post");
		dukglue_register_function(ctx, &js_post_a,				"post_a");
		dukglue_register_function(ctx, &js_postForm,			"postForm");
		dukglue_register_function(ctx, &js_download,			"download");
		dukglue_register_function(ctx, &js_getBody,				"getBody");
		dukglue_register_function(ctx, &js_getCode,				"getCode");
		dukglue_register_function(ctx, &js_getResponseHeader,	"getResponseHeader");
		dukglue_register_function(ctx, &js_urlGB2312Encode,		"urlGB2312Encode");
		dukglue_register_function(ctx, &js_urlGB2312Decode,		"urlGB2312Decode");
		dukglue_register_function(ctx, &js_urlUTF8Encode,		"urlUTF8Encode");
		dukglue_register_function(ctx, &js_urlUTF8Decode,		"urlUTF8Decode");
		dukglue_register_function(ctx, &js_utf8ToAnsi,			"utf8ToAnsi");
		dukglue_register_function(ctx, &js_ansiToUTF8,			"ansiToUTF8");
		dukglue_register_function(ctx, &js_print,				"print");
		dukglue_register_function(ctx, &js_alert,				"alert");
		dukglue_register_function(ctx, &js_input,				"input");
		dukglue_register_function(ctx, &js_inputBox,			"inputBox");
		dukglue_register_function(ctx, &js_readText,			"readText");
		dukglue_register_function(ctx, &js_writeText,			"writeText");
		dukglue_register_function(ctx, &js_appendText,			"appendText");
		dukglue_register_primitive_function(ctx, &js_include,	"include");

		//执行脚本
		for (int i = 0; i < vctParams.size(); ++i)
		{
			ParamsInfo& pi = vctParams[i];

			if (pi.isFile)
			{
				sCurScriptFile = pi.data;
				if (!IsFileExitst(sCurScriptFile))
				{
					sCurScriptFile = HTTP_CLIENT::Ins().UTF8ToAnsi(sCurScriptFile.c_str());
					if (!IsFileExitst(sCurScriptFile))
						throw std::exception("脚本不存在。");

				}
				std::string sScript = readText(sCurScriptFile.c_str());
				dukglue_peval<void>(ctx, sScript.c_str());
			}
			else
			{
				sCurScriptFile = pi.data;
				dukglue_peval<void>(ctx, pi.data.c_str());
			}
		}		
	}
	catch (DukErrorException ex)
	{
		std::cout << '“' << sCurScriptFile.c_str() << "”出错，出错信息为：" << std::endl
			<< ex.what() << std::endl;
	}
	catch (std::exception ex)
	{
		std::cout << '“' << sCurScriptFile.c_str() << "”出错，出错信息为：" << std::endl
			      << ex.what() << std::endl;
	}

	duk_destroy_heap(ctx);
    return 0;
}

size_t writeFile(const char * path, const char * writeContent, size_t & in_outLen, int start/* = -1*/, bool bInsert/* = true*/, bool bDelTail/* = true*/)
{
	if (!path)
	{
		return -1;
	}

	FILE *f = NULL;
	if ((f = fopen(path, "rb+")) == NULL)
	{
		//文件不存在，则新建一个空的
		if ((f = fopen(path, "wb")) == NULL)
		{
			return -1;
		}
		fclose(f);

		if ((f = fopen(path, "rb+")) == NULL)
			return -1;
	}

	do
	{
		int nFileSize = 0;
		if (fseek(f, 0, SEEK_END) != 0)
		{
			nFileSize = 0;
		}
		else
		{
			nFileSize = ftell(f);
		}
		if (nFileSize < 0)
		{
			break;
		}

		char* pOldStart = NULL;
		char* pOldEnd = NULL;
		if (start > -1)
		{
			if (nFileSize < start)
			{//插入处比原有的文件大，则在原文件尾到插入处空白的这些位置填充NULL
				int nSpace = start - nFileSize + 1;
				char *pSpace = new char[nSpace];
				memset(pSpace, 0, nSpace);
				fwrite(pSpace, nSpace, 1, f);
				delete[] pSpace;
			}
			else if (bInsert && (nFileSize > start))
			{//在中间插入要写的内容
			 //读取插入处到原文件结尾处的内容，以回写
				int nRead = (nFileSize - start);
				fseek(f, start, SEEK_SET);
				pOldEnd = new char[nRead];
				fread(pOldEnd, 1, nRead, f);
			}
			else if (bDelTail && (nFileSize > start) && (in_outLen < nFileSize - start))
			{//回写插入处前面的内容，而丢弃插入处后面的内容
			 //读取插入处前面的内容
				if (start > 0)
				{
					fseek(f, 0, SEEK_SET);
					pOldStart = new char[start];
					fread(pOldStart, 1, start, f);
				}

				//删除原有的文件，新建一个空的
				fclose(f);
				remove(path);
				if ((f = fopen(path, "wb")) == NULL)
				{
					return -1;
				}
				//回写插入处前面的内容，而丢弃插入处后面的内容
				if (start > 0)
					fwrite(pOldStart, 1, start, f);
			}

			if (fseek(f, start, SEEK_SET) != 0)
			{
				if (pOldEnd)
				{
					delete[] pOldEnd;
				}
				break;
			}
		}

		in_outLen = fwrite(writeContent, 1, in_outLen, f);
		if (pOldEnd)
		{
			fwrite(pOldEnd, 1, (nFileSize - start), f);
			delete[] pOldEnd;
		}

	} while (0);

	int nFileSize = 0;
	if (fseek(f, 0, SEEK_END) != 0)
	{
		nFileSize = -1;
	}
	else
	{
		nFileSize = ftell(f);
	}

	fclose(f);
	return nFileSize;
}

std::string readText(const char * path)
{
	FILE *f = NULL;
	long sz;

	if (!path)
	{
		return "";
	}

	std::string sRet;

	f = fopen(path, "rb");
	if (!f)
	{
		return "";
	}

	do
	{
		if (fseek(f, 0, SEEK_END) < 0)
		{
			break;
		}

		sz = ftell(f);
		if (sz < 0)
		{
			break;
		}

		if (fseek(f, 0, SEEK_SET) < 0)
		{
			break;
		}

		sRet.resize((size_t)sz + 1, '\0');

		if ((size_t)fread(const_cast<char*>(sRet.c_str()), 1, (size_t)sz, f) != (size_t)sz)
		{
			sRet = "";
			break;
		}
	} while (0);

	fclose(f);

	return sRet;
}

std::string validUrl(const char* url)
{
	if (url == NULL)
		return "";
	return HTTP_CLIENT::Ins().UTF8ToAnsi(url);
}

void StringSplit(const std::string & s, const std::string & delim, std::vector<std::string>& ret)
{
	size_t last = 0;
	size_t index = s.find_first_of(delim, last);
	while (index != std::string::npos)
	{
		ret.push_back(s.substr(last, index - last));
		last = index + 1;
		index = s.find_first_of(delim, last);
	}
	if (index - last > 0)
	{
		ret.push_back(s.substr(last, index - last));
	}
}

void StringTrim(std::string& str, const std::string& trimChars)
{
	if (!str.empty())
	{
		str.erase(0, str.find_first_not_of(trimChars));
		str.erase(str.find_last_not_of(trimChars) + 1);
	}
}

std::string GetFileNameFromPath(const std::string & sPath)
{
	size_t split1 = sPath.find_last_of('/');
	size_t split2 = sPath.find_last_of('\\');
	size_t split = 0;
	if (split1 == std::string::npos)
		split = split2;
	else if (split2 == std::string::npos)
		split = split1;
	else
		split = max(split1, split2);

	if (std::string::npos == split)
		return sPath;

	return sPath.substr(split + 1).c_str();
}


bool IsFileExitst(const std::string & filePath)
{
	WIN32_FIND_DATAA wfd;
	bool rValue = false;
	HANDLE hFind = FindFirstFileA(filePath.c_str(), &wfd);
	if ((hFind != INVALID_HANDLE_VALUE) && !(wfd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))
	{
		rValue = true;
	}
	FindClose(hFind);
	return rValue;
}