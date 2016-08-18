#include "stdafx.h"
#include "include/PipeProtocol/SetAutoGaneControlEnabledOut.h"

CSetAutoGaneControlEnabledOut::CSetAutoGaneControlEnabledOut(byte aID, bool aEnabled) : CCommonPipePacketOut( aID)
{
	data->Add( aEnabled);
}