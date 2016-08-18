#include "stdafx.h"
#include "UploadFileInfoQueryOut.h"

using namespace ResManProtocol;

CUploadFileInfoQueryOut::CUploadFileInfoQueryOut(const wchar_t* aResPath)
{
	unsigned short size = (unsigned short)wcslen(aResPath);
	data.addData(sizeof(unsigned short), (BYTE*)&size);
	data.addData(size*2, (BYTE*)aResPath);

}
