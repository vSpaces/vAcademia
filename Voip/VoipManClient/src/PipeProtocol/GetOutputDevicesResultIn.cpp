#include "stdafx.h"
#include "include/PipeProtocol/GetOutputDevicesResultIn.h"


CGetOutputDevicesResultIn::CGetOutputDevicesResultIn(BYTE *aData, int aDataSize) : 
		CCommonPipePacketIn(aData, aDataSize)
		, MP_VECTOR_INIT(outputDevicesInfo) 
{
}

bool CGetOutputDevicesResultIn::Analyse()
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
		if(!data->Read( sd.nMaxOutputChannels))
			return false;
		if(!data->Read( sd.nMaxOutputChannels))
			return false;
		outputDevicesInfo.push_back( sd);
	}
	
	if( !data->Read( btNeedAnswerToRmml))
		return false;

	return EndOfData();
}

vecDeviceInfo *CGetOutputDevicesResultIn::GetOutputDevicesInfo()
{
	return &outputDevicesInfo;
}