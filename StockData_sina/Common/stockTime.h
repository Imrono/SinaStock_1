#include "time.h"
#include "stockData.h"
#include "TraceMicro.h"
#include <windows.h>

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

	static void getLocalJiDu(int &year, int &jidu);
	inline static int getLocalJiDu(tm time) {
		switch (time.tm_mon) {
		case  1: case  2: case  3:
			return 0;
		case  4: case  5: case  6:
			return 1;
		case  7: case  8: case  9:
			return 2;
		case 10: case 11: case 12:
			return 3;
		default:
			return -1;
		}
	}

	// return 1 for s1, -1 for s2, 0 for equal
	inline static int GetLaterSeason(const stockSeason &s1, const stockSeason &s2) {
		return (s1.year-s2.year)*4+(s1.season-s2.season);
	}
	static SYSTEMTIME SystemTimeDiff(const SYSTEMTIME &st1, const SYSTEMTIME &st2, int &ret);
	static void GetWeekDay(SYSTEMTIME &st);
	static bool IsLastWorkDayInMonth(SYSTEMTIME st);
	static int ReturnWeekDay(unsigned int iYear, unsigned int iMonth, unsigned int iDay);
private:

private:
	static time_t _tmNow;
	static tm _tm;

};