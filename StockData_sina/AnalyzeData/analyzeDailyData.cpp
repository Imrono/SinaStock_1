#include "analyzeDailyData.h"

analyzeDailyData::~analyzeDailyData() {
	ClearDailyData();
}
void analyzeDailyData::ResetStatus() {
	_vecTmpDailyData.clear();
	_stockID.clear();
	_file.close();
}

vector<averageData> *analyzeDailyData::GetDailyDataFromFile(int year, int data_Jidu, getType priceType) {
	const char *tp = ::getPriceType(priceType);
	string fileName;
	stockFile::SetFileNameFormate(_stockID, year, data_Jidu, tp, fileName);

	if (_file.open(fileName, "r", "")) {
		const int sizeBuffer = 256;
		char tmpBuffer[sizeBuffer] = {0};
		averageData it;
		while (NULL != _file.readline(tmpBuffer, sizeBuffer)) {
			sscanf_s(tmpBuffer, "%d-%2d-%2d : %*f,%*f,%.3f,%*f,%.0f,%.3f,%.3f\n", it.date.year, it.date.month, it.date.day
				, it.close, it.exchangeStock, it.exchangeMoney, it.factor);
			_vecTmpDailyData.push_back(it);
		}
		_file.close();
	}
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
	int tmpCount[32] = {0};
	averageData **tmpData = new averageData*[avgNum];
	for (int i = 0; i < avgNum; i++) {
		tmpData[i] = new averageData[avgDay[i]];
		for (int j = 0; j < avgDay[i]; j++) {
			memset(tmpData[i], 0, sizeof(averageData)*avgDay[i]);
		}
		tmpCount[i] = avgDay[i];
	}

	// main analyze procedure
	int idx_tmp = 0, idx_wt = 0;
	// for raw data, [*]from previous to recent such as 1998 -> 2015
	for (vector<averageData>::iterator it = _vecTmpDailyData.begin(); it != _vecTmpDailyData.end(); ++it) {
		// for average type, such as 5, 10, 15 days
		for (int i = 0; i < avgNum; i++) {
			idx_tmp = i%avgDay[i];
			// for days in one average type
			for (int j = 0; j < avgDay[i]; j++) {
				idx_wt = (idx_tmp + j)%avgDay[i];
				// average data = weight1*data1 + weight2*data2 + ...
				tmpData[i][idx_tmp] = tmpData[i][idx_tmp] + ((*it)*avgWeight[i][idx_wt]);
				if (0 == idx_wt) {
					// if count less than needed average days, ignore it because it's incomplete
					if (count >= avgDay[i])
						// record average data
						avgData[i].push_back(tmpData[i][idx_tmp]);
					tmpData[i][idx_tmp].clear();
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