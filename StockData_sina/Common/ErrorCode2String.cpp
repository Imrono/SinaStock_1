#include "ErrorCode2String.h"
#include <wininet.h>

char* ErrorCode2Str(DWORD errCode)
{
	switch (errCode) {
	case ERROR_IO_PENDING: return "ERROR_IO_PENDING";
	case ERROR_SUCCESS: return "ERROR_SUCCESS";
	default: return "Unknown ErrCode2Str";
	}
}