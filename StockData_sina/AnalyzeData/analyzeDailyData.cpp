#include "analyzeDailyData.h"

analyzeDailyData::~analyzeDailyData() {
	ClearDailyData();
}
void analyzeDailyData::ResetStatus() {
	_vecTmpDailyDataAvg.clear();
	_stockID.clear();
	_stkFile.close();
}

// For signal record file, this function should looped
// [*]recent -> previous
vector<averageData> *analyzeDailyData::GetDailyDataFromFile(int year, int data_Jidu, getType priceType, int ExtractionType) {
	const char *tp = ::getPriceType(priceType);
	string fileName;
	stockFile::SetFileNameFormate(_stockID, year, data_Jidu, tp, fileName);
	string pathName = DataDir + string("\\") + _stockID + DailyDataDir + string("\\") + fileName;

	int lines = 0;
	if (_stkFile.open(pathName, "r", "")) {
		const int sizeBuffer = 256;
		char tmpBuffer[sizeBuffer] = {0};
		averageData it;
		memset(&it, 0, sizeof(averageData));
		while (NULL != _stkFile.readline(tmpBuffer, sizeBuffer)) {
			sscanf_s(tmpBuffer, "%d-%d-%d : %*f,%*f,%f,%*f,%f,%f,%f\n", &it.date.year, &it.date.month, &it.date.day
				, &it.close, &it.exchangeStock, &it.exchangeMoney, &it.factor);
			_vecTmpDailyDataAvg.push_back(it);
			memset(&it, 0, sizeof(averageData));
			lines ++;
		}
		_stkFile.close();
	} else {
		ERRR("open \"%s\" failed!\n", fileName.c_str());
	}
	DYNAMIC_TRACE(PROGRESS_TRACE, "$%s$%d年%d季度，数据抽取完成 lines:%d\n", _stockID.c_str(), year, TO_DISPLAY(data_Jidu), lines);
	return &_vecTmpDailyDataAvg;
}
// [IN] avgDay: the first col is average days, the following cols are weights
// [*]weight is from recent to previous
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
	/********************************************************************************/
	/* average algorithm description
	/* data: 5,4,3,2,1,... weight:0.5,0.3,0.2 avgDay[i]:3 (the ith calculation)
     --------------------------------------------------------------------------------
       count   idx_tmp       tmpData[i][0]      tmpData[i][1]      tmpData[i][2]
                                clear()
         0        0             5*0.5(0)           5*0.2(2)           5*0.3(1)
     --------------------------------------------------------------------------------
                                                   clear()
         1        1          5*0.5+4*0.3(1)        4*0.5(0)        5*0.3+4*0.2(2)
     --------------------------------------------------------------------------------
                                                                      clear()
         2        2       5*0.5+4*0.3+3*0.2(2)   4*0.5+3*0.3(1)       3*0.5(0)
                              push_back(0)
     --------------------------------------------------------------------------------
                                clear()
         3        0             2*0.5(0)      4*0.5+3*0.3+2*0.2(2)  3*0.5+2*0.3(1)
                                                  push_back(1)
     --------------------------------------------------------------------------------
                                                    clear()
         4        1          2*0.5+1*0.3(1)         1*0.5(0)      3*0.5+2*0.3+1*0.2(2)
                                                                      push_back(2)
     --------------------------------------------------------------------------------
       count count%avgDay[i]    tmpData[i][j] += avgData[i][count]*((idx_tmp-j)%avgDay[i])
    idx_tmp.clear()
    if (idx_tmp >= avgDay[i]-1) push_back((idx_tmp+1)%avgDay[i])
	/********************************************************************************/
	// main analyze procedure
	int idx_tmp = 0, idx_wt = 0;
	// loop each raw data
	// [*]from recent to previous such as 2015 -> 1998
	for (vector<averageData>::iterator it = _vecTmpDailyDataAvg.begin(); it != _vecTmpDailyDataAvg.end(); ++it) {
		// for average type, such as 5, 10, 15 days
		for (int i = 0; i < avgNum; i++) {
			idx_tmp = count%avgDay[i];
			tmpData[i][idx_tmp].clear(); // ready for record
			// for days in one average type
			for (int j = 0; j < avgDay[i]; j++) {
				idx_wt = (idx_tmp - j + avgDay[i])%avgDay[i];
				// average data = weight1*data1 + weight2*data2 + ...
				tmpData[i][j] = tmpData[i][j] + ((*it)*avgWeight[i][idx_wt]);
				if (avgDay[i]-1 == idx_wt) {
					// if count less than needed average days, ignore it because it's incomplete
					if (count >= avgDay[i]-1)
						// record average data
						avgData[i].push_back(tmpData[i][(idx_tmp+1)%avgDay[i]]);
				}
			}
		}
		count ++;
		printf_s("%5d,%5d\r", avgData[0].size(), avgData[1].size());
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
	string id;
	// For all files of one stock
	// [*]recent -> previous
	while (files.size()) {
		int year = 0, data_Season = 0;
		int maxYear = 0, maxSeason = 0;
		vector<string>::iterator it_latest;
		for (vector<string>::iterator it = files.begin(); it != files.end(); ++it) {
			// get latest file
			stockFile::GetFileNameFormate(it->c_str(), pt, year, data_Season, id);
			if (year > maxYear) {
				maxYear = year;
				maxSeason = data_Season;
				it_latest = it;
			} else if (year == maxYear) {
				if (maxSeason < data_Season) {
					maxSeason = data_Season;
					it_latest = it;
				}
			} else {}
		}
		// list data from file to vector
		GetDailyDataFromFile(year, data_Season, priceType);
		DYNAMIC_TRACE(PROGRESS_TRACE, "\"%s\" data get finish!\n", it_latest->c_str());
		files.erase(it_latest);
	}
}

/********************************************************************************/
/* TR (True Range) = max(H-L,H-PDC,PDC-L)
   PDC is yesterday's close
   N is TR's 20 days average
   ATR : average TR
   N = (19×PDN+TR)/20  PDN : yesterday's N
/********************************************************************************/
double analyzeDailyData::turtleAnalyze(stockDate start, stockDate end, int ATRdays, double unit) {
	struct AvgTureRange { float ATR; stockDate date;};

	return 0.0;
}
