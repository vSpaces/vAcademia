#include "stdafx.h"
#include "MultiFileInfoQueryOut.h"

using namespace ResManProtocol;

CMultiFileInfoQueryOut::CMultiFileInfoQueryOut( const wchar_t* aResPath)
{
	unsigned short size = (unsigned short)wcslen(aResPath);
	data.addData(sizeof(unsigned short), (BYTE*)&size);
	data.addData(size*2, (BYTE*)aResPath);
}
