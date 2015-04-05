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

	//a. ʹ�ñ�� INTERNET_FLAG_ASYNC ��ʼ�� InternetOpen
	string user_agent("asyn test!!");

	p->hInternet = InternetOpen(user_agent.c_str(), INTERNET_OPEN_TYPE_PRECONFIG, NULL, NULL, INTERNET_FLAG_ASYNC);

//	ResetEvent(p->hEvent[0]);
//	p->hCallbackThread = CreateThread(NULL,
//	0,
//	AsyncWinINet::AsyncCallbackThread,
//	p,
//	NULL,
//	&p->dwCallbackThreadID);
//	WaitForSingleObject(p->hEvent[0], INFINITE);//�ȴ��ص��������óɹ��¼�
	InternetSetStatusCallback(p->hInternet, AsyncWinINet::AsyncInternetCallback);

	ResetEvent(p->hEvent[HANDLE_SUCCESS]);	//���þ���������¼�
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

		//��ȡ�����ļ�ͷxxx
	
		//e. ʹ�� HttpQueryInfo ����ͷ��Ϣ HttpQueryInfo ʹ�÷�������ʽ�����Բ��õȴ�
		DWORD dwStatusSize = sizeof(p->dwStatusCode);
		if (FALSE == HttpQueryInfo(p->hFile, HTTP_QUERY_STATUS_CODE | HTTP_QUERY_FLAG_NUMBER, &p->dwStatusCode, &dwStatusSize, NULL)) { break; }
	
		//�ж�״̬���ǲ��� 200
		if (HTTP_STATUS_OK != p->dwStatusCode) break;
	
		//��ȡ���ص�Content-Length
		//DWORD dwLengthSize = sizeof(p->dwContentLength); 
		//if (FALSE == HttpQueryInfo(p->hFile,
		//HTTP_QUERY_CONTENT_LENGTH | HTTP_QUERY_FLAG_NUMBER,
		//&p->dwContentLength, &dwLengthSize, NULL)) { p->dwContentLength = 0; }
	
		//f. ʹ�ñ�� IRF_ASYNC ������ InternetReadFileEx
		//Ϊ�������̱߳�����ȣ���������ÿ�ζ�������� 1024 �ֽ�
		fopen_s(&fp, p->saved_filename.c_str(), "w+");

		char lpvBuffer[1024];
		p->dwContentLength = 0; //Content-Length: 202749
		while(true)
		{
			INTERNET_BUFFERS i_buf = {0};
			i_buf.dwStructSize = sizeof(INTERNET_BUFFERS);
			i_buf.lpvBuffer = lpvBuffer;
			i_buf.dwBufferLength = 1024;
	
			//���ö������¼�
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
		InternetCloseHandle(p->hFile);	//�ر� m_hFile
		while (!WaitExitEvent(p)) {		//�ȴ�������ر��¼�����Ҫ�����߳��˳��¼�
			ResetEvent(p->hEvent[HANDLE_SUCCESS]);
		}
	}

	//�������߳��˳��¼���֪ͨ�ص��߳��˳�
	SetEvent(p->hEvent[2]);
  
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
	SetEvent(p->hEvent[0]);

	//�ȴ��û���ֹ�¼��������߳̽����¼�
	//���߳̽���ǰ��Ҫ�������߳̽����¼������ȴ��ص��߳̽���
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

	//�����ǵ�Ӧ���У�����ֻ������������״̬
	switch(dwInternetStatus)
	{
		//���������
		case INTERNET_STATUS_HANDLE_CREATED:
			p->hFile = (HINTERNET)(((LPINTERNET_ASYNC_RESULT)(lpvStatusInformation))->dwResult);
			break;
  
		//������ر�
		case INTERNET_STATUS_HANDLE_CLOSING:
			SetEvent(p->hEvent[1]);
			break;

		//һ��������ɣ�����һ�ξ�����������󣬻���һ�ζ����ݵ�����
		case INTERNET_STATUS_REQUEST_COMPLETE:
			if (ERROR_SUCCESS == ((LPINTERNET_ASYNC_RESULT)(lpvStatusInformation))->dwError)
			{
				//���þ���������¼����߶����ݳɹ�����¼�
				SetEvent(p->hEvent[0]);
			}
			else
			{
				//��������������������߳��˳��¼� ����Ҳ��һ�����壬��������Ӵ����������
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
		case WAIT_OBJECT_0:		//����������¼����߶���������ɹ�����¼�
		case WAIT_OBJECT_0+1:	//������ر��¼�
		case WAIT_OBJECT_0+2:	//�û�Ҫ����ֹ���߳��¼����߷��������¼�
		break;
	}
	return WAIT_OBJECT_0 != dwRet;
}