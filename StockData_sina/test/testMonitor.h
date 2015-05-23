#include <time.h>
#include <string>
using namespace std;

class testTime
{
public:
	testTime(string name = "", bool NeedStart = true) {
		_name = name;
		if (NeedStart) start();
	}
	~testTime() {
		if (_IsStart)
			stop();
	}
	void start() {_start = clock(); _IsStart = true;}
	void stop() {
		_end = clock();
		_IsStart = false;
		_diff = _end - _start;
		TEST_TRACE("%s using time %ds%dms\n", _name.c_str(), _diff/CLOCKS_PER_SEC, _diff-(clock_t)(_diff/CLOCKS_PER_SEC));
	}
private:
	bool _IsStart;
	clock_t _diff;
	clock_t _start;
	clock_t _end;
	string _name;
};