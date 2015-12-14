#ifndef __STOCK_HOLD__
#define __STOCK_HOLD__

#include <string>
#include <map>
using namespace std;

struct mySecuritiesTrader {
	mySecuritiesTrader(float inExchangeFees = 0.6, float inTaxRate = 0.03, float inCommissionRate = 0.0003, float inMinCommission = 5.0, 
		string inTraderName = "", string inAccount = "", string inMyNaceName = "")
		: exchangeFees(inExchangeFees), taxRate(inTaxRate), commissionRate(), minCommission(), traderName(inTraderName)
		, myAccount(inAccount), myNackName(inMyNaceName) {
	};
	float exchangeFees;
	float taxRate;
	float commissionRate;
	float minCommission;
	string traderName;

	string myAccount;
	string myNackName;
};

class stockHold
{
private:
	map<string, int> _holds;
	float _cash;
	float _creditCost;

public:
	const mySecuritiesTrader _myTrader;

	stockHold();
	stockHold(string ID, bool isAccount);
	~stockHold();
	bool init(string ID, bool isAccount);

	map<string, int> &getAllHolds() {
		return _holds;
	}
	int getHoldCount(const string &name);
	float getCash() {
		return _cash;
	}
	// cash "+"为存入， "-"为取出;
	void changeCash(float cash) {
		_cash += cash;
	}
	bool changeHold(const string &name, int count);
};


#endif