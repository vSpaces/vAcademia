#include "stdafx.h"
#include "include/PipeProtocol/GetInputDevicesResultOut.h"

CGetInputDevicesResultOut::CGetInputDevicesResultOut(byte aID, vecDeviceInfo *pInputDevicesInfo, byte abtNeedAnswerToRmml) : CCommonPipePacketOut( aID)
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
	
	data->Add( abtNeedAnswerToRmml);
}