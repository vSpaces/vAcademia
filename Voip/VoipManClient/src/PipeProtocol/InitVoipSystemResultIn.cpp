#include "stdafx.h"
#include "include/PipeProtocol/InitVoipSystemResultIn.h"


CInitVoipSystemResultIn::CInitVoipSystemResultIn(BYTE *aData, int aDataSize) : 
		CCommonPipePacketIn(aData, aDataSize)
		,MP_WSTRING_INIT(sDeviceName)
		,MP_WSTRING_INIT(sOutDeviceName)
{
}

bool CInitVoipSystemResultIn::Analyse()
{
	if ( !CCommonPipePacketIn::Analyse())
		return false;

	if(!data->Read( errorCode))
		return false;

	if(!data->Read(btInitVoipSystem))
		return false;
	
	if(!data->Read(iVasDevice))
		return false;

	if(!data->Read(iToIncludeSoundDevice))
		return false;

	if(!EndOfData() && !data->Read((std::wstring&)sDeviceName))
		return false;

	if(!EndOfData() && !data->Read((std::wstring&)sOutDeviceName))
		return false;

	return EndOfData();
}

unsigned int CInitVoipSystemResultIn::GetErrorCode()
{
	return errorCode;
}

std::wstring CInitVoipSystemResultIn::GetDeviceName()
{
	return sDeviceName.c_str();
}

std::wstring CInitVoipSystemResultIn::GetOutDeviceName()
{
	return sOutDeviceName.c_str();
}