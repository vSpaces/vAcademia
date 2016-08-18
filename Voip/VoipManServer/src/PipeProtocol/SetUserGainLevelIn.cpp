#include "stdafx.h"
#include "../../include/PipeProtocol/SetUserGainLevelIn.h"


CSetUserGainLevelIn::CSetUserGainLevelIn(BYTE *aData, int aDataSize) : 
		CCommonPipePacketIn(aData, aDataSize)
{
}

bool CSetUserGainLevelIn::Analyse()
{
	if ( !CCommonPipePacketIn::Analyse())
		return false;

	if(!data->Read( (std::wstring&)sUserName))
		return false;

	if(!data->Read( value))
		return false;

	return EndOfData();
}

std::wstring CSetUserGainLevelIn::GetUserName()
{
	return sUserName;
}

double CSetUserGainLevelIn::GetValue()
{
	return value;
}