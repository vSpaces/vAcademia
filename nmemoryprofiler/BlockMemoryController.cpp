
#include "StdAfx.h"
#include "BlockMemoryController.h"

CBlockMemoryController::CBlockMemoryController()
{
	m_areas[0].initialize(SMALL_ALLOCATION_SIZE, 1 * 1024 * 1024);
	m_areas[1].initialize(MEDIUM_ALLOCATION_SIZE, 2 * 1024 * 1024);
	m_areas[2].initialize(LARGE_ALLOCATION_SIZE, 4 * 1024 * 1024);
}

CBlockMemoryController* CBlockMemoryController::GetInstance()
{
	static CBlockMemoryController* obj = NULL;

	if (!obj)
	{
		obj = new CBlockMemoryController();
	}

	return obj;
}

void* CBlockMemoryController::GetMemory(unsigned int size)
{
	if (size <= SMALL_ALLOCATION_SIZE)
	{
		return m_areas[0].malloc(size);
	}
	else if (size <= MEDIUM_ALLOCATION_SIZE)
	{
		return m_areas[1].malloc(size);
	}
	else if (size <= LARGE_ALLOCATION_SIZE)
	{
		return m_areas[2].malloc(size);
	}

	return NULL;
}

bool CBlockMemoryController::FreeMemory(void* ptr)
{
	if (m_areas[0].free(ptr))
	{
		return true;
	}

	if (m_areas[1].free(ptr))
	{
		return true;
	}

	if (m_areas[2].free(ptr))
	{
		return true;
	}

	return false;
}

CBlockMemoryController::~CBlockMemoryController()
{
}