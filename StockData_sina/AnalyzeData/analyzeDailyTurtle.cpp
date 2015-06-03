#include "analyzeDailyTurtle.h"
#include "..//Common//TraceMicro.h"

// rawData recent -> previous
// turtleATRData previou -> recent
// 历史数据处理，若要时时处理，则要将N前移一位
int WayOfTurtle::SetNandTopBottom(vector<sinaDailyData> &rawData, int *avgN, int atrNum, int *avgTopButtom, int tbNum) {
	if (0 == rawData.size()) {
		ERRR("The raw data for ATR of Turtle is empty~\n");
		return -1;
	}
// 	if (0 != atrNum%2) return -1; //atrNum应该为偶数
	for (int i = 0; i < atrNum; i++) {
		_avgN[i] = avgN[i];
	}
	_atrNum = atrNum;
	for (int i = 0; i < tbNum; i++) {
		_avgTopButtom[i] = avgTopButtom[i];
	}
	_tbNum = tbNum;

	int count = 0;
	turtleRawData turtlePrepare;
	turtleAvgTRData *tmpN = new turtleAvgTRData[atrNum];
	memset(tmpN, 0, atrNum*sizeof(turtleAvgTRData));
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
			if (count < avgN[i]) {
				tmpN[i] = tmpN[i] + turtlePrepare;
				tmpN[i] = tmpN[i] / (float)(avgN[i]);
				// count < avgDay[i]-1开始新循环，不存储数据
				if (count < avgN[i]-1) {
					continue;
				}
			} else { // 之后的周期部分
				// N = (19×PDN+TR)/20
				// TR (True Range) = max(H-L,H-PDC,PDC-L)
				// 前2*avgDay[i]-1不稳定，之后前avgDay[i]位所占的比重就确定了
				tmpN[i] = (tmpN[i]*(float)(avgN[i]-1)+turtlePrepare) / (float)(avgN[i]);
			}
			_N[i].push_back(tmpN[i]);
		}

		// 处理n天的最高最低点
		for (int i = 0; i < tbNum; i++) {
			// 求得周期内的最大，最小值（包括当天的数据）
			memset(&TopButtom, 0, sizeof(TopButtom));
			for (int j = 0; j < avgTopButtom[i]; j++) {
				TopButtom.Top = getMax((r_it-j)->top, TopButtom.Top);
				TopButtom.Buttom = getMin((r_it-j)->buttom, TopButtom.Buttom);
			}
			_topButtom[i].push_back(TopButtom);
		}
		count ++;
	}
	delete []tmpN;
	return count;
}

// 注意nDays要和vector<turtleATRData>匹配
// vector<turtleATRData>不包含前期不满nDays的数据
// 止损位 depend 仓位
void WayOfTurtle::GetPositionPoint(_in_ vector<sinaDailyData> &rawData, _in_ vector<turtleAvgTRData> N, _in_ vector<turtleAvgTopButtomData> *TopButtom
	, _out_ vector<TradingPoint> &trading, _in_out_ HoldPosition &pt, _in_ int StopLoss)
{
	// 这些变量可能要定义成接口
	float maxPosition = pt.getTotal();
	const float factorN = 0.5f;

	if (0 == N.size()) ERRR("turtleATRData中不包含数据!\n");

	// N与TopButtom数据的迭代器，N有若干种
	vector<turtleAvgTRData>::iterator it_N_begin = N.begin()+1;
	vector<turtleAvgTRData>::iterator it_N_end = N.end();
	vector<turtleAvgTRData>::iterator it_N;
	bool DataEnable = false;

	vector<turtleAvgTopButtomData>::iterator *it_TopButtom_begin = new vector<turtleAvgTopButtomData>::iterator[_tbNum];
	vector<turtleAvgTopButtomData>::iterator *it_TopButtom_end = new vector<turtleAvgTopButtomData>::iterator[_tbNum];
	vector<turtleAvgTopButtomData>::iterator *it_TopButtom = new vector<turtleAvgTopButtomData>::iterator[_tbNum];
	for (int i = 0; i < _atrNum; i++) {
		it_TopButtom_begin[i] = TopButtom[i].begin()+1; // 第一个数据没有分析价值
		it_TopButtom_end[i] = TopButtom[i].end();
	}

	int count = 0;
	TradingPoint tmpTradePoint;
	bool N_HasEnable = false;
	bool TopButtom_HasEnable = false;
	// 价格时间数据的迭代器
	vector<sinaDailyData>::reverse_iterator r_it_begin = rawData.rbegin();
	vector<sinaDailyData>::reverse_iterator r_it_end = rawData.rend();
	for (vector<sinaDailyData>::reverse_iterator r_it = r_it_begin; r_it != r_it_end; ++r_it) {
		_sendOrderThisBar = false;
		// 指标日期跟随数据日期
			while (r_it->date > it_N->date)
				++it_N;
			if (r_it->date < it_N->date)
				continue;
			else if (r_it->date == it_N->date) {
				DataEnable = true;
				N_HasEnable = true;
			} else {}

		for (int i = 0; i < _tbNum; i++) {
			while (r_it->date > it_TopButtom[i]->date)
				++it_TopButtom[i];
			if (r_it->date < it_TopButtom[i]->date)
				continue;
			else if (r_it->date == it_TopButtom[i]->date) {
				TopButtom_HasEnable = true;
			} else {}
		}

		// 冲突处理

		// 建仓
		if (N_HasEnable && TopButtom_HasEnable) {
			if (!_HasPosition()) {
				_CreatePosition(it_TopButtom, DataEnable, *r_it, tmpTradePoint);
			} else {
				// 平仓
				if (1) {

				} else {
					// 加仓

					// 止损

				}
			}
		}
		// 观望
	}


	delete []it_TopButtom_end;
	delete []it_TopButtom_begin;
	delete []it_TopButtom;
}

// 只能建一个仓，若有两种指标同时满足，不能同时建两个仓
bool WayOfTurtle::_CreatePosition(vector<turtleAvgTopButtomData>::iterator *it_TopButtom, bool DataEnable, sinaDailyData today, TradingPoint &Trade) {
	// 已经有了持仓就不再建立
	if (_position.getKeeps() > g_EPS) return false;

	// 建仓条件 1.有剩余头寸 2.现在不持有头寸 3.做多突破长期或短期n日的上轨
	if (_position.getTotal() > 1.0f &&			// 1.有剩余头寸
		_position.getKeeps() < g_EPS &&			// 2.现在不持有头寸
		(1 || _preBreakoutFailure)) {			// 4.附加的条件
		for (int i = 0; i < _tbNum/2; i++) {
			if (DataEnable) {
				if ((it_TopButtom[i]-1)->Top < today.top) { // 3.做多突破长期或短期n日的上轨
					Trade.date = today.date;
					Trade.price = (it_TopButtom[i]-1)->Top;
					Trade.trade = BUY_UP;
					Trade.amount = _turtleUnit;
					_position.buy(Trade.price, Trade.amount);
					_sendOrderThisBar = true;
					_tradeHistory.push_back(Trade);
					return true;
				}
			} else continue;
		}
		return false;
	} else return false;
}
bool WayOfTurtle::_ClearPosition(vector<turtleAvgTopButtomData>::iterator *it_TopButtom, sinaDailyData today, TradingPoint &Trade) {
	float ExitLowestPrice = it_TopButtom[_statusType]->Buttom;
	if (today.buttom < ExitLowestPrice) {
		Trade.date = today.date;
		Trade.price = ExitLowestPrice;
		Trade.trade = SELL_DOWN;
		Trade.amount = _position.getKeeps();
		_preEntryPrice = Trade.price;
		_position.sell(Trade.price, Trade.amount);
		_tradeHistory.push_back(Trade);
		return true;
	} else {
		return false;
	}
}

void WayOfTurtle::_AddPosition(vector<turtleAvgTRData>::iterator *it_N, sinaDailyData today, TradingPoint &Trade) {
	float N = it_N[0]->price;
	if (_preEntryPrice < g_EPS && _position.getKeeps() >= (1.0f-g_EPS)) {
		if (today.open >= _preEntryPrice-g_EPS + 0.5f*N) { // 如果开盘就超过设定的1/2N,则直接用开盘价增仓。
			Trade.date = today.date;
			Trade.price = today.open;
			Trade.trade = BUY_UP;
			Trade.amount = _turtleUnit;
			_preEntryPrice = Trade.price;
			_position.buy(Trade.price, Trade.amount);
			_sendOrderThisBar = true;
			_tradeHistory.push_back(Trade);
		}
		while (today.top >= _preEntryPrice-g_EPS + 0.5*N)// 以最高价为标准，判断能进行几次增仓
		{
			Trade.date = today.date;
			Trade.price = _preEntryPrice + 0.5f * N;
			Trade.trade = BUY_UP;
			Trade.amount = _turtleUnit;
			_preEntryPrice = Trade.price;
			_position.buy(Trade.price, Trade.amount);
			_sendOrderThisBar = true;
			_tradeHistory.push_back(Trade);
		}
	}
}

void WayOfTurtle::_StopLoss(vector<turtleAvgTRData>::iterator *it_N, sinaDailyData today, TradingPoint &Trade) {
	if (0 == _tradeHistory.size()) {
		ERRR("Unexpected Stop Loss\n");
		return;
	}
	float N = it_N[0]->price;
	if (today.buttom <= (_preEntryPrice-g_EPS - 2.0f * N) && _sendOrderThisBar == false) { // 加仓Bar不止损
		Trade.date = today.date;
		Trade.price = _preEntryPrice - 2.0f * N;
		Trade.trade = STOP_LOSS_SELL_DOWN;
		Trade.amount = _turtleUnit;
		_position.sell(Trade.price, Trade.amount);
		_tradeHistory.push_back(Trade);
		_preBreakoutFailure = true;
	}
}
