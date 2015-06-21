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

	// �����ʽ�
	float Add (float Money);
	// �����ʽ�
	float Sub(float money);

	// ����
	bool Buy(float Price, int Position, string StockId);
	bool Sell(float Price, int Position, string StockId);

	// �õ�״̬
	bool getStockMount   (string StockId, int   &Mount);
	bool getStockKeeps   (string StockId, float &Keeps);
	bool updateStockKeeps(string StockId, float  Price);

	inline float getTotal()  {return _total;}
	inline float getKeeps()  {return _keeps;}
	inline float getRemain() {return _remain;}

	// �������ʲ�
	void UpdateTotal();
	// ���²���ֵ
	void UpdateData();

private:
	float _getPrice(string StockId);
	bool _checkStock(string StockId) {
		return _stocksPosition.count(StockId) > 0;
	}
	void _record2File();
	float _getCurrentStockPrice(string StockId);

	float _total;		// ���ʲ�
	float _remain;		// �������
	float _keeps;		// ����ֵ

	map<string, PositionBase> _stocksPosition;
};