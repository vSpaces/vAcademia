#include "stdafx.h"
#include "ReceiveBadSyncDataIn.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

using namespace SyncManProtocol;

CReceiveBadSyncDataIn::CReceiveBadSyncDataIn( BYTE *aData, int aDataSize ) : CCommonPacketIn(aData, aDataSize)
{

}

bool CReceiveBadSyncDataIn::Analyse()
{
	if (!CCommonPacketIn::Analyse())
		return false;
	int idx=0;
	if(!memparse_object(pData, idx, uObjectID))
		return false;
	if(!memparse_object(pData, idx, uBornRealityID))
		return false;
	if(!memparse_object(pData, idx, uRealityID))
		return false;
	return EndOfData(idx);
}

unsigned int CReceiveBadSyncDataIn::GetObjectID(){
	return uObjectID;
}

unsigned int CReceiveBadSyncDataIn::GetBornRealityID(){
	return uBornRealityID;
}

unsigned int CReceiveBadSyncDataIn::GetRealityID(){

	return uRealityID;
}
