#include "stdafx.h"
#include "../../include/RecordServer/Protocol/RecordStartedQueryIn.h"


CRecordStartedQueryIn::CRecordStartedQueryIn(BYTE *aData, int aDataSize) : 
		CCommonPacketIn(aData, aDataSize)
{
}

bool CRecordStartedQueryIn::Analyse()
{
	if ( !CCommonPacketIn::Analyse())
		return false;

	int idx = 0;

	// 2. TcpPort - 2 bytes - порт
	if ( !memparse_object( pData, idx, recordID))
		return false;

	return EndOfData( idx);
}

unsigned int	CRecordStartedQueryIn::GetRecordID()
{
	return recordID;
}

