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
// latest weight is at head 
int analyzeDailyData::GetnDayAverage(int **avgDay, vector<averageData> *avgData, int avgNum) {
	int count = 0;

	// initial tmp structure
	if (avgNum > 32) {
		ERRR("avgNum is %d, larger than 32, modify it or change the program!\n", avgNum);
		return 0;
	}
	int tmpCount[32] = {0};
	averageData **tmpData = new averageData*[avgNum];
	for (int i = 0; i < avgNum; i++) {
		tmpData[i] = new averageData[avgDay[i][0]];
		for (int j = 0; j < avgDay[i][0]; j++) {
			memset(tmpData[i], 0, sizeof(averageData)*avgDay[i][0]);
		}
		tmpCount[i] = avgDay[i][0];
	}

	// main analyze procedure
	int ix = 0, tmpDays = 0;
	for (vector<averageData>::iterator it = _vecTmpDailyData.begin(); it != _vecTmpDailyData.end(); ++it) {
		for (int i = 0; i < avgNum; i++) {
			tmpDays = avgDay[i][0];
			ix = i%tmpDays;
			if (count < avgDay[i][0]) {
				// average data = weight1*data1 + weight2*data2 + ...
// 				tmpData[i][ix] += avgDay[i][ix+1] * ;
				// factor ix%3+y
				// write when = days-1 then clean
			} else {

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