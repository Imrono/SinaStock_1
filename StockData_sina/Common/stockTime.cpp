#include "stockTime.h"

time_t stockTimeHandler::_tmNow;
tm stockTimeHandler::_tm;

int stockTimeHandler::ReturnWeekDay(unsigned int iYear, unsigned int iMonth, unsigned int iDay) {
	int iWeek = 0;
	unsigned int y = 0, c = 0, m = 0, d = 0;

	if (iMonth == 1 || iMonth == 2) {
		c = (iYear - 1) / 100;
		y = (iYear - 1) % 100;
		m = iMonth + 12;
		d = iDay;
	} else {
		c = iYear / 100;
		y = iYear % 100;
		m = iMonth;
		d = iDay;
	}

	iWeek = y + y / 4 + c / 4 - 2 * c + 26 * (m + 1)/10 + d - 1;		//Zeller formula
	iWeek = iWeek >= 0 ? (iWeek % 7) : (iWeek % 7 + 7);				//mode iWeek

	return iWeek;  
}
