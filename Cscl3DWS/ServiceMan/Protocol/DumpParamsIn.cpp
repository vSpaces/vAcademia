#include "stdafx.h"
#include "DumpParamsIn.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

using namespace ServiceManProtocol;

CDumpParamsIn::CDumpParamsIn( BYTE *apData, int apDataSize) :
	CCommonPacketIn( apData, apDataSize)
{
}

CDumpParamsIn::~CDumpParamsIn()
{
}

bool CDumpParamsIn::Analyse()
{
	if (!CCommonPacketIn::Analyse())
		return false;

	int idx=0;
	if(!memparse_object(pData, idx, code))
		return false;
	return EndOfData( idx);
}

unsigned int CDumpParamsIn::getCode() const
{
	return code;
}