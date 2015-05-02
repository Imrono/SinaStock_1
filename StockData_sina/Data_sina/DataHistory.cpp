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