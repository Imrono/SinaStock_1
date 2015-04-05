#include <iostream>
#include <string>
#include <map>
#include <utility>
#include <ctime>
using namespace std;

#include "Data_sina//DataStruct_sina.h"
#include "Data_sina//getData_sina.h"
#include "Common//stock2fpTable.h"
#include "Common//HttpUrl.h"
#include "Common//GlobalParam.h"

int main()
{
	printf_s("######Now @ UrlAsynchronous branch######\n");
//////////////////////////////////////////////////////////////////////
	AsyncWinINet asyn_test("http://hq.sinajs.cn/list=sh600036", "test.stk");
	AsyncWinINet::AsyncThread(asyn_test.getThreadInfo());

//////////////////////////////////////////////////////////////////////
	HttpUrlGetSyn Url_sina("stock_sina");
	BYTE* tmp = Url_sina.GetBuf();
	tmp[0] = '4';
	Url_sina.PrintBuf();


	clock_t start=clock();
	stock2fpTable tb;

	char fileName[100] = "test.txt";
	char url_tmp[150] = "http://hq.sinajs.cn/list=sh600036";
// 	urlopen_sina(url_tmp, fileName, "a+");

	string url_req = "http://hq.sinajs.cn/list=";
	string symbol;
	// -- 1 ----------------------------
	symbol = "sh600036";
	url_req += symbol;
	tb.addStock2File(symbol);
	tb.getFile(symbol).open(symbol);
	printf_s("#######%d\n", tb.getTableSize());

	// -- 2 ----------------------------
	symbol = "sz000333";
	url_req += ",";
	url_req += symbol;
	tb.addStock2File(symbol);
	tb.getFile(symbol).open(symbol);
	printf_s("#######%d\n", tb.getTableSize());

	// -- 3 ----------------------------
	symbol = "sh600820";
	url_req += ",";
	url_req += symbol;
	tb.addStock2File(symbol);
	tb.getFile(symbol).open(symbol);
	printf_s("#######%d\n", tb.getTableSize());

	// -- 4 ----------------------------
	symbol = "sz000623";
	url_req += ",";
	url_req += symbol;
	tb.addStock2File(symbol);
	tb.getFile(symbol).open(symbol);
	printf_s("#######%d\n", tb.getTableSize());

	//urlopen_sina_TB(url_req.c_str(), tb);
// 	urlopen_sina_TB_ex(url_req.c_str(), tb);

	// -- close ----------------------------
	tb.closeAllFiles();

	int diff_time=(clock()-start)/CLOCKS_PER_SEC;
	printf_s("total time used:%d s\n", diff_time);

	getchar();
	return 0;
}