#include "testAnalyze.h"
#include <time.h>
#include <vector>
using namespace std;

void testAnalyzeAverage() {
	printf_s("\n######################################\n");
	printf_s("# Now begin to test Analyze Average! #\n");
	printf_s("######################################\n\n");
	clock_t clock_start = clock();

	averageData it;
	memset(&it, 0, sizeof(averageData));
	char *tmpBuffer = "2015- 5-19 : 33.330,35.000,34.830,33.300,35521084.000,1217427328.000,1.000";
	sscanf_s(tmpBuffer, "%d- %d-%d : %*f,%*f,%f,%*f,%f,%f,%f\n", &it.date.year, &it.date.month, &it.date.day
			, &it.close, &it.exchangeStock, &it.exchangeMoney, &it.factor);
	getchar();

	printf_s("# begin get history data of 000333\n");
	HistoryData historyData;
	historyData.StockDailyData("000333", NO_FUQUAN);

	printf_s("# begin to extraction data\n");
	analyzeDailyData alzDailyData;
	alzDailyData.setStockID("000333");
	alzDailyData.ExtractionData(NO_FUQUAN);
	vector<averageData> *pData = alzDailyData.getAvgData();
	vector<averageData> ans[2];

	printf_s("# begin to get average data\n");
	int Days[2] = {3,5};
	float wt[2][5] = {0.4,0.3,0.3,0,0,0.5,0.1,0.1,0.1,0.1};
	float *pwt[2] = {nullptr};
	pwt[0] = wt[0];
	pwt[1] = wt[1];
	alzDailyData.GetnDayAverage(Days, pwt, ans, 2);

	clock_t clock_end = clock();
	INFO("Test Analyze Average using time: %d s\n", (int)((clock_end-clock_start)/CLOCKS_PER_SEC));
	printf_s("######################################\n");
	printf_s("# Now test Analyze Average finished! #\n");
	printf_s("######################################\n\n");
	getchar();
}