#ifndef TRACE_MICRO_H
#define TRACE_MICRO_H

#ifdef _DEBUG
#define STATIC_TRACE(...) printf(__VA_ARGS__)
#else
#define STATIC_TRACE(...)
#endif // STATIC_TRACE_OPEN
#endif // TRACE_MICRO_H