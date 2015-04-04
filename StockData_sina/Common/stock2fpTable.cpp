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
}

void stock2fpTable::clean()
{
	delete []files;
	NofFiles = 0;
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