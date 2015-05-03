#include "DataHistory.h"

DataInSeason::DataInSeason() {}
DataInSeason::~DataInSeason() {
	_dataDaily.clear();
}

vector<DataOfDay>* DataInSeason::getDateDaily() {
	return &_dataDaily;
}
int DataInSeason::DataAnalyze(const char* rawData) {
	const char* tmp = nullptr;
	int count = 0;
	// get year
	if (nullptr != (tmp = strstr(rawData, "year")))
		if (nullptr != (tmp = strstr(tmp, "selected")))
			sscanf_s(tmp, "selected>%d</option>", &_dataSeason.year);
		else return -1;
	else return -1;

	// get season
	if (nullptr != (tmp = strstr(tmp, "jidu")))
		if (nullptr != (tmp = strstr(tmp, "selected")))
			sscanf_s(tmp-18, "<option value=\"%d\" selected>", &_dataSeason.season);
		else return -1;
	else return -1;

	// get daily date
	if (nullptr != (tmp = strstr(tmp, "<table id=\"FundHoldSharesTable\">"))) {
		const char* tmpTable = strstr(tmp, "</table>");
		while (nullptr != (tmp = strstr(tmp, "<a target='_blank' href=")) && tmp < tmpTable) {
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
			sscanf_s(tmp, 
				"<td><div align=\"center\">%f</div></td>\t"\
				"<td><div align=\"center\">%f</div></td>\t"\
				"<td><div align=\"center\">%f</div></td>\t"\
				"<td class=\"tdr\"><div align=\"center\">%f</div></td>\t"\
				"<td class=\"tdr\"><div align=\"center\">%d</div></td>\t"\
				"<td class=\"tdr\"><div align=\"center\">%d</div></td>\t"
				, &tmpData.open, &tmpData.top, &tmpData.close, &tmpData.buttom
				, &tmpData.exchangeStock, &tmpData.exchangeMoney);
			_dataDaily.push_back(tmpData);
			count++;
		}
		return count;
	}
	else return -1;
}

HistoryURL::HistoryURL() : _synHttpUrl("HISTORY") {

// 	char *tmp = "http://money.finance.sina.com.cn/corp/go.php/vMS_MarketHistory/stockid/xxxxxx.phtml?year=xxxx&jidu=x";
	strcpy_s(_URL_StockHistory, 256, "http://money.finance.sina.com.cn/corp/go.php");
	_startLength = strlen(_URL_StockHistory);

	_strPriceType[FUQUAN] = "/vMS_FuQuanMarketHistory";
	_strPriceType[NO_FUQUAN] = "/vMS_MarketHistory";

	_strQuarter[1] = "&jidu=1";
	_strQuarter[2] = "&jidu=2";
	_strQuarter[3] = "&jidu=3";
	_strQuarter[4] = "&jidu=4";
}
HistoryURL::~HistoryURL() {
}

const char* HistoryURL::PrepareURL(int year, int quarter, string stockID, getType priceType) {
	sprintf_s(_strStockID, 32, "/stockid/%s.phtml", stockID.c_str());
	sprintf_s(_strYear,    32, "?year=%4d",          year);
	sprintf_s(_URL_StockHistory+_startLength, 256-_startLength, 
		"%s%s%s%s", _strPriceType[priceType], _strStockID, _strYear, _strQuarter[quarter]);
	return _URL_StockHistory;
}
