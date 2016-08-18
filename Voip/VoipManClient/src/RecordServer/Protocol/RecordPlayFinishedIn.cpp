#include "stdafx.h"
#include "../../include/RecordServer/Protocol/RecordPlayFinishedIn.h"


CRecordPlayFinishedIn::CRecordPlayFinishedIn(BYTE *aData, int aDataSize) : 
		CCommonPacketIn(aData, aDataSize)
{
}

bool CRecordPlayFinishedIn::Analyse()
{
	if ( !CCommonPacketIn::Analyse())
		return false;

	int idx = 0;

	// 2. TcpPort - 2 bytes - порт
	if ( !memparse_object( pData, idx, recordID))
		return false;

	return EndOfData( idx);
}

unsigned int CRecordPlayFinishedIn::GetRecordID()
{
	return recordID;
}

