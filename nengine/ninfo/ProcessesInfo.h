
#pragma once

#include "CommonInfoHeader.h"

#define ANTIVIRUS_NOT_FOUND	0
#define ANTIVIRUS_AVP		1
#define ANTIVIRUS_AVAST		2
#define ANTIVIRUS_NOD		4
#define ANTIVIRUS_DRWEB		8
#define ANTIVIRUS_AVG		16
#define ANTIVIRUS_NORTON	32
#define ANTIVIRUS_MICROSOFT	64

typedef struct _SProcessInfo
{
	MP_STRING processName;
	MP_VECTOR<unsigned int> processID;
	MP_VECTOR<void*> cpuUsage;

	_SProcessInfo(char* name, unsigned int id);	
} SProcessInfo;

typedef struct _SMemoryProcessInfo
{
	MP_STRING processName;
	__int64 memorySize;

	_SMemoryProcessInfo(std::string name, __int64 size);
	_SMemoryProcessInfo(const _SMemoryProcessInfo& other);
} SMemoryProcessInfo;

typedef struct _SCPUProcessInfo
{
	MP_STRING processName;
	double cpuUsage;

	_SCPUProcessInfo(std::string name, double usage);
	_SCPUProcessInfo(const _SCPUProcessInfo& other);	
} SCPUProcessInfo;

class CProcessesInfo
{
public:
	CProcessesInfo();
	~CProcessesInfo();

	void EnumerateAllProcesses();

	void GetProcessesWithCPUUsageHigherThan(std::vector<SCPUProcessInfo>& list, unsigned int minCPUUsagePercent);

	void GetProcessesWithMemoryUsageHigherThan(std::vector<SMemoryProcessInfo>& list, unsigned int minMemorySize);

	unsigned char DetectAntivirus();	

private:
	void CleanUp();
	void AddAntivir(std::string programName1, std::string programName2, unsigned char signID);
	
	SProcessInfo* GetProcessInternalStructure(std::string name);

	MP_VECTOR<SProcessInfo *> m_processes;	

	MP_MAP<MP_STRING, unsigned char> m_antivirSignatures;
};