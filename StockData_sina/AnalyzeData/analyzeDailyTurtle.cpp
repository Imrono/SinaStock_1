#include "analyzeDailyTurtle.h"
#include "..//Common//TraceMicro.h"

// rawData recent -> previous
int WayOfTurtle::GetATR(vector<sinaDailyData> &rawData, int *avgDay, vector<turtleATRData> *N, int atrNum) {
	if (0 == rawData.size()) {
		ERRR("The raw data for ATR of Turtle is empty~\n");
		return -1;
	}

	// reverse, previous -> recent
	int count = 0;
	turtleRawData turtlePrepare;
	turtleATRData *tmpN = new turtleATRData[atrNum];
	vector<sinaDailyData>::reverse_iterator r_it_begin = rawData.rbegin();
	vector<sinaDailyData>::reverse_iterator r_it_end = rawData.rend();
	for (vector<sinaDailyData>::reverse_iterator r_it = r_it_begin; r_it != r_it_end; ++r_it) {
		if (r_it_begin == r_it) {
			turtlePrepare.lastClose = r_it->open;
		} else {
			turtlePrepare.lastClose = (r_it-1)->close;
		}
		turtlePrepare.thisButtom = r_it->buttom;
		turtlePrepare.thisTop = r_it->top;

		// for ATR days, such as 5, 10, 15 days
		for (int i = 0; i < atrNum; i++) {
			if (count < avgDay[i]) {
				tmpN[i] = tmpN[i] + turtlePrepare;
				tmpN[i] = tmpN[i] / (float)(avgDay[i]);
				if (count < avgDay[i]-1) {
					continue;
				}
			} else {
				tmpN[i] = (tmpN[i]*(float)(avgDay[i]-1)+turtlePrepare) / (float)(avgDay[i]);
			}
			tmpN[i].date = r_it->date;
			N[i].push_back(tmpN[i]);
		}
		count ++;
// 		printf_s("%5d,%5d\r", avgData[0].size(), avgData[1].size());
	}

	delete []tmpN;
	return count;
}
