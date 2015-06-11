#include "PositionEx.h"

MyPosition::MyPosition() {
	_total = 0.0f;
	_remain = 0.0f;
	_lastTotal = 0.0f;
	_ratio = 0.0f;
}


// �����ʽ�
float MyPosition::Add (float Money) {
	_total += Money;
	_remain += Money;
	return _total;
}
// �����ʽ�
float MyPosition::Sub(float money) {
	if (_remain >= money) {
		_total -= money;
		_remain -= money;
	}
	return _total;
}

// ʲô��Ǯ�����
bool MyPosition::Buy(float Price, int Position, string StockId) {
	float BuyPosition = Price*(float)Position;
	if (_remain >= BuyPosition) {
		_remain -= BuyPosition;
		_mount[StockId] += Position;
		return true;
	} else {
		INFO("%s->������%d��������%.2f����ʣ����%.2f\n", StockId.c_str(), Position, BuyPosition, _remain);
		return false;
	}
}
// ʲô��������
bool MyPosition::Sell(float Price, int Position, string StockId) {
	float SellPosition = Price*(float)Position;
	if (_mount.count(StockId) > 0) { // ���������Ĺ�Ʊ�Ƿ����
		if (_mount[StockId] >= Position) {
			_remain += SellPosition;
			_mount[StockId] -= Position;
			return true;
		} else {
			INFO("%s->������%d������ȫ��%.2f���ڳ�������%d\n", StockId.c_str(), SellPosition, SellPosition, _mount[StockId]);
			return false;
		}
	} else {
		INFO("û��%s�ĳֲ֣���������\n", StockId.c_str());
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
