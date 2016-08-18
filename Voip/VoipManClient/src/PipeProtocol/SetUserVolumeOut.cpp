#include "stdafx.h"
#include "include/PipeProtocol/SetUserVolumeOut.h"

CSetUserVolumeOut::CSetUserVolumeOut(byte aID, const wchar_t *aUserName, int aVolume) : CCommonPipePacketOut( aID)
{
	data->Add( aUserName);
	data->Add( aVolume);
}