#include "stock2fpTable.h"

int stockFile::openedFiles = 0;
void stockFile::getFiles(string path, vector<string>& files)
{  
	long hFile = 0;
	struct _finddata_t fileinfo;
	string p;
	if((hFile = _findfirst(p.assign(path).append("\\*").c_str(),&fileinfo)) !=  -1)
	{  
		do {
			if((fileinfo.attrib &  _A_SUBDIR)) {	//if directory, iterate
				if(strcmp(fileinfo.name,".") != 0  &&  strcmp(fileinfo.name,"..") != 0)
					getFiles(p.assign(path).append("\\").append(fileinfo.name), files);
			} else {			//if not directory, add it
				files.push_back(p.assign(path).append("\\").append(fileinfo.name));
			}
		}while(_findnext(hFile, &fileinfo)  == 0);
		_findclose(hFile);
	}
}
bool stockFile::createFile(string path) {
	// if exist return false
	if (stockFile::IsAccessable(path.c_str()))
		return false;
	// if not exist, create and return true
	else {
		FILE *fp;
		fopen_s(&fp, path.c_str(), "w");
		fclose(fp);
		return true;
	}
}

stockFile::stockFile()
{
	counter = 0;
	file = NULL;
	used = false;
	IsOpened = false;
}
stockFile::~stockFile()
{
	if (NULL != file)	close();
	used = false;
	IsOpened = false;
	counter = 0;
}
void stockFile::close()
{
	fclose(file);
	IsOpened = false;
}
bool stockFile::open(string fileName, char* fileType, char* tp)
{
	fileName += tp;
	if (0 == fopen_s(&file, fileName.c_str(), fileType)) {
		IsOpened = true;
		if (!strcmp(".stk", tp))
			stockFile::openedFiles ++;
		return true;
	}
	else return false;
}
char* stockFile::readline(char *OutBuffer, int size) {
	if (!IsOpened) return nullptr;
	else {
		return fgets(OutBuffer, size, file);
	}
}

void stockFile::SetFileNameFormate(const string &id, int year, int season, char *tp, string &str) {
	char tmp[128] = {0};
	str.clear();
	sprintf_s(tmp, 128, "[$%s$]%d-%d%s", id.c_str(), year, TO_DISPLAY(season), tp);
	str = tmp;
	return;

}
void stockFile::GetFileNameFormate(const string &str, char *tp, int &year, int &season, string &id) {
	char tmpID[128] = {0};
	int tmpSeason;
	const char *p = str.c_str();
	do {
		const char *tp;
		tp = strstr(p+1, "\\");
		if (tp) p = tp;
		else { p = strstr(p, "["); break;}
	} while (1);

	if (!strcmp(tp, ".dstk")) {
		sscanf_s(p, "[$%[^$]]]%d-%d.dstk", tmpID, 128, &year, &tmpSeason);
		season = TO_DATA(tmpSeason);
		id = tmpID;
	} else 	if (!strcmp(tp, ".FQdstk")) {
		sscanf_s(p, "[%[^]]]%d-%d.FQdstk", tmpID, 128, &year, &tmpSeason);
		season = TO_DATA(tmpSeason);
		id = tmpID;
	} else 	if (!strcmp(tp, ".NFQstk")) {
		sscanf_s(p, "[%[^]]]%d-%d.NFQdstk", tmpID, 128, &year, &tmpSeason);
		season = TO_DATA(tmpSeason);
		id = tmpID;
	}


}

bool stockFile::CheckFolderExist(const string &strPath) {
	WIN32_FIND_DATA  wfd;
	bool rValue = false;
	HANDLE hFind = FindFirstFile(strPath.c_str(), &wfd);
	if ((hFind != INVALID_HANDLE_VALUE) && (wfd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))
		rValue = true;
	FindClose(hFind);
	return rValue;
}
bool stockFile::CheckDSTKFileExist(const string &strPath, bool IsFolder, char *pt) {
	string p;
	if (IsFolder) {
		char tmp[32] = {0};
		sprintf_s(tmp, 32, "\\*.%s", pt);
		p.assign(strPath).append(tmp);
	}
	else
		p.assign(strPath);
	WIN32_FIND_DATA  wfd;
	HANDLE hFind = FindFirstFile(p.c_str(), &wfd);
	if (INVALID_HANDLE_VALUE == hFind) {
		FindClose(hFind);
		return false;
	} else {
		FindClose(hFind);
		return true;
	}
}
void stockFile::FindLatestDTSK(const string &strPath, string &OutStr, char *pt) {
	vector<string> files;
	getFiles(strPath, files);
	int year = 0, season = 0;
	int maxYear = 0, maxSeason = 0;
	string id;
	for (vector<string>::iterator it = files.begin(); it != files.end(); ++it) {
		GetFileNameFormate(it->c_str(), pt, year, season, id);
		if (year > maxYear) {
			maxYear = year;
			maxSeason = season;
		}
		else if (year == maxYear) {
			maxSeason = maxSeason >= season ? maxSeason : season;
		}
		else {}
	}
	SetFileNameFormate(id, maxYear, maxSeason, pt, OutStr);
}


bool stockFile::FileWriteTime(string path, SYSTEMTIME &s_t) {
	WIN32_FIND_DATA p;
	HANDLE hFind = FindFirstFile(path.c_str(), &p);
	if (INVALID_HANDLE_VALUE != hFind) {
		FILETIME f_tm;
		if (!FileTimeToLocalFileTime(&p.ftLastWriteTime, &f_tm)) goto FileWriteTimeError;
		if (!FileTimeToSystemTime(&f_tm, &s_t)) goto FileWriteTimeError;
		FindClose(hFind);
		return true;
	} else goto FileWriteTimeError;
FileWriteTimeError:
	FindClose(hFind);
	return false;
}



/////////////////////////////////////////////////////////////
stockTable::stockTable()
{
	files = NULL;
	dataStores = NULL;
	NofFiles = 0;

	initial(DEFAULT_TABLE_SIZE);
	AttentionFile.open("stockAttention", "a+", ".att");
}

stockTable::~stockTable()
{
	clean();
}

void stockTable::initial(int TableSize)
{
	if (0 == NofFiles) {
		files = new stockFile[TableSize];
		dataStores = new Data_Store[TableSize];
		for (int i = 0; i < TableSize; i++) {
			memset(dataStores, 0, sizeof(Data_Store));
		}
	}
	NofFiles += TableSize;
}

void stockTable::clean()
{
	delete []files;
	delete []dataStores;
	NofFiles = 0;
	AttentionFile.close();
}

bool stockTable::addStock2File(string strSymbol)
{
	int idx = 0;
	if (findFreeFile(idx)) {
		files[idx].used = true;
		pair<map<string, int>::iterator, bool> ret = mapStockFile.insert(StockType(strSymbol, idx));
		if (ret.second == false)	printf_s("Not inserted, may be existed!!!\n");
		return ret.second;
	}
	else return false;
}
int stockTable::removeStock2File(string strSymbol)
{
	int idx = mapStockFile[strSymbol];
	if (true == files[idx].IsFileOpened()) { files[idx].close();}

	int NofErase = mapStockFile.erase(strSymbol);
	if (0 == NofErase)	printf_s("Not removed, may be not exist!!!\n");
	else				files[idx].used = false;
	return NofErase;
}

bool stockTable::findFreeFile(int &idx)
{
	for (idx = 0; idx < NofFiles; idx++)
	{
		if (true == files[idx].used)	continue;
		else {
			NofOpened++;
			return true;
		}
	}
	if (TABLE_MAX_SIZE > NofFiles && NofFiles == NofOpened)
	{
		printf_s("pre allocation is not writed!!!\n");
	}
	return false;
}

int stockTable::closeAllFiles() {
	int size = getFileTableSize();
	int i = 0;
	for (; i < size; i++)
		files[i].close();
	return i;
}
