#include "GlobalParam.h"

int time_step = TIME_STEP;
int times_get = TIMES_GET;

unsigned int StaticTraceParam = 0;
unsigned int DynamicTraceParam = 0;

extern float upOrderThreshold = THRESHOLD;
extern float downOrderThreshold = THRESHOLD;

extern char* DataDir = "d:\\StockData";
extern char* DailyDataDir = "\\DailyData";

extern char* fqPostfix = ".FQdstk";
extern char* nfqPostfix = ".NFQdstk";

extern inline const char *getPriceType(getType priceType) {
	if (FUQUAN == priceType) return fqPostfix;
	else if (NO_FUQUAN == priceType) return nfqPostfix;
	else return ".dstk";
}

