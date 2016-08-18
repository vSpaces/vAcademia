#include "stdafx.h"
#include "WriteRemoteResourceIn.h"

using namespace ResManProtocol;

CWriteRemoteResourceIn::CWriteRemoteResourceIn(BYTE *aData, int aDataSize) : CCommonPacketIn(aData, aDataSize)
{
	packetID = 0;
	errorID = 0;
}

bool CWriteRemoteResourceIn::Analyse()
{
	if ( !CCommonPacketIn::Analyse())
		return false;
	int idx = 0;
	
	errorID = 0;	
	
	if ( !memparse_object( pData, idx, errorID))
		return false;

	unpackStatus = 0;

	if ( !memparse_object( pData, idx, unpackStatus))
		return false;

	if ( !memparse_object( pData, idx, writeBytesCount))
		return false;

	return EndOfData( idx);
}

DWORD CWriteRemoteResourceIn::GetPacketID()
{
	return packetID;
}

BYTE CWriteRemoteResourceIn::GetErrorID()
{
	return errorID;
}

BYTE CWriteRemoteResourceIn::GetUnpackStatus()
{
	return unpackStatus;
}

DWORD CWriteRemoteResourceIn::GetWriteBytesCount()
{
	return writeBytesCount;
}
