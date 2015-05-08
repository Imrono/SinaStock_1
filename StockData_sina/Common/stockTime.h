#include "time.h"

class stockTimeHandler
{
public:
	inline static int getLocalWeekDay() {
		_tmNow = time(nullptr);
		localtime_s(&_tm, &_tmNow);
		return _tm.tm_wday;
	}
	inline static tm getLocalDate() {
		_tmNow = time(nullptr);
		localtime_s(&_tm, &_tmNow);
		return _tm;
	}
	inline static int getLocalJiDu() {
		switch (getLocalDate().tm_mon) {
		case  1: case  2: case  3:
			return 0;
		case  4: case  5: case  6:
			return 1;
		case  7: case  8: case  9:
			return 2;
		case 10: case 11: case 12:
			return 3;
		}
	}

private:
	int ReturnWeekDay(unsigned int iYear, unsigned int iMonth, unsigned int iDay);

private:
	static time_t _tmNow;
	static tm _tm;

};