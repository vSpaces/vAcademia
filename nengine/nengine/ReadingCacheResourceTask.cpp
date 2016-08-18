
#include "StdAfx.h"
#include "ReadingCacheResourceTask.h"
#include "GlobalSingletonStorage.h"

CReadingCacheResourceTask::CReadingCacheResourceTask():
	MP_STRING_INIT(m_fileName),
	m_userData(NULL),
	m_data(NULL),
	m_size(0)
{
}

void CReadingCacheResourceTask::PrintInfo()
{
	g->lw.WriteLn("ReadingCacheResourceTask ", m_fileName);
}

void CReadingCacheResourceTask::SetFileName(std::string& fileName)
{
	m_fileName = fileName;
}

std::string CReadingCacheResourceTask::GetFileName()const
{
	return m_fileName;
}

void CReadingCacheResourceTask::SetUserData(void* userData)
{
	m_userData = userData;
}

void* CReadingCacheResourceTask::GetUserData()const
{
	return m_userData;
}

void CReadingCacheResourceTask::Start()
{
	if (!g->dc.GetData((void **)&m_data, (int *)&m_size, m_fileName))
	{
		m_data = NULL;
		m_size = 0;
	}
}

void* CReadingCacheResourceTask::GetData()const
{
	return m_data;
}

unsigned int CReadingCacheResourceTask::GetDataSize()const
{
	return m_size;
}

void CReadingCacheResourceTask::SelfDestroy()
{
	MP_DELETE_THIS;
}

CReadingCacheResourceTask::~CReadingCacheResourceTask()
{
}