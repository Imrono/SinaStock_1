#include "HttpUrl.h"


HttpUrlGet::HttpUrlGet(const char* HttpName, DWORD dwAccessType) {
	hSession = InternetOpen(HttpName, dwAccessType, NULL, NULL, 0);
	bufSize = MAX_RECV_BUF_SIZE;
	buf = new BYTE[bufSize];
	memset(buf, 0, bufSize*sizeof(BYTE));
	printf_s("hSession initialed.\n");
}
HttpUrlGet::~HttpUrlGet() {
	InternetCloseHandle(hSession);	hSession = NULL;
	delete []buf;
}
HINTERNET HttpUrlGet::OpenUrl(const char* url) {
	hHttp = InternetOpenUrl(hSession, url, NULL, 0, INTERNET_FLAG_DONT_CACHE, 0);
	if (NULL != hHttp) printf_s("successful opened:\n%s\n", url);
	return hHttp;
}
void HttpUrlGet::CloseUrl() {
	InternetCloseHandle(hHttp);	hHttp = NULL;
}

int HttpUrlGet::ReadUrlOne(DWORD &Number) {
	int ret = InternetReadFile(hHttp, buf, bufSize-1, &Number);
	buf[Number] = 0;
	return ret;
}
void HttpUrlGet::ReadUrlAll(string &data_recv) {

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
