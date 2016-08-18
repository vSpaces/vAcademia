#include "stdafx.h"
#include "GetServiceOut.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

using namespace ServiceManProtocol;


CGetServiceOut::CGetServiceOut(unsigned int aRmmlID, const wchar_t *aServiceName ){
	data.clear();
	data.add(aRmmlID);
	unsigned short size = wcslen(aServiceName);
	data.addData(sizeof(unsigned short), (BYTE*)&size);
	data.addData(size*2, (BYTE*)aServiceName);
}

