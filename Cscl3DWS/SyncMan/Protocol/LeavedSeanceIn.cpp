#include "stdafx.h"
#include "LeavedSeanceIn.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

using namespace SyncManProtocol;

CLeavedSeanceIn::CLeavedSeanceIn( BYTE *apData, int apDataSize) :
	CCommonPacketIn( apData, apDataSize)
{
}

bool CLeavedSeanceIn::Analyse()
{
	if (!CCommonPacketIn::Analyse())
		return false;
	int idx=0;
	memparse_object<unsigned int>(pData, idx, uiLogicalObjectID);
	return EndOfData( idx);
}

