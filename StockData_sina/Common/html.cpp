#include "html.h"

const char *const MY_HTML::td_col_lt = "<td";
const char *const MY_HTML::td_col_rt = "</td>";
const char *const MY_HTML::th_col_lt = "<th";
const char *const MY_HTML::th_col_rt = "</th>";
const char *const MY_HTML::tr_row_lt = "<td";
const char *const MY_HTML::tr_row_rt = "</td>";
const char *const MY_HTML::thread_head_lt = "<thead";
const char *const MY_HTML::thread_head_rt = "</thead>";
const char *const MY_HTML::table_tb_lt = "<table";
const char *const MY_HTML::table_tb_rt = "</thead>";


void MY_HTML::getTable(const char *const str, int length) {
	int tdCount = 0;
	int thCount = 0;
	int trCount = 0;
	int threadCount = 0;

	const char *tmp;
	const char *tmp_end;
	// check基本格式
	if (nullptr == (tmp = strstr(str, MY_HTML::table_tb_lt))) {
		ERRR("None table label start\n");
		return;
	} else {
		if (nullptr == (tmp_end = strstr(str, MY_HTML::table_tb_rt))) {
			ERRR("None table label end\n");
			return;
		} else {
			tmp_end += strlen(MY_HTML::table_tb_rt);
		}
	}
	// 得到table head
	if (1);

}
