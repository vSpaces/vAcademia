
#include "StdAfx.h"
#include "memfile.h"
#include "memory_leak.h"

CalMemFile::CalMemFile()
{
	lpData = NULL;
	dwLocation = 0;
	dwSize = 0;
}

void CalMemFile::init(void *apData, DWORD adwSize)
{
	dwLocation = 0;
	lpData = apData;
	dwSize = adwSize;
}

CalMemFile::~CalMemFile()
{

}
