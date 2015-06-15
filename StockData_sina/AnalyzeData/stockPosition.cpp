#include "stockPosition.h"

HoldPosition4Stock::HoldPosition4Stock() : subMount(5, 0), subLastTotal(5, 0.0f), subTotal(5, 0.0f), subBuyCount(5, 0) {
	_keeps = 0.0f;
}

bool HoldPosition4Stock::Buy(int Position, int idx) {
	if (buyCount <= _maxBuyCount) {
		if (-1 != idx) {
			if (subType > idx) {
				subMount[idx] += Position;
				subBuyCount[idx]++;
			} else {
				ERRR("idx:%d >= _subType:%d when buy @HoldPosition4Stock\n", idx, subType);
				return false;
			}
		}
		_mount += Position;
		buyCount ++;
		return true;
	} else {
		INFO("超过最大买入次数%d @HoldPosition4Stock\n", _maxBuyCount);
		return false;
	}
}
bool HoldPosition4Stock::Sell(int Position, int idx) {
	if (_mount >= Position) {
		if (-1 != idx) {
			if (subType > idx) {
				if (subMount[idx] < Position) {
					return false;
				} else {
					subMount[idx] -= Position;
				}
			} else {
				ERRR("StockName:%s, idx:%d >= _subType:%d when sell\n", _stockName.c_str(), idx, subType);
			}
		}
		_mount -= Position;
		return true;
	}  else {
		ERRR("卖出量%d大于剩余量%d\n", Position, _mount);
		return false;
	}
}
float HoldPosition4Stock::UpdateKeeps(float Price) {
	_keeps = Price * _mount;
	return _keeps;
}
int HoldPosition4Stock::addType(int Num){
	for (int i = 0; i < Num; i++) {
		subBuyCount.push_back(0);
		subMount.push_back(0);
		subLastTotal.push_back(0.0f);
		subTotal.push_back(0.0f);
		subType ++;
	}
	return subType;
}

void HoldPosition4Stock::setMaxLoaded(int MaxBuyCount) {
	if (MaxBuyCount >= buyCount) {	// 增大
		_maxBuyCount = MaxBuyCount;
	} else {						// 减小
		_maxBuyCount = MaxBuyCount;
	}
}
