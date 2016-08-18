
#include "StdAfx.h"
#include "DataCache.h"
#include "FileFunctions.h"
#include "PlatformDependent.h"
#include <time.h>
#include <wchar.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <iostream>
#include <mmsystem.h>
#include "GlobalSingletonStorage.h"

// размер кеша, до которого мы будем сокращать
#define CACHE_NORMAL_SIZE	(1960 * 1024 * 1024)
// размер кеша, начина€ с которого мы будем удал€ть
#define CACHE_MAX_SIZE		(2040 * 1024 * 1024)
// ожидаемое количество файлов в кеше (превышает максимальное допустимое, т.к. если до запуска
// было максимально допустимое, то после него файлы добавл€ютс€ без ограничений
#define FILES_COUNT			11000
// максимальное число файлов в кеше
#define MAX_FILES_COUNT		10000

//#define DELETE_FILES_ON_ADD_DATA	1

std::vector<FILETIME *>* g_filesLastAccessTime;

int CDataCache::CompareFile(const void* a, const void* b)
{
	int fileIdA = *(int*)a;
	int fileIdB = *(int*)b;

	FILETIME ftA = *((*g_filesLastAccessTime)[fileIdA]);
	FILETIME ftB = *((*g_filesLastAccessTime)[fileIdB]);

	if (ftA.dwHighDateTime > ftB.dwHighDateTime)
	{
		return 1;
	}

	if (ftA.dwHighDateTime < ftB.dwHighDateTime)
	{
		return -1;
	}

	if (ftA.dwLowDateTime > ftB.dwLowDateTime)
	{
		return 1;
	}

	if (ftA.dwLowDateTime < ftB.dwLowDateTime)
	{
		return -1;
	}

	return 0;
}

CDataCache::CDataCache():
	m_allSize(0),
	m_fileCount(0),
	m_needDelete(false),
	MP_WSTRING_INIT(m_path),
	MP_WSTRING_INIT(m_cacheFilePath),
	MP_VECTOR_INIT(m_filesNames),
	MP_VECTOR_INIT(m_filesLastAccessTime),
	MP_MAP_INIT(m_deletingFiles),
	MP_MAP_INIT(m_addDataTasks),
	MP_MAP_INIT(m_files)
{
	int t1 = timeGetTime();

	InitializeCriticalSection(&m_cs);
	InitializeCriticalSection(&m_fileWriteCS);
	InitializeCriticalSection(&m_addDataCS);
	InitializeCriticalSection(&m_fileDeleteCS);

	m_filesNames.reserve(FILES_COUNT);
	m_filesLastAccessTime.reserve(FILES_COUNT);

	m_path = GetApplicationDataDirectory();
	m_path += L"\\";
	m_path += L"ncache";

	CreateDirectoryW(m_path.c_str(), NULL);

	m_path += L"\\";

	m_cacheFilePath = m_path + L"cacheFiles.txt";

	LoadFilesList();	

	int t2 = timeGetTime();
	m_loadingTime = t2 - t1;
	//g->lw.WriteLn("Loading ncache in ", t2 - t1, " ms");
}

void CDataCache::OnFilesListLoadError()
{
	_wremove(m_cacheFilePath.c_str());

	m_files.clear();
	m_filesNames.clear();
	m_filesLastAccessTime.clear();
}

void CDataCache::LoadFilesList()
{
	//избавл€емс€ от сканировани€ всей папки ncache на запуске
	//записыва€ в текстовый файл инф-цию про все файлы, что есть в кеше
	//считывать список файлов
	std::ifstream fl;
	fl.open(m_cacheFilePath.c_str(), std::ios::in);
	if (fl.good())
	{
		std::string name = "";
		std::string fileSize = "";
		std::string lastAccessTime = "";

		m_allSize = 0;
		m_fileCount = 0;

		while (!fl.eof())
		{
			if (!getline(fl, name))
			{
				fl.close();
				OnFilesListLoadError();
				return;
			}			

			name.erase(name.length() - 1, 1);

			name = StringToLower(name);

			if (!getline(fl, fileSize))
			{
				fl.close();
				OnFilesListLoadError();				
				return;
			}

			fileSize.erase(fileSize.length() - 1, 1);

			if (!getline(fl, lastAccessTime))
			{
				fl.close();
				OnFilesListLoadError();				
				return;
			}

			lastAccessTime.erase(lastAccessTime.length() - 1, 1);

			__int64 time = rtl_atoi64(lastAccessTime.c_str());
			FILETIME flTime;
			flTime.dwLowDateTime = (DWORD)time;
			flTime.dwHighDateTime = (DWORD)(time >> 32);
					
			m_files.insert(MP_MAP<MP_STRING, unsigned int>::value_type(MAKE_MP_STRING(name), rtl_atoi(fileSize.c_str())));

			MP_STRING_DC* _name = MP_NEW(MP_STRING_DC);			
			*_name = name;
			MP_STRING_DC_INIT((*_name));
			m_filesNames.push_back(_name);
			MP_NEW_P(_flTime, FILETIME, flTime);
			m_filesLastAccessTime.push_back(_flTime);

			m_fileCount++;
			m_allSize += rtl_atoi(fileSize.c_str());
		}

#ifndef DELETE_FILES_ON_ADD_DATA
		//если превысили размер кеша и количество файлов в кеше превышает максимальное допустимое
		if (AsyncCheckFileSizeAndCount(m_allSize, m_fileCount))
		{					
			UpdateCacheDescriptionAfterDelete(m_deletingFiles);
		}
#endif

		fl.close();
	}
	else //если этого текстового файла нет, то сканируем как раньше и такой файл формируем
	{
		ScanCacheDirectory();
	}
}

std::wstring CDataCache::DeleteNextFile()
{
	std::wstring nameFile;
	if (m_deletingFiles.size() == 0)
		return L"";
	EnterCriticalSection(&m_fileDeleteCS);
	MP_MAP<MP_STRING, unsigned int>::iterator it = m_deletingFiles.begin();
	nameFile = CharToWide(((*it).first).c_str());
	m_deletingFiles.erase(it);
	std::wstring filePath = m_path + nameFile;
	_wremove(filePath.c_str());
	LeaveCriticalSection(&m_fileDeleteCS);
	return m_path + nameFile;
}

bool CDataCache::AsyncCheckFileSizeAndCount(unsigned int allSize, unsigned int fileCount)
{
	g_filesLastAccessTime = &m_filesLastAccessTime;
	if ((allSize > CACHE_MAX_SIZE) || (fileCount > MAX_FILES_COUNT))
	{
		unsigned int* filesIDs = MP_NEW_ARR(unsigned int, m_filesNames.size());
		for (unsigned int i = 0; i < m_filesNames.size(); i++)
		{
			filesIDs[i] = i;
		}

		qsort(filesIDs, m_filesNames.size(), sizeof(int), CompareFile);

		unsigned int currentFileNum = 0;
		while ((allSize > 0.9*CACHE_NORMAL_SIZE) || (fileCount > 0.9*MAX_FILES_COUNT))
		{
			if (currentFileNum >= m_filesNames.size())
				break;
			std::string name = *m_filesNames[filesIDs[currentFileNum]];
			std::wstring filePath = m_path + CharToWide(name.c_str());

			name = StringToLower(name);
			MP_MAP<MP_STRING, unsigned int>::iterator it = m_files.find(MAKE_MP_STRING(name));
			if (it != m_files.end())
			{
				m_deletingFiles.insert(MP_MAP<MP_STRING, unsigned int>::value_type(MAKE_MP_STRING(name), currentFileNum));
				allSize -= (*it).second;
				m_files.erase(it);
				fileCount--;
			}
			currentFileNum++;
		}

		MP_DELETE_ARR(filesIDs);

		m_fileCount = fileCount;
		m_allSize = allSize;

		m_needDelete = true;
		return true;
	}

	return false;
}

bool CDataCache::CheckFileSizeAndCount(unsigned int allSize, unsigned int fileCount, bool bUpdateCache)
{
	if ((allSize > CACHE_MAX_SIZE) || (fileCount > MAX_FILES_COUNT))
	{
		unsigned int* filesIDs = MP_NEW_ARR(unsigned int, m_filesNames.size());
		for (unsigned int i = 0; i < m_filesNames.size(); i++)
		{
			filesIDs[i] = i;
		}

		qsort(filesIDs, m_filesNames.size(), sizeof(int), CompareFile);

		std::map<MP_STRING, unsigned int> deletedFiles;

		unsigned int currentFileNum = 0;
		while ((allSize > 0.9*CACHE_NORMAL_SIZE) || (fileCount > 0.9*MAX_FILES_COUNT))
		{
			if (currentFileNum >= m_filesNames.size())
				break;
			std::string name = *m_filesNames[filesIDs[currentFileNum]];
			std::wstring filePath = m_path + CharToWide(name.c_str());

			bool res = (_wremove(filePath.c_str()) != -1);

			if (res)
			{
				name = StringToLower(name);
				MP_MAP<MP_STRING, unsigned int>::iterator it = m_files.find(MAKE_MP_STRING(name));
				if (it != m_files.end())
				{
					deletedFiles.insert(std::map<MP_STRING, unsigned int>::value_type(MAKE_MP_STRING(name), currentFileNum));
					allSize -= (*it).second;
					m_files.erase(it);
					fileCount--;
				}
			}	
			currentFileNum++;
		}
		if (bUpdateCache)
			UpdateCacheDescriptionAfterDelete(deletedFiles);
		deletedFiles.clear();

		MP_DELETE_ARR(filesIDs);

		m_fileCount = fileCount;
		m_allSize = allSize;

		return true;
	}

	return false;
}

void CDataCache::UpdateCacheDescriptionAfterDelete(std::map<MP_STRING, unsigned int>& aDeletedFiles)
{
	std::ifstream fl;
	fl.open(m_cacheFilePath.c_str(), std::ios::in);
	if (fl.good())
	{
		std::string name = "";
		std::string fileSize = "";
		std::string lastAccessTime = "";

		std::string infoFile = "";
		bool bFirst = true;
		bool bCanAdd = true;

		while (!fl.eof())
		{
			bCanAdd = true;

			if (!getline(fl, name))
			{
				fl.close();
				return;
			}
			name.erase(name.length() - 1, 1);

			name = StringToLower(name);

			USES_CONVERSION;
			MP_MAP<MP_STRING, unsigned int>::iterator it = aDeletedFiles.find(MAKE_MP_STRING(name));
			if (it != aDeletedFiles.end())
			{
				bCanAdd = false;
			}
			if (bCanAdd) {
				if (bFirst)
					infoFile += name;
				else
				{
					infoFile += "\r\n";
					infoFile += name;
				}
				bFirst = false;
			}

			if (!getline(fl, fileSize))
			{
				fl.close();
				return;
			}
			if (bCanAdd) {
				fileSize.erase(fileSize.length() - 1, 1);
				
				infoFile += "\r\n";
				infoFile += fileSize;
			}
			if (!getline(fl, lastAccessTime))
			{
				fl.close();
				return;
			}
			if (bCanAdd) {
				lastAccessTime.erase(lastAccessTime.length() - 1, 1);

				infoFile += "\r\n";
				infoFile += lastAccessTime;
			}
		}
		fl.close();
		FILE* flout = FileOpen(m_cacheFilePath, L"w");

		if (flout)
		{
			fwrite(infoFile.c_str(), infoFile.size(), 1, flout);			
			fclose(flout);
		}
	}
}

void CDataCache::ScanCacheDirectory()
{
	unsigned int allSize = 0;
	unsigned int fileCount = 0;

	WIN32_FIND_DATAW findData;
	HANDLE hSearch = FindFirstFileW((m_path + L"*.*").c_str(), &findData);
	if (hSearch != INVALID_HANDLE_VALUE)
	{
		do
		{
			if ((wcscmp(findData.cFileName, L".") != 0) && (wcscmp(findData.cFileName, L"..") != 0))
				if (findData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)	
				{
				}
				else
				{
					OnFileFound(findData.cFileName, findData.nFileSizeLow, findData.ftLastAccessTime);
					allSize += findData.nFileSizeLow;
					fileCount++;
				}
		} while (FindNextFileW(hSearch, &findData));
		FindClose(hSearch);		
	}	

	//CheckFileSizeAndCount(allSize, fileCount);
	AsyncCheckFileSizeAndCount(allSize, fileCount);
	
	UpdateCacheFiles();
}

void CDataCache::OnFileFound(wchar_t* fileName, unsigned int fileSize, FILETIME lastAccessTime)
{
	if (fileSize != 0)
	{
		USES_CONVERSION;
		std::string name = W2A(fileName);
		name = StringToLower(name);
		m_files.insert(MP_MAP<MP_STRING, unsigned int>::value_type(MAKE_MP_STRING(name), fileSize));		

		MP_STRING_DC* _name = MP_NEW(MP_STRING_DC);
		*_name = name;
		MP_STRING_DC_INIT((*_name));
		m_filesNames.push_back(_name);
		MP_NEW_P(_lastAccessTime, FILETIME, lastAccessTime);
		m_filesLastAccessTime.push_back(_lastAccessTime);
	}
}

void CDataCache::Update()
{
	if (m_loadingTime != 0)
	{
//		g->lw.WriteLn("Loading ncache in ", m_loadingTime, " ms");
		m_loadingTime = 0;
	}

	if (m_needDelete)
	{	
//		g->taskm.AddTask(&m_removeFilesTask, MAY_BE_MULTITHREADED, PRIORITY_LOW, true);
		m_needDelete = false;
	}

	EnterCriticalSection(&m_addDataCS);

	MP_MAP<MP_STRING, CAddNCacheDataTask *>::iterator it = m_addDataTasks.begin();
	
	while (it != m_addDataTasks.end())	
	if (((*it).second)->IsPerformed())
	{
		((*it).second)->FreeData();
		MP_DELETE((*it).second);
		it = m_addDataTasks.erase(it);		
	}
	else
	{
		it++;
	}

	LeaveCriticalSection(&m_addDataCS);
}

void CDataCache::AddDataAsynch(void* data, int size, std::string& name)
{
	name = StringToLower(name);
	CAddNCacheDataTask* task = MP_NEW(CAddNCacheDataTask);
	task->SetParams(name, data, size);
	g->taskm.AddTask(task, MUST_BE_MULTITHREADED, PRIORITY_LOW, true);

	EnterCriticalSection(&m_addDataCS);
	m_addDataTasks.insert(MP_MAP<MP_STRING, CAddNCacheDataTask *>::value_type(MAKE_MP_STRING(name), task));
	LeaveCriticalSection(&m_addDataCS);
}

void CDataCache::AddData(void* data, int size, std::string& name)
{	
	EnterCriticalSection(&m_fileDeleteCS); // такой файл удал€етс€, уберем его из списка удал€емых файлов
	std::wstring wname = CharToWide(name.c_str());
	MP_MAP<MP_STRING, unsigned int>::iterator it = m_deletingFiles.find(MAKE_MP_STRING(StringToLower( name)));
	if (it != m_deletingFiles.end())
	{	
		m_deletingFiles.erase(it);
	}
	LeaveCriticalSection(&m_fileDeleteCS);

	EnterCriticalSection(&m_fileWriteCS);
	g_filesLastAccessTime = &m_filesLastAccessTime;
	std::wstring filePath = m_path + wname;
	FILE* fl = FileOpen(filePath, L"wb");
	if (fl)
	{		
		fwrite(data, size, 1, fl);
		fclose(fl);

		EnterCriticalSection(&m_cs);
		MP_MAP<MP_STRING, unsigned int>::iterator it = m_files.find(MAKE_MP_STRING(name));
		if (it == m_files.end())
		{
			name = StringToLower(name);
			m_files.insert(MP_MAP<MP_STRING, unsigned int>::value_type(MAKE_MP_STRING(name), size));
			
			fl = FileOpen(m_cacheFilePath, L"a");

			if (fl)
			{
				std::string infoFile;
				
				if (m_files.size() == 1)
				{
					infoFile = name;
				}
				else
				{
					infoFile = "\r\n";
					infoFile += name;
				}
				 
				fwrite(infoFile.c_str(), infoFile.size(), 1, fl);				
				infoFile = "\r\n";
				infoFile += IntToStr(size);
				fwrite(infoFile.c_str(), infoFile.size(), 1, fl);
	
				struct _stat buf;
				_wstat(filePath.c_str(), &buf);
				__int64 time = buf.st_atime;

				infoFile = "\r\n";
				infoFile += Int64ToStr(time);
				fwrite(infoFile.c_str(), infoFile.size(), 1, fl);

				FILETIME flTime;
				flTime.dwLowDateTime = (DWORD)time;
				flTime.dwHighDateTime = (DWORD)(time >> 32);
				MP_NEW_P(_flTime, FILETIME, flTime);			
				m_filesLastAccessTime.push_back(_flTime);
				MP_STRING_DC* _name = MP_NEW(MP_STRING_DC);
				*_name = name;
				MP_STRING_DC_INIT((*_name));
				m_filesNames.push_back(_name);

				fclose(fl);

				m_fileCount++;
				m_allSize += size; 
			}
		}

#ifdef DELETE_FILES_ON_ADD_DATA
		CheckFileSizeAndCount(m_allSize, m_fileCount, true);
#endif
		LeaveCriticalSection(&m_cs);
	}
	LeaveCriticalSection(&m_fileWriteCS);	
}

void CDataCache::DeleteByMask(std::string mask)
{
	MP_MAP<MP_STRING, unsigned int>::iterator it =	m_files.begin();
	
	for ( ; it != m_files.end(); )
	{
		if ((*it).first.find(mask) != -1)
		{
			it = m_files.erase(it);			
		}
		else
		{
			it++;
		}
	}

	UpdateCacheFiles();
}

void CDataCache::UpdateCacheFiles()
{
	FILE* fl = FileOpen(m_cacheFilePath, L"w");

	char tmp[40];

	int  i = 0;

	if (fl)
	{
		MP_MAP<MP_STRING, unsigned int>::iterator it =	m_files.begin();
		MP_MAP<MP_STRING, unsigned int>::iterator itEnd = m_files.end();

		//дл€ каждого файла надо им€, размер и дату последнего обновлени€
		std::string infoFile = "";
		infoFile.reserve(1024*1024);
		for ( ; it != itEnd; it++, i++) 
		{
		
			if (it != m_files.begin())
			{
				infoFile += "\r\n";				
				infoFile += (*it).first;
			}
			else
			{
				infoFile = (*it).first;
			}
		
			infoFile += "\r\n";
			_itoa_s((*it).second, tmp, 40, 10);
			infoFile += tmp;		

			std::wstring wname = CharToWide(((*it).first).c_str());
			struct _stat buf;
			_wstat((m_path + wname).c_str(), &buf);
			__int64 time = buf.st_atime;

			// определение даты модификации файла	
			infoFile += "\r\n";
			_i64toa_s(time, tmp, 40, 10);
			infoFile += tmp;			
		}
		fwrite(infoFile.c_str(), infoFile.size(), 1, fl);
		fclose(fl);
	}	
}

CReadingCacheFileTask* CDataCache::GetDataAsynch(std::string& name)
{
	CReadingCacheFileTask* task = MP_NEW(CReadingCacheFileTask);
	task->SetFileName(name);
	g->taskm.AddTask(task, MUST_BE_MULTITHREADED, PRIORITY_HIGH, true);
	return task;
}

bool CDataCache::GetData(void** data, int* size, std::string& name)
{
	std::wstring filePath = m_path + CharToWide(name.c_str());
	FILE* fl = FileOpen(filePath.c_str(), L"rb");
	if (!fl)
	{
		EnterCriticalSection(&m_addDataCS);
		MP_MAP<MP_STRING, CAddNCacheDataTask *>::iterator it = m_addDataTasks.find(MAKE_MP_STRING(name));
		if (it != m_addDataTasks.end())
		{
			((*it).second)->LockData();
			*size = ((*it).second)->GetDataSize();			
			*data = NULL;
			if (((*it).second)->GetDataPointer())
			{
				*data = MP_NEW_ARR(unsigned char, *size);
				memcpy(*data, ((*it).second)->GetDataPointer(), *size);
			}
			((*it).second)->UnlockData();
			LeaveCriticalSection(&m_addDataCS);
			return (*data != NULL);
		}
		else
		{
			LeaveCriticalSection(&m_addDataCS);
			//DeleteIfDataExists(name);

			*data = NULL;
			*size = 0;
			return false;
		}
	}

	fseek(fl, 0, 2);
	*size = ftell(fl);
	fseek(fl, 0, 0);

	if (*size != 0)
	{
		*data = MP_NEW_ARR(unsigned char, *size);

		fread(*data, *size, 1, fl);
	}
	else
	{
		*data = NULL;
	}
	fclose(fl);

	return true;
}

bool CDataCache::CheckIfDataExists(std::string& name)
{
	name = StringToLower(name);

	EnterCriticalSection(&m_cs);
	MP_MAP<MP_STRING, unsigned int>::iterator it = m_files.find(MAKE_MP_STRING( name));
	bool res = (it != m_files.end());

	if (!res)
	{
		EnterCriticalSection(&m_addDataCS);
		MP_MAP<MP_STRING, CAddNCacheDataTask *>::iterator it = m_addDataTasks.find(MAKE_MP_STRING(name));
		res = (it != m_addDataTasks.end());
		LeaveCriticalSection(&m_addDataCS);
	}

	LeaveCriticalSection(&m_cs);
	return res;
}

bool CDataCache::DeleteFromMemoryList(std::string& name)
{
	name = StringToLower(name);

	bool isFound = false;

	EnterCriticalSection(&	m_cs);
	MP_MAP<MP_STRING, unsigned int>::iterator it = m_files.find(MAKE_MP_STRING(name));
	if (it != m_files.end())
	{
		m_files.erase(it);
		isFound = true;
	}
	LeaveCriticalSection(&m_cs);	

	return isFound;
}

bool CDataCache::DeleteIfDataExists(std::string& name)
{
	name = StringToLower(name);

	bool isFound = false;

	EnterCriticalSection(&	m_cs);
	MP_MAP<MP_STRING, unsigned int>::iterator it = m_files.find(MAKE_MP_STRING(name));
	if (it != m_files.end())
	{
		m_files.erase(it);
		isFound = true;
	}
	LeaveCriticalSection(&m_cs);

	if (isFound)
	{
		UpdateCacheFiles();
	}

	return isFound;
}

CDataCache::~CDataCache()
{
	DeleteCriticalSection(&m_cs);
	DeleteCriticalSection(&m_addDataCS);
	DeleteCriticalSection(&m_fileWriteCS);
	DeleteCriticalSection(&m_fileDeleteCS);
}