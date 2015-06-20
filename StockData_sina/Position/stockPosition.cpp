#include "stockPosition.h"

bool HoldPosition4Stock::Buy(int Position, int idx) {
	if (buyCount > maxBuyCount) {
		INFO("超过最大买入次数%d @HoldPosition4Stock\n", maxBuyCount);
		return false;
	}
	if (-1 != idx) {
		if (subType > idx) {
			subMount[idx] += Position;
			subBuyCount[idx]++;
		} else {
			ERRR("idx:%d >= _subType:%d @HoldPosition4Stock::Buy\n", idx, subType);
			return false;
		}
	}
	PositionBase::Buy(Position);
	buyCount ++;
	return true;
}
bool HoldPosition4Stock::Sell(int Position, int idx) {
	if (_mount < Position) {
		ERRR("卖出量%d大于剩余量 @HoldPosition4Stock::Sell%d\n", Position, _mount);
		return false;
	}
	if (-1 != idx) {
		if (subType > idx) {
			if (subMount[idx] < Position) {
				ERRR("(StockID:%s) idx:%d 卖出量%d大于剩余量%d @HoldPosition4Stock::Sell\n", _stockName.c_str(), idx, Position, _mount);
				return false;
			} else {
				subMount[idx] -= Position;
			}
		} else {
			ERRR("(StockID:%s) idx:%d >= _subType:%d @HoldPosition4Stock::Sell\n", _stockName.c_str(), idx, subType);
		}
	}
	PositionBase::Sell(Position);
	return true;
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
		maxBuyCount = MaxBuyCount;
	} else {						// 减小
		maxBuyCount = MaxBuyCount;
	}
}

void HoldPosition4Stock::Init(string StockName) {
	PositionBase::Init(StockName);

	maxBuyCount = 0;
	buyCount = 0;
	subType = 0;
}
