#include "stdafx.h"
#include "include/PipeProtocol/LogIn.h"

CLogIn::CLogIn(BYTE *aData, int aDataSize) : 
		CCommonPipePacketIn(aData, aDataSize)
{
}

bool CLogIn::Analyse()
{
	if ( !CCommonPipePacketIn::Analyse())
		return false;

	if(!EndOfData() && !data->Read( sLog))
		return false;

	return EndOfData();
}

std::string CLogIn::GetLog()
{
	return sLog;
}