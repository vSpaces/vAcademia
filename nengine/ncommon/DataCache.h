
#pragma once

//#include "CommonCommonHeader.h"

#include <string>
#include <vector>
#include <map>
#include <hash_map>
#include "AddNCacheDataTask.h"
#include "removefilesfromncachetask.h"
#include "ReadingCacheFileTask.h"

class CDataCache
{
public:
	CDataCache();
	~CDataCache();

	void AddData(void* data, int size, std::string& name);
	void AddDataAsynch(void* data, int size, std::string& name);

	bool GetData(void** data, int* size, std::string& name);
	CReadingCacheFileTask* GetDataAsynch(std::string& name);
	bool CheckIfDataExists(std::string& name);

	static int CompareFile(const void* a, const void* b);

	void Update();

	std::wstring DeleteNextFile();

	bool DeleteFromMemoryList(std::string& name);
	void DeleteByMask(std::string mask);

private:
	void OnFileFound(wchar_t* fileName, unsigned int fileSize, FILETIME lastAccessTime);
	bool CheckFileSizeAndCount(unsigned int allSize, unsigned int fileCount, bool bUpdateCache = false);
	bool AsyncCheckFileSizeAndCount(unsigned int allSize, unsigned int fileCount);

	void OnFilesListLoadError();

	bool DeleteIfDataExists(std::string& name);
	void ScanCacheDirectory();	
	void UpdateCacheFiles();
	void LoadFilesList();
	void UpdateCacheDescriptionAfterDelete(std::map<MP_STRING, unsigned int>& aDeletedFiles);

	MP_MAP<MP_STRING, unsigned int> m_files;
	MP_VECTOR<MP_STRING *> m_filesNames;

	MP_MAP<MP_STRING, CAddNCacheDataTask *> m_addDataTasks;

	MP_VECTOR<FILETIME *> m_filesLastAccessTime;
	MP_MAP<MP_STRING, unsigned int> m_deletingFiles;
	
	CRITICAL_SECTION m_cs;
	CRITICAL_SECTION m_addDataCS;
	CRITICAL_SECTION m_fileWriteCS;
	CRITICAL_SECTION m_fileDeleteCS;

	MP_WSTRING m_path;
	MP_WSTRING m_cacheFilePath;

	unsigned int m_allSize;
	unsigned int m_fileCount;
	unsigned int m_loadingTime;	

	CRemoveFilesFromNcacheTask m_removeFilesTask;
	bool	m_needDelete;
};