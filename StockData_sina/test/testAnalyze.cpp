#include "testAnalyze.h"
#include <time.h>
#include <vector>
using namespace std;
// #include "testMonitor.h"

void testAnalyzeAverage() {
	printf_s("\n######################################\n");
	printf_s("# Now begin to test Analyze Average! #\n");
	printf_s("######################################\n\n");
	printf_s("enter return to start!\n");
	getchar();

// 	testTime tst_Tm("testAnalyzeAverage");

	averageData it;
	memset(&it, 0, sizeof(averageData));
	char *tmpBuffer = "2015- 5-19 : 33.330,35.000,34.830,33.300,35521084.000,1217427328.000,1.000";
	sscanf_s(tmpBuffer, "%d- %d-%d : %*f,%*f,%f,%*f,%f,%f,%f\n", &it.date.year, &it.date.month, &it.date.day
			, &it.close, &it.exchangeStock, &it.exchangeMoney, &it.factor);

	string testStockID = "000651";
	printf_s("# begin get history data of %s\n", testStockID.c_str());
	HistoryData historyData;
	historyData.StockDailyData(testStockID, NO_FUQUAN);

	printf_s("# begin to extraction data\n");
	analyzeDailyData alzDailyData;
	alzDailyData.setStockID(testStockID);
	alzDailyData.ExtractionData(NO_FUQUAN);
	vector<averageData> *pData = alzDailyData.getAvgData();
	vector<averageData> ans[2];

	printf_s("# begin to get average data\n");
	int Days[2] = {3,5};
	float wt[2][5] = {0.4f,0.3f,0.3f,0.0f,0.0f,0.5f,0.1f,0.1f,0.1f,0.1f};
	float *pwt[2] = {nullptr};
	pwt[0] = wt[0];
	pwt[1] = wt[1];
	alzDailyData.GetnDayAverage(Days, pwt, ans, 2);

	printf_s("######################################\n");
	printf_s("# Now test Analyze Average finished! #\n");
	printf_s("######################################\n\n");
}