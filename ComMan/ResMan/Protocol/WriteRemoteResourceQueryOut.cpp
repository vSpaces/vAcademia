#include "stdafx.h"
#include "WriteRemoteResourceQueryOut.h"

using namespace ResManProtocol;

CWriteRemoteResourceQueryOut::CWriteRemoteResourceQueryOut(LPCTSTR aResPath, DWORD dwPos, BYTE *aData, DWORD aSizeToWrite, DWORD aTotalSize)
{
	// »д-р ресурса
//	wchar_t * pcwh= new
	std::wstring ws;
	MB2WC((const char*)aResPath, ws);
	const wchar_t* pResPath = ws.c_str();
	unsigned short size = (unsigned short)wcslen(pResPath);
	/*data.addData(sizeof(unsigned short), (BYTE*)&size);
	data.addData(size*2, (BYTE*)pResPath);*/
	data.addStringUnicode(size, (BYTE*)pResPath);
	data.add( dwPos);
	//unsigned char *sQueryDate = (unsigned char *) "3.06.2008";
	//data.add((unsigned short)strlen((char*)sQueryDate ), (BYTE*)sQueryDate );
	data.add( aSizeToWrite, aData);
	data.add(aTotalSize);
}
