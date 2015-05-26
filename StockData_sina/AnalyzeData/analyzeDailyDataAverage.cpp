#include "analyzeDailyDataAverage.h"

// [IN] avgDay: the first col is average days, the following cols are weights
// [*]weight is from recent to previous
int WayOfAverage::GetnDayAverage(vector<sinaDailyData> &rawData, int *avgDay, float **avgWeight, vector<averageData> *avgData, int avgNum) {
	int count = 0;
	// initial tmp structure
	if (avgNum > 32) {
		ERRR("avgNum is %d, larger than 32, modify it or change the program!\n", avgNum);
		return 0;
	}
	averageData **tmpData = new averageData*[avgNum];
	for (int i = 0; i < avgNum; i++) {
		tmpData[i] = new averageData[avgDay[i]];
		for (int j = 0; j < avgDay[i]; j++) {
			memset(tmpData[i], 0, sizeof(averageData)*avgDay[i]);
		}
	}
	/********************************************************************************/
	/* average algorithm description
	/* data: 5,4,3,2,1,... weight:0.5,0.3,0.2 avgDay[i]:3 (the ith calculation)
     --------------------------------------------------------------------------------
       count   idx_tmp       tmpData[i][0]      tmpData[i][1]      tmpData[i][2]
                                clear()
         0        0             5*0.5(0)           5*0.2(2)           5*0.3(1)
     --------------------------------------------------------------------------------
                                                   clear()
         1        1          5*0.5+4*0.3(1)        4*0.5(0)        5*0.3+4*0.2(2)
     --------------------------------------------------------------------------------
                                                                      clear()
         2        2       5*0.5+4*0.3+3*0.2(2)   4*0.5+3*0.3(1)       3*0.5(0)
                              push_back(0)
     --------------------------------------------------------------------------------
                                clear()
         3        0             2*0.5(0)      4*0.5+3*0.3+2*0.2(2)  3*0.5+2*0.3(1)
                                                  push_back(1)
     --------------------------------------------------------------------------------
                                                    clear()
         4        1          2*0.5+1*0.3(1)         1*0.5(0)      3*0.5+2*0.3+1*0.2(2)
                                                                      push_back(2)
     --------------------------------------------------------------------------------
       count count%avgDay[i]    tmpData[i][j] += avgData[i][count]*((idx_tmp-j)%avgDay[i])
    idx_tmp.clear()
    if (idx_tmp >= avgDay[i]-1) push_back((idx_tmp+1)%avgDay[i])
	/********************************************************************************/
	// main analyze procedure
	int idx_tmp = 0, idx_wt = 0;
	// loop each raw data
	// [*]from recent to previous such as 2015 -> 1998
	vector<sinaDailyData>::iterator it_begin = rawData.begin();
	vector<sinaDailyData>::iterator it_end = rawData.end();
	for (vector<sinaDailyData>::iterator it = it_begin; it != it_end; ++it) {
		averageData avgTmp;
		avgTmp %= (*it);
		// for average type, such as 5, 10, 15 days
		for (int i = 0; i < avgNum; i++) {
			idx_tmp = count%avgDay[i];
			tmpData[i][idx_tmp].clear(); // ready for record
			// for days in one average type
			for (int j = 0; j < avgDay[i]; j++) {
				idx_wt = (idx_tmp - j + avgDay[i])%avgDay[i];
				// average data = weight1*data1 + weight2*data2 + ...
				tmpData[i][j] = tmpData[i][j] + (avgTmp*avgWeight[i][idx_wt]);
				if (avgDay[i]-1 == idx_wt) {
					// if count less than needed average days, ignore it because it's incomplete
					if (count >= avgDay[i]-1)
						// record average data
						avgData[i].push_back(tmpData[i][(idx_tmp+1)%avgDay[i]]);
				}
			}
		}
		count ++;
		printf_s("%5d,%5d\r", avgData[0].size(), avgData[1].size());
	}
	// clean temp data
	for (int i = 0; i < avgNum; i++) {
		delete []tmpData[i];
	}
	delete []tmpData;

	return count;
}
