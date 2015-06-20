#ifndef POSITION_BASE_H
#define POSITION_BASE_H

#include <string>
using namespace std;

class PositionBase
{
public:
	inline virtual int Buy (int Position) {return _mount += Position;}
	inline virtual int Sell(int Position) {return _mount -= Position;}
	inline float UpdateKeeps(float Price) {
		_keeps = Price * _mount;
		_lastPrice = Price;
		return _keeps;
	}

	inline int GetMount() {return _mount;}
	inline float GetKeeps() {return _keeps;}

	inline void SetName(string StockName) {_stockName = StockName;}

	virtual void Init(string StockName);

protected:
	string _stockName;
	//////////////////////////////////////////////////////////////////////////
	int _mount;
	float _keeps;
	float _lastPrice;
};

#endif