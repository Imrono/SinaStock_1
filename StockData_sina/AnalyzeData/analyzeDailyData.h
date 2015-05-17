#include <cstring>
#include <string.h>
#include <vector>

using namespace std;
#include "..//Common//GlobalParam.h"
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

	void ClearDailyData() { _vecTmpDailyData.clear();}
	void ResetStatus();

	vector<averageData> *GetDailyDataFromFile(int year, int data_Jidu, getType priceType);
	int GetnDayAverage(int **avgDay, vector<averageData> *avgData, int avgNum);



private:
	stockFile _file;

	// class status
	// now class is used for this stock
	string _stockID;
	// _vecTmpDailyData 28(size)*260(work days/year) < 7.3 kB/year
	vector<averageData> _vecTmpDailyData;
};