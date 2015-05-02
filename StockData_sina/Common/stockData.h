#ifndef STOCK_DATA_H
#define STOCK_DATA_H

enum stockMarkets
{
	SH,
	SZ,
	HK
};

struct stockDate
{
	int year;
	int month;
	int day;
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
};

#endif