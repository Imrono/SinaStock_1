#ifndef ANALYZE_DAILY_DATA_AVERAGE_H
#define ANALYZE_DAILY_DATA_AVERAGE_H
#include "..//Common//stockData.h"
#include "..//Data_sina//DataHistory.h"

class averageData {
public:
	stockDate date;
	float close;
	float exchangeStock;
	float exchangeMoney;

	float factor;

	inline averageData operator+ (const averageData &aR) {
		averageData ans;
		ans.date = aR.date;
		ans.close = aR.close + close;
		ans.exchangeMoney = aR.exchangeMoney + exchangeMoney;
		ans.exchangeStock = aR.exchangeStock + exchangeStock;
		ans.factor = aR.factor;
		return ans;
	}
	inline averageData operator* (float scalar) {
		averageData ans;
		ans.date = date;
		ans.close = scalar * close;
		ans.exchangeMoney = scalar * exchangeMoney;
		ans.exchangeStock = scalar * exchangeStock;
		ans.factor = factor;
		return ans;
	}
	inline averageData operator/ (float scalar) {
		averageData ans;
		ans.date = date;
		ans.close = close / scalar;
		ans.exchangeMoney = exchangeMoney / scalar;
		ans.exchangeStock = exchangeStock / scalar;
		ans.factor = factor;
		return ans;
	}
	inline averageData &operator%= (sinaDailyData fullData) {
		date = fullData.date;
		close = fullData.close;
		exchangeMoney = fullData.exchangeMoney;
		exchangeStock = fullData.exchangeStock;
		factor = factor;
		return *this;
	}
	inline void clear() {
		memset(&date, 0, sizeof(stockDate));
		close = 0.0;
		exchangeStock = 0.0;
		exchangeMoney = 0.0;
		factor = 0.0;
	}
};
#endif
