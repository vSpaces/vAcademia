
#pragma once

#include "IBlockMemoryController.h"
#include "BlockMemoryManager.h"

#define SMALL_ALLOCATION_SIZE	255
#define MEDIUM_ALLOCATION_SIZE	2048
#define LARGE_ALLOCATION_SIZE	32767

class CBlockMemoryController : public IBlockMemoryController
{
public:
	CBlockMemoryController();
	~CBlockMemoryController();

	static CBlockMemoryController* GetInstance();

	void* GetMemory(unsigned int size);
	bool FreeMemory(void* ptr);

private:
	CBlockMemoryManager m_areas[3];
};