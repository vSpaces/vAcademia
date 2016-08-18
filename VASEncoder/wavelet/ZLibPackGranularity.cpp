
#include "StdAfx.h"
#include "ZLibPackGranularity.h"

#define OPTIMAL_BLOCK_SIZE	(1024 * 50)

unsigned char GetPackSubdivisionCount(unsigned int originalSize)
{
	unsigned int count = (unsigned int)(originalSize / OPTIMAL_BLOCK_SIZE);

	if (count > MAX_PACK_TASKS)
	{
		count = MAX_PACK_TASKS;
	}

	if (0 == count)
	{
		count = 1;
	}

	return count;
}

unsigned int GetPartSize(unsigned char partCount, unsigned int originalSize, unsigned char partID)
{
	if (0 == partCount)
	{
		return 0;
	}

	if (partID >= partCount)
	{
		return 0;
	}

	int partSize = (unsigned int)((originalSize - originalSize%partCount) / partCount);
	if (partID == partCount - 1)
	{
		return partSize + originalSize%partCount;
	}
	else
	{
		return partSize;
	}
}