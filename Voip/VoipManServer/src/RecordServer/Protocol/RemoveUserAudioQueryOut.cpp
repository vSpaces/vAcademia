#include "stdafx.h"
#include "../../include/RecordServer/Protocol/RemoveUserAudioQueryOut.h"

CRemoveUserAudioQueryOut::CRemoveUserAudioQueryOut(LPCSTR alpcUserName, unsigned int aRecordID, unsigned int aRealityID)
{
	ATLASSERT( alpcUserName);

	USES_CONVERSION;
	GetDataBuffer().addString( A2W(alpcUserName));
	GetDataBuffer().addData( &aRecordID, sizeof(aRecordID));
	GetDataBuffer().addData( &aRealityID, sizeof(aRealityID));
}