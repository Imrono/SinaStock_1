#ifndef TRACE_MICRO_H
#define TRACE_MICRO_H
#include "GlobalParam.h"

#define URL_TRACE		0
#define CALLBACE_TRACE	1
#define WAIT_TRACE		2
#define 
-3
#define ANALYZE_TRACE	4
#define PROGRESS_TRACE	5

#define NUM_TRACES				9
#define TRACE_CONFIG_BUF_SIZE	128

#ifdef _DEBUG
#define STATIC_TRACE(n, ...) \
	do { \
		if (0 != ((1u << n) & StaticTraceParam)) { \
			printf_s(__VA_ARGS__); \
		} \
	} while (0)
#else
#define STATIC_TRACE(n, ...)
#endif // STATIC_TRACE_OPEN | _DEBUG

#ifdef DYNAMIC_TRACE_OPEN
#define DYNAMIC_TRACE(n, ...) \
	do { \
		if (0 != ((1u << n) & DynamicTraceParam)) { \
			printf_s(__VA_ARGS__); \
		} \
	} while (0)
#else
#define DYNAMIC_TRACE(n, ...)
#endif // DYNAMIC_TRACE_OPEN

#define INFO(...) \
	do { printf_s("INFO: "); printf_s(__VA_ARGS__); } while (0)
#define EROR(...) \
	do { printf_s("ERROR: "); printf_s(__VA_ARGS__); } while (0)

void getTraceConfigFromFile();

#endif // TRACE_MICRO_H