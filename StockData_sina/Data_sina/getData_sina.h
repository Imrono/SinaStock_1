#ifndef GET_DATA_SINA_H
#define GET_DATA_SINA_H
#include <cstring>
#include <windows.h>
#include <wininet.h>
using namespace std;
#include "DataStruct_sina.h"
#include "..//Common//stock2fpTable.h"
#include "..//Common//HttpUrl.h"
#include "..//Common//GlobalParam.h"

enum ReceiveDataType
{
	NORMAL_SINA,
	INDEX_SINA,
	NOT_INTACT_SINA,
	UNKNOW_SINA
};

void urlopen_sina(const char* url, char* fileName, char* fileType);
void urlopen_sina_TB(const char* url, const stock2fpTable &tb);
void ReadDataFromInternet(HINTERNET hHttp, string &data_recv);



int data_analyseAll(const char* const In_data, Data_sina *Out_data, int data_len);
const char* data_analyseOne(const char* const In_data, Data_sina &Out_data);

void data_WriteAll(const char* fileName, char* fileType, const Data_sina *const In_data, int write_times);
bool data_WriteOne(const char* fileName, char* fileType, const DataUpdate_sina & In_dataUpdate);
void data_WriteTable(const stock2fpTable &tb, const Data_sina* const In_data, int write_times);
int data_WriteTableOne(const stock2fpTable &tb, const Data_sina &In_data);

ReceiveDataType GetDataType(char* data);
string GetStrSymbol(const char* const In_data);
int openURL_write(HINTERNET hSession, const char* url, const stock2fpTable &tb);

int data_analyse2FileAll(const char* const In_data, const stock2fpTable &tb);
void urlopen_sina_TB_ex(const char* url, const stock2fpTable &tb);
int openURL_write_ex(const char* url, const stock2fpTable &tb);
#endif