#ifndef ANALYZE_DAILY_DATA_TURTLE_H
#define ANALYZE_DAILY_DATA_TURTLE_H
#include <vector>
using namespace std;
#include "..//Common//stockData.h"
#include "..//Data_sina//DataHistory.h"

class turtleData {
public:
	turtleData () {
		clear();
	}
	stockDate date;
	float price;

	inline turtleData operator+ (const sinaDailyData &aR) {
		turtleData ans;
		ans.date = aR.date;
		ans.price = price + aR.close;
		return ans;
	}
	inline turtleData operator* (float scalar) {
		turtleData ans;
		ans.date = date;
		return ans;
	}
	inline turtleData operator/ (float scalar) {
		turtleData ans;
		ans.date = date;
		ans.price = price / scalar;
		return ans;
	}
	inline void clear() {
		memset(&date, 0, sizeof(stockDate));
		price = 0.0;
	}
};

class WayOfTurtle {
public:
	int GetATR(vector<sinaDailyData> &rawData, int *avgDay, vector<turtleData> *N, int atrNum);

private:
	inline float _turtleTR(float H, float L, float PDC) {
		return H-L > H-PDC
			? (H-L >= PDC-L ? H-L : PDC-L)
			: (H-PDC >= PDC-L ? H-PDC : PDC-L);
	}
};

#endif