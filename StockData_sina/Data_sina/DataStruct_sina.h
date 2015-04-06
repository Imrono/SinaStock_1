#ifndef DATA_STRUCT_SINA_H
#define DATA_STRUCT_SINA_H
#include <tchar.h>
#include <cstring>
#include <string>
using namespace std;
#include "..//Common//stockData.h"
#include "..//Common//GlobalParam.h"
#define NUM_ORDERS	5

enum RecordType {
	NOT_NEED_RECORD = 0,
	WEIMAI3_THRESHOLD_TOUCH = 0x01,
	WEIMAI4_THRESHOLD_TOUCH = 0x02,
	WEIMAI3_TOTAL_TOUCH = 0x04,
	WEIMAI4_TOTAL_TOUCH = 0x08
};

struct Data_Monitor;

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

struct Data_Monitor
{
	float sMoney[NUM_ORDERS];
	float bMoney[NUM_ORDERS];
	float sTotalMoneyIn5;
	float bTotalMoneyIn5;
	float sLargeMoneyIn5;
	float bLargeMoneyIn5;
	float diffMoneyIn5;
	string strSymbol;
	stockTime time;

	int need2Record;
};

#endif
