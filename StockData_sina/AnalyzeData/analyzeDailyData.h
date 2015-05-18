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

	averageData operator+ (const averageData &aR) {
		averageData ans;
		ans.date = date;
		ans.close = aR.close + close;
		ans.exchangeMoney = aR.exchangeMoney + exchangeMoney;
		ans.exchangeStock = aR.exchangeStock + exchangeStock;
		return ans;
	}
	averageData operator* (float scalar) {
		averageData ans;
		ans.date = date;
		ans.close = scalar * close;
		ans.exchangeMoney = scalar * exchangeMoney;
		ans.exchangeStock = scalar * exchangeStock;
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
	analyzeDailyData();
	~analyzeDailyData();

	string getStockID()			{ return _stockID;}
	void setStockID(string id)	{ _stockID = id;}

	void ClearDailyData() { _vecTmpDailyData.clear();}
	void ResetStatus();

	void ExtractionData(getType priceType);
	vector<averageData> *GetDailyDataFromFile(int year, int data_Jidu, getType priceType);
	int GetnDayAverage(int *avgDay, float **avgWeight, vector<averageData> *avgData, int avgNum);



private:
	void RecordAverage();
	stockFile _stkFile;

	// class status
	// now class is used for this stock
	string _stockID;
	// _vecTmpDailyData 28(size)*260(work days/year) < 7.3 kB/year
	vector<averageData> _vecTmpDailyData;
};