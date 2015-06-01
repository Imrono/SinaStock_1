#ifndef STOCK_DATA_H
#define STOCK_DATA_H

enum stockMarkets
{
	SH,
	SZ,
	HK
};
#define NEED_UPDATE false
#define HAVE_UPDATED true

struct stockDate
{
	int year;
	int month;
	int day;

	inline bool operator> (stockDate a) {
		return year > a.year ? true :
			year < a.year ? false :
			month > a.month ? true :
			month < a.month ? false :
			day > a.day ? true : false;
	}
	inline bool operator< (stockDate a) {
		return year < a.year ? true :
			year > a.year ? false :
			month < a.month ? true :
			month > a.month ? false :
			day < a.day ? true : false;
	}
	inline bool operator>= (stockDate a) {
		return year > a.year ? true :
			year < a.year ? false :
			month > a.month ? true :
			month < a.month ? false :
			day >= a.day ? true : false;
	}
	inline bool operator<= (stockDate a) {
		return year < a.year ? true :
			year > a.year ? false :
			month < a.month ? true :
			month > a.month ? false :
			day <= a.day ? true : false;
	}
	inline bool operator== (stockDate a) {
		return year != a.year ? false :
			month != a.month ? false :
			day != a.day ? false : true;
	}

};

struct stockTime
{
	int hour;
	int minute;
	int second;
};

struct stockSeason
{
	int year;
	int season;

	int operator- (const stockSeason &aR) {
		return (year-aR.year)*4+(season-aR.season);
	}
};

class stockStatus
{
public:
	stockStatus();

	int year;

	bool seasons[4];	// (true)have updated, (false)need updated
	bool prepare[4];
};

#endif