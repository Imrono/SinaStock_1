﻿#include "analyzeDailyTurtle.h"
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
vector<TradingPoint> *WayOfTurtle::GetPositionPoint(_in_ vector<sinaDailyData> &rawData, _in_ int StopLoss)
{
	// 这些变量可能要定义成接口
	const float factorN = 0.5f;
	//////////////////////////////////////////////////////////////////////////
	// 1. check N
	if (0 == _N.size() || 0 == _avgN) {
		ERRR("turtleATRData中不包含数据!\n");
		return nullptr;
	}
	// 2. check create Top&Buttom
	if (0 == _tbNumCreate) {
		ERRR("turtle Create Position Top&Buttom 中不包含数据!\n");
		return nullptr;
	} else {
		for (int i = 0; i < _tbNumCreate; i++) {
			if (0 == _topButtomCreate[i].size() || 0 == _tbNumCreate) {
				ERRR("turtle Create Position Top&Buttom[%d]中不包含数据!\n", i);
				return nullptr;
			}
		}
	}
	// 3. check leave Top&Buttom
	if (0 == _tbNumLeave) {
		ERRR("turtle Create Position Top&Buttom 中不包含数据!\n");
		return nullptr;
	} else {
		for (int i = 0; i < _tbNumLeave; i++) {
			if (0 == _topButtomLeave[i].size() || 0 == _tbNumLeave) {
				ERRR("turtle Leave Position Top&Buttom[%d]中不包含数据!\n", i);
				return nullptr;
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
		// 对于A股， it_N->price为每一股的单价波动，_pointValue = 1（1股为单位），_turtleUnit的单位为（股）
		_turtleUnit = (_position.getTotal()*_riskRatio)/(it_N->price*_pointValue);

		// 冲突处理（交易规则）
		// 一日内顺序：1.建仓，2.平仓，3.加仓，4.止损
		// 日内有建仓||平仓||加仓，则不止损
		// 一天只做一个操作，但一天可以加几次仓

		// 交易过程
		// 建仓，平仓的n是最高最低点及TR的n日移动平均线都available，则开始交易过程
		DataEnable = N_HasEnable && TopButtomCreate_HasEnable && TopButtomLeave_HasEnable;
		if (DataEnable) {
			// 建仓
			if (_CreatePosition(it_TopButtomCreate, *r_it, tmpTradePoint))
				continue;
			// 平仓
			if (_ClearPosition(it_TopButtomLeave, *r_it, tmpTradePoint))
				continue;
			// 加仓
			if (_AddPosition(it_N, *r_it, tmpTradePoint))
				continue;
			// 止损
			if (_StopLoss(it_N, *r_it, tmpTradePoint))
				continue;
		}
		// 观望
	}

	delete []it_TopButtomLeave;
	delete []it_TopButtomLeave_end;
	delete []it_TopButtomLeave_begin;
	delete []it_TopButtomCreate;
	delete []it_TopButtomCreate_end;
	delete []it_TopButtomCreate_begin;
	return &_tradeHistory;
}

// 只能建一个仓，若有两种指标同时满足，不能同时建两个仓
bool WayOfTurtle::_CreatePosition(vector<turtleAvgTopButtomData>::iterator *it_TopButtom, const sinaDailyData &today, TradingPoint &Trade) {
	// 已经有了持仓就不再建立
	if (_HasPosition()) {
// 		ERRR("建仓时已有头寸\n");
		return false;
	}

	float EntryPrice; // （单价/股）
	// 建仓条件：做多突破长期或短期n日的上轨
	if (1 || _preBreakoutFailure) {				// 附加的条件
		// 有一个入场条件满足，就开始建仓
		for (int i = 0; i < _tbNumCreate; i++) {
			EntryPrice = (it_TopButtom[i]-1)->Top;
			if (EntryPrice < today.top) {		// 做多突破长期或短期n日的上轨
				// 操作
				Trade.date = today.date;
				if (today.open > EntryPrice) {	// 如果开盘就突破，则直接用开盘价建仓。
					Trade.price = today.open+_minPoint;
				} else {						// 正常建仓的情况
					Trade.price = EntryPrice+_minPoint;
				}
				Trade.trade = BUY_UP;
				Trade.amount = _turtleUnit;
				if (_position.buy(Trade.price, Trade.amount)) { // 有剩余头寸，买入成功
					// 显示
					Trade.ShowThisTradeInfo("建仓");
					_position.getInfo();
					// 记录
					_lastEntryPrice = Trade.price;
					_sendOrderThisBar = true;
					_tradeHistory.push_back(Trade);
					return true;
				} else { // 没有足够余额
					return false;
				}
			}
		} // *END* for (int i = 0; i < _tbNumCreate; i++)
		return false;
	} else 
		return false;
}
bool WayOfTurtle::_ClearPosition(vector<turtleAvgTopButtomData>::iterator *it_TopButtom, const sinaDailyData &today, TradingPoint &Trade) {
	// 已经空仓则不再平仓
	if (!_HasPosition()) {
// 		ERRR("Turtle: 平仓时没有头寸\n");
		return false;
	}

	float ExitPrice; // （单价/股）
	// 有一个离场条件满足，就全部平仓
	for (int i = 0; i < _tbNumLeave; i++) {
		ExitPrice = (it_TopButtom[i]-1)->Buttom;
		if (today.buttom < ExitPrice) {		// 做多反向突破长期或短期n日的下轨
			// 操作
			Trade.date = today.date;
			if (today.open < ExitPrice) {	// 如果开盘直接反向突破，则直接用开盘价平仓。
				Trade.price = today.open-_minPoint;
			} else {						// 正常平仓的情况
				Trade.price = ExitPrice-_minPoint;
			}
			Trade.trade = SELL_DOWN;
			Trade.amount = _position.getKeeps();
			_position.sellAll();
			// 显示
			Trade.ShowThisTradeInfo("平仓");
			_position.getInfo();
			// 记录
			_tradeHistory.push_back(Trade);
			return true;
		} else {}
	}
	return false;
}

int WayOfTurtle::_AddPosition(vector<turtleAvgTRData>::iterator it_N, const sinaDailyData &today, TradingPoint &Trade) {
	int Count = 0; // 记录加了几次仓
	if (!_HasPosition()) {
// 		ERRR("Turtle: 加仓时没有建仓\n");
		return Count;
	}

	float N = (it_N-1)->price;
	float AddPrice = _lastEntryPrice + 0.5f*N; // （单价/股）
	// 一天可以加多次仓
	while (today.top > AddPrice) {		// 以突破最高价为标准，可以进行几次增仓
		// 操作
		Trade.date = today.date;
		if (today.open > AddPrice) {	// 如果开盘就突破1/2N，则直接用开盘价增仓。
			Trade.price = today.open + _minPoint;
		} else {						// 正常加仓的情况
			Trade.price = AddPrice + _minPoint;
		}
		Trade.trade = BUY_UP;
		Trade.amount = _turtleUnit;
		if (_position.buy(Trade.price, Trade.amount)) { // 有剩余头寸，买入成功
			// 显示
			Trade.ShowThisTradeInfo("加仓");
			_position.getInfo();
			// 记录
			_lastEntryPrice = Trade.price;
			_sendOrderThisBar = true;
			_tradeHistory.push_back(Trade);
			Count++;
		} else {}
	}
	return Count;
}

bool WayOfTurtle::_StopLoss(vector<turtleAvgTRData>::iterator it_N, const sinaDailyData &today, TradingPoint &Trade) {
	if (!_HasPosition()) {
// 		ERRR("止损时没有头寸\n");
		return false;
	}
	float N = (it_N-1)->price;
	float StopLossPrice = _lastEntryPrice - 2.0f*N; // （单价/股）
	if ((today.buttom < StopLossPrice) && _sendOrderThisBar == false) { // 加仓Bar不止损
		// 操作
		Trade.date = today.date;
		if (today.open < StopLossPrice) {	// 如果开盘反射突破2N，则直接用开盘价止损
			Trade.price = today.open - _minPoint;
		} else {							// 正常平仓的情况
			Trade.price = StopLossPrice - _minPoint;
		}
		Trade.trade = STOP_LOSS_SELL_DOWN;
		Trade.amount = _position.getKeeps();
		_position.sellAll();
		// 显示
		Trade.ShowThisTradeInfo("止损");
		_position.getInfo();
		// 记录
		_preBreakoutFailure = true;
		_tradeHistory.push_back(Trade);
		return true;
	}
	return false;
}
