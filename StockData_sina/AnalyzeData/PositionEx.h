#include <map>
#include <string>
using namespace std;
#include "..//Common//TraceMicro.h"
// #include "..//Common//stockData.h"

class MyPosition
{
public:
	MyPosition();

	// �����ʽ�
	float Add (float Money);
	// �����ʽ�
	float Sub(float money);

	// ����
	bool Buy(float Price, int Position, string StockId);
	bool Sell(float Price, int Position, string StockId);

	// �õ�״̬
	inline int   getMount(string StockId) {return _mount[StockId];}
	inline float getTotal() {return _total;}
	inline float getKeeps() {return _keeps;}
	inline float getRemain() {return _remain;}

	// �������ʲ�
	void UpdateTotal();

private:
	float _getPrice(string StockId);

	float _lastTotal;	// ����ǰ���ʲ���¼
	float _ratio;		// ���ʲ����֮ǰ�ı仯
	float _total;		// ���ʲ�
	float _remain;		// �������
	map<string, int> _mount;	// ÿ�����͵ĳֲ�
	float _keeps;		// mount��Ӧ����ֵ
};