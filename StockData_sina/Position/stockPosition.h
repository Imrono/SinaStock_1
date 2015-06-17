#include <vector>
#include <map>
#include <utility>
#include <string>
using namespace std;
#include "..//Common//GlobalParam.h"
#include "..//Common//TraceMicro.h"
#include "PositionBase.h"

class HoldPosition4Stock : public PositionBase
{
public:
	int subType;
	int maxBuyCount;	// ����������
	int buyCount;		// �������
	vector<int> subMount;
	vector<float> subLastTotal;
	vector<float> subTotal;
	vector<int> subBuyCount;

	bool Buy(int Position, int idx = -1);
	bool Sell(int Position, int idx = -1);

	// ����һ������
	int addType(int Num = 1);
	void setMaxLoaded(int MaxBuyCount);

	void Init(string StockName);

};