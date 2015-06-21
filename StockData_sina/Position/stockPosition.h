#include <vector>
#include <map>
#include <utility>
#include <string>
using namespace std;
#include "..//Common//GlobalParam.h"
#include "..//Common//TraceMicro.h"
#include "PositionBase.h"

class stockPosition : public PositionBase
{
public:
	int subType;
	int maxBuyCount;	// 最大买入次数
	int buyCount;		// 买入次数
	vector<int> subMount;
	vector<float> subLastTotal;
	vector<float> subTotal;
	vector<int> subBuyCount;

	bool Buy(int Position, int idx = -1);
	bool Sell(int Position, int idx = -1);

	// 插入一种类型
	int addType(int Num = 1);
	void setMaxLoaded(int MaxBuyCount);

	void Init(string StockName);

};