﻿#include <cmath> 
using namespace std;
#include "analyzeDailyTurtle.h"
#include "..//Common//TraceMicro.h"

WayOfTurtle::WayOfTurtle(string StockName, float TotalPosition, float RiskRatio, float PointValue) : _stockID(StockName) {
	// 参数
	_position.setRemain(TotalPosition);
	_riskRatio = RiskRatio;
// 	_riskRatio = 0.005f;
	_stoplossFactor = 2.0f;
	_addFactor = 0.5f;
	// 状态
	_pointValue = PointValue;
	_minPoint = 0.01f;
	_turtleUnit = 0;
	_sendOrderThisBar = false;
	_lastEntryPrice = nullptr;
	_avgN = 0;
	_tbNum = 0;
	_avgTopButtomCreate = nullptr;
	_topButtomCreate = nullptr;
	_avgTopButtomLeave = nullptr;
	_topButtomLeave = nullptr;
	_isCreate = nullptr;

	if (!_position.AddStock(_stockID)) {
		ERRR("Add Stock Failed!\n");
	}
}
WayOfTurtle::~WayOfTurtle() {
	Clear();
}
void WayOfTurtle::InitTopButtom(int TbNum) {
	_tbNum = TbNum;
	_position.addType(_stockID, TbNum);
	if (nullptr == _avgTopButtomCreate)
		_avgTopButtomCreate = new int[_tbNum];
	if (nullptr == _topButtomCreate)
		_topButtomCreate = new vector<turtleAvgTopButtomData>[_tbNum];
	if (nullptr == _avgTopButtomLeave)
		_avgTopButtomLeave = new int[_tbNum];
	if (nullptr == _topButtomLeave)
		_topButtomLeave = new vector<turtleAvgTopButtomData>[_tbNum];
	if (nullptr == _isCreate)
		_isCreate = new bool[_tbNum];
	if (nullptr == _lastEntryPrice)
		_lastEntryPrice = new float[_tbNum];

	for (int i = 0; i < _tbNum; i++) {
		_isCreate[i] = false;
		_lastEntryPrice[i] = 0.0f;
	}
}
void WayOfTurtle::Clear() {
	if (nullptr != _avgTopButtomCreate)
		delete []_avgTopButtomCreate;
	if (nullptr != _topButtomCreate)
		delete []_topButtomCreate;
	if (nullptr != _avgTopButtomLeave)
		delete []_avgTopButtomLeave;
	if (nullptr != _topButtomLeave)
		delete []_topButtomLeave;
	if (nullptr != _isCreate)
		delete []_isCreate;
	if (nullptr != _lastEntryPrice)
		delete []_lastEntryPrice;
}

// rawData recent -> previous
// turtleATRData previou -> recent
// 历史数据处理，若要时时处理，则要将N前移一位
int WayOfTurtle::SetNandTopBottom(vector<sinaDailyData> &rawData, int avgN, int *avgTopButtomCreate, int *avgTopButtomLeave, int tbNum) {
	if (0 == rawData.size()) {
		ERRR("The raw data for ATR of Turtle is empty~\n");
		return -1;
	}

	//////////////////////////////////////////////////////////////////////////
	// 记录交易所需的MA到类的状态
	// 1. N的平均天数
	_avgN = avgN;
	
	if (_tbNum == tbNum) {
	// 2. 建仓的平均天数
		for (int i = 0; i < tbNum; i++) {
			_avgTopButtomCreate[i] = avgTopButtomCreate[i];
		}
	// 3. 平仓的平均天数
		for (int i = 0; i < tbNum; i++) {
			_avgTopButtomLeave[i] = avgTopButtomLeave[i];
		}
	} else {
		ERRR("state _tbNum != input tbNum\n");
		return -1;
	}
	//////////////////////////////////////////////////////////////////////////

	int Count = 0;
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
		// 不满一个周期的部分
		if (Count < _avgN) {
			// N = (N1+N2+...+Nn)/n
			tmpN.AddNewAvgElement(turtlePrepare, _avgN);
		} else { // 之后的周期部分
			// N = (19*PDN+TR)/20
			// TR (True Range) = max(H-L,H-PDC,PDC-L)
			// 前2*avgDay[i]-1不稳定，之后前avgDay[i]位所占的比重就确定了
			tmpN = (tmpN*(float)(_avgN-1)+turtlePrepare) / (float)(_avgN);
		}
		// count .= avgDay[i]-1时，才开始存储数据
		if (Count >= _avgN-1) {
			tmpN.date = r_it->date;
			_N.push_back(tmpN);
		}

		//////////////////////////////////////////////////////////////////////////
		// 处理n天的最高最低点
		for (int i = 0; i < tbNum; i++) {
		// 1. 建仓时的n日最高点和n是最低点
			if (Count < avgTopButtomCreate[i]-1) continue; // 前面不足平均天数的数据直接跳过
			// 求得周期内的最大，最小值（包括当天的数据）
			memset(&TopButtom, 0, sizeof(TopButtom));
			for (int j = 0; j < avgTopButtomCreate[i]; j++) {
				if (r_it_begin == r_it-j) {
					break;
				} else {
					if (0 == j) {
						TopButtom.Top = (r_it-j)->top;
						TopButtom.Buttom = (r_it-j)->buttom;
					} else {
						TopButtom.Top = getMax((r_it-j)->top, TopButtom.Top);
						TopButtom.Buttom = getMin((r_it-j)->buttom, TopButtom.Buttom);
					}
				}
			}
			TopButtom.date = r_it->date;
			_topButtomCreate[i].push_back(TopButtom);
		// 2. 平仓时的tbNumLeave
			if (Count < avgTopButtomLeave[i]-1) continue; // 前面不足平均天数的数据直接跳过
			// 求得周期内的最大，最小值（包括当天的数据）
			memset(&TopButtom, 0, sizeof(TopButtom));
			for (int j = 0; j < avgTopButtomLeave[i]; j++) {
				if (r_it_begin == r_it-j) {
					break;
				} else {
					if (0 == j) {
						TopButtom.Top = (r_it-j)->top;
						TopButtom.Buttom = (r_it-j)->buttom;
					} else {
						TopButtom.Top = getMax((r_it-j)->top, TopButtom.Top);
						TopButtom.Buttom = getMin((r_it-j)->buttom, TopButtom.Buttom);
					}
				}
			}
			TopButtom.date = r_it->date;
			_topButtomLeave[i].push_back(TopButtom);
		}
		//////////////////////////////////////////////////////////////////////////
		Count ++;
	}
	vector<turtleAvgTopButtomData>::iterator it_begin[2];
	it_begin[0] = _topButtomCreate[0].begin();
	it_begin[1] = _topButtomCreate[1].begin();
	vector<turtleAvgTopButtomData>::iterator it_end[2];
	it_end[0] = _topButtomCreate[0].end();
	it_end[1] = _topButtomCreate[1].end();
	vector<turtleAvgTopButtomData>::iterator it[2];
	for (int i = 0; i < 1; i++) {
		for (it[i] = it_begin[i]; it[i] != it_end[i]; ++it[i]) {
			printf_s("Create: %d-> (%4d-%2d-%2d) Top:%.3f, Buttom:%.3f\n", _avgTopButtomCreate[i], it[i]->date.year, it[i]->date.month, it[i]->date.day
				, it[i]->Top, it[i]->Buttom);
		}
	}
	vector<turtleAvgTRData>::iterator it_N;
	for (it_N = _N.begin(); it_N != _N.end(); ++it_N) {
		printf_s("Create: %d-> (%4d-%2d-%2d) N:%.2f\n", _avgN, it_N->date.year, it_N->date.month, it_N->date.day , it_N->price);
	}


	return Count;
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
	if (0 == _tbNum) {
		ERRR("turtle Position Top&Buttom 中不包含数据!\n");
		return nullptr;
	} else {
		for (int i = 0; i < _tbNum; i++) {
	// 2. check create Top&Buttom
			if (0 == _topButtomCreate[i].size() || 0 == _tbNum) {
				ERRR("turtle Create Position Top&Buttom[%d]中不包含数据!\n", i);
				return nullptr;
			}
	// 3. check leave Top&Buttom
			if (0 == _topButtomLeave[i].size() || 0 == _tbNum) {
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
	vector<turtleAvgTRData>::iterator it_N = it_N_begin;
	// 2. 建仓n日最高最低点
	bool TopButtomCreate_HasEnable = false;
	bool *TmpCreateEnable = new bool[_tbNum];
	vector<turtleAvgTopButtomData>::iterator *it_TopButtomCreate_begin = new vector<turtleAvgTopButtomData>::iterator[_tbNum];
	vector<turtleAvgTopButtomData>::iterator *it_TopButtomCreate_end = new vector<turtleAvgTopButtomData>::iterator[_tbNum];
	vector<turtleAvgTopButtomData>::iterator *it_TopButtomCreate = new vector<turtleAvgTopButtomData>::iterator[_tbNum];
	for (int i = 0; i < _tbNum; i++) {
		it_TopButtomCreate_begin[i] = _topButtomCreate[i].begin()+1; // 第一个数据没有分析价值，之后分析时要-1
		it_TopButtomCreate_end[i] = _topButtomCreate[i].end();
		it_TopButtomCreate[i] = it_TopButtomCreate_begin[i];
		TmpCreateEnable[i] = false;
	}
	// 3. 平仓n日最高最低点
	bool TopButtomLeave_HasEnable = false;
	bool *TmpLeaveEnable = new bool[_tbNum];
	vector<turtleAvgTopButtomData>::iterator *it_TopButtomLeave_begin = new vector<turtleAvgTopButtomData>::iterator[_tbNum];
	vector<turtleAvgTopButtomData>::iterator *it_TopButtomLeave_end = new vector<turtleAvgTopButtomData>::iterator[_tbNum];
	vector<turtleAvgTopButtomData>::iterator *it_TopButtomLeave = new vector<turtleAvgTopButtomData>::iterator[_tbNum];
	for (int i = 0; i < _tbNum; i++) {
		it_TopButtomLeave_begin[i] = _topButtomLeave[i].begin()+1; // 第一个数据没有分析价值，之后分析时要-1
		it_TopButtomLeave_end[i] = _topButtomLeave[i].end();
		it_TopButtomLeave[i] = it_TopButtomLeave_begin[i];
		TmpLeaveEnable[i] = false;
	}
	//////////////////////////////////////////////////////////////////////////
	int Count = 0;
	TradingPoint tmpTradePoint;
	_position.setMaxLoaded(_stockID, 4);
	// 价格时间数据的迭代器
	vector<sinaDailyData>::reverse_iterator r_it_begin = rawData.rbegin();
	vector<sinaDailyData>::reverse_iterator r_it_end = rawData.rend();
	vector<sinaDailyData>::reverse_iterator r_it;
	for (r_it = r_it_begin; r_it != r_it_end; ++r_it) {
		_sendOrderThisBar = false;
		// 指标日期跟随数据日期
		// 1. 真实波动 N
		while (r_it->date > it_N->date)
			++it_N;
		if (!N_HasEnable) {
			if (r_it->date == it_N->date) {
				N_HasEnable = true;
			} else {}
		}
		for (int i = 0; i < _tbNum; i++) {
		// 2. 建仓用n日最高最低点
			while (r_it->date > it_TopButtomCreate[i]->date)
				++it_TopButtomCreate[i];
			if (!TopButtomCreate_HasEnable) {
				if (r_it->date == it_TopButtomCreate[i]->date) {
					TmpCreateEnable[i] = true;
					for (int j = 0; j < _tbNum; j++) {
						if (0 == j)	TopButtomCreate_HasEnable = TmpCreateEnable[j];
						else		TopButtomCreate_HasEnable = TopButtomCreate_HasEnable && TmpCreateEnable[j];
					}
				} else {}
			}
		// 3. 平仓用n日最高最低点
			while (r_it->date > it_TopButtomLeave[i]->date)
				++it_TopButtomLeave[i];
			if (!TopButtomLeave_HasEnable) {
				if (r_it->date == it_TopButtomLeave[i]->date) {
					TmpLeaveEnable[i] = true;
					for (int j = 0; j < _tbNum; j++) {
						if (0 == j)	TopButtomLeave_HasEnable = TmpLeaveEnable[j];
						else		TopButtomLeave_HasEnable = TopButtomLeave_HasEnable && TmpLeaveEnable[j];
					}
				} else {}
			}
		}

		// U = (C * 1%) / (N * 每点价值);
		// 对于A股， it_N->price为每一股的单价波动，_pointValue = 100（1手为单位），_turtleUnit的单位为（股）
		_turtleUnit = (int)((_position.getTotal()*_riskRatio)/(it_N->price*_pointValue)+g_EPS); // 向下取整
		if (1 > _turtleUnit) INFO("date:%4d-%2d-%2d, _turtleUnit == 0\n", r_it->date.year, r_it->date.month, r_it->date.day);
// 		printf_s("raw date:%4d-%2d-%2d -> N date:%4d-%2d-%2d, N:%f, unit:%d\n", r_it->date.year, r_it->date.month, r_it->date.day, 
// 			it_N->date.year, it_N->date.month, it_N->date.day, it_N->price, _turtleUnit*100);
// 		_position.getInfo();

		// 冲突处理（交易规则）
		// 一日内顺序：1.建仓，2.平仓，3.加仓，4.止损
		// 日内有建仓||平仓||加仓，则不止损
		// 一天只做一个操作，但一天可以加几次仓
		// 总头寸规模未买时，可以按不同种类的指标可以买入

		// 交易过程
		// 建仓，平仓的n是最高最低点及TR的n日移动平均线都available，则开始交易过程
		DataEnable = N_HasEnable && TopButtomCreate_HasEnable && TopButtomLeave_HasEnable;
		if (DataEnable) {
			if (memcmp(&(it_N->date), &(r_it->date), sizeof(stockDate))) {
				ERRR("N date and Raw date not equre!\n");
			}

			// 建仓
			if (_CreatePosition(it_TopButtomCreate, *r_it, tmpTradePoint))
				goto UPDATE_CLOSE;
			// 平仓
			if (_ClearPosition(it_TopButtomLeave, *r_it, tmpTradePoint))
				goto UPDATE_CLOSE;
			// 加仓
			if (_AddPosition(it_N, *r_it, tmpTradePoint))
				goto UPDATE_CLOSE;
			// 止损
			if (_StopLoss(it_N, *r_it, tmpTradePoint))
				goto UPDATE_CLOSE;
		}
UPDATE_CLOSE:
		_position.updateTotal(_stockID, r_it->close);
	}

	delete []it_TopButtomLeave;
	delete []it_TopButtomLeave_end;
	delete []it_TopButtomLeave_begin;
	delete []TmpLeaveEnable;
	delete []it_TopButtomCreate;
	delete []it_TopButtomCreate_end;
	delete []it_TopButtomCreate_begin;
	delete []TmpCreateEnable;
	return &_tradeHistory;
}

// 只能建一个仓，若有两种指标同时满足，不能同时建两个仓
bool WayOfTurtle::_CreatePosition(vector<turtleAvgTopButtomData>::iterator *it_TopButtom, const sinaDailyData &today, TradingPoint &Trade) {
	float EntryPrice; // （单价/股）
	// 建仓条件：做多突破长期或短期n日的上轨
	if ((1 || _preBreakoutFailure) && _turtleUnit > 0) {
		// 有一个入场条件满足，就开始建仓
		for (int i = 0; i < _tbNum; i++) {
			if (!_isCreate[i] && !_position.GetPosition(_stockID, i)) {
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
					Trade.tmpData = _avgTopButtomCreate[i];
					Trade.amount = _turtleUnit*100;
					if (_position.Buy(Trade.price, Trade.amount, _stockID, i)) { // 有剩余头寸，买入成功
						// 显示
						Trade.ShowThisTradeInfo("建仓");
						_position.getInfo();
						// 记录
						_lastEntryPrice[i] = Trade.price;
						_sendOrderThisBar = true;
						_tradeHistory.push_back(Trade);
						_isCreate[i] = true;
						return true;
					} else { // 没有足够余额
						return false;
					}
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
	for (int i = 0; i < _tbNum; i++) {
		if (_isCreate[i] && _position.GetPosition(_stockID, i) > 0) {		// 这种指标可以平仓
			// 有一个离场条件满足，就全部平仓
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
				Trade.tmpData = _avgTopButtomLeave[i];
				Trade.amount = _position.getMount(_stockID);
				_position.sellAll(Trade.price, _stockID, i);
				// 显示
				Trade.ShowThisTradeInfo("平仓");
				_position.getInfo();
				// 记录
				_tradeHistory.push_back(Trade);
				_isCreate[i] = false;
				return true;
			} else {}
			return false;
		}
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
	// 不同参数，不同处理
	for (int i = 0; i < _tbNum; i++) {
		if (_isCreate[i] && _position.GetPosition(_stockID, i) > 0) {
			float AddPrice = _lastEntryPrice[i] + _addFactor*N; // （单价/股）
			// 一天可以加多次仓
			while (today.top > AddPrice && _turtleUnit > 0) {
				// 操作
				Trade.date = today.date;
				if (today.open > AddPrice) {	// 如果开盘就突破1/2N，则直接用开盘价增仓。
					Trade.price = today.open + _minPoint;
				} else {						// 正常加仓的情况
					Trade.price = AddPrice + _minPoint;
				}
				Trade.trade = BUY_UP;
				Trade.tmpData = i;
				Trade.amount = _turtleUnit*100;
				if (_position.Buy(Trade.price, Trade.amount, _stockID, i)) { // 有剩余头寸，买入成功
					// 显示
					Trade.ShowThisTradeInfo("加仓");
					_position.getInfo();
					// 记录
					_lastEntryPrice[i] = Trade.price;
					_sendOrderThisBar = true;
					_tradeHistory.push_back(Trade);
					AddPrice = _lastEntryPrice[i] + _addFactor*N; // （单价/股）
					Count++;
				} else {break;}
			}
		}
	}
	return Count;
}
bool WayOfTurtle::_StopLoss(vector<turtleAvgTRData>::iterator it_N, const sinaDailyData &today, TradingPoint &Trade) {
	if (!_HasPosition()) {
// 		ERRR("止损时没有头寸\n");
		return false;
	}
	float N = (it_N-1)->price;
	float StopLossPrice;
	for (int i = 0; i < _tbNum; i++) {
		if (_isCreate[i] && _position.GetPosition(_stockID, i) > 0) {
			StopLossPrice = _lastEntryPrice[i] - _stoplossFactor*N; // （单价/股）
			if ((today.buttom < StopLossPrice)) {
				// 操作
				Trade.date = today.date;
				if (today.open < StopLossPrice) {	// 如果开盘反射突破2N，则直接用开盘价止损
					Trade.price = today.open - _minPoint;
				} else {							// 正常平仓的情况
					Trade.price = StopLossPrice - _minPoint;
				}
				Trade.trade = STOP_LOSS_SELL_DOWN;
				Trade.tmpData = i;
				Trade.amount = _position.getMount(_stockID);
				_position.sellAll(Trade.price, _stockID, i);
				// 显示
				Trade.ShowThisTradeInfo("止损");
				_position.getInfo();
				// 记录
				_preBreakoutFailure = true;
				_tradeHistory.push_back(Trade);
				_isCreate[i] = false;
				return true;
			}
		}
	}
	return false;
}
