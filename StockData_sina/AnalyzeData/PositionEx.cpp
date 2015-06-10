#include "PositionEx.h"

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
	if (_mount.count(StockId) > 0) {
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
