#ifndef GLOBAL_PARAM_H
#define GLOBAL_PARAM_H

#define TIME_STEP	1000
#define TIMES_GET	10
#define THRESHOLD	5000000

extern int time_step;
extern int times_get;

extern float upOrderThreshold;
extern float downOrderThreshold;
extern unsigned int StaticTraceParam;
extern unsigned int DynamicTraceParam;

#endif