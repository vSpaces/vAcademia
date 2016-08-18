#include "StdAfx.h"
#define ZLIB_WINAPI
#include "zlib.h"
#include "ZLibPackTask.h"
#include "..\logwriter\logwriter.h"

CZLibPackTask::CZLibPackTask():
	m_allocatedSize(0),
	m_packedData(NULL),
	m_packedSize(0),
	m_data(NULL),
	m_context(NULL),
	m_size(0)
{
}

int CZLibPackTask::SetData(void* data, unsigned int size)
{
	m_data = data;
	m_size = size;

	m_packedSize = compressBound(m_size);

	if (m_packedSize > m_allocatedSize)
	{
		CleanUp();

		m_allocatedSize = m_packedSize;
		m_packedData = MP_NEW_ARR( unsigned char, m_allocatedSize);
	}	

	return m_packedSize;
}

void CZLibPackTask::SetContext(oms::omsContext* aContext)
{
	m_context = aContext;
}

void CZLibPackTask::Start()
{
	_SYSTEM_INFO nfo;
	GetSystemInfo(&nfo);
	int compressLevel = 3;
	if (nfo.dwNumberOfProcessors == 2)
		compressLevel = 4;
	else if (nfo.dwNumberOfProcessors > 2)
		compressLevel = 6;
	else if (nfo.dwNumberOfProcessors >= 4)
		compressLevel = 7;
	int res = compress2((Bytef *)m_packedData, (uLongf *)&m_packedSize, (const Bytef *)m_data, m_size, compressLevel);
	if (res != Z_OK)
	{
		m_packedSize = 0;
	}
}

void CZLibPackTask::CleanUp()
{
	if (m_packedData)
	{
		MP_DELETE_ARR( m_packedData);
		m_allocatedSize = 0;
		m_packedData = NULL;
	}
}

void* CZLibPackTask::GetPackedData()const
{
	if (0 == m_packedSize)
	{
		return NULL;
	}

	return m_packedData;
}

unsigned int CZLibPackTask::GetPackedDataSize()const
{
	return m_packedSize;
}

void CZLibPackTask::PrintInfo()
{
	if (m_context)
	{		
		m_context->mpLogWriter->WriteLn("ZLibPackTask");
	}
}

void CZLibPackTask::SelfDestroy()
{
	MP_DELETE_THIS;
}

CZLibPackTask::~CZLibPackTask()
{
	CleanUp();
}