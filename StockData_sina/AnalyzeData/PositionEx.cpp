#include "PositionEx.h"

// 增加资金
float MyPosition::Add (float Money) {
	_total += Money;
	_remain += Money;
	return _total;
}
// 减少资金
float MyPosition::Sub(float money) {
	if (_remain >= money) {
		_total -= money;
		_remain -= money;
	}
	return _total;
}

// 什么价钱买多少
bool MyPosition::Buy(float Price, int Position, string StockId) {
	float BuyPosition = Price*(float)Position;
	if (_remain >= BuyPosition) {
		_remain -= BuyPosition;
		_mount[StockId] += Position;
		return true;
	} else {
		INFO("%s->买入量%d，买入金额%.2f大于剩余金额%.2f\n", StockId.c_str(), Position, BuyPosition, _remain);
		return false;
	}
}
// 什么价卖多少
bool MyPosition::Sell(float Price, int Position, string StockId) {
	float SellPosition = Price*(float)Position;
	if (_mount.count(StockId) > 0) {
		if (_mount[StockId] >= Position) {
			_remain += SellPosition;
			_mount[StockId] -= Position;
			return true;
		} else {
			INFO("%s->卖出量%d，卖出全额%.2f大于持有数量%d\n", StockId.c_str(), SellPosition, SellPosition, _mount[StockId]);
			return false;
		}
	} else {
		INFO("没有%s的持仓，卖出错误\n", StockId.c_str());
		return false;
	}
}
