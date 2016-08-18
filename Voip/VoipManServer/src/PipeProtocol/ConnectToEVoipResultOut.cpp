#include "stdafx.h"
#include "include/PipeProtocol/ConnectToEVoipResultOut.h"

CConnectToEVoipResultOut::CConnectToEVoipResultOut(byte aID, unsigned int aErrorCode) : CCommonPipePacketOut( aID)
{
	data->Add( aErrorCode);
}