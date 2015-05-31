#include "html.h"

const char *const MY_HTML::td_lt = "<td";
const char *const MY_HTML::td_rt = "</td>";
const char *const MY_HTML::th_lt = "<th";
const char *const MY_HTML::th_rt = "</th>";
const char *const MY_HTML::tr_start = "<tr";
const char *const MY_HTML::tr_end = "</tr>";
const int MY_HTML::ColRowLtLen = 3;
const int MY_HTML::ColRowRtLen = 4;
const char *const MY_HTML::thead_start = "<thead";
const char *const MY_HTML::thead_end = "</thead>";
const char *const MY_HTML::table_start = "<table";
const char *const MY_HTML::table_end = "</table>";
const char *const MY_HTML::tbody_lt = "<table";
const char *const MY_HTML::tbody_rt = "</table>";
const char *const MY_HTML::tfoot_lt = "<table";
const char *const MY_HTML::tfoot_rt = "</table>";
const int MY_HTML::SegmentLtLen = 6;
const int MY_HTML::SegmentRtLen = 8;

void MY_HTML::getTable(char *const str, int length) {
	const int searchScope = 32;
	int tdCount = 0;
	int thCount = 0;
	int trCount = 0;

	char *tmp;
	const char *tmp_end;
	char *table_end;
	char tmpStoreTable;
	char tmpStoreSegment;
	const char *MarkStart;
	char *MarkEnd;
	const char *SegStart;
	char *SegEnd;
	vector<StrElem> *RowAns;

	// check table的基本格式
	if (nullptr == (tmp = strstr(str, MY_HTML::table_start))) {
		ERRR("None table label start\n");
		return;
	} else {
		if (nullptr == (table_end = strstr(str, MY_HTML::table_end))) {
			ERRR("None table label end\n");
			return;
		} else {
			table_end += MY_HTML::SegmentRtLen;
			_setZeroAfter(table_end);
		}
	}
	// 得到table head
	while (nullptr != (tmp = strstr(tmp, MY_HTML::thead_start))) {
		SegStart = tmp;
		if (nullptr == (SegEnd = strstr(tmp, MY_HTML::thead_end))) {
			ERRR("None table label end\n");
			return;
		} else {
			SegEnd += MY_HTML::SegmentLtLen;
			_setZeroAfter(SegEnd);

			MarkStart = strstr(tmp, MY_HTML::tr_start);
			tmp = strstr(tmp, MY_HTML::tr_end);
			tmp += MY_HTML::ColRowRtLen; // tmp在SegEnd
			MarkEnd = tmp;
			_setZeroAfter(MarkEnd);

			// 求解detail table
			RowAns = getRow(MarkStart, MarkEnd, th_type);
			// 存储
			tableEle[head_type].push_back(rowEle);
			rowEle.clear();
			// 恢复之前状态
			_recoverDataAfter(); // mark
			MarkStart = nullptr; MarkEnd = nullptr;
			_recoverDataAfter(); // seg
			SegStart = nullptr; SegEnd = nullptr;
		}
	}

	// 得到table body


	// 得到table foot


	_recoverDataAfter(); // table
}

// start = "<tr", end = "<\tr>"的最后
vector<StrElem> *MY_HTML::getRow(const char *start, const char *end, int td_th) {
	if (nullptr == start || nullptr == end) return nullptr;

	int colCount = 0;
	StrElem tmpColStr;
	memset(&tmpColStr, 0, sizeof(StrElem));

	const char *colTypeStart = nullptr;
	const char *colTypeEnd = nullptr;
	if (td_type == td_th) {
		colTypeStart = MY_HTML::td_lt;
		colTypeEnd = MY_HTML::td_rt;
	} else if (th_type == td_th) {
		colTypeStart = MY_HTML::th_lt;
		colTypeEnd = MY_HTML::td_rt;
	} else INFO("None COL type, known colType is td or th\n");

	for (const char *tmp = start; tmp < end;) {
		if (nullptr == (tmp = strstr(tmp, colTypeStart))) goto ERROR_CASE;
		tmp += MY_HTML::ColRowLtLen;
		if (nullptr == (tmp = strchr(tmp, '>'))) goto ERROR_CASE;
		while ('<' == *(tmp+1)) {
			if (nullptr == (tmp = strchr(tmp, '>'))) goto ERROR_CASE;
			else continue;
		}
		tmpColStr.Elem = tmp+1;
		if (nullptr == (tmp = strchr(tmp, '<'))) goto ERROR_CASE;
		tmpColStr.Length = tmp - tmpColStr.Elem;
		rowEle.push_back(tmpColStr);
		memset(&tmpColStr, 0, sizeof(StrElem));
		if (nullptr == (tmp = strstr(tmp, colTypeEnd))) goto ERROR_CASE;
		else continue;
	}
	return &rowEle;

ERROR_CASE:
	return nullptr;
}
