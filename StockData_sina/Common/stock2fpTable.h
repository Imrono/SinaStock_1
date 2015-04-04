#ifndef STOCK_2_FP_TABLE_H
#define STOCK_2_FP_TABLE_H
#include <cstdio>
#include <map>
#include <cstring>
#include <string>
using namespace std;

#include "..//Common//GlobalParam.h"

#define DEFAULT_TABLE_SIZE	10
#define TABLE_SIZE_STEP		10
#define TABLE_MAX_SIZE		100

typedef map<string, int>::value_type StockType;

class stock2fpTable;
class stockFile
{
public:
	stockFile();
	~stockFile();
	friend class stock2fpTable;
public:
	FILE* file;
	bool used;

	bool open(string fileName, char* fileType = "a+", char* tp = ".stk");
	void close();
	int write();
	int write(const char* str, int len) {
		return fwrite(str, len, 1, file);
	}
	int read();
	bool IsFileOpened()	{ return IsOpened; }
	static int getNofOpenedFiles() { return openedFiles; }

private:
	static int openedFiles;

	bool IsOpened;
	int counter;
};

class stock2fpTable
{
public:
	stock2fpTable();
	~stock2fpTable();

	FILE* getFPfromSambol(string strSymbol) {
		return files[mapStockFile[strSymbol]].file;
	}

	int getIdxfromSambol(string strSymbol) {
		return mapStockFile[strSymbol];
	}

	bool addStock2File(string strSymbol);
	int removeStock2File(string strSymbol);
	int getTableSize() { return mapStockFile.size();}
	stockFile &getFile(string strSymbol) const {
		return files[mapStockFile.find(strSymbol)->second];
	}

	int write2File(string strSymbol, const char* str, int len) const {
		return files[mapStockFile.find(strSymbol)->second].write(str, len);
	}
	int AddFileCounter(string strSymbol) const {
		return files[mapStockFile.find(strSymbol)->second].counter ++;
	}

	int closeAllFiles();

protected:
	
private:

	int NofFiles;
	int NofOpened;
	stockFile *files;

	void initial();
	void clean();
	bool findFreeFile(int &idx);

	map<string, int> mapStockFile;
};

#endif