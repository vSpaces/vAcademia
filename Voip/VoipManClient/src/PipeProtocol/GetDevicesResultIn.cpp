#include "stdafx.h"
#include "include/PipeProtocol/GetDevicesResultIn.h"


CGetDevicesResultIn::CGetDevicesResultIn(BYTE *aData, int aDataSize) : 
		CCommonPipePacketIn(aData, aDataSize)
		,MP_VECTOR_INIT(inputDevicesInfo)
		,MP_VECTOR_INIT(outputDevicesInfo)
{
}

bool CGetDevicesResultIn::Analyse()
{
	if ( !CCommonPipePacketIn::Analyse())
		return false;

	int size = 0;
	if(!data->Read( size))
		return false;
	for( int  i = 0; i < size; i++)
	{
		SoundDevice sd;
		if(!data->Read( sd.nDeviceID))
			return false;
		byte nSoundSystem = 0;
		if(!data->Read( nSoundSystem))
			return false;
		sd.nSoundSystem = (SoundSystem)nSoundSystem;
		std::wstring str;
		if(!data->Read( str))
			return false;
		ZeroMemory(sd.szDeviceName, EVO_STRLEN * sizeof(TTCHAR));
		wcscpy_s( sd.szDeviceName, EVO_STRLEN, str.c_str());
		byte btSupports3D = 0;
		if(!data->Read( btSupports3D))
			return false;
		sd.bSupports3D = (BOOL)btSupports3D;
		if(!data->Read( sd.nMaxInputChannels))
			return false;
		if(!data->Read( sd.nMaxOutputChannels))
			return false;
		std::wstring guid;
		if(!data->Read( guid))
			return false;
		ZeroMemory(sd.sGUID, EVO_STRLEN * sizeof(TTCHAR));
		wcscpy_s( sd.sGUID, EVO_STRLEN, guid.c_str());
		inputDevicesInfo.push_back( sd);
	}

	size = 0;
	if(!data->Read( size))
		return false;
	for( int  i = 0; i < size; i++)
	{
		SoundDevice sd;
		if(!data->Read( sd.nDeviceID))
			return false;
		byte nSoundSystem = 0;
		if(!data->Read( nSoundSystem))
			return false;
		sd.nSoundSystem =  (SoundSystem)nSoundSystem;
		std::wstring str;
		if(!data->Read( str))
			return false;
		ZeroMemory(sd.szDeviceName, EVO_STRLEN * sizeof(TTCHAR));
		wcscpy_s( sd.szDeviceName, EVO_STRLEN, str.c_str());
		byte btSupports3D = 0;
		if(!data->Read( btSupports3D))
			return false;
		sd.bSupports3D = (BOOL)btSupports3D;
		if(!data->Read( sd.nMaxInputChannels))
			return false;
		if(!data->Read( sd.nMaxOutputChannels))
			return false;
		std::wstring guid;
		if(!data->Read( guid))
			return false;
		ZeroMemory(sd.sGUID, EVO_STRLEN * sizeof(TTCHAR));
		wcscpy_s( sd.sGUID, EVO_STRLEN, guid.c_str());
		outputDevicesInfo.push_back( sd);
	}
	if( !data->Read( btNeedAnswerToRmml))
		return false;

	return EndOfData();
}

vecDeviceInfo *CGetDevicesResultIn::GetInputDevicesInfo()
{
	return &inputDevicesInfo;
}

vecDeviceInfo *CGetDevicesResultIn::GetOutputDevicesInfo()
{
	return &outputDevicesInfo;
}