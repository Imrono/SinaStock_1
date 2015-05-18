#include "analyzeDailyData.h"

analyzeDailyData::~analyzeDailyData() {
	ClearDailyData();
}
void analyzeDailyData::ResetStatus() {
	_vecTmpDailyData.clear();
	_stockID.clear();
	_stkFile.close();
}

// For signal record file, this function should looped
// [*]recent -> previous
vector<averageData> *analyzeDailyData::GetDailyDataFromFile(int year, int data_Jidu, getType priceType) {
	const char *tp = ::getPriceType(priceType);
	string fileName;
	stockFile::SetFileNameFormate(_stockID, year, data_Jidu, tp, fileName);

	if (_stkFile.open(fileName, "r", "")) {
		const int sizeBuffer = 256;
		char tmpBuffer[sizeBuffer] = {0};
		averageData it;
		while (NULL != _stkFile.readline(tmpBuffer, sizeBuffer)) {
			sscanf_s(tmpBuffer, "%d-%2d-%2d : %*f,%*f,%.3f,%*f,%.0f,%.3f,%.3f\n", it.date.year, it.date.month, it.date.day
				, it.close, it.exchangeStock, it.exchangeMoney, it.factor);
			_vecTmpDailyData.push_back(it);
		}
		_stkFile.close();
	}
	DYNAMIC_TRACE(PROGRESS_TRACE, "$%s$%d年%d季度，数据抽取完成\n", _stockID.c_str(), year, TO_DISPLAY(data_Jidu));
	return &_vecTmpDailyData;
}
// [IN] avgDay: the first col is average days, the following cols are weights
// [*]weight is from previous to recent
int analyzeDailyData::GetnDayAverage(int *avgDay, float **avgWeight, vector<averageData> *avgData, int avgNum) {
	int count = 0;
	// initial tmp structure
	if (avgNum > 32) {
		ERRR("avgNum is %d, larger than 32, modify it or change the program!\n", avgNum);
		return 0;
	}
	averageData **tmpData = new averageData*[avgNum];
	for (int i = 0; i < avgNum; i++) {
		tmpData[i] = new averageData[avgDay[i]];
		for (int j = 0; j < avgDay[i]; j++) {
			memset(tmpData[i], 0, sizeof(averageData)*avgDay[i]);
		}
	}
	// main analyze procedure
	int idx_tmp = 0, idx_wt = 0;
	// loop each raw data
	// [*]from recent to previous such as 2015 -> 1998
	for (vector<averageData>::iterator it = _vecTmpDailyData.begin(); it != _vecTmpDailyData.end(); ++it) {
		// for average type, such as 5, 10, 15 days
		for (int i = 0; i < avgNum; i++) {
			idx_tmp = count%avgDay[i];
			tmpData[i][idx_tmp].clear(); // ready for record
			// for days in one average type
			for (int j = 0; j < avgDay[i]; j++) {
				idx_wt = (idx_tmp - j)%avgDay[i];
				// average data = weight1*data1 + weight2*data2 + ...
				tmpData[i][idx_tmp] = tmpData[i][idx_tmp] + ((*it)*avgWeight[i][idx_wt]);
				if (avgDay[i]-1 == idx_wt) {
					// if count less than needed average days, ignore it because it's incomplete
					if (count >= avgDay[i]-1)
						// record average data
						avgData[i].push_back(tmpData[i][idx_tmp]);
				}
			}
		}
		count ++;
	}
	// clean temp data
	for (int i = 0; i < avgNum; i++) {
		delete []tmpData[i];
	}
	delete []tmpData;

	return count;
}

// Extraction all data of one stock
void analyzeDailyData::ExtractionData(getType priceType) {
	char tmp[128] = {0};
	// check stock directory and files
	int len = sprintf_s(tmp, 128, "%s\\%s", DataDir, _stockID.c_str());
	if (!_stkFile.CheckFolderExist(tmp)) {	// stock directory not exist
		ERRR("\"%s\" not exist! Extraction data failed!\n", tmp);
		return;
	}
	len += sprintf_s(tmp+len, 128-len, "%s", DailyDataDir);
	if (!_stkFile.CheckFolderExist(tmp)) {	// daily directory not exist
		ERRR("\"%s\" not exist! Extraction data failed!\n", tmp);
		return;
	}
	const char *pt = ::getPriceType(priceType);
	if (!stockFile::CheckDSTKFileExist(tmp, true, pt)) {
		ERRR("\"%s\" contains no %s file! Extraction data failed!\n", tmp, pt);
		return;
	}
	// get file information to status
	stockHistoryStatus status;
	vector<string> files;
	stockFile::getFiles(tmp, files);
	int year = 0, data_Season = 0;
	int maxYear = 0, maxSeason = 0;
	string id;
	// For all files of one stock
	// [*]recent -> previous
	for (vector<string>::iterator it = files.begin(); it != files.end(); ++it) {
		// get latest file
		stockFile::GetFileNameFormate(it->c_str(), pt, year, data_Season, id);
		if (year > maxYear) {
			maxYear = year;
			maxSeason = data_Season;
		} else if (year == maxYear) {
			maxSeason = maxSeason >= data_Season ? maxSeason : data_Season;
		} else {}
		// list data from file to vector
		GetDailyDataFromFile(year, data_Season, priceType);
		DYNAMIC_TRACE(PROGRESS_TRACE, "get \"%s\" data to vector\n", it->c_str());

		files.erase(it);
	}

}
