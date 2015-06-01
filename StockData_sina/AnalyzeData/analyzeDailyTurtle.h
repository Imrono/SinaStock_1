#ifndef ANALYZE_DAILY_DATA_TURTLE_H
#define ANALYZE_DAILY_DATA_TURTLE_H
#include <vector>
using namespace std;
#include "..//Common//stockData.h"
#include "..//Common//GlobalParam.h"
#include "..//Data_sina//DataHistory.h"
#include "analyzePosition.h"

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
	// 日期
	stockDate date;
	// N（n天的平均波动）
	float price;
	// 最高最低点（n天的）
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
	inline float _turtleTR(_in_ const turtleRawData & rawData) {
		return _turtleTR(rawData.thisTop, rawData.thisButtom, rawData.lastClose);
	}
};

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
class WayOfTurtle {
public:
	// 计算N和avgDay内的最高最低点
	int GetATR(_in_ vector<sinaDailyData> &rawData, _in_ int *avgDay, _out_ vector<turtleATRData> *N_TopButtom, _in_ int atrNum);

	void GetPositionPoint(_in_ vector<sinaDailyData> &rawData, _in_ vector<turtleATRData> *N_TopButtom
		, _in_ int atrNum, _out_ vector<TradingPoint> &trading, _in_out_ HoldPosition &pt, _in_ int StopLoss);

	// 默认NV的单位为100股的价格，N以股票价格为单位
	static float unitPosition(float N, float unit = 100.0) {return (float)(0.01/(N*unit));}

private:
	// 目前只考虑多头情况
	bool _CreatePosition(vector<turtleATRData>::iterator *it, bool *DataEnable, sinaDailyData today, TradingPoint &Trade, _in_ int atrNum);
	void _ClearPosition();
	void _AddPosition();
	void _StopLoss();

	bool _HasPosition() { return _position.getKeeps() > g_EPS;}

	inline float getMin(float a, float b) {
		return a < b ? a : b;
	}
	inline float getMax(float a, float b) {
		return a > b ? a : b;
	}

	vector<TradingPoint> _tradeHistory;
	HoldPosition _position;
};

#endif