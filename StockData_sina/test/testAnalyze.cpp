#include "testAnalyze.h"
#include <time.h>
#include <vector>
using namespace std;
#include "testMonitor.h"

void testAnalyzeAverage() {
	printf_s("\n######################################\n");
	printf_s("# Now begin to test Analyze Average! #\n");
	printf_s("######################################\n\n");
	printf_s("enter return to start!\n");
	getchar();

	testTime tst_Tm("testAnalyzeAverage");

	string testStockID = "000651";
	printf_s("# begin get history data of %s\n", testStockID.c_str());
	HistoryData historyData;
	historyData.StockDailyData(testStockID, NO_FUQUAN);

	printf_s("# begin to extraction data\n");
	analyzeDailyData alzDailyData;
	alzDailyData.setStockID(testStockID);
	alzDailyData.ExtractionData(NO_FUQUAN);
	vector<sinaDailyData> *pData = alzDailyData.getExtractData();
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