// BlockMemoryManager.cpp
//
#include "stdafx.h"
#include "BlockMemoryManager.h"

CBlockMemoryManager::CBlockMemoryManager()
{
	items = NULL;
	pmemory = NULL;
	pmemoryend = NULL;
	allocatedSize = 0;
	allocatedItemsNumber = 0;
	usedItemsNumber = 0;
	itemMemorySize = 0;

	InitializeCriticalSection(&cs);
}

CBlockMemoryManager::~CBlockMemoryManager()
{
	DeleteCriticalSection(&cs);
}

void	CBlockMemoryManager::destroy()
{
	if( items)
		delete[] items;
	items = NULL;

	if( pmemory)
		delete[] pmemory;

	pmemory = NULL;
	pmemoryend = NULL;

	allocatedSize = 0;
	allocatedItemsNumber = 0;
	usedItemsNumber = 0;
}

bool	CBlockMemoryManager::initialize( unsigned int auMemBlockSize, unsigned long auMemAmount)
{
	destroy();

	itemMemorySize = auMemBlockSize;
	allocatedSize = auMemAmount;

	while( allocatedSize > auMemBlockSize)
	{
		try
		{
			pmemory = new byte[allocatedSize];
		}
		catch(...)
		{
			pmemory = NULL;
		}
		
		if( pmemory != NULL)
		{
			pmemoryend = pmemory + allocatedSize;
			break;
		}
		allocatedSize -= auMemBlockSize;
	}

	allocatedItemsNumber = allocatedSize / auMemBlockSize;
	if( allocatedItemsNumber > 0)
	{
		items = new unsigned int[allocatedItemsNumber];
		if( items == NULL)
			destroy();
		else
		{
			for( unsigned int i=0; i<allocatedItemsNumber; i++)
				items[i] = i;
		}
	}

	return true;
}

void*	CBlockMemoryManager::malloc( unsigned int auSize)
{
	EnterCriticalSection(&cs);
	if( auSize > itemMemorySize || usedItemsNumber == allocatedItemsNumber)
	{
		LeaveCriticalSection(&cs);
		return NULL;
	}

	byte* allocatedMemory = NULL;
	allocatedMemory = pmemory + itemMemorySize * items[usedItemsNumber];
	usedItemsNumber++;
	LeaveCriticalSection(&cs);

	return (void*)allocatedMemory;
}

bool	CBlockMemoryManager::free( void* apData)
{
	byte* memoryToFree = (byte*)apData;
	if( memoryToFree < pmemory || memoryToFree >= pmemoryend)
	{
		return false;
	}

	int indexToFree = (int)((memoryToFree - pmemory) / itemMemorySize);

	EnterCriticalSection(&cs);
	usedItemsNumber--;
	items[indexToFree] = items[usedItemsNumber];
	items[usedItemsNumber] = indexToFree;
	LeaveCriticalSection(&cs);

	return true;
}