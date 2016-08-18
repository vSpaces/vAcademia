#include "stdafx.h"
#include "../../include/Protocol/VoipServerRequestQueryOut.h"

CVoipServerRequestQueryOut::CVoipServerRequestQueryOut(LPCSTR alpcRoomName)
{
	ATLASSERT( alpcRoomName);
	USES_CONVERSION;
	GetDataBuffer().addString( A2W(alpcRoomName));
}