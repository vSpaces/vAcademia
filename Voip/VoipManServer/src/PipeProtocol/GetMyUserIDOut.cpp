#include "stdafx.h"
#include "include/PipeProtocol/GetMyUserIDOut.h"

CGetMyUserIDOut::CGetMyUserIDOut(byte aID, int aUserID, bool abInitVoipSystem) : CCommonPipePacketOut( aID)
{
	data->Add( aUserID);
	byte btInitVoipSystem = abInitVoipSystem;
	data->Add( btInitVoipSystem);
}