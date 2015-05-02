#pragma once

#include <cstring>
#include <string>
#include <vector>
using namespace std;
#include "..//Common//stockData.h"

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