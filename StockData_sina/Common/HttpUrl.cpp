#include <iostream>
using namespace std;

#include "HttpUrl.h"
#include "TraceMicro.h"

HttpUrlGetSyn::HttpUrlGetSyn(const char* HttpName, DWORD dwAccessType) {
	hSession = InternetOpen(HttpName, dwAccessType, NULL, NULL, 0);
	bufSize = MAX_RECV_BUF_SIZE;
	buf = new BYTE[bufSize];
	memset(buf, 0, bufSize*sizeof(BYTE));
	STATIC_TRACE(URL_TRACE, "URL: hSession initialed.\n");
}
HttpUrlGetSyn::~HttpUrlGetSyn() {
	InternetCloseHandle(hSession);	hSession = NULL;
	delete []buf;
}
HINTERNET HttpUrlGetSyn::OpenUrl(const char* url) {
	hHttp = InternetOpenUrl(hSession, url, NULL, 0, INTERNET_FLAG_DONT_CACHE, 0);
	if (NULL != hHttp) STATIC_TRACE(URL_TRACE, "URL: successful opened:\n%s\n", url);
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
	STATIC_TRACE(URL_TRACE, "URL: hSession initialed.\n");
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
	
//	a. ʹ�ñ�� INTERNET_FLAG_ASYNC ��ʼ�� InternetOpen
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
//	WaitForSingleObject(p->hEvent[0], INFINITE);//�ȴ��ص��������óɹ��¼�
	InternetSetStatusCallback(p->hInternet, AsyncWinINet::AsyncInternetCallback);

	FILE *fp = NULL;
	fopen_s(&fp, p->saved_filename.c_str(), "r+");


	ResetEvent(p->hEvent[HANDLE_SUCCESS]);	//���þ���������¼�
	p->hFile = InternetOpenUrl(p->hInternet, p->url.c_str(), NULL, NULL, INTERNET_FLAG_DONT_CACHE|INTERNET_FLAG_RELOAD, (DWORD)p);
	STATIC_TRACE(URL_TRACE, "ASYN: URL opened!!\n");
	while(true) {
		if (NULL == p->hFile) {
			DWORD dwError = ::GetLastError();
			if (ERROR_IO_PENDING == dwError || ERROR_SUCCESS == dwError) {
				if (WaitExitEvent(p)) { break; }
			}
			else break;
		}

		//��ȡ�����ļ�ͷxxx
	
		//e. ʹ�� HttpQueryInfo ����ͷ��Ϣ HttpQueryInfo ʹ�÷�������ʽ�����Բ��õȴ�
		DWORD dwStatusSize = sizeof(p->dwStatusCode);
		if (false == HttpQueryInfo(p->hFile, HTTP_QUERY_STATUS_CODE | HTTP_QUERY_FLAG_NUMBER, &p->dwStatusCode, &dwStatusSize, NULL)) { break; }
		STATIC_TRACE(URL_TRACE, "ASYN: HttpQueryInfo!!\n");
		//�ж�״̬���ǲ��� 200
		if (HTTP_STATUS_OK != p->dwStatusCode) {
			STATIC_TRACE(URL_TRACE, "ASYN: HTTP_STATUS_OK is failed\n");
			break;
		}
	
		//��ȡ���ص�Content-Length
		//DWORD dwLengthSize = sizeof(p->dwContentLength); 
		//if (FALSE == HttpQueryInfo(p->hFile,
		//HTTP_QUERY_CONTENT_LENGTH | HTTP_QUERY_FLAG_NUMBER,
		//&p->dwContentLength, &dwLengthSize, NULL)) { p->dwContentLength = 0; }
	
		//f. ʹ�ñ�� IRF_ASYNC ������ InternetReadFileEx
		//Ϊ�������̱߳�����ȣ���������ÿ�ζ�������� 1024 �ֽ�

		char lpvBuffer[1024];
		p->dwContentLength = 0; //Content-Length: 202749
		while(true)
		{
			INTERNET_BUFFERS i_buf = {0};
			i_buf.dwStructSize = sizeof(INTERNET_BUFFERS);
			i_buf.lpvBuffer = lpvBuffer;
			i_buf.dwBufferLength = 1024;
	
			//���ö������¼�
			ResetEvent(p->hEvent[HANDLE_SUCCESS]);
			STATIC_TRACE(URL_TRACE, "ASYN: InternetReadFileEx before asyn!!\n");
			if (false == InternetReadFileEx(p->hFile, &i_buf, IRF_ASYNC, (DWORD)p)) {
				STATIC_TRACE(URL_TRACE, "ASYN: InternetReadFileEx == FALSE asyn!!\n");
				if (ERROR_IO_PENDING == ::GetLastError())
					if (WaitExitEvent(p)) break;
				else break; 
			}
			STATIC_TRACE(URL_TRACE, "ASYN: InternetReadFileEx ok, begin to write!!length=%d\n", i_buf.dwBufferLength);
			if(fp) fwrite(i_buf.lpvBuffer, sizeof(char), i_buf.dwBufferLength, fp);
			if (i_buf.dwBufferLength == 0) break;
		}
		break;
	}

	STATIC_TRACE(URL_TRACE, "ASYN: begin to CLOSE files!!\n");
	if(fp) { fflush(fp); fclose(fp); fp = NULL; }
	if(p->hFile) {
		InternetCloseHandle(p->hFile);	//�ر� m_hFile
		while (!WaitExitEvent(p)) {		//�ȴ�������ر��¼�����Ҫ�����߳��˳��¼�
			ResetEvent(p->hEvent[HANDLE_SUCCESS]);
		}
	}

	//�������߳��˳��¼���֪ͨ�ص��߳��˳�
	SetEvent(p->hEvent[THREAD_EXIT]);
  
	//�ȴ��ص��̰߳�ȫ�˳�
	//WaitForSingleObject(p->hCallbackThread, INFINITE);
	//CloseHandle(p->hCallbackThread);

	//ע���ص�����
	InternetSetStatusCallback(p->hInternet, NULL);
	InternetCloseHandle(p->hInternet);

	return TRUE;
}

//------------------------------------------------------------------------------------
DWORD WINAPI AsyncWinINet::AsyncCallbackThread(LPVOID lpParameter)
{
	thread_info *p = (thread_info*)lpParameter;
	InternetSetStatusCallback(p->hInternet, AsyncWinINet::AsyncInternetCallback);

	//֪ͨ���̻߳ص��������óɹ������߳̿��Լ�������
	SetEvent(p->hEvent[HANDLE_SUCCESS]);

	//�ȴ��û���ֹ�¼��������߳̽����¼�
	//���߳̽���ǰ��Ҫ�������߳̽����¼������ȴ��ص��߳̽���
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
	thread_info* p = (thread_info*)dwContext;

	//�����ǵ�Ӧ���У�����ֻ������������״̬
	switch(dwInternetStatus)
	{
		//���������
		case INTERNET_STATUS_HANDLE_CREATED:
			p->hFile = (HINTERNET)(((LPINTERNET_ASYNC_RESULT)(lpvStatusInformation))->dwResult);
			STATIC_TRACE(CALLBACE_TRACE, "callback: HINTERNET created!! no event set\n");
			break;
  
		//������ر�
		case INTERNET_STATUS_HANDLE_CLOSING:
			STATIC_TRACE(CALLBACE_TRACE, "callback: HINTERNET closed!! event HANDLE_CLOSE set\n");
			SetEvent(p->hEvent[HANDLE_CLOSE]);
			break;

		//һ��������ɣ�����һ�ξ�����������󣬻���һ�ζ����ݵ�����
		case INTERNET_STATUS_REQUEST_COMPLETE:
			if (ERROR_SUCCESS == ((LPINTERNET_ASYNC_RESULT)(lpvStatusInformation))->dwError) {
				STATIC_TRACE(CALLBACE_TRACE, "callback: ���������������ݳɹ�, event HANDLE_SUCCESS set\n");
				SetEvent(p->hEvent[HANDLE_SUCCESS]);
			}
			else {
				STATIC_TRACE(CALLBACE_TRACE, "callback: �緢���������������߳��˳�, event THREAD_EXIT set\n");
				SetEvent(p->hEvent[THREAD_EXIT]);
			}
			break;

		case INTERNET_STATUS_CONNECTION_CLOSED:
			STATIC_TRACE(CALLBACE_TRACE, "callback: HINTERNET connection closed!! event THREAD_EXIT set\n");
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
		case WAIT_OBJECT_0:		//��������������������ɹ����
			STATIC_TRACE(WAIT_TRACE, "wait: HANDLE CREATE/SUCCESS OPERATION!!\n");
			break;
		case WAIT_OBJECT_0+1:	//������ر�
			STATIC_TRACE(WAIT_TRACE, "wait: HANDLE CLOSE!!\n");
			break;
		case WAIT_OBJECT_0+2:	//��ֹ���̻߳�������
			STATIC_TRACE(WAIT_TRACE, "wait: THREAD EXIT!!\n");
			break;
		default:
			STATIC_TRACE(WAIT_TRACE, "wait: (ERROR) wait signal is unexpected!!\n");
	}
	return WAIT_OBJECT_0 != dwRet;
}