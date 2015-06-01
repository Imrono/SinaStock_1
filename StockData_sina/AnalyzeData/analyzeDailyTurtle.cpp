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
	, _in_ int atrNum, _out_ vector<TradingPoint> &trading, _in_out_ HoldPosition &pt, _in_ int StopLoss)
{
	// 这些变量可能要定义成接口
	float maxPosition = pt.getTotal();
	const float factorN = 0.5f;

	for (int i = 0; i < atrNum; i++)
		if (0 == N_TopButtom[i].size()) ERRR("turtleATRData中不包含数据!\n");

	// N与TopButtom数据的迭代器，N有若干种
	vector<turtleATRData>::iterator *it_begin = new vector<turtleATRData>::iterator[atrNum];
	vector<turtleATRData>::iterator *it_end = new vector<turtleATRData>::iterator[atrNum];
	vector<turtleATRData>::iterator *it = new vector<turtleATRData>::iterator[atrNum];
	bool *DataEnable = new bool[atrNum];
	for (int i = 0; i < atrNum; i++) {
		it_begin[i] = N_TopButtom[i].begin()+1; // 第一个数据没有分析价值
		it_end[i] = N_TopButtom[i].end();
		DataEnable[i] = false;
	}

	int count = 0;
	TradingPoint tmpTradePoint;
	bool HasEnable = false;
	// 价格时间数据的迭代器
	vector<sinaDailyData>::reverse_iterator r_it_begin = rawData.rbegin();
	vector<sinaDailyData>::reverse_iterator r_it_end = rawData.rend();
	for (vector<sinaDailyData>::reverse_iterator r_it = r_it_begin; r_it != r_it_end; ++r_it) {
		// 指标日期跟随数据日期
		for (int i = 0; i < atrNum; i++) {
			while (r_it->date > it[i]->date)
				++it[i];
			if (r_it->date < it[i]->date)
				continue;
			else if (r_it->date == it[i]->date) {
				DataEnable[i] = true;
				HasEnable = true;
			} else {}
		}
		// 冲突处理

		// 建仓
		if (HasEnable) {
			if (!_HasPosition()) {
				_CreatePosition(it, DataEnable, *r_it, tmpTradePoint, atrNum);
			}
				// 平仓
				// 加仓

				// 止损
		}
		// 观望
	}

	delete []DataEnable;
	delete []it_begin;
	delete []it_end;
	delete []it;
}

void WayOfTurtle::_StopLoss() {

}

// 只能建一个仓，若有两种指标同时满足，不能同时建两个仓
bool WayOfTurtle::_CreatePosition(vector<turtleATRData>::iterator *it, bool *DataEnable, sinaDailyData today, TradingPoint &Trade, _in_ int atrNum) {
	// 已经有了持仓就不再建立
	if (_position.getKeeps() > g_EPS) return false;

	// 建仓条件 1.有剩余头寸 2.现在不持有头寸 3.做多突破长期或短期n日的上轨
	if (_position.getTotal() > 1.0f &&			// 1.有剩余头寸
		_position.getKeeps() < g_EPS) {			// 2.现在不持有头寸
		for (int i = 0; i < atrNum; i++) {
			if (DataEnable) {
				if ((it[i]-1)->lastTopButtom.Top < today.top) { // 3.做多突破长期或短期n日的上轨
					Trade.date = today.date;
					Trade.price = (it[i]-1)->lastTopButtom.Top;
					Trade.trade = BUY_UP;
					return true;
				}
			} else continue;
		}
	} else return false;
}
void WayOfTurtle::_AddPosition() {

}
void WayOfTurtle::_ClearPosition() {

}
