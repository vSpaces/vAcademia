#include "stdafx.h"
#include "TOInfoIn.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

using namespace ServiceManProtocol;

CTOInfoIn::CTOInfoIn( BYTE *apData, int apDataSize) :
	CCommonPacketIn( apData, apDataSize)
{
	minutes = 30;
}

CTOInfoIn::~CTOInfoIn()
{
}

bool CTOInfoIn::Analyse()
{
	if (!CCommonPacketIn::Analyse())
		return false;

	int idx=0;
	if(!memparse_object(pData, idx, minutes))
		return false;
	return EndOfData( idx);
}

unsigned int CTOInfoIn::getMinutes() const
{
	return minutes;
}