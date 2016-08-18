#include "stdafx.h"
#include "include/PipeProtocol/GetDevicesResultOut.h"

CGetDevicesResultOut::CGetDevicesResultOut(byte aID, vecDeviceInfo *pInputDevicesInfo, vecDeviceInfo *pOutputDevicesInfo, byte abtNeedAnswerToRmml) : CCommonPipePacketOut( aID)
{
	int size = (int)pInputDevicesInfo->size();
	data->Add( size);
	vecDeviceInfoIt it= pInputDevicesInfo->begin();
	vecDeviceInfoIt end = pInputDevicesInfo->end();
	for (; it!=end; ++it)
	{
		data->Add( it->nDeviceID);
		byte nSoundSystem = (byte)it->nSoundSystem;
		data->Add( nSoundSystem);
		//data->Add( it->nSoundSystem);
		std::wstring str = it->szDeviceName;
		data->Add( str);
		byte btSupports3D = (byte)it->bSupports3D;
		data->Add( btSupports3D);
		data->Add( it->nMaxInputChannels);
		data->Add( it->nMaxOutputChannels);
		std::wstring guid = it->sGUID;
		data->Add( guid);
	}

	size = (int)pOutputDevicesInfo->size();
	data->Add( size);
	it= pOutputDevicesInfo->begin();
	end = pOutputDevicesInfo->end();
	for (; it!=end; ++it)
	{
		data->Add( it->nDeviceID);
		byte nSoundSystem = (byte)it->nSoundSystem;
		data->Add( nSoundSystem);
		//data->Add( it->nSoundSystem);
		std::wstring str = it->szDeviceName;
		data->Add( str);
		byte btSupports3D = (byte)it->bSupports3D;
		data->Add( btSupports3D);
		data->Add( it->nMaxInputChannels);
		data->Add( it->nMaxOutputChannels);
		std::wstring guid = it->sGUID;
		data->Add( guid);
	}
	data->Add( abtNeedAnswerToRmml);
}