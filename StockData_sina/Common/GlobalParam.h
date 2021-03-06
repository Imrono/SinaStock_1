﻿#ifndef GLOBAL_PARAM_H
#define GLOBAL_PARAM_H

#ifndef nullptr
#define nullptr NULL
#endif

#define TIME_STEP	2700
#define TIMES_GET	2
#define THRESHOLD	5000000

enum getType {
	FUQUAN = 0,
	NO_FUQUAN = 1
};

#define _in_
#define _out_
#define _in_out_

extern int time_step;
extern int times_get;

extern float upOrderThreshold;
extern float downOrderThreshold;

extern unsigned int StaticTraceParam;
extern unsigned int DynamicTraceParam;

extern char* DataDir;
extern char* DailyDataDir;

extern char* fqPostfix;
extern char* nfqPostfix;

extern const char *getPriceType(getType priceType);

#define g_szPath	128

extern float g_EPS;
#endif