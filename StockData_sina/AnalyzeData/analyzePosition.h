#include <vector>
#include <map>
#include <utility>
#include <string>
using namespace std;
#include "..//Common//TraceMicro.h"
#include "..//Common//stockData.h"
#include "stockPosition.h"

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
	HoldPosition(float Total = 0.0f);
	// 增加资金
	float add (float Money);
	// 减少资金
	float sub(float money);
	// 设置最大资金
	bool setRemain(float Total);
	// 最大买入次数
	int setMaxLoaded(string StockID, int MaxBuyCount);
	// 插入一种类型
	int addType(string StockID, int Num = 1);

	// 什么价买多少
	bool buy(float Price, int Position, string StockName = "test", int idx = -1);
	// 什么价卖多少
	bool sell(float Price, int Position, string StockName = "test", int idx = -1);
	// 全部卖出
	void sellAll(float Price, string StockName = "test", int idx = -1);

	inline int GetPosition(string StockID = "test", int idx = -1) {
		HoldPosition4Stock *tmpPosition = nullptr;
		if (nullptr != (tmpPosition = _checkStockId(StockID))) {
			if (-1 != idx) {
				if (tmpPosition->subType > idx) {
					return tmpPosition->subBuyCount[idx];
				} else {
					ERRR("idx:%d >= _subType:%d @GetPosition\n", idx, tmpPosition->subType);
					return 0;
				}
			} else {
				return tmpPosition->buyCount;
			}
			return tmpPosition->GetMount();
		} else {
			ERRR("StockID:%s doesn't exist @GetPosition\n", StockID.c_str());
			return 0;
		}
	}

	void showUPorDOWN() {
		DYNAMIC_TRACE(POSITION_TRACE, "lastTotal:%.2f, Total:%.2f, %s:%.2f%%\n", _lastTotal, _total
			, _total>_lastTotal ? "UP" : "DOWN", (_total>_lastTotal ? (_total-_lastTotal) : (_lastTotal-_total))/_lastTotal*100.0f);
	}

	inline float getTotal() {return _total;}
	inline float getRemain() {return _remain;}
	inline int   getMount(string StockID) {
		HoldPosition4Stock *tmpPosition = nullptr;
		if (nullptr != (tmpPosition = _checkStockId(StockID))) {
			return tmpPosition->GetMount();
		} else {
			return 0;
		}
	}

	inline float updateTotal(string StockID, float Price) {
		_lastTotal = _total;
		_total = _remain + Price*(float)getMount(StockID);
		showUPorDOWN();
		return _total;
	}

	inline void getInfo() {
		DYNAMIC_TRACE(POSITION_TRACE, "total:%.2f,remain:%.2f\n", _total, _remain);
	}
	int ShowThisCmp(HoldPosition *hp);

//////////////////////////////////////////////////////////////////////////
	bool AddStock(string StockName);

private:
	void _recordTotal(float Price, int idx = -1);
	HoldPosition4Stock *_checkStockId(string StockID);

	float _total; // 总资产
	float _remain; // 可用余额

// 	int _maxBuyCount; // 最大买入次数
	float _lastTotal; // 操作前的总资产
// 	int _buyCount; // 买入次数
//////////////////////////////////////////////////////////////////////////
	string _lastStock;
//////////////////////////////////////////////////////////////////////////
// 	int _subType;
// 	vector<int> _subMount;
// 	vector<float> _subLastTotal;
// 	vector<float> _subTotal;
// 	vector<int> _subBuyCount;

	map<string, HoldPosition4Stock> _stockPosition;
};