#pragma once

#include <cstring>
#include <string>
#include <vector>
#include <map>
using namespace std;
#include "..//Common//stockData.h"
#include "..//Common//HttpUrl.h"
#include "..//Common//stock2fpTable.h"

enum historyType {
	DAILY_DATA = 0,
	ONTIME_DATA = 1
};

struct sinaDailyData
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
	vector<sinaDailyData>* getDateDaily();
	stockSeason &getDataSeason() {return _dataSeason;}

private:
	stockSeason _dataSeason;
	vector<sinaDailyData> _dataDaily;
};



class HistoryData
{
public:
	HistoryData();
	HistoryData(string stockID);
	~HistoryData();

	vector<sinaDailyData> * URL2Data(int year, int quarter, string stockID, getType priceType, stockHistoryStatus &status);
	const char* PrepareURL(int year, int quarter, string stockID, getType priceType);
	void StockDailyData(string stockID, getType priceType);

	void CheckAndSetFolder(stockHistoryStatus &status, getType priceType);
	void DailyData2File(vector<sinaDailyData> *DailyData, const string &filename);
	bool CheckUpdate(SYSTEMTIME lcTime, const string &file);
private:

	vector<stockHistoryStatus> statusOfStocks;
	stockFile _stkFile;
	HttpUrlGetSyn _synHttpUrl;
	DataInSeason _HistoryAnalyze;
	bool _IsPrepare;

	char _URL_StockHistory[256];
	int _startLength;
	
	string _stockID;
	getType IsFQ;

	map <getType, char*> _strPriceType;
	map <int, char*> _strQuarter;
	char _urlStockID[32];
	char _urlYear[32];
};