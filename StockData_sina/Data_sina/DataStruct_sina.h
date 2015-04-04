#ifndef DATA_STRUCT_SINA_H
#define DATA_STRUCT_SINA_H
#include <tchar.h>
#include <cstring>
#include <string>
using namespace std;
#include "..//Common//stockData.h"
#include "..//Common//GlobalParam.h"
#define NUM_ORDERS	5

struct DataStockNavi_sina
{
	string strSymbol;
	stockMarkets market;
	int symbol;
	char name[20];
};

struct DataUpdate_sina
{
	float cur_price;
	float td_high;
	float td_low;

	int turnover_stock;
	double turnover_current;

	int upOrder_stock[NUM_ORDERS];	//1~5
	float upOrder_price[NUM_ORDERS];	//1~5
	int downOrder_stock[NUM_ORDERS];	//1~5
	float downOrder_price[NUM_ORDERS];	//1~5

	stockTime time;
};

struct Data_sina
{
	DataStockNavi_sina stockNavi;

	float td_open;
	float ysd_close;

	float buyup_price;
	float buyor_price;
	DataUpdate_sina dataUpdate;

	stockDate date;
};

#endif
