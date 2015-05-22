#ifndef ANALYZE_DAILY_DATA_H
#define ANALYZE_DAILY_DATA_H

#include <cstring>
#include <string.h>
#include <vector>

using namespace std;
#include "..//Common//GlobalParam.h"
#include "..//Common//stock2fpTable.h"
#include "..//Data_sina//DataHistory.h"

enum GetDataType {
	AVERAGE_EXTRACTION = 0,
	TURTLE_EXTRACTION = 1
};

struct averageData {
	stockDate date;
	float close;
	float exchangeStock;
	float exchangeMoney;

	float factor;

	averageData operator+ (const averageData &aR) {
		averageData ans;
		ans.date = aR.date;
		ans.close = aR.close + close;
		ans.exchangeMoney = aR.exchangeMoney + exchangeMoney;
		ans.exchangeStock = aR.exchangeStock + exchangeStock;
		ans.factor = aR.factor;
		return ans;
	}
	averageData operator* (float scalar) {
		averageData ans;
		ans.date = date;
		ans.close = scalar * close;
		ans.exchangeMoney = scalar * exchangeMoney;
		ans.exchangeStock = scalar * exchangeStock;
		ans.factor = factor;
		return ans;
	}
	averageData operator/ (float scalar) {
		averageData ans;
		ans.date = date;
		ans.close = close / scalar;
		ans.exchangeMoney = exchangeMoney / scalar;
		ans.exchangeStock = exchangeStock / scalar;
		ans.factor = factor;
		return ans;
	}

	void clear() {
		memset(&date, 0, sizeof(stockDate));
		close = 0.0;
		exchangeStock = 0.0;
		exchangeMoney = 0.0;
		factor = 0.0;
	}
};

class analyzeDailyData
{
public:
	analyzeDailyData(){}
	~analyzeDailyData();

	vector<averageData> *getAvgData() {return &_vecTmpDailyDataAvg;}
	string getStockID()			{ return _stockID;}
	void setStockID(string id)	{
		ResetStatus();
		_stockID = id;
	}

	void ClearDailyData() { _vecTmpDailyDataAvg.clear();}
	void ResetStatus();

	void ExtractionData(getType priceType);
	vector<averageData> *GetDailyDataFromFile(int year, int data_Jidu, getType priceType, int ExtractionType = AVERAGE_EXTRACTION);
	int GetnDayAverage(int *avgDay, float **avgWeight, vector<averageData> *avgData, int avgNum);

	double turtleAnalyze(stockDate start, stockDate end, int ATRdays, double unit);

private:
	void RecordAverage();
	float _turtleTR(float H, float L, float PDC) {
		return H-L > H-PDC
			? (H-L >= PDC-L ? H-L : PDC-L)
			: (H-PDC >= PDC-L ? H-PDC : PDC-L);
	}
	stockFile _stkFile;

	// class status
	// now class is used for this stock
	string _stockID;
	// _vecTmpDailyData 28(size)*260(work days/year) < 7.3 kB/year
	vector<averageData> _vecTmpDailyDataAvg;
	vector<sinaDailyData> _vecTmpDailyDataFull;
};

#endif