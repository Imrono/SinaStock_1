#include <map>
#include <string>
using namespace std;
#include "..//Common//TraceMicro.h"
#include "PositionBase.h"
// #include "..//Common//stockData.h"

class RecordPosition
{
public:
	RecordPosition();

	// 增加资金
	float Add (float Money);
	// 减少资金
	float Sub(float money);

	// 交易
	bool Buy(float Price, int Position, string StockId);
	bool Sell(float Price, int Position, string StockId);

	// 得到状态
	bool getStockMount   (string StockId, int   &Mount);
	bool getStockKeeps   (string StockId, float &Keeps);
	bool updateStockKeeps(string StockId, float  Price);

	inline float getTotal()  {return _total;}
	inline float getKeeps()  {return _keeps;}
	inline float getRemain() {return _remain;}

	// 更新总资产
	void UpdateTotal();
	// 更新部市值
	void UpdateData();

private:
	float _getPrice(string StockId);
	bool _checkStock(string StockId) {
		return _stocksPosition.count(StockId) > 0;
	}
	void _record2File();
	float _getCurrentStockPrice(string StockId);

	float _total;		// 总资产
	float _remain;		// 可用余额
	float _keeps;		// 总市值

	map<string, PositionBase> _stocksPosition;
};