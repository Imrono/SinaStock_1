#include <string>
using namespace std;
#include "TraceMicro.h"

class MY_HTML
{
	inline static bool getRow(const char *str, char *start, char *end) {

	}
	inline static bool getCol(const char *str, char *start, char *end) {

	}

	void getTable(const char *const str, int length);

public:
	static const char *const td_col_rt;
	static const char *const td_col_lt;
	static const char *const th_col_rt;
	static const char *const th_col_lt;
	static const char *const tr_row_rt;
	static const char *const tr_row_lt;
	static const char *const thread_head_rt;
	static const char *const thread_head_lt;
	static const char *const table_tb_rt;
	static const char *const table_tb_lt;
};