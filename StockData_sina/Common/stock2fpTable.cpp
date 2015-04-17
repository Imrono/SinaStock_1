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
stockTable::stockTable()
{
	initial();
}

stockTable::~stockTable()
{
	clean();
}

void stockTable::initial()
{
	NofFiles = DEFAULT_TABLE_SIZE;
	files = new stockFile[DEFAULT_TABLE_SIZE];
	dataStores = new Data_Store[DEFAULT_TABLE_SIZE];
	for (int i = 0; i < DEFAULT_TABLE_SIZE; i++) {
		memset(dataStores, 0, sizeof(Data_Store));
	}
	AttentionFile.open("stockAttention", "a+", ".att");
}

void stockTable::clean()
{
	delete []files;
	delete []dataStores;
	NofFiles = 0;
	AttentionFile.close();
}

bool stockTable::addStock2File(string strSymbol)
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
int stockTable::removeStock2File(string strSymbol)
{
	int idx = mapStockFile[strSymbol];
	if (true == files[idx].IsFileOpened()) { files[idx].close();}

	int NofErase = mapStockFile.erase(strSymbol);
	if (0 == NofErase)	printf_s("Not removed, may be not exist!!!\n");
	else				files[idx].used = false;
	return NofErase;
}

bool stockTable::findFreeFile(int &idx)
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

int stockTable::closeAllFiles()
{
	int size = getFileTableSize();
	int i = 0;
	for (; i < size; i++)
		files[i].close();
	return i;
}
