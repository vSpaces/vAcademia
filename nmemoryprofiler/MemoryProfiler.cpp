CMemoryProfiler* gMP = NULL;

#include "StdAfx.h"
#include <Assert.h>
#include "shlobj.h"
#include <windows.h>
#include "CSVFile.h"
#include "Shellapi.h"
#include "MemoryProfiler.h"

// НАСТРОЙКИ 
// вся память должна быть известна профайлеру
//#define ALL_MEMORY_MUST_BE_KNOWN_BY_PROFILER		1
// нулевые аллокации запрещены
#define ZERO_ALLOCATIONS_PROHIBITED					1
// выделения и освобождения памяти внутри одной функции запрещены, кроме FAST-методов
//#define TEMP_ALLOCATIONS_PROHIBITED					1
// ловить освобождение памяти вне профайлера
//#define CATCH_DEALLOCATIONS_WITHOUT_PROFILER		1
// ловить неправильное освобождение памяти (delete вместо delete[] и наоборот)
#define CATCH_WRONG_DEALLOCATIONS					1
// ловить неправильное освобождение памяти (например, удаление объекта по ссылке на void*)
#define CATCH_WRONG_OBJECT_DEALLOCATIONS			1

// id для csv-файлов
#define CSV_FILES_STAT				1
#define CSV_FILES_AND_LINES_STAT	2
#define CSV_DLLS_STAT				3
#define CSV_FUNCTIONS_STAT			4
#define CSV_CLASSES_STAT			5

// временные глобальные переменные для сортировок
std::vector<int>* globalSize;
unsigned int* addGlobalSize;
std::map<__int64, int>* globalLineSize;

CMemoryProfiler* CMemoryProfiler::GetInstance()
{
	static CMemoryProfiler* obj = NULL;

	if (!obj)
	{
		obj = new CMemoryProfiler();
	}

	return obj;
}

CMemoryProfiler::CMemoryProfiler():
	m_lw(NULL),
	m_isFirstSave(true),
	m_isSavingNow(false),
	m_isNeedToShowDifference(true)
{
	InitializeCriticalSection(&m_cs);
	InitializeCriticalSection(&m_stlCS);
	memset(m_totalDllSize, 0,sizeof(int)*255);
}

std::string CMemoryProfiler::GetCSVFileName(int csvID)
{
	CHAR tmp[MAX_PATH];
	SHGetSpecialFolderPath(NULL, tmp, CSIDL_APPDATA, true);
	std::string fileName = tmp;
	fileName += "\\";

	switch (csvID)
	{
	case CSV_FILES_STAT:
		fileName += "files.csv";
		break;

	case CSV_FILES_AND_LINES_STAT:
		fileName += "files_and_lines.csv";
		break;

	case CSV_DLLS_STAT:
		fileName += "dlls.csv";
		break;

	case CSV_FUNCTIONS_STAT:
		fileName += "functions.csv";
		break;

	case CSV_CLASSES_STAT:
		fileName += "classes.csv";
		break;
	}

	return fileName;
}

void CMemoryProfiler::OpenCSV(int csvID)
{
	ShellExecute( 0, "open", GetCSVFileName(csvID).c_str(), "", "", SW_SHOWNORMAL);
}

__int64	CMemoryProfiler::GetInt64Ptr(const void* _ptr, const unsigned char dllID)const
{
	__int64 ptr = 0;
	*(unsigned int*)&ptr = dllID;
#pragma warning(push)
#pragma warning(disable : 4311)
	*(((unsigned int*)&ptr) + 1) = (unsigned int)_ptr;
#pragma warning(pop)

	return ptr;
}

__int64 CMemoryProfiler::GetFileLineID(const unsigned int fileID, const unsigned int lineID)
{
	__int64 b;

	unsigned int* a = (unsigned int*)&b;
	a[0] = fileID;
	a[1] = lineID;

	return b;
}

std::string CMemoryProfiler::GetFunctionNameByID(const unsigned int id)const
{
	if (0xFFFFFFFF == id)
	{
		return "NOFUNCTION";
	}

	if (id >= m_functionsName.size())
	{
		return "ERROR";
	}

	return m_functionsName[id];
}

std::string CMemoryProfiler::GetFileNameByID(const unsigned int id)const
{
	if (id >= m_filesName.size())
	{
		return "ERROR";
	}

	return m_filesName[id];
}

unsigned int CMemoryProfiler::GetFunctionID(const char* funcName)
{
	if (!funcName)
	{
		return 0xFFFFFFFF;
	}

	std::map<std::string, int>::iterator it = m_functionIDByName.find(funcName);
	if (it != m_functionIDByName.end())
	{
		return (*it).second;
	}
	else
	{
		m_functionsName.push_back(funcName);
		m_totalFunctionsSize.push_back(0);
		m_totalFunctionsAllocationCount.push_back(0);
		m_functionIDByName.insert(std::map<std::string, int>::value_type(funcName, (unsigned int)(m_functionsName.size() - 1)));
		return (unsigned int)(m_functionsName.size() - 1);
	}
}	
	
unsigned int CMemoryProfiler::GetFileNameID(const char* fileName)
{
	std::map<std::string, int>::iterator it = m_fileIDByName.find(fileName);		

	if (it == m_fileIDByName.end())
	{
		m_totalFilesSize.push_back(0);
		m_totalFilesAllocationCount.push_back(0);
		int id = (int)(m_totalFilesSize.size() - 1);
		m_fileIDByName.insert(std::map<std::string, int>::value_type(fileName, id));
		m_filesName.push_back(fileName);
		return id;
	}                        
	else
	{
		return (*it).second;		 
	}
}

void CMemoryProfiler::SaveInfo(const unsigned char dllID, void* _ptr, const unsigned int size, const char* fileName, const unsigned short line, const char* funcName, const unsigned char memoryType, const unsigned short objectSize, unsigned char location)
{
	if (objectSize == 0)
	{
		int ii = 0;
	}
	assert(objectSize != 0);

	if (dllID == 255)
	{
		if (m_lw)
		{
			m_lw->WriteLn("Allocation from code without MP_DLL_ID defined.");
			m_lw->WriteLn("Filename: ", fileName, " line: ", line);
		}
	}
	assert(dllID != 255);

#ifdef ZERO_ALLOCATIONS_PROHIBITED
	// Проверка на выделение массива нулевого размера
	if (0 == size)
	{
		if (m_lw)
		{
			m_lw->WriteLn("Allocation array with size = 0.");
			m_lw->WriteLn("Filename: ", fileName, " line: ", line);
		}
	}
	assert(size != 0);
#endif

	if (size < MIN_MEMORY_PROFILING_ALLOC_SIZE)
	{	
		return;
	}

	__int64 ptr = GetInt64Ptr(_ptr, dllID);	

	EnterCriticalSection( &m_cs);
	
	unsigned int id = GetFileNameID(fileName);

	m_totalDllSize[dllID] += size;
	m_totalFilesSize[id] +=size;
	m_totalFilesAllocationCount[id]++;

	__int64 b = GetFileLineID(id, line);

	std::map<__int64, int>::iterator itFI = m_totalFilesLineMemorySize.find(b);
	std::map<__int64, int>::iterator itFA = m_totalFilesLineAllocationCount.find(b);

	if (itFI == m_totalFilesLineMemorySize.end())
	{
		m_totalFilesLineMemorySize.insert(std::map<__int64, int>::value_type(b, size));
	}
	else
	{
		(*itFI).second +=size;
	}

	if (itFA == m_totalFilesLineAllocationCount.end())
	{
		m_totalFilesLineAllocationCount.insert(std::map<__int64, int>::value_type(b, 1));
	}
	else
	{
		(*itFA).second++;
	}

	unsigned int functionID = GetFunctionID(funcName);
	if (functionID != 0xFFFFFFFF)
	{
		m_totalFunctionsSize[functionID] += size;
		m_totalFunctionsAllocationCount[functionID]++;
	}

	SFileInfo* fileInfo = new SFileInfo(id, size, line, memoryType, location, functionID, objectSize);

	std::map<__int64, SFileInfo*>::iterator itS = m_singleMemorySize.find(ptr);

	if (itS != m_singleMemorySize.end())
	{
#ifdef CATCH_DEALLOCATIONS_WITHOUT_PROFILER
		id = (*itS).second->id;
		std::string fileName = m_filesName[id];			
		if (m_lw)
		{
			m_lw->WriteLn("[MEMPROFILING] Memory was freed without nmemoryprofiler defines: ");
			m_lw->WriteLn("[MEMPROFILING] fileName: ", fileName, " line: ", (*itS).second->line, " size: ", (*itS).second->size);
		}
		assert(false);
#endif

		DeleteInfo(dllID, _ptr, NULL, (*itS).second->memoryType, (*itS).second->objectSize);
	}
	m_singleMemorySize.insert(std::map<__int64, SFileInfo*>::value_type(ptr, fileInfo));
	
	LeaveCriticalSection( &m_cs);			
}

bool CMemoryProfiler::DeleteInfo(const unsigned char dllID, void* _ptr, const char* funcName, const unsigned char memoryType, const unsigned short objectSize)
{
	if (_ptr == NULL) // освобождение памяти по нулевой ссылке
	{
		if (m_lw)
		{
			m_lw->WriteLn("Attempt to delete NULL-ptr in ", funcName);
		}
		assert(false);
	}
		
    __int64 ptr = GetInt64Ptr(_ptr, dllID);
	
	int size = 0;
	int line = 0;
	int id = 0;

	EnterCriticalSection( &m_cs);

	std::map<__int64, SFileInfo*>::iterator itS = m_singleMemorySize.find(ptr);

	if (itS != m_singleMemorySize.end())
	{
		line = (*itS).second->line;
		id = (*itS).second->id;
		size = (*itS).second->size;
		int functionID = GetFunctionID(funcName);
		if ((*itS).second->funcID != 0xFFFFFFFF)
		{
			m_totalFunctionsSize[(*itS).second->funcID] -= size;
			m_totalFunctionsAllocationCount[(*itS).second->funcID]--;
		}

#ifdef TEMP_ALLOCATIONS_PROHIBITED
		if ((functionID == (*itS).second->funcID) && ((*itS).second->memoryType != MEMORY_TYPE_FAST_MALLOC))
		{
			if (m_lw)
			{
				m_lw->WriteLn("Allocation and deallocation within one function: ", GetFunctionNameByID(functionID));
			}
			// выделение и освобождение памяти внутри одной и той же функции
			assert(false);
		}
#endif

#ifdef CATCH_WRONG_DEALLOCATIONS
		if (memoryType != (*itS).second->memoryType)
		{
			std::string fileName = m_filesName[id];			
			if (m_lw)
			{
				m_lw->WriteLn("[MEMPROFILING] Attempt to free memory with wrong method: ");
				m_lw->WriteLn("[MEMPROFILING] fileName: ", fileName, " line: ", (*itS).second->line, " size: ", (*itS).second->size); 	
			}
		}
		assert(memoryType == (*itS).second->memoryType);
#endif

#ifdef CATCH_WRONG_OBJECT_DEALLOCATIONS
		if ((*itS).second->memoryType != MEMORY_TYPE_ARRAY)
		if (objectSize != (*itS).second->objectSize)
		{
			std::string fileName = m_filesName[id];
			m_lw->WriteLn("[MEMPROFILING] Attempt to free memory as pointer to another class: ");
			m_lw->WriteLn("[MEMPROFILING] Allocation in fileName: ", fileName, " line: ", (*itS).second->line, " size: ", (*itS).second->size); 	
			m_lw->WriteLn("[MEMPROFILING] Deallocation in ", funcName);
			assert(false);
		}
#endif
        
		delete (*itS).second;
		m_singleMemorySize.erase(itS);
	}
	else
	{
#ifdef ALL_MEMORY_MUST_BE_KNOWN_BY_PROFILER
		if (m_lw)
		{
			m_lw->WriteLn("[MEMPROFILING] Attempt to free memory that is not known by the profiler: ", funcName);

			for (int _dllID = 0; _dllID <= 255; _dllID++)
			{
				__int64 ptr = GetInt64Ptr(_ptr, _dllID);
				std::map<__int64, SFileInfo*>::iterator itS = m_singleMemorySize.find(ptr);

				if (itS != m_singleMemorySize.end())
				{
					m_lw->WriteLn("This pointer can be found in library ", GetDLLNameByID(_dllID));
				}
			}
		}
		assert(false);
#endif
		LeaveCriticalSection(&m_cs);
		return false;
	}

	m_totalDllSize[dllID] -= size;

	m_totalFilesSize[id] -= size;
	m_totalFilesAllocationCount[id]--;
	
	__int64 b = GetFileLineID(id, line);
	
	std::map<__int64, int>::iterator itFI = m_totalFilesLineMemorySize.find(b);
	
	if(itFI != m_totalFilesLineMemorySize.end())
	{
		(*itFI).second -= size;
	}
	else
	{
		assert(false);
	}

	std::map<__int64, int>::iterator itFA = m_totalFilesLineAllocationCount.find(b);

	if (itFA != m_totalFilesLineAllocationCount.end())
	{
		(*itFA).second--;
	}
	else
	{
		assert(false);
	}

	LeaveCriticalSection( &m_cs);

	return true;
}

void CMemoryProfiler::SetLogWriter(ILogWriter* lw)
{
	m_lw = lw;
}

void CMemoryProfiler::WriteToLogFunctionsInfo()
{
	if (!m_lw)
	{
		return;
	}

	EnterCriticalSection( &m_cs);

	CCSVFile csv(GetCSVFileName(CSV_FUNCTIONS_STAT));
	csv.Rewrite();

	CheckDifferenceState();

	unsigned int allDynamicSize = 0;
	unsigned int* dynamicSize = new unsigned int[m_totalFunctionsSize.size()];
	unsigned int* dynamicAllocations = new unsigned int[m_totalFunctionsSize.size()];
	memset(dynamicSize, 0, m_totalFunctionsSize.size() * 4);
	memset(dynamicAllocations, 0, m_totalFunctionsSize.size() * 4);

	std::map<__int64, SSTLInfo*>::iterator it = m_stlObjectsByPtr.begin();
	std::map<__int64, SSTLInfo*>::iterator itEnd = m_stlObjectsByPtr.end();

	for ( ; it != itEnd; it++)
	if ((((*it).second)->funcID >= 0) && (((*it).second)->funcID < m_functionsName.size()))
	{
		int objectSize = ((*it).second)->obj->GetSize();
		dynamicSize[((*it).second)->funcID] += objectSize;
		dynamicAllocations[((*it).second)->funcID]++;
		allDynamicSize += objectSize;
	}	
	
	int* sortIDBySize = new int[m_totalFunctionsSize.size()];

	for (unsigned int i = 0; i < m_totalFunctionsSize.size(); i++)
	{
		sortIDBySize[i] = i;
	}

	globalSize = &m_totalFunctionsSize;
	addGlobalSize = dynamicSize;

	qsort(sortIDBySize, m_totalFunctionsSize.size(), sizeof(int), CompareFilesTotalMemory);

	unsigned int totalMemorySize = 0; 
	int totalDelta = 0;

	if (!m_isNeedToShowDifference)
	{
		csv.WriteLn("functions", "size", "allocations");
	}
	else
	{
		csv.WriteLn("functions", "size", "allocations", "delta_size", "delta_allocations");
	}

	for (unsigned int i = 0; i < m_totalFunctionsSize.size(); i++)
	{
		unsigned int id = sortIDBySize[i];
		int size = m_totalFunctionsSize[id] + dynamicSize[id];
		int allocCount = m_totalFunctionsAllocationCount[id] + dynamicAllocations[id];
		totalMemorySize += size;		

		if (size > 0)
		if (m_isNeedToShowDifference)
		{
			int deltaSize = size - ((id >= m_totalFunctionsSizeSavedState.size()) ? 0 : m_totalFunctionsSizeSavedState[id]);
			int deltaAllocCount = allocCount - ((id >= m_totalFunctionsAllocationCountSavedState.size()) ? 0 : m_totalFunctionsAllocationCountSavedState[id]);

			totalDelta += deltaSize;
			csv.WriteLn(GetFunctionNameByID(id), size, allocCount, deltaSize, deltaAllocCount);
		}
		else
		{			
			csv.WriteLn(GetFunctionNameByID(id), size, allocCount);
		}

		if (m_isSavingNow)
		{
			if (id < m_totalFunctionsSizeSavedState.size())
			{
				m_totalFunctionsSizeSavedState[id] += dynamicSize[id];
			}

			if (id < m_totalFunctionsAllocationCountSavedState.size())
			{
				m_totalFunctionsAllocationCountSavedState[id] += dynamicAllocations[id];
			}
		}
	}

	if (m_isNeedToShowDifference)
	{
		csv.WriteLn("Total memory (static + dynamic)", totalMemorySize);
		csv.WriteLn("Total dynamic", allDynamicSize);
		csv.WriteLn("Total delta", totalDelta);		
	}
	else
	{
		csv.WriteLn("Total memory (static + dynamic)", totalMemorySize);
		csv.WriteLn("Total dynamic", allDynamicSize);		
	}

	delete[] sortIDBySize;
	delete[] dynamicSize;
	delete[] dynamicAllocations;

	csv.Close();
	OpenCSV(CSV_FUNCTIONS_STAT);

	LeaveCriticalSection( &m_cs);
}

void CMemoryProfiler::WriteToLogTotalFileInfo(bool isNeedShowFile)
{
	if (!m_lw)
	{
		return;
	}

	CheckDifferenceState();

	EnterCriticalSection( &m_cs);

	CCSVFile csv(GetCSVFileName(CSV_FILES_STAT));
	csv.Rewrite();

	if (!m_isNeedToShowDifference)
	{
		csv.WriteLn("file", "size", "allocations");
	}
	else
	{
		csv.WriteLn("file", "size", "allocations", "delta_size", "delta_allocations");
	}

	int* sortIDBySize = new int[m_totalFilesSize.size()];

	for (unsigned int i = 0; i < m_filesName.size(); i++)
	{
		sortIDBySize[i] = i;
	}

	unsigned int dynamicDLLSize[256];
	memset(&dynamicDLLSize[0], 0, 256 * sizeof(unsigned int));
	unsigned int* dynamicSize = new unsigned int[m_filesName.size()];
	unsigned int* dynamicAllocations = new unsigned int[m_filesName.size()];
	memset(dynamicSize, 0, m_filesName.size() * 4);
	memset(dynamicAllocations, 0, m_filesName.size() * 4);

	std::map<__int64, SSTLInfo*>::iterator it = m_stlObjectsByPtr.begin();
	std::map<__int64, SSTLInfo*>::iterator itEnd = m_stlObjectsByPtr.end();

	for ( ; it != itEnd; it++)
	if (((*it).second)->fileID < m_filesName.size())
	{
		dynamicSize[((*it).second)->fileID] += ((*it).second)->obj->GetSize();
		dynamicAllocations[((*it).second)->fileID]++;		
		dynamicDLLSize[((*it).second)->dllID] += ((*it).second)->obj->GetSize();
	}	

	globalSize = &m_totalFilesSize;
	addGlobalSize = dynamicSize;

	qsort(sortIDBySize, m_filesName.size(), sizeof(int), CompareFilesTotalMemory);

	unsigned int totalMemorySize = 0; 
	unsigned int delta = 0; 
	unsigned int deltaAllocationCount = 0; 

	for (unsigned int i = 0; i < m_fileIDByName.size(); i++)
	{
		unsigned int id = sortIDBySize[i];
		int size = m_totalFilesSize[id] + dynamicSize[id];
		int allocCount = m_totalFilesAllocationCount[id] + dynamicAllocations[id];
		totalMemorySize += size;
		if (m_isNeedToShowDifference)
		{
			std::map<std::string, int>::iterator it = m_fileIDByNameSavedState.find(m_filesName[id]);

			if (m_totalFilesSize[id] > 0)
			if(it != m_fileIDByNameSavedState.end())
			{
				delta += m_totalFilesSize[id] - m_totalFilesSizeSavedState[(*it).second];
				deltaAllocationCount += m_totalFilesAllocationCount[id] - m_totalFilesAllocationCountSavedState[(*it).second];

				csv.WriteLn(m_filesName[id], size, allocCount, size - m_totalFilesSizeSavedState[(*it).second], allocCount - m_totalFilesAllocationCountSavedState[(*it).second]);
			}
			else
			{
				csv.WriteLn(m_filesName[id], size, allocCount);
			}
		}
		else
		{
			csv.WriteLn(m_filesName[id], size, allocCount);
		}

		if (m_isSavingNow)
		{
			if (id < m_totalFilesSizeSavedState.size())
			{
				m_totalFilesSizeSavedState[id] += dynamicSize[id];
			}

			if (id < m_totalFilesAllocationCountSavedState.size())
			{
				m_totalFilesAllocationCountSavedState[id] += dynamicAllocations[id];
			}
		}
 	}

	csv.Close();

	if (isNeedShowFile)
		OpenCSV(CSV_FILES_STAT);

	CCSVFile csv2(GetCSVFileName(CSV_DLLS_STAT));
	csv2.Rewrite();
	csv2.WriteLn("dll", "total size", "dynamic size");

	delete[] sortIDBySize;
	delete[] dynamicSize;
	delete[] dynamicAllocations;

	for (unsigned int i = 0; i < 255; i++)
	{
		if (m_totalDllSize[i] + dynamicDLLSize[i] != 0)
		{
			csv2.WriteLn(GetDLLNameByID(i), m_totalDllSize[i], dynamicDLLSize[i]);
		}
	}

	csv2.Close();

	if (isNeedShowFile)
		OpenCSV(CSV_DLLS_STAT);

	LeaveCriticalSection( &m_cs);
}

void CMemoryProfiler::WriteToLogDetailedFileInfo()
{
	if (!m_lw)
	{
		return;
	}
	
	CheckDifferenceState();

	EnterCriticalSection( &m_cs);

	std::map<__int64, int> allMemory = m_totalFilesLineMemorySize;

	{
		std::map<__int64, SSTLInfo*>::iterator it = m_stlObjectsByPtr.begin();
		std::map<__int64, SSTLInfo*>::iterator itEnd = m_stlObjectsByPtr.end();

		for ( ; it != itEnd; it++)
		{
			std::map<__int64, int>::iterator iter = allMemory.find((*it).first);

			if (iter != allMemory.end())
			{
				(*iter).second += ((*it).second)->obj->GetSize();
			}
			else
			{
				allMemory.insert(std::map<__int64, int>::value_type((*it).first, ((*it).second)->obj->GetSize()));
			}
		}
	}

	std::map<__int64, int>::iterator it = allMemory.begin();
	std::map<__int64, int>::iterator itEnd = allMemory.end();

	unsigned int size = (unsigned int)allMemory.size();

	__int64* sortIDBySize = new __int64[size];

	int i = 0;

	unsigned int totalMemorySize = 0; 
	unsigned int delta = 0; 
	unsigned int deltaAllocationCount = 0; 

	for (;it!=itEnd;it++)
	{
		sortIDBySize[i] = (*it).first;
		i++;
	}

	CCSVFile csv(GetCSVFileName(CSV_FILES_AND_LINES_STAT));
	csv.Rewrite();
	if (!m_isNeedToShowDifference)
	{
		csv.WriteLn("file", "line", "size", "allocations");
	}
	else
	{
		csv.WriteLn("file", "line", "size", "allocations", "delta_size", "delta_allocations");
	}

	globalLineSize = &allMemory;

	qsort(sortIDBySize, size, sizeof(__int64), CompareFilesLineMemory);

	for (unsigned int i = 0; i < size; i++)
	{
		int* a = (int*)&sortIDBySize[i];

		std::string name = m_filesName[a[0]];	

		totalMemorySize += allMemory[sortIDBySize[i]];

		if (m_totalFilesLineMemorySize[sortIDBySize[i]] > 0)
		if (m_isNeedToShowDifference)
		{
			std::map<__int64, int>::iterator itMS = m_totalFilesLineMemorySizeSavedState.find(sortIDBySize[i]);
			std::map<__int64, int>::iterator itAC = m_totalFilesLineAllocationCountSavedState.find(sortIDBySize[i]);

			if (itMS != m_totalFilesLineMemorySizeSavedState.end() && itAC != m_totalFilesLineAllocationCountSavedState.end())
			{
				delta += allMemory[sortIDBySize[i]] - m_totalFilesLineMemorySizeSavedState[sortIDBySize[i]];
				deltaAllocationCount += m_totalFilesLineAllocationCount[sortIDBySize[i]] - m_totalFilesLineAllocationCountSavedState[sortIDBySize[i]];

				csv.WriteLn(name.c_str(), a[1], allMemory[sortIDBySize[i]], m_totalFilesLineAllocationCount[sortIDBySize[i]], allMemory[sortIDBySize[i]] - m_totalFilesLineMemorySizeSavedState[sortIDBySize[i]], m_totalFilesLineAllocationCount[sortIDBySize[i]] - m_totalFilesLineAllocationCountSavedState[sortIDBySize[i]]);
			}
			else
			{
				csv.WriteLn(name.c_str(), a[1], allMemory[sortIDBySize[i]], m_totalFilesLineAllocationCount[sortIDBySize[i]]);
			}
		}
		else
		{
			csv.WriteLn(name.c_str(), a[1], allMemory[sortIDBySize[i]], m_totalFilesLineAllocationCount[sortIDBySize[i]]);
		}
	}

	csv.Close();
	OpenCSV(CSV_FILES_AND_LINES_STAT);

	m_lw->WriteLn("Special allocations: ");

	std::map<__int64, SFileInfo*>::iterator it1 = m_singleMemorySize.begin();
	std::map<__int64, SFileInfo*>::iterator it1End = m_singleMemorySize.end();

	for ( ; it1 != it1End; it1++)
	if ((*it1).second->location != 0)
	{
		std::string fileName = m_filesName[(*it1).second->id];	
		m_lw->WriteLn("filename [", fileName, "] line [", (*it1).second->line, "] size [", (*it1).second->size, "] location [", (int)((*it1).second->location), "] ");
	}

	LeaveCriticalSection( &m_cs);
}

int CMemoryProfiler::CompareFilesTotalMemory(const void* a, const void* b)
{
	int fileIdA = *(int*)a;
	int fileIdB = *(int*)b;

	int sizeA = (*globalSize)[fileIdA] + (addGlobalSize ? addGlobalSize[fileIdA] : 0);
	int sizeB = (*globalSize)[fileIdB] + (addGlobalSize ? addGlobalSize[fileIdB] : 0);

	if (sizeA < sizeB)
	{
		return 1;
	}

	if (sizeA > sizeB)
	{
		return -1;
	}

	return 0;
}

int CMemoryProfiler::CompareFilesLineMemory(const void* a, const void* b)
{
	__int64 fileIdA = *(__int64*)a;
	__int64 fileIdB = *(__int64*)b;

	int sizeA = (*globalLineSize)[fileIdA];
	int sizeB = (*globalLineSize)[fileIdB];

	if (sizeA < sizeB)
	{
		return 1;
	}

	if (sizeA > sizeB)
	{
		return -1;
	}

	return 0;
}

void CMemoryProfiler::CheckDifferenceState()
{
	if (!m_isFirstSave)
		m_isNeedToShowDifference = true;
}

void CMemoryProfiler::WriteToLogTotalFileInfoWithDiference()
{
	if (!m_lw)
		return;

	CheckDifferenceState();

	if (m_isFirstSave)
		m_isFirstSave = false;

	SaveAll();
	m_isSavingNow = true;
	WriteToLogTotalFileInfo();
	WriteToLogDetailedFileInfo();
	WriteToLogFunctionsInfo();
	m_isSavingNow = false;
}

void CMemoryProfiler::SaveAll()
{
	EnterCriticalSection( &m_cs);

	m_fileIDByNameSavedState = m_fileIDByName;
	m_totalFilesLineMemorySizeSavedState = m_totalFilesLineMemorySize;
	m_totalFilesLineAllocationCountSavedState = m_totalFilesLineAllocationCount;

	m_totalFilesSizeSavedState = m_totalFilesSize;
	m_totalFilesAllocationCountSavedState = m_totalFilesAllocationCount;

	m_filesNameSavedState = m_filesName;

	m_totalFunctionsSizeSavedState = m_totalFunctionsSize;
	m_totalFunctionsAllocationCountSavedState = m_totalFunctionsAllocationCount;

	LeaveCriticalSection( &m_cs);
}

std::string CMemoryProfiler::GetDLLNameByID(const unsigned int id)const
{
	switch (id){
		case 1:  return "AUDIERE";		break;
		case 2:	 return "BIGFILEMAN";	break;
		case 3:  return "CAL3D";		break;
		case 4:	 return "COMMAN";		break;
		case 5:  return "CSCL";			break;
		case 6:  return "DESKTOPMAN";	break;
		case 7:	 return "INFOMAN";		break;
		case 8:  return "JSONSPIRIT";	break;
		case 9:	 return "LOGGER";		break;
		case 10: return "LOGWRITER";	break;
		case 11: return "NOMODULE";		break;
		case 12: return "RENDERMANAGER + N(*)";		break;
		case 13: return "OMSPLAYER";	break;
		case 14: return "PATHREFINDER";	break;
		case 15: return "NOMODULE";		break;
		case 16: return "RESLOADER";	break;
		case 17: return "RMML";			break;
		case 18: return "RTL";			break;
		case 19: return "SERVICEMAN";	break;
		case 20: return "SOUNDMAN";		break;
		case 21: return "SYNCMAN";		break;
		case 22: return "AUDIOCAPTURE";	break;
		case 23: return "VOIPMAN";	break;
		case 24: return "VASEENCODER";	break;
		case 25: return "UPDATERDLL";	break;
		case 26: return "VCSETTINGS";	break;
		case 27: return "WEBCAMGRABBER";	break;
		case 28: return "JS";	break;
		case 32: return "FTGL";	break;
		case 33: return "VOIPMANCLIENT";	break;
		case 34: return "VOIPMANSERVER";	break;
		default: return "NO NAME";		break;
	}
}

void CMemoryProfiler::ReRegisterSTLObject(ISTLObject* oldPtr, const unsigned char dllID, ISTLObject* newPtr)
{
	__int64 _ptr = GetInt64Ptr(oldPtr, dllID);
	__int64 _ptrNew = GetInt64Ptr(newPtr, dllID);

	EnterCriticalSection(&m_stlCS);	

	std::map<__int64, SSTLInfo*>::iterator it = m_stlObjectsByPtr.find(_ptr);
	if (it != m_stlObjectsByPtr.end())
	{
		SSTLInfo* stlInfo = new SSTLInfo(newPtr, (*it).second->dllID, (*it).second->fileID, (*it).second->line, (*it).second->funcID);	
		m_stlObjectsByPtr.insert(std::map<__int64, SSTLInfo*>::value_type(_ptrNew, stlInfo));
	}

	LeaveCriticalSection(&m_stlCS);
}

void CMemoryProfiler::RegisterSTLObject(ISTLObject* ptr, const unsigned char dllID, const char* fileName, const int line, const char* funcName)
{
	__int64 _ptr = GetInt64Ptr(ptr, dllID);
	
	EnterCriticalSection(&m_cs);	

	int functionID = GetFunctionID(funcName);
	int fileID = GetFileNameID(fileName);

	LeaveCriticalSection(&m_cs);	

	EnterCriticalSection(&m_stlCS);	

	SSTLInfo* stlInfo = new SSTLInfo(ptr, dllID, fileID, line, functionID);	
	m_stlObjectsByPtr.insert(std::map<__int64, SSTLInfo*>::value_type(_ptr, stlInfo));

	LeaveCriticalSection(&m_stlCS);
}

void CMemoryProfiler::UnregisterSTLObject(ISTLObject* ptr, const unsigned char dllID)
{
	__int64 _ptr = GetInt64Ptr(ptr, dllID);

	EnterCriticalSection(&m_stlCS);	

	std::map<__int64, SSTLInfo*>::iterator it = m_stlObjectsByPtr.find(_ptr);
	if (it != m_stlObjectsByPtr.end())
	{
		SSTLInfo* info = (*it).second;
		delete info;
		m_stlObjectsByPtr.erase(it);
	}
	else
	{
		assert(false);
	}

	LeaveCriticalSection(&m_stlCS);
}

CMemoryProfiler::~CMemoryProfiler()
{
	DeleteCriticalSection(&m_cs);
	DeleteCriticalSection(&m_stlCS);

	std::map<__int64, SFileInfo*>::iterator it = m_singleMemorySize.begin();
	std::map<__int64, SFileInfo*>::iterator itEnd = m_singleMemorySize.end();

	for(; it!=itEnd; ++it)
	{
		SFileInfo *fileInfo = (SFileInfo *) it->second;
		if ( fileInfo)
		{
			delete fileInfo;
			fileInfo = NULL;
		}
	}

	m_singleMemorySize.clear();

	std::map<__int64, SSTLInfo*>::iterator its = m_stlObjectsByPtr.begin();
	std::map<__int64, SSTLInfo*>::iterator itsEnd = m_stlObjectsByPtr.end();

	for ( ; its != itsEnd; it++)
	{
		SSTLInfo* info = (*its).second;
		if (info)
		{
			delete info;
		}
	}

	m_stlObjectsByPtr.clear();
}