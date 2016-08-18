#include "stdafx.h"
#include "include/PipeProtocol/LogOut.h"

CLogOut::CLogOut(byte aID, const char *alpcLog) : CCommonPipePacketOut( aID)
{	
	data->Add( alpcLog);
}