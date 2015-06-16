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
	_lastTotal = 0.0f;
}
// 增加资金
float HoldPosition::add (float Money) {
	_total += Money;
	_remain += Money;
	return _total;
}
// 减少资金
float HoldPosition::sub(float Money) {
	if (_remain >= Money) {
		_total -= Money;
		_remain -= Money;
	}
	return _total;
}

bool HoldPosition::setRemain(float Remain) {
	if ( Remain > 0.0f) {
		_total += (Remain - _remain);
		_remain = Remain;
		return true;
	} else {
		return false;
	}
}

int HoldPosition::setMaxLoaded(string StockID, int MaxBuyCount) {
	HoldPosition4Stock *tmpPosition = nullptr;
	if (nullptr != (tmpPosition = _checkStockId(StockID))) {
		if (MaxBuyCount >= tmpPosition->buyCount) {	// 增大
			tmpPosition->maxBuyCount = MaxBuyCount;
		} else {									// 减小
			tmpPosition->maxBuyCount = MaxBuyCount;
		}
		return tmpPosition->maxBuyCount;
	} else {
		return 0;
	}
}

int HoldPosition::addType(string StockID, int Num){
	HoldPosition4Stock *tmpPosition = nullptr;
	if (nullptr != (tmpPosition = _checkStockId(StockID))) {
		for (int i = 0; i < Num; i++) {
			tmpPosition->subBuyCount.push_back(0);
			tmpPosition->subMount.push_back(0);
			tmpPosition->subLastTotal.push_back(0.0f);
			tmpPosition->subTotal.push_back(0.0f);
			tmpPosition->subType ++;
		}
		return tmpPosition->subType;
	} else {
		return 0;
	}
}

void HoldPosition::_recordTotal(float Price, int idx) {
	_lastTotal = _total;
	float tmp = 0.0f;
	for (map<string, HoldPosition4Stock>::iterator iter = _stockPosition.begin(); iter != _stockPosition.end(); ++iter) {
		tmp += Price*iter->second.GetMount();
		if (-1 != idx) {
			if (iter->second.subType > idx) {
				iter->second.subLastTotal[idx] = iter->second.subTotal[idx];
				iter->second.subTotal[idx] = iter->second.subMount[idx]*Price;
			} else {
				ERRR("idx:%d >= _subType:%d when record\n", idx, iter->second.subType);
			}
		}

	}
	_total = _remain + tmp;
}

int HoldPosition::ShowThisCmp(HoldPosition *hp) {
	float ThisTotal = _total;
	float InTotal = hp->getTotal();
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

bool HoldPosition::buy(float Price, int Position, string StockName, int idx) {
	float BuyPosition = Price*(float)Position;
	if (_remain < BuyPosition) {
		INFO("买入量%.2f大于剩余量%.2f @HoldPosition::buy\n", BuyPosition, _remain);
		return false;
	}
	HoldPosition4Stock *tmpPosition;
	if (nullptr == (tmpPosition = _checkStockId(StockName))) {
		ERRR("StockName:%s not exist @HoldPosition::buy\n", StockName.c_str());
		return false;
	}
	if (tmpPosition->Buy(Position, idx)) {
		_remain -= BuyPosition;
		_recordTotal(Price, idx);
		return true;
	} else {
		return false;
	}
}
// 什么价卖多少
bool HoldPosition::sell(float Price, int Position, string StockName, int idx) {
	float SellPosition = Price*(float)Position;
	HoldPosition4Stock *tmpPosition;
	if (nullptr == (tmpPosition = _checkStockId(StockName))) {
		ERRR("StockName:%s not exist @HoldPosition::sell\n", StockName.c_str());
		return false;
	}
	if (tmpPosition->Sell(Position, idx)) {
		_remain += SellPosition;
		_recordTotal(Price, idx);
		return true;
	} else {
		return false;
	}
}
// 全部卖出
void HoldPosition::sellAll(float Price, string StockName, int idx) {
	HoldPosition4Stock *tmpPosition;
	if (nullptr != (tmpPosition = _checkStockId(StockName))) {
		sell(Price, tmpPosition->subMount[idx], StockName, idx);
		tmpPosition->subBuyCount[idx] = 0;
		tmpPosition->buyCount = 0;
	}
	showUPorDOWN();
}

bool HoldPosition::AddStock(string StockName) {
	HoldPosition4Stock tmpPosition;
	tmpPosition.Init(StockName);
	pair<map<string, HoldPosition4Stock>::iterator, bool> Insert_Pair;
	Insert_Pair = _stockPosition.insert(map<string, HoldPosition4Stock>::value_type(StockName, tmpPosition));
	return Insert_Pair.second;
}
HoldPosition4Stock *HoldPosition::_checkStockId(string StockID) {
	if (_stockPosition.count(StockID) > 0) {
		return &_stockPosition[StockID];
	} else {
		ERRR("Stock ID %s not exist in map @HoldPosition::_checkStockId\n", StockID.c_str());
		return nullptr;
	}
}

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
