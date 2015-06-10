#include "analyzePosition.h"

char *TradingPoint::stockTradeStr[] = {	"BUY_UP",
									"SELL_DOWN",
									// 做空
									"SELL_SHORT",
									"BUY_COVER",
									// 止损
									"STOP_LOSS_SELL_DOWN",
									"STOP_LOSS_BUY_COVER",
									"ORIGIN"};

HoldPosition::HoldPosition(float Total) {
	_total = Total;
	_remain = 0.0f;
	_keeps = 0.0f;
	_mount = 0;
	_lastTotal = 0.0f;
	_buyCount = 0;
}
// 增加资金
float HoldPosition::add (float Money) {
	_total += Money;
	_remain += Money;
	return _total;
}
// 减少资金
float HoldPosition::sub(float money) {
	if (_remain >= money) {
		_total -= money;
		_remain -= money;
	}
	return _total;
}
