#ifndef TRACE_MICRO_H
#define TRACE_MICRO_H
#include "GlobalParam.h"

#define URL_TRACE		0
#define CALLBACE_TRACE	1
#define WAIT_TRACE		2

#define TRACE_CONFIG_BUF_SIZE	128

#ifdef _DEBUG
#define STATIC_TRACE(n, ...) \
	do { \
		if (0 != ((1u << n) & StaticTraceParam)) { \
			printf_s(__VA_ARGS__); \
		} \
	} while (0)
#else
#define STATIC_TRACE(...)
#endif // STATIC_TRACE_OPEN | _DEBUG

#define INFO(...) \
	do { printf_s(__VA_ARGS__); } while (0)

void getTraceConfigFromFile();

#endif // TRACE_MICRO_H