#include "stdafx.h"
#include "include/PipeProtocol/GetOutputDevicesResultOut.h"

CGetOutputDevicesResultOut::CGetOutputDevicesResultOut(byte aID, vecDeviceInfo *pOutputDevicesInfo, byte abtNeedAnswerToRmml) : CCommonPipePacketOut( aID)
{
	int size = (int)pOutputDevicesInfo->size();
	data->Add( size);
	vecDeviceInfoIt it= pOutputDevicesInfo->begin();
	vecDeviceInfoIt end = pOutputDevicesInfo->end();
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
		data->Add( it->nMaxOutputChannels);
		data->Add( it->nMaxOutputChannels);
		std::wstring guid = it->sGUID;
		data->Add( guid);
	}
	
	data->Add( abtNeedAnswerToRmml);
}