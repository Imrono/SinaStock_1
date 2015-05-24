#include "DataHistory.h"
#include "..//Common//GlobalParam.h"
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

vector<sinaDailyData>* DataInSeason::getDateDaily() {
	return &_dataDaily;
}
int DataInSeason::DataAnalyze(const char* rawData, stockHistoryStatus &status) {
	if (nullptr == rawData) {
		return -2;
	}
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
				for (int i = 0; i < 4; i++) {
					tmpStatus.seasons[i] = NEED_UPDATE;
				}
				status.status.push_back(tmpStatus);
				tmpYear ++;
			}
			else break;
		} while(1);
	}
	for (vector<stockStatus>::iterator it = status.status.begin(); it != status.status.end(); ++it) {
		if (it->year == _dataSeason.year) {
			if (false == it->prepare[TO_DATA(_dataSeason.season)])
				it->prepare[TO_DATA(_dataSeason.season)] = true;
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
			sinaDailyData tmpData;
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
HistoryData::HistoryData(string stockID) : _stockID(stockID), _synHttpUrl((string("History Daily Data : ") + stockID).c_str()) {
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
	sprintf_s(_urlStockID, 32, "/stockid/%s.phtml", stockID.c_str());
	sprintf_s(_urlYear,    32, "?year=%4d",         year);
	sprintf_s(_URL_StockHistory+_startLength, 256-_startLength, 
		"%s%s%s%s", _strPriceType[priceType], _urlStockID, _urlYear, _strQuarter[quarter]);
	_IsPrepare = true;
	return _URL_StockHistory;
}
vector<sinaDailyData> * HistoryData::URL2Data(int year, int quarter, string stockID, getType priceType, stockHistoryStatus &status) {
	const char* url = PrepareURL(year, quarter, stockID, priceType);
	_synHttpUrl.OpenUrl(url);

	//loop + read & analyze all data
	Write2Buffer w2b(true, MAX_RECV_BUF_SIZE);
// 	w2b.AddSearchString("<!--历史交易begin-->", "<!--历史交易end-->", HISTORY_EXCHANGE);
	w2b.AddSearchString("<!--历史交易begin-->", "<!-- 首页标准尾_END -->", HISTORY_EXCHANGE);
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
			if (w2b.getData(1))
				DYNAMIC_TRACE(PROGRESS_TRACE, "analyze daily history OK\n");
			break;
		}
	} while (1);

	if (0 > _HistoryAnalyze.DataAnalyze(w2b.getData(1)->ResultStr.c_str(), status)) {
		ERRR("_HistoryAnalyze.DataAnalyze failed!\n");
	}
	vector<sinaDailyData> *dataDaily = _HistoryAnalyze.getDateDaily();
	for (vector<stockStatus>::iterator it = status.status.begin(); it != status.status.end(); ++it) {
		if (it->year == year) {
			if (NEED_UPDATE == it->seasons[TO_DATA(quarter)] && true == it->prepare[TO_DATA(quarter)])
				it->seasons[TO_DATA(quarter)] = HAVE_UPDATED;
			else {
				ERRR("Stock history season analyze status mismatched!\n");
			}
		}
	}

	_synHttpUrl.CloseUrl();
	return dataDaily;
}
void HistoryData::StockDailyData(string stockID, getType priceType) {
	_stockID = stockID;
	IsFQ = priceType;
	stockHistoryStatus status;
	status.symbol = stockID;
	const char *pt = ::getPriceType(priceType);

	CheckAndSetFolder(status, priceType);
	char tmp[g_szPath] = {0};
	int len = sprintf_s(tmp, g_szPath, "%s\\%s", DataDir, status.symbol.c_str());
	string FilePath = tmp;
	FilePath += DailyDataDir; FilePath += "\\";
	string FileName;

	// init
	if (!status.HasDailyDataInit) {
		SYSTEMTIME lcTime;
		GetLocalTime(&lcTime);
		stockSeason lcStockData;
		stockTimeHandler::getLocalJiDu(lcStockData.year, lcStockData.season);

		vector<sinaDailyData> *dataDaily = URL2Data(lcStockData.year, TO_DISPLAY(lcStockData.season), stockID, priceType, status);
		if (0 != dataDaily->size()) {
			stockFile::SetFileNameFormate(stockID, lcStockData.year, lcStockData.season, pt, FileName);
			FileName = FilePath + FileName;
			_stkFile.open(FileName, "a+", "");
			for (vector<sinaDailyData>::iterator it = (*dataDaily).begin(); it != (*dataDaily).end(); ++it) {
				char tmp[256] = {0};
				sprintf_s(tmp, 256, "%d-%2d-%2d : %.3f,%.3f,%.3f,%.3f,%.3f,%.3f,%.3f\n", it->date.year, it->date.month, it->date.day
					, it->open, it->top, it->close, it->buttom, it->exchangeStock, it->exchangeMoney, it->factor);
				DYNAMIC_TRACE(DATA_TRACE, "%s", tmp);
				_stkFile.write(tmp, strlen(tmp));
			}
			_stkFile.close();
		}
		dataDaily->clear();

		// the feature seasons set to HAVE_UPDATED
		for (vector<stockStatus>::iterator it = status.status.begin(); it != status.status.end(); ++it) {
			if (it->year == lcStockData.year) {
				for (int i = lcStockData.season; i < 4; i++) {
					if (NEED_UPDATE == it->seasons[i]) 
						it->seasons[i] = HAVE_UPDATED;
				}
			}
		}
		status.HasDailyDataInit = true;
	}
	// get stock all data and write 2 file
	for (vector<stockStatus>::iterator it = status.status.begin(); it != status.status.end(); ++it) {
		int season;
		for (int i = 0; i < 4; i++) {
			if (NEED_UPDATE == it->seasons[i]) {
				season = i;
				vector<sinaDailyData> *dataDaily = URL2Data(it->year, TO_DISPLAY(season), stockID, priceType, status);
				if (0 != dataDaily->size()) {
					stockFile::SetFileNameFormate(stockID, it->year, season, pt, FileName);
					FileName = FilePath + FileName;
					_stkFile.open(FileName, "w", "");
					for (vector<sinaDailyData>::iterator it = (*dataDaily).begin(); it != (*dataDaily).end(); ++it) {
						char tmp[256] = {0};
						sprintf_s(tmp, 256, "%d-%2d-%2d : %.3f,%.3f,%.3f,%.3f,%.3f,%.3f,%.3f\n", it->date.year, it->date.month, it->date.day
							, it->open, it->top, it->close, it->buttom, it->exchangeStock, it->exchangeMoney, it->factor);
						DYNAMIC_TRACE(DATA_TRACE, "%s", tmp);
						_stkFile.write(tmp, strlen(tmp));
					}
					_stkFile.close();
				}
				dataDaily->clear();
			}
		}
	}
}



void HistoryData::CheckAndSetFolder(stockHistoryStatus &status, getType priceType) {
	char tmp[128] = {0};

	// check stock directory
	int len = sprintf_s(tmp, 128, "%s\\%s", DataDir, status.symbol.c_str());
	if (!_stkFile.CheckFolderExist(tmp)) {	//create direct stock @
		INFO("%s not exist! create it.\n", tmp);
		if (-1 == _mkdir(tmp)) ERRR("Make Directory \"%s\"Error!", tmp);
		status.HasDailyDataInit = false;
		status.HasOnTimeDataInit = false;
		len += sprintf_s(tmp+len, 128-len, "\\%s", DailyDataDir);
		if (-1 == _mkdir(tmp)) ERRR("Make Directory \"%s\"Error!", tmp);
		return;
	}
	const char *pt = ::getPriceType(priceType);

	// check Daily Data directory
	if (DAILY_DATA == status.HistoryType) {
		len += sprintf_s(tmp+len, 128-len, "%s", DailyDataDir);
		if (!_stkFile.CheckFolderExist(tmp)) {
			INFO("%s not exist! create it.\n", tmp);
			if (-1 == _mkdir(tmp)) ERRR("Make Directory \"%s\"Error!", tmp);
			status.HasDailyDataInit = false;
		} else {
			// if .dstk exist, only update needed
			if (!stockFile::CheckDSTKFileExist(tmp, true, pt)) 
				status.HasDailyDataInit = false;
			else { // only update needed
	/********************************************************************************/
	/* update data algorithm description
	"lcStockData" : local time's season
	"ftStockData" : latest season read from file name
	"lcTime"      : local time
	lcStockData > ftStockData : check new file + check update
	lcStockData < ftStockData : unexpected
	lcStockData = ftStockData : check update
	/********************************************************************************/
				stockSeason lcStockData;
				stockTimeHandler::getLocalJiDu(lcStockData.year, lcStockData.season);
				status.HasDailyDataInit = true;
				string path = tmp;
				string latestUpdate;
				stockFile::FindLatestDTSK(path, latestUpdate, pt);
				// get latest updated file time
				stockSeason ftStockData;
				string id;
				stockFile::GetFileNameFormate(latestUpdate, pt, ftStockData.year, ftStockData.season, id);
				latestUpdate = path + "\\" + latestUpdate;
				// compare the file write time with the local clock
				int delt_season = lcStockData - ftStockData;
				INFO("TEST: later %d\n", delt_season);
				SYSTEMTIME lcTime;
				::GetLocalTime(&lcTime);
				if (delt_season > 0) { // lcStockData > ftStockData
					// new file & add to status
					int year = ftStockData.year;
					int data_season = ftStockData.season;
					for (int i = 0; i < delt_season; i++) {
						data_season++;
						if (data_season >= 4) {
							year ++;
							data_season -= 4;
						}
						stockStatus stk_st;
						stk_st.year = year;
						stk_st.seasons[data_season] = false;
						status.status.push_back(stk_st);
						string NewFileName;
						id = status.symbol;
						stockFile::SetFileNameFormate(id, year, data_season, pt, NewFileName);
						char nFile[128] = {0};
						sprintf_s(nFile, 128, "%s//%s", tmp, NewFileName.c_str());
						if (!stockFile::createFile(nFile)) {
							ERRR("create file failed!\n");
						}
					}
					// check latest data file update
					if (CheckUpdate(lcTime, latestUpdate)) {
						stockStatus stk_st;
						stk_st.year = ftStockData.year;
						stk_st.seasons[ftStockData.season] = false;
						status.status.push_back(stk_st);
					}
				} else if (delt_season < 0) { // lcStockData < ftStockData -> impossible
					INFO("unexpected NEW data!\n");
				} else { // lcStockData = ftStockData -> check update
					if (CheckUpdate(lcTime, latestUpdate)) {
						stockStatus stk_st;
						stk_st.year = ftStockData.year;
						stk_st.seasons[ftStockData.season] = false;
						status.status.push_back(stk_st);
					}
				}
			}
		}
	}
}

void HistoryData::DailyData2File(vector<sinaDailyData> *DailyData, const string &filename) {
	for (vector<sinaDailyData>::iterator it = (*DailyData).begin(); it != (*DailyData).end(); ++it) {
		char tmp[256] = {0};
		sprintf_s(tmp, 256, "%d-%2d-%2d : %.3f,%.3f,%.3f,%.3f,%.0f,%.3f,%.3f\n", it->date.year, it->date.month, it->date.day
			, it->open, it->top, it->close, it->buttom, it->exchangeStock, it->exchangeMoney, it->factor);
		DYNAMIC_TRACE(DATA_TRACE, "%s", tmp);
		_stkFile.open(filename);
		if (_stkFile.IsFileOpened()) {
			_stkFile.write(tmp, strlen(tmp));
		}
		_stkFile.close();
	}
}

bool HistoryData::CheckUpdate(SYSTEMTIME lcTime, const string &file) {
	/********************************************************************************/
	/*lcTime : local time
	  stData : latest data time
	  ~ Don't update
	  1. stData is the last day in the season
	  2. stData later than lcTime
	  ~ Do update
	  1. lcTime later than stData, but stData is not the last day in season
	  2. lcTime - stData > threshold
	/********************************************************************************/
	// latest data time in file
	SYSTEMTIME stData;

	// month end
	FILE *fp;
	fopen_s(&fp, file.c_str(), "r");
	if (nullptr == fp) {
		ERRR ("File %s can't open!", file.c_str());
	} else {
		char tmp[128] = {0};
		fscanf_s(fp, "%[^\n]", tmp, 128);
		if (!strlen(tmp)) return true;
		sscanf_s(tmp, "%d-%d-%d", &stData.wYear, &stData.wMonth, &stData.wDay);
		stockTimeHandler::GetWeekDay(stData);
		fclose(fp);
	}
	// if file contains the data of the last day in the season, don't update it
	if (0 == stData.wMonth%3) {
		if (stockTimeHandler::IsLastWorkDayInMonth(stData))
			return false;
	}

	if (lcTime.wYear > stData.wYear || 
	  (lcTime.wYear == stData.wYear && lcTime.wMonth > stData.wMonth)) {
		  return true;
	} else if (lcTime.wYear < stData.wYear || 
		     (lcTime.wYear == stData.wYear && lcTime.wMonth < stData.wMonth)) {
		INFO("unexpected: Local Time < Data Time!\n");
		return false;
	} else {
		WORD addtionDay = 0;
		if (5 == stData.wDayOfWeek) addtionDay = 2;
		else addtionDay = 0;
		if (lcTime.wDay < stData.wDay + addtionDay + 1 ||
		  (lcTime.wDay == stData.wDay + addtionDay && lcTime.wHour < 15)) {
			return false;
		} else {
			return true;
		}
	}
}
