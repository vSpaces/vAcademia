#include "stdafx.h"
#include "RegisterRemoteResourceQueryOut.h"

using namespace ResManProtocol;

CRegisterRemoteResourceQueryOut::CRegisterRemoteResourceQueryOut(const wchar_t *aFilePath)
{
	unsigned short size = (unsigned short)wcslen(aFilePath);
	data.addData(sizeof(unsigned short), (BYTE*)&size);
	data.addData(size*2, (BYTE*)aFilePath);

}
