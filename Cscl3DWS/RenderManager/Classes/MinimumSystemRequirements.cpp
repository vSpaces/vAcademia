
#include "StdAfx.h"
#include "MinimumSystemRequirements.h"
#include "GlobalSingletonStorage.h"
#include "XMLPropertiesList.h"
#include "PlatformDependent.h"

CMinimumSystemRequirements::CMinimumSystemRequirements():
	m_minMemorySize(0),
	m_minProcessorFreq(0),
	m_minMinorSystemVersion(0),
	m_minMajorSystemVersion(0),
	m_minVideocardGeneration(0),
	m_minVideoMemorySize(0)
{
}

void CMinimumSystemRequirements::Init()
{	
	CXMLPropertiesList list(GetApplicationRootDirectory() + L"minimum_system_requirements.xml", ERROR_IF_NOT_FOUND);	
	if (!list.IsLoaded())
	{
		return;
	}

	m_minMemorySize = list.GetInteger("system_memory");
	m_minProcessorFreq = list.GetInteger("cpu");
	m_minMinorSystemVersion = ((int)(list.GetFloat("OS") * 10.0))%10;
	m_minMajorSystemVersion = ((int)(list.GetFloat("OS") * 10.0 - m_minMinorSystemVersion)) / 10;
	m_minVideocardGeneration = list.GetInteger("videocard_generation");
	m_minVideoMemorySize = list.GetInteger("video_memory");
}

bool CMinimumSystemRequirements::IsAllOk()
{
	return ((IsSystemOk()) && (IsProcessorOk()) && (IsMemoryOk()) && (IsVideocardOk()) && (IsVideoMemoryOk()));
}

bool CMinimumSystemRequirements::IsSystemOk()
{	
	assert(m_minMajorSystemVersion > 0);	
	unsigned int minor, major, temp;
	g->ci.GetOSVersion(&major, &minor, &temp);
	return ((major > m_minMajorSystemVersion) || ((major == m_minMajorSystemVersion) && (minor >= m_minMinorSystemVersion)));
}

bool CMinimumSystemRequirements::IsProcessorOk()
{
	assert(m_minProcessorFreq > 0);

	if (g->ci.GetProcessorCount() > 1)
	{
		return true;
	}

	if (g->gi.IsION())
	{
		return true;
	}

	return (g->ci.GetCPUFrequency() >= m_minProcessorFreq);
}

bool CMinimumSystemRequirements::IsMemoryOk()
{
	assert(m_minMemorySize > 0);
	return (g->ci.GetPhysicalMemorySize() >= m_minMemorySize * 1024 * 1024);	
}


bool CMinimumSystemRequirements::IsVideoMemoryOk()
{
	if (g->gi.GetVendorName() == "Unknown")
	{
		return true;
	}

	assert(m_minVideoMemorySize > 0);
	return (g->gi.GetVideoMemory()->GetTotalSize() >= m_minVideoMemorySize * 1024 * 1024);	
}

bool CMinimumSystemRequirements::IsVideocardOk()
{
	if (g->gi.GetVendorName() == "Unknown")
	{
		return true;
	}

	return (g->gi.GetVideocardGeneration() >= m_minVideocardGeneration);
}

CMinimumSystemRequirements::~CMinimumSystemRequirements()
{
}