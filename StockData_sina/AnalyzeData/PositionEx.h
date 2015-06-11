#include <map>
#include <string>
using namespace std;
#include "..//Common//TraceMicro.h"
// #include "..//Common//stockData.h"

class MyPosition
{
public:
	MyPosition();

	// 增加资金
	float Add (float Money);
	// 减少资金
	float Sub(float money);

	// 交易
	bool Buy(float Price, int Position, string StockId);
	bool Sell(float Price, int Position, string StockId);

	// 得到状态
	inline int   getMount(string StockId) {return _mount[StockId];}
	inline float getTotal() {return _total;}
	inline float getKeeps() {return _keeps;}
	inline float getRemain() {return _remain;}

	// 更新总资产
	void UpdateTotal();

private:
	float _getPrice(string StockId);

	float _lastTotal;	// 交易前总资产记录
	float _ratio;		// 总资产相对之前的变化
	float _total;		// 总资产
	float _remain;		// 可用余额
	map<string, int> _mount;	// 每种类型的持仓
	float _keeps;		// mount对应的市值
};