#pragma once
#include <cstring>
#include <string>
#include <vector>
using namespace std;

struct bufferStatus
{
	bool IsStarted;
	string SearchStr;
	string StartStr;
	string EndStr;
	string ResultStr;
	int Num;
};

class Write2Buffer
{
public:
	Write2Buffer();
	Write2Buffer(bool round, int n);
	~Write2Buffer();
	
	inline void SetBufferSize(int n);
	inline int GetBufferSize();

	int Write(char *InData, unsigned int InSize);
	bool Read(char *OutData, unsigned int InSize);
	bool ReadEx(char *OutData, int InLoc, unsigned int InSize);
	bool Find(char *InData, int& OutLoc);

	int AddSearchString(const string &InStartStr, const string &InEndStr);
	void RemoveSearchString(int num);

	void getBuffer4Write(char* OutBuffer, int& len);
	void updateAfterWrite(int len, bool* ans);
	const char* getData(int num);

private:
	char *_buffer;
	unsigned int _bufferSize;
	bool _round;

	int _writeCount;
	unsigned int _1stLength;
	unsigned int _2ndLength;

	unsigned int _maxSearchLength;
	vector<bufferStatus> searchStatus;
	int currentSearchNum;
};