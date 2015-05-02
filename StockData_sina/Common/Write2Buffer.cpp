#include "Write2Buffer.h"

Write2Buffer::Write2Buffer() :_buffer(nullptr), _bufferSize(0), _round(false), _writeCount(0), _1stLength(0), _2ndLength(0), 
	_maxSearchLength(0), currentSearchNum(0) {}
Write2Buffer::Write2Buffer(bool round, int n):_writeCount(0), _1stLength(0), _2ndLength(0), 
	_maxSearchLength(0), currentSearchNum(0) {
	_round = round;
	if (_round)
		_bufferSize = 2*n;
	else
		_bufferSize = n;

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

	tmp.ResultStr = "";
	tmp.SearchStr = InStartStr;
	tmp.StartStr = InStartStr;
	tmp.EndStr = InEndStr;

	tmp.Count = 0;
	tmp.NaviNum = num;

	searchStatus.push_back(tmp);

	currentSearchNum ++;
	_maxSearchLength = _maxSearchLength > tmp.SearchStr.size() ? _maxSearchLength : tmp.SearchStr.size();

	return true;
}
bool Write2Buffer::RemoveSearchString(int num) {
	for (vector<bufferStatus>::iterator it = searchStatus.begin(); it != searchStatus.end(); ++it) {
		if (it->NaviNum == num) {
			searchStatus.erase(it);

			currentSearchNum--;
			_maxSearchLength = _maxSearchLength > it->SearchStr.size() ? _maxSearchLength : it->SearchStr.size();
			return true;
		}
	}
	return false;
}

void Write2Buffer::getBuffer4Write(char* OutBuffer, int& len) {
	if (_1stLength+_2ndLength <= _maxSearchLength) {
		_1stLength += _2ndLength;
	}
	else {
		char* tmp = new char[_maxSearchLength];
		memmove(_buffer, _buffer+ (_1stLength+_2ndLength-_maxSearchLength), _maxSearchLength);
		delete []tmp;
		memset(_buffer+_maxSearchLength, 0, _1stLength+_2ndLength-_maxSearchLength);
		_1stLength = _maxSearchLength;
	}
	_2ndLength = 0;

	OutBuffer = _buffer + _1stLength;
	len = _bufferSize - _1stLength;
}

void Write2Buffer::updateAfterWrite(int len, bool* ans) {
	_2ndLength = len;

	//Analyse
	char* tmp;
	for (vector<bufferStatus>::iterator it = searchStatus.begin(); it != searchStatus.end(); ++it) {
		if (nullptr != (tmp = strstr(_buffer, it->SearchStr.c_str()))) {
			if (false == it->IsStarted) {
				it->IsStarted = true;
				it->IsFinished = false;
				it->SearchStr = it->EndStr;
				it->ResultStr += tmp;
			}
			else {
				it->IsStarted = false;
				it->IsFinished = true;
				it->SearchStr = it->StartStr;
				it->ResultStr += tmp;
			}
		}
	}
}

const char* Write2Buffer::getData(int num) {
	for (vector<bufferStatus>::iterator it = searchStatus.begin(); it != searchStatus.end(); ++it) {
		if (it->NaviNum == num)
			return it->ResultStr.c_str();
	}
	return nullptr;
}

