#include "stdafx.h"
#include "DumpFileOut.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

using namespace ServiceManProtocol;

CDumpFileOut::CDumpFileOut( BYTE *apData, int apDataSize, int aDataTotalSize)	
{
	data.addData( sizeof(unsigned long), (BYTE *) &aDataTotalSize);
	data.addData( sizeof(unsigned long), (BYTE *) &apDataSize);
	data.addData( apDataSize, apData);
}

CDumpFileOut::~CDumpFileOut()
{
}