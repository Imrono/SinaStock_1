#include <string>
#include <vector>
using namespace std;
#include "TraceMicro.h"

struct StrElem {
	const char *Elem;
	int Length;
};
struct tmpStoreReplace {
	char *Location;
	char c;
};

class MY_HTML
{
	vector<StrElem> *getRow(const char *start, const char *end, int td_th);
	inline static bool getCol(const char *str, char *start, char *end) {

	}

	void getTable(char *const str, int length);

	enum ColType {
		th_type = 0,
		td_type = 1
	};
	enum RowType {
		head_type = 0,
		body_type = 1,
		foot_type = 2,
		other_type = 3
	};

public:
	// head body foot分开
	vector<vector<StrElem>> tableEle[3];
	vector<StrElem> rowEle;

	static const char *const td_rt;
	static const char *const td_lt;
	static const char *const th_rt;
	static const char *const th_lt;
	static const char *const tr_end;
	static const char *const tr_start;
	static const char *const thead_head_end;
	static const char *const thead_head_start;
	static const char *const table_start;
	static const char *const table_end;
	static const int ColRowLtLen;
	static const int ColRowRtLen;
	static const char *const thead_start;
	static const char *const thead_end;
	static const char *const table_lt;
	static const char *const table_rt;
	static const char *const tbody_lt;
	static const char *const tbody_rt;
	static const char *const tfoot_lt;
	static const char *const tfoot_rt;
	static const int SegmentLtLen;
	static const int SegmentRtLen;

private:
	vector<tmpStoreReplace> _tmpStore;
	void _setZeroAfter(char *tmp) {
		tmpStoreReplace x;
		x.c = *(tmp+1);
		x.Location = tmp+1;
		_tmpStore.push_back(x);
		*(tmp+1) = '\0';
	}
	void _recoverDataAfter() {
		tmpStoreReplace x = _tmpStore.back();
		_tmpStore.pop_back();
		*x.Location = x.c;
	}
};