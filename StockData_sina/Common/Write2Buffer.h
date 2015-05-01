#pragma once

class Write2Buffer
{
public:
	Write2Buffer();
	Write2Buffer(bool round, int n);
	~Write2Buffer();
	
	void SetBufferSize(int n);
	int GetBufferSize();

	int Write(char *InData, int InSize);
	bool Read(char *OutData, int InSize);
	bool ReadEx(char *OutData, int InLoc, int InSize);
	bool Find(char *InData, int& OutLoc);

private:
	char *_buffer;
	int _bufferSize;
	bool _round;

	int _writeCount;
	int _1stLength;
	int _2ndLength;
};