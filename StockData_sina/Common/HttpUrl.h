#ifndef HTTP_URL_GET_H
#define HTTP_URL_GET_H
#include <vector>
#include <cstring>
#include <windows.h>
#include <wininet.h>
using namespace std;

#include "GlobalParam.h"
#define MAX_RECV_BUF_SIZE 128

class HttpUrlGetSyn
{
public:
	HttpUrlGetSyn(const char* HttpName = NULL, DWORD dwAccessType = INTERNET_OPEN_TYPE_PRECONFIG);
	~HttpUrlGetSyn();

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


#endif