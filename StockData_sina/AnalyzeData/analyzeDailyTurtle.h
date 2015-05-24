#ifndef ANALYZE_DAILY_DATA_TURTLE_H
#define ANALYZE_DAILY_DATA_TURTLE_H
#include <vector>
using namespace std;
#include "..//Common//stockData.h"
#include "..//Data_sina//DataHistory.h"

struct turtleTopButtom {
	float Top;
	float Buttom;
};

struct turtleRawData {
	float lastClose;
	float thisTop;
	float thisButtom;
};

class turtleATRData {
public:
	turtleATRData () {
		clear();
	}
	stockDate date;
	float price;
	turtleTopButtom lastTopButtom;

	inline turtleATRData operator+ (const turtleRawData &rawData) {
		turtleATRData ans;
		ans.price = price + _turtleTR(rawData);
		return ans;
	}
	inline turtleATRData operator+ (float add) {
		turtleATRData ans;
		ans.price = price + add;
		return ans;
	}
	inline turtleATRData operator* (float scalar) {
		turtleATRData ans;
		ans.price = price * scalar;
		return ans;
	}
	inline turtleATRData operator/ (float scalar) {
		turtleATRData ans;
		ans.price = price / scalar;
		return ans;
	}
	inline void clear() {
		memset(&date, 0, sizeof(stockDate));
		price = 0.0;
	}
private:
	inline float _turtleTR(float H, float L, float PDC) {
		return H-L > H-PDC
			? (H-L >= PDC-L ? H-L : PDC-L)
			: (H-PDC >= PDC-L ? H-PDC : PDC-L);
	}
	inline float _turtleTR(const turtleRawData & rawData) {
		return _turtleTR(rawData.thisTop, rawData.thisButtom, rawData.lastClose);
	}
};

class WayOfTurtle {
public:
	int GetATR(vector<sinaDailyData> &rawData, int *avgDay, vector<turtleATRData> *N, int atrNum);
private:
	float getMin(float a, float b) {
		return a < b ? a : b;
	}
	float getMax(float a, float b) {
		return a > b ? a : b;
	}

};

#endif