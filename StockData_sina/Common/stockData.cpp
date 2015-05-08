#include "stockData.h"

stockStatus::stockStatus()
{
	year = -1;
	for (int i = 0; i < 4; i++) {
		seasons[i] = false;
		prepare[i] = false;
	}
}