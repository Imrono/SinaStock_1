#include <iostream>
using namespace std;

#include "HttpUrl.h"


HttpUrlGetSyn::HttpUrlGetSyn(const char* HttpName, DWORD dwAccessType) {
	hSession = InternetOpen(HttpName, dwAccessType, NULL, NULL, 0);
	bufSize = MAX_RECV_BUF_SIZE;
	buf = new BYTE[bufSize];
	memset(buf, 0, bufSize*sizeof(BYTE));
	printf_s("hSession initialed.\n");
}
HttpUrlGetSyn::~HttpUrlGetSyn() {
	InternetCloseHandle(hSession);	hSession = NULL;
	delete []buf;
}
HINTERNET HttpUrlGetSyn::OpenUrl(const char* url) {
	hHttp = InternetOpenUrl(hSession, url, NULL, 0, INTERNET_FLAG_DONT_CACHE, 0);
	if (NULL != hHttp) printf_s("successful opened:\n%s\n", url);
	return hHttp;
}
void HttpUrlGetSyn::CloseUrl() {
	InternetCloseHandle(hHttp);	hHttp = NULL;
}

int HttpUrlGetSyn::ReadUrlOne(DWORD &Number) {
	int ret = InternetReadFile(hHttp, buf, bufSize-1, &Number);
	buf[Number] = 0;
	return ret;
}
void HttpUrlGetSyn::ReadUrlAll(string &data_recv) {

	int ReadTimes = 0;
	data_recv = "";
	DWORD Number = 0;
	while (1) {
		if (!InternetReadFile(hHttp, buf, bufSize - 1, &Number))
			printf_s("Read Internet File failure!!!!!\n");
		buf[Number] = '\0';
		data_recv += (char*)buf;
		ReadTimes ++;
		if (Number) { 
// 			printf_s("Temp:\n%s\ndata_recv:\n%s", buf, data_recv.c_str());
			printf_s("ReadTimes:%d, ReadNum:%d\n", ReadTimes, Number);
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
	printf_s("hSession initialed.\n");
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

	//a. 使用标记 INTERNET_FLAG_ASYNC 初始化 InternetOpen
	string user_agent("asyn test!!");

	p->hInternet = InternetOpen(user_agent.c_str(), INTERNET_OPEN_TYPE_PRECONFIG, NULL, NULL, INTERNET_FLAG_ASYNC);

//	ResetEvent(p->hEvent[0]);
//	p->hCallbackThread = CreateThread(NULL,
//	0,
//	AsyncWinINet::AsyncCallbackThread,
//	p,
//	NULL,
//	&p->dwCallbackThreadID);
//	WaitForSingleObject(p->hEvent[0], INFINITE);//等待回调函数设置成功事件
	InternetSetStatusCallback(p->hInternet, AsyncWinINet::AsyncInternetCallback);

	ResetEvent(p->hEvent[HANDLE_SUCCESS]);	//重置句柄被创建事件
	p->hFile = InternetOpenUrl(p->hInternet, p->url.c_str(), NULL, NULL, INTERNET_FLAG_DONT_CACHE|INTERNET_FLAG_RELOAD, (DWORD)p);
	FILE *fp = NULL;

	while(true) {
		if (NULL == p->hFile) {
			DWORD dwError = ::GetLastError();
			if (ERROR_IO_PENDING == dwError || ERROR_SUCCESS == dwError) {
				if (WaitExitEvent(p)) { break; }
			}
			else break;
		}

		//读取返回文件头xxx
	
		//e. 使用 HttpQueryInfo 分析头信息 HttpQueryInfo 使用非阻塞方式，所以不用等待
		DWORD dwStatusSize = sizeof(p->dwStatusCode);
		if (FALSE == HttpQueryInfo(p->hFile, HTTP_QUERY_STATUS_CODE | HTTP_QUERY_FLAG_NUMBER, &p->dwStatusCode, &dwStatusSize, NULL)) { break; }
	
		//判断状态码是不是 200
		if (HTTP_STATUS_OK != p->dwStatusCode) break;
	
		//获取返回的Content-Length
		//DWORD dwLengthSize = sizeof(p->dwContentLength); 
		//if (FALSE == HttpQueryInfo(p->hFile,
		//HTTP_QUERY_CONTENT_LENGTH | HTTP_QUERY_FLAG_NUMBER,
		//&p->dwContentLength, &dwLengthSize, NULL)) { p->dwContentLength = 0; }
	
		//f. 使用标记 IRF_ASYNC 读数据 InternetReadFileEx
		//为了向主线程报告进度，我们设置每次读数据最多 1024 字节
		fopen_s(&fp, p->saved_filename.c_str(), "w+");

		char lpvBuffer[1024];
		p->dwContentLength = 0; //Content-Length: 202749
		while(true)
		{
			INTERNET_BUFFERS i_buf = {0};
			i_buf.dwStructSize = sizeof(INTERNET_BUFFERS);
			i_buf.lpvBuffer = lpvBuffer;
			i_buf.dwBufferLength = 1024;
	
			//重置读数据事件
			ResetEvent(p->hEvent[0]);
			if (FALSE == InternetReadFileEx(p->hFile, &i_buf, IRF_ASYNC, (DWORD)p)) {
				if (ERROR_IO_PENDING == ::GetLastError())
					if (WaitExitEvent(p)) break;
				else break; 
			}

			if(fp) fwrite(i_buf.lpvBuffer, sizeof(char), i_buf.dwBufferLength, fp);
			if (i_buf.dwBufferLength == 0) break;
		}
		break;
	}

	if(fp) { fflush(fp); fclose(fp); fp = NULL; }

	if(p->hFile) {
		InternetCloseHandle(p->hFile);	//关闭 m_hFile
		while (!WaitExitEvent(p)) {		//等待句柄被关闭事件或者要求子线程退出事件
			ResetEvent(p->hEvent[HANDLE_SUCCESS]);
		}
	}

	//设置子线程退出事件，通知回调线程退出
	SetEvent(p->hEvent[2]);
  
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
	SetEvent(p->hEvent[0]);

	//等待用户终止事件或者子线程结束事件
	//子线程结束前需要设置子线程结束事件，并等待回调线程结束
	WaitForSingleObject(p->hEvent[2], INFINITE);

	return 0;
}

//----------------------------------------------------------------------------
void CALLBACK AsyncWinINet::AsyncInternetCallback(HINTERNET hInternet,
	DWORD dwContext,
	DWORD dwInternetStatus,
	LPVOID lpvStatusInformation,
	DWORD dwStatusInformationLength)
{
	thread_info* p = (thread_info*)dwContext;

	//在我们的应用中，我们只关心下面三个状态
	switch(dwInternetStatus)
	{
		//句柄被创建
		case INTERNET_STATUS_HANDLE_CREATED:
			p->hFile = (HINTERNET)(((LPINTERNET_ASYNC_RESULT)(lpvStatusInformation))->dwResult);
			break;
  
		//句柄被关闭
		case INTERNET_STATUS_HANDLE_CLOSING:
			SetEvent(p->hEvent[1]);
			break;

		//一个请求完成，比如一次句柄创建的请求，或者一次读数据的请求
		case INTERNET_STATUS_REQUEST_COMPLETE:
			if (ERROR_SUCCESS == ((LPINTERNET_ASYNC_RESULT)(lpvStatusInformation))->dwError)
			{
				//设置句柄被创建事件或者读数据成功完成事件
				SetEvent(p->hEvent[0]);
			}
			else
			{
				//如果发生错误，则设置子线程退出事件 这里也是一个陷阱，经常会忽视处理这个错误，
				SetEvent(p->hEvent[2]);
			}
			break;

		case INTERNET_STATUS_CONNECTION_CLOSED:
			SetEvent(p->hEvent[2]);
			break;
	}
}

//--------------------------------------------------------------------
BOOL AsyncWinINet::WaitExitEvent(thread_info *p)
{
	DWORD dwRet = WaitForMultipleObjects(3, p->hEvent, FALSE, INFINITE);

	switch (dwRet)
	{
		case WAIT_OBJECT_0:		//句柄被创建事件或者读数据请求成功完成事件
		case WAIT_OBJECT_0+1:	//句柄被关闭事件
		case WAIT_OBJECT_0+2:	//用户要求终止子线程事件或者发生错误事件
		break;
	}
	return WAIT_OBJECT_0 != dwRet;
}