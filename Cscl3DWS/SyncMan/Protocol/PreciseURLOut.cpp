#include "stdafx.h"
#include "PreciseURLOut.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

using namespace SyncManProtocol;


CPreciseURLOut::CPreciseURLOut(const wchar_t *aURL)
{
	data.clear();
	unsigned short size = wcslen(aURL);
	data.add(size);
	data.addData(size*2, (BYTE*)aURL);
}
