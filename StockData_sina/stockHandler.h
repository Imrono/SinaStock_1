#include <vector>
#include <map>
using namespace std;

#include "Common//TraceMicro.h"
#include "Common//stock2fpTable.h"
#include "Data_sina//DataHistory.h"

class StockHandler
{
public:
	void AddMyStock(string id, bool add2file = false);
	void AddMyStockFromFile();

private:
	stockFile _MyStockFile;
	map<string, HistoryData> mapHistoryData;
};