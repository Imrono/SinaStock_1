#pragma once
#include <cstring>
#include <string>
#include <vector>
#include <map>
using namespace std;

struct bufferStatus
{
	bool IsStarted;
	bool IsFinished;

	string SearchStr;
	string StartStr;
	string EndStr;

	int NaviNum;
};
struct searchResult
{
	int Count;
	string ResultStr;
};

class Write2Buffer
{
public:
	Write2Buffer();
	Write2Buffer(bool round, int BufferSize);
	~Write2Buffer();
	
	inline void SetBufferSize(int n);
	inline int GetBufferSize();

	int Write(char *InData, unsigned int InSize);
	bool Read(char *OutData, unsigned int InSize);
	bool ReadEx(char *OutData, int InLoc, unsigned int InSize);
	bool Find(char *InData, int& OutLoc);

	bool AddSearchString(const string &InStartStr, const string &InEndStr, int num);
	bool RemoveSearchString(int num);

	char* getBuffer4Write(int& len);
	void updateAfterWrite(int len);
	const searchResult* getData(int num);

private:
	char *_buffer;
	unsigned int _bufferSize;
	bool _round;

	int _writeCount;
	unsigned int _1stLength;
	unsigned int _2ndLength;

	unsigned int _maxSearchLength;
	vector<bufferStatus> searchStatus;
	int NofCurrentSearch;

	map<int, searchResult> mapSearchResult;
};