#include "RecordPosition.h"

RecordPosition::RecordPosition() {
	_total = 0.0f;
	_remain = 0.0f;
}


// 增加资金
float RecordPosition::Add (float Money) {
	_total += Money;
	_remain += Money;
	return _total;
}
// 减少资金
float RecordPosition::Sub(float money) {
	if (_remain >= money) {
		_total -= money;
		_remain -= money;
	}
	return _total;
}

bool RecordPosition::getStockMount   (string StockId, int   &Mount) {
	if (_checkStock(StockId)) {
		Mount = _stocksPosition[StockId].GetMount();
		return true;
	} else {
		Mount = 0;
		return false;
	}
}
bool RecordPosition::getStockKeeps   (string StockId, float &Keeps) {
	if (_checkStock(StockId)) {
		Keeps = _stocksPosition[StockId].GetKeeps();
		return true;
	} else {
		Keeps = 0.0f;
		return false;
	}
}
bool RecordPosition::updateStockKeeps(string StockId, float  Price) {
	if (_checkStock(StockId)) {
		_stocksPosition[StockId].UpdateKeeps(Price);
		return true;
	} else {
		return false;
	}
}


// 什么价钱买多少
bool RecordPosition::Buy(float Price, int Position, string StockId) {
	if (!_checkStock(StockId)) {
		INFO("没有%s的持仓，开始建仓~~~\n", StockId.c_str());
		PositionBase tmpPosition;
		tmpPosition.Init(StockId);
		pair<map<string, PositionBase>::iterator, bool> Insert_Pair;
		Insert_Pair = _stocksPosition.insert(map<string, PositionBase>::value_type(StockId, tmpPosition));
		if (!Insert_Pair.second) {
			INFO("建仓失败\n", StockId.c_str());
			return false;
		} else {
			INFO("建仓成功\n", StockId.c_str());
		}
	}
	float BuyPosition = Price*(float)Position;
	if (_remain >= BuyPosition) {
		_remain -= BuyPosition;
		_stocksPosition[StockId].Buy(Position);
		return true;
	} else {
		INFO("%s->买入量%d，买入金额%.2f大于剩余金额%.2f @RecordPosition::Buy\n", StockId.c_str(), Position, BuyPosition, _remain);
		return false;
	}
}
// 什么价卖多少
bool RecordPosition::Sell(float Price, int Position, string StockId) {
	if (!_checkStock(StockId)) {
		INFO("没有%s的持仓，卖出错误 @RecordPosition::Sell\n", StockId.c_str());
		return false;
	}
	float SellPosition = Price*(float)Position;
	if (_stocksPosition[StockId].GetMount() >= Position) {
		_remain += SellPosition;
		_stocksPosition[StockId].Sell(Position);
		return true;
	} else {
		INFO("%s->卖出量%d，卖出全额%.2f大于持有数量%d @RecordPosition::Sell\n", StockId.c_str(), SellPosition, SellPosition, _stocksPosition[StockId].GetMount());
		return false;
	}
}

void RecordPosition::UpdateTotal() {
	float TmpKeeps = 0.0f;
	float TmpValue = 0.0f;
	float TmpPrice = 0.0f;
	map<string, PositionBase>::iterator it_begin = _stocksPosition.begin();
	map<string, PositionBase>::iterator it_end = _stocksPosition.end();
	map<string, PositionBase>::iterator it;
	for(it = it_begin; it != it_end; ++it) {
		TmpPrice = _getPrice(it->first);
		TmpValue = TmpPrice * it->second.GetMount();
		TmpKeeps += TmpValue;
	}
	_total = TmpKeeps + _remain;
}
void RecordPosition::UpdateData() {
	float tmpKeeps = 0.0f;
	float tmpPrice = 0.0f;
	map<string, PositionBase>::iterator it_begin = _stocksPosition.begin();
	map<string, PositionBase>::iterator it_end = _stocksPosition.end();
	for(map<string, PositionBase>::iterator it = it_begin; it != it_end; ++it) {
// 		tmpPrice = _getCurrentStockPrice(it->first);
// 		it->second.UpdateKeeps(tmpPrice);
		tmpKeeps += it->second.GetKeeps();
	}
	_keeps = tmpKeeps;
	_total = tmpKeeps + _remain;
}

float RecordPosition::_getPrice(string StockId) {
	return 0.0f;
}
void RecordPosition::_record2File() {
	// 总市值，总资产记录
}
float RecordPosition::_getCurrentStockPrice(string StockId) {
	return 0.0f;
}
