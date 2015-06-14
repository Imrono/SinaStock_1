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

HoldPosition::HoldPosition(int Types, float Total) : _subMount(5, 0), _subLastTotal(5, 0.0f), _subTotal(5, 0.0f), _subBuyCount(5, 0)
{
	for (int i = 0; i < Types; i++) {
		addType();
	}
	_subType = Types;
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

bool HoldPosition::setTotal(float Total) {
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

void HoldPosition::setMaxLoaded(int MaxBuyCount) {
	if (MaxBuyCount >= _buyCount) {	// 增大
		_maxBuyCount = MaxBuyCount;
	} else {						// 减小
		_maxBuyCount = MaxBuyCount;
	}
}

int HoldPosition::addType(int Num){
	for (int i = 0; i < Num; i++) {
		_subBuyCount.push_back(0);
		_subMount.push_back(0);
		_subLastTotal.push_back(0.0f);
		_subTotal.push_back(0.0f);
		_subType ++;
	}
	return _subType;
}

void HoldPosition::_recordTotal(float Price, int idx) {
	_lastTotal = _total;
// 	_keeps = _mount*Price;
	_total = _remain + _mount*Price;
	if (-1 != idx) {
		if (_subType > idx) {
			_subLastTotal[idx] = _subTotal[idx];
			_subTotal[idx] = _subMount[idx]*Price;
		} else {
			ERRR("idx:%d >= _subType:%d when record\n", idx, _subType);
		}
	}
}

int HoldPosition::ShowThisCmp(HoldPosition *hp) {
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

bool HoldPosition::buy(float Price, int Position, int idx) {
	float BuyPosition = Price*(float)Position;
	if (_remain >= BuyPosition && _buyCount <= _maxBuyCount) {
		_remain -= BuyPosition;
		_mount += Position;
		_buyCount ++;
		if (-1 != idx) {
			if (_subType > idx) {
				_subMount[idx] += Position;
				_subBuyCount[idx]++;
			} else {
				ERRR("idx:%d >= _subType:%d when buy\n", idx, _subType);
			}
		}
		_recordTotal(Price, idx);
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
bool HoldPosition::sell(float Price, int Position, int idx) {
	float SellPosition = Price*(float)Position;
	if (_mount >= Position) {
		_remain += SellPosition;
		_mount -= Position;
		if (-1 != idx) {
			if (_subType > idx) {
				_subMount[idx] -= Position;
			} else {
				ERRR("idx:%d >= _subType:%d when sell\n", idx, _subType);
			}
		}
		_recordTotal(Price, idx);
		return true;
	} else {
		ERRR("卖出量大于持有量！\n");
		return false;
	}
}
// 全部卖出
void HoldPosition::sellAll(float Price) {
	sell(Price, _mount);
	_buyCount = 0;
	showUPorDOWN();
}
