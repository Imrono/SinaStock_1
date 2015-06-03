#ifndef ANALYZE_DAILY_DATA_TURTLE_H
#define ANALYZE_DAILY_DATA_TURTLE_H
#include <vector>
using namespace std;
#include "..//Common//stockData.h"
#include "..//Common//GlobalParam.h"
#include "..//Data_sina//DataHistory.h"
#include "analyzePosition.h"

enum AvgDaysType {
	BuyShort_20day = 0,
	BuyLong_50day = 10,
	SellShort_10day = 20,
	SellLong_20day = 30,

	DEF_N_DAY = 100,
};

struct turtleTopButtom {
	stockDate date;
	float Top;
	float Buttom;
};

struct turtleRawData {
	float lastClose;
	float thisTop;
	float thisButtom;
};
// 最高最低点（n天的）
typedef turtleTopButtom turtleAvgTopButtomData;
class turtleAvgTRData {
public:
	turtleAvgTRData () {
		clear();
	}
	// 日期
	stockDate date;
	// N（n天的平均波动）
	float price;

	inline turtleAvgTRData operator+ (const turtleRawData &rawData) {
		turtleAvgTRData ans;
		ans.price = price + _turtleTR(rawData);
		return ans;
	}
	inline turtleAvgTRData operator+ (float add) {
		turtleAvgTRData ans;
		ans.price = price + add;
		return ans;
	}
	inline turtleAvgTRData operator* (float scalar) {
		turtleAvgTRData ans;
		ans.price = price * scalar;
		return ans;
	}
	inline turtleAvgTRData operator/ (float scalar) {
		turtleAvgTRData ans;
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
	WayOfTurtle(float TurtleUnit = 1.0) {_turtleUnit = TurtleUnit; _sendOrderThisBar = false;}
	// 计算N和avgDay内的最高最低点
	int SetNandTopBottom(vector<sinaDailyData> &rawData, int *avgN, int atrNum, int *avgTopButtom, int tbNum);

	void GetPositionPoint(_in_ vector<sinaDailyData> &rawData, _in_ vector<turtleAvgTRData> N, _in_ vector<turtleAvgTopButtomData> *TopButtom
		, _out_ vector<TradingPoint> &trading, _in_out_ HoldPosition &pt, _in_ int StopLoss);

	// 默认NV的单位为100股的价格，N以股票价格为单位
	static float unitPosition(float N, float unit = 100.0) {return (float)(0.01/(N*unit));}

private:
	// 目前只考虑多头情况
	bool _CreatePosition(vector<turtleAvgTopButtomData>::iterator *it_TopButtom, bool DataEnable, sinaDailyData today, TradingPoint &Trade);
	bool _ClearPosition(vector<turtleAvgTopButtomData>::iterator *it_TopButtom, sinaDailyData today, TradingPoint &Trade);
	void _AddPosition(vector<turtleAvgTRData>::iterator *it_N, sinaDailyData today, TradingPoint &Trade);
	void _StopLoss(vector<turtleAvgTRData>::iterator *it_N, sinaDailyData today, TradingPoint &Trade);

	bool _HasPosition() { return _position.getKeeps() > g_EPS;}

	inline float getMin(float a, float b) {
		return a < b ? a : b;
	}
	inline float getMax(float a, float b) {
		return a > b ? a : b;
	}

	float _turtleUnit;
	// 前一半是建仓数据，后一半是平仓数据
	int _atrNum;
	int *_avgN;
	int _tbNum;
	int *_avgTopButtom;
	vector<turtleAvgTRData> *_N;
	vector<turtleAvgTopButtomData> *_topButtom;

	int _statusType;
	float _preEntryPrice;
	bool _sendOrderThisBar;
	bool _preBreakoutFailure; // 是否有止损

	vector<TradingPoint> _tradeHistory;
	HoldPosition _position;
};

#endif