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
	void ShowThisTradeInfo(const char *s = "") {
		TradingPoint::_showTradeInfo(*this, s);
	}

private:
	static void _showTradeInfo(const TradingPoint &Trade, const char *s = "") {
		DYNAMIC_TRACE(TRADE_TRACE, "date:%4d-%2d-%2d -> %s %s @%.2f, mount:%d\n", 
			Trade.date.year, Trade.date.month, Trade.date.day, s, stockTradeStr[Trade.trade], Trade.price, Trade.amount);
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
	}
	// 增加资金
	inline void add (float money) {
		_total += money;
		_remain += money;
	}

	// 减少资金
	inline float sub(float money) {
		if (_remain < money) return false;
		else {
			_total -= money;
			_remain -= money;
			return true;
		}
	}

	// 什么价钱买多少
	inline bool buy(float price, int position) {
		float buyPosition = price*(float)position;
		if (_remain > buyPosition) {
			_keeps += buyPosition;
			_remain -= buyPosition;
			_mount += position;
			return true;
		} else {
			ERRR("买入量大于剩余量！\n");
			return false;
		}
	}
	inline bool buy(float buyPosition) {
		if (_remain > buyPosition) {
			_keeps += buyPosition;
			_remain -= buyPosition;
			return true;
		} else {
			ERRR("买入量大于剩余量！\n");
			return false;
		}
	}
	// 什么价卖多少
	inline bool sell(float price, int position) {
		float sellPosition = price*(float)position;
		if (_keeps > sellPosition && _mount >= position) {
			_keeps -= sellPosition;
			_remain += sellPosition;
			_mount -= position;
			return true;
		} else {
			ERRR("卖出量大于持有量！\n");
			return false;
		}
	}
	inline bool sell(float sellPosition) {
		if (_keeps >sellPosition ) {
			_keeps -= sellPosition;
			_remain += sellPosition;
			return true;
		} else {
			ERRR("卖出量大于持有量！\n");
			return false;
		}
	}
	inline void sellAll(float Price) {
		_keeps = Price*_mount;
		_total = _keeps + _remain;
		_keeps = 0.0f;
		_remain = _total;
		_mount = 0;
	}

	inline float getKeeps() {return _keeps;}
	inline int getMount() {return _mount;}
	inline float getTotal() {return _total;}
	inline bool setTotal(float Total) {
		if (_keeps < Total) {
			_total = Total;
			_remain = _total - _keeps;
			_mount = 0;
			return true;
		} else {
			return false;
		}
	}
	inline float getRemain() {return _remain;}

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
	float _getRemain() {return _total - _keeps;}
	float _total;
	float _keeps;
	float _remain;
	int _mount;
};