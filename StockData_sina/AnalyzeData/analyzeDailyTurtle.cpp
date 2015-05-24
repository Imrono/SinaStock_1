#include "analyzeDailyTurtle.h"
#include "..//Common//TraceMicro.h"

// rawData recent -> previous
int WayOfTurtle::GetATR(vector<sinaDailyData> &rawData, int *avgDay, vector<turtleData> *N, int atrNum) {
	if (0 == rawData.size()) {
		ERRR("The raw data for ATR of Turtle is empty~\n");
		return -1;
	}

	// reverse, previous -> recent
	int count = 0;
	turtleData *tmpN = new turtleData[atrNum];
	vector<sinaDailyData>::reverse_iterator r_it_begin = rawData.rbegin();
	vector<sinaDailyData>::reverse_iterator r_it_end = rawData.rend();
	for (vector<sinaDailyData>::reverse_iterator r_it = r_it_begin; r_it != r_it_end; ++r_it) {
		// for ATR days, such as 5, 10, 15 days
		for (int i = 0; i < atrNum; i++) {
			if (count < avgDay[i]) {
				tmpN[i] = tmpN[i] + *r_it;
				tmpN[i] = tmpN[i] / (float)(avgDay[i]);
			} else {
			}
			for (int j = 0; j < avgDay[i]; j++) {
			}
		}
		count ++;
// 		printf_s("%5d,%5d\r", avgData[0].size(), avgData[1].size());
	}

	delete []tmpN;
	return count;
}
