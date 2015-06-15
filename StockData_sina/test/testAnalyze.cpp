#include "testAnalyze.h"
#include <time.h>
#include <vector>
using namespace std;
#include "testMonitor.h"
#include "..//AnalyzeData//analyzeDailyData.h"

void testAnalyzeAverage(bool IsNeedTest) {
	if (!IsNeedTest) return;

	printf_s("\n######################################\n");
	printf_s("# Now begin to test Analyze Average! #\n");
	printf_s("######################################\n\n");
	printf_s("enter return to start!\n");
	getchar();
	testTime tst_Tm("testAnalyzeAverage");

	string testStockID = "000001";
	printf_s("# begin get history data of %s\n", testStockID.c_str());
	// 得到历史数据并存至文件
	HistoryData historyData;
	historyData.StockDailyData(testStockID, NO_FUQUAN);

	printf_s("# begin to extraction data\n");
	analyzeDailyData alzDailyData(testStockID);
	alzDailyData.ExtractionData(NO_FUQUAN);
	vector<sinaDailyData> *pData = alzDailyData.getExtractData();
	vector<averageData> ans[2];

	printf_s("# begin to get average data\n");
	int Days[2] = {3,5};
	float wt[2][5] = {0.4f,0.3f,0.3f,0.0f,0.0f,0.5f,0.1f,0.1f,0.1f,0.1f};
	float *pwt[2] = {nullptr};
	pwt[0] = wt[0];
	pwt[1] = wt[1];
	alzDailyData.averageAnalyze(Days, pwt, ans, 2);

	printf_s("######################################\n");
	printf_s("# Now test Analyze Average finished! #\n");
	printf_s("######################################\n\n");
}

void testAnalyzeTurtle(bool IsNeedTest) {
	if (!IsNeedTest) return;

	printf_s("\n######################################\n");
	printf_s("# Now begin to test Turtle Average!  #\n");
	printf_s("######################################\n\n");
	printf_s("enter return to start!\n");
	getchar();
	testTime tst_Tm("testAnalyzeTurtle");

	// 0.1 得到历史数据并存至文件
	string testStockID = "000333";
	INFO("# begin get history data of %s\n", testStockID.c_str());
	HistoryData historyData;
	historyData.StockDailyData(testStockID, FUQUAN);
	// 0.2 提取文件中数据
	INFO("# begin to extraction data\n");
	analyzeDailyData alzDailyData(testStockID);
	alzDailyData.ExtractionData(FUQUAN);
	vector<sinaDailyData> *pData = alzDailyData.getExtractData(); // raw data

// 	vector<sinaDailyData> rawData = *pData;
// 	vector<sinaDailyData>::reverse_iterator r_it_begin = rawData.rbegin();
// 	vector<sinaDailyData>::reverse_iterator r_it_end = rawData.rend();
// 	for (vector<sinaDailyData>::reverse_iterator r_it = r_it_begin; r_it != r_it_end; ++r_it) {
// 		printf_s("(raw date:%4d-%2d-%2d) open:%.3f, top:%.3f, close:%.3f, buttom:%.3f\n", r_it->date.year, r_it->date.month, r_it->date.day, 
// 			r_it->open, r_it->top, r_it->close, r_it->buttom);
// 	}

// 	getchar();
	// 1. test 建仓和平仓的TopButtom，平滑波动N
	// 1.1 param
	int numN = 20;
	int CreateTopButtom[2] = {20, 55};
	int LeaveTopButtom[2] = {10, 20};

	float TotalMoney = 500000.0f;
	HoldPosition Chip;
	Chip.setRemain(TotalMoney);
	INFO("alzDailyData.turtleAnalyze\n");
	vector<TradingPoint> *ans = alzDailyData.turtleAnalyze(numN, CreateTopButtom, LeaveTopButtom, 2, TotalMoney);
	getchar();
	// 2. test 打印交易过程及盈亏
	vector<TradingPoint>::iterator it_begin = (*ans).begin();
	vector<TradingPoint>::iterator it_end = (*ans).end();
	for (vector<TradingPoint>::iterator it = it_begin; it != it_end; ++it) {
		it->ShowThisTradeInfo();
	}
	Chip.ShowThisCmp(&alzDailyData.getPosition(TURTLE_ANALYZE));
	getchar();
	printf_s("######################################\n");
	printf_s("# Now test Analyze Turtle finished!  #\n");
	printf_s("######################################\n\n");
}

void testOnTimeData(bool IsNeedTest) {
	if (!IsNeedTest) return;

	HistoryData historyData;
// 	historyData.URL2Data()
}