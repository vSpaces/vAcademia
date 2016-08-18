#include "stdafx.h"
#include "DeleteObjectIn.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

using namespace SyncManProtocol;

CDeleteObjectIn::CDeleteObjectIn(BYTE *aData, int aDataSize) :
	CCommonPacketIn(aData, aDataSize)
{
	uiObjectID = 0;
	uiBornRealityID = 0;
};

bool CDeleteObjectIn::Analyse()
{
	if (!CCommonPacketIn::Analyse())
		return false;
	int idx=0;
	if(!memparse_object(pData, idx, uiObjectID))
		return false;
	if(!memparse_object(pData, idx, uiBornRealityID))
		return false;
	return EndOfData( idx);
}
