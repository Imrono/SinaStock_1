#include <iostream>
using namespace std;
#include "ErrorCode2String.h"
#include "HttpUrl.h"
#include "TraceMicro.h"

HttpUrlGetSyn::HttpUrlGetSyn(const char* HttpName, DWORD dwAccessType) {
	strcpy_s(_HttpName, 128, HttpName);
	_dwAccessType = dwAccessType;
	_hSession = InternetOpen(HttpName, dwAccessType, NULL, NULL, 0);
	_bufSize = MAX_RECV_BUF_SIZE;
	_buf = new BYTE[_bufSize];
	memset(_buf, 0, _bufSize*sizeof(BYTE));
	STATIC_TRACE(URL_TRACE, "hSession (%s) initialed.\n", _HttpName);
}
HttpUrlGetSyn::HttpUrlGetSyn(const HttpUrlGetSyn &copy) {
	strcpy_s(_HttpName, 128, copy._HttpName);
	_dwAccessType = copy._dwAccessType;
	_hSession = InternetOpen(_HttpName, _dwAccessType, NULL, NULL, 0);
	_bufSize = MAX_RECV_BUF_SIZE;
	_buf = new BYTE[_bufSize];
	memset(_buf, 0, _bufSize*sizeof(BYTE));
	STATIC_TRACE(URL_TRACE, "hSession (%s) initialed using copy.\n", _HttpName);
}

HttpUrlGetSyn::~HttpUrlGetSyn() {
	InternetCloseHandle(_hSession);	_hSession = nullptr;
	delete []_buf; _buf = nullptr;
	_hHttp = nullptr;
	_bufSize = 0;
}
HINTERNET HttpUrlGetSyn::OpenUrl(const char* url) {
	_hHttp = InternetOpenUrl(_hSession, url, NULL, 0, INTERNET_FLAG_DONT_CACHE, 0);
	if (NULL != _hHttp) STATIC_TRACE(URL_TRACE, "successful opened:\n%s\n", url);
	else ERRR("OpenURL failed: ErrorCode:%d\n", GetLastError());
	return _hHttp;
}
void HttpUrlGetSyn::CloseUrl() {
	InternetCloseHandle(_hHttp);	_hHttp = NULL;
}

int HttpUrlGetSyn::ReadUrlOne(DWORD &Number) {
	int ret = InternetReadFile(_hHttp, _buf, _bufSize-1, &Number);
	_buf[Number] = 0;
	return ret;
}
int HttpUrlGetSyn::ReadUrlOne(char* OutBuffer, int &Length) {
	DWORD tmp = 0;
	int ret = InternetReadFile(_hHttp, OutBuffer, Length-1, &tmp);
	Length = (int)tmp;
	OutBuffer[Length] = 0;
	return ret;
}

void HttpUrlGetSyn::ReadUrlAll(string &data_recv) {

	int ReadTimes = 0;
	data_recv = "";
	DWORD Number = 0;
	while (1) {
		if (!InternetReadFile(_hHttp, _buf, _bufSize - 1, &Number))
			STATIC_TRACE(URL_TRACE, "Read Internet File failure!!!!!\n");
		_buf[Number] = '\0';
		data_recv += (char*)_buf;
		ReadTimes ++;
		if (Number) { 
// 			printf_s("Temp:\n%s\ndata_recv:\n%s", buf, data_recv.c_str());
			STATIC_TRACE(URL_TRACE, "ReadTimes:%d, ReadNum:%d\n", ReadTimes, Number);
		}
		else {break;}
	}
}
void HttpUrlGetSyn::ReadUrlAll(char* OutBuffer, int &Length) {
	int tmpLength = 0;
	int ReadTimes = 0;
	memset(OutBuffer, 0, Length);
	DWORD Number = 0;
	while (1) {
		if (!InternetReadFile(_hHttp, _buf, _bufSize - 1, &Number))
			STATIC_TRACE(URL_TRACE, "Read Internet File failure!!!!!\n");
		_buf[Number] = '\0';
		if (tmpLength+(int)Number < Length) {
			memcpy(OutBuffer+tmpLength, _buf, Number);
			tmpLength += Number;
		}
		ReadTimes ++;
		if (Number) { 
			// 			printf_s("Temp:\n%s\ndata_recv:\n%s", buf, data_recv.c_str());
			STATIC_TRACE(URL_TRACE, "ReadTimes:%d, ReadNum:%d\n", ReadTimes, Number);
		}
		else {break;}
	}
}


/////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////
HttpUrlGetAsyn::HttpUrlGetAsyn(const char* HttpName, DWORD dwAccessType) {
	hSession = InternetOpen(HttpName, dwAccessType, NULL, NULL, 0);
	if (NULL == hSession)	cout << "InternetOpen failed, error " << GetLastError();
	bufSize = MAX_RECV_BUF_SIZE;
	buf = new BYTE[bufSize];
	memset(buf, 0, bufSize*sizeof(BYTE));
	STATIC_TRACE(URL_TRACE, "hSession initialed.\n");
}
HttpUrlGetAsyn::~HttpUrlGetAsyn() {
	InternetCloseHandle(hSession);	hSession = NULL;
	delete []buf;
}



/////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////
AsyncWinINet::AsyncWinINet(const string& url, const string& _saved_filename): thd(url, _saved_filename)
{}

DWORD WINAPI AsyncWinINet::AsyncThread(LPVOID lpParameter)
{
	thread_info* p = (thread_info*)lpParameter;
	string user_agent("asyn test!!");
	DWORD dwError;
//	a. 使用标记 INTERNET_FLAG_ASYNC 初始化 InternetOpen
/////////////////////////////////////////////////////////////////////
	p->hInternet = InternetOpen(user_agent.c_str(), INTERNET_OPEN_TYPE_PRECONFIG, NULL, NULL, INTERNET_FLAG_ASYNC);
	STATIC_TRACE(URL_TRACE, "ASYN: InternetOpen after!!\n");
//	ResetEvent(p->hEvent[0]);
//	p->hCallbackThread = CreateThread(NULL,
//	0,
//	AsyncWinINet::AsyncCallbackThread,
//	p,
//	NULL,
//	&p->dwCallbackThreadID);
//	WaitForSingleObject(p->hEvent[0], INFINITE);//等待回调函数设置成功事件
	InternetSetStatusCallback(p->hInternet, AsyncWinINet::AsyncInternetCallback);

	FILE *fp = NULL;
	fopen_s(&fp, p->saved_filename.c_str(), "w+");


	ResetEvent(p->hEvent[HANDLE_SUCCESS]);	//重置句柄被创建事件
	p->hFile = InternetOpenUrl(p->hInternet, p->url.c_str(), NULL, NULL, INTERNET_FLAG_DONT_CACHE|INTERNET_FLAG_RELOAD, (DWORD)p);
	STATIC_TRACE(URL_TRACE, "ASYN: to open %s\n", p->url.c_str());
	while(true) {
		if (NULL == p->hFile) {
			dwError = ::GetLastError();
			STATIC_TRACE(URL_TRACE, "ASYN: InternetOpenUrl ERROR: %d->%s\n", dwError, ErrorCode2Str(dwError));
			if (ERROR_IO_PENDING == dwError || ERROR_SUCCESS == dwError) {
				if (WaitExitEvent(p)) { break; }
			}
			else break;
		}

		//读取返回文件头xxx
	
		//e. 使用 HttpQueryInfo 分析头信息 HttpQueryInfo 使用非阻塞方式，所以不用等待
		DWORD dwStatusSize = sizeof(p->dwStatusCode);
		if (false == HttpQueryInfo(p->hFile, HTTP_QUERY_STATUS_CODE | HTTP_QUERY_FLAG_NUMBER, &p->dwStatusCode, &dwStatusSize, NULL)) { break; }
		STATIC_TRACE(URL_TRACE, "ASYN: HttpQueryInfo!!\n");
		//判断状态码是不是 200
		if (HTTP_STATUS_OK != p->dwStatusCode) {
			STATIC_TRACE(URL_TRACE, "ASYN: HTTP_STATUS_OK is failed\n");
			break;
		}
	
		//获取返回的Content-Length
		//DWORD dwLengthSize = sizeof(p->dwContentLength); 
		//if (FALSE == HttpQueryInfo(p->hFile,
		//HTTP_QUERY_CONTENT_LENGTH | HTTP_QUERY_FLAG_NUMBER,
		//&p->dwContentLength, &dwLengthSize, NULL)) { p->dwContentLength = 0; }
	
		//f. 使用标记 IRF_ASYNC 读数据 InternetReadFileEx
		//为了向主线程报告进度，我们设置每次读数据最多 1024 字节

		char lpvBuffer[MAX_RECV_BUF_SIZE] = {0};
		p->dwContentLength = 0; //Content-Length: 202749
		INTERNET_BUFFERS i_buf = {0};
		i_buf.dwStructSize = sizeof(INTERNET_BUFFERS);
		i_buf.lpvBuffer = lpvBuffer;
		i_buf.dwBufferLength = MAX_RECV_BUF_SIZE-1;
		while(true)
		{
	
			//重置读数据事件
			ResetEvent(p->hEvent[HANDLE_SUCCESS]);
			STATIC_TRACE(URL_TRACE, "ASYN: InternetReadFileEx (before)!!\n");
			if (false == InternetReadFileEx(p->hFile, &i_buf, IRF_ASYNC, (DWORD)p)) {
				STATIC_TRACE(URL_TRACE, "ASYN: InternetReadFileEx == FALSE asyn!!\n");
				if (ERROR_IO_PENDING == (dwError = ::GetLastError())) {
					STATIC_TRACE(URL_TRACE, "ASYN: InternetReadFileEx ERROR: %d->%s\n", dwError, ErrorCode2Str(dwError));
					if (WaitExitEvent(p)) break;
				}
				else break; 
			}
			((char*)i_buf.lpvBuffer)[i_buf.dwBufferLength] = 0;
			DYNAMIC_TRACE(DATA_TRACE, "ASYN: %s\n", lpvBuffer);
			STATIC_TRACE(URL_TRACE, "ASYN: InternetReadFileEx ok, begin to write!!length=%d\n", i_buf.dwBufferLength);

			if(fp) fwrite(i_buf.lpvBuffer, sizeof(char), i_buf.dwBufferLength, fp);
			if (i_buf.dwBufferLength == 0) break;
		}
		break;
	}

	STATIC_TRACE(PROGRESS_TRACE, "ASYN: begin to CLOSE files!!\n");
	if(fp) { fflush(fp); fclose(fp); fp = NULL; }
	if(p->hFile) {
		InternetCloseHandle(p->hFile);	//关闭 m_hFile
		while (!WaitExitEvent(p)) {		//等待句柄被关闭事件或者要求子线程退出事件
			ResetEvent(p->hEvent[HANDLE_SUCCESS]);
		}
	}

	//设置子线程退出事件，通知回调线程退出
	SetEvent(p->hEvent[THREAD_EXIT]);
  
	//等待回调线程安全退出
	//WaitForSingleObject(p->hCallbackThread, INFINITE);
	//CloseHandle(p->hCallbackThread);

	//注销回调函数
	InternetSetStatusCallback(p->hInternet, NULL);
	InternetCloseHandle(p->hInternet);

	return TRUE;
}

//------------------------------------------------------------------------------------
DWORD WINAPI AsyncWinINet::AsyncCallbackThread(LPVOID lpParameter)
{
	thread_info *p = (thread_info*)lpParameter;
	InternetSetStatusCallback(p->hInternet, AsyncWinINet::AsyncInternetCallback);

	//通知子线程回调函数设置成功，子线程可以继续工作
	SetEvent(p->hEvent[HANDLE_SUCCESS]);

	//等待用户终止事件或者子线程结束事件
	//子线程结束前需要设置子线程结束事件，并等待回调线程结束
	WaitForSingleObject(p->hEvent[THREAD_EXIT], INFINITE);

	return 0;
}

//----------------------------------------------------------------------------
void CALLBACK AsyncWinINet::AsyncInternetCallback(HINTERNET hInternet,
	DWORD dwContext,
	DWORD dwInternetStatus,
	LPVOID lpvStatusInformation,
	DWORD dwStatusInformationLength)
{
// 	STATIC_TRACE(CALLBACE_TRACE, "dwContext:%d, dwInternetStatus:%d, dwStatusInformationLength:%d\n", dwContext, dwInternetStatus, dwStatusInformationLength);
	thread_info* p = (thread_info*)dwContext;

	//在我们的应用中，我们只关心下面三个状态
	switch(dwInternetStatus)
	{
		//句柄被创建
		case INTERNET_STATUS_HANDLE_CREATED:
			p->hFile = (HINTERNET)(((LPINTERNET_ASYNC_RESULT)(lpvStatusInformation))->dwResult);
			STATIC_TRACE(CALLBACE_TRACE, "HINTERNET created!! no event set\n");
			break;
  
		//句柄被关闭
		case INTERNET_STATUS_HANDLE_CLOSING:
			STATIC_TRACE(CALLBACE_TRACE, "HINTERNET closed!! event HANDLE_CLOSE set\n");
			SetEvent(p->hEvent[HANDLE_CLOSE]);
			break;

		//一个请求完成，比如一次句柄创建的请求，或者一次读数据的请求
		case INTERNET_STATUS_REQUEST_COMPLETE:
			if (ERROR_SUCCESS == ((LPINTERNET_ASYNC_RESULT)(lpvStatusInformation))->dwError) {
				STATIC_TRACE(CALLBACE_TRACE, "句柄被创建或读数据成功, event HANDLE_SUCCESS set\n");
				SetEvent(p->hEvent[HANDLE_SUCCESS]);
			}
			else {
				STATIC_TRACE(CALLBACE_TRACE, "如发生错误，则设置子线程退出, event THREAD_EXIT set\n");
				SetEvent(p->hEvent[THREAD_EXIT]);
			}
			break;

		case INTERNET_STATUS_CONNECTION_CLOSED:
			STATIC_TRACE(CALLBACE_TRACE, "HINTERNET connection closed!! event THREAD_EXIT set\n");
			SetEvent(p->hEvent[THREAD_EXIT]);
			break;
	}
}

//--------------------------------------------------------------------
BOOL AsyncWinINet::WaitExitEvent(thread_info *p)
{
	DWORD dwRet = WaitForMultipleObjects(3, p->hEvent, FALSE, INFINITE);

	switch (dwRet)
	{
		case WAIT_OBJECT_0:		//句柄被创建或读数据请求成功完成
			STATIC_TRACE(WAIT_TRACE, "HANDLE CREATE/SUCCESS OPERATION!!\n");
			break;
		case WAIT_OBJECT_0+1:	//句柄被关闭
			STATIC_TRACE(WAIT_TRACE, "HANDLE CLOSE!!\n");
			break;
		case WAIT_OBJECT_0+2:	//终止子线程或发生错误
			STATIC_TRACE(WAIT_TRACE, "THREAD EXIT!!\n");
			break;
		default:
			STATIC_TRACE(WAIT_TRACE, "(ERROR) wait signal is unexpected!!\n");
	}
	return WAIT_OBJECT_0 != dwRet;
}