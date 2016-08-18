#include "stdafx.h"
#include "ErrorCreateTempLocationIn.h"

using namespace SyncManProtocol;

CErrorCreateTempLocationIn::CErrorCreateTempLocationIn( BYTE *apData, int apDataSize) :
	CCommonPacketIn( apData, apDataSize)
{
}

CErrorCreateTempLocationIn::~CErrorCreateTempLocationIn(){
}



bool CErrorCreateTempLocationIn::Analyse()
{
	if (!CCommonPacketIn::Analyse())
		return false;

	int idx=0;
	if (!memparse_object(pData, idx, errorCode))
		return false;
	return EndOfData( idx);
}