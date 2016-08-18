#include "stdafx.h"
#include "../../include/PipeProtocol/SetEnableDenoisingIn.h"


CSetEnableDenoisingIn::CSetEnableDenoisingIn(BYTE *aData, int aDataSize) : 
		CCommonPipePacketIn(aData, aDataSize)
{
}

bool CSetEnableDenoisingIn::Analyse()
{
	if ( !CCommonPipePacketIn::Analyse())
		return false;

	if(!data->Read( bEnabled))
		return false;

	return EndOfData();
}

bool CSetEnableDenoisingIn::GetEnabled()
{
	return bEnabled;
}