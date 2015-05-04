#include "Write2Buffer.h"

Write2Buffer::Write2Buffer() :_buffer(nullptr), _bufferSize(0), _round(false), _writeCount(0), _1stLength(0), _2ndLength(0), 
	_maxSearchLength(0), NofCurrentSearch(0) {}
Write2Buffer::Write2Buffer(bool round, int BufferSize):_writeCount(0), _1stLength(0), _2ndLength(0), 
	_maxSearchLength(0), NofCurrentSearch(0) {
	_round = round;
	if (_round)
		_bufferSize = 2*BufferSize;
	else
		_bufferSize = BufferSize;

	_buffer = new char[_bufferSize+1];
	memset(_buffer, 0, _bufferSize+1);
}
Write2Buffer::~Write2Buffer() {
	_1stLength = 0;
	_2ndLength = 0;
	_writeCount = 0;
	_bufferSize = 0;
	delete []_buffer;
	_buffer = nullptr;
}

void Write2Buffer::SetBufferSize(int n) {
	_bufferSize = n;
}
int Write2Buffer::GetBufferSize() {
	return _bufferSize;
}

int Write2Buffer::Write(char *InData, unsigned int InSize) {
	if (_round) {
		for (unsigned int len = 0; len < _2ndLength; len += _1stLength) {
			int tmpLength = 0;
			if (len+_1stLength > _2ndLength)
				tmpLength = _2ndLength - len;
			else
				tmpLength = _1stLength;

			memcpy(_buffer+len, _buffer+len+_1stLength, tmpLength);
		}
		_1stLength = _2ndLength;

		if (InSize < (_bufferSize - _1stLength))
			_2ndLength = InSize;
		else
			_2ndLength = _bufferSize - _1stLength;

		memcpy(_buffer+_1stLength, InData, _2ndLength);
	}
	else {
		if (InSize <= _bufferSize)
			_2ndLength = InSize;
		else
			_2ndLength = _bufferSize;

		memcpy(_buffer, InData, _bufferSize);
	}
	memset(_buffer+_1stLength+_2ndLength, 0, _bufferSize-_1stLength-_2ndLength);
	return _2ndLength;
}
bool Write2Buffer::Read(char *OutData, unsigned int InSize) {
	if (InSize > _bufferSize)
		return false;
	else {
		memcpy(OutData, _buffer, InSize);
		return true;
	}
}
bool Write2Buffer::ReadEx(char *OutData, int InLoc, unsigned int InSize) {
	if (InSize > _bufferSize-InLoc)
		return false;
	else {
		memcpy(OutData, _buffer+InLoc, InSize);
		return true;
	}
}
bool Write2Buffer::Find(char *InData, int& OutLoc) {
	char* subStr = strstr(_buffer, InData);
	if (subStr) {
		OutLoc = subStr - _buffer;
		return true;
	}
	else
		return false;
}


bool Write2Buffer::AddSearchString(const string &InStartStr, const string &InEndStr, int num) {
	for (vector<bufferStatus>::iterator it = searchStatus.begin(); it != searchStatus.end(); ++it) {
		if (it->NaviNum == num) return false;
	}

	bufferStatus tmp;
	tmp.IsStarted = false;
	tmp.IsFinished = false;
	tmp.NaviNum = num;
	tmp.SearchStr = InStartStr;
	tmp.StartStr = InStartStr;
	tmp.EndStr = InEndStr;


	searchStatus.push_back(tmp);

	NofCurrentSearch ++;
	_maxSearchLength = _maxSearchLength > tmp.SearchStr.size() ? _maxSearchLength : tmp.SearchStr.size();

	mapSearchResult[num].Count = 0;
	mapSearchResult[num].ResultStr = "";
	return true;
}
bool Write2Buffer::RemoveSearchString(int num) {
	bool vecReady = false;
	for (vector<bufferStatus>::iterator it = searchStatus.begin(); it != searchStatus.end(); ++it) {
		if (it->NaviNum == num) {
			searchStatus.erase(it);
			_maxSearchLength = _maxSearchLength > it->SearchStr.size() ? _maxSearchLength : it->SearchStr.size();
			NofCurrentSearch--;

			vecReady = true;
		}
	}
	if (mapSearchResult.erase(num)) {
		if (vecReady)	return true;
		else			return false;
	}
	else return false;
}

/****************************************************************/
/* return buffer address
/* param(s)
/* int &len : out max available buffer size
/****************************************************************/
char* Write2Buffer::getBuffer4Write(int& len) {
	if (_1stLength+_2ndLength <= _maxSearchLength) {
		_1stLength += _2ndLength;
	}
	else {
		memmove(_buffer, _buffer+ (_1stLength+_2ndLength-_maxSearchLength), _maxSearchLength);
		memset(_buffer+_maxSearchLength, 0, _1stLength+_2ndLength-_maxSearchLength);
		_1stLength = _maxSearchLength;
	}
	_2ndLength = 0;

	len = _bufferSize - _1stLength;
	return 	_buffer + _1stLength;
}

void Write2Buffer::updateAfterWrite(int len) {
	_2ndLength = len;

	//ANALYZE
	char* tmp, *tmpStart, *tmpEnd;
	for (vector<bufferStatus>::iterator it = searchStatus.begin(); it != searchStatus.end(); ++it) {
		tmp = nullptr; tmpStart = nullptr; tmpEnd = nullptr;
		bool loop = false;
		do {
			if (nullptr != (tmp = strstr(_buffer, it->SearchStr.c_str()))) {		// find subStr?
				if (false == it->IsStarted) {	// IsStarted?
					tmpStart = tmp;
					it->IsStarted = true;
					it->IsFinished = false;
					it->SearchStr = it->EndStr;

					// startStr & endStr in one data stream
					if (nullptr != (tmp = strstr(_buffer+it->StartStr.size(), it->SearchStr.c_str()))) {
						tmpEnd = tmp + it->EndStr.size();
						mapSearchResult[it->NaviNum].Count ++;
						loop = true;
					}
					else {
						tmpEnd = _buffer + _1stLength+_2ndLength;
						loop = false;
					}
				} // IsStarted? End if
				else {							// IsStarted?
					tmpStart = _buffer + _1stLength;
					tmpEnd = tmp + it->EndStr.size();

					it->IsStarted = false;
					it->IsFinished = true;
					mapSearchResult[it->NaviNum].Count ++;

					it->SearchStr = it->StartStr;
					loop = true;
				} // IsStarted? End else
			} // find subStr? End if
			else {								// find subStr?
				if (true == it->IsStarted) {
					tmpStart = _buffer + _1stLength;
					tmpEnd = _buffer + _1stLength + _2ndLength;
					loop = false;
				}
				else {
					tmpStart = nullptr;
					tmpEnd = nullptr;
					loop = false;
				}
			} // find subStr? End else
			if (nullptr != tmpStart && nullptr != tmpEnd) {
				mapSearchResult[it->NaviNum].ResultStr.append(tmpStart, tmpEnd);
// 				it->ResultStr.append(_buffer, tmpStart-_buffer, tmpEnd-tmpStart);
			}
		} while(loop);
	} //End for <searchStatus>
}

/****************************************************************/
/* return string between start sub str and end sub str
/* param(s)
/* int idx : start & end sub str idx
/****************************************************************/
const searchResult* Write2Buffer::getData(int idx) {
	for (vector<bufferStatus>::iterator it = searchStatus.begin(); it != searchStatus.end(); ++it) {
		if (it->NaviNum == idx && true == it->IsFinished)
			return &mapSearchResult[it->NaviNum];
	}
	return nullptr;
}
unsigned int Write2Buffer::getBufferSize() {
	return _bufferSize;
}
