#ifndef STOCK_2_FP_TABLE_H
#define STOCK_2_FP_TABLE_H
#include <cstdio>
#include <map>
#include <cstring>
#include <string>
using namespace std;

#include "..//Common//GlobalParam.h"
#include "..//Common//TraceMicro.h"
#include "..//Data_sina//DataStruct_sina.h"

#define DEFAULT_TABLE_SIZE	10
#define TABLE_SIZE_STEP		10
#define TABLE_MAX_SIZE		100

#define L_HAND_IDX	0
#define L_MONEY_IDX	1
#define L_MAI3		0
#define L_MAI4		1


typedef map<string, int>::value_type StockType;

class stockTable;
struct Data_Store
{
	float Price[2][2];
	float HighWaterMark[2][2];
};

class stockFile
{
public:
	stockFile();
	~stockFile();
	friend class stockTable;
public:
	bool used;

	bool open(string fileName, char* fileType = "a+", char* tp = ".stk");
	void close();
	int write();
	int write(const char* str, int len) const {
		return fwrite(str, len, 1, file);
	}
	int read();
	bool IsFileOpened()	{ return IsOpened; }
	static int getNofOpenedFiles() { return openedFiles; }

private:
	FILE* file;
	bool IsOpened;
	int counter;

	static int openedFiles;
};

class stockTable
{
public:
	stockTable();
	~stockTable();

	FILE* getFPfromSambol(string strSymbol) {
		return files[mapStockFile[strSymbol]].file;
	}

	int getIdxfromSambol(string strSymbol) {
		return mapStockFile[strSymbol];
	}

	int writeAttentionFile(const char* WriteData) const {
		return AttentionFile.write(WriteData, strlen(WriteData));
	}

	bool addStock2File(string strSymbol);
	int removeStock2File(string strSymbol);
	int getFileTableSize() { return mapStockFile.size();}
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

	void RecordHighWaterMark(string strSymbol, const Data_Store &DataStore) const {
		dataStores[mapStockFile.find(strSymbol)->second] = DataStore;
	}
	Data_Store& GetHighWaterMark(string strSymbol) const {
		return dataStores[mapStockFile.find(strSymbol)->second];
	}

protected:
	
private:

	int NofFiles;
	int NofOpened;
	stockFile *files;
	stockFile AttentionFile;
	Data_Store *dataStores;

	void initial();
	void clean();
	bool findFreeFile(int &idx);

	map<string, int> mapStockFile;
};

#endif