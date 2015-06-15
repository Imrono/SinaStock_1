#include <vector>
#include <map>
#include <utility>
#include <string>
using namespace std;
#include "..//Common//GlobalParam.h"
#include "..//Common//TraceMicro.h"

class HoldPosition4Stock
{
public:
	int subType;
	int _maxBuyCount;	// ����������
	int buyCount;		// �������
	vector<int> subMount;
	vector<float> subLastTotal;
	vector<float> subTotal;
	vector<int> subBuyCount;

	HoldPosition4Stock();

	bool Buy(int Position, int idx = -1);
	bool Sell(int Position, int idx = -1);
	float UpdateKeeps(float Price);

	int GetMount() {return _mount;}
	float GetKeeps() {return _keeps;}

	// ����һ������
	int addType(int Num = 1);
	void SetName(string StockName) {_stockName = StockName;}
	void setMaxLoaded(int MaxBuyCount);

private:
	string _stockName;
	//////////////////////////////////////////////////////////////////////////
	int _mount;
	float _keeps;
	//////////////////////////////////////////////////////////////////////////
};