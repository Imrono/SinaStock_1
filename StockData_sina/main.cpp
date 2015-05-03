#include <iostream>
#include <string>
#include <map>
#include <utility>
#include <ctime>
using namespace std;

#include "Data_sina//DataStruct_sina.h"
#include "Data_sina//getData_sina.h"
#include "Data_sina//DataHistory.h"
#include "Common//stock2fpTable.h"
#include "Common//HttpUrl.h"
#include "Common//GlobalParam.h"
#include "Common//TraceMicro.h"
#include "Common//Write2Buffer.h"

int main()
{
	printf_s("######Now @ master branch!!######\n");
	printf_s("###### start: test Write2Buffer ######\n");

	Write2Buffer w2b(true, 1024);
	w2b.AddSearchString("<!--历史交易begin-->", "<!--历史交易end-->", 1);
	DataInSeason dataSeason;

	int len = 0;
	const int constLength = 1024;
	char* buf;
	FILE* fp = fopen("600820_2015-1.stk", "r+");
	int sizeRead = 0;
	do {
		buf = w2b.getBuffer4Write(len);
		len = constLength > len ? len : constLength;
		sizeRead = fread(buf, 1, len, fp);
		w2b.updateAfterWrite(len);
		if (w2b.getData(1)) {
			printf_s("%s", w2b.getData(1)->ResultStr.c_str());

			FILE* fp1 = fopen("aaa.stk", "w");
			fwrite(w2b.getData(1)->ResultStr.c_str(), 1, strlen(w2b.getData(1)->ResultStr.c_str()), fp1);
			fclose(fp1);
		}
	} while (sizeRead);
	fclose(fp);

	dataSeason.DataAnalyze(w2b.getData(1)->ResultStr.c_str());
	vector<DataOfDay> *dataDaily = dataSeason.getDateDaily();
	for (vector<DataOfDay>::iterator it = (*dataDaily).begin(); it != (*dataDaily).end(); ++it) {
		char tmp[256] = {0};
		sprintf(tmp, "%d-%d-%d : %.3f,%.3f,%.3f,%.3f,%d,%d\n", it->date.year, it->date.month, it->date.day
			, it->open, it->top, it->close, it->buttom, it->exchangeStock, it->exchangeMoney);

		FILE* fp2 = fopen("bbb.stk", "a+");
		fwrite(tmp, 1, strlen(tmp), fp2);
		fclose(fp2);
	}

	HistoryURL hstURL;
	const char *tmpURL = hstURL.PrepareURL(2015,2,"000333",NO_FUQUAN);
	printf_s("\n%s\n", tmpURL);

	printf_s("###### end: test Write2Buffer ######\n");
	getchar();



	getTraceConfigFromFile();
	for (int i = 0; i < NUM_TRACES; i++) {
		STATIC_TRACE(i, "static trace(%d) %s ok!!\n", i, "test");
		DYNAMIC_TRACE(i, "dynamic trace(%d) %s ok!!\n", i, "test");
	}

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