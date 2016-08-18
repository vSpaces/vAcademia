#include "stdafx.h"
#include "../../include/RecordServer/Protocol/MixedAudioCreatedQueryIn.h"

CMixedAudioCreatedQueryIn::CMixedAudioCreatedQueryIn( BYTE *aData, int aDataSize) : Protocol::CCommonPacketIn( aData, aDataSize)
{
	recordID = 0;
}

bool CMixedAudioCreatedQueryIn::Analyse()
{
	if ( !CCommonPacketIn::Analyse())
		return false;

	int idx = 0;

	// 1. RecordID
	if ( !memparse_object( pData, idx, recordID))
		return false;

	// 2. RecordID
	if ( !memparse_string_unicode( pData, idx, mixedAudioName))
		return false;

	return EndOfData( idx);
}

unsigned int	CMixedAudioCreatedQueryIn::GetRecordID()
{
	return recordID;
}

LPCSTR	CMixedAudioCreatedQueryIn::GetMixedAudioName()
{
	return mixedAudioName;
}