#include "stdafx.h"
#include "../../include/PipeProtocol/InitVoipSystemIn.h"


CInitVoipSystemIn::CInitVoipSystemIn(BYTE *aData, int aDataSize) : 
		CCommonPipePacketIn(aData, aDataSize),
		MP_WSTRING_INIT(sDeviceGuid),
		MP_WSTRING_INIT(sOutDeviceGuid)
{
}

bool CInitVoipSystemIn::Analyse()
{
	if ( !CCommonPipePacketIn::Analyse())
		return false;

	if(!data->Read(iVasDevice))
		return false;

	if( !data->Read( iToIncludeSoundDevice))
		return false;

	if(!data->Read((std::wstring&)sDeviceGuid))
		return false;	
	if(!data->Read((std::wstring&)sOutDeviceGuid))
		return false;

	return EndOfData();
}

const wchar_t *CInitVoipSystemIn::GetDeviceGuid()
{
	return sDeviceGuid.c_str();
}

const wchar_t *CInitVoipSystemIn::GetOutDeviceGuid()
{
	return sOutDeviceGuid.c_str();
}

