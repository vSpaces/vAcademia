
#include "StdAfx.h"
#include "ProcessesInfo.h"
#include "Tlhelp32.h"
#include "Psapi.h"
#include <pdh.h>
#include <pdhmsg.h>
#include "CpuUsage.h"

_SProcessInfo::_SProcessInfo(char* name, unsigned int id):
	MP_STRING_INIT(processName),
	MP_VECTOR_INIT(processID),
	MP_VECTOR_INIT(cpuUsage)
{
	processName = name;
	processID.push_back(id);
}

_SMemoryProcessInfo::_SMemoryProcessInfo(std::string name, __int64 size):
	MP_STRING_INIT(processName)
{
	processName = name;
	memorySize = size;
}

_SMemoryProcessInfo::_SMemoryProcessInfo(const _SMemoryProcessInfo& other):
	MP_STRING_INIT(processName)
{
	processName = other.processName;
	memorySize = other.memorySize;
}

_SCPUProcessInfo::_SCPUProcessInfo(std::string name, double usage):
	MP_STRING_INIT(processName)
{
	processName = name;
	cpuUsage = usage;
}

_SCPUProcessInfo::_SCPUProcessInfo(const _SCPUProcessInfo& other):
	MP_STRING_INIT(processName)
{
	processName = other.processName;
	cpuUsage = other.cpuUsage;
}

CProcessesInfo::CProcessesInfo():
	MP_VECTOR_INIT(m_processes),
	MP_MAP_INIT(m_antivirSignatures)
{	
	AddAntivir("vp", "a", ANTIVIRUS_AVP);
	AddAntivir("astsvc", "av", ANTIVIRUS_AVAST);
	AddAntivir("d32", "no", ANTIVIRUS_NOD);
	AddAntivir("web", "dr", ANTIVIRUS_DRWEB);
	AddAntivir("vg", "a", ANTIVIRUS_AVG);
	AddAntivir("psvc", "nava", ANTIVIRUS_NORTON);		
	AddAntivir("ces", "msse", ANTIVIRUS_MICROSOFT);		
}

void CProcessesInfo::AddAntivir(std::string programName1, std::string programName2, unsigned char signID)
{
	std::string programName = programName2;
	programName1 += ".e";
	programName += programName1;
	programName += "xe";
	m_antivirSignatures.insert(MP_MAP<MP_STRING, unsigned char>::value_type(MAKE_MP_STRING(programName), signID));
}

void CProcessesInfo::CleanUp()
{
	MP_VECTOR<SProcessInfo *>::iterator it = m_processes.begin();
	MP_VECTOR<SProcessInfo *>::iterator itEnd = m_processes.end();

	for ( ; it != itEnd; it++)
	{
		MP_VECTOR<void*>::iterator cit = (*it)->cpuUsage.begin();
		MP_VECTOR<void*>::iterator citEnd = (*it)->cpuUsage.end();

		for ( ; cit != citEnd; cit++)
		{
			CCpuUsage* cpuUsage = (CCpuUsage*)(*cit);
			MP_DELETE(cpuUsage);
		}
		MP_DELETE(*it);
	}

	m_processes.clear();
}

SProcessInfo* CProcessesInfo::GetProcessInternalStructure(std::string name)
{
	MP_VECTOR<SProcessInfo *>::iterator it = m_processes.begin();
	MP_VECTOR<SProcessInfo *>::iterator itEnd = m_processes.end();

	for ( ; it != itEnd; it++)
	if ((*it)->processName == name)
	{
		return (*it);
	}

	return NULL;
}

void CProcessesInfo::EnumerateAllProcesses()
{
	CleanUp();

	g->lw.WriteLn("List of all processes");

	HANDLE pSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	 
	PROCESSENTRY32 ProcEntry;	
	ProcEntry.dwSize = sizeof(ProcEntry);
	BOOL isOk = Process32First(pSnap, &ProcEntry);		
	while (isOk)
	{
		if (strcmp(ProcEntry.szExeFile, "[System Process]") != 0)
		{
			SProcessInfo* info = GetProcessInternalStructure(ProcEntry.szExeFile);
			if (info)
			{
				info->processID.push_back(ProcEntry.th32ProcessID);				
			}
			else
			{
				MP_NEW_V2(info, SProcessInfo, ProcEntry.szExeFile, ProcEntry.th32ProcessID);
				m_processes.push_back(info);						
			}
			CCpuUsage* cpuUsage = MP_NEW(CCpuUsage);
			info->cpuUsage.push_back(cpuUsage);
			cpuUsage->GetCpuUsage(ProcEntry.th32ProcessID);
		}
		g->lw.WriteLn("Process: ", ProcEntry.szExeFile);
		isOk = Process32Next(pSnap, &ProcEntry);
	}	
	CloseHandle(pSnap);
}

unsigned char CProcessesInfo::DetectAntivirus()
{
	MP_VECTOR<SProcessInfo *>::iterator it = m_processes.begin();
	MP_VECTOR<SProcessInfo *>::iterator itEnd = m_processes.end();

	unsigned char mask = ANTIVIRUS_NOT_FOUND;

	for ( ; it != itEnd; it++)
	{
		std::string processName = (*it)->processName;
		processName = StringToLower(processName);

		MP_MAP<MP_STRING, unsigned char>::iterator it1 = m_antivirSignatures.find(MAKE_MP_STRING(processName));
		if (it1 != m_antivirSignatures.end())
		{
			mask += (*it1).second;
		}
	}
	
	return mask;
}

void CProcessesInfo::GetProcessesWithCPUUsageHigherThan(std::vector<SCPUProcessInfo>& list, unsigned int minCPUUsagePercent)
{	
	float allPercent = 0;

	MP_VECTOR<SProcessInfo *>::iterator it = m_processes.begin();
	MP_VECTOR<SProcessInfo *>::iterator itEnd = m_processes.end();

	for ( ; it != itEnd; it++)
	{
		std::string processName = (*it)->processName;
		processName = StringToLower(processName);
		if ((processName == "player.exe") || (processName == "system"))
		{
			continue;
		}

		MP_VECTOR<void *>::iterator vit = (*it)->cpuUsage.begin();
		MP_VECTOR<void *>::iterator vitEnd = (*it)->cpuUsage.end();
		MP_VECTOR<unsigned int>::iterator idit = (*it)->processID.begin();

		double percent = 0;

		for ( ; vit != vitEnd; vit++, idit++)
		{
			CCpuUsage* cpuUsage = (CCpuUsage*)(*vit);
			percent += cpuUsage->GetCpuUsage(*idit);
		}

		if (percent > minCPUUsagePercent)
		{
			list.push_back(SCPUProcessInfo((*it)->processName, percent));
			g->lw.WriteLn("cpu consuming: ", (*it)->processName, " ", percent);
			allPercent += (float)percent;
		}
	}

	int coreCount = g->ci.GetProcessorCount();

	g->lw.WriteLn("coreCount = ", coreCount, " allPercent ", allPercent);

	// если процессор одноядерный, то должно быть загружено не менее 20% единственного ядра, чтобы бить тревогу
	if ((coreCount == 1) && (allPercent < 20))
	{
		list.clear();
	}

	// если процессор двуядерный, то должно быть загружено не менее 40% ядра, чтобы бить тревогу
	if ((coreCount == 2) && (allPercent < 40))
	{
		list.clear();
	}

	// если процессор трехядерный, то должно быть загружено не менее 100% ядра, чтобы бить тревогу
	if ((coreCount == 3) && (allPercent < 100))
	{
		list.clear();
	}

	// если процессор трехядерный, то должно быть загружено не менее 50% суммарной мощности ядер, чтобы бить тревогу
	if ((coreCount > 3) && (allPercent < coreCount * 50))
	{
		list.clear();
	}
}

void CProcessesInfo::GetProcessesWithMemoryUsageHigherThan(std::vector<SMemoryProcessInfo>& list, unsigned int minMemorySize)
{
	__int64 freeSize = g->ci.GetAvailablePhysicalMemorySize();
	__int64 allMemorySize = g->ci.GetPhysicalMemorySize();

	int koef = 1;
	if (allMemorySize > (__int64)1024 * (__int64)1024 * (__int64)1024)
	{
		koef = 2;
	}
	if (allMemorySize > (__int64)2048 * (__int64)1024 * (__int64)1024)
	{
		koef = 3;
	}
	if (allMemorySize > (__int64)4096 * (__int64)1024 * (__int64)1024)
	{
		koef = 5;
	}

	if (freeSize < (__int64)800 * (__int64)1024 * (__int64)1024)
	{
		koef = 1;
	}

	minMemorySize *= koef;

	__int64 allSize = 0;

	MP_VECTOR<SProcessInfo *>::iterator it = m_processes.begin();
	MP_VECTOR<SProcessInfo *>::iterator itEnd = m_processes.end();

	for ( ; it != itEnd; it++)
	{
		std::string processName = (*it)->processName;
		processName = StringToLower(processName);
		if ((processName == "player.exe") || (processName == "system"))
		{
			continue;
		}

		MP_VECTOR<unsigned int>::iterator idit = (*it)->processID.begin();
		MP_VECTOR<unsigned int>::iterator iditEnd = (*it)->processID.end();

		__int64 size = 0;

		for ( ; idit != iditEnd; idit++)
		{
			PROCESS_MEMORY_COUNTERS memCounters;

			HANDLE hProcess = OpenProcess(PROCESS_QUERY_INFORMATION, FALSE, *idit);
			
			if (GetProcessMemoryInfo(hProcess, &memCounters, sizeof(memCounters)))
			{
				size += memCounters.WorkingSetSize + memCounters.QuotaPagedPoolUsage + memCounters.QuotaNonPagedPoolUsage;				
			}		
			if (hProcess)
				CloseHandle(hProcess);
		}

		if (size > minMemorySize)
		{
			list.push_back(SMemoryProcessInfo((*it)->processName, size));			
			g->lw.WriteLn("Memory consuming: ", (*it)->processName, " ", size);
			allSize += size;
		}
	}

	if (freeSize > 1024 * 1024 * 1024)
	{		
		g->lw.WriteLn("No memory consuming apps. Free memory size: ", freeSize, " minMemorySize = ", minMemorySize);
		list.clear();
	}
}

CProcessesInfo::~CProcessesInfo()
{
	CleanUp();	
}