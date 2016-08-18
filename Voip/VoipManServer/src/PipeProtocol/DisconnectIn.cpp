#include "stdafx.h"
#include "../../include/PipeProtocol/DisconnectIn.h"


CDisconnectIn::CDisconnectIn(BYTE *aData, int aDataSize) : 
		CCommonPipePacketIn(aData, aDataSize)
{
}

bool CDisconnectIn::Analyse()
{
	if ( !CCommonPipePacketIn::Analyse())
		return false;

	return EndOfData();
}