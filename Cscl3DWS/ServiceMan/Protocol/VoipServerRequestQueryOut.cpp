#include "stdafx.h"
#include "VoipServerRequestQueryOut.h"

CVoipServerRequestQueryOut::CVoipServerRequestQueryOut(LPCSTR alpcRoomName)
{
	ATLASSERT( alpcRoomName);
	USES_CONVERSION;
	GetDataBuffer().addString( A2W(alpcRoomName));
}