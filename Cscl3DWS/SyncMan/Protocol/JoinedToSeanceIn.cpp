#include "stdafx.h"
#include "JoinedToSeanceIn.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

using namespace SyncManProtocol;

CJoinedToSeanceIn::CJoinedToSeanceIn( BYTE *apData, int apDataSize) :
	CCommonPacketIn( apData, apDataSize)
{
}

bool CJoinedToSeanceIn::Analyse()
{
	if (!CCommonPacketIn::Analyse())
		return false;
	int idx=0;
	memparse_object<unsigned int>(pData, idx, uiLogicalObjectID);
	memparse_object<unsigned int>(pData, idx, iError);
	memparse_object<unsigned int>(pData, idx, uiSeanceMode);
	return EndOfData( idx);
}

unsigned int CJoinedToSeanceIn::GetSeanceMode() const{
	return uiSeanceMode;
}

unsigned int CJoinedToSeanceIn::GetLogicalObjectID() const{
	return uiLogicalObjectID;
}
