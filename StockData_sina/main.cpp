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
#include "Common//TraceMicro.h"

int main()
{
	printf_s("######Now @ master branch!!######\n");
	getTraceConfigFromFile();
	for (int i = 0; i < NUM_TRACES; i++) {
		STATIC_TRACE(i, "static trace(%d) %s ok!!\n", i, "test");
		DYNAMIC_TRACE(i, "dynamic trace(%d) %s ok!!\n", i, "test");
	}

//////////////////////////////////////////////////////////////////////
	string history = "http://money.finance.sina.com.cn/corp/go.php/vMS_MarketHistory/stockid/000333.phtml?year=2014&jidu=3";
	AsyncWinINet asyn_test("http://hq.sinajs.cn/list=sh600036,sz000333", "test.stk");
// 	AsyncWinINet asyn_test(history.c_str(), "asyn.stk");
	AsyncWinINet::AsyncThread(asyn_test.getThreadInfo());
	getchar();
	return 0;
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
	INFO("stock2fpTable Size: %d << %s add\n", tb.getTableSize(), symbol.c_str());

	// -- 2 ----------------------------
	symbol = "sz000333";
	url_req += ",";
	url_req += symbol;
	tb.addStock2File(symbol);
	tb.getFile(symbol).open(symbol);
	INFO("stock2fpTable Size: %d << %s add\n", tb.getTableSize(), symbol.c_str());

	// -- 3 ----------------------------
	symbol = "sh600820";
	url_req += ",";
	url_req += symbol;
	tb.addStock2File(symbol);
	tb.getFile(symbol).open(symbol);
	INFO("stock2fpTable Size: %d << %s add\n", tb.getTableSize(), symbol.c_str());

	// -- 4 ----------------------------
	symbol = "sz000623";
	url_req += ",";
	url_req += symbol;
	tb.addStock2File(symbol);
	tb.getFile(symbol).open(symbol);
	INFO("stock2fpTable Size: %d << %s add\n", tb.getTableSize(), symbol.c_str());

	//urlopen_sina_TB(url_req.c_str(), tb);
	urlopen_sina_TB_ex(url_req.c_str(), tb);

	// -- close ----------------------------
	tb.closeAllFiles();

	int diff_time=(clock()-start)/CLOCKS_PER_SEC;
	INFO("total time used:%d s\n", diff_time);

	getchar();
	return 0;
}