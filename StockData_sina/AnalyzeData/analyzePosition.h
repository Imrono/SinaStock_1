#include <vector>
using namespace std;
#include "..//Common//TraceMicro.h"
#include "..//Common//stockData.h"

enum stockTrade {
	// 做多
	BUY_UP = 0,
	SELL_DOWN = 1,
	// 做空
	SELL_SHORT = 2,
	BUY_COVER = 3,
	// 止损
	STOP_LOSS_SELL_DOWN = 4,
	STOP_LOSS_BUY_COVER = 5,

	// 初始量
	ORIGIN = 6
};

struct TradingPoint {
public:
	stockDate date;
	float price;
	stockTrade trade;
	int amount;
	int tmpData; // 辅助数据，如突破哪条均线
	void ShowThisTradeInfo(const char *s = "") {
		TradingPoint::_showTradeInfo(*this, s);
	}

private:
	static void _showTradeInfo(const TradingPoint &Trade, const char *s = "") {
		DYNAMIC_TRACE(TRADE_TRACE, "date:%4d-%2d-%2d -> %s(%2d) %s @%.2f, mount:%d\n", 
			Trade.date.year, Trade.date.month, Trade.date.day, s, Trade.tmpData, stockTradeStr[Trade.trade], Trade.price, Trade.amount);
	}

	static char *stockTradeStr[];
};


class HoldPosition
{
public:
	HoldPosition(int Types = 0, float Total = 0.0f);
	// 增加资金
	float add (float Money);
	// 减少资金
	float sub(float money);
	// 设置最大资金
	bool setTotal(float Total);
	// 最大买入次数
	void setMaxLoaded(int MaxBuyCount);
	// 插入一种类型
	int addType(int Num = 1);

	// 什么价买多少
	bool buy(float Price, int Position, int idx = -1);
	// 什么价卖多少
	bool sell(float Price, int Position, int idx = -1);
	// 全部卖出
	void sellAll(float Price);
	// 检查最大买入头寸
	inline bool CheckPosition() {
		return _maxBuyCount >= _buyCount;
	}

	void showUPorDOWN() {
		DYNAMIC_TRACE(POSITION_TRACE, "lastTotal:%.2f, Total:%.2f, %s:%.2f%%\n", _lastTotal, _total
			, _total>_lastTotal ? "UP" : "DOWN", (_total>_lastTotal ? (_total-_lastTotal) : (_lastTotal-_total))/_lastTotal*100.0f);
	}
	inline float getKeeps() {return _keeps;}
	inline int   getMount() {return _mount;}
	inline float getTotal() {return _total;}
	inline float getRemain() {return _remain;}
	inline float updateTotal(float Price) {
		_lastTotal = _total;
		_total = _remain + Price*(float)_mount;
		showUPorDOWN();
		return _total;
	}

	inline void getInfo() {
		DYNAMIC_TRACE(POSITION_TRACE, "total:%.2f,keeps:%.2f,remain:%.2f,mount:%d\n", _total, _keeps, _remain,_mount);
	}
	int ShowThisCmp(HoldPosition *hp);

private:
	void _recordTotal(float Price, int idx = -1);

	float _total; // 总资产
	float _keeps; // 现有市值
	float _remain; // 可用余额
	int _mount; // 持有总数
	int _maxBuyCount; // 最大买入次数
	float _lastTotal; // 操作前的总资产
	int _buyCount; // 买入次数
//////////////////////////////////////////////////////////////////////////
	int _subType;
	vector<int> _subMount;
	vector<float> _subLastTotal;
	vector<float> _subTotal;
	vector<int> _subBuyCount;
};