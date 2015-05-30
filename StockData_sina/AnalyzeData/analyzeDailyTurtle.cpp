#include "analyzeDailyTurtle.h"
#include "..//Common//TraceMicro.h"

// rawData recent -> previous
// turtleATRData previou -> recent
// 历史数据处理，若要时时处理，则要将N前移一位
int WayOfTurtle::GetATR(_in_ vector<sinaDailyData> &rawData, _in_ int *avgDay, _out_ vector<turtleATRData> *N_TopButtom, _in_ int atrNum) {
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

			N_TopButtom[i].push_back(tmpN[i]);
		}
		count ++;
	}
	delete []tmpN;
	return count;
}

// 注意nDays要和vector<turtleATRData>匹配
// vector<turtleATRData>不包含前期不满nDays的数据
// 止损位 depend 仓位
void WayOfTurtle::GetPositionPoint(_in_ vector<sinaDailyData> &rawData, _in_ vector<turtleATRData> *N_TopButtom
	, _in_ int atrNum, _out_ vector<TradingPoint> &trading, _in_out_ PositionAndTrade &pt, _in_ int StopLoss)
{
	// 这些变量可能要定义成接口
	float maxPosition;
	const float factorN = 0.5f;

	for (int i = 0; i < atrNum; i++)
		if (0 == N_TopButtom[i].size()) ERRR("turtleATRData中不包含数据!\n");

	// N与TopButtom数据的迭代器，N有若干种
	vector<turtleATRData>::iterator *it_begin = new vector<turtleATRData>::iterator[atrNum];
	vector<turtleATRData>::iterator *it_end = new vector<turtleATRData>::iterator[atrNum];
	vector<turtleATRData>::iterator *it = new vector<turtleATRData>::iterator[atrNum];
	for (int i = 0; i < atrNum; i++) {
		it_begin[i] = N_TopButtom[i].begin()+1; // 第一个数据没有分析价值
		it_end[i] = N_TopButtom[i].end();
	}

	int count = 0;
	bool IsIgnore = false;
	TradingPoint tmpTradePoint;
	// 价格时间数据的迭代器
	vector<sinaDailyData>::reverse_iterator r_it_begin = rawData.rbegin();
	vector<sinaDailyData>::reverse_iterator r_it_end = rawData.rend();
	for (vector<sinaDailyData>::reverse_iterator r_it = r_it_begin; r_it != r_it_end; ++r_it) {
		for (int i = 0; i < atrNum; i++) {
			// 日期不同则不处理
			if (memcmp(&(r_it->date), &(it[i]->date), sizeof(stockDate)))
				continue;

			// 确定turtle交易法的买点和卖点
			// 要是一天同时有买入，卖出信号则提示，并默认先买后卖
			if (0) {

			}
			// 买入条件
			if ((it[i]-1)->lastTopButtom.Top < r_it->top) {
				if (!IsIgnore) {
					IsIgnore = true;
					tmpTradePoint.date = r_it->date;
					tmpTradePoint.price = (it[i]-1)->lastTopButtom.Top;
					tmpTradePoint.trade = BUY_UP;
				}
			}

			// 卖出条件
			else if((it[i]-1)->lastTopButtom.Top < r_it->top) {

			}

			// 观望条件
			else {

			}

			// 将N向前移动
			for (int j = 0; j < atrNum; j++) {
				// 日期相同则往前移动
				if (!memcmp(&(it[j]->date), &(r_it->date), sizeof(stockDate))) {
					++it[j];
				} else {
					continue;
				}
			}
		}
	}

	delete []it_begin;
	delete []it_end;
	delete []it;
}
