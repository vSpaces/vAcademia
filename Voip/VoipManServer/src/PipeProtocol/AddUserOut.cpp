#include "stdafx.h"
#include "include/PipeProtocol/AddUserOut.h"

CAddUserOut::CAddUserOut(byte aID, unsigned int aUserID, wchar_t *apNickName) : CCommonPipePacketOut( aID)
{
	data->Add( aUserID);
	data->Add( apNickName);
}