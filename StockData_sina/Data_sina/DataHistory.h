#pragma once

#include <cstring>
#include <string>
#include <vector>
#include <map>
using namespace std;
#include "..//Common//stockData.h"
#include "..//Common//HttpUrl.h"
#include "..//Common//stock2fpTable.h"

enum getType {
	FUQUAN = 0,
	NO_FUQUAN = 1
};
enum historyType {
	DAILY_DATA = 0,
	ONTIME_DATA = 1
};

struct DataOfDay
{
	string DetailWeb;
	stockDate date;
	float open;
	float top;
	float close;
	float buttom;
	float exchangeStock;
	float exchangeMoney;

	float factor;
};

class stockHistoryStatus
{
public:
	stockHistoryStatus();
	int HistoryType;
	string symbol;
	bool HasDailyDataInit;
	bool NeedDailyDataUpdate;
	bool HasOnTimeDataInit;
	vector<stockStatus> status;
};

class DataInSeason
{
public:
	DataInSeason();
	~DataInSeason();

	int DataAnalyze(const char* rawData, stockHistoryStatus &status);
	vector<DataOfDay>* getDateDaily();
	stockSeason &getDataSeason() {return _dataSeason;}

private:
	stockSeason _dataSeason;
	vector<DataOfDay> _dataDaily;
};



class HistoryData
{
public:
	HistoryData();
	~HistoryData();

	vector<DataOfDay> * URL2Data(int year, int quarter, string stockID, getType priceType, stockHistoryStatus &status);
	const char* PrepareURL(int year, int quarter, string stockID, getType priceType);

	void CheckAndSetFolder(stockHistoryStatus &status);
	void DailyData2File(vector<DataOfDay> *DailyData);

private:
	stockFile stkFile;
	HttpUrlGetSyn _synHttpUrl;
	DataInSeason _HistoryAnalyze;
	bool _IsPrepare;

	char _URL_StockHistory[256];
	int _startLength;
	
	map <getType, char*> _strPriceType;
	map <int, char*> _strQuarter;
	char _strStockID[32];
	char _strYear[32];
};