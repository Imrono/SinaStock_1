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
	STOP_LOSS_BUY_COVER = 5
};

struct TradingPoint {
	stockDate date;
	float price;
	stockTrade trade;
	float amount;
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
	inline bool buy(float price, float position) {
		float buyPosition = price*position;
		if (_remain > buyPosition) {
			_keeps += buyPosition;
			_remain -= buyPosition;
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
	inline bool sell(float price, float position) {
		float sellPosition = price*position;
		if (_keeps >sellPosition ) {
			_keeps -= sellPosition;
			_remain += sellPosition;
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

	inline float getKeeps() {return _keeps;}
	inline float getTotal() {return _total;}
	inline float setTotal(float total) {_total = total;}
	inline float getRemain() {return _remain;}

	inline void getInfo() {
		INFO("total:%3f,keeps:%3f,remain:%3f\n", _total, _keeps, _remain);
	}
private:
	float _total;
	float _keeps;
	float _remain;
};