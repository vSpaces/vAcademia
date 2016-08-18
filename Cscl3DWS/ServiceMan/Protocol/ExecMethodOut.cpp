#include "stdafx.h"
#include "ExecMethodOut.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

using namespace ServiceManProtocol;


CExecMethodOut::CExecMethodOut(int requestID, unsigned int rmmlID, const wchar_t *aMethodDescription)
{
	data.clear();

	data.add(requestID);
	data.add(rmmlID);
	
	unsigned short size = wcslen(aMethodDescription);
	data.addData(sizeof(unsigned short), (BYTE*)&size);
	data.addData(size*2, (BYTE*)aMethodDescription);
}

