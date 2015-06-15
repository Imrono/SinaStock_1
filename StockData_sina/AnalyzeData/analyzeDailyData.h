#ifndef ANALYZE_DAILY_DATA_H
#define ANALYZE_DAILY_DATA_H

#include <cstring>
#include <string.h>
#include <vector>
using namespace std;
#include "..//Common//GlobalParam.h"
#include "..//Common//stock2fpTable.h"
#include "..//Data_sina//DataHistory.h"

#include "analyzeDailyDataAverage.h"
#include "analyzeDailyTurtle.h"

enum AnalyzeType {
	TURTLE_ANALYZE = 0,
	AVERAGE_ANALYZE = 1
};

class analyzeDailyData
{
public:
	analyzeDailyData(string StockId);
	~analyzeDailyData();

	vector<sinaDailyData> *getExtractData() {return &_vecExtractData;}
	string getStockID()			{ return _stockID;}
	void setStockID(string id)	{
		ResetStatus();
		_stockID = id;
	}

	void ClearDailyData() { _vecExtractData.clear();}
	void ResetStatus();

	void ExtractionData(getType priceType);

	vector<TradingPoint> *turtleAnalyze(int DaysN, int *CreateTB, int *LeaveTB, int DaysTB, float Chip);
	void averageAnalyze(int *avgDay, float **avgWeight, vector<averageData> *avgData, int avgNum);

	HoldPosition &getPosition(AnalyzeType tpye) {
// 		if (TURTLE_ANALYZE == tpye)
			return _turtle.GetPosition();
	}
private:
	vector<sinaDailyData> *_GetDailyDataFromFile(int year, int data_Jidu, getType priceType);

	//////////////////////////////////////////////////////////////////////////
	// 这里添加不同的分析方法
	WayOfAverage _average;
	WayOfTurtle _turtle;

	//////////////////////////////////////////////////////////////////////////
	stockFile _stkFile;
	// class status
	// now class is used for this stock
	string _stockID;
	// _vecExtractData 28(size)*260(work days/year) < 7.5 kB/year
	vector<sinaDailyData> _vecExtractData;
};

#endif