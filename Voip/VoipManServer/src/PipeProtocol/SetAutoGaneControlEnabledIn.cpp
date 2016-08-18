#include "stdafx.h"
#include "../../include/PipeProtocol/SetAutoGaneControlEnabledIn.h"


CSetAutoGaneControlEnabledIn::CSetAutoGaneControlEnabledIn(BYTE *aData, int aDataSize) : 
		CCommonPipePacketIn(aData, aDataSize)
{
}

bool CSetAutoGaneControlEnabledIn::Analyse()
{
	if ( !CCommonPipePacketIn::Analyse())
		return false;

	if(!data->Read( bEnabled))
		return false;

	return EndOfData();
}

bool CSetAutoGaneControlEnabledIn::GetEnabled()
{
	return bEnabled;
}