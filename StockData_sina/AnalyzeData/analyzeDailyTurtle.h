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
		// 日期会被0000-00-00复盖
		turtleAvgTRData ans;
		ans.price = price + _turtleTR(rawData);
		return ans;
	}

	turtleAvgTRData AddNewAvgElement(const turtleRawData &rawData, int avg) {
		price += _turtleTR(rawData)/(float)avg;
		return *this;
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
	WayOfTurtle(float TotalPosition = 0.0f, float RiskRatio = 0.01f, float PointValue = 100.0f);
	~WayOfTurtle();
	// 计算N和avgDay内的最高最低点
	int SetNandTopBottom(vector<sinaDailyData> &rawData, int avgN, int *avgTopButtomCreate, int *avgTopButtomLeave, int tbNum);
	void InitTopButtom(int TbNum);
	void Clear();
	vector<TradingPoint> *GetPositionPoint(_in_ vector<sinaDailyData> &rawData, _in_ int StopLoss);

	// 默认NV的单位为100股的价格，N以股票价格为单位
	static float unitPosition(float N, float unit = 100.0) {return (float)(0.01/(N*unit));}

	vector<turtleAvgTRData> &getN() {return _N;}

	HoldPosition &GetPosition() {return _position;}
	bool SetPosition(float Total) { return _position.setTotal(Total);}

private:
	// 目前只考虑多头情况
	bool _CreatePosition(vector<turtleAvgTopButtomData>::iterator *it_TopButtom, const sinaDailyData &today, TradingPoint &Trade);
	bool _ClearPosition(vector<turtleAvgTopButtomData>::iterator *it_TopButtom, const sinaDailyData &today, TradingPoint &Trade);
	int _AddPosition(vector<turtleAvgTRData>::iterator it_N, const sinaDailyData &today, TradingPoint &Trade);
	bool _StopLoss(vector<turtleAvgTRData>::iterator it_N, const sinaDailyData &today, TradingPoint &Trade);

	bool _HasPosition() { return _position.getKeeps() > g_EPS;}

	inline float getMin(float a, float b) {
		return a < b ? a : b;
	}
	inline float getMax(float a, float b) {
		return a > b ? a : b;
	}

	// 前一半是建仓数据，后一半是平仓数据
	int _avgN;
	vector<turtleAvgTRData> _N; // 本日的数据也计算在内
	// 建仓信息Comein与离场信息Leave应该一一对应
	int _tbNum;
	bool *_witchTopButtom;
	int *_avgTopButtomCreate;
	vector<turtleAvgTopButtomData> *_topButtomCreate; // 本日的数据也计算在内
	int *_avgTopButtomLeave;
	vector<turtleAvgTopButtomData> *_topButtomLeave;  // 本日的数据也计算在内

	float _lastEntryPrice; // 最近一次买入价
	bool _sendOrderThisBar; // 同一天内加仓就不止损
	bool _preBreakoutFailure; // 是否有止损，用于判断止损后还要不要建仓

	vector<TradingPoint> _tradeHistory;
	HoldPosition _position;
	int _turtleUnit;
	float _riskRatio;
	float _pointValue; //每点价值，A股中为0.01元
	float _minPoint; // 加减一个单位，确保能成交
};

#endif