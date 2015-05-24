#include "stockHandler.h"

StockHandler::StockHandler() {
	char filepath[128] = {0};
	int len = sprintf_s(filepath, 128, "%s\\%s", DataDir, "MyStocks.status");
	_filepath = filepath;
	stockFile::createFile(_filepath.c_str());
}
StockHandler::~StockHandler() {
	for (map<string, HistoryData*>::iterator iter = _mapHistoryData.begin(); iter != _mapHistoryData.end(); ++iter) {
		delete iter->second;
	}
}
void StockHandler::AddMyStock(string id, bool add2file) {
	map<string, HistoryData*>::iterator iter = _mapHistoryData.find(id);
	if (iter == _mapHistoryData.end()) {
		HistoryData *tmp = new HistoryData(id);
		_mapHistoryData[id] = tmp;

		if (add2file) {
			_MyStockFile.open(_filepath.c_str(), "a+", "");
			char tmpBuf[32] = {0};
			sprintf_s(tmpBuf, 32, "$%s$\n", id.c_str());
			_MyStockFile.write(tmpBuf, strlen(tmpBuf));
			_MyStockFile.close();
		}
	}
	else INFO("%s has already added\n", id.c_str());
}

void StockHandler::AddMyStockFromFile() {
	char tmp[128] = {0};
	int len = sprintf_s(tmp, 128, "%s\\%s", DataDir, "MyStocks.status");
	_MyStockFile.open(tmp, "r", "");
	do {
		char fileTmp[128] = {0};
		char tmpID[32] = {0};
		if (nullptr == _MyStockFile.readline(fileTmp, 128)) break;
		if (strstr(fileTmp, "$"))	sscanf_s(fileTmp, "$%[^$]", tmpID, 32);
		AddMyStock(tmpID, false);
	} while (1);
	_MyStockFile.close();
}

int StockHandler::UpdateAllStock() {
	int count = 0;
	for (map<string, HistoryData*>::iterator iter = _mapHistoryData.begin(); iter != _mapHistoryData.end(); ++iter) {
		INFO("Now get %s's Daily History Data\n", iter->first.c_str());
		iter->second->StockDailyData(iter->first, NO_FUQUAN);
		DYNAMIC_TRACE(PROGRESS_TRACE, "%s update finished!\n", iter->first.c_str());
		count ++;
	}
	INFO("GR8, Update Finished!!\n");
	return count;
}

