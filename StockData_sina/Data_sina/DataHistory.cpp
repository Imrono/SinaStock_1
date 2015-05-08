#include "DataHistory.h"
#include "..//Common//Write2Buffer.h"
#include "..//Common//TraceMicro.h"
#include "..//Common//stockTime.h"
#include <direct.h>

stockHistoryStatus::stockHistoryStatus() {
	HasDailyDataInit = false;
	NeedDailyDataUpdate = true;
	HasOnTimeDataInit = false;
	HistoryType = DAILY_DATA;
}

////////////////////////////////////////////////////////////////////////////
DataInSeason::DataInSeason() {}
DataInSeason::~DataInSeason() {
	_dataDaily.clear();
}

vector<DataOfDay>* DataInSeason::getDateDaily() {
	return &_dataDaily;
}
int DataInSeason::DataAnalyze(const char* rawData, stockHistoryStatus &status) {
	const char* tmp = nullptr;
	bool HasFactor = false;
	const char* endCheck = nullptr;
	int count = 0;
	// check begin
	if (nullptr == (tmp = strstr(rawData, "<!--历史交易begin-->")))
		return -1;
	endCheck = tmp;
	const char* tmpYearBegin = nullptr, *tmpYearEnd = nullptr;
	// get year
	if (nullptr != (tmp = strstr(rawData, "<select name=\"year\">"))) {
		tmpYearBegin = tmp;
		if (nullptr != (tmp = strstr(tmp, "selected")))
			sscanf_s(tmp, "selected>%d</option>", &_dataSeason.year);
		else return -1;
	}
	else return -1;

	// get season
	if (nullptr != (tmp = strstr(tmp, "jidu"))) {
		tmpYearEnd = tmp;
		if (nullptr != (tmp = strstr(tmp, "selected")))
			sscanf_s(tmp-18, "<option value=\"%d\" selected>", &_dataSeason.season);
		else return -1;
	}
	else return -1;

	stockStatus tmpStatus;
	if (!status.HasDailyDataInit) {
		if (status.status.size() != 0) {
			ERRR("Stock history initial status mismatched!\n");
			return -1;
		}
		const char* tmpYear = tmpYearBegin;
		do {
			tmpYear = strstr(tmpYear, "<option value=");
			if (nullptr != tmpYear && tmpYear < tmpYearEnd) {
				sscanf_s(tmpYear, "<option value=\"%d\"", &tmpStatus.year);
				status.status.push_back(tmpStatus);
				tmpYear ++;
			}
			else break;
		} while(1);
	}
	for (vector<stockStatus>::iterator it = status.status.begin(); it != status.status.end(); ++it) {
		if (it->year == _dataSeason.year) {
			if (false == it->prepare[_dataSeason.season-1])
				it->prepare[_dataSeason.season-1] = true;
			else {
				ERRR("Stock history season prepare status mismatched!\n");
				return -1;
			}
		}
	}

	if (nullptr != strstr(tmp, "复权因子"))
		HasFactor = true;

	// get daily date
	if (nullptr != (tmp = strstr(tmp, "<table id=\"FundHoldSharesTable\">"))) {
		const char* tmpTable = strstr(tmp, "</table>");
		while (nullptr != (tmp = strstr(tmp, "<a target='_blank' href=")) && tmp < tmpTable) {
			endCheck = tmp;
			DataOfDay tmpData;
			if (nullptr != (tmp = strstr(tmp, "'http://"))) {
				const char* startURL = strstr(tmp, "'");
				const char* endURL = strstr(startURL+1, "'");
				tmpData.DetailWeb.append(startURL+1, endURL);
				if (nullptr != (tmp = strstr(endURL, "'>"))) {
					sscanf_s(tmp, "'>%d-%d-%d</a>", &tmpData.date.year, &tmpData.date.month, &tmpData.date.day);
				}
				else return -1;
			}
			else return -1;

			tmp = strstr(tmp, "<td><div align=\"center\">");
			if (!HasFactor) {
				sscanf_s(tmp, 
					"<td><div align=\"center\">%f</div></td>\t"\
					"<td><div align=\"center\">%f</div></td>\t"\
					"<td><div align=\"center\">%f</div></td>\t"\
					"<td class=\"tdr\"><div align=\"center\">%f</div></td>\t"\
					"<td class=\"tdr\"><div align=\"center\">%f</div></td>\t"\
					"<td class=\"tdr\"><div align=\"center\">%f</div></td>\t"
					, &tmpData.open, &tmpData.top, &tmpData.close, &tmpData.buttom
					, &tmpData.exchangeStock, &tmpData.exchangeMoney);
				tmpData.factor = 1.0;
			}
			else {
				sscanf_s(tmp, 
					"<td><div align=\"center\">%f</div></td>\t"\
					"<td><div align=\"center\">%f</div></td>\t"\
					"<td><div align=\"center\">%f</div></td>\t"\
					"<td class=\"tdr\"><div align=\"center\">%f</div></td>\t"\
					"<td class=\"tdr\"><div align=\"center\">%f</div></td>\t"\
					"<td class=\"tdr\"><div align=\"center\">%f</div></td>\t"\
					"<td class=\"tdr\"><div align=\"center\">%f</div></td>\t"
					, &tmpData.open, &tmpData.top, &tmpData.close, &tmpData.buttom
					, &tmpData.exchangeStock, &tmpData.exchangeMoney, &tmpData.factor);
			}
			_dataDaily.push_back(tmpData);
			count++;
		}
		return count;
	}
	else return -1;
}

HistoryData::HistoryData() : _synHttpUrl("HISTORY") {
	if (!stockFile::IsAccessable(DataDir))
		_mkdir(DataDir);

// 	char *tmp = "http://money.finance.sina.com.cn/corp/go.php/vMS_MarketHistory/stockid/xxxxxx.phtml?year=xxxx&jidu=x";
	strcpy_s(_URL_StockHistory, 256, "http://money.finance.sina.com.cn/corp/go.php");
	_startLength = strlen(_URL_StockHistory);

	_strPriceType[FUQUAN] = "/vMS_FuQuanMarketHistory";
	_strPriceType[NO_FUQUAN] = "/vMS_MarketHistory";

	_strQuarter[1] = "&jidu=1";
	_strQuarter[2] = "&jidu=2";
	_strQuarter[3] = "&jidu=3";
	_strQuarter[4] = "&jidu=4";

	_IsPrepare = false;
}
HistoryData::~HistoryData() {
	_IsPrepare = false;
}

const char* HistoryData::PrepareURL(int year, int quarter, string stockID, getType priceType) {
	sprintf_s(_strStockID, 32, "/stockid/%s.phtml", stockID.c_str());
	sprintf_s(_strYear,    32, "?year=%4d",         year);
	sprintf_s(_URL_StockHistory+_startLength, 256-_startLength, 
		"%s%s%s%s", _strPriceType[priceType], _strStockID, _strYear, _strQuarter[quarter]);
	_IsPrepare = true;
	return _URL_StockHistory;
}
vector<DataOfDay> * HistoryData::URL2Data(int year, int quarter, string stockID, getType priceType, stockHistoryStatus &status) {
	const char* url = PrepareURL(year, quarter, stockID, priceType);
	_synHttpUrl.OpenUrl(url);

	//loop + read & analyze all data
	Write2Buffer w2b(true, MAX_RECV_BUF_SIZE);
	w2b.AddSearchString("<!--历史交易begin-->", "<!--历史交易end-->", HISTORY_EXCHANGE);
	int len = 0;
	char* buf;
	int ReadTimes = 0;
	do {
		buf = w2b.getBuffer4Write(len);
		len = MAX_RECV_BUF_SIZE > len ? len : MAX_RECV_BUF_SIZE;
		_synHttpUrl.ReadUrlOne(buf, len);
		if (len) {
			w2b.updateAfterWrite(len);
			ReadTimes++;
		}
		else {
			if (w2b.getData(1)) {
				DYNAMIC_TRACE(PROGRESS_TRACE, "analyze daily history OK\n");
				break;
			}
			else ERRR("Unexpected end when analyze daily history data\n");
		}
	} while (1);

	_HistoryAnalyze.DataAnalyze(w2b.getData(1)->ResultStr.c_str(), status);
	vector<DataOfDay> *dataDaily = _HistoryAnalyze.getDateDaily();
	for (vector<stockStatus>::iterator it = status.status.begin(); it != status.status.end(); ++it) {
		if (it->year == year) {
			if (false == it->seasons[quarter-1] && true == it->prepare[quarter-1])
				it->seasons[quarter-1] = true;
			else {
				ERRR("Stock history season analyze status mismatched!\n");
			}
		}
	}

	_synHttpUrl.CloseUrl();
	return dataDaily;
}

void HistoryData::CheckAndSetFolder(stockHistoryStatus &status) {
	char tmp[128] = {0};
	int len = sprintf_s(tmp, 128, "%s\\%s", DataDir, status.symbol.c_str());
	if (!stkFile.CheckFolderExist(tmp)) {	//create direct stock @
		_mkdir(tmp);
		status.HasDailyDataInit = false;
		status.HasOnTimeDataInit = false;
	}
	if (DAILY_DATA == status.HistoryType) {
		len += sprintf_s(tmp+len, 128-len, "\\%s", DailyDataDir);
		if (!stkFile.CheckFolderExist(tmp))
			_mkdir(tmp);
			status.HasDailyDataInit = false;
	}
	// check latest data & local time 4 latest request
	int localJidu = stockTimeHandler::getLocalJiDu();
	int localYear = stockTimeHandler::getLocalDate().tm_year;
	len += sprintf_s(tmp+len, 128-len, "\\%d.sd", _HistoryAnalyze.getDataSeason().year);
	if (!stockFile::IsAccessable(tmp))	status.HasDailyDataInit = false;
	else {
		status.HasDailyDataInit = true;
	}

}

void HistoryData::DailyData2File(vector<DataOfDay> *DailyData) {

	for (vector<DataOfDay>::iterator it = (*DailyData).begin(); it != (*DailyData).end(); ++it) {
		char tmp[256] = {0};
		sprintf(tmp, "%d-%2d-%2d : %.3f,%.3f,%.3f,%.3f,%.0f,%.3f,%.3f\n", it->date.year, it->date.month, it->date.day
			, it->open, it->top, it->close, it->buttom, it->exchangeStock, it->exchangeMoney, it->factor);
		DYNAMIC_TRACE(DATA_TRACE, "%s", tmp);
		stkFile.open("eee");
		if (stkFile.IsFileOpened()) {
			stkFile.write(tmp, strlen(tmp));
		}
		stkFile.close();
	}
}
