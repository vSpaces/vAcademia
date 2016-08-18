
#include "StdAfx.h"
#include "ReadingCacheFileTask.h"
#include "GlobalSingletonStorage.h"

CReadingCacheFileTask::CReadingCacheFileTask():
	MP_STRING_INIT(m_fileName)
{
	m_data = NULL;
	m_size = 0;
}

void CReadingCacheFileTask::SetFileName(std::string& fileName)
{
	m_fileName = fileName;
	m_data = NULL;
	m_size = 0;
}

std::string CReadingCacheFileTask::GetFileName()const
{
	return m_fileName;
}

void* CReadingCacheFileTask::GetData()
{
	return m_data;
}

unsigned int CReadingCacheFileTask::GetDataSize()
{
	return m_size;
}

void CReadingCacheFileTask::Start()
{
	if (!g->dc.GetData(&m_data, (int*)&m_size, m_fileName))
	{
		m_data = NULL;
		m_size = 0;
	}	
}

void CReadingCacheFileTask::Destroy()
{
	if (m_data)
	{
		MP_DELETE_ARR(m_data);
	}

	m_data = NULL;
	m_size = 0;	
}

void CReadingCacheFileTask::PrintInfo()
{
	g->lw.WriteLn("ncommon::CReadingCacheFileTask");
}

void CReadingCacheFileTask::SelfDestroy()
{
	MP_DELETE_THIS;
}

CReadingCacheFileTask::~CReadingCacheFileTask()
{
}
