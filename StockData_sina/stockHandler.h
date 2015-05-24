#include <vector>
#include <map>
using namespace std;

#include "Common//TraceMicro.h"
#include "Common//stock2fpTable.h"
#include "Data_sina//DataHistory.h"

class StockHandler
{
public:
	StockHandler();
	~StockHandler();
	void AddMyStock(string id, bool add2file = false);
	void AddMyStockFromFile();
	int UpdateAllStock();

private:
	stockFile _MyStockFile;
	map<string, HistoryData*> _mapHistoryData;
	map<string, string> _test;

	string _filepath;
};