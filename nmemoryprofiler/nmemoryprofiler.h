#pragma once

#define MEMORY_PROFILER_DECL		__declspec(dllimport)

extern "C" MEMORY_PROFILER_DECL	IMemoryProfiler* CreateMemoryProfiler();
extern "C" MEMORY_PROFILER_DECL	IBlockMemoryController* CreateBlockMemoryController();
extern "C" MEMORY_PROFILER_DECL	void* GetMemory(unsigned int size, unsigned char dllID, char* file, int line, char* func);
extern "C" MEMORY_PROFILER_DECL	void FreeMemory(void* ptr, unsigned char dllID, char* file, int line, char* func);