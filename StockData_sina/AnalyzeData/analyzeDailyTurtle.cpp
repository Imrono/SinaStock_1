#include "analyzeDailyTurtle.h"
#include "..//Common//TraceMicro.h"

// rawData recent -> previous
// turtleATRData previou -> recent
// 历史数据处理，若要时时处理，则要将N前移一位
int WayOfTurtle::SetNandTopBottom(vector<sinaDailyData> &rawData, int avgN, int *avgTopButtomCreate, int tbNumCreate, int *avgTopButtomLeave, int tbNumLeave) {
	if (0 == rawData.size()) {
		ERRR("The raw data for ATR of Turtle is empty~\n");
		return -1;
	}

	//////////////////////////////////////////////////////////////////////////
	// 记录交易所需的MA到类的状态
	// 1. N的平均天数
	_avgN = avgN;
	// 2. 建仓的平均天数
	for (int i = 0; i < tbNumCreate; i++) {
		_avgTopButtomCreate[i] = avgTopButtomCreate[i];
	}
	_tbNumCreate = tbNumCreate;
	// 3. 平仓的平均天数
	for (int i = 0; i < tbNumLeave; i++) {
		_avgTopButtomLeave[i] = avgTopButtomLeave[i];
	}
	_tbNumLeave = tbNumLeave;
	//////////////////////////////////////////////////////////////////////////

	int count = 0;
	turtleRawData turtlePrepare;
	turtleAvgTRData tmpN;
	memset(&tmpN, 0, sizeof(turtleAvgTRData));
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

		//////////////////////////////////////////////////////////////////////////
		// 处理ATR平均天数
		tmpN.date = r_it->date;
		// 不满一个周期的部分
		if (count < _avgN) {
			tmpN = tmpN + turtlePrepare;
			tmpN = tmpN / (float)(_avgN);
			// count < avgDay[i]-1开始新循环，不存储数据
			if (count < _avgN-1) {
				continue;
			}
		} else { // 之后的周期部分
			// N = (19×PDN+TR)/20
			// TR (True Range) = max(H-L,H-PDC,PDC-L)
			// 前2*avgDay[i]-1不稳定，之后前avgDay[i]位所占的比重就确定了
			tmpN = (tmpN*(float)(_avgN-1)+turtlePrepare) / (float)(_avgN);
		}
		_N.push_back(tmpN);

		//////////////////////////////////////////////////////////////////////////
		// 处理n天的最高最低点
		// 1. 建仓时的n日最高点和n是最低点
		for (int i = 0; i < tbNumCreate; i++) {
			if (count < avgTopButtomCreate[i]) continue; // 前面不足平均天数的数据直接跳过
			// 求得周期内的最大，最小值（包括当天的数据）
			memset(&TopButtom, 0, sizeof(TopButtom));
			for (int j = 0; j < avgTopButtomCreate[i]; j++) {
				if (r_it_begin == r_it-j) {
					break;
				} else {
					TopButtom.Top = getMax((r_it-j)->top, TopButtom.Top);
					TopButtom.Buttom = getMin((r_it-j)->buttom, TopButtom.Buttom);
				}
			}
			_topButtomCreate[i].push_back(TopButtom);
		}
		// 2. 平仓时的tbNumLeave
		for (int i = 0; i < tbNumLeave; i++) {
			if (count < avgTopButtomLeave[i]) continue; // 前面不足平均天数的数据直接跳过
			// 求得周期内的最大，最小值（包括当天的数据）
			memset(&TopButtom, 0, sizeof(TopButtom));
			for (int j = 0; j < avgTopButtomLeave[i]; j++) {
				if (r_it_begin == r_it-j) {
					break;
				} else {
					TopButtom.Top = getMax((r_it-j)->top, TopButtom.Top);
					TopButtom.Buttom = getMin((r_it-j)->buttom, TopButtom.Buttom);
				}
			}
			_topButtomLeave[i].push_back(TopButtom);
		}
		//////////////////////////////////////////////////////////////////////////
		count ++;
	}
	return count;
}

// 注意nDays要和vector<turtleATRData>匹配
// vector<turtleATRData>不包含前期不满nDays的数据
// 止损位 depend 仓位
void WayOfTurtle::GetPositionPoint(_in_ vector<sinaDailyData> &rawData, _in_out_ HoldPosition &pt, _in_ int StopLoss)
{
	// 这些变量可能要定义成接口
	float maxPosition = pt.getTotal();
	const float factorN = 0.5f;
	//////////////////////////////////////////////////////////////////////////
	// 1. check N
	if (0 == _N.size() || 0 == _avgN) {
		ERRR("turtleATRData中不包含数据!\n");
		return;
	}
	// 2. check create Top&Buttom
	if (0 == _tbNumCreate) {
		ERRR("turtle Create Position Top&Buttom 中不包含数据!\n");
		return;
	} else {
		for (int i = 0; i < _tbNumCreate; i++) {
			if (0 == _topButtomCreate[i].size() || 0 == _tbNumCreate) {
				ERRR("turtle Create Position Top&Buttom[%d]中不包含数据!\n", i);
				return;
			}
		}
	}
	// 3. check leave Top&Buttom
	if (0 == _tbNumLeave) {
		ERRR("turtle Create Position Top&Buttom 中不包含数据!\n");
		return;
	} else {
		for (int i = 0; i < _tbNumLeave; i++) {
			if (0 == _topButtomLeave[i].size() || 0 == _tbNumLeave) {
				ERRR("turtle Leave Position Top&Buttom[%d]中不包含数据!\n", i);
				return;
			}
		}
	}
	//////////////////////////////////////////////////////////////////////////
	bool DataEnable = false;
	// 1. it_N_begin = N.begin()+1 是因为前n天的数据是历史数据，要用第n+1天的当时数据与前n天的历史数据分析
	bool N_HasEnable = false;
	vector<turtleAvgTRData>::iterator it_N_begin = _N.begin()+1;
	vector<turtleAvgTRData>::iterator it_N_end = _N.end();
	vector<turtleAvgTRData>::iterator it_N;
	// 2. 建仓n日最高最低点
	bool TopButtomCreate_HasEnable = false;
	vector<turtleAvgTopButtomData>::iterator *it_TopButtomCreate_begin = new vector<turtleAvgTopButtomData>::iterator[_tbNumCreate];
	vector<turtleAvgTopButtomData>::iterator *it_TopButtomCreate_end = new vector<turtleAvgTopButtomData>::iterator[_tbNumCreate];
	vector<turtleAvgTopButtomData>::iterator *it_TopButtomCreate = new vector<turtleAvgTopButtomData>::iterator[_tbNumCreate];
	for (int i = 0; i < _tbNumCreate; i++) {
		it_TopButtomCreate_begin[i] = _topButtomCreate[i].begin()+1; // 第一个数据没有分析价值，之后分析时要-1
		it_TopButtomCreate_end[i] = _topButtomCreate[i].end();
	}
	// 3. 平仓n日最高最低点
	bool TopButtomLeave_HasEnable = false;
	vector<turtleAvgTopButtomData>::iterator *it_TopButtomLeave_begin = new vector<turtleAvgTopButtomData>::iterator[_tbNumLeave];
	vector<turtleAvgTopButtomData>::iterator *it_TopButtomLeave_end = new vector<turtleAvgTopButtomData>::iterator[_tbNumLeave];
	vector<turtleAvgTopButtomData>::iterator *it_TopButtomLeave = new vector<turtleAvgTopButtomData>::iterator[_tbNumLeave];
	for (int i = 0; i < _tbNumLeave; i++) {
		it_TopButtomLeave_begin[i] = _topButtomLeave[i].begin()+1; // 第一个数据没有分析价值，之后分析时要-1
		it_TopButtomLeave_end[i] = _topButtomLeave[i].end();
	}
	//////////////////////////////////////////////////////////////////////////
	int count = 0;
	TradingPoint tmpTradePoint;
	// 价格时间数据的迭代器
	vector<sinaDailyData>::reverse_iterator r_it_begin = rawData.rbegin();
	vector<sinaDailyData>::reverse_iterator r_it_end = rawData.rend();
	for (vector<sinaDailyData>::reverse_iterator r_it = r_it_begin; r_it != r_it_end; ++r_it) {
		_sendOrderThisBar = false;
		// 指标日期跟随数据日期
		// 1. 真实波动 N
		while (r_it->date > it_N->date)
			++it_N;
		if (r_it->date < it_N->date)
			continue;
		else if (r_it->date == it_N->date) {
			N_HasEnable = true;
		} else {}
		// 2. 建仓用n日最高最低点
		for (int i = 0; i < _tbNumCreate; i++) {
			while (r_it->date > it_TopButtomCreate[i]->date)
				++it_TopButtomCreate[i];
			if (r_it->date < it_TopButtomCreate[i]->date)
				continue;
			else if (r_it->date == it_TopButtomCreate[i]->date) {
				TopButtomCreate_HasEnable = true;
			} else {}
		}
		// 3. 平仓用n日最高最低点
		for (int i = 0; i < _tbNumLeave; i++) {
			while (r_it->date > it_TopButtomLeave[i]->date)
				++it_TopButtomLeave[i];
			if (r_it->date < it_TopButtomLeave[i]->date)
				continue;
			else if (r_it->date == it_TopButtomLeave[i]->date) {
				TopButtomLeave_HasEnable = true;
			} else {}
		}

		// U = (C * 1%) / (N * 每点价值);
		// 对于A股， _pointValue = 1（每点价值1块钱）
		_turtleUnit = (_position.getTotal()*_riskRatio)/(_pointValue*it_N->price);

		// 冲突处理

		// 交易过程
		DataEnable = N_HasEnable && TopButtomCreate_HasEnable && TopButtomLeave_HasEnable;
		if (DataEnable) {
			if (!_HasPosition()) {
				// 建仓
				_CreatePosition(it_TopButtomCreate, DataEnable, *r_it, tmpTradePoint);
			} else {
				// 平仓
				if (!_ClearPosition(it_TopButtomLeave, *r_it, tmpTradePoint)) {
					// 加仓
					_AddPosition(it_N, *r_it, tmpTradePoint);
					// 止损
					_StopLoss(it_N, *r_it, tmpTradePoint);
				} else {}
			}
		}
		// 观望

		// 迭代器随着r_it往前移动
		++it_N;
		for (int i = 0; i < _tbNumCreate; i++) {
			++it_TopButtomCreate_end[i];
		}
		for (int i = 0; i < _tbNumLeave; i++) {
			++it_TopButtomLeave_end[i];
		}
	}

	delete []it_TopButtomLeave;
	delete []it_TopButtomLeave_end;
	delete []it_TopButtomLeave_begin;
	delete []it_TopButtomCreate;
	delete []it_TopButtomCreate_end;
	delete []it_TopButtomCreate_begin;
	return;
}

// 只能建一个仓，若有两种指标同时满足，不能同时建两个仓
bool WayOfTurtle::_CreatePosition(vector<turtleAvgTopButtomData>::iterator *it_TopButtom, bool DataEnable, const sinaDailyData &today, TradingPoint &Trade) {
	// 已经有了持仓就不再建立
	if (_position.getKeeps() > g_EPS) return false;
	float EntryPrice;
	// 建仓条件 1.现在不持有头寸 2.做多突破长期或短期n日的上轨
	if (_position.getKeeps() < g_EPS &&			// 1.现在不持有头寸
		(1 || _preBreakoutFailure)) {			// 3.附加的条件
		// 有一个入场条件满足，就开始建仓
		for (int i = 0; i < _tbNumCreate; i++) {
			if (DataEnable) {
				EntryPrice = (it_TopButtom[i]-1)->Top;
				if (EntryPrice < today.top) { // 2.做多突破长期或短期n日的上轨
					Trade.date = today.date;
					Trade.price = EntryPrice;
					Trade.trade = BUY_UP;
					Trade.amount = _turtleUnit;
					if (_position.buy(Trade.price, Trade.amount)) { // 检查有剩余头寸
						_sendOrderThisBar = true;
						_tradeHistory.push_back(Trade);
						TradingPoint::ShowTradeInfo(Trade, "建仓");
						return true;
					} else { // 没有足够余额
						return false;
					}
				}
			} else continue; // !DataEnable
		}
		return false;
	} else return false;
}
bool WayOfTurtle::_ClearPosition(vector<turtleAvgTopButtomData>::iterator *it_TopButtom, const sinaDailyData &today, TradingPoint &Trade) {
	float ExitPrice;
	// 有一个离场条件满足，就全部平仓
	for (int i = 0; i < _tbNumLeave; i++) {
		ExitPrice = (it_TopButtom[i]-1)->Buttom;
		if (today.buttom < ExitPrice) {
			Trade.date = today.date;
			Trade.price = ExitPrice;
			Trade.trade = SELL_DOWN;
			Trade.amount = _position.getKeeps();
			_lastEntryPrice = Trade.price;
			_position.sellAll();
			TradingPoint::ShowTradeInfo(Trade, "平仓");
			_tradeHistory.push_back(Trade);
			return true;
		} else {}
	}
	return false;
}

void WayOfTurtle::_AddPosition(vector<turtleAvgTRData>::iterator it_N, const sinaDailyData &today, TradingPoint &Trade) {
	float N = (it_N-1)->price;
	if (_lastEntryPrice < g_EPS && _position.getKeeps() >= (1.0f-g_EPS)) {
		if (today.open >= _lastEntryPrice-g_EPS + 0.5f*N) { // 如果开盘就超过设定的1/2N,则直接用开盘价增仓。
			Trade.date = today.date;
			Trade.price = today.open;
			Trade.trade = BUY_UP;
			Trade.amount = _turtleUnit;
			_lastEntryPrice = Trade.price;
			if (_position.buy(Trade.price, Trade.amount)) { // 1.有剩余头寸
				TradingPoint::ShowTradeInfo(Trade, "加仓");
				_sendOrderThisBar = true;
				_tradeHistory.push_back(Trade);
			}
		}
		// 一天可以加多次仓
		while (today.top >= _lastEntryPrice-g_EPS + 0.5f*N)// 以最高价为标准，判断能进行几次增仓
		{
			Trade.date = today.date;
			Trade.price = _lastEntryPrice + 0.5f*N;
			Trade.trade = BUY_UP;
			Trade.amount = _turtleUnit;
			_lastEntryPrice = Trade.price;
			if (_position.buy(Trade.price, Trade.amount)) { // 1.有剩余头寸
				TradingPoint::ShowTradeInfo(Trade, "加仓");
				_sendOrderThisBar = true;
				_tradeHistory.push_back(Trade);
			}
		}
	}
}

void WayOfTurtle::_StopLoss(vector<turtleAvgTRData>::iterator it_N, const sinaDailyData &today, TradingPoint &Trade) {
	if (0 == _tradeHistory.size()) {
		ERRR("Unexpected Stop Loss\n");
		return;
	}
	float N = (it_N-1)->price;
	if (today.buttom <= (_lastEntryPrice-g_EPS - 2.0f*N) && _sendOrderThisBar == false) { // 加仓Bar不止损
		Trade.date = today.date;
		Trade.price = _lastEntryPrice - 2.0f*N;
		Trade.trade = STOP_LOSS_SELL_DOWN;
		Trade.amount = _position.getKeeps();
		_position.sellAll();
		TradingPoint::ShowTradeInfo(Trade, "止损");
		_preBreakoutFailure = true;
		_tradeHistory.push_back(Trade);
	}
}
