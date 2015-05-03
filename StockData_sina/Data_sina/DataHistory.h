#pragma once

#include <cstring>
#include <string>
#include <vector>
#include <map>
using namespace std;
#include "..//Common//stockData.h"
#include "..//Common//HttpUrl.h"

enum getType {
	FUQUAN = 0,
	NO_FUQUAN = 1
};

struct DataOfDay
{
	string DetailWeb;
	stockDate date;
	float open;
	float top;
	float close;
	float buttom;
	int exchangeStock;
	int exchangeMoney;

	float factor;
};

class DataInSeason
{
public:
	DataInSeason();
	~DataInSeason();

	int DataAnalyze(const char* rawData);
	vector<DataOfDay>* getDateDaily();

private:
	stockSeason _dataSeason;
	vector<DataOfDay> _dataDaily;
};



class HistoryURL
{
public:
	HistoryURL();
	~HistoryURL();

	void URL2Data(int year, int quarter, string stockID, getType priceType);
	const char* PrepareURL(int year, int quarter, string stockID, getType priceType);

private:
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