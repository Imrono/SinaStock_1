#ifndef __TRADING__
#define __TRADING__

#include <string>
using namespace std;
#include "..//Position/stockHold.h"

#define CREDIT_TRADE true
#define NORMAL_TRADE false
// enum TradeType {
// 	BUY,
// 	SELL
// };

class Trade
{
private:
	bool _isEnable;
	static Trade* _instance;

public:
	Trade(): _isEnable(true) {}
	~Trade() { delete _instance; _instance = nullptr;}
	static Trade* getInstance() {
		if (_instance == nullptr) {  
			_instance = new Trade();  
		} 
		return _instance; 
	}
	bool go(stockHold &hold, const string &code, int exchangeNum, float price, bool isCredit = false);

	void setEnable(bool enable) {_isEnable = enable; }
	bool getEnable() {return _isEnable; }
};


#endif