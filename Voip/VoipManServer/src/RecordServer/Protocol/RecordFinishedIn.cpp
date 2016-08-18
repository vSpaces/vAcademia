#include "stdafx.h"
#include "../../include/RecordServer/Protocol/RecordFinishedIn.h"


CRecordFinishedIn::CRecordFinishedIn(BYTE *aData, int aDataSize) : 
		CCommonPacketIn(aData, aDataSize)
{
}

bool CRecordFinishedIn::Analyse()
{
	if ( !CCommonPacketIn::Analyse())
		return false;

	int idx = 0;

	// 2. TcpPort - 2 bytes - порт
	if ( !memparse_object( pData, idx, recordID))
		return false;

	if ( !memparse_object( pData, idx, errorCode))
		return false;

	return EndOfData( idx);
}

unsigned int CRecordFinishedIn::GetRecordID()
{
	return recordID;
}

unsigned int CRecordFinishedIn::GetErrorCode()
{
	return errorCode;
}
