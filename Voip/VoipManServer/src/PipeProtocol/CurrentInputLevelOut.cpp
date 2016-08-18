#include "stdafx.h"
#include "include/PipeProtocol/CurrentInputLevelOut.h"

CCurrentInputLevelOut::CCurrentInputLevelOut(byte aID, float aCurrentInputLevel) : CCommonPipePacketOut( aID)
{
	data->Add( aCurrentInputLevel);
}