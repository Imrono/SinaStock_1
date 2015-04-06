#include <cstdio>
#include <cstring>
#include <string>
using namespace std;
#include <errno.h>
#include <windows.h>
#include "TraceMicro.h"

void getTraceConfigFromFile()
{
	FILE* fp = NULL;
	errno_t err = fopen_s(&fp, "TraceConfig.trace", "r");
	if(0 == err)
		INFO("\"TraceConfig.trace\" opened success\n");
	else
		INFO("\"TraceConfig.trace\" open failed, reason:%d\n", err);

	char tmp[TRACE_CONFIG_BUF_SIZE] = {0};
	int num[6] = {0};
	unsigned int val[6] = {0};
	unsigned int* tmpParam = NULL;
	while (!feof(fp)) {
		fgets(tmp, TRACE_CONFIG_BUF_SIZE, fp);
		if (!strncmp(tmp, "[STATIC_TRACE]", strlen("[STATIC_TRACE]")))
			tmpParam = &StaticTraceParam;
		else if (!strncmp(tmp, "NUM:", strlen("NUM:")))
			sscanf_s(tmp, "NUM: %d,%d,%d,%d,%d,%d;", num, num+1, num+2, num+3, num+4, num+5);
		else if (!strncmp(tmp, "VAL:", strlen("VAL:"))) {
			sscanf_s(tmp, "VAL: %d,%d,%d,%d,%d,%d;", val, val+1, val+2, val+3, val+4, val+5);
			for (int i = 0; i < 6; i++)
				if (0u == val[i] || 1u == val[i])
					*tmpParam |= (val[i] << num[i]);
		}
		else if ('#' == tmp[0]) {}
		else {}
		memset(tmp, 0, TRACE_CONFIG_BUF_SIZE);
	}

	fclose(fp);
}
