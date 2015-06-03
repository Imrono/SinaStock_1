#include "analyzeDailyData.h"

analyzeDailyData::~analyzeDailyData() {
	ClearDailyData();
}
void analyzeDailyData::ResetStatus() {
	_vecExtractData.clear();
	_stockID.clear();
	_stkFile.close();
}

// Extract signal record file, this function should be looped
// [*]recent -> previous
vector<sinaDailyData> *analyzeDailyData::_GetDailyDataFromFile(int year, int data_Jidu, getType priceType) {
	const char *tp = ::getPriceType(priceType);
	string fileName;
	stockFile::SetFileNameFormate(_stockID, year, data_Jidu, tp, fileName);
	string pathName = DataDir + string("\\") + _stockID + DailyDataDir + string("\\") + fileName;

	int lines = 0;
	if (_stkFile.open(pathName, "r", "")) {
		const int sizeBuffer = 256;
		char tmpBuffer[sizeBuffer] = {0};
		sinaDailyData it;
		memset(&it, 0, sizeof(sinaDailyData));
		while (NULL != _stkFile.readline(tmpBuffer, sizeBuffer)) {
			sscanf_s(tmpBuffer, "%d-%d-%d : %f,%f,%f,%f,%f,%f,%f\n", &it.date.year, &it.date.month, &it.date.day
				, &it.open, &it.top, &it.close, &it.buttom, &it.exchangeStock, &it.exchangeMoney, &it.factor);
			_vecExtractData.push_back(it);
			memset(&it, 0, sizeof(sinaDailyData));
			lines ++;
		}
		_stkFile.close();
	} else {
		ERRR("open \"%s\" failed!\n", fileName.c_str());
	}
	DYNAMIC_TRACE(PROGRESS_TRACE, "$%s$%d年%d季度，数据抽取完成 lines:%d\n", _stockID.c_str(), year, TO_DISPLAY(data_Jidu), lines);
	return &_vecExtractData;
}

// Extraction all data of one stock
// record in private member "_vecExtractData"
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
		_GetDailyDataFromFile(year, data_Season, priceType);
		DYNAMIC_TRACE(PROGRESS_TRACE, "\"%s\" data get finish!\n", it_latest->c_str());
		files.erase(it_latest);
	}
}

// averaget //////////////////////////////////////////////////////////////////////
void analyzeDailyData::averageAnalyze(int *avgDay, float **avgWeight, vector<averageData> *avgData, int avgNum) {
	_average.GetnDayAverage(_vecExtractData, avgDay, avgWeight, avgData, avgNum);
}

// turtle ////////////////////////////////////////////////////////////////////////
/********************************************************************************/
/* TR (True Range) = max(H-L,H-PDC,PDC-L)
   PDC is yesterday's close
   N is TR's 20 days average
   ATR : average TR
   N = (19×PDN+TR)/20  PDN : yesterday's N
/********************************************************************************/
double analyzeDailyData::turtleAnalyze(stockDate start, stockDate end, int ATRdays, double unit, getType priceType) {
	struct AvgTureRange { float ATR; stockDate date;};
	if (0 == _vecExtractData.size())
		ExtractionData(priceType);

	int avgDay[2] = {20,55};
// 	vector<turtleATRData> N[2];
// 	_turtle.SetNandTopBottom(*getExtractData(), avgDay, 2);
	return 0.0;
}
//////////////////////////////////////////////////////////////////////////