#pragma once

#define MEMORY_PROFILER

#include <string>
#include <map>
#include <vector>

#include "IMemoryProfiler.h"

typedef struct _SSTLInfo
{
	unsigned short fileID;
	unsigned short line;
	unsigned int funcID;
	unsigned char dllID;
	ISTLObject* obj;

	_SSTLInfo(ISTLObject* _obj, unsigned char _dllID, unsigned short _fileID, unsigned short _line, unsigned int _funcID)
	{
		obj = _obj;
		dllID = _dllID;
		fileID = _fileID;
		line = _line;
		funcID = _funcID;
	}
} SSTLInfo;

typedef struct _SFileInfo
{
	int id;
	unsigned int size;
	unsigned int funcID;
	unsigned short line;
	unsigned short objectSize;
	unsigned char memoryType;
	unsigned char location;

	_SFileInfo(int _id, unsigned int _size, unsigned short _line, unsigned char _memoryType, unsigned char _location, unsigned int _funcID, unsigned short _objectSize)
	{
		id = _id;
		size = _size;
		line = _line;
		memoryType = _memoryType;
		location = _location;
		funcID = _funcID;
		objectSize = _objectSize;
	}

} SFileInfo;

class CMemoryProfiler: public IMemoryProfiler
{
public:
	static CMemoryProfiler* GetInstance();

	CMemoryProfiler();
	~CMemoryProfiler();

	void SetLogWriter(ILogWriter* lw);

	// реализация итерфейса IMemoryProfiler
	void WriteToLogFunctionsInfo();
	void WriteToLogTotalFileInfo(bool isNeedShowFile = true);
	void WriteToLogDetailedFileInfo();
	void WriteToLogTotalFileInfoWithDiference();

	// Сохраняем указатель на память, информацию о размере выделенной памяти,
	// имени файла и строчке, где вызвано выделение. 
	void SaveInfo(const unsigned char dllID, void* ptr, const unsigned int size, const char* fileName, const unsigned short line, const char* funcName, const unsigned char memoryType, unsigned short objectSize, unsigned char location = 0);
	//	Удаляем сохраненную информацию и корректируем статистику по файлам
	bool DeleteInfo(const unsigned char dllID, void* ptr, const char* funcName, const unsigned char memoryType, const unsigned short objectSize);

	void RegisterSTLObject(ISTLObject* ptr, const unsigned char dllID, const char* fileName, const int line, const char* funcName);
	void ReRegisterSTLObject(ISTLObject* oldPtr, const unsigned char dllID, ISTLObject* newPtr);
	void UnregisterSTLObject(ISTLObject* ptr, const unsigned char dllID);

private:
	static int CompareFilesTotalMemory(const void* a, const void* b);
	static int CompareFilesLineMemory(const void* a, const void* b);

	void CheckDifferenceState();

	void SaveAll();

	std::string GetDLLNameByID(const unsigned int id)const;
	std::string GetFunctionNameByID(const unsigned int id)const;
	std::string GetFileNameByID(const unsigned int id)const;

	unsigned int GetFunctionID(const char* funcName);
	unsigned int GetFileNameID(const char* fileName);

	std::string GetCSVFileName(int csvID);
	void OpenCSV(int csvID);

	__int64	GetInt64Ptr(const void* ptr, const unsigned char dllID)const;
	__int64 GetFileLineID(const unsigned int fileID, const unsigned int lineID);

	std::map<std::string, int> m_functionIDByName;
	std::vector<std::string> m_functionsName;

	std::map<std::string, int> m_fileIDByName;

	// По паре файл исходников и номер строчки также суммируем выделенный объем памяти.
	std::map<__int64, int> m_totalFilesLineMemorySize;

	// По паре файл исходников и номер строчки суммируем числа аллокаций.
	std::map<__int64, int> m_totalFilesLineAllocationCount;

	// Объем памяти под информацию за 1 вызов.
	std::map<__int64, SFileInfo*> m_singleMemorySize;

	// По каждой функции суммируем выделенный объем памяти.
	std::vector<int> m_totalFunctionsSize;
	// По каждой функции суммируем кол-во аллокаций.
	std::vector<int> m_totalFunctionsAllocationCount;
	// По каждому файлу исходников суммируем выделенный объем памяти.
	std::vector<int> m_totalFilesSize;
	// По каждому файлу исходников суммируем число аллокаций.
	std::vector<int> m_totalFilesAllocationCount;

	// По каждоой dll суммируем выделенный объем памяти.
	int m_totalDllSize [256];

	std::vector<std::string> m_filesName;

	// сохраненные данные
	std::map<std::string, int> m_fileIDByNameSavedState;
	std::map<__int64, int> m_totalFilesLineMemorySizeSavedState;
	std::map<__int64, int> m_totalFilesLineAllocationCountSavedState;

	std::vector<int> m_totalFilesSizeSavedState;
	std::vector<int> m_totalFilesAllocationCountSavedState;
	std::vector<std::string> m_filesNameSavedState;

	std::vector<int> m_totalFunctionsSizeSavedState;	
	std::vector<int> m_totalFunctionsAllocationCountSavedState;
	
	std::map<__int64, SSTLInfo*> m_stlObjectsByPtr;

	CRITICAL_SECTION m_cs;
	CRITICAL_SECTION m_stlCS;

	bool m_isNeedToShowDifference;
	bool m_isFirstSave;
	bool m_isSavingNow;

	ILogWriter* m_lw;
};