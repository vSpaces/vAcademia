
#include "StdAfx.h"
#include "AddNCacheDataTask.h"
#include "GlobalSingletonStorage.h"

static unsigned int g_taskCount = 0;

CAddNCacheDataTask::CAddNCacheDataTask():
	MP_STRING_INIT(m_name),
	m_data(NULL),
	m_size(0)
{
	InitializeCriticalSection(&m_cs);
}

void CAddNCacheDataTask::SetParams(std::string& name, void* data, unsigned int size)
{
	m_name = name;
	m_data = MP_NEW_ARR(unsigned char, size);
	memcpy(m_data, data, size);		
	m_size = size;
	
}

void CAddNCacheDataTask::Start()
{
	if ((!m_data) || (0 == m_size) || (m_name.size() == 0))
	{
		return;
	}

	g->dc.AddData(m_data, m_size, m_name);

	FreeData();
}

void CAddNCacheDataTask::FreeData()
{	
	if (m_data)
	{
		LockData();
		MP_DELETE_ARR(m_data);		
		m_data = NULL;
		m_size = 0;
		UnlockData();
	}	
}

void CAddNCacheDataTask::PrintInfo()
{
	g->lw.WriteLn("CAddNCacheDataTask ", m_name, " ", m_size);
}

void* CAddNCacheDataTask::GetDataPointer()const
{
	return m_data;
}

unsigned int CAddNCacheDataTask::GetDataSize()const
{
	return m_size;
}

void CAddNCacheDataTask::Destroy()
{ 
	FreeData(); 
}

void CAddNCacheDataTask::SelfDestroy()
{
	MP_DELETE_THIS;
}

void CAddNCacheDataTask::LockData()
{
	EnterCriticalSection(&m_cs);
}

void CAddNCacheDataTask::UnlockData()
{
	LeaveCriticalSection(&m_cs);
}

CAddNCacheDataTask::~CAddNCacheDataTask()
{	
	FreeData();
	DeleteCriticalSection(&m_cs);
}