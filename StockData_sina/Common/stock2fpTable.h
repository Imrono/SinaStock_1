#ifndef STOCK_2_FP_TABLE_H
#define STOCK_2_FP_TABLE_H
#include <cstdio>
#include <map>
#include <cstring>
#include <string>
#include <vector>
using namespace std;
#include <windows.h>
#include  <io.h>

#include "..//Common//GlobalParam.h"
#include "..//Common//TraceMicro.h"
#include "stockData.h"
#include "..//Data_sina//DataStruct_sina.h"

#define DEFAULT_TABLE_SIZE	10
#define TABLE_SIZE_STEP		10
#define TABLE_MAX_SIZE		100

#define L_HANDS_IDX	0
#define L_MONEY_IDX	1
#define L_MAI3		0
#define L_MAI4		1


typedef map<string, int>::value_type StockType;

class stockTable;
struct Data_Store
{
	float bPrice;
	float sPrice;

	float bHighWaterMark[2];
	float sHighWaterMark[2];

	stockTime bTime;
	stockTime sTime;
};

class stockFile
{
public:
	stockFile();
	~stockFile();
	friend class stockTable;
public:
	bool used;

	bool open(string fileName, const char *fileType = "a+", const char *tp = ".stk");
	void close();
	int write(const char* str, int len) const {
		return fwrite(str, len, 1, file);
	}
	int writeHead(const char* str, int len) const {
		fseek(file,0,SEEK_SET);
		return fwrite(str, len, 1, file);
	}
	char* readline(char *OutBuffer, int size) {
		if (!IsOpened)	return nullptr;
		else			return fgets(OutBuffer, size, file);
	}

	bool IsFileOpened()	{ return IsOpened; }
	static int getNofOpenedFiles() { return openedFiles; }

	static bool CheckFolderExist(const string &strPath);
	static bool CheckDSTKFileExist(const string &strPath, bool IsFolder, const char *pt);
	static void FindLatestDTSK(const string &strPath, string &OutStr, const char *pt);

	static bool IsAccessable(const char *path) {
		return -1 != _access(path, 0);	//#define F_OK 0 /* Test for existence. */
	}
	static void getFiles(string path, vector<string>& files, const char *suffix = nullptr);
	static bool createFile(string path);
	static bool FileWriteTime(string path, SYSTEMTIME &s_t);

	static void SetFileNameFormate(const string &id, int year, int data_Season, const char *tp, string &str);
	static void GetFileNameFormate(const string &str, const char *tp, int &year, int &data_Season, string &id);

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

	void initial(int TableSize);
	void clean();
	bool findFreeFile(int &idx);

	map<string, int> mapStockFile;
};

#endif