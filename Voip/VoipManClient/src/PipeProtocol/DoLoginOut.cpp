#include "stdafx.h"
#include "include/PipeProtocol/DoLoginOut.h"

CDoLoginOut::CDoLoginOut(byte aID, const wchar_t *aUserName, const wchar_t *aUserPassword, const wchar_t *aRoomName) : CCommonPipePacketOut( aID)
{
	data->Add( aUserName);
	data->Add( aUserPassword);
	data->Add( aRoomName);
}