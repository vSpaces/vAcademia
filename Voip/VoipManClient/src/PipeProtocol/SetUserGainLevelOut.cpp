#include "stdafx.h"
#include "include/PipeProtocol/SetUserGainLevelOut.h"

CSetUserGainLevelOut::CSetUserGainLevelOut(byte aID, const wchar_t* alwpcUserLogin, double aValue) : CCommonPipePacketOut( aID)
{
	data->Add( alwpcUserLogin);
	data->Add( aValue);
}