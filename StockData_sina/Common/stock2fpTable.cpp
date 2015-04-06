#include "stock2fpTable.h"

int stockFile::openedFiles = 0;
stockFile::stockFile()
{
	counter = 0;
	file = NULL;
	used = false;
	IsOpened = false;
}
stockFile::~stockFile()
{
	if (NULL != file)	close();
	used = false;
	IsOpened = false;
	counter = 0;
}
void stockFile::close()
{
	fclose(file);
	IsOpened = false;
}
bool stockFile::open(string fileName, char* fileType, char* tp)
{
	fileName += tp;
	if (0 == fopen_s(&file, fileName.c_str(), fileType)) {
		IsOpened = true;
		if (!strcmp(".stk", tp))
			stockFile::openedFiles ++;
		return true;
	}
	else return false;
}

/////////////////////////////////////////////////////////////
stock2fpTable::stock2fpTable()
{
	initial();
}

stock2fpTable::~stock2fpTable()
{
	clean();
}

void stock2fpTable::initial()
{
	NofFiles = DEFAULT_TABLE_SIZE;
	files = new stockFile[DEFAULT_TABLE_SIZE];
	AttentionFile.open("stockAttention", "a+", ".att");
}

void stock2fpTable::clean()
{
	delete []files;
	NofFiles = 0;
	AttentionFile.close();
}

bool stock2fpTable::addStock2File(string strSymbol)
{
	int idx = 0;
	if (findFreeFile(idx)) {
		files[idx].used = true;
		pair<map<string, int>::iterator, bool> ret = mapStockFile.insert(StockType(strSymbol, idx));
		if (ret.second == false)	printf_s("Not inserted, may be existed!!!\n");
		return ret.second;
	}
	else return false;
}
int stock2fpTable::removeStock2File(string strSymbol)
{
	int idx = mapStockFile[strSymbol];
	if (true == files[idx].IsFileOpened()) { files[idx].close();}

	int NofErase = mapStockFile.erase(strSymbol);
	if (0 == NofErase)	printf_s("Not removed, may be not exist!!!\n");
	else				files[idx].used = false;
	return NofErase;
}

bool stock2fpTable::findFreeFile(int &idx)
{
	for (idx = 0; idx < NofFiles; idx++)
	{
		if (true == files[idx].used)	continue;
		else {
			NofOpened++;
			return true;
		}
	}
	if (TABLE_MAX_SIZE > NofFiles && NofFiles == NofOpened)
	{
		printf_s("pre allocation is not writed!!!\n");
	}
	return false;
}

int stock2fpTable::closeAllFiles()
{
	int size = getTableSize();
	int i = 0;
	for (; i < size; i++)
		files[i].close();
	return i;
}

int stock2fpTable::writeAttentionFile(const Data_Monitor& In_data) const
{
	if (0 == In_data.need2Record) return 0;
	else {
		int idx = 0;
		char Tmp[1024] = {0};
		char* pTmp = Tmp;
		sprintf_s(pTmp, 128, "$%s  %d-%d-%d %d:%d:%d\n", In_data.strSymbol.c_str(), In_data.date.year, In_data.date.month, In_data.date.day,
			In_data.time.hour, In_data.time.minute, In_data.time.second);
		pTmp += strlen(pTmp);

		if (In_data.need2Record & WEIMAI3_THRESHOLD_TOUCH) {
			sprintf_s(pTmp, 128, "weimai3: large touch %.2fw @ price %.2f\n", In_data.bTotalMoneyIn5[MONEY_IDX]/10000, In_data.bTotalMoneyIn5[PRICE_IDX]);
			pTmp += strlen(pTmp);
		}
		if (In_data.need2Record & WEIMAI4_THRESHOLD_TOUCH) {
			sprintf_s(pTmp, 128, "weimai4: large touch %.2fw @ price %.2f\n", In_data.bTotalMoneyIn5[MONEY_IDX]/10000, In_data.bTotalMoneyIn5[PRICE_IDX]);
			pTmp += strlen(pTmp);
		}
		if (In_data.need2Record & WEIMAI3_TOTAL_TOUCH) {
			sprintf_s(pTmp, 128, "weimai3: total touch %.2fw now @ price %.2f\n", In_data.bTotalMoneyIn5[MONEY_IDX]/10000, In_data.bTotalMoneyIn5[PRICE_IDX]);
			pTmp += strlen(pTmp);
		}
		if (In_data.need2Record & WEIMAI4_TOTAL_TOUCH) {
			sprintf_s(pTmp, 128, "weimai4: total touch %.2fw now @ price %.2f\n", In_data.sTotalMoneyIn5[MONEY_IDX]/10000, In_data.sTotalMoneyIn5[PRICE_IDX]);
			pTmp += strlen(pTmp);
		}

		return AttentionFile.write(Tmp, strlen(Tmp));
	}
}
