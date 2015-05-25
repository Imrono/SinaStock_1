#include "analyzeDailyTurtle.h"
#include "..//Common//TraceMicro.h"

// rawData recent -> previous
// 历史数据处理，若要时时处理，则要将N前移一位
int WayOfTurtle::GetATR(vector<sinaDailyData> &rawData, int *avgDay, vector<turtleATRData> *N, int atrNum) {
	if (0 == rawData.size()) {
		ERRR("The raw data for ATR of Turtle is empty~\n");
		return -1;
	}

	int count = 0;
	turtleRawData turtlePrepare;
	turtleATRData *tmpN = new turtleATRData[atrNum];
	memset(tmpN, 0, atrNum*sizeof(turtleATRData));
	turtleTopButtom TopButtom;
	memset(&TopButtom, 0, sizeof(TopButtom));

	// 反向迭代器，privious -> recent
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

		// 处理设定的不同的ATR平均天数
		for (int i = 0; i < atrNum; i++) {
			tmpN[i].date = r_it->date;
			// 不满一个周期的部分
			if (count < avgDay[i]) {
				tmpN[i] = tmpN[i] + turtlePrepare;
				tmpN[i] = tmpN[i] / (float)(avgDay[i]);
				// count < avgDay[i]-1开始新循环，不存储数据
				if (count < avgDay[i]-1) {
					continue;
				}
			} else { // 之后的周期部分
				// N = (19×PDN+TR)/20
				// 前2*avgDay[i]-1不稳定，之后前avgDay[i]位所占的比重就确定了
				tmpN[i] = (tmpN[i]*(float)(avgDay[i]-1)+turtlePrepare) / (float)(avgDay[i]);
			}
			// 求得周期内的最大，最小值（包括当天的数据）
			// TR (True Range) = max(H-L,H-PDC,PDC-L)
			memset(&TopButtom, 0, sizeof(TopButtom));
			for (int j = 0; j < avgDay[i]; j++) {
				TopButtom.Top = getMax((r_it-j)->top, TopButtom.Top);
				TopButtom.Buttom = getMin((r_it-j)->buttom, TopButtom.Buttom);
			}
			tmpN[i].lastTopButtom = TopButtom;

			N[i].push_back(tmpN[i]);
		}
		count ++;
	}
	delete []tmpN;
	return count;
}
