#include "stdafx.h"
#include "../../include/PipeProtocol/ConnectToEVoipResultIn.h"


CConnectToEVoipResultIn::CConnectToEVoipResultIn(BYTE *aData, int aDataSize) : 
		CCommonPipePacketIn(aData, aDataSize)
{
}

bool CConnectToEVoipResultIn::Analyse()
{
	if ( !CCommonPipePacketIn::Analyse())
		return false;

	if(!data->Read( code))
		return false;

	return EndOfData();
}

unsigned int CConnectToEVoipResultIn::GetCode()
{
	return code;
}