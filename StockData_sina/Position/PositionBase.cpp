#include "PositionBase.h"

void PositionBase::Init(string StockName) {
	_stockName = StockName;
	_keeps = 0.0f;
	_lastPrice = 0.0f;
	_mount = 0;
}
