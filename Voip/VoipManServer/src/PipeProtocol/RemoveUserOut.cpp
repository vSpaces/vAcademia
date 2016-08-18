#include "stdafx.h"
#include "include/PipeProtocol/RemoveUserOut.h"

CRemoveUserOut::CRemoveUserOut(byte aID, unsigned int aUserID) : CCommonPipePacketOut( aID)
{
	data->Add( aUserID);
}