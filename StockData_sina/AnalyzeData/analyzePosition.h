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
	HoldPosition(float total = 0.0f) {
		_total = total;
		_remain = 0.0f;
		_keeps = 0.0f;
		_mount = 0;
		_lastTotal = 0.0f;
		_buyCount = 0;
	}
	// 增加资金
	inline float add (float money) {
		_total += money;
		_remain += money;
		return _total;
	}
	// 减少资金
	inline float sub(float money) {
		if (_remain >= money) {
			_total -= money;
			_remain -= money;
		}
		return _total;
	}
	// 最大买入次数
	inline void setMaxBuyCount(int MaxBuyCount) {
		_maxBuyCount = MaxBuyCount;
	}

	// 什么价钱买多少
	inline bool buy(float Price, int Position) {
		float BuyPosition = Price*(float)Position;
		if (_remain >= BuyPosition && _buyCount <= _maxBuyCount) {
			_lastTotal = _total;
			_keeps = (float)_mount*Price + BuyPosition;
			_remain -= BuyPosition;
			_mount += Position;
			_total = _keeps + _remain;
			_buyCount ++;
			return true;
		} else {
			if (_buyCount > _maxBuyCount)
				INFO("超过最大买入次数%d\n", _maxBuyCount);
			else
				INFO("买入量%.2f大于剩余量%.2f。买入次数%d\n", BuyPosition, _remain, _buyCount);
			return false;
		}
	}
	// 什么价卖多少
	inline bool sell(float Price, int Position) {
		if (_mount >= Position) {
			_lastTotal = _total;
			float SellPosition = Price*(float)Position;
			if (_mount == Position)	_keeps = 0.0f;
			else					_keeps = (float)_mount*Price - SellPosition;
			_remain += SellPosition;
			_mount -= Position;
			_total = _keeps + _remain;
			return true;
		} else {
			ERRR("卖出量大于持有量！\n");
			return false;
		}
	}
	inline void sellAll(float Price) {
		sell(Price, _mount);
		_buyCount = 0;
		showUPorDOWN();
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

	inline bool setTotal(float Total) {
		if (_keeps < Total) {
			_total = Total;
			_remain = _total - _keeps;
			_mount = 0;
			_lastTotal = 0.0f;

			return true;
		} else {
			return false;
		}
	}

	inline void getInfo() {
		DYNAMIC_TRACE(POSITION_TRACE, "total:%.2f,keeps:%.2f,remain:%.2f,mount:%d\n", _total, _keeps, _remain,_mount);
	}
	int ShowThisCmp(HoldPosition *hp) {
		float ThisTotal = _keeps+_remain;
		float InTotal = hp->getKeeps()+hp->getRemain();
		int ans = InTotal > ThisTotal ?
				  1 : InTotal == ThisTotal ?
				  0 : -1;
		char *s;
		float ratio;
		switch (ans) {
		case -1:
			s = "Down";
			ratio = (ThisTotal-InTotal)/ThisTotal;
			break;
		case 0:
			s = "Draw";
			ratio = 0.0;
			break;
		case 1:
			s = "Up";
			ratio = (InTotal-ThisTotal)/ThisTotal;
			break;
		default:
			s = "Unknown";
			ratio = 0.0;
			break;
		}
		DYNAMIC_TRACE(POSITION_TRACE, "ThisTotal:%.2f, InTotal:%.2f, %s%.2f%%\n", ThisTotal, InTotal, s, ratio*100.0f);
		return ans;
	}

private:
	void _setLastTotal(float Price) {
		_lastTotal = _remain + _mount*Price;
	}
	float _total; // 总资产
	float _keeps; // 现有市值
	float _remain; // 可用余额
	int _mount; // 持有总数
	float _lastTotal; // 交易前总资产记录
	int _maxBuyCount; //最大买入次数
	int _buyCount; // 买入次数
};