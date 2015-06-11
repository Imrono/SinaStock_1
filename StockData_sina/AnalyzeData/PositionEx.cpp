#include "PositionEx.h"

MyPosition::MyPosition() {
	_total = 0.0f;
	_remain = 0.0f;
	_lastTotal = 0.0f;
	_ratio = 0.0f;
}


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
	if (_mount.count(StockId) > 0) { // 查找卖出的股票是否存在
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

void MyPosition::UpdateTotal() {
	_lastTotal = _total;
	float TmpKeeps = 0.0f;
	float TmpValue = 0.0f;
	float TmpPrice = 0.0f;
	map<string,int>::iterator it_begin = _mount.begin();
	map<string,int>::iterator it_end = _mount.end();
	map<string,int>::iterator it;
	for(it = it_begin; it != it_end; ++it) {
		TmpPrice = _getPrice(it->first);
		TmpValue = TmpPrice * it->second;
		TmpKeeps += TmpValue;
	}
	_total = TmpKeeps + _remain;
	_ratio = (_total-_lastTotal)/_lastTotal;
}

float MyPosition::_getPrice(string StockId) {
	return 0.0f;
}
