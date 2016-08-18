
#include "StdAfx.h"
#include "MemoryProfilerCreateImpl.h"
#include "MemoryProfilerFuncs.h"

extern "C"
{
	void* GetMemory(unsigned int size, unsigned char dllID, char* file, int line, char* func)
	{
		return mp_alloc<unsigned char>(dllID, file, line, func, size);
	}

	void* GetZeroMemory(unsigned int size, unsigned char dllID, char* file, int line, char* func)
	{
		void* ptr = mp_alloc<unsigned char>(dllID, file, line, func, size);
		memset(ptr, 0, size);
		return ptr;
	}

	void* ReallocMemory(void* ptr, unsigned int size, unsigned char dllID, char* file, int line, char* func)
	{		
		return mp_realloc<unsigned char>(dllID, file, line, func, size, 0, ptr);
	}

	void FreeMemory(void* ptr, unsigned char dllID, char* file, int line, char* func)
	{
		if (gMP) gMP->DeleteInfo(dllID, ptr, func, MEMORY_TYPE_ARRAY, 0);
		delete[] ptr;	
	}
}

