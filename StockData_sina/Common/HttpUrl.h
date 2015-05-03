#ifndef HTTP_URL_GET_H
#define HTTP_URL_GET_H
#include <vector>
#include <cstring>
#include <string>
#include <windows.h>
#include <wininet.h>
using namespace std;

#include "GlobalParam.h"
#define MAX_RECV_BUF_SIZE 1024


enum asyn_event
{
	HANDLE_SUCCESS = 0,
	HANDLE_CLOSE = 1,
	THREAD_EXIT = 2
};


class HttpUrlGetSyn
{
public:
	HttpUrlGetSyn(const char* HttpName = NULL, DWORD dwAccessType = INTERNET_OPEN_TYPE_PRECONFIG);
	~HttpUrlGetSyn();

	HINTERNET OpenUrl(const char* url);
	void CloseUrl();

	int ReadUrlOne(DWORD &Number);
	void ReadUrlAll(string &data_recv);

	BYTE* GetBuf()	{ return _buf; }
	void PrintBuf()	{ printf_s("buf in HttpUrlGet:\n%s\n", _buf); }

private:
	HINTERNET _hSession;
	HINTERNET _hHttp;
	BYTE* _buf;
	int _bufSize;
};

class HttpUrlGetAsyn
{
public:
	HttpUrlGetAsyn(const char* HttpName = NULL, DWORD dwAccessType = INTERNET_OPEN_TYPE_PRECONFIG);
	~HttpUrlGetAsyn();

	HINTERNET OpenUrl(const char* url);
	void CloseUrl();

	int ReadUrlOne(DWORD &Number);
	void ReadUrlAll(string &data_recv);

	BYTE* GetBuf()	{ return buf; }
	void PrintBuf()	{ printf_s("buf in HttpUrlGet:\n%s\n", buf);
	}

private:
	HINTERNET hSession;
	HINTERNET hHttp;
	BYTE* buf;
	int bufSize;
};

/////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////
class AsyncWinINet
{
public:
// 	typedef void (*notify_fp)(const StringMap&);
	AsyncWinINet(const string& url, const string& _saved_filename);

	class thread_info
	{
	public:
		thread_info(const string& url, const string& _saved_filename):saved_filename(_saved_filename)
		{
			this->url = url;
			for(int i = 0; i < 3; ++i)
			{
				this->hEvent[i] = CreateEvent(NULL,TRUE,FALSE,NULL);
			}
		}

		HANDLE hThread;
		DWORD dwThreadID;
		HANDLE hCallbackThread;
		DWORD dwCallbackThreadID;
		HANDLE hEvent[3];
		LPVOID hInternet;
		LPVOID hFile;
		DWORD dwStatusCode;
		DWORD dwContentLength;

		string url;

		const string saved_filename;
	};

	thread_info* getThreadInfo() { return &thd; }
	static DWORD WINAPI AsyncThread(LPVOID lpParameter);

protected:
	static BOOL WaitExitEvent(thread_info *p);
	static DWORD WINAPI AsyncCallbackThread(LPVOID lpParameter);
	static VOID CALLBACK AsyncInternetCallback(HINTERNET hInternet,
	DWORD dwContext,
	DWORD dwInternetStatus,
	LPVOID lpvStatusInformation,
	DWORD dwStatusInformationLength);
	thread_info thd;

};






#endif