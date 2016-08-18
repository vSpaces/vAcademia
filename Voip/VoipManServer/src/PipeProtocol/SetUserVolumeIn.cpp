#include "stdafx.h"
#include "../../include/PipeProtocol/SetUserVolumeIn.h"


CSetUserVolumeIn::CSetUserVolumeIn(BYTE *aData, int aDataSize) : 
		CCommonPipePacketIn(aData, aDataSize)
{
}

bool CSetUserVolumeIn::Analyse()
{
	if ( !CCommonPipePacketIn::Analyse())
		return false;	
	
	if(!data->Read( (std::wstring&)sUserName))
		return false;
	
	if(!data->Read( volume))
		return false;

	return EndOfData();
}

std::wstring CSetUserVolumeIn::GetUserName()
{
	return sUserName;
}

int CSetUserVolumeIn::GetVolume()
{
	return volume;
}