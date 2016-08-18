#include "stdafx.h"
#include "PackUpdateClientFilesAndGetInfoQueryOut.h"

using namespace ResManProtocol;

CPackUpdateClientFilesAndGetInfoQueryOut::CPackUpdateClientFilesAndGetInfoQueryOut(const wchar_t *aCurVersion)
{
	unsigned short size = (unsigned short)wcslen(aCurVersion);
	data.addData(sizeof(unsigned short), (BYTE*)&size);
	data.addData(size*2, (BYTE*)aCurVersion);

}
