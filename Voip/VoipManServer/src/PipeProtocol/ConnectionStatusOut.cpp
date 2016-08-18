#include "stdafx.h"
#include "include/PipeProtocol/ConnectionStatusOut.h"

CConnectionStatusOut::CConnectionStatusOut(byte aID, unsigned int aErrorCode) : CCommonPipePacketOut( aID)
{
	data->Add( aErrorCode);
}