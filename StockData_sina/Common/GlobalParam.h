#ifndef GLOBAL_PARAM_H
#define GLOBAL_PARAM_H

#define TIME_STEP	2700
#define TIMES_GET	2
#define THRESHOLD	5000000

extern int time_step;
extern int times_get;

extern float upOrderThreshold;
extern float downOrderThreshold;

extern unsigned int StaticTraceParam;
extern unsigned int DynamicTraceParam;

extern char* DataDir;
extern char* DailyDataDir;
#endif