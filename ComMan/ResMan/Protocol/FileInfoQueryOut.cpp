#include "stdafx.h"
#include "FileInfoQueryOut.h"

using namespace ResManProtocol;

CFileInfoQueryOut::CFileInfoQueryOut(unsigned char *aResPath)
{
	//data.addString((const char*)aResPath);
	std::wstring ws;
	MB2WC((const char*)aResPath, ws);

	const wchar_t* pResPath = ws.c_str();
	unsigned short size = (unsigned short)wcslen(pResPath);
	data.addData(sizeof(unsigned short), (BYTE*)&size);
	data.addData(size*2, (BYTE*)pResPath);

}
