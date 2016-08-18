#include "stdafx.h"
#include "include/PipeProtocol/SetEnableDenoisingOut.h"

CSetEnableDenoisingOut::CSetEnableDenoisingOut(byte aID, bool aEnabled) : CCommonPipePacketOut( aID)
{
	data->Add( aEnabled);
}