#include <cstdio>
#include <string>
#include <iostream>
using namespace std;

#pragma comment(lib, "Wininet.lib")

#include "getData_sina.h"

static const char* stock_identify = "var hq_str_";

void urlopen_sina(const char* url, char* fileName, char* fileType)
{
	printf_s("%d,%d\n", sizeof(DataUpdate_sina), sizeof(Data_sina));
	HINTERNET hSession = InternetOpen(_T("UrlTest"), INTERNET_OPEN_TYPE_PRECONFIG, NULL, NULL, 0);
	if(hSession != NULL)
	{
		HINTERNET hHttp = InternetOpenUrl(hSession, url, NULL, 0, INTERNET_FLAG_DONT_CACHE, 0);

		if (hHttp != NULL)
		{
			printf_s("%s\n", url);
			int DataSize = 2;
			Data_sina *Out_data = new Data_sina[DataSize];
			BYTE Temp[MAX_RECV_BUF_SIZE] = {0};
			do
			{
				int NofStock = 0;
				int ReadTimes = 0;
				ULONG Number = 0;
				InternetReadFile(hHttp, Temp, MAX_RECV_BUF_SIZE - 1, &Number);
				Temp[Number] = '\0';
				if (Number) { 
					if (Number == MAX_RECV_BUF_SIZE-1) {printf("not all read");}
					NofStock = data_analyseAll((char*) Temp, Out_data, DataSize);
					printf_s("%s", Temp);
					data_WriteAll(fileName, fileType, Out_data, DataSize <= NofStock ? DataSize : NofStock);
// 					getchar();
					printf_s("ReadTimes:%d, ReadNum:%d\n", ++ReadTimes, Number);
				}
				else {break;}
			} while (1);
			delete []Out_data;
			InternetCloseHandle(hHttp);	hHttp = NULL;
		}
		InternetCloseHandle(hSession);	hSession = NULL;
	} 
}

void data_WriteAll(const char* fileName, char* fileType, const Data_sina *const In_data, int write_times)
{
	for (int i = 0; i < write_times; i++)
		data_WriteOne(fileName, fileType, In_data[i].dataUpdate);
}
bool data_WriteOne(const char* fileName, char* fileType, const DataUpdate_sina & In_dataUpdate)
{
	FILE * fp = NULL;
	fopen_s(&fp, fileName, fileType);
	char Temp[MAX_RECV_BUF_SIZE] = {0};
	sprintf_s(Temp, "%.2f\t%02d:%02d:%02d\r\n%.2f\t%.2f\t%.2f\t%.2f\t%.2f\t||\t%.2f\t%.2f\t%.2f\t%.2f\t%.2f\r\n%4d\t%4d\t%4d\t%4d\t%4d\t||\t%4d\t%4d\t%4d\t%4d\t%4d\t(hands)\r\n", 
		In_dataUpdate.cur_price, In_dataUpdate.time.hour, In_dataUpdate.time.minute, In_dataUpdate.time.second,
		In_dataUpdate.downOrder_price[4], In_dataUpdate.downOrder_price[3], In_dataUpdate.downOrder_price[2],
		In_dataUpdate.downOrder_price[1], In_dataUpdate.downOrder_price[0],
		In_dataUpdate.upOrder_price[0], In_dataUpdate.upOrder_price[1], In_dataUpdate.upOrder_price[2],
		In_dataUpdate.upOrder_price[3], In_dataUpdate.upOrder_price[4],
		In_dataUpdate.downOrder_stock[4]/100, In_dataUpdate.downOrder_stock[3]/100, In_dataUpdate.downOrder_stock[2]/100,
		In_dataUpdate.downOrder_stock[1]/100, In_dataUpdate.downOrder_stock[0]/100,
		In_dataUpdate.upOrder_stock[0]/100, In_dataUpdate.upOrder_stock[1]/100, In_dataUpdate.upOrder_stock[2]/100,
		In_dataUpdate.upOrder_stock[3]/100, In_dataUpdate.upOrder_stock[4]/100
	);
	fwrite(Temp, strlen(Temp), 1, fp);
	fclose(fp);	fp = NULL;
	return true;
}

void data_WriteTable(const stock2fpTable &tb, const Data_sina* const In_data, int write_times)
{
	for (int i = 0; i < write_times; i++)
		data_WriteTableOne(tb, In_data[i]);
}
int data_WriteTableOne(const stock2fpTable &tb, const Data_sina &In_data)
{
	const int bufSize = 384;
	const DataUpdate_sina & In_dataUpdate = In_data.dataUpdate;
	char Temp[bufSize] = {0};
	sprintf_s(Temp, "---%5d---\n%.2f\t%02d:%02d:%02d\r\n%.2f\t%.2f\t%.2f\t%.2f\t%.2f\t||\t%.2f\t%.2f\t%.2f\t%.2f\t%.2f\r\n%4d\t%4d\t%4d\t%4d\t%4d\t||\t%4d\t%4d\t%4d\t%4d\t%4d\t(hands)\r\n", 
		tb.AddFileCounter(In_data.stockNavi.strSymbol),
		In_dataUpdate.cur_price, In_dataUpdate.time.hour, In_dataUpdate.time.minute, In_dataUpdate.time.second,
		In_dataUpdate.downOrder_price[4], In_dataUpdate.downOrder_price[3], In_dataUpdate.downOrder_price[2],
		In_dataUpdate.downOrder_price[1], In_dataUpdate.downOrder_price[0],
		In_dataUpdate.upOrder_price[0], In_dataUpdate.upOrder_price[1], In_dataUpdate.upOrder_price[2],
		In_dataUpdate.upOrder_price[3], In_dataUpdate.upOrder_price[4],
		In_dataUpdate.downOrder_stock[4]/100, In_dataUpdate.downOrder_stock[3]/100, In_dataUpdate.downOrder_stock[2]/100,
		In_dataUpdate.downOrder_stock[1]/100, In_dataUpdate.downOrder_stock[0]/100,
		In_dataUpdate.upOrder_stock[0]/100, In_dataUpdate.upOrder_stock[1]/100, In_dataUpdate.upOrder_stock[2]/100,
		In_dataUpdate.upOrder_stock[3]/100, In_dataUpdate.upOrder_stock[4]/100
		);

	return tb.write2File(In_data.stockNavi.strSymbol, Temp, strlen(Temp));
}

int data_analyseAll(const char* const In_data, Data_sina *Out_data, int data_len)
{
	int i = 0;
	int idx = 0;
	const char* tmp = In_data;
	while (i < data_len)
	{
		if (NULL != (tmp = data_analyseOne(tmp, Out_data[i])))
			i++;
		else
		{
			printf_s("Data_analyse All Finished!!!!\n");
			break;
		}
	}
	printf_s("%d stocks have been analyzed!!\n", i);
	return i;
}
const char* data_analyseOne(const char* const In_data, Data_sina &Out_data)
{
	//var hq_str_sh600820="ËíµÀ¹É·Ý,10.20,10.19,10.22,10.41,10.07,10.21,10.22,94059479
		//,963103849,103200,10.21,890466,10.20,304837,10.19,248400,10.18,59300,10.17,18300
		//,10.22,368525,10.24,785450,10.25,211552,10.26,184150,10.27,2015-03-27,15:03:04,0
		//0";
	memset(&Out_data, 0, sizeof(Data_sina));
	Out_data.stockNavi.strSymbol = GetStrSymbol(In_data);
	int idx = 0;
	const char* pIn_data = strstr(In_data, "var hq_str_");
	if (pIn_data) {
		idx += 11;
		if		(strcmp(pIn_data+idx, "sh")) { Out_data.stockNavi.market = SH;	idx += 2;}
		else if	(strcmp(pIn_data+idx, "sz")) { Out_data.stockNavi.market = SZ;	idx += 2;}
		else if	(strcmp(pIn_data+idx, "hk")) { Out_data.stockNavi.market = HK;	idx += 2;}
		else {	printf_s("Undefined Market!!");}
		
		sscanf_s(pIn_data+idx, "%d=\"%[^,],%f,%f,%f,%f,%f,%f,%f,%d,%lf,%d,%f,%d,%f,%d,%f,%d,%f,%d,%f,%d,%f,%d,%f,%d,%f,%d,%f,%d,%f,%d-%d-%d,%d:%d:%d,00\";",
			&Out_data.stockNavi.symbol, Out_data.stockNavi.name, sizeof(Out_data.stockNavi.name), &Out_data.td_open, &Out_data.ysd_close,
			&Out_data.dataUpdate.cur_price, &Out_data.dataUpdate.td_high, &Out_data.dataUpdate.td_low,
			&Out_data.buyup_price, &Out_data.buyor_price,
			&Out_data.dataUpdate.turnover_stock, &Out_data.dataUpdate.turnover_current,
			&Out_data.dataUpdate.upOrder_stock[0], &Out_data.dataUpdate.upOrder_price[0],
			&Out_data.dataUpdate.upOrder_stock[1], &Out_data.dataUpdate.upOrder_price[1],
			&Out_data.dataUpdate.upOrder_stock[2], &Out_data.dataUpdate.upOrder_price[2],
			&Out_data.dataUpdate.upOrder_stock[3], &Out_data.dataUpdate.upOrder_price[3],
			&Out_data.dataUpdate.upOrder_stock[4], &Out_data.dataUpdate.upOrder_price[4],
			&Out_data.dataUpdate.downOrder_stock[0], &Out_data.dataUpdate.downOrder_price[0],
			&Out_data.dataUpdate.downOrder_stock[1], &Out_data.dataUpdate.downOrder_price[1],
			&Out_data.dataUpdate.downOrder_stock[2], &Out_data.dataUpdate.downOrder_price[2],
			&Out_data.dataUpdate.downOrder_stock[3], &Out_data.dataUpdate.downOrder_price[3],
			&Out_data.dataUpdate.downOrder_stock[4], &Out_data.dataUpdate.downOrder_price[4],
			&Out_data.date.year, &Out_data.date.month, &Out_data.date.day,
			&Out_data.dataUpdate.time.hour, &Out_data.dataUpdate.time.minute, &Out_data.dataUpdate.time.second	);

// 		printf_s("var hq_str_xx%06d=\"%s,%.2f,%.2f,%.2f,%.2f,%.2f,%.2f,%.2f,%d,%.2f,%d,%.2f,%d,%.2f,%d,%.2f,%d,%.2f,%d,%.2f,%d,%.2f,%d,%.2f,%d,%.2f,%d,%.2f,%d,%.2f,%d-%d-%d,%d:%d:%d,00\";\n\n",
// 			Out_data.stockNavi.symbol, Out_data.stockNavi.name, Out_data.td_open, Out_data.ysd_close,
// 			Out_data.dataUpdate.cur_price, Out_data.dataUpdate.td_high, Out_data.dataUpdate.td_low,
// 			Out_data.buyup_price, Out_data.buyor_price,
// 			Out_data.dataUpdate.turnover_stock, Out_data.dataUpdate.turnover_current,
// 			Out_data.dataUpdate.upOrder_stock[0], Out_data.dataUpdate.upOrder_price[0],
// 			Out_data.dataUpdate.upOrder_stock[1], Out_data.dataUpdate.upOrder_price[1],
// 			Out_data.dataUpdate.upOrder_stock[2], Out_data.dataUpdate.upOrder_price[2],
// 			Out_data.dataUpdate.upOrder_stock[3], Out_data.dataUpdate.upOrder_price[3],
// 			Out_data.dataUpdate.upOrder_stock[4], Out_data.dataUpdate.upOrder_price[4],
// 			Out_data.dataUpdate.downOrder_stock[0], Out_data.dataUpdate.downOrder_price[0],
// 			Out_data.dataUpdate.downOrder_stock[1], Out_data.dataUpdate.downOrder_price[1],
// 			Out_data.dataUpdate.downOrder_stock[2], Out_data.dataUpdate.downOrder_price[2],
// 			Out_data.dataUpdate.downOrder_stock[3], Out_data.dataUpdate.downOrder_price[3],
// 			Out_data.dataUpdate.downOrder_stock[4], Out_data.dataUpdate.downOrder_price[4],
// 			Out_data.date.year, Out_data.date.month, Out_data.date.day,
// 			Out_data.dataUpdate.time.hour, Out_data.dataUpdate.time.minute, Out_data.dataUpdate.time.second);
		return strstr(pIn_data+idx, ";")+1;
	}
	else
	{
		return NULL;
	}
}

ReceiveDataType GetDataType(char* data)
{
	const int NofComma1 = 32;

	char *tmp1 = "var hq_str_";
	int i = 0; int j = 0;
	for (j = 0; j < 11; j++, i++) {
		if (tmp1[j] == data[i])	continue;
		else if (0 == data[i])	return NOT_INTACT_SINA;
		else return UNKNOW_SINA;
	}
	char* tmp2 = "=\"";
	for (j = 0; j < 2; j++, i++) {
		if (tmp1[j] == data[i])	continue;
		else if (0 == data[i])	return NOT_INTACT_SINA;
		else return UNKNOW_SINA;
	}
	int NofComma = 0;
	while (0 != data[i]) {
		if (',' == data[i])		{ NofComma ++;}
		else if (0 == data[i])	return NOT_INTACT_SINA;
		else if ('\"' == data[i]) {
			if (';' == data[i])		break;
			else if (0 == data[i])	return NOT_INTACT_SINA;
			else					return UNKNOW_SINA;
		}
		i++;
	}

	if ( NofComma1 == NofComma)	return NORMAL_SINA;
	else						return UNKNOW_SINA;
}
string GetStrSymbol(const char* const In_data)
{
	string ret;
	const char* tmp = strstr(In_data, "var hq_str_");
	if (tmp)
		ret.assign(tmp+11, 8);
	return ret;
}

void urlopen_sina_TB(const char* url, const stock2fpTable &tb)
{
	printf_s("%d,%d\n", sizeof(DataUpdate_sina), sizeof(Data_sina));
	HINTERNET hSession = InternetOpen(NULL, INTERNET_OPEN_TYPE_PRECONFIG, NULL, NULL, 0);
	if(hSession != NULL) {
		int i = 0;
		while (1) {
			openURL_write(hSession, url, tb);
			Sleep(time_step);

			if (++i > times_get)	break;
			else					printf_s("i=%d\n", i);
		}
	}
	InternetCloseHandle(hSession);	hSession = NULL;
}
int openURL_write(HINTERNET hSession, const char* url, const stock2fpTable &tb)
{
	if (NULL == hSession) return -2;

	HINTERNET hHttp = InternetOpenUrl(hSession, url, NULL, 0, INTERNET_FLAG_DONT_CACHE, 0);

	if (hHttp != NULL) {
		printf_s("opened:\n%s\n", url);
		string data_recv;

		ReadDataFromInternet(hHttp, data_recv);
		int NofStock = data_analyse2FileAll(data_recv.c_str(), tb);

		InternetCloseHandle(hHttp);	hHttp = NULL;

		return NofStock;
	}
	else return -1;
}
void ReadDataFromInternet(HINTERNET hHttp, string &data_recv)
{
	BYTE Temp[MAX_RECV_BUF_SIZE*2] = {0};
	int ReadTimes = 0;
	data_recv = "";

	while (1) {
		ULONG Number = 0;
		InternetReadFile(hHttp, Temp, MAX_RECV_BUF_SIZE - 1, &Number);
		Temp[Number] = '\0';
		data_recv += (char*)Temp;

		if (Number) { 
// 			printf_s("Temp:\n%s\ndata_recv:\n%s", Temp, data_recv.c_str());
			printf_s("ReadTimes:%d, ReadNum:%d\n", ++ReadTimes, Number);
		}
		else {break;}
	}
}
int data_analyse2FileAll(const char* const In_data, const stock2fpTable &tb)
{
	Data_sina Out_data;
	int i = 0;
	const char* tmp = In_data;
	while (1)
	{
		if (NULL != (tmp = data_analyseOne(tmp, Out_data))) {
			data_WriteTableOne(tb, Out_data);
			i++;
		}
		else {
			printf_s("Data_analyse All Finished!!!!\n");
			break;
		}
	}
	printf_s("%d stocks have been analyzed!!\n", i);
	return i;
}

void urlopen_sina_TB_ex(const char* url, const stock2fpTable &tb)
{
	int i = 0;
	while (1) {
		openURL_write_ex(url, tb);

		Sleep(time_step);
		if (++i > times_get)	break;
		else					printf_s("times_get=%d\n", i);
	}
}
int openURL_write_ex(const char* url, const stock2fpTable &tb)
{
	HttpUrlGetSyn urlGet;
	if (NULL != urlGet.OpenUrl(url)) {
		printf_s("opened:\n%s\n", url);
		string data_recv;
		urlGet.ReadUrlAll(data_recv);
		int NofStock = data_analyse2FileAll(data_recv.c_str(), tb);

		return NofStock;
	}
	else return -1;
}
