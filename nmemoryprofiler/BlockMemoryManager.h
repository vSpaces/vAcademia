#pragma once

class CBlockMemoryManager
{
public:
	CBlockMemoryManager();
	~CBlockMemoryManager();

public:
	bool	initialize( unsigned int auMemBlockSize, unsigned long auMemAmount);
	void*	malloc( unsigned int auSize);
	bool	free( void* apData);

private:
	void	destroy();

private:
	unsigned int*	items;
	byte*	pmemory;
	byte*	pmemoryend;
	unsigned long	itemMemorySize;
	unsigned long	allocatedSize;
	unsigned long	allocatedItemsNumber;
	unsigned long	usedItemsNumber;
	CRITICAL_SECTION	cs;
};