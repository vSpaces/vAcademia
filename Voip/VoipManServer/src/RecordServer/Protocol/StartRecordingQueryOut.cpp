#include "stdafx.h"
#include "../../include/RecordServer/Protocol/StartRecordingQueryOut.h"

CStartRecordingQueryOut::CStartRecordingQueryOut(LPCSTR alpcVoipServerAddress, const wchar_t* alwcRoomName, unsigned int aRecordID)
{
	ATLASSERT( alwcRoomName);

	USES_CONVERSION;
	GetDataBuffer().addString( A2W(alpcVoipServerAddress));
	GetDataBuffer().addData( &aRecordID, sizeof(aRecordID));
	GetDataBuffer().addString( alwcRoomName);
}