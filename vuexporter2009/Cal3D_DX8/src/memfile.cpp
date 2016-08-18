// memfile.cpp: implementation of the CalMemFile class.
//
//////////////////////////////////////////////////////////////////////

#include "memfile.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CalMemFile::CalMemFile()
{
	lpData = NULL;
}

CalMemFile::~CalMemFile()
{

}

DWORD CalMemFile::read(void *lpBuf, DWORD adwSize)
{
	DWORD readed = adwSize;

	if( lpData == NULL) return 0;

	if( dwLocation + adwSize >= dwSize)
		readed = dwSize - dwLocation;
	memcpy( lpBuf, (void*)((DWORD) lpData + dwLocation), readed);

	dwLocation += readed;

	/*if( readed == 0)
		dwLocation += 1;*/

	return readed;
}

void CalMemFile::init(void *apData, DWORD adwSize)
{
	dwLocation = 0;
	lpData = apData;
	dwSize = adwSize;
}

bool CalMemFile::eof()
{
	if( dwLocation > dwSize) return true;
	return false;
}
