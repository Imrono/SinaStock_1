#include "Trading.h"

bool Trade::go(stockHold &hold, const string &code, int exchangeNum, float price, bool isCredit) {
	float changeMoney = price*exchangeNum;
	float additionMoney = 0.0;


	if (0 == exchangeNum) {
		return true;
	} else if (exchangeNum > 0) { //buy
		// trading fees
		additionMoney = changeMoney*hold._myTrader.commissionRate > hold._myTrader.minCommission ? changeMoney*hold._myTrader.commissionRate : hold._myTrader.minCommission;
		
		if (hold.getCash()-additionMoney >= changeMoney && !isCredit) { // normal
			hold.changeCash(-changeMoney);
			hold.changeHold(code, exchangeNum);
			hold.changeCash(-additionMoney);
			return true;
		} else {
			return false;
		}
	} else if (exchangeNum < 0) { //sell
		// trading fees
		additionMoney = -changeMoney*hold._myTrader.commissionRate > hold._myTrader.minCommission ? -changeMoney*hold._myTrader.commissionRate : hold._myTrader.minCommission;
		additionMoney += -changeMoney*hold._myTrader.exchangeFees;
		
		if (hold.getHoldCount(code) >= -exchangeNum && !isCredit) {    // normal
			hold.changeHold(code, exchangeNum);
			hold.changeCash(-changeMoney);
			hold.changeCash(-additionMoney);
			return true;
		} else {
			return false;
		}
	} else {
		// should not happen
		return false;
	}
}