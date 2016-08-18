#pragma once

#define WIN32_LEAN_AND_MEAN		// Exclude rarely-used stuff from Windows headers

#include "..\LogWriter\ILogWriter.h"
#include "ISTLObject.h"

#define	MIN_MEMORY_PROFILING_ALLOC_SIZE						0

#define MEMORY_TYPE_NOT_ARRAY								0
#define MEMORY_TYPE_ARRAY									1
#define MEMORY_TYPE_FAST_MALLOC								2

#include <string>
#include <map>
#include <vector>

class IMemoryProfiler
{
public:
	virtual void WriteToLogFunctionsInfo() = 0;
	virtual void WriteToLogTotalFileInfo(bool isNeedShowFile = true) = 0;
	virtual void WriteToLogDetailedFileInfo() = 0;
	virtual void WriteToLogTotalFileInfoWithDiference() = 0;

	virtual void SaveInfo(const unsigned char dllID, void* ptr, const unsigned int size, const char* fileName, const unsigned short line, const char* funcName, const unsigned char memoryType, unsigned short objectSize, unsigned char location = 0) = 0;
	virtual bool DeleteInfo(const unsigned char dllID, void* ptr, const char* funcName, const unsigned char memoryType, const unsigned short objectSize) = 0;

	virtual void SetLogWriter(ILogWriter* lw) = 0;

	virtual void RegisterSTLObject(ISTLObject* ptr, const unsigned char dllID, const char* fileName, const int line, const char* funcName) = 0;
	virtual void ReRegisterSTLObject(ISTLObject* oldPtr, const unsigned char dllID, ISTLObject* newPtr) = 0;
	virtual void UnregisterSTLObject(ISTLObject* ptr, const unsigned char dllID) = 0;
};