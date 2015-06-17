#include <iostream>
#include <string>
#include <map>
#include <utility>
#include <ctime>
using namespace std;
#include <direct.h>
#include <io.h>

#include "Data_sina//DataStruct_sina.h"
#include "Data_sina//getData_sina.h"
#include "Data_sina//DataHistory.h"
#include "Common//stock2fpTable.h"
#include "Common//HttpUrl.h"
#include "Common//GlobalParam.h"
#include "Common//TraceMicro.h"
#include "Common//Write2Buffer.h"
#include "Common//stockTime.h"
#include "stockHandler.h"

#include "test//testAnalyze.h"

int main()
{
	int a = 0, b = 1, c = 2;
	printf_s("%d\n", a +=b);
	printf_s("%d\n", a);

	printf_s("######Now @ master branch!!######\n");
	printf_s("###### start: test Write2Buffer ######\n");

	getTraceConfigFromFile();
	for (int i = 0; i < NUM_TRACES; i++) {
		STATIC_TRACE(i, "(on) static trace(%d) %s ok!!\n", i, "test");
		DYNAMIC_TRACE(i, "(on) dynamic trace(%d) %s ok!!\n", i, "test");
	}

	testAnalyzeAverage(false);
	testAnalyzeTurtle(true);
	getchar();

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
	StockHandler sh;
// 	sh.AddMyStock("000333", true);
// 	sh.AddMyStock("600820", true);
	sh.AddMyStockFromFile();
	sh.UpdateAllStock();
	getchar();


//////////////////////////////////////////////////////////////////////
	string history = "http://money.finance.sina.com.cn/corp/go.php/vMS_MarketHistory/stockid/000333.phtml";
	string histor1 = "http://vip.stock.finance.sina.com.cn/corp/go.php/vMS_MarketHistory/stockid/600820.phtml?year=2015&judu=1";
// 	AsyncWinINet asyn_test("http://hq.sinajs.cn/list=sh600036,sz000333", "test.stk");
	AsyncWinINet asyn_test(histor1.c_str(), "asyn.stk");
	stockTable tb1;

// 	urlopen_sina_TB_ex(history.c_str(), tb1);

// 	AsyncWinINet asyn_test(his2, "asyn1.stk");
	AsyncWinINet::AsyncThread(asyn_test.getThreadInfo());
	getchar();
	return 0;
//////////////////////////////////////////////////////////////////////
	HttpUrlGetSyn Url_sina("stock_sina");
	BYTE* tmp = Url_sina.GetBuf();
	tmp[0] = '4';
	Url_sina.PrintBuf();


	clock_t start=clock();
	stockTable tb;

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
	INFO("stock2fpTable Size: %d << %s add\n", tb.getFileTableSize(), symbol.c_str());

	// -- 2 ----------------------------
	symbol = "sz000333";
	url_req += ",";
	url_req += symbol;
	tb.addStock2File(symbol);
	tb.getFile(symbol).open(symbol);
	INFO("stock2fpTable Size: %d << %s add\n", tb.getFileTableSize(), symbol.c_str());

	// -- 3 ----------------------------
	symbol = "sh600820";
	url_req += ",";
	url_req += symbol;
	tb.addStock2File(symbol);
	tb.getFile(symbol).open(symbol);
	INFO("stock2fpTable Size: %d << %s add\n", tb.getFileTableSize(), symbol.c_str());

	// -- 4 ----------------------------
	symbol = "sz000623";
	url_req += ",";
	url_req += symbol;
	tb.addStock2File(symbol);
	tb.getFile(symbol).open(symbol);
	INFO("stock2fpTable Size: %d << %s add\n", tb.getFileTableSize(), symbol.c_str());

	//TB means using stock2fpTable
	//urlopen_sina_TB(url_req.c_str(), tb);
	urlopen_sina_TB_ex(url_req.c_str(), tb);

	// -- close ----------------------------
	tb.closeAllFiles();

	int diff_time=(clock()-start)/CLOCKS_PER_SEC;
	INFO("total time used:%d s\n", diff_time);

	getchar();
	return 0;
}