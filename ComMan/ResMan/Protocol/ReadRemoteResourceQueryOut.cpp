#include "stdafx.h"
#include "ReadRemoteResourceQueryOut.h"

using namespace ResManProtocol;

CReadRemoteResourceQueryOut::CReadRemoteResourceQueryOut(const wchar_t*  aResPath, DWORD aPos, DWORD aCount)
{
	unsigned short size = (unsigned short)wcslen(aResPath);
	data.addData(sizeof(unsigned short), (BYTE*)&size);
	data.addData(size*2, (BYTE*)aResPath);
	data.add(aPos);
	data.add(aCount);
}
