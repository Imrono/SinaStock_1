#include "stockHandler.h"


void StockHandler::AddMyStock(string id, bool add2file) {
	HistoryData tmp(id);
	if (mapHistoryData.find(id) == mapHistoryData.end()) {
		mapHistoryData[id] = tmp;

		if (add2file) {
			char filepath[128] = {0};
			int len = sprintf_s(filepath, 128, "%s\\%s", DataDir, "MyStocks.status");
			_MyStockFile.open(filepath, "w+", "");
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
	_MyStockFile.open(tmp, "w+", "");
	do {
		char fileTmp[128] = {0};
		char tmpID[32] = {0};
		_MyStockFile.readline(fileTmp, 128);
		if (strstr(fileTmp, "$"))	sscanf_s(fileTmp, "$%[^$]", tmpID, 128);
		AddMyStock(tmpID, false);
	} while (1);
	_MyStockFile.close();
}
