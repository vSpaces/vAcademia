
#pragma once

#include "PBO.h"

typedef struct _SPBOInfo
{
	CPBO* pbo;
	int allSize;
	int mipmapCount;
	int* mipmapsSize;
	unsigned char** blocks;
	int* blocksSize;

	_SPBOInfo()
	{
		pbo = NULL;
		mipmapCount = 0;
		mipmapsSize = NULL;
		blocks = NULL;
		blocksSize = NULL;
	}
} SPBOInfo;