#include <string.h>
#include "Write2Buffer.h"

Write2Buffer::Write2Buffer() :_buffer(nullptr), _bufferSize(0), _round(false), _writeCount(0), _1stLength(0), _2ndLength(0) {}
Write2Buffer::Write2Buffer(bool round, int n):_writeCount(0), _1stLength(0), _2ndLength(0) {
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

int Write2Buffer::Write(char *InData, int InSize) {
	if (_round) {
		for (int len = 0; len < _2ndLength; len += _1stLength) {
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
bool Write2Buffer::Read(char *OutData, int InSize) {
	if (InSize > _bufferSize)
		return false;
	else {
		memcpy(OutData, _buffer, InSize);
		return true;
	}
}
bool Write2Buffer::ReadEx(char *OutData, int InLoc, int InSize) {
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