#include "stockTime.h"

time_t stockTimeHandler::_tmNow;
tm stockTimeHandler::_tm;

// sunday = 0, monday = 1, tuesday = 2 ...
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

void stockTimeHandler::getLocalJiDu(int &year, int &jidu) {
	tm tmp = getLocalDate();
	tmp.tm_year += 1900;
	tmp.tm_mon += 1;
	switch (tmp.tm_mon) {
	case  1: case  2: case  3:
		jidu = 0; break;
	case  4: case  5: case  6:
		jidu = 1; break;
	case  7: case  8: case  9:
		jidu = 2; break;
	case 10: case 11: case 12:
		jidu = 3; break;
	default:
		jidu = -1; break;
	}
	year = tmp.tm_year;
}

SYSTEMTIME stockTimeHandler::SystemTimeDiff(const SYSTEMTIME &st1, const SYSTEMTIME &st2, int &ret) {
	FILETIME ft1;
	SystemTimeToFileTime(&st1, &ft1);
	ULARGE_INTEGER const u1 = {ft1.dwLowDateTime, ft1.dwHighDateTime};

	FILETIME ft2;
	SystemTimeToFileTime(&st2, &ft2);
	ULARGE_INTEGER const u2 = {ft2.dwLowDateTime, ft2.dwHighDateTime};

	ULARGE_INTEGER u3;
	u3.QuadPart = max(u1.QuadPart, u2.QuadPart) - min(u1.QuadPart, u2.QuadPart);
	FILETIME const ft3 = {u3.LowPart, u3.HighPart};

	ret = u1.QuadPart > u2.QuadPart ?
		1 : u1.QuadPart < u2.QuadPart ?
		-1 : 0;

	SYSTEMTIME st3;
	FileTimeToSystemTime(&ft3, &st3);
	// FILETIME's starting point is 1601-01-01
	st3.wYear -= 1601;
	st3.wMonth -= 1;
	st3.wDay -= 1;
	st3.wDayOfWeek = 0; // useless for this purpose, make it always 0 so it's always ignored
	return st3;
}
// sunday = 0, monday = 1, tuesday = 2 ...
void stockTimeHandler::GetWeekDay(SYSTEMTIME &st) {
	int a,b = 0;
	WORD d = st.wDay;
	WORD m = st.wMonth;
	WORD y = st.wYear;
	WORD &iWeek = st.wDayOfWeek;
	if(m > 12 || m < 0) {
		b = 1;
		ERRR("Month is error.\n");
	} else {
		if(d < 0) {
			b = 1;
			ERRR("Day is error.\n");
		} 
		else {
			switch(m)
			{
			case 1: if(d>32) {b=1; ERRR("day is error.\n");}; break;
			case 2: if((y%4 == 0 && y%100 != 0) || (y%400 == 0)) a = 1;
					else a = 2;
					if(a==1&&d>30) {b=1; ERRR("day is error.\n");}; break;
					if(a==2&&d>29) {b=1; ERRR("day is error.\n");}; break;
			case 3: if(d>32) {b=1; ERRR("day is error.\n");}; break;
			case 4: if(d>31) {b=1; ERRR("day is error.\n");}; break;
			case 5: if(d>32) {b=1; ERRR("day is error.\n");}; break;
			case 6: if(d>31) {b=1; ERRR("day is error.\n");}; break;
			case 7: if(d>32) {b=1; ERRR("day is error.\n");}; break;
			case 8: if(d>32) {b=1; ERRR("day is error.\n");}; break;
			case 9: if(d>31) {b=1; ERRR("day is error.\n");}; break;
			case 10: if(d>32) {b=1; ERRR("day is error.\n");}; break;
			case 11: if(d>31) {b=1; ERRR("day is error.\n");}; break;
			case 12: if(d>32) {b=1; ERRR("day is error.\n");}; break;
			}
		}
// 		if(m == 1 || m == 2) {
// 			m += 12;
// 			y--;
// 		} 

		if(b==0) {
			//iWeek=(d+2*m+3*(m+1)/5+y+y/4-y/100+y/400)%7;
			iWeek = ReturnWeekDay(y, m, d);
		}
	}
}

bool stockTimeHandler::IsLastWorkDayInMonth(SYSTEMTIME st) {
	int d = st.wDay;
	int m = st.wMonth;
	int y = st.wYear;
	int dayOfWeek = st.wDayOfWeek;
	int lastDayInMonth;
	if(m > 12 || m < 0) {
		ERRR("Month is error.\n");
	} else {
		switch(m)
		{
		case 1: lastDayInMonth = 31; break;
		case 2: if((y%4 == 0 && y%100 != 0) || (y%400 == 0)) lastDayInMonth = 29;
				else lastDayInMonth = 28;
				break;
		case 3: lastDayInMonth = 31; break;
		case 4: lastDayInMonth = 30; break;
		case 5: lastDayInMonth = 31; break;
		case 6: lastDayInMonth = 30; break;
		case 7: lastDayInMonth = 31; break;
		case 8: lastDayInMonth = 31; break;
		case 9: lastDayInMonth = 30; break;
		case 10: lastDayInMonth = 31; break;
		case 11: lastDayInMonth = 30; break;
		case 12: lastDayInMonth = 31; break;
		}
	}
	if (d == lastDayInMonth) return true;
	else if (d+1 == lastDayInMonth || d+2 == lastDayInMonth) {
		if (5 == dayOfWeek) return true;
		else return false;
	}
	else return false;
}
