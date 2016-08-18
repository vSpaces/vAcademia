#pragma once

class IBlockMemoryController
{
public:
	virtual void* GetMemory(unsigned int size) = 0;
	virtual bool FreeMemory(void* ptr) = 0;
};