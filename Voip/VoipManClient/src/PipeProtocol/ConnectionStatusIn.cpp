#include "stdafx.h"
#include "../../include/PipeProtocol/ConnectionStatusIn.h"


CConnectionStatusIn::CConnectionStatusIn(BYTE *aData, int aDataSize) : 
		CCommonPipePacketIn(aData, aDataSize)
{
}

bool CConnectionStatusIn::Analyse()
{
	if ( !CCommonPipePacketIn::Analyse())
		return false;

	if(!data->Read( code))
		return false;

	return EndOfData();
}

unsigned int CConnectionStatusIn::GetCode()
{
	return code;
}