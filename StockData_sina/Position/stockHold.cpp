#include "stockHold.h"

stockHold::stockHold(): _myTrader(), _cash(0.0), _creditCost(0.0) {}
stockHold::~stockHold() {}

stockHold::stockHold(string ID, bool isAccount): _myTrader(), _cash(0.0), _creditCost(0.0) {
	if (!init(ID, isAccount)) {
		; // No user
		; // Add one
	}
	else {
		; // OK
		; // get account data from database
	}
}
bool stockHold::init(string ID, bool isAccount) {
	return true;
}

int stockHold::getHoldCount(const string &name) {
	map<string, int>::iterator it= _holds.find(name);

	if(it == _holds.end())
		return 0;
	else {
		return _holds[name];
	}
}

bool stockHold::changeHold(const string &name, int count) {
	map<string, int>::iterator it= _holds.find(name);

	if(it == _holds.end()) { // new one
		if (count < 0)
			return false;
		else if (0 == count)
			return true;
		else {
			_holds[name] = count;
			return true;
		}
	} else {
		if (count + _holds[name] < 0)
			return false;
		else {
			_holds[name] += count;
			return true;
		}
	}
}