#include <cstring>
#include <string.h>
#include <vector>

using namespace std;
#include "..//Common//stock2fpTable.h"
#include "..//Data_sina//DataHistory.h"

struct averageData {
	stockDate date;
	float close;
	float exchangeStock;
	float exchangeMoney;

	float factor;
};

class analyzeDailyData
{
public:
	analyzeDailyData();
	~analyzeDailyData();

	string getStockID()			{ return _stockID;}
	void setStockID(string id)	{ _stockID = id;}

	void ClearDailyData() { _vecDailyData.clear();}
	vector<sinaDailyData> *GetDailyDataFromFile(int year, int Jidu);
	void GetnDayAverage(const vector<int> &NofDay);



private:
	vector<sinaDailyData> _vecDailyData;
	stockFile _file;
	string _stockID;

};