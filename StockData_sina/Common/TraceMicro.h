#ifndef TRACE_MICRO_H
#define TRACE_MICRO_H
#include "GlobalParam.h"
#include <cstdio>
using namespace std;

#define URL_TRACE		0
#define CALLBACE_TRACE	1
#define WAIT_TRACE		2
#define FILE_TRACE		3
#define ANALYZE_TRACE	4
#define PROGRESS_TRACE	5
#define TRADE_TRACE		6
#define POSITION_TRACE	7
#define DATA_TRACE		8

#define NUM_TRACES		9
#define TRACE_CONFIG_BUF_SIZE	128

#define TRACE_NAME_TABLE(n) \
		  (0 == n ? "URL: " \
		: (1 == n ? "CALLBACK: " \
		: (2 == n ? "ASYN_WAIT: " \
		: (3 == n ? "FILE: " \
		: (4 == n ? "ANALYZE: " \
		: (5 == n ? "PROGRESS: " \
		: (6 == n ? "TRADE: " \
		: (7 == n ? "POSITION: " \
		: "UNEXPECT: " /*0*/)/*1*/)/*2*/)/*3*/)/*4*/)/*5*/)/*6*/)/*7*/)

#ifdef _DEBUG
#define STATIC_TRACE(n, ...) \
	do { \
		if (0 != ((1u << n) & StaticTraceParam)) { \
			printf_s(TRACE_NAME_TABLE(n)); printf_s(__VA_ARGS__); \
		} \
	} while (0)
#else
#define STATIC_TRACE(n, ...)
#endif // STATIC_TRACE_OPEN | _DEBUG

#ifdef DYNAMIC_TRACE_OPEN
#define DYNAMIC_TRACE(n, ...) \
	do { \
		if (0 != ((1u << n) & DynamicTraceParam)) { \
			printf_s(TRACE_NAME_TABLE(n)); printf_s(__VA_ARGS__); \
		} \
	} while (0)
#else
#define DYNAMIC_TRACE(n, ...)
#endif // DYNAMIC_TRACE_OPEN

#ifdef TEST_TRACE_OPEN
#define TEST_TRACE(...) \
	do { \
	printf_s("TEST: "); printf_s(__VA_ARGS__); \
	} while (0)
#else
#define DYNAMIC_TRACE(n, ...)
#endif // TEST_TRACE_OPEN

#define INFO(...) \
	do { printf_s("INFO: "); printf_s(__VA_ARGS__); } while (0)
#define ERRR(...) \
	do { \
		printf_s("%s(%d)", __FILE__, __LINE__); \
		printf_s("ERROR: "); \
		printf_s(__VA_ARGS__); \
	} while (0)

void getTraceConfigFromFile();

#define TO_DISPLAY(x) (x+1)
#define TO_DATA(x) (x-1)

#endif // TRACE_MICRO_H